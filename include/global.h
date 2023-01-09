#ifndef ORANGE_OS_GLOBAL_H
#define ORANGE_OS_GLOBAL_H

#include "types.h"
#include "x86def.h"
#include "proc.h"

extern void *StackTop; /* Kernel Stack */

extern int PtDisp;

extern int k_reenter;

extern proc_t *p_proc_ready;

extern proc_t proc_table[];
extern char task_stack[];
extern TASK init_task[];

extern void *syscall_table[];

extern u32 old_esp;

// For get_ticks
extern u32 ticks;

u32 seg2phys(u16 seg);

#endif // ORANGE_OS_GLOBAL_H