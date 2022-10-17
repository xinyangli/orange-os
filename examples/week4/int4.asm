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

    mov ax, cs
    mov ds, ax
    mov si, _int9

    mov ax, 0
    mov es, ax
    mov di, 204h

    ; 写入中断处理程序
    cld
    mov cx, _int9end - _int9
.loop:
    lodsb
    stosb
    loop .loop

    cli
    ; 保存原中断向量 60:d0d6
    ; 但是我并不知道怎么用这个东西来 jmp far
    mov eax, dword [es:36]
    mov [es:200h], eax
    ; 修改 9 号中断的中断向量
    mov word [es:36], 204h
    mov word [es:38], 0
    sti

    mov ax, 4c00h
    int 21h

    jmp $

_int9:
    push ax
    push gs
    ; 运行自定义的中断程序
    mov ax, 0b800h
    mov gs, ax
    mov byte [gs:(CharPos + 1)], 0Ch
    inc byte [gs:CharPos]
    jmp .exit
.exit:
    pop gs
    pop ax
    ; 由原本的中断程序接管
    ; jmp dword 60h:0d0d6h
    push word [cs:202h]
    push word [cs:200h]
    retf

_int9end:
    nop

CharPos equ ((80 * 0 + 75) * 2)
