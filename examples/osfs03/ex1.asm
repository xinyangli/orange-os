%include "pm.inc"
%include "utils.inc"

org 0100h
jmp LABEL_BEGIN

[SECTION .gdt]

LABEL_GDT:
        Descriptor 0, 0, 0
    .DATA:
        Descriptor 0, 0ffffh, DA_DRW
    .WRITE:
        Descriptor 0, SegWriteLen - 1, DA_C + DA_32
    .LDT:
        Descriptor 0, SegLDTLen - 1, DA_LDT
    .VIDEO:
        Descriptor 0b8000h, 0ffffh, DA_DRW

GdtLen equ $ - LABEL_GDT
GdtPtr dw GdtLen - 1
       dw LABEL_GDT
; END of [SECTION .gdt]

; GDT Selectors
SelectorData equ LABEL_GDT.DATA - LABEL_GDT
SelectorWrite equ LABEL_GDT.WRITE - LABEL_GDT
SelectorLDT equ LABEL_GDT.LDT - LABEL_GDT
SelectorVideo equ LABEL_GDT.VIDEO - LABEL_GDT

[SECTION data]
LABEL_DATA:
    times 512 db 0
    .str:
        db "Hello, world!", 0
    DataStrOffset equ .str - $$

[SECTION .s16]
[BITS 16]
LABEL_BEGIN:

    ; Initilize segments in GDT
    InitSeg LABEL_GDT.DATA, LABEL_DATA ; Initialize segment DATA
    InitSeg LABEL_GDT.WRITE, LABEL_SEG_WRITE ; Initialize segment WRITE
    InitSeg LABEL_GDT.LDT, LABEL_LDT ; Initialize segment LDT
    
    ; Initialize segments in LDT
    InitSeg LABEL_LDT.READ, LABEL_SEG_READ ; Initialize segment READ

    xor eax, eax
    mov ax, ds
    shl eax, 4
    add eax, LABEL_GDT ; eax <- gdt 基地址
    mov dword [GdtPtr + 2], eax ; [GdtPtr + 2] <- gdt 基地址

    ; Load GDT
    lgdt [GdtPtr]
    
    cli

    ; Enable A20 address line
    in al, 92h
    or al, 00000010b
    out 92h, al

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Jump to 32-bit protected mode
    jmp dword SelectorWrite:0

[SECTION .s32]
[BITS 32]
LABEL_SEG_WRITE:
    DebugBreak
    ; Write string to data segment
    mov ax, SelectorData
    mov ds, ax ; source segment for movs
    mov es, ax ; destination segment for movs
    
    mov esi, DataStrOffset ; source offset
    mov edi, 0 ; desination offset
    cld
    .1:
        movsb ; move byte from ds:[esi] to es:[edi]
        test al, al ; test for '\0'
        jnz .1
    .2:  
        ; Load LDT
        mov ax, SelectorLDT
        lldt ax
        ; Jump to LDT Read sction
        jmp SelectorLDTRead:0

SegWriteLen equ $ - $$

[SECTION .ldt]
LABEL_LDT:
.READ:
    Descriptor 0, SegReadLen - 1, DA_C + DA_32

SelectorLDTRead equ (LABEL_LDT.READ - LABEL_LDT) | SA_TIL
SegLDTLen equ $ - $$

[SECTION .s32]
[BITS 32]
LABEL_SEG_READ:
    DebugBreak

    mov ax, SelectorVideo
    mov gs, ax

    mov ah, 0ch ; set font color to red
    mov esi, 0 ; source offset in data segment
    mov edi, (80 * 10 + 0) * 2 ; Coordinate (10, 0) in video memory
    cld
    .1:
        lodsb
        test al, al
        jz .2
        mov [gs:edi], ax
        add edi, 2
        jmp .1
    .2:
        jmp $

SegReadLen equ $ - $$