#include "klib.h"
#include "types.h"
#include "x86def.h"
#include "proc.h"
#include "global.h"
#include "asm.h"
#include "syscall.h"

#define QUEUE_LEVEL 3
int queue_slice[] = {10, 20, 30};
int queue_len[] = {NR_TASKS, 0, 0};

void TestA() {
    while (1) {
        dist_str("A");
        u32 t = get_ticks();
        disp_int(t);
        dist_str(".");
        while (get_ticks() - t < 1)
            ;
    }
}

void TestB() {
    int i = 1000;
    while (1) {
        dist_str("B");
        u32 t = get_ticks();
        disp_int(t);
        dist_str(".");
        while (get_ticks() - t < 1)
            ;
    }
}

void TestC() {
    int i = 100000;
    while (1) {
        dist_str("C");
        u32 t = get_ticks();
        disp_int(t);
        dist_str(".");
        while (get_ticks() - t < 1)
            ;
    }
}

__attribute__((noreturn)) int init_proc() {
    disp_clear();
    dist_str("-----\"kernel_main\" begins-----\n");

    PROCESS *p_proc = proc_table;
    TASK *p_task = init_task;
    u16 selector_ldt = SELECTOR_LDT_FIRST;
    u32 stack_top = (u32)task_stack + STACK_SIZE_TOTAL;

    for (int i = 0; i < NR_TASKS; i++, selector_ldt += 8, p_proc++, p_task++) {
        p_proc->ldt_sel = selector_ldt;
        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
               sizeof(DESCRIPTOR));
        p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5; // change the DPL
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
               sizeof(DESCRIPTOR));
        p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5; // change the DPL
        p_proc->regs.cs =
            ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ds =
            ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.es =
            ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.fs =
            ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ss =
            ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;
        p_proc->regs.eip = p_task->init_eip;
        p_proc->regs.esp = stack_top;
        p_proc->time = 10;
        p_proc->q = 0;
        p_proc->pos = i;
        p_proc->wait = 0;
        stack_top -= p_task->stack_size;
        p_proc->regs.eflags = 0x1202; // IF=1, IOPL=1, bit 2 is always 1.
    }

    p_proc_ready = proc_table;
    restart();

    while (1)
        ;
}

int check_testA() {
    // 静态变量
    static u8 ti = 0xFFU;
    static int disp2 = 0;
    static u32 ori = 0;
    // 初始 hash
    if (ori == 0) ori = get_hash((u8 *)TestA, (int)init_proc - (int)TestA);
    // 检查间隔
    ++ti;
    if (ti != 0) return 0;
    // 设置 PtDisp
    int t = PtDisp;
    PtDisp = disp2;
    // 检查 hash
    if (ori != get_hash((u8 *)TestA, (int)init_proc - (int)TestA))
        dist_colstr("Warning: TestA has been changed!\n", 0x0C);
    else
        dist_colstr("Success!\n", 0x02);
    // 还原 PtDisp
    disp2 = PtDisp;
    PtDisp = t;
    return 0;
}

void schedule() {
    PROCESS *cur = p_proc_ready;
    int i = 0;
    int j = 0;
    if (cur->time <= 0) {
        queue_len[cur->q]--; // 弹出当前进程

        for (i = 0; i < NR_TASKS; i++)
            if (proc_table[i].q == cur->q) proc_table[i].pos--;

        cur->q += (cur->q == 2 ? 0 : 1); // 将当前进程重新加入队列

        cur->time = queue_slice[cur->q];
        cur->pos = queue_len[cur->q];
        queue_len[cur->q]++;
        int i_next_proc = 0; // 选出优先级最高的队列
        int min_prio = 1000;
        for (i = 0; i < NR_TASKS; i++) {
            int prio = proc_table[i].pos;
            for (j = 0; j < proc_table[i].q; j++) {
                prio += queue_len[j];
            }
            if (min_prio > prio) {
                min_prio = prio;
                i_next_proc = i;
            }
        }
        p_proc_ready = proc_table + i_next_proc;
    } else if (cur->q > 0 && queue_len[0] > 0) { // 需要发生抢占
        for (i = 0; i < NR_TASKS; i++) {
            if (proc_table[i].q == cur->q) proc_table[i].pos--;
            if (proc_table[i].q == 0 &&
                proc_table[i].pos == 0) // 选出抢占的进程
                p_proc_ready = proc_table + i;
        }
        cur->pos = queue_len[cur->q] - 1; //放到队尾
    }
    /*
    p_proc_ready = p_proc_ready + 1;
    if (p_proc_ready == proc_table + NR_TASKS)
        p_proc_ready = proc_table;
        */
}

void restart(void) {
    tss.esp0 = (u32)p_proc_ready + sizeof(STACK_FRAME);
    lldt(p_proc_ready->ldt_sel);
    __asm__ __volatile__("mov %0, %%esp\n"
                         "pop %%gs\n"
                         "pop %%fs\n"
                         "pop %%es\n"
                         "pop %%ds\n"
                         "popal\n"
                         "iret\n"
                         :
                         : "r"(p_proc_ready)
                         : "memory");
}

void restart_reenter(void) {
    __asm__ __volatile__("pop %%gs\n"
                         "pop %%fs\n"
                         "pop %%es\n"
                         "pop %%ds\n"
                         "popal\n"
                         "iret\n"
                         :
                         :
                         : "memory");
}