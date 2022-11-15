#include "global.h"
#include "proc.h"
#include "types.h"
#include "x86def.h"
#include "syscall.h"

int PtDisp;
u8 gdt_ptr[6];
DESCRIPTOR gdt[GDT_SIZE];
u8 idt_ptr[6];
GATE idt[IDT_SIZE];

u32 k_reenter = -1;
u32 old_esp;

u32 ticks;

TSS tss;
PROCESS *p_proc_ready;

PROCESS proc_table[NR_TASKS];
char task_stack[STACK_SIZE_TOTAL];

TASK init_task[NR_TASKS] = {
    {(u32)TestA, STACK_SIZE_TESTA},
    {(u32)TestB, STACK_SIZE_TESTB},
    {(u32)TestC, STACK_SIZE_TESTC}
};

void *syscall_table[] = {
    sys_get_ticks,
};

void init_gate(GATE *p_gate, u8 type, void *handler, u8 privilege) {
    u32 base = (u32)handler;
    p_gate->offset_low = base & 0xFFFF;
    p_gate->selector = SELECTOR_KERNEL_CS;
    p_gate->dcount = 0;
    p_gate->attr = type | (privilege << 5);
    p_gate->offset_high = (base >> 16) & 0xFFFF;
}

void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute) {
    p_desc->limit_low = limit & 0x0FFFF;     // 段界限 1  (2 字节)
    p_desc->base_low = base & 0x0FFFF;       // 段基址 1  (2 字节)
    p_desc->base_mid = (base >> 16) & 0x0FF; // 段基址 2  (1 字节)
    p_desc->attr1 = attribute & 0xFF;        // 属性 1
    p_desc->limit_high_attr2 =
        ((limit >> 16) & 0x0F) | (attribute >> 8) & 0xF0; // 段界限 2 + 属性 2
    p_desc->base_high = (base >> 24) & 0x0FF; // 段基址 3  (1 字节)
}

u32 seg2phys(u16 seg) {
    DESCRIPTOR *p_dest = &gdt[seg >> 3];
    return (p_dest->base_high << 24 | p_dest->base_mid << 16 |
            p_dest->base_low);
}