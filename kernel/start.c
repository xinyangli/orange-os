#include "klib.h"
#include "global.h"
#include "proc.h"
#include "types.h"
#include "x86def.h"
#include "handler.h"
#include "asm.h"
#include "tty.h"
#include "console.h"

void relocate_gdt() {
    gain_gdt();
    u16 *p_gdt_limit = (u16 *)(&gdt_ptr[0]);
    u32 *p_gdt_base = (u32 *)(&gdt_ptr[2]);
    memcpy(gdt, (void *)(*p_gdt_base), (size_t)p_gdt_limit + 1); // why &gdt
    *p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32)&gdt;
    apply_gdt();
}

void add_ldt_desc() {
    // LDTs for each task
    PROCESS *p_proc = proc_table;
    u16 index_ldt = INDEX_LDT_FIRST;
    for (int i = 0; i < NR_TASKS; i++, p_proc++, index_ldt++) {
        init_descriptor(
            &gdt[index_ldt],
            vir2phys(seg2phys(SELECTOR_KERNEL_DS), proc_table[i].ldts),
            LDT_SIZE * sizeof(DESCRIPTOR) - 1, DA_LDT);
    }
}

void add_tss_desc() {
    // Add TSS descriptor to gdt[INDEX_TSS]
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = SELECTOR_KERNEL_DS;
    init_descriptor(&gdt[INDEX_TSS],
                    vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss),
                    sizeof(TSS) - 1, DA_386TSS);
    tss.iobase = sizeof(tss);
}

static char SUPER_MARIO[] = {
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 42, 42,
        42, 42, 42, 42, 42, 42, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 10, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35, 35,
        46, 46, 46, 46, 35, 46, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 35, 46, 46, 35, 35, 35, 46, 46, 46, 46, 46, 35, 35, 46, 46, 46,
        46, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35,
        46, 46, 46, 46, 46, 46, 46, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 35, 35, 35, 10, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
        46, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 46,
        46, 46, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 46, 46, 46, 35,
        10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35,
        42, 35, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 35, 46, 35, 46, 35, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 35, 46, 35, 46, 35, 10, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 35, 35, 35, 35, 42, 42, 42, 42, 42, 42, 42, 35, 35, 35,
        35, 35, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 46,
        35, 46, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 46, 35, 46, 35,
        10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 46, 46, 46, 35, 42, 42,
        42, 46, 42, 42, 42, 42, 46, 42, 35, 35, 35, 46, 46, 46, 46, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 35, 46, 46, 46, 35, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 32, 35, 46, 46, 46, 35, 10, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 32, 46, 46, 46, 46, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 35,
        35, 46, 46, 46, 46, 46, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35,
        35, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35, 10,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 46, 46, 46, 46, 42, 42, 42,
        42, 32, 32, 32, 32, 42, 42, 42, 42, 42, 46, 46, 46, 46, 10, 32, 32, 32,
        32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35, 35, 35, 32, 32, 32, 32,
        32, 32, 32, 32, 35, 35, 35, 35, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32,
        32, 32, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 32, 32, 32, 32, 35, 35,
        35, 35, 35, 35, 10, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 10, 35, 46, 46, 46,
        35, 46, 46, 46, 46, 46, 46, 35, 46, 35, 35, 46, 46, 46, 35, 46, 46, 46,
        46, 46, 46, 35, 46, 35, 35, 46, 46, 46, 35, 46, 46, 46, 46, 46, 46, 35,
        46, 35, 35, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
        45, 45, 45, 35, 10, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 45, 45, 45, 45, 45, 45,
        45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 35, 10, 35, 46, 46, 35,
        46, 46, 46, 46, 35, 46, 46, 46, 46, 35, 35, 46, 46, 35, 46, 46, 46, 46,
        35, 46, 46, 46, 46, 35, 35, 46, 46, 35, 46, 46, 46, 46, 35, 46, 46, 46,
        46, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 10, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 35, 45, 45,
        45, 45, 45, 45, 45, 45, 45, 45, 35, 10, 35, 46, 46, 46, 46, 46, 35, 46,
        46, 46, 46, 46, 46, 35, 35, 46, 46, 46, 46, 46, 35, 46, 46, 46, 46, 46,
        46, 35, 35, 46, 46, 46, 46, 46, 35, 46, 46, 46, 46, 46, 46, 35, 32, 32,
        32, 32, 35, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 35, 10, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 32, 32, 32, 32, 35, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
        35, 10, 35, 46, 35, 46, 46, 35, 46, 46, 46, 46, 35, 46, 46, 35, 35, 46,
        35, 46, 46, 35, 46, 46, 46, 46, 35, 46, 46, 35, 35, 46, 35, 46, 46, 35,
        46, 46, 46, 46, 35, 46, 46, 35, 32, 32, 32, 32, 35, 45, 45, 45, 45, 45,
        45, 45, 45, 45, 45, 35, 10, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 32, 32, 32, 32, 35,
        35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 0};

void kernel_start() {
    // 清屏
    disp_clear();
    // 显示超级马里奥
    disp_str(SUPER_MARIO);
    disp_str("\n");
    // 设置 gdt
    relocate_gdt();
    add_ldt_desc();
    add_tss_desc();
    apply_tss(INDEX_TSS << 3);
    // 设置 idt
    init_idt();
    // show graphic for some time
    delay(3);
    tty_init(ttys);
    for (int i = 0; i < NR_CONSOLES; i++)
        console_init(&ttys[i].console, i);
    // 进程
    init_proc();
    return;
}