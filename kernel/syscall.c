#include "global.h"
#include "handler.h"
#define xstr(s) str(s)
#define str(s) #s
#define foo 4
#define SYSCALL_GET_TICKS 0
#define SYSCALL_WRAPPER(__NAME__, __NUMBER__)                        \
    u32 __NAME__(void) {                                                      \
        __asm__("mov "#__NUMBER__ ", %%eax\n\t"                                                  \
                "int #INT_VECTOR_SYSCALL\n\t"\
                "ret\n\t");                                    \
    }

u32 sys_get_ticks() {
    return ticks;
}

u32 get_ticks() {
    __asm__("mov $"xstr(SYSCALL_GET_TICKS)",%%eax\n\t"
            "int $"xstr(INT_VECTOR_SYSCALL)"\n\t"\
            "ret\n\t");
}

SYSCALL_WRAPPER(get_ticks, "$"SYSCALL_GET_TICKS)