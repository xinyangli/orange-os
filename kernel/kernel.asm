extern kernel_start
extern PtDisp

extern	cstart
extern	kernel_main
extern	exception_handler
extern	spurious_irq
extern	disp_str
extern	delay

; 导入全局变量
extern	gdt_ptr
extern	idt_ptr
extern	p_proc_ready
extern	tss
extern	k_reenter

global StackTop, _start, restart

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

[SECTION .bss]
    StackSpace resb 2*1024
    StackTop:

[SECTION .text]
_start:
    mov esp, StackTop
    call kernel_start
    ; 调用中断
    int 80h
    ; 进入死循环
    jmp $

restart:
    mov esp, [p_proc_ready]
    lldt [esp + P_LDT_SEL] 
    lea eax, [esp + P_STACKTOP]
    mov dword [tss + TSS3_S_SP0], eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 4
    iretd