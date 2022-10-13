%include "utils.inc"

%define DEBUG

org 0100h
jmp LABEL_BEGIN

[SECTION .real]
[BITS 16]
LABEL_BEGIN:
    mov ax, cs
    mov ss, ax
    mov sp, 100h

    mov ax, 0b800h
    mov gs, ax

    mov byte [gs:((80 * 0 + 70) * 2)], `A`
    mov byte [gs:((80 * 0 + 70) * 2 + 1)], 0Ch

    mov ax, cs
    mov ds, ax
    mov si, _int9

    mov ax, 0
    mov es, ax
    mov di, 200h

    ; 写入中断处理程序
    cld
    mov cx, _int9end - _int9
.loop:
    lodsb
    stosb
    loop .loop

    ; 修改 9 号中断的中断向量
    cli
    mov word [es:36], 200h
    mov word [es:38], 0
    sti

    ;mov ax, 4c00h
    ;int 21h

    jmp $

_int9:
    push eax
    push gs
    mov ax, 0b800h
    mov gs, ax
    in al, 60h ; 读取缓冲区
    cmp byte [gs:((80 * 0 + 70) * 2)], `Z`
    je .2
    inc byte [gs:((80 * 0 + 70) * 2)]
    jmp .exit
.2:
    mov byte [gs:((80 * 0 + 70) * 2)], `A`
.exit:
    mov al, 20h
    out 20h, al ; 输出 EOI
    pop gs
    pop eax
    iret

_int9end:
    nop
