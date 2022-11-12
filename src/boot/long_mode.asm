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
    ; ; add eax, 0x100000
    mov dword [page_table_l2], 0xfd000000 | 0b10000011 
    ; ; maps 0xfd000000 to 0x0
    ; invlpg [eax]

    ; ; mov eax, [page_table_l2]

    ; ; mov [0x100000], eax
    mov byte [0], 0xff
;     mov ecx, 0
; .fill_white:
;     mov byte [ecx], 0xff ; make this work
;     inc ecx
;     cmp ecx, 1280 * 800 * 4
;     jne .fill_white

    call kernel_main

    hlt