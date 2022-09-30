
; Usage: InitSeg label_gdt, label_seg
; Initilize base address of a segment descriptor in the GDT
;
; label_gdt: Label name of Descriptor in gdt
; label_seg: Label at the start of the segment
%macro InitSeg 2
    xor eax, eax
    mov ax, cs
    shl eax, 4
    add eax, %2
    mov word [%1 + 2], ax
    shr eax, 16
    mov byte [%1 + 4], al
    mov byte [%1 + 7], ah
%endmacro

; Usage: DebugBreak
; Set magic breakpoint for bochs
%macro DebugBreak 0
    %ifdef DEBUG
    xchg bx, bx
    %endif
%endmacro