global memcpy

section .text
memcpy:
    mov rcx, 0
.copy_loop:
    mov al, [rsi + rcx]
    mov byte [rdi + rcx], al

    inc rcx
    cmp rcx, rdx
    jl .copy_loop

    ret