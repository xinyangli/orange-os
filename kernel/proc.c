#include "klib.h"
#include "types.h"
#include "x86def.h"
#include "proc.h"
#include "global.h"
#include "asm.h"

void TestA() {
    int i = 0;
    while (1) {
        disp_str("A");
        disp_int(i++);
        disp_str(".");
        delay(5);
    }
}

void TestB() {
    int i = 1000;
    while (1) {
        disp_str("B");
        disp_int(i++);
        disp_str(".");
        delay(5);
    }
}

void TestC() {
    int i = 100000;
    while(1) {
        disp_str("C");
        disp_int(i++);
        disp_str(".");
        delay(5);
    }
}

__attribute__((noreturn)) int init_proc() {
    disp_clear();
    disp_str("-----\"kernel_main\" begins-----\n");

    PROCESS *p_proc = proc_table;
    TASK *p_task = init_task;
    u16 selector_ldt = SELECTOR_LDT_FIRST;
    u32 stack_top = (u32)task_stack + STACK_SIZE_TOTAL;

    for(int i = 0; i < NR_TASKS; i++, selector_ldt += 8, p_proc++, p_task++) {
        p_proc->ldt_sel = selector_ldt;
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
        p_proc->regs.eip = p_task->init_eip;
        p_proc->regs.esp = stack_top;
        stack_top -= p_task->stack_size;
        p_proc->regs.eflags = 0x1202; // IF=1, IOPL=1, bit 2 is always 1.
    }

    p_proc_ready = proc_table;
    tss.esp0 = (u32)p_proc_ready + sizeof(STACK_FRAME);
    lldt(p_proc_ready->ldt_sel);
    load_proc_state(&p_proc_ready->regs);
    
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
        disp_colstr("Warning: TestA has been changed!\n", 0x0C);
    else
        disp_colstr("Success!\n", 0x02);
    // 还原 PtDisp
    disp2 = PtDisp;
    PtDisp = t;
    return 0;
}

void schedule(void) {
    static int now = 0;
    now = (now + 1) % NR_TASKS;
    p_proc_ready = proc_table + now;
    return;
}