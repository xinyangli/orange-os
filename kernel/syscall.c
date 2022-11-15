#include "global.h"
#include "handler.h"
#define xstr(s) str(s)
#define str(s) #s
#define foo 4
#define SYSCALL_GET_TICKS 0

u32 sys_get_ticks() {
    return ticks;
}
