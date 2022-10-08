%include "pm.inc"
%include "utils.inc"

%define DEBUG

;DebugBreak

org 0100h
jmp LABEL_BEGIN

[SECTION .gdt]
; 描述符表
LABEL_GDT:
.EMPTY: Descriptor 0, 0, 0

.NORMAL: Descriptor 0, 0ffffh, DA_DRW
SelectorNormal equ LABEL_GDT.NORMAL - LABEL_GDT

.CODE16: Descriptor 0, 0ffffh, DA_C 
SelectorCode16 equ LABEL_GDT.CODE16 - LABEL_GDT

.CODE32: Descriptor 0, SegCode32Len - 1, DA_C + DA_32
SelectorCode32 equ LABEL_GDT.CODE32 - LABEL_GDT

.VIDEO: Descriptor 0b8000h, 0ffffh, DA_DRW
SelectorVideo equ LABEL_GDT.VIDEO - LABEL_GDT

.DATA: Descriptor 0, SegDataLen - 1, DA_DRW
SelectorData equ LABEL_GDT.DATA - LABEL_GDT

.STACK: Descriptor 0, TopOfStack, DA_DRWA + DA_32
SelectorStack equ LABEL_GDT.STACK - LABEL_GDT

GdtLen equ $ - LABEL_GDT
GdtPtr dw GdtLen - 1, 0, 0 

; --------------- end of .gdt ------------------

[SECTION .idt]
ALIGN 32 
[BITS 32] ; 这里为啥要对齐和指明 32 位
LABEL_IDT:

.00h_to_1Fh: ; 填充
    times 20h dw 0, 0, 0, 0

.20h: ; 这只是个标签，实际的位置取决于前面填充的数量
    Gate SelectorCode32, ClockHandler, 0, DA_386IGate

.21h_to_7Fh: ; 填充
    times 5Fh dw 0, 0, 0, 0

.80h:
    Gate SelectorCode32, UserIntHandler, 0, DA_386IGate

; 就算要调用也是用中断号，而不是用选择子，所以不需要定义选择子

IdtLen equ $ - LABEL_IDT
IdtPtr dw IdtLen - 1, 0, 0

; --------------- end of .idt ------------------

[SECTION .stack]
ALIGN 32
[BITS 32]
LABEL_STACK: times 512 db 0
TopOfStack equ $ - $$ - 1

; --------------- end of .stack ------------------

[SECTION .data]
ALIGN 32
[BITS 32]
LABEL_DATA:

_RealSP: dw 0
RealSP equ _RealSP - $$

_strPro: db `Hi! Here is protect mode!\n`, 0 ; 使用反引号可以在 nasm 中使用反义字符
strPro equ _strPro - $$

; --- lib.inc ---
_strCRLF: db `\n`, 0 ; 回车换行
strCRLF equ _strCRLF - $$

_PtDisp: dd (80 * 1 + 0) * 2 ; 默认从第 1 行第 0 列开始显示，每个字符两字节
PtDisp equ _PtDisp - $$
; --- end ---

_SavedIDTR: ; pmtest9 开了两个 dd，但其实 IDTR 只有 48 位？
    dw 0, 0, 0 
SavedIDTR equ _SavedIDTR - $$

_SavedIMREG: db 0
SavedIMREG equ _SavedIMREG - $$

SegDataLen equ $ - $$

; --------------- end of .data ------------------

[SECTION .start]
[BITS 16]
LABEL_BEGIN:
    
    ; 初始化段寄存器
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0100h ; 被装载到 0100h，前面的空白内容可以用来当实模式栈
    
    ;DebugBreak

    mov [_RealSP], sp
    mov [LABEL_GO_BACK + 3], ax ; 设置返回时的段地址
    
    ; 初始化描述符
    InitSeg LABEL_GDT.CODE16, LABEL_CODE16
    InitSeg LABEL_GDT.CODE32, LABEL_CODE32
    InitSeg LABEL_GDT.DATA, LABEL_DATA
    InitSeg LABEL_GDT.STACK, LABEL_STACK

    ; 补充 GdtPtr
    xor eax, eax
    mov ax, ds
    shl eax, 4
    add eax, LABEL_GDT ; 计算 gdt 基地址
    mov dword [GdtPtr + 2], eax

    ; 补充 IdtPtr
    xor eax, eax
    mov ax, ds
    shl eax, 4
    add eax, LABEL_IDT
    mov dword [IdtPtr + 2], eax

    ; 保存 IDTR
    sidt [_SavedIDTR]
    ; 保存中断屏蔽寄存器 IMREG 值
    in al, 21h
    mov [_SavedIMREG], al

    lgdt [GdtPtr]
    lidt [IdtPtr]

    ; 进入保护模式
    cli ; 关中断
    in al, 92h
    or al, 02h
    out 92h, al ; 打开 A20
    mov eax, cr0
    or eax, 1
    mov cr0, eax ; 修改 PE 位
    jmp dword SelectorCode32:0

LABEL_BACK: ; 从保护模式回到这里
    
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, [_RealSP] ; 重置

    lidt [_SavedIDTR]

    mov al, _SavedIMREG
    out 21h, al ; 恢复 IMREG

    in al, 92h
    and al, 0FDh
    out 92h, al ; 关闭 A20
    ; sti ; 开中断

    ;DebugBreak

    mov ax, 4c00h
    int 21h ; 润
; --------------- end of .start ------------------

[SECTION .s32]
[BITS 32]
LABEL_CODE32:

    ; 重置段寄存器等
    mov ax, SelectorData
    mov ds, ax
    mov es, ax ; 数据段
    mov ax, SelectorVideo
    mov gs, ax ; 视频段
    mov ax, SelectorStack
    mov ss, ax ; 堆栈段
    mov esp, TopOfStack
    
    ShowStr strPro
    call DispCRLF

    DebugBreak

    call Set8259A

    sti
    int 80h
    
    ; 由于时钟中断，字符会不停的跳动
    mov ecx, 10000000
.loop:
    nop
    loop .loop

    ; 把时钟中断屏蔽后，字符不再跳动
    mov al, 0FFh
    out 021h, al
    call io_delay

    jmp $

    call Reset8259A

    ; 结束
    jmp SelectorCode16:0

    ; 定义了一些显示的函数
    %include "lib.inc"
    ; 定义了设置 8259A 的函数
    %include "s8259a.inc"
    ; 定义了一些中断
    %include "handler.inc"

SegCode32Len equ $ - $$

; ---------------- end of .s32 -------------------

[SECTION .s16]
ALIGN 32
[BITS 16]
LABEL_CODE16:
    
    mov ax, SelectorNormal
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax ; 刷新段寄存器

    mov eax, cr0
    and eax, 7FFFFFFEh ; 停止分页和保护模式
    mov cr0, eax

LABEL_GO_BACK:
    jmp 0:LABEL_BACK

SegCode16Len equ $ - $$