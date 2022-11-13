global long_mode_start
extern kernel_main
extern gdt64.data
extern page_table_l2
global framebuffer
global map_framebuffer

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
    ; mov dword [page_table_l2], 0xfd000000 | 0b10000011 ; maps 0xfd000000 to 0x0
    ; invlpg [0] ; this fucks everything up

    call kernel_main

    hlt

map_framebuffer:
    mov eax, [framebuffer]
    or eax, 0b10000011
    ; adding 8 moves add 2MiB to the mapped address
    ; example: page_table_l2 + 8 maps to 0x200000
    ;          page_table_l2 + 16 maps to 0x400000
    ;          page_table_l2 + 8 * 488 = 0x3D000000
    mov dword [page_table_l2 + 8 * 488], eax 

    ; map second half of framebuffer for full image
    add eax, 0x200000
    mov dword [page_table_l2 + 8 * 489], eax 
    ret

section .bss
framebuffer resb 4