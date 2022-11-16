#ifndef ORANGE_OS_PROC_H
#define ORANGE_OS_PROC_H
#include "asm.h"
#include "types.h"
#include "x86def.h"

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
                               /* 2 is LDT_SIZE - avoid include x86def.h */
    int q;  // 记录当前在哪个队列
    int pos; // 记录在队列里的排序
    int time; // 记录当前还剩下多少时间片
    int wait;
    
    u32 pid;                   /* process id passed in from MM */
    char p_name[16];           /* name of the process */
} PROCESS;

typedef struct {
    u32 init_eip;
    u32 stack_size;
} TASK;

/* Number of tasks */
#define NR_TASKS 4

/* stacks of tasks */
#define STACK_SIZE_TESTA 0x8000
#define STACK_SIZE_TESTB 0x4000
#define STACK_SIZE_TESTC 0x2000
#define STACK_SIZE_TESTD 0x2000

#define STACK_SIZE_TOTAL 0xE000


void to_kstack();

void TestA();
void TestB();
void TestC();
int init_proc();
int check_testA();

static void inline load_proc_state(STACK_FRAME *p_frame) {
    __asm__ __volatile__("mov %0, %%esp\n"
                       "pop %%gs\n"
                       "pop %%fs\n"
                       "pop %%es\n"
                       "pop %%ds\n"
                       "popal\n"
                       :
                       : "rm"(p_frame)
                       : "memory");
}


static void inline save_proc_state() {
    __asm__ __volatile__("pushal\n"
                         "pushl %%ds\n"
                         "pushl %%es\n"
                         "pushl %%fs\n"
                         "pushl %%gs\n"
                         :
                         :
                         : "memory");
}

void schedule(void);

void restart(void);

#endif // ORANGE_OS_PROC_H