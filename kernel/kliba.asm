global DispStr
global DispColStr
global gain_gdt
global apply_gdt
global apply_idt
global apply_tss

[SECTION .text]

extern PtDisp

; ================================================
; 显示字符串
; void DispStr(char *s)

%if 0
DispStr:

    push ebp
    mov ebp, esp ; 此时 ebp 前面有 ebp、eip、str
    push esi
    push edi 
    push eax ; 用于存放待写入的数据
    push ebx ; 用于计算显存偏移

    mov edi, [PtDisp] ; 这里取的是 ds
    mov esi, [ebp + 8] ; ebp 默认取的是 ss
    mov ah, 0Fh ; 黑底白字
    cld ; 方向标志位 DF 置 0
.loop:
    lodsb ; 从 ds:si 取一个字节放入 al
    test al, al
    jz .exit
    cmp al, 0Ah ; 10 是 '\n'
    jnz .normal
.return: ; 处理回车
    push eax ; 除法需要使用 eax
    mov eax, edi
    mov bl, 160
    div bl ; 除以 160，商是当前行号，在 al 中
    and eax, 0FFh
    inc eax
    mov bl, 160
    mul bl ; 根据行号算出新行的偏移
    mov edi, eax 
    pop eax ; 算出来的交给 edi 后还原
    jmp .loop
.normal: ; 处理一般字符
    mov [gs:edi], ax
    add edi, 2
    jmp .loop

.exit:
    mov [PtDisp], edi ; 更新这个指针的值
    pop ebx
    pop eax
    pop edi
    pop esi
    pop ebp
    ret
%endif

; ================================================
; 显示指定颜色的字符串
; void DispColStr(char *s, u8 col)

%if 0
DispColStr:

    push ebp
    mov ebp, esp ; 此时 ebp 前面有 ebp, eip, col, str
    push esi
    push edi 
    push eax ; 用于存放待写入的数据
    push ebx ; 用于计算显存偏移

    mov edi, [PtDisp] ; 这里取的是 ds
    mov esi, [ebp + 8] ; ebp 默认取的是 ss
    mov ah, [ebp + 12]
    cld ; 方向标志位 DF 置 0
.loop:
    lodsb ; 从 ds:si 取一个字节放入 al
    test al, al
    jz .exit
    cmp al, 0Ah ; 10 是 '\n'
    jnz .normal
.return: ; 处理回车
    push eax ; 除法需要使用 eax
    mov eax, edi
    mov bl, 160
    div bl ; 除以 160，商是当前行号，在 al 中
    and eax, 0FFh
    inc eax
    mov bl, 160
    mul bl ; 根据行号算出新行的偏移
    mov edi, eax 
    pop eax ; 算出来的交给 edi 后还原
    jmp .loop
.normal: ; 处理一般字符
    mov [gs:edi], ax
    add edi, 2
    jmp .loop

.exit:
    mov [PtDisp], edi ; 更新这个指针的值
    pop ebx
    pop eax
    pop edi
    pop esi
    pop ebp
    ret
%endif

; ================================================

; 内核的代码段选择子
; 每个描述符 64 位 8 个字节
; 根据 loader.asm 这个选择子对应第一个描述符
; TODO: Make cs portable
SELECTOR_KERNEL_CS equ 8

extern gdt_ptr
extern idt_ptr

; 将当前 gdt 的信息加载到 gdt_ptr
; void gain_gdt()
gain_gdt:
    sgdt [gdt_ptr]
    ret

; 将 gdt_ptr 加载为 gdt
apply_gdt:
    lgdt [gdt_ptr]
    ; 刷新 cs
    jmp SELECTOR_KERNEL_CS:.exit
.exit:
    ret

; 将 idt_ptr 加载为 idt 并开启中断
apply_idt:
    lidt [idt_ptr]
    sti
    ret

apply_tss:
    xor eax, eax
    mov ax, [esp + 4]
    ltr ax
    ret