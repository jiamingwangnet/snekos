global start
global gdt64.data

extern long_mode_start

section .text ; program
bits 32
start:
    ; entry

    mov esp, stack_top; setup stack pointer  

    call setup_tables
    call enable_paging ; page fault

    ; grub starts protected mode which means it also loads its own gdt, however, grub's gdt should not be used
    lgdt [gdt64.pointer] ; load the gdt

    jmp gdt64.code:long_mode_start ; far jump

    hlt

setup_tables:
    mov eax, page_table_l3
    or eax, 0b11 ; present bit and writable bit
    mov dword [page_table_l4 + 0], eax ; make page_table_l4 point to page_table_l3

    mov eax, page_table_l2
    or eax, 0b11
    mov dword [page_table_l3 + 0], eax

    ; loop
    mov ecx, 0 ; ecx is the counter
.map_l2_table: ; maps the l2 table to point to valid pages
    mov eax, 0x200000  ; 2 MiB, eax is used for multiplication
    mul ecx ; muliplies ecx by eax and stores the result in eax
    or eax, 0b10000011 ; the first bit indicates that this page is very large (huge page bit)
    mov [page_table_l2 + ecx * 8], eax ; write the page into the table

    inc ecx
    cmp ecx, 512 ; loop 512 times
    jne .map_l2_table

    ret

enable_paging:
    mov eax, page_table_l4 ; cannot directly move to cr3
    mov cr3, eax ; move the page table to cr3 (control register)

    ; enable PAE
    mov eax, cr4 ; modify the contents of cr4
    or eax, 1 << 5 ; set the 5th bit
    mov cr4, eax

    ; set the long mode bit
    mov ecx, 0xC0000080
    rdmsr ; accesses machine specific registers
    or eax, 1 << 8
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    or eax, 1 << 16
    mov cr0, eax

    ret

section .bss ; uninitialised data
; stack
align 4096 ; 4KiB
page_table_l4:
    resb 4096
page_table_l3:
    resb 4096
page_table_l2:
    resb 4096

align 16
stack_bottom:
    resb 16384 ; reserve 16KiB of space
stack_top:

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