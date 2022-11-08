#include "handler.h"
#include "types.h"
#include "klib.h"
#include "asm.h"

void set_8259a() {
    outb(0x20, 0x11); // 主8259 <= ICW1
    outb(0xA0, 0x11); // 从8259 <= ICW1

    outb(0x21, 0x20); // 主8259 <= ICW2
    outb(0xA1, 0x28); // 从8259 <= ICW2

    outb(0x21, 0x4); // 主8259 <= ICW3
    outb(0xA1, 0x2); // 从8259 <= ICW3

    outb(0x21, 0x1); // 主8259 <= ICW4
    outb(0xA1, 0x1); // 从8259 <= ICW4
}