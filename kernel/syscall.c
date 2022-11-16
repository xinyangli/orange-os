#include "asm.h"
#include "global.h"
#include "handler.h"
#include "syscall.h"
#define SYSCALL_GET_TICKS 0

// TODO: save_ret is used in assembly. Maybe we can make it inline?
void save_ret() {
    __asm__ (
        "mov %%eax, %0" : "=m"(p_proc_ready->regs.eax) : : "eax"
    );
}

// TODO: syscall with arguments
void syscall(u32 number) {
    __asm__ __volatile__ (
        "mov %1, %%eax\n"
        "int %0\n"
        : /* No write */
        : "i"(INT_VECTOR_SYSCALL), "r"(number)
        : "eax"
    );
}

u32 sys_get_ticks() {
    return ticks;
}

u32 get_ticks() {
    syscall(SYSCALL_GET_TICKS);
    u32 ret;
    __asm__ (
        "mov %%eax, %0":"=r"(ret)
    );
    return ret;
}