global start
global gdt64.data
global multiboot_info
extern long_mode_start
global serial_com1

extern _kernel_start
extern _kernel_end

section multiboot.text
bits 32

setup_tables:
    ; map the kernel to 3gb

    mov edi, page_table_1 - 0xC0000000
    mov esi, 0
    mov ecx, 1023

one:
    cmp esi, _kernel_start    
    jl two
    cmp esi, _kernel_end - 0xC0000000
    jge three

    mov edx, esi
    or edx, 0x003
    mov edi, edx
two:
    add esi, 4096
    add edi, 4
    loop 1b
three:   
    mov dword [page_table_1 - 0xC0000000 + 1023 * 4], 0x000B8000 | 0x003
    mov dword [page_directory - 0xC0000000 + 0], page_table_1 - 0xC0000000 + 0x003
    mov dword [page_directory - 0xC0000000 + 768 * 4], page_table_1 - 0xC0000000 + 0x003

    mov ecx, page_directory - 0xC0000000
    mov cr3, ecx

    mov ecx, cr0
    or ecx, 0x80010000
    mov cr0, ecx

    mov ecx, four
    jmp [ecx]

    ret

start:
    call setup_tables

section .text ; program
four:
    mov dword [page_directory + 0], 0
    mov ecx, cr3
    mov cr3, ecx

    cmp eax, 0x36d76289
    jne .err

    mov dword [multiboot_info], ebx

    ; entry
    mov esp, stack_top ; setup stack pointer   

;     mov ecx, 0
; .fill
;     mov byte [0xfd000000 + ecx], 0xff ; works before paging because it is accessing it as a physical address
;     inc ecx
;     cmp ecx, 1280 * 800 * 4
;     jne .fill

    ; grub starts protected mode which means it also loads its own gdt, however, grub's gdt should not be used
    lgdt [gdt64.pointer] ; load the gdt
    jmp gdt64.code:long_mode_start ; far jump
.err:
oneb: hlt
      jmp oneb

serial_com1:
    mov dx, 0x3F8
    out dx, eax

    ret

section .bss ; uninitialised data
; stack
align 4096 ; 4KiB
page_directory:
    resb 4096
page_table_1:
    resb 4096

stack_bottom:
    resb 4096 ; reserve 4KiB of space
stack_top:

multiboot_info resb 4

section .rodata ; readonly data
; global discriptor table
gdt64:
    dq 0 ; zero entry (dq is quadword 64bit)
; code segment
.code: equ $ - gdt64 ; get the offset of this segment
    ; 44: discriptor type 
    ; 47: present bit 
    ; 41: read/write 
    ; 43: executable (set to one for code seg) 
    ; 53: 64bit
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53) 
; data segment
.data: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41)
; special structure for loading gdt
.pointer:
    dw .pointer - gdt64 - 1 ; length (2 bytes)
    dq gdt64 ; the address of the table
kernel_end: