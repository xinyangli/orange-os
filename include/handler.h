#ifndef ORANGE_OS_HANDLER_H
#define ORANGE_OS_HANDLER_H
#include "x86def.h"
#include "asm.h"

#define INT_VECTOR_DIVIDE 0x0
#define INT_VECTOR_DEBUG 0x1
#define INT_VECTOR_NMI 0x2
#define INT_VECTOR_BREAKPOINT 0x3
#define INT_VECTOR_OVERFLOW 0x4
#define INT_VECTOR_BOUNDS 0x5
#define INT_VECTOR_INVAL_OP 0x6
#define INT_VECTOR_COPROC_NOT 0x7
#define INT_VECTOR_DOUBLE_FAULT 0x8
#define INT_VECTOR_COPROC_SEG 0x9
#define INT_VECTOR_INVAL_TSS 0xA
#define INT_VECTOR_SEG_NOT 0xB
#define INT_VECTOR_STACK_FAULT 0xC
#define INT_VECTOR_PROTECTION 0xD
#define INT_VECTOR_PAGE_FAULT 0xE
#define INT_VECTOR_COPROC_ERR 0x10

#define INT_VECTOR_IRQ_CLOCK 0x20
#define INT_VECTOR_IRQ_KEYBOARD 0x21
#define INT_VECTOR_IRQ_CASCADE 0x22

#define INT_VECTOR_IRQ_RTCLOCK 0x28

#define INT_VECTOR_SYS_CALL 0x90

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20

void empty_handler();
void clock_handler();
void KeyBoardHandler();
void init_idt();
void set_8259a();

static void inline mask_hwint() {
    ;
}

static void inline unmask_hwint() {
    ;
}

typedef void (*ptr_handler_t)(void);

// TODO: mask correspoding 8259A when handling interrupt
// For re-entered interrupt, state is saved onto kernel stack
#define HANDLER_WRAPPER(__NAME__, __NUMBER__, __BODY__)                        \
    static void inline __handler_##__NAME__(void) {                                          \
        save_proc_state();                                                     \
        ++k_reenter;                                                           \
        if (k_reenter == 0)                                                    \
            __asm__ __volatile__("mov $%0, %%esp\n" : : "m"(StackTop));         \
        outb(PIC1_COMMAND, PIC_EOI);                                           \
        sti();                                                                 \
        {__BODY__};                                                            \
        cli();                                                                 \
        if (k_reenter == 0) {                                                  \
            lldt(p_proc_ready->ldt_sel);                                       \
            tss.esp0 = (u32)p_proc_ready + sizeof(STACK_FRAME);                \
        }                                                                      \
        --k_reenter;                                                           \
        load_proc_state(&p_proc_ready->regs);                                  \
        iret();                                                                \
    }

#endif // ORANGE_OS_HANDLER_H