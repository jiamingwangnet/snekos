global long_mode_start
global load_idt

extern kernel_main
extern gdt64.data
extern stack_top
extern gdt64.pointer

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

    mov rax, higher_half
    jmp higher_half

higher_half:
    mov rsp, stack_top
    lgdt [gdt64.pointer]
    
    %ifdef ENABLE_AVX
    call enable_avx
    %endif
    
    call kernel_main

    hlt

load_idt:
    lidt [edi] ; the first param gets passed into edi
    ret

%ifdef ENABLE_AVX
enable_avx:
    push rax
    push rcx
    push rdx
 
    xor rcx, rcx
    xgetbv ;Load XCR0 register
    or eax, 7 ;Set AVX, SSE, X87 bits
    xsetbv ;Save back to XCR0
 
    pop rdx
    pop rcx
    pop rax
    ret
%endif