#include "handler.h"
#include "types.h"
#include "global.h"
#include "klib.h"
#include "asm.h"
#include "proc.h"

typedef void (*ptr_handler_t)(void);

HANDLER_WRAPPER(
    clock, INT_VECTOR_IRQ_CLOCK,
    ticks++;
    if (k_reenter != 0) {
        // re-entered interrupt
        return;
    }
    disp_int(ticks);
    schedule();)

ptr_handler_t handlers[IDT_SIZE] = {
    [INT_VECTOR_IRQ_CLOCK] = __handler_clock,
};

void init_idt() {
    // 初始化中断重入计数器
    k_reenter = -1;
    // 初始化中断门
    int i;
    for (i = 0; i < IDT_SIZE; i++)
        init_gate(&idt[i], DA_386IGate,
                  handlers[i] != 0 ? handlers[i] : empty_handler, 0);
    // 设置 idt_ptr
    u16 *p_idt_limit = (u16 *)(&idt_ptr[0]);
    u32 *p_idt_base = (u32 *)(&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
    *p_idt_base = (u32)&idt;
    // 设置 8259a
    set_8259a();
    // 设置 IMREG 开启键盘和定时器中断
    outb(0x21, 0xFE); // 主 <= OCW1
    outb(0xA1, 0xFF); // 从 <= OCW1
    lidt(idt_ptr);
}

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
