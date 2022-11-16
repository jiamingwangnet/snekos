global long_mode_start
extern kernel_main
extern gdt64.data
extern page_table_l2
global screen_size
global load_idt
extern pointer
extern serial_com1
global init_pic

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

init_pic:
    mov dx, 0x20
    mov ax, 0x11
    out dx, ax

    mov dx, 0xa0
    out dx, ax

    mov dx, 0x21
    mov ax, 0x20
    out dx, ax

    mov dx, 0xa1
    mov ax, 0x70
    out dx, ax

    mov dx, 0x21
    mov ax, 0x04
    out dx, ax

    mov dx, 0xa1
    mov ax, 0x02
    out dx, ax

    mov dx, 0x21
    mov ax, 0x01
    out dx, ax

    mov dx, 0xa1
    out dx, ax

    mov dx, 0x21
    mov ax, 0x0
    out dx, ax

    mov dx, 0xa1
    out dx, ax

    ret
