global long_mode_start

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
    jmp rax

higher_half:
    mov rsp, stack_top
    lgdt [gdt64.pointer]

    call kernel_main

    hlt