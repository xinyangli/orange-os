; -----------------------------------

Set8259A:
    push eax

    mov al, 011h ; ICW1
    out 020h, al ; 主8259
    call io_delay
    out 0A0h, al ; 从8259
    call io_delay

    mov al, 020h ; ICW2, IRQ0--0x20
    out 021h, al ; 主 8259
    call io_delay
    mov al, 028h ; IRQ8--0x28
    out 0A1h, al
    call io_delay

    mov al, 004h ; ICW3
    out 021h, al
    call io_delay
    mov al, 002h
    out 0A1h, al
    call io_delay

    mov al, 001h ; ICW4
    out 021h, al
    call io_delay
    out 0A1h, al
    call io_delay

    ; OCW1
    ; 开启主 8259 的 IRQ0，20h 号中断（定时器中断）
    mov al, 0FEh 
    out 021h, al
    call io_delay
    ; 屏蔽从 8259 的所有中断
    mov al, 0FFh
    out 0A1h, al 
    call io_delay

    pop eax
    ret

; -------------------------------------

Reset8259A:
    push eax

    mov eax, SelectorData
    mov fs, ax

    ; BIOS 初始化时，IRQ0~7 -- 08h~0Fh
    ; 此处要恢复它

    mov al, 017h ; ICW1
    out 020h, al ; 主
    call io_delay

    mov al, 008h ; ICW2, IRQ0 对应 0x8
    out 021h, al
    call io_delay

    mov al, 001h ; ICW4
    out 021h, al
    call io_delay

    mov al, [fs:SavedIMREG]
    out 021h, al
    call io_delay

    pop eax
    ret

; -------------------------------------

io_delay:
    nop
    nop
    nop
    nop
    ret