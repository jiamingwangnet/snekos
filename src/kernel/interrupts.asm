extern isr_handler
extern irq_handler

bits 64
section .text
align 4

%macro save_registers 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro restore_registers 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    push 0 ; error code default to 0
    push %1 ; interrupt number
    save_registers
    mov rdi, rsp ; rdi and edi same thing. pass number as param
    cld ; clear direction flag

    call isr_handler

    restore_registers

    add rsp, 16 ; discard error code and interrupt num
    iretq
%endmacro

%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    ; stack already contains interrupt number
    push %1 ; interrupt number
    save_registers
    mov rdi, rsp ; rdi and edi same thing. pass number as param
    cld ; clear direction flag

    call isr_handler

    restore_registers
    add rsp, 16 ; discard error code and interrupt num
    iretq
%endmacro

%macro IRQ 2
  global irq%1
  irq%1:
    push 0
    push %2
    save_registers
    mov rdi, rsp
    cld 

    call irq_handler

    restore_registers
    add rsp, 16
    iretq
%endmacro

ISR_NOERRCODE  0 ; division by 0
ISR_NOERRCODE  1 ; single step interrupt
ISR_NOERRCODE  2 ; NMI
ISR_NOERRCODE  3 ; breakpoint
ISR_NOERRCODE  4 ; overflow
ISR_NOERRCODE  5 ; bound range exceeded
ISR_NOERRCODE  6 ; invalid opcode
ISR_NOERRCODE  7 ; coprocessor not available
ISR_ERRCODE  8   ; double fault
ISR_NOERRCODE  9 ; coprocessor segment overrun
ISR_ERRCODE 10   ; invalid task state segment
ISR_ERRCODE 11   ; segment not present
ISR_ERRCODE 12   ; stack segment fault
ISR_ERRCODE 13   ; general protection fault
ISR_ERRCODE 14   ; page fault
ISR_NOERRCODE 15 ; reserved
ISR_NOERRCODE 16 ; x87 floating point exception
ISR_NOERRCODE 17 ; alignment check
ISR_NOERRCODE 18 ; machine check
ISR_NOERRCODE 19 ; SIMD floating-point exception
ISR_NOERRCODE 20 ; virtualization exception
ISR_NOERRCODE 21 ; control protection exception (only available with CET)
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

IRQ  0, 32 ; programmable interrupt timer interrupt
IRQ  1, 33 ; keyboard interrupt
IRQ  2, 34 ; cascade (never raised, used by the PIC internally)
IRQ  3, 35 ; COM2
IRQ  4, 36 ; COM1
IRQ  5, 37 ; LPT2
IRQ  6, 38 ; floppy disk
IRQ  7, 39 ; LPT1
IRQ  8, 40 ; CMOS realtime clock
IRQ  9, 41 ; free for peripherals
IRQ 10, 42 ; free for peripherals
IRQ 11, 43 ; free for peripherals
IRQ 12, 44 ; PS2 mouse
IRQ 13, 45 ; FPU
IRQ 14, 46 ; primary ata hard disk
IRQ 15, 47 ; secondary ata hard disk

