#ifndef ORANGE_OS_GLOBAL_H
#define ORANGE_OS_GLOBAL_H

#include "types.h"
#include "proc.h"
#include "x86def.h"

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

extern int PtDisp;
extern u8 gdt_ptr[6];
extern DESCRIPTOR gdt[GDT_SIZE];
extern u8 idt_ptr[6];
extern GATE idt[IDT_SIZE];

extern u32 k_reenter;

extern TSS tss;
extern PROCESS *p_proc_ready;

extern PROCESS proc_table[];
extern char task_stack[];

extern u32 old_esp;

void init_gate(GATE *p_gate, u8 type, void *handler, u8 privilege);
void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute);

u32 seg2phys(u16 seg);

#endif // ORANGE_OS_GLOBAL_H