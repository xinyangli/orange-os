#ifndef ORANGE_OS_ASM_H
#define ORANGE_OS_ASM_H
#include "types.h"

static inline u8 inb(u16 port) {
    u8 data;

    __asm__ __volatile__("in %1,%0" : "=a"(data) : "d"(port));
    return data;
}

static inline void outb(u16 port, u8 data) {
    __asm__ __volatile__("out %0,%1" : : "a"(data), "d"(port));
}

static inline void insl(int port, void *addr, int cnt) {
    __asm__ __volatile__("cld; rep insl"
                         : "=D"(addr), "=c"(cnt)
                         : "d"(port), "0"(addr), "1"(cnt)
                         : "memory", "cc");
}

static inline void lldt(u16 sel) {
    __asm__ __volatile__("lldtw %0" : : "r"(sel));
}

static inline void iret() {
    __asm__ __volatile__("iretl");
}

static inline void BOCHS_BREAK() {
    __asm__ __volatile__("xchgw %bx, %bx");
}
#endif // ORANGE_OS_ASM_H