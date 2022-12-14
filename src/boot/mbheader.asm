section .multiboot
header_start:
    dd 0xE85250D6 ; magic
    dd 0 ; architecture 0 is the code for protected mode
    dd header_end - header_start ; the size of the header
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; checksum (0x100000000 minus the sum of everything in the header)

    align 8
    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end: