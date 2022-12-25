[section .text]

global _start

_start:
    pushad
    jmp get_msg_addr
main:
    pop edx
    xor eax, eax
    int 0x90
    popad
    mov ebx, 0x00000000 ;在inject时要用文件原始入口地址覆盖掉
    jmp ebx
get_msg_addr:
    call main

msg:
    db "This file is injected!", 0xA, 0x0
