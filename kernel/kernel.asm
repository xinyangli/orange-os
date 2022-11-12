extern kernel_start
extern PtDisp

global StackTop, _start

[SECTION .bss]
    StackSpace resb 2*1024
    StackTop:

[SECTION .text]
_start:  
    mov esp, StackTop
    ; 设置了 gdt idt 等
    call kernel_start
    ; 调用中断
    int 80h
    ; 进入死循环
    jmp $
