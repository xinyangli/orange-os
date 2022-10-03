%include "pm.inc"
%include "utils.inc"

%define DEBUG

PageDirBase equ 200000h
PageTblBase equ 201000h

org 0100h
jmp LABEL_BEGIN


[SECTION .gdt]
; 描述符表
LABEL_GDT:
.EMPTY:
    Descriptor 0, 0, 0 ; 这里之所以要有一个空的，是因为 LDTR 默认为 0，指向一个空描述符
.NORMAL:
    Descriptor 0, 0ffffh, DA_DRW
.CODE16:
    ;Descriptor 0, SegCode16Len - 1, DA_C
    Descriptor 0, 0ffffh, DA_C 
    ; 这里之所以不能用那个段本身的界限，是因为这是个回到实模式的过渡段
    ; cs 和其它段寄存器一样，它都需要在回去之前刷新
    ; 其它段寄存器通过 NORMAL，cs 则通过进入这个 16 位代码段
    ; 这也是退出保护模式一定要经过一个界限和属性合适的 16 位段的原因
.CODE32:
    Descriptor 0, SegCode32Len - 1, DA_C + DA_32
.VIDEO:
    Descriptor 0b8000h, 0ffffh, DA_DRW
.DATA:
    Descriptor 0, SegDataLen - 1, DA_DRW
.STACK:
    Descriptor 0, TopOfStack, DA_DRWA + DA_32 
.FLAT_RW: ; 一个特别大的象征全局的段，方便页表的存取读写
    Descriptor 0, 0fffffh, DA_DRW + DA_LIMIT_4K ; 一个完整的页目录能指向的所有空间

; GDT 大小
GdtLen equ $ - LABEL_GDT
; GDT 界限 (16) 和 GDT 基地址 (32)
GdtPtr dw GdtLen - 1, 0, 0 
; --------------- end of .gdt ------------------


; 选择子 宏定义 并不是真正占据空间的数据 因此不认为它属于任何一段
SelectorNormal equ LABEL_GDT.NORMAL - LABEL_GDT
SelectorCode16 equ LABEL_GDT.CODE16 - LABEL_GDT
SelectorCode32 equ LABEL_GDT.CODE32 - LABEL_GDT
SelectorVideo equ LABEL_GDT.VIDEO - LABEL_GDT
SelectorData equ LABEL_GDT.DATA - LABEL_GDT
SelectorStack equ LABEL_GDT.STACK - LABEL_GDT
SelectorFlatRW equ LABEL_GDT.FLAT_RW - LABEL_GDT
; --------------- end of selector ------------------


[SECTION .data]
ALIGN 32
[BITS 32]
LABEL_DATA:

_strPro: db `Hi! Here is protect mode!\n`, 0 ; 使用反引号可以在 nasm 中使用反义字符
strPro equ _strPro - $$

_strListHead: db `BaseAddrL BaseAddrH  LengthL   LengthH    Type   \n`, 0
;_strListNode: db `12345678h 12345678h 12345678h 12345678h 12345678h\n`, 0
strListHead equ _strListHead - $$
;strListNode equ _strListNode - $$

_strPaging: db `Paging finished, yeah!\n`, 0
strPaging equ _strPaging - $$

_strLr: db `LinearAddr: `, 0
strLr equ _strLr - $$

_strPTr: db `PT_Addr: `, 0
strPTr equ _strPTr - $$ 

_strPAGEr: db `PAGE_Addr: `, 0
strPAGEr equ _strPAGEr - $$ 

_strErrAdr: db `Oh, it is a wrong address!\n`, 0
strErrAdr equ _strErrAdr - $$

_strPhyAdr: db `Physical: `, 0
strPhyAdr equ _strPhyAdr - $$

_strh32: db `h `, 0
strh32 equ _strh32 - $$

_strCRLF: db `\n`, 0 ; 回车换行
strCRLF equ _strCRLF - $$

_strDEBUG: db `DEBUG\n`, 0
strDEBUG equ _strDEBUG - $$

_MemChkBuf: times 240 db 0 ; 存放 ARDS 的缓冲区
MemChkBuf equ _MemChkBuf - $$

_ARDSNumber: dd 0 ; 读取的 ARDS 的数量，取 dword 是为了对齐
ARDSNumber equ _ARDSNumber - $$

_MemSize: dd 0
MemSize equ _MemSize - $$

_PDENumber: dd 0
PDENumber equ _PDENumber - $$

_RealSP: dw 0
RealSP equ _RealSP - $$

_PtDisp: dd (80 * 1 + 0) * 2 ; 默认从第 4 行第 0 列开始显示，每个字符两字节
PtDisp equ _PtDisp - $$

_BitMap: 
    ; 观察可以发现，我们实际使用的内存都在 0x00100000 内
    ; 不妨直接假定 1M 内的内存都是被分配的
    ; 1M ~ 2M 的内存是可分配的
    times 32 db 0FFh ; 32 * 8 * 4K = 1M
    times 32 db 00h
BitMap equ _BitMap - $$

_LastPDE: dd 0
LastPDE equ _LastPDE - $$

SegDataLen equ $ - $$
; --------------- end of .data ------------------


[SECTION .stack]
ALIGN 32
[BITS 32]
LABEL_STACK: times 1024 db 0
TopOfStack equ $ - $$ - 1 ; 用的基本是界限 所以直接定义一个界限值比较方便
; --------------- end of .stack ------------------

[SECTION .start]
[BITS 16]
LABEL_BEGIN:
    
    ; 初始化段寄存器
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0100h ; 被装载到 0100h，前面的空白内容可以用来当实模式栈
    
    mov [_RealSP], sp
    mov [LABEL_GO_BACK + 3], ax ; 设置返回时的段地址

    ; 获取内存数
    mov ebx, 0 ; 根据它的值可以确认 ARDS 是否读完
    mov ecx, 20 ; 请求、返回的 ARDS 大小，一般为 20 不变 
    mov edx, 0534D4150h ; "SWAP"
    ; eax 在每次调用后会变成 "SWAP" 用于校准，书上 76 页所说的 eax 不变是错的
    mov di, _MemChkBuf ; ARDS 数组的地址，es:di
    .loop:
        mov eax, 0E820h
        int 15h ; 调用 15h 中断
        jc .fail
        add di, 20
        inc dword [_ARDSNumber] ; ds:_ARDS_n 存放的是数组大小
        cmp ebx, 0 ; 为 0 代表已经读完了
        jne .loop
        jmp .ok
    .fail:
        jmp $ ; 调用失败
    .ok:
    
    ; 初始化描述符
    InitSeg LABEL_GDT.CODE16, LABEL_CODE16
    InitSeg LABEL_GDT.CODE32, LABEL_CODE32
    InitSeg LABEL_GDT.DATA, LABEL_DATA
    InitSeg LABEL_GDT.STACK, LABEL_STACK

    ; 加载 GDTR
    xor eax, eax
    mov ax, ds
    shl eax, 4
    add eax, LABEL_GDT ; 计算 gdt 基地址
    mov dword [GdtPtr + 2], eax
    lgdt [GdtPtr]

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

    in al, 92h
    and al, 0FDh
    out 92h, al ; 关闭 A20
    sti ; 开中断

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

    call DispMemInfo ; 顺便算出了内存大小
    call DispCRLF

    push dword [MemSize]
    call DispInt
    add esp, 4
    ShowStr strh32
    call DispCRLF ; 显示内存大小

    call SetupPaging

    ShowStr strPaging
    ;call DispCRLF

    ;mov eax, 000000100h
    ;call TestL2P

    mov eax, 012345678h
    call TestL2P

    mov eax, 012345000h
    mov ecx, 2000h
    call alloc_pages
    test ebx, ebx
    jz .d1
    ShowStr strDEBUG
.d1:
    mov eax, 012345123h
    call TestL2P

    mov eax, 012346123h
    call TestL2P

    mov eax, 012345000h
    mov ecx, 2000h
    call free_pages

    mov eax, 012345123h
    call TestL2P

    ; 结束
    jmp SelectorCode16:0

    ; 定义了一些显示的函数
    %include "lib.inc"
    ; 定义了页表内存相关的函数
    %include "libm.inc"
    %include "manage_page.inc"
    %include "bitmap.inc"

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