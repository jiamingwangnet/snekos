; global long_mode_start

; extern kernel_main
; extern gdt64.data
; extern gdt64.pointer
; extern stack_top

; section .text 
; bits 64
; long_mode_start:
;     ; update selectors
;     mov ax, gdt64.data
;     mov ss, ax
;     mov ds, ax
;     mov es, ax
;     mov fs, ax
;     mov gs, ax

;     mov rax, higher_half
;     jmp rax
    
; higher_half:
;     mov rsp, stack_top
;     lgdt [0xc0102018]

;     call kernel_main

;     hlt