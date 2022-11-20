#include "../idt.h"
#include "../serial.h"
#include "../stdlib.h"
#include "../io.h"
#include "../graphics.h"

extern void load_idt();

struct IDTEntry entries[256] = {};
struct IDTPtr pointer;

void add_idt_entry(uint8_t index, uint64_t offset, uint16_t sel, uint8_t attr)
{
    entries[index].offset_1 = (uint16_t) (offset & 0xffff);
    entries[index].offset_2 = (uint16_t) (offset >> 16 & 0xffff);
    entries[index].offset_3 = (uint32_t) (offset >> 32 & 0xffffffff);

    entries[index].zero = 0;
    entries[index].ist = 0;

    entries[index].selector = sel;
    entries[index].type_attributes = attr;
}

void irq_handler()
{
    serial_str("\n\nIRQ HANDLER\n");
    draw_rect(SCRN_WIDTH/2 - 250,SCRN_HEIGHT/2 - 250,500,500,(Color){0,0,255}, tagfb);
}
void isr_handler()
{
    serial_str("\n\nISR HANDLER\n");
    draw_rect(SCRN_WIDTH/2 - 250,SCRN_HEIGHT/2 - 250,500,500,(Color){0,0,255}, tagfb);
}

void init_idt()
{
    init_pic();

    add_idt_entry(0, (uint64_t)isr0, 0x8, 0x8e);
    add_idt_entry(1, (uint64_t)isr1, 0x8, 0x8e);
    add_idt_entry(2, (uint64_t)isr2, 0x8, 0x8e);
    add_idt_entry(3, (uint64_t)isr3, 0x8, 0x8e);
    add_idt_entry(4, (uint64_t)isr4, 0x8, 0x8e);
    add_idt_entry(5, (uint64_t)isr5, 0x8, 0x8e);
    add_idt_entry(6, (uint64_t)isr6, 0x8, 0x8e);
    add_idt_entry(7, (uint64_t)isr7, 0x8, 0x8e);
    add_idt_entry(8, (uint64_t)isr8, 0x8, 0x8e);
    add_idt_entry(9, (uint64_t)isr9, 0x8, 0x8e);
    add_idt_entry(10,(uint64_t)isr10, 0x8, 0x8e);
    add_idt_entry(11,(uint64_t)isr11, 0x8, 0x8e);
    add_idt_entry(12,(uint64_t)isr12, 0x8, 0x8e);
    add_idt_entry(13,(uint64_t)isr13, 0x8, 0x8e);
    add_idt_entry(14,(uint64_t)isr14, 0x8, 0x8e);
    add_idt_entry(15,(uint64_t)isr15, 0x8, 0x8e);
    add_idt_entry(16,(uint64_t)isr16, 0x8, 0x8e);
    add_idt_entry(17,(uint64_t)isr17, 0x8, 0x8e);
    add_idt_entry(18,(uint64_t)isr18, 0x8, 0x8e);
    add_idt_entry(19,(uint64_t)isr19, 0x8, 0x8e);
    add_idt_entry(20,(uint64_t)isr20, 0x8, 0x8e);
    add_idt_entry(21,(uint64_t)isr21, 0x8, 0x8e);
    add_idt_entry(22,(uint64_t)isr22, 0x8, 0x8e);
    add_idt_entry(23,(uint64_t)isr23, 0x8, 0x8e);
    add_idt_entry(24,(uint64_t)isr24, 0x8, 0x8e);
    add_idt_entry(25,(uint64_t)isr25, 0x8, 0x8e);
    add_idt_entry(26,(uint64_t)isr26, 0x8, 0x8e);
    add_idt_entry(27,(uint64_t)isr27, 0x8, 0x8e);
    add_idt_entry(28,(uint64_t)isr28, 0x8, 0x8e);
    add_idt_entry(29,(uint64_t)isr29, 0x8, 0x8e);
    add_idt_entry(30,(uint64_t)isr30, 0x8, 0x8e);
    add_idt_entry(31,(uint64_t)isr31, 0x8, 0x8e);

    add_idt_entry(32, (uint64_t) irq0, 0x08, 0x8E);
    add_idt_entry(33, (uint64_t) irq1, 0x08, 0x8E);
    add_idt_entry(35, (uint64_t) irq3, 0x08, 0x8E);
    add_idt_entry(36, (uint64_t) irq4, 0x08, 0x8E);
    add_idt_entry(37, (uint64_t) irq5, 0x08, 0x8E);
    add_idt_entry(38, (uint64_t) irq6, 0x08, 0x8E);
    add_idt_entry(39, (uint64_t) irq7, 0x08, 0x8E);
    add_idt_entry(40, (uint64_t) irq8, 0x08, 0x8E);
    add_idt_entry(41, (uint64_t) irq9, 0x08, 0x8E);
    add_idt_entry(42, (uint64_t) irq10, 0x08, 0x8E);
    add_idt_entry(43, (uint64_t) irq11, 0x08, 0x8E);
    add_idt_entry(44, (uint64_t) irq12, 0x08, 0x8E);
    add_idt_entry(45, (uint64_t) irq13, 0x08, 0x8E);
    add_idt_entry(46, (uint64_t) irq14, 0x08, 0x8E);
    add_idt_entry(47, (uint64_t) irq15, 0x08, 0x8E);

    pointer.size = sizeof(entries) - 1;
    pointer.base = (uint32_t) &entries;

    load_idt();
}

void init_pic()
{
    // Init ICW1
    out(0x20, 0x11);
    out(0xA0, 0x11);

    // Init ICW2
    out(0x21, 0x20);
    out(0xA1, 0x70);

    // Init ICW3
    out(0x21, 0x04);
    out(0xA1, 0x02);

    // Init ICW4
    out(0x21, 0x01);
    out(0xA1, 0x01);

    // mask interrupts
    out(0x21, 0x0);
    out(0xA1, 0x0);
}