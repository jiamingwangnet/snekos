global long_mode_start
extern kernel_main
extern gdt64.data
extern page_table_l2
global framebuffer
global map_framebuffer
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

    ; mov eax, page_table_l2
    ; mov dword [page_table_l2], 0xfd000000 | 0b10000011 ; maps 0xfd000000 to 0x0
    ; invlpg [0] ; this fucks everything up

    call kernel_main

    hlt

map_framebuffer:
    mov ebx, [framebuffer]
    or ebx, 0b10000011
    ; adding 8 moves add 2MiB to the mapped address
    ; example: page_table_l2 + 8 maps to 0x200000
    ;          page_table_l2 + 16 maps to 0x400000
    ;          page_table_l2 + 8 * 488 = 0x3D000000
    mov dword [page_table_l2 + 8 * 488], ebx 

    ; map more memory if needed
    cmp dword [screen_size], 0x200000
    jge .map_extra
    ret

.map_extra:
    mov ecx, 1
.loop:
    add ebx, 0x200000
    mov dword [page_table_l2 + 8 * (488 + ecx)], ebx 

    mov edx, ecx
    mov eax, 0x200000
    mul edx
    mov edx, eax
    add edx, 0x200000

    inc ecx
    cmp edx, [screen_size]
    jl .loop

    ret

section .bss
framebuffer resb 4
; width * height * bpp
screen_size resb 4