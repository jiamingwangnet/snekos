global start
global gdt64.data
global multiboot_info
global gdt64.pointer
global stack_top
global page_table_l2

extern long_mode_start

%define VOFFSET 0xFFFFFFFF80000000
%define L2_TABLES 4

section .multiboot.text ; program
bits 32
start:
    cli
    ; entry

    mov esp, stack_top - VOFFSET ; setup stack pointer  

    call save_multiboot_info 
    call check_multiboot
    call check_cpuid
    call check_long_mode
    call check_sse

    call setup_tables
    call enable_paging

    ; grub starts protected mode which means it also loads its own gdt, however, grub's gdt should not be used
    lgdt [gdt64.pointer_low - VOFFSET] ; load the gdt
    
    jmp gdt64.code:(long_mode_start - VOFFSET) ; far jump
err:
    mov ecx, 0xA0000
.fill_red:
    mov dword [ecx], eax
    add ecx, 4
    cmp ecx, 0xAFFFF
    jne .fill_red

    hlt

check_multiboot: ; 18c718
    cmp eax, 0x36d76289

    mov eax, 0x18c718 ; error color
    jne err
    ret

check_cpuid: ; 0xe805be
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx

    mov eax, 0xe805be ; error color
    je err
    ret

check_sse: ; 0x160ef0
    mov eax, 0x1
    cpuid
    test edx, 1 << 25

    mov eax, 0x160ef0 ; error color
    jz err
    
    ; TODO: check for fpu?
    ; init fpu (floating point unit)
    fninit

    ; enable sse
    mov eax, cr0
    and ax, 0xfffb
    or ax, 0x2
    mov cr0, eax
    
    mov eax, cr4
    
    %ifdef ENABLE_AVX
    or eax, 515 << 9
    %else
    or eax, 3 << 9
    %endif

    mov cr4, eax
    ret

check_long_mode: ; f00e0e
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb err

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    mov eax, 0xf00e0e ; error color
    jz err

    ret

save_multiboot_info:
    mov dword [multiboot_info - VOFFSET], ebx
    ret

setup_tables:
    mov eax, page_table_l3_low - VOFFSET ; get physical address
    or eax, 0b11 ; present bit and writable bit
    mov dword [(page_table_l4 - VOFFSET) + 0], eax ; make page_table_l4 point to page_table_l3

    mov eax, page_table_l3_high - VOFFSET ; mapping of the higher half
    or eax, 0b11 
    mov dword [(page_table_l4 - VOFFSET) + 511 * 8], eax 

    mov eax, page_table_l4 - VOFFSET ; map page table 4 to itself
    or eax, 0b11 
    mov dword [(page_table_l4 - VOFFSET) + 510 * 8], eax 
    
    mov eax, page_table_l2 - VOFFSET
    or eax, 0b11
    mov dword [(page_table_l3_high - VOFFSET) + 510 * 8], eax

    xor ecx, ecx
    mov eax, page_table_l2 - VOFFSET
    or eax, 0b11
.map_l3_table:
    mov dword [(page_table_l3_low - VOFFSET) + ecx * 8], eax

    add eax, 4096
    inc ecx
    cmp ecx, L2_TABLES
    jne .map_l3_table

    ; loop
    mov ecx, 0 ; ecx is the counter
.map_l2_table: ; maps the l2 table to point to valid pages
    mov eax, 0x200000  ; 2 MiB, eax is used for multiplication
    mul ecx ; muliplies ecx by eax and stores the result in eax
    or eax, 0b10000011 ; the first bit indicates that this page is very large (huge page bit)
    mov dword [(page_table_l2 - VOFFSET) + ecx * 8], eax ; write the page into the table

    inc ecx
    cmp ecx, 512 ; loop 512 times
    jne .map_l2_table

    ret

enable_paging:
    mov eax, page_table_l4 - VOFFSET ; cannot directly move to cr3
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
page_table_l3_low:
    resb 4096
page_table_l3_high:
    resb 4096
page_table_l2:
    resb 4096 * L2_TABLES

; page_table_l2_high: ; TODO: reserve more memory for page_table_l2 instead of making a new label
;     resb 4096

align 16
stack_bottom:
    resb 16384 ; reserve 16KiB of space
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
.pointer_low:
    dw .pointer - gdt64 - 1 ; length (2 bytes)
    dq gdt64 - VOFFSET ; the address of the table