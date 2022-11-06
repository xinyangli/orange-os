extern kernel_start
extern PtDisp

extern	cstart
extern	kernel_main
extern	exception_handler
extern	spurious_irq
extern	disp_str
extern	delay

; 导入全局变量
extern	gdt_ptr
extern	idt_ptr

extern	disp_pos

global _start, restart

[SECTION .bss]
    StackSpace resb 2*1024
    StackTop:

[SECTION .text]
_start:
    mov esp, StackTop
    call kernel_start
    ; 调用中断
    int 80h
    ; 进入死循环
    jmp $

restart:
    mov esp, [p_proc_ready]
    lldt [esp + P_LDT_SEL] 
    lea eax, [esp + P_STACKTOP]
    mov dword [tss + TSS3_S_SP0], eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 4
    iretd