%include "pm.inc"
%include "utils.inc"

org 0100h
jmp LABEL_BEGIN

[SECTION .gdt]

LABEL_GDT:
        Descriptor 0, 0, 0
    .CPL0:
        Descriptor 0, SegCPL0Len - 1, DA_C | DA_32 | DA_DPL0
    .CPL3:
        Descriptor 0, SegCPL3Len - 1, DA_C | DA_32 | DA_DPL3
    .STACK:
        Descriptor 0, TopOfStack, DA_DRWA | DA_32 | DA_DPL0
    .STACK3:
        Descriptor 0, TopOfStack3, DA_DRWA | DA_32 | DA_DPL3
    .TSS:
        Descriptor 0, TSSLen - 1, DA_386TSS
    .VIDEO:
        Descriptor 0b8000h, 0ffffh, DA_DRW | DA_DPL3

; Gates
    .G_PROMOTE:
        Gate SelectorCPL0, OffsetCPL0_1, 0, DA_386CGate | DA_DPL3

GdtLen equ $ - LABEL_GDT
GdtPtr dw GdtLen - 1
       dd 0
; END of [SECTION .gdt]

; GDT Selectors
SelectorCPL0 equ LABEL_GDT.CPL0 - LABEL_GDT 
SelectorCPL3 equ (LABEL_GDT.CPL3 - LABEL_GDT) | SA_RPL3
SelectorStack equ LABEL_GDT.STACK - LABEL_GDT
SelectorStack3 equ (LABEL_GDT.STACK3 - LABEL_GDT) | SA_RPL3
SelectorTSS equ LABEL_GDT.TSS - LABEL_GDT
SelectorVideo equ LABEL_GDT.VIDEO - LABEL_GDT
; Gate Selectors
SelectorCallGateTest equ (LABEL_GDT.G_PROMOTE - LABEL_GDT) | SA_RPL3

; Global stack
[SECTION .gs]
ALIGN 32
[BITS 32]
LABEL_STACK:
    times 512 db 0
TopOfStack equ $ - LABEL_STACK - 1

; Ring3 stack
[SECTION .s3]
ALIGN 32
[BITS 32]
LABEL_STACK3:
    times 512 db 0
TopOfStack3 equ $ - LABEL_STACK3 - 1

; TSS
[SECTION .tss]
ALIGN 32
[BITS 32]
LABEL_TSS:
    DD  0      ; Back
    DD  TopOfStack    ; 0 级堆栈
    DD  SelectorStack    ; 
    DD  0      ; 1 级堆栈
    DD  0      ; 
    DD  0      ; 2 级堆栈
    DD  0      ; 
    DD  0      ; CR3
    DD  0      ; EIP
    DD  0      ; EFLAGS
    DD  0      ; EAX
    DD  0      ; ECX
    DD  0      ; EDX
    DD  0      ; EBX
    DD  0      ; ESP
    DD  0      ; EBP
    DD  0      ; ESI
    DD  0      ; EDI
    DD  0      ; ES
    DD  0      ; CS
    DD  0      ; SS
    DD  0      ; DS
    DD  0      ; FS
    DD  0      ; GS
    DD  0      ; LDT
    DW  0      ; 调试陷阱标志
    DW  $ - LABEL_TSS + 2  ; I/O位图基址
    DB  0ffh      ; I/O位图结束标志
TSSLen    equ  $ - LABEL_TSS

[SECTION .s16]
[BITS 16]
LABEL_BEGIN:
    ; Initialize segment registers
    mov  ax, cs
    mov  ds, ax
    mov  es, ax
    mov  ss, ax
    mov  sp, 0100h

    ; Initilize segments in GDT
    InitSeg LABEL_GDT.CPL0, LABEL_SEG_CPL0 ; Initialize segment CPL0
    InitSeg LABEL_GDT.CPL3, LABEL_SEG_CPL3 ; Initialize segment CPL3
    InitSeg LABEL_GDT.STACK, LABEL_STACK
    InitSeg LABEL_GDT.STACK3, LABEL_STACK3
    InitSeg LABEL_GDT.TSS, LABEL_TSS

    xor eax, eax
    mov ax, ds
    shl eax, 4
    add eax, LABEL_GDT  ; eax <- gdt 基地址
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

    DebugBreak
    ; Jump to 32-bit protected mode
    jmp dword SelectorCPL0:0

; Segment .cpl0 print data from data segment 
[SECTION .cpl0]
[BITS 32]
LABEL_SEG_CPL0:
    DebugBreak

    mov ax, SelectorVideo
    mov gs, ax

    mov ax, SelectorStack
    mov ss, ax

    mov esp, TopOfStack

    ; Save current stack selector to TSS
    mov ax, SelectorTSS
    ltr ax
 
    ; Return to cpl3 section
    push SelectorStack3
    push TopOfStack3
    push SelectorCPL3
    push 0
    retf

    ; Jump back from Ring3 to here
    ; set ax to '0'
    .1:
    DebugBreak
    OffsetCPL0_1 equ $ - $$
    mov ax, '0'
    retf

SegCPL0Len equ $ - $$

[SECTION .cpl3]
[BITS 32]
LABEL_SEG_CPL3:
    DebugBreak
    ; Display char '3' on screen
    mov ax, SelectorVideo
    mov gs, ax

    mov edi, (80 * 12 + 0) * 2; 12th line, 0th column
    mov ah, 0ch
    mov al, '3'
    mov [gs:edi], ax 

    DebugBreak
    ; Call cpl0 through gate
    call SelectorCallGateTest:0
    
    DebugBreak
    ; Print ax to screen
    add edi, 2
    mov ah, 0ch
    mov [gs:edi], ax

    jmp $

SegCPL3Len equ $ - $$