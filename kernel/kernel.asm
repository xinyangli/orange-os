extern kernel_start
extern PtDisp

global _start


[SECTION .text]
_start:
    call kernel_start
    ; 调用中断
    int 80h
    ; 进入死循环
    jmp $
