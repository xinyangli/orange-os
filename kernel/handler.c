#include "handler.h"
#include "protect.h"
#include "types.h"
#include "klib.h"

void set_8259a() {
    out_byte(0x20, 0x11); // 主8259 <= ICW1
    out_byte(0xA0, 0x11); // 从8259 <= ICW1

    out_byte(0x21, 0x20); // 主8259 <= ICW2
    out_byte(0xA1, 0x28); // 从8259 <= ICW2

    out_byte(0x21, 0x4); // 主8259 <= ICW3
    out_byte(0xA1, 0x2); // 从8259 <= ICW3

    out_byte(0x21, 0x1); // 主8259 <= ICW4
    out_byte(0xA1, 0x1); // 从8259 <= ICW4
}