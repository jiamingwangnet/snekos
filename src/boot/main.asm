global start

section .text ; program
bits 32
start:
    ; entry
    mov word [0xb8000], 0x0248 ; move H to video memory (0xb8000)
    hlt

section .bss ; uninitialised data

section .rodata ; readonly data
