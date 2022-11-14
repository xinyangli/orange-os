[SECTION .text]

extern PtDisp
extern	p_proc_ready
extern	tss
extern	k_reenter
extern StackTop
extern old_esp
extern check_testA

; TODO: 干掉这两个变量
P_STACKTOP equ 72
TSS3_S_SP0	equ	4

%if 0
save:
        ; We point esp0 to top of STACK_FRAME in last clock_handler
        ; So, stack is switched to the top of STACK_FRAME when interrupt happens
        pushad
        push ds
        push es
        push fs
        push gs
        mov dx, ss
        mov ds, dx
        mov es, dx

        ; move esp0 to esi
        mov esi, esp

        inc dword [k_reenter]
        cmp dword [k_reenter], 0
        jne .1

        mov esp, StackTop
        push restart

        ; 既然这里要保存retaddr才能跳转回去
        ; 那为什么不直接inline?
.1:
        push restart_reenter

restart:
	mov	esp, [p_proc_ready]	
	lldt	[esp + P_LDT_SEL]
	lea	eax, [esp + P_STACKTOP]
	mov	dword [tss + TSS3_S_SP0], eax
restart_reenter:
	dec	dword [k_reenter]
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad
	add	esp, 4
	iretd

; Wrapper function for hardware interrupt
%macro    hwint_master 1
    call save

    ; Mask current interrupt
    in al, INT_M_CTLMASK
    or al, (1 << %1)
    out INT_M_CTLMASK, al

    ; Acknoledge interrupt
    mov al, EOI
    out INT_M_CTL, al

    sti
    push %1

    call [irq_table + 4 * %1] ; Real interrupt handler

    pop ecx
    cli
    ; Unmask current intterupt
    in al, INT_M_CTLMASK
    and al, ~(1 << %1)
    out INT_M_CTLMASK, al
    ret
%endmacro
%endif

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
    jne .reenter

    mov [old_esp], esp
    mov esp, StackTop

    sti

    mov byte [gs:CharPos + 1], 0Ch
    cmp byte [gs:CharPos], `Z`
    je .2
    inc byte [gs:CharPos]
    jmp .exit
.2:
    mov byte [gs:CharPos], `A`

.exit:
    ; 检查 testA 的代码完整性
    call check_testA

    cli
    mov esp, [old_esp] 

    lea eax, [esp + P_STACKTOP] 
    mov dword [tss + TSS3_S_SP0], eax

.reenter:
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
