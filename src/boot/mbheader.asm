section .multiboot
header_start:
    dd 0xE85250D6 ; magic
    dd 0 ; architecture 0 is the code for protected mode
    dd header_end - header_start ; the size of the header
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; checksum (0x100000000 minus the sum of everything in the header)

    align 8
frame_buffer_tag_start:
    dw 0x05    ; type
    dw 0x01    ; flags
    dd frame_buffer_tag_end - frame_buffer_tag_start  ; size
    ; setting to 0 will give recommended settings
    dd 0  ; width
    dd 0  ; height
    dd 0   ; depth
frame_buffer_tag_end:

    align 8
    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end: