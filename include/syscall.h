#ifndef ORANGE_OS_SYSCALL_H
#define ORANGE_OS_SYSCALL_H
#include "types.h"
#define SYSCALL_GET_TICKS 0
u32 sys_get_ticks();
u32 get_ticks();
#endif // ORANGE_OS_SYSCALL_H