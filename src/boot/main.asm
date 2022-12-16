global start
global gdt64.data
global stack_top
global gdt64.pointer

extern kernel_main
%define KERNEL_VOFFSET 0xffffffff80000000
%define PAGE_SIZE 0x200000
%define PAGE_TABLE_ENTRY HUGEPAGE_BIT | WRITE_BIT | PRESENT_BIT
%define LOOP_LIMIT 512
%define PAGE_DIR_ENTRY_FLAGS 0b11
%define PRESENT_BIT 1
%define WRITE_BIT 0b10
%define HUGEPAGE_BIT 0b10000000

section .multiboot.text
bits 32
start:
    mov edi, ebx ; Address of multiboot structure
    mov esi, eax ; Magic number

    mov esp, stack_top - KERNEL_VOFFSET
    
    mov eax, page_table_l3 - KERNEL_VOFFSET; Copy page_table_l3 address in eax
    or eax, PRESENT_BIT | WRITE_BIT        ; set writable and present bits to 1
    mov dword [(page_table_l4 - KERNEL_VOFFSET) + 0], eax   ; Copy eax content into the entry 0 of p4 table

    mov eax, page_table_l3_hh - KERNEL_VOFFSET  ; This will contain the mapping of the kernel in the higher half
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword [(page_table_l4 - KERNEL_VOFFSET) + 511 * 8], eax

    mov eax, page_table_l4 - KERNEL_VOFFSET ; Mapping the PML4 into itself
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword [(page_table_l4 - KERNEL_VOFFSET) + 510 * 8], eax

    mov eax, page_table_l2 - KERNEL_VOFFSET  ; Let's do it again, with page_table_l2
    or eax, PRESENT_BIT | WRITE_BIT       ; Set the writable and present bits
    mov dword [(page_table_l3 - KERNEL_VOFFSET) + 0], eax   ; Copy eax content in the 0th entry of p3

    mov eax, page_table_l2 - KERNEL_VOFFSET
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword[(page_table_l3_hh - KERNEL_VOFFSET) + 510 * 8], eax

    ; Now let's prepare a loop...
    mov ecx, 0  ; Loop counter

    .map_page_table_l2:
        mov eax, PAGE_SIZE  ; Size of the page
        mul ecx             ; Multiply by counter
        or eax, PAGE_TABLE_ENTRY ; We set: huge page bit, writable and present 

        ; Moving the computed value into page_table_l2 entry defined by ecx * 8
        ; ecx is the counter, 8 is the size of a single entry
        mov [(page_table_l2 - KERNEL_VOFFSET) + ecx * 8], eax

        inc ecx             ; Let's increase ecx
        cmp ecx, LOOP_LIMIT        ; have we reached 512 ? (1024 for small pages)
                            ; When small pages is enabled:
                            ; each table is 4k size. Each entry is 8bytes
                            ; that is 512 entries in a table
                            ; when small pages enabled: two tables are adjacent in memory
                            ; they are mapped in the pdir during the map_pd_table cycle
                            ; this is why the loop is up to 1024
        
        jne .map_page_table_l2   ; if ecx < 512 then loop


    ; All set... now we are nearly ready to enter into 64 bit
    ; Is possible to move into cr3 only from another register
    ; So let's move page_table_l4 address into eax first
    ; then into cr3
    mov eax, (page_table_l4 - KERNEL_VOFFSET)
    mov cr3, eax

    ; Now we can enable PAE
    ; To do it we need to modify cr4, so first let's copy it into eax
    ; we can't modify cr registers directly
    mov eax, cr4
    or eax, 1 << 5  ; Physical address extension bit
    mov cr4, eax
    
    ; Now set up the long mode bit
    mov ecx, 0xC0000080
    ; rdmsr is to read a a model specific register (msr)
    ; it copy the values of msr into eax
    rdmsr
    or eax, 1 << 8
    ; write back the value
    wrmsr
    
    ; Now is tiem to enable paging
    mov eax, cr0    ;cr0 contains the values we want to change
    or eax, 1 << 31 ; Paging bit
    or eax, 1 << 16 ; Write protect, cpu  can't write to read-only pages when
                    ; privilege level is 0
    mov cr0, eax    ; write back cr0
    ; load gdt 
    lgdt [gdt64.pointer_low - KERNEL_VOFFSET]
    jmp (0x8):(kernel_jumper - KERNEL_VOFFSET)
    bits 64

section .text
kernel_jumper:
    bits 64    

    ; update segment selectors
    mov ax, 0x10
    mov ss, ax  ; Stack segment selector
    mov ds, ax  ; data segment register
    mov es, ax  ; extra segment register
    mov fs, ax  ; extra segment register
    mov gs, ax  ; extra segment register
    
    lea rax, [rdi+8]
    
    ;.bss section should be already 0  at least on unix and windows systems
    ;no need to initialize

    mov rax, higher_half
    jmp rax

higher_half:
    ; Far jump to long mode
    mov rsp, stack_top
    lgdt [gdt64.pointer]

    ; The two lines below are needed to un map the kernel in the lower half
    ; But i'll leave them commented for now because the code in the kernel need 
    ; to be changed and some addresses need to be updated (i.e. multiboot stuff)
    ;mov eax, 0x0
    ;mov dword [(page_table_l4 - KERNEL_VOFFSET) + 0], eax
    call kernel_main

section .bss ; uninitialised data
; stack
align 4096 ; 4KiB
page_table_l4:
    resb 4096
page_table_l3:
    resb 4096
page_table_l3_hh:
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
.pointer_low:
    dw .pointer - gdt64 - 1 ; length (2 bytes)
    dq gdt64 - KERNEL_VOFFSET ; the address of the table