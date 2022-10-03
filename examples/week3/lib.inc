; ----------------------------------------

; DispStr 将栈顶的字符串显示在显存的 [ds:PtDisp] 处 
; DispCRLF 将下一个显示的位置换行并回到行首
; DispAL 以 16 进制显示 al
; DispInt 以 16 进制显示栈顶的 32 位整数

; ----------------------------------------
; 把栈顶的字符串显示在 gs:[PtDisp] 处，顺便更新 PtDisp 的值
; 调用前需要塞入字符串的地址
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

; --------------------------------------------------

; 换行
DispCRLF:
    push strCRLF 
    call DispStr
    add esp, 4
    ret

; --------------------------------------------------

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

.exit:
    mov [PtDisp], edi
    pop ecx
    pop edx
    pop eax
    pop edi
    ret

; --------------------------------------------------

DispInt: ; 传入栈中数
    push eax
    push edi
    push ecx

    mov cl, 24
.for_hex:
    mov eax, [esp + 16] ; eip eax edi ecx data
    shr eax, cl
    call DispAL
    test cl, cl
    jz .exit
    sub cl, 8
    jmp .for_hex
    
.exit:
    pop ecx
    pop edi
    pop eax
    ret