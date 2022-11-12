global long_mode_start
extern kernel_main
extern gdt64.data
extern page_table_l2

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

    ; mov eax, page_table_l2
    mov dword [page_table_l2], 0xfd000000 | 0b10000011 ; maps 0xfd000000 to 0x0
    ; invlpg [eax] ; this fucks everything up

    ; mov byte [0], 0xff ; works now
    mov ecx, 0
.fill_white:
    mov dword [ecx * 4], 0xffffffff ; make this work
    inc ecx
    cmp ecx, 1280 * 100 ; 100 lines
    jne .fill_white

    call kernel_main

    hlt