#include "klib.h"
#include "types.h"
#include "x86def.h"
#include "proc.h"
#include "global.h"
#include "asm.h"

void TestA() {
    int i = 0;
    while (1) {
        dist_str("A");
        disp_int(i++);
        dist_str(".");
        delay(10000);
    }
}

/* Implemnet restart using inline asm,
 * so we don't need to declare proc_t in asm*/
void restart() {
    save_proc_state();
    ++k_reenter;
    if (k_reenter == 0) {
        to_kstack();
    }

    --k_reenter;
    load_proc_state(&p_proc_ready->regs);
    tss.esp0 = (u32)p_proc_ready + sizeof(STACK_FRAME);
    iret();
}

/* Load proc state from STACK_FRAME */
// TODO: Change argument type to PROCESS*
void inline load_proc_state(STACK_FRAME *p_frame) {
    __asm__ __inline__("mov %0, %%esp\n"
                       "pop %%gs\n"
                       "pop %%fs\n"
                       "pop %%es\n"
                       "pop %%ds\n"
                       "popal\n"
                       "add $4, %%esp\n"
                       :
                       : "rm"(p_frame)
                       : "memory");
}

void inline save_proc_state() {
    __asm__ __volatile__("sub $4, %%esp\n"
                         "pushal\n"
                         "pushl %%ds\n"
                         "pushl %%es\n"
                         "pushl %%fs\n"
                         "pushl %%gs\n"
                         :
                         :
                         : "memory");
}

void inline to_kstack() {
    __asm__ __volatile__("mov %%esp, %0\n" : : "m"(StackTop));
}

__attribute__((noreturn)) int init_proc() {
    disp_clear();
    dist_str("-----\"kernel_main\" begins-----\n");

    PROCESS *p_proc = proc_table;
    p_proc->ldt_sel = SELECTOR_LDT_FIRST;
    memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
    p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5; // change the DPL
    memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
    p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5; // change the DPL
    p_proc->regs.cs = ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc->regs.ds = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc->regs.es = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc->regs.fs = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc->regs.ss = ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
    p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;
    p_proc->regs.eip = (u32)TestA;
    p_proc->regs.esp = (u32)task_stack + STACK_SIZE_TOTAL;
    p_proc->regs.eflags = 0x1202; // IF=1, IOPL=1, bit 2 is always 1.

    p_proc_ready = proc_table;
    tss.esp0 = (u32)p_proc_ready + sizeof(STACK_FRAME);
    lldt(p_proc_ready->ldt_sel);
    load_proc_state(&p_proc_ready->regs);
    BOCHS_BREAK();
    iret(); // Goto first stack point by p_proc_ready

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

void schedule(void) {
    return;
}