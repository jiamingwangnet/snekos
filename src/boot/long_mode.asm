global long_mode_start
global load_idt

extern kernel_main
extern gdt64.data
extern stack_top
extern gdt64.pointer

section .text
bits 64
long_mode_start:
    ; update selectors
    mov ax, gdt64.data
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rax, higher_half
    jmp higher_half

higher_half:
    mov rsp, stack_top
    lgdt [gdt64.pointer]
    
    call kernel_main

    hlt

load_idt:
    lidt [edi] ; the first param gets passed into edi
    ret