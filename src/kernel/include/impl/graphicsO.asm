section .text
bits 64

%ifdef OPTIMISE_GRAPHICS
global put_pixel
global draw_rect
global fill_screen
global update_buffer

extern SCRN_WIDTH
extern SCRN_HEIGHT
extern SCRN_SIZE
extern B_BUFFER ; gives the physical address or something idk but you gotta subtract the higher half offset thing
extern SRN_BUFFER
extern SCRN_PITCH

update_buffer:
    push rax
    push rbx
    push rcx

    mov rax, [B_BUFFER - 0xffffffff80000000]
    mov rbx, [SRN_BUFFER - 0xffffffff80000000]
    xor rcx, rcx
.fill:
    ; TODO: check alignment for movaps to get speed, i just changed it to movups which is slower on old processors.
    ; Backbuffer (B_BUFFER) is misaligned which gives problems for movaps
    movups xmm0, [rax + rcx]
    movups [rbx + rcx], xmm0
    add rcx, 16
    cmp rcx, [SCRN_SIZE]
    jne .fill

    pop rcx
    pop rbx
    pop rax
    ret

put_pixel:
    imul rsi, [SCRN_WIDTH]
    add rsi, rdi
    mov rax, [B_BUFFER - 0xffffffff80000000]
    mov dword [rax + rsi*4], edx

    ret

draw_rect:
    ; FIXME: i fucked up the registers with the optimisation shit now i have to push all of them for safety
    push rcx
    push rsi
    push rdx
    push rdi
    push rax
    push r9
    push rbx

    mov rax, [B_BUFFER - 0xffffffff80000000]

    add rcx, rsi
    imul rsi, [SCRN_PITCH]
    imul rcx, [SCRN_PITCH]
    add rcx, rax
    add rsi, rax

    add rdx, rdi ; x
    mov r9, rdi

.loop_y:
    mov rdi, r9
    
.loop_x:
    mov dword [rsi + rdi*4], r8d

    inc rdi
    cmp rdi, rdx
    jne .loop_x

    add rsi, [SCRN_PITCH]
    cmp rsi, rcx
    jne .loop_y

    pop rbx
    pop r9
    pop rax
    pop rdi
    pop rdx
    pop rsi
    pop rcx

    ret

fill_screen:
    push rax
    push rcx
    push rbx
    push rdx

    mov rax, [B_BUFFER - 0xffffffff80000000]

    mov rbx, rdi
    shl rdi, 32
    add rbx, rdi

    movq xmm0, rbx 
    movq xmm1, rbx
    pslldq xmm0, 8
    addps xmm0, xmm1
    
    xor rcx, rcx
.fill:
    ; TODO: check alignment for movaps to get speed, i just changed it to movups which is slower on old processors.
    movups [rax + rcx], xmm0
    add rcx, 16
    cmp rcx, [SCRN_SIZE]
    jne .fill

    pop rdx
    pop rbx
    pop rcx
    pop rax

    ret

%endif