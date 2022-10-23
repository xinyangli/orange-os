#include "handler.h"
#include "protect.h"
#include "types.h"
#include "klib.h"

void init_gate(GATE *p_gate, u8 type, void *handler, u8 privilege) {
    u32 base = (u32)handler;
    p_gate->offset_low = base & 0xFFFF;
    p_gate->selector = SELECTOR_KERNEL_CS;
    p_gate->dcount = 0;
    p_gate->attr = type | (privilege << 5);
    p_gate->offset_high = (base >> 16) & 0xFFFF;
}

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