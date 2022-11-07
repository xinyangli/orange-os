global DispStr
global DispColStr
global out_byte
global memset
global memcpy
global in_byte
global gain_gdt
global apply_gdt
global apply_idt

[SECTION .text]

extern PtDisp

; ================================================
; 显示字符串
; void DispStr(char *s)


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

; ================================================
; 显示指定颜色的字符串
; void DispColStr(char *s, u8 col)


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


; ================================================
; 往端口传输数据
; void out_byte(u16 port, u8 data)

out_byte:
    push ebp
    mov ebp, esp
    mov dx, [ebp + 8]
    mov al, [ebp + 12]
    out dx, al
    nop
    nop
    pop ebp
    ret

; ================================================
; 从端口读入数据
; u8 in_byte(u16 port)


in_byte:
    push ebp
    mov ebp, esp
    mov dx, [ebp + 8]
    in al, dx
    nop
    nop
    ret

; ================================================
; 内存复制
; void* memcpy(void *dst, void *src, size_t siz)


memcpy:
    push ebp
    mov ebp, esp
    
    push edi
    push esi
    push ecx
    
    mov edi, [ebp + 8]
    mov esi, [ebp + 12]
    mov ecx, [ebp + 16]
.cp:
    cld
    lodsb
    stosb
    loop .cp

    mov eax, [ebp + 8]

    pop ecx
    pop esi
    pop edi
    pop ebp
    ret

; ================================================
; 内存覆盖
; void* memset(void *dst, u8 val, size_t siz)


memset:
    push ebp
    mov ebp, esp
    
    push edi
    push ecx
    
    mov edi, [ebp + 8]
    mov al, [ebp + 12]
    mov ecx, [ebp + 16]
.cp:
    stosb
    loop .cp

    mov eax, [ebp + 8]

    pop ecx
    pop edi
    pop ebp
    ret

; ================================================

; 内核的代码段选择子
; 每个描述符 64 位 8 个字节
; 根据 loader.asm 这个选择子对应第一个描述符
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
    pop eax
    ltr ax
    ret