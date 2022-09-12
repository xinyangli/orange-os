        org 07c00h
        mov ax, cs
        mov ds, ax
        mov es, ax
        call DispStr
        jmp $
DispStr:
        mov ax, BootMessage
        mov bp, ax
        mov cx, 19                     ; Length of string (CX)
        mov ax, 01301h                 ; BIOS Function code write string (AH), write mode (AL)
        mov bx, 000ch                  ; Page number is 0 (BH), black background and red character (BL)
        mov dl, 0
        int 10h                        ; BIOS interrupt
        ret
BootMessage:
        db "Hello, Xinyang Li!"
        times 510-($-$$) db 0          ; Fill the remaining MBR with 0
        dw 0xaa55
