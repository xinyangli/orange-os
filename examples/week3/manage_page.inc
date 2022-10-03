; ------------------------------------------

; SetupPaging 开启分页
; alloc_pages 给 eax 开始连续 ecx 个字节的虚拟内存分配物理地址
; link_page 把第 ebx 个物理页分配给 eax 这个虚拟地址
; free_pages 释放虚拟地址 eax 对应的 ecx 个字节
; stop_page 把虚拟地址 eax 对应的页释放出来
; alloc_PDE 如果第 eax 个 PDE 不存在，就初始化它

; -------------------------------------------

SetupPaging: ; 无传入 开启分页
    push es
    mov eax, [MemSize]
    xor edx, edx ; 余数
    mov ebx, 400000h ; 4M
    div ebx
    mov ecx, eax
    test edx, edx
    jz .no_remainder
    inc ecx
.no_remainder:
    mov [PDENumber], ecx ; 保存页表数

    ; 页目录初始化
    ; mov ecx, 1024
    mov ax, SelectorFlatRW
    mov es, ax
    mov edi, PageDirBase
    xor eax, eax
    mov eax, PageTblBase | PG_P | PG_USU | PG_RWW
.initPDE:
    stosd
    add eax, 4096
    loop .initPDE
    
    mov [LastPDE], eax

    ; 页表初始化
    mov eax, [PDENumber]
    mov ebx, 1024
    mul ebx
    mov ecx, eax ; 求出页表个数
    mov edi, PageTblBase
    xor eax, eax
    mov eax, PG_P  | PG_USU | PG_RWW
.initPTE:
    stosd
    add eax, 4096
    loop .initPTE

    ; 开启分页
    mov eax, PageDirBase
    mov cr3, eax
    mov eax, cr0
    or eax, 80000000h
    mov cr0, eax
    jmp short .exit

.exit:
    nop
    pop es
    ret

; --------------------------------------------

alloc_pages: 
    ; 分配 ecx 字节内存给 eax 地址
    ; 错误信息保存在 ebx 处

    DebugBreak

    mov ebx, cr0
    and ebx, 7FFFFFFFh
    mov cr0, ebx ; 先关分页

    ; 我们假定不需要考虑 eax ~ ebx 原本的映射
    mov ebx, eax
    add ebx, ecx 
    sub ebx, 1
    and eax, 0FFFFF000h ; 无视页内偏移
    and ebx, 0FFFFF000h ; eax ~ ebx
    mov ecx, ebx
    sub ecx, eax
    shr ecx, 12
    inc ecx ; 求出需要的页的数量
    
    push eax

    mov edx, ecx
    mov ebx, 0
    ; 寻找连续的 ecx 个空闲页
    ; 可支配的页共有 32 * 16 个
.for_map:
    push ebx
    mov ecx, edx
    .for_pages:
        push ebx
        mov eax, ebx
        call CheckVaild
        test ebx, ebx ; ebx 为 1 表示 Vaild
        jz .fail
        pop ebx
        inc ebx
        loop .for_pages
    jmp .suc ; 走到这里说明 ebx 开始的页是够的
    .fail: ; 有一个页不行 分配不出连续的
        pop ebx
        jmp .find_next
    .find_next: ; 从这个页开始不行，看下一个
        pop ebx
        inc ebx
        cmp ebx, 512
        jne .for_map
    jmp .no_mem; 已经没有页可找了
    .suc: 
        pop ebx
        jmp .suc_mem

.no_mem:
    DebugBreak
    pop eax
    mov ebx, 1 ; 表示错误
    jmp .exit

.suc_mem:

    DebugBreak
    pop eax
    mov ecx, edx
    ; 将 ebx 开始的连续 ecx 个页分配给 eax
    .alloc:
        call link_page
        inc ebx
        add eax, 4096
        loop .alloc
    mov ebx, 0 ; 表示成功
    jmp .exit

.exit:
    mov eax, cr0
    or eax, 80000000h
    mov cr0, eax ; 恢复分页
    ret

; ---------------------------------------------

link_page: ; 把 ebx 这个页分配给 eax
    push ebx
    push eax
    push ds
    push ecx
    push edx

    DebugBreak

    push eax
    push ebx
    mov eax, ebx
    call SetVaild
    pop ebx
    pop eax ; 更新 Bit Map

    mov ecx, SelectorFlatRW
    mov ds, ecx

    mov ecx, eax 
    mov edx, ebx; 暂存
    ; 先寻找 eax 对应的页目录
    shr eax, 22
    and eax, 03FFh
    call alloc_PDE ; 如果不存在 则创建
    shl eax, 2 ; 页目录的偏移
    mov ebx, cr3
    mov ebx, [ebx + eax] ; 找到页表
    and ebx, 0FFFFF000h ; 记得删掉后面的属性位
    mov eax, ecx
    shr eax, 12
    and eax, 03FFh 
    shl eax, 2 ; 求出页表中的偏移
    shl edx, 12 ; ebx 对应的物理页的物理地址
    or edx, PG_P | PG_USU | PG_RWW ; 填入页表中的值
    mov [ebx + eax], edx

.exit:
    pop edx
    pop ecx
    pop ds
    pop eax
    pop ebx
    ret

; ---------------------------------------------

alloc_PDE: ; 初始化 PDE （传入 eax 是序号）
    push ds
    push es
    push ebx
    push ecx
    push edx
    push edi
    push eax

    DebugBreak

    mov ebx, SelectorFlatRW
    mov ds, ebx
    mov ebx, SelectorData
    mov es, ebx

    mov edi, LastPDE
    mov edx, [es:edi] ; 准备把这个送给 PDE
    
    shl eax, 2 ; 每个 PDE 四个字节
    mov ebx, cr3
    mov ecx, [ebx + eax]
    test ecx, PG_P
    jz .ini ; 初始化 PDE
    jmp .exit ; 已经有了，不用再分配
.ini:
    mov [ebx + eax], edx
    add edx, 4096
    mov [es:edi], edx
.exit:
    pop eax
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop es
    pop ds
    ret

; ---------------------------------------------

free_pages: 
    ; 释放 eax 开始的 ecx 个字节
    ; 这里我们钦定它一定是个合法的调用
    mov ebx, cr0
    and ebx, 7FFFFFFFh
    mov cr0, ebx ; 先关分页

    mov ebx, eax
    add ebx, ecx 
    and eax, 0FFFFF000h ; 无视页内偏移
    and ebx, 0FFFFF000h ; eax ~ ebx
    mov ecx, ebx
    sub ecx, eax
    shr ecx, 12
    inc ecx

.release:
    call stop_page
    add eax, 4096
    loop .release

.exit:
    mov eax, cr0
    or eax, 80000000h
    mov cr0, eax ; 恢复分页
    ret

; ----------------------------------------------

stop_page: ; 把虚拟地址 eax 对应的页释放出来
    push ebx
    push eax
    push ds
    push ecx
    push edx

    push eax
    call ResetVaild
    pop eax ; 重置 bit map

    mov ecx, SelectorFlatRW
    mov ds, ecx

    mov ecx, eax 
    mov edx, ebx; 暂存
    ; 先寻找 eax 对应的页目录
    shr eax, 22
    and eax, 03FFh
    shl eax, 2 ; 页目录的偏移
    mov ebx, cr3
    mov ebx, [ebx + eax] ; 找到页表
    and ebx, 0FFFFF000h ; 记得删掉后面的属性位
    mov eax, ecx
    shr eax, 12
    and eax, 03FFh 
    shl eax, 2 ; 求出页表中的偏移
    mov dword [ebx + eax], 0 ; 删除映射

.exit:
    pop edx
    pop ecx
    pop ds
    pop eax
    pop ebx
    ret