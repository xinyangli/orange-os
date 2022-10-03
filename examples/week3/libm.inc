; --------------------------------------------

; DispMemInfo 显示内存状况并计算内存大小
; TestL2P 传入 eax 线性地址 显示线性地址转化为物理地址的过程
; GetL2P 返回 eax 对应的物理地址在 ebx 中，错误信息放在 ecx 中

; --------------------------------------------

DispMemInfo: ; 无传入 计算并显示内存状况
    push esi
    push edi
    push eax
    push ecx

    push dword [ARDSNumber]
    call DispInt
    add esp, 4
    call DispCRLF

    ShowStr strListHead

    mov esi, MemChkBuf
    mov ecx, [ARDSNumber] ; 默认对齐 ecx，也就是 32 位，因此为了方便前面将 ARDSNumber 设为 32 位
    .for_ARDS: ; 第一层循环，遍历 ARDS
        ; 求内存大小
        mov eax, [esi + 16]
        cmp eax, 1
        jne .disp ; 不可用 跳到显示
        mov eax, [esi]
        add eax, [esi + 8]
        cmp eax, [MemSize]
        jb .disp ; 不足以更新大小
        mov [MemSize], eax
        ; 显示信息
        .disp:
        push ecx
        mov ecx, 5
        .for_INT:
            ; 显示这个属性
            push dword [esi]
            call DispInt
            add esp, 4
            ; 显示 "h "
            ShowStr strh32
            add esi, 4
            loop .for_INT
        call DispCRLF
        pop ecx
        loop .for_ARDS

    pop ecx
    pop eax
    pop edi
    pop esi
    ret

; ------------------------------------------------------

TestL2P: ; 传入 eax 线性地址 显示线性地址转化为物理地址的过程
    push ds
    push es
    push eax
    push ebx
    push ecx
    push edx

    ; 显示线性地址
    ShowStr strLr
    push eax
    call DispInt
    pop eax
    ShowStr strh32
    call DispCRLF

    mov ecx, eax ; 暂存 eax

    ; 为保险起见 模拟前 我们先关闭分页
    ; 其实也可以不关，因为我们的分页是线性等于物理的分页
    mov eax, cr0
    and eax, 7FFFFFFFh
    mov cr0, eax

    mov eax, ecx

    ; 获取页目录项
    ShowStr strPTr
    mov ebx, cr3
    shr eax, 22
    and eax, 03FFh
    shl eax, 2 ; 每个页目录项占 4 个字节
    push ds
    mov edx, SelectorFlatRW
    mov ds, edx
    mov ebx, [ebx + eax]
    pop ds
    push ebx
    call DispInt
    pop ebx
    ShowStr strh32
    test ebx, PG_P
    jz .error

    and ebx, 0FFFFF000h
    ; 获取页地址
    ShowStr strPAGEr
    mov eax, ecx
    shr eax, 12
    and eax, 03FFh
    shl eax, 2
    push ds
    mov edx, SelectorFlatRW
    mov ds, edx
    mov ebx, [ebx + eax]
    pop ds
    push ebx
    call DispInt
    pop ebx
    ShowStr strh32
    test ebx, PG_P
    jz .error

    ; 求出物理地址
    ShowStr strPhyAdr
    and ebx, 0FFFFF000h
    and ecx, 0FFFh
    add ebx, ecx
    push ebx
    call DispInt
    add esp, 4
    ShowStr strh32
    call DispCRLF
    jmp .exit

.error:
    call DispCRLF
    ShowStr strErrAdr

.exit:
    ;call DispCRLF
    mov eax, cr0
    or eax, 80000000h
    mov cr0, eax ; 恢复分页

    pop edx
    pop ecx
    pop ebx
    pop eax
    pop es
    pop ds
    ret

; -------------------------------------------

GetL2P: 
    ; 返回 eax 对应的线性地址在 ebx 中 
    ; 错误信号存放在 ecx
    push ds
    push es
    push eax
    push edx

    mov ecx, eax ; 暂存 eax

    ; 为保险起见 模拟前 我们先关闭分页
    ; 其实也可以不关，因为我们的分页是线性等于物理的分页
    mov eax, cr0
    and eax, 7FFFFFFFh
    mov cr0, eax

    mov eax, ecx

    mov dx, SelectorFlatRW
    mov ds, dx

    ; 获取页目录项
    mov ebx, cr3
    shr eax, 22
    and eax, 03FFh
    shl eax, 2 ; 每个页目录项占 4 个字节
    mov ebx, [ebx + eax]
    test ebx, PG_P
    jz .error1

    and ebx, 0FFFFF000h
    ; 获取页地址
    mov eax, ecx
    shr eax, 12
    and eax, 03FFh
    shl eax, 2
    mov ebx, [ebx + eax]
    test ebx, PG_P
    jz .error2

    ; 求出物理地址
    and ebx, 0FFFFF000h
    and ecx, 0FFFh
    add ebx, ecx
    jmp .exit

.error1:
    mov ecx, 1 ; 页目录缺失
    jmp .exit
.error2:
    mov ecx, 2 ; 页表缺失

.exit:
    ;call DispCRLF
    mov eax, cr0
    or eax, 80000000h
    mov cr0, eax ; 恢复分页

    pop edx
    pop eax
    pop es
    pop ds
    ret

