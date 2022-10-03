; -------------------------------------------

; CheckVaild 检查第 eax 个页是否空闲 返回 ebx=1 表示未被分配
; SetVaild 设置第 eax 个页为忙碌
; ResetVaild 设置第 eax 个页为空闲

; -------------------------------------------

CheckVaild: ; 检查第 eax 个页是否空闲 返回在 ebx 中
    push ds
    push eax
    push ecx

    mov ebx, SelectorData
    mov ds, ebx
    mov ecx, eax ; 暂存地址
    shr eax, 3 ; 位于 bitmap 的哪个字节
    test eax, 0FFFFFFC0h
    jz .inmap
    jmp .outmap
.inmap:
    mov ebx, BitMap
    mov bl, byte [ebx + eax] ; map 中的该字节
    mov al, cl
    and cl, 07h
    mov eax, 1
    shl eax, cl ; 这个页对应的比特位
    test bl, al ; ebx & eax 测试这一位是否为 1
    jz .ok ; 为 0 表明未被分配
.outmap:
    mov ebx, 0
    jmp .exit
.ok:
    mov ebx, 1
    jmp .exit
.exit:
    pop ecx
    pop eax
    pop ds
    ret

; -------------------------------------------

SetVaild: ; 设置第 eax 个页为忙碌 如果出错会放在 ebx
    push ds
    push eax
    push ecx
    push edx

    mov ebx, SelectorData
    mov ds, ebx
    mov ecx, eax ; 暂存地址
    shr eax, 3 ; 位于 bitmap 的哪个字节
    test eax, 0FFFFFFC0h
    jz .inmap
    jmp .outmap
.inmap:
    mov ebx, BitMap
    mov edx, ecx
    and ecx, 07h
    mov edx, 1
    shl edx, cl ; 这个页对应的比特位
    or byte [ebx + eax], dl
    jmp .exit
.outmap:
    mov ebx, 0
.exit:
    pop edx
    pop ecx
    pop eax
    pop ds
    ret

; ------------------------------------------

ResetVaild: ; 设置第 eax 个页为空闲
    push ds
    push eax
    push ecx
    push edx

    mov ebx, SelectorData
    mov ds, ebx
    mov ecx, eax ; 暂存地址
    shr eax, 3 ; 位于 bitmap 的哪个字节
    test eax, 0FFFFFFC0h
    jz .inmap
    jmp .outmap
.inmap:
    mov ebx, BitMap
    mov edx, ecx
    and ecx, 07h
    mov edx, 1
    shl edx, cl ; 这个页对应的比特位
    mov ecx, 0FFFFFFFFh
    sub ecx, edx
    and byte [ebx + eax], cl
    jmp .exit
.outmap:
    mov ebx, 0
.exit:
    pop edx
    pop ecx
    pop eax
    pop ds
    ret