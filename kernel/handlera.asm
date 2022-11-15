
PIC1 equ 0x20
PIC2 equ 0xA0 ; IO base address for slave PIC
PIC1_COMMAND equ PIC1
PIC1_DATA equ (PIC1 + 1)
PIC2_COMMAND equ PIC2
PIC2_DATA equ (PIC2 + 1)
PIC_EOI equ 0x20

IDT_SIZE equ 256

; External Variables
extern k_reenter
extern StackTop        ; Kernel Stack
; External Functions
extern restart
extern restart_reenter

%macro regsave 0
    pushad
    push ds
    push es
    push fs
    push gs
    mov dx, ss
    mov ds, dx
    mov es, dx

    inc dword [k_reenter]
    cmp dword [k_reenter], 0
    jne %%.reenter
    mov esp, StackTop
    %%.reenter:
%endmacro

; arguments:
; %1: handler function name, e.g irqclock
; %2: IRQ number
%macro irq_wrapper 1-2 128

extern %1
%assign i_idt 0x20+%2
global __handler_pic0_%1
global __handler_%[i_idt]
%assign __handler_%[i_idt]_defined 1
  __handler_pic0_%1:
  __handler_%[i_idt]:
%undef i_idt
    regsave
%if %0==2
    ; Mask current IRQ type
    in al, PIC1_DATA
    or al, (1 << %2)
    out PIC1_DATA, al
%endif
    ; Acknoledge current IRQ
    mov al, PIC_EOI
%if %0==2
    out PIC1_COMMAND, al
%else 
    out PIC2_COMMAND, al
%endif
    ; Accept other IRQ
    sti

    call %1

    cli
    ; Unmask current IRQ type
%if %0==2
    in al, PIC1_DATA
    and al, ~(1 << %2)
    out PIC1_DATA, al 
%endif
    
    dec dword [k_reenter]
    cmp dword [k_reenter], -1
    jne restart_reenter
    jmp restart
%endmacro

; arguments:
; %1: system call name, e.g sys_get_ticks
; %2: syscall number
%macro syscall_wrapper 2
extern %1
%endmacro

[SECTION .text]

; ==== IRQ Handlers ====
irq_wrapper irqclock,0
irq_wrapper irqreport

[SECTION .data]
global handlers
handlers:
%assign i_idt 0
%rep IDT_SIZE
  %ifdef __handler_%[i_idt]_defined
    dd __handler_%[i_idt]
  %else
    dd __handler_pic0_irqreport
  %endif
  %assign i_idt i_idt+1
%endrep  
%defstr PATH i_idt
%undef i_idt
  