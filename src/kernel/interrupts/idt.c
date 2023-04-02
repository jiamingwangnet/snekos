#include "../include/interrupts/idt.h"
#include "../include/io/serial.h"
#include "../include/stdlib/stdlib.h"
#include "../include/io/io.h"
#include "../include/graphics/graphics.h"
#include "../include/graphics/font.h"
#include "../include/memory/memory.h"

extern void load_idt(struct IDTPtr* pointer);

__attribute__((aligned(0x10))) 
struct IDTEntry entries[256] = {};
struct IDTPtr pointer;

void add_idt_entry(uint8_t index, void* offset, uint16_t sel, uint8_t attr, uint8_t ist)
{
    entries[index].offset_2 = (uint16_t) ((uint64_t)offset >> 16 & 0xffff);
    entries[index].offset_3 = (uint32_t) ((uint64_t)offset >> 32 & 0xffffffff);
    entries[index].offset_1 = (uint16_t) ((uint64_t)offset & 0xffff);

    entries[index].zero = (uint32_t)0;
    entries[index].ist = ist;

    entries[index].selector = sel;
    entries[index].type_attributes = attr;
}

#define ERR_FG 0xe0e0e0
#define ERR_BG 0x0000ff

void isr_handler(reg_status_t status)
{
    char c_id[15];
    itoa(status.interrupt_number, c_id, 10);
    char c_code[15];
    itoa(status.error_code, c_code, 2);

    char c_rax[15];
    itoa(status.rax, c_rax, 16);

    char c_rbx[15];
    itoa(status.rbx, c_rbx, 16);
    
    char c_rcx[15];
    itoa(status.rcx, c_rcx, 16);

    char c_rdx[15];
    itoa(status.rdx, c_rdx, 16);

    char c_rsi[15];
    itoa(status.rsi, c_rsi, 16);

    char c_rdi[15];
    itoa(status.rdi, c_rdi, 16);

    char c_rip[15];
    itoa(status.rip, c_rip, 16);

    char c_cs[15];
    itoa(status.cs, c_cs, 16);

    char c_ss[15];
    itoa(status.ss, c_ss, 16);

    fill_screen(ERR_BG);
    
    draw_str(20, 20, ERR_FG, ERR_BG, "FATAL ERROR!");

    draw_str(20, 37, ERR_FG, ERR_BG, "ID: ");
    draw_str(20 + PSF1_WIDTH + 17 * 11, 37, ERR_FG, ERR_BG, c_id);

    draw_str(20, 54, ERR_FG, ERR_BG, "Error Code: ");
    draw_str(20 + PSF1_WIDTH + 17 * 11, 54, ERR_FG, ERR_BG, c_code);

    draw_str(20, 88, ERR_FG, ERR_BG, "Register Info: ");

    draw_str(20, 105, ERR_FG, ERR_BG, "RAX:                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 105, ERR_FG, ERR_BG, c_rax);

    draw_str(20, 122, ERR_FG, ERR_BG, "RBX:                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 122, ERR_FG, ERR_BG, c_rbx);

    draw_str(20, 139, ERR_FG, ERR_BG, "RCX:                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 139, ERR_FG, ERR_BG, c_rcx);

    draw_str(20, 156, ERR_FG, ERR_BG, "RDX:                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 156, ERR_FG, ERR_BG, c_rdx);

    draw_str(20, 173, ERR_FG, ERR_BG, "RSI:                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 173, ERR_FG, ERR_BG, c_rsi);

    draw_str(20, 190, ERR_FG, ERR_BG, "RDI:                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 190, ERR_FG, ERR_BG, c_rdi);

    draw_str(20, 207, ERR_FG, ERR_BG, "RIP:                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 207, ERR_FG, ERR_BG, c_rip);

    draw_str(20, 224, ERR_FG, ERR_BG, "CS :                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 224, ERR_FG, ERR_BG, c_cs);

    draw_str(20, 241, ERR_FG, ERR_BG, "SS :                0x");
    draw_str(20 + PSF1_WIDTH + 17 * 11 + 4, 241, ERR_FG, ERR_BG, c_ss);
    update_buffer();

#ifdef DEBUG_LOG
    serial_str("\n\ninterrupt hit!! ID: ");
    serial_str(c_id);
    serial_str(" error code: ");
    serial_str(c_code);
    serial_char('\n');

    serial_str("register info: \n");

    serial_str("RAX: 0x");
    serial_str(c_rax);
    serial_char('\n');
    
    serial_str("RBX: 0x");
    serial_str(c_rbx);
    serial_char('\n');
    
    serial_str("RCX: 0x");
    serial_str(c_rcx);
    serial_char('\n');
    
    serial_str("RDX: 0x");
    serial_str(c_rdx);
    serial_char('\n');
    
    serial_str("RSI: 0x");
    serial_str(c_rsi);
    serial_char('\n');
    
    serial_str("RDI: 0x");
    serial_str(c_rdi);
    serial_char('\n');

    serial_str("RIP: 0x");
    serial_str(c_rip);
    serial_char('\n');
    
    serial_str("CS : ");
    serial_str(c_cs);
    serial_char('\n');
    
    serial_str("SS : ");
    serial_str(c_ss);
    serial_char('\n');
#endif
    __asm__ volatile("cli\nhlt");
}

void init_idt()
{
    add_idt_entry(0,  &isr0, 0x8, 0x8e,  0);
    add_idt_entry(1,  &isr1, 0x8, 0x8e,  0);
    add_idt_entry(2,  &isr2, 0x8, 0x8e,  0);
    add_idt_entry(3,  &isr3, 0x8, 0x8e,  0);
    add_idt_entry(4,  &isr4, 0x8, 0x8e,  0);
    add_idt_entry(5,  &isr5, 0x8, 0x8e,  0);
    add_idt_entry(6,  &isr6, 0x8, 0x8e,  0);
    add_idt_entry(7,  &isr7, 0x8, 0x8e,  0);
    add_idt_entry(8,  &isr8, 0x8, 0x8e,  0);
    add_idt_entry(9,  &isr9, 0x8, 0x8e,  0);
    add_idt_entry(10, &isr10, 0x8, 0x8e, 0);
    add_idt_entry(11, &isr11, 0x8, 0x8e, 0);
    add_idt_entry(12, &isr12, 0x8, 0x8e, 0);
    add_idt_entry(13, &isr13, 0x8, 0x8e, 0);
    add_idt_entry(14, &isr14, 0x8, 0x8e, 0);
    add_idt_entry(15, &isr15, 0x8, 0x8e, 0);
    add_idt_entry(16, &isr16, 0x8, 0x8e, 0);
    add_idt_entry(17, &isr17, 0x8, 0x8e, 0);
    add_idt_entry(18, &isr18, 0x8, 0x8e, 0);
    add_idt_entry(19, &isr19, 0x8, 0x8e, 0);
    add_idt_entry(20, &isr20, 0x8, 0x8e, 0);
    add_idt_entry(21, &isr21, 0x8, 0x8e, 0);
    add_idt_entry(22, &isr22, 0x8, 0x8e, 0);
    add_idt_entry(23, &isr23, 0x8, 0x8e, 0);
    add_idt_entry(24, &isr24, 0x8, 0x8e, 0);
    add_idt_entry(25, &isr25, 0x8, 0x8e, 0);
    add_idt_entry(26, &isr26, 0x8, 0x8e, 0);
    add_idt_entry(27, &isr27, 0x8, 0x8e, 0);
    add_idt_entry(28, &isr28, 0x8, 0x8e, 0);
    add_idt_entry(29, &isr29, 0x8, 0x8e, 0);
    add_idt_entry(30, &isr30, 0x8, 0x8e, 0);
    add_idt_entry(31, &isr31, 0x8, 0x8e, 0);

    add_idt_entry(32, &irq0,  0x08,  0x8E,  0);
    add_idt_entry(33, &irq1,  0x08,  0x8E,  0);
    add_idt_entry(34, &irq2,  0x08,  0x8E,  0);
    add_idt_entry(35, &irq3,  0x08,  0x8E,  0);
    add_idt_entry(36, &irq4,  0x08,  0x8E,  0);
    add_idt_entry(37, &irq5,  0x08,  0x8E,  0);
    add_idt_entry(38, &irq6,  0x08,  0x8E,  0);
    add_idt_entry(39, &irq7,  0x08,  0x8E,  0);
    add_idt_entry(40, &irq8,  0x08,  0x8E,  0);
    add_idt_entry(41, &irq9,  0x08,  0x8E,  0);
    add_idt_entry(42, &irq10, 0x08,  0x8E,  0);
    add_idt_entry(43, &irq11, 0x08,  0x8E,  0);
    add_idt_entry(44, &irq12, 0x08,  0x8E,  0);
    add_idt_entry(45, &irq13, 0x08,  0x8E,  0);
    add_idt_entry(46, &irq14, 0x08,  0x8E,  0);
    add_idt_entry(47, &irq15, 0x08,  0x8E,  0);
    
    for(uint8_t i = 0; i < 16; i++)
        pic_mask_irq(i);

    init_pic(0x20, 0x28);

    for(uint8_t i = 0; i < 16; i++)
        pic_unmask_irq(i);
    
    pointer.size = (uint16_t)sizeof(entries) - 1;
    pointer.base = (uintptr_t) &entries[0];

    load_idt((struct IDTPtr*)virt_to_phys(&pointer));

    __asm__ ("sti"); // enable irq or something idk;
}

void init_pic(uint8_t offset1, uint8_t offset2)
{
    uint8_t mask_1, mask_2;
    mask_1 = in(PIC1_DATA); // save masks
    mask_2 = in(PIC2_DATA);

    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // starts init sequences (cascade mode)
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    out(PIC1_DATA, offset1); // ICW2 master PIC offset
    out(PIC2_DATA, offset2); // ICW2 slave PIC offset

    out(PIC1_DATA, 4); // ICW3 tell master PIC there is slave PIC at IRQ 2 (0000 0100)
    out(PIC2_DATA, 2); // ICW3 tell slave PIC cascade identity (0000 0010)

    out(PIC1_DATA, ICW4_8086);
    out(PIC2_DATA, ICW4_8086);

    out(PIC1_DATA, mask_1); // restore masks
    out(PIC2_DATA, mask_2);
}

void pic_mask_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t masks;

    if(irq < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    masks = in(port);
    masks |= (1 << irq);
    out(port, masks);
}

void pic_unmask_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t masks;
    
    if(irq < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    masks = in(port);
    masks &= ~(1 << irq);
    out(port, masks);
}