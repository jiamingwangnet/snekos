section .text
bits 64

; 1: size 2: register 3: decrement 4: name
%macro _MEMCPY 4
global memcpy%4
memcpy%4:
    mov %1 %2, [rsi + rdx - %3]
    mov %1 [rdi + rdx - %3], %2

    sub rdx, %3
    cmp rdx,0
    jne memcpy%4

    mov rax, rdi ; return destination pointer
    ret
%endmacro

_MEMCPY qword, rax, 8, 64
_MEMCPY dword, eax, 4, 32
_MEMCPY  word, ax,  2, 16
_MEMCPY  byte, al,  1, 8