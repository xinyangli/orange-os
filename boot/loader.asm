global start
extern main, dataseg

%include "pm.inc"
%include "utils.inc"

[SECTION .text]
start:
[BITS 16]
        cli

        mov ax, 0B800h
        mov gs, ax
        mov ah, 0Fh                             ; 0000: 黑底    1111: 白字
        mov al, 'L'
        mov [gs:((80 * 0 + 39) * 2)], ax        ; 屏幕第 0 行, 第 39 列。

        mov eax, dataseg
        mov ds, ax
        ; gdt.ptr是一个32位指针
        ; TODO: 或许有更优雅的方式加载 gdt
        lgdt [ds:gdt.ptr - gdt]
        
        ; Enable A20
        in al, 92h
        or al, 00000010b
        out 92h, al

        ; 启用保护模式
        mov eax, cr0
        or eax, 1
        mov cr0, eax
        
        jmp dword SelectorText:start32

start32:
[BITS 32]

        ; 初始化数据选择子
        mov ax, SelectorData
        mov ss, ax
        mov ds, ax
        mov es, ax

        mov ax, SelectorVideo
        mov gs, ax

        ; 初始化栈
        mov esp, start

        call main

        ; 跳转到C不应该返回，返回则Break后死循环
        BOCHS_BREAK
        hlt

[SECTION .data]
align 32
; TODO: 确保GDT是对齐的
gdt:
.null:  Descriptor 0, 0, 0
.text:  Descriptor 0, 0xFFFFF, DA_CR | DA_32 | DA_LIMIT_4K
.data:  Descriptor 0, 0xFFFFF, DA_DRW | DA_32 | DA_LIMIT_4K
.video: Descriptor 0B8000h, 0ffffh, DA_DRW | DA_DPL3

.ptr:   dw $ - gdt - 1
        dd gdt

SelectorText equ gdt.text - gdt
SelectorData equ gdt.data - gdt
SelectorVideo equ gdt.video - gdt
