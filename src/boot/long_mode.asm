global long_mode_start
global load_idt

extern kernel_main
extern gdt64.data
extern pointer

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

    call kernel_main

    hlt

load_idt:
    mov eax, pointer
    lidt [eax]
    ret