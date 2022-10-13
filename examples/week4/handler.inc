CharPos equ ((80 * 4 + 10) * 2)

_UserIntHandler:
    push eax
    mov ah, 0Ch
    mov al, `A`
    mov [gs:CharPos], ax
    pop eax
    iretd
UserIntHandler equ _UserIntHandler - $$

_ClockHandler:
    push eax
    cmp byte [gs:CharPos], `Z`
    je .2
    inc byte [gs:CharPos]
    jmp .exit
.2:
    mov byte [gs:CharPos], `A`
.exit:
    mov al, 20h
    out 20h, al
    pop eax
    iretd
ClockHandler equ _ClockHandler - $$

_KeyBoardHandler:
    push eax
    
    in al, 60h
    call DispAL
    ShowStr str32

.exit:
    mov al, 20h
    out 20h, al    
    pop eax
    iretd
KeyBoardHandler equ _KeyBoardHandler - $$