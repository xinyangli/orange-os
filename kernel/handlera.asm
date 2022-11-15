
%assign PIC1 0x20
%assign PIC2 0xA0 ; IO base address for slave PIC
%assign PIC1_COMMAND PIC1
%assign PIC1_DATA (PIC1 + 1)
%assign PIC2_COMMAND PIC2
%assign PIC2_DATA (PIC2 + 1)
%assign PIC_EOI 0x20

%assign INT_VECTOR_SYSCALL 0x90

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

%macro restart_entry 0
    dec dword [k_reenter]
    cmp dword [k_reenter], -1
    jne restart_reenter
    jmp restart
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
    
    restart_entry
%endmacro


[SECTION .text]

__handler_syscall:
__handler_%[INT_VECTOR_SYSCALL]:
%assign __handler_%[INT_VECTOR_SYSCALL]_defined 1
; External variables
extern syscall_table
; ExternalFunctions
extern save_ret

    regsave
    sti

    call [syscall_table + 4 * eax]
    call save_ret

    cli
    restart_entry

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
%undef i_idt
  