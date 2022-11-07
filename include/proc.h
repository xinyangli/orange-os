#ifndef ORANGE_OS_PROC_H
#define ORANGE_OS_PROC_H
#include "types.h"
#include "protect.h"

#define LDT_SIZE 128

typedef struct {
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
	/* popad start */
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 kernel_esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
	/* popad end */
    u32 retaddr;
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
} STACK_FRAME;

typedef struct {
    STACK_FRAME regs;          /* process' registers saved in stack frame */
    u16 ldt_sel;               /* selector in gdt giving ldt base and limit*/
    DESCRIPTOR ldts[LDT_SIZE]; /* local descriptors for code and data */
                               /* 2 is LDT_SIZE - avoid include protect.h */
    u32 pid;                   /* process id passed in from MM */
    char p_name[16];           /* name of the process */
} PROCESS;

/* Number of tasks */
#define NR_TASKS 1

/* stacks of tasks */
#define STACK_SIZE_TESTA 0x8000

#define STACK_SIZE_TOTAL STACK_SIZE_TESTA

void TestA();
int init_proc();
int check_testA();

#endif // ORANGE_OS_PROC_H