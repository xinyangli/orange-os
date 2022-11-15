#ifndef ORANGE_OS_HANDLER_H
#define ORANGE_OS_HANDLER_H
#include "global.h"
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

typedef void (__handler_pic0)(void);
typedef void (__handler_pic1)(void);
// type: pic0, pic1, syscall
#define __glue2(x,y) x##y
#define __glue(x,y) __glue2(x,y)

extern void *handlers[IDT_SIZE];
void init_idt();
void set_8259a();


#endif // ORANGE_OS_HANDLER_H