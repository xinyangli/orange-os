[SECTION .text]

extern PtDisp
extern	p_proc_ready
extern	tss
extern	k_reenter
extern StackTop

; struct PROCESS
P_STACKBASE	equ	0
GSREG		equ	P_STACKBASE
FSREG		equ	GSREG		+ 4
ESREG		equ	FSREG		+ 4
DSREG		equ	ESREG		+ 4
EDIREG		equ	DSREG		+ 4
ESIREG		equ	EDIREG		+ 4
EBPREG		equ	ESIREG		+ 4
KERNELESPREG	equ	EBPREG		+ 4
EBXREG		equ	KERNELESPREG	+ 4
EDXREG		equ	EBXREG		+ 4
ECXREG		equ	EDXREG		+ 4
EAXREG		equ	ECXREG		+ 4
RETADR		equ	EAXREG		+ 4
EIPREG		equ	RETADR		+ 4
CSREG		equ	EIPREG		+ 4
EFLAGSREG	equ	CSREG		+ 4
ESPREG		equ	EFLAGSREG	+ 4
SSREG		equ	ESPREG		+ 4
P_STACKTOP	equ	SSREG		+ 4
P_LDT_SEL	equ	P_STACKTOP
P_LDT		equ	P_LDT_SEL	+ 4

TSS3_S_SP0	equ	4

CharPos equ ((80 * 4 + 10) * 2)

; ===============================================
; 默认中断 在指定位置显示 `A`
global empty_handler
empty_handler:
    push eax
    mov ah, 0Ch
    mov al, `A`
    mov [gs:CharPos], ax
    pop eax
    iretd

; ===============================================
; 时钟中断 效果是显示一个变化的字符
global clock_handler
clock_handler:
    sub esp, 4
    pushad
    push ds
    push es
    push fs
    push gs
    mov dx, ss
    mov ds, dx
    mov es, dx

    mov al, 20h
    out 20h, al

    inc dword [k_reenter]
    cmp dword [k_reenter], 0
    jne .exit

    mov esp, StackTop

    sti

    cmp byte [gs:CharPos], `Z`
    je .2
    inc byte [gs:CharPos]
    jmp .exit
.2:
    mov byte [gs:CharPos], `A`
 
    cli
    mov esp, [p_proc_ready] 

    lea eax, [esp + P_STACKTOP] 
    mov dword [tss + TSS3_S_SP0], eax

.exit:
    dec dword [k_reenter]    
    
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 4
     
    iretd

; ===============================================
; 键盘中断 效果是显示扫描码
global KeyBoardHandler
KeyBoardHandler:
    push eax
    in al, 60h
    call DispAL
.exit:
    mov al, 20h
    out 20h, al    
    pop eax
    iretd

; ===============================================
; 私有函数 用于显示
DispAL:
    push edi
    push eax
    push edx
    push ecx ; 还是需要 ecx，比较自然

    mov edi, [PtDisp]
    mov ecx, 2
    mov ah, 0Fh
    mov dl, al ; 保存下来 
    shr al, 4
.chk:
    and al, 0Fh
    cmp al, 9
    ja .1
    add al, `0`
    jmp .2
.1:
    sub al, 0Ah
    add al, `A`
.2:
    mov [gs:edi], ax
    add edi, 2
    mov al, dl
    loop .chk

    mov al, `h`
    mov [gs:edi], ax
    add edi, 2
    
    mov al, 32
    mov [gs:edi], ax
    add edi, 2

.exit:
    mov [PtDisp], edi
    pop ecx
    pop edx
    pop eax
    pop edi
    ret
