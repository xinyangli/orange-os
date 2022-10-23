extern kernel_start
extern PtDisp

global _start

[SECTION .bss]
times 1024 dw 0
StackTop:

[SECTION .text]
_start:
    mov esp, StackTop
    call kernel_start
    ; 调用中断
    int 80h
    ; 进入死循环
    jmp $
