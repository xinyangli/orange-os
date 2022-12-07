#ifndef ORANGE_OS_GLOBAL_H
#define ORANGE_OS_GLOBAL_H

#include "types.h"
#include "x86def.h"
#include "proc.h"

#define GDT_SIZE 128
#define IDT_SIZE 256

/* 权限 */
#define PRIVILEGE_KRNL 0
#define PRIVILEGE_TASK 1
#define PRIVILEGE_USER 3

/* RPL */
#define RPL_KRNL 0
#define RPL_TASK 1
#define RPL_USER 3

extern void *StackTop; /* Kernel Stack */

extern int PtDisp;
extern u8 gdt_ptr[6];
extern DESCRIPTOR gdt[GDT_SIZE];
extern u8 idt_ptr[6];
extern GATE idt[IDT_SIZE];

extern u32 k_reenter;

extern TSS tss;
extern proc_t *p_proc_ready;

extern proc_t proc_table[];
extern char task_stack[];
extern TASK init_task[];

extern void *syscall_table[];

extern u32 old_esp;

// For get_ticks
extern u32 ticks;

void init_gate(GATE *p_gate, u8 type, void *handler, u8 privilege);
void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute);

u32 seg2phys(u16 seg);

#endif // ORANGE_OS_GLOBAL_H