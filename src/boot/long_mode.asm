global long_mode_start
extern kernel_main
extern gdt64.data
extern page_table_l2
global screen_size

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

    ; trigger division by 0 exception
    ; mov eax, 0
    ; div eax

    call kernel_main

    hlt