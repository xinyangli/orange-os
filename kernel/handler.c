#include "handler.h"
#include "types.h"
#include "global.h"
#include "klib.h"
#include "asm.h"
#include "proc.h"
#include "kqueue.h"
#include "keyboard.h"

// Default irq handler
void irqreport(void) {
}

// clock handler
void irqclock(void) {
    ++ticks;
    --p_proc_ready->time;
    if (k_reenter != 0) {
        // re-entered interrupt
        return;
    }
    schedule();
}


// Keyboard handler
void irqkeyboard(void) {
    u8 scan_code = inb(0x60);

    /* Discard scan_code if buffer is full */
    if(!kqueue_full(p_kb_queue)) {
        kqueue_push(p_kb_queue, &scan_code, sizeof(u8));
    }
}

void init_idt() {
    // 初始化中断门
    for (int i = 0; i < IDT_SIZE; i++) {
        if (i == INT_VECTOR_SYSCALL)
            init_gate(&idt[i], DA_386IGate, handlers[i], 1);
        else
            init_gate(&idt[i], DA_386IGate, handlers[i], 0);
    }
    // 设置 idt_ptr
    u16 *p_idt_limit = (u16 *)(&idt_ptr[0]);
    u32 *p_idt_base = (u32 *)(&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
    *p_idt_base = (u32)&idt;
    // 设置 8259a
    set_8259a();
    // 设置 IMREG 开启键盘和定时器中断
    outb(0x21, 0xFC); // 主 <= OCW1
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
