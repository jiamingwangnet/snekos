#include "../idt.h"
#include "../serial.h"
#include "../stdlib.h"
#include "../io.h"
#include "../graphics.h"
#include "../memory.h"

extern void load_idt(struct IDTPtr* pointer);

__attribute__((aligned(0x10))) 
struct IDTEntry entries[256] = {};
struct IDTPtr pointer;

void add_idt_entry(uint8_t index, uint64_t offset, uint16_t sel, uint8_t attr, uint8_t ist)
{
    entries[index].offset_1 = (uint16_t) (offset & 0xffff);
    entries[index].offset_2 = (uint16_t) (offset >> 16 & 0xffff);
    entries[index].offset_3 = (uint32_t) (offset >> 32 & 0xffffffff);

    entries[index].zero = (uint32_t)0;
    entries[index].ist = ist;

    entries[index].selector = sel;
    entries[index].type_attributes = attr;
}

void isr_handler(uint8_t id)
{
    serial_str("\n\ninterrupt hit!! ID: ");
    char c_id[15];
    itoa(id, c_id, 10);
    serial_str(c_id);
    serial_char('\n');
    __asm__ volatile("cli\nhlt");
}

void init_idt()
{
    add_idt_entry(0, (uint64_t)&isr0, 0x8, 0x8e,  0);
    add_idt_entry(1, (uint64_t)&isr1, 0x8, 0x8e,  0);
    add_idt_entry(2, (uint64_t)&isr2, 0x8, 0x8e,  0);
    add_idt_entry(3, (uint64_t)&isr3, 0x8, 0x8e,  0);
    add_idt_entry(4, (uint64_t)&isr4, 0x8, 0x8e,  0);
    add_idt_entry(5, (uint64_t)&isr5, 0x8, 0x8e,  0);
    add_idt_entry(6, (uint64_t)&isr6, 0x8, 0x8e,  0);
    add_idt_entry(7, (uint64_t)&isr7, 0x8, 0x8e,  0);
    add_idt_entry(8, (uint64_t)&isr8, 0x8, 0x8e,  0);
    add_idt_entry(9, (uint64_t)&isr9, 0x8, 0x8e,  0);
    add_idt_entry(10,(uint64_t)&isr10, 0x8, 0x8e, 0);
    add_idt_entry(11,(uint64_t)&isr11, 0x8, 0x8e, 0);
    add_idt_entry(12,(uint64_t)&isr12, 0x8, 0x8e, 0);
    add_idt_entry(13,(uint64_t)&isr13, 0x8, 0x8e, 0);
    add_idt_entry(14,(uint64_t)&isr14, 0x8, 0x8e, 0);
    add_idt_entry(15,(uint64_t)&isr15, 0x8, 0x8e, 0);
    add_idt_entry(16,(uint64_t)&isr16, 0x8, 0x8e, 0);
    add_idt_entry(17,(uint64_t)&isr17, 0x8, 0x8e, 0);
    add_idt_entry(18,(uint64_t)&isr18, 0x8, 0x8e, 0);
    add_idt_entry(19,(uint64_t)&isr19, 0x8, 0x8e, 0);
    add_idt_entry(20,(uint64_t)&isr20, 0x8, 0x8e, 0);
    add_idt_entry(21,(uint64_t)&isr21, 0x8, 0x8e, 0);
    add_idt_entry(22,(uint64_t)&isr22, 0x8, 0x8e, 0);
    add_idt_entry(23,(uint64_t)&isr23, 0x8, 0x8e, 0);
    add_idt_entry(24,(uint64_t)&isr24, 0x8, 0x8e, 0);
    add_idt_entry(25,(uint64_t)&isr25, 0x8, 0x8e, 0);
    add_idt_entry(26,(uint64_t)&isr26, 0x8, 0x8e, 0);
    add_idt_entry(27,(uint64_t)&isr27, 0x8, 0x8e, 0);
    add_idt_entry(28,(uint64_t)&isr28, 0x8, 0x8e, 0);
    add_idt_entry(29,(uint64_t)&isr29, 0x8, 0x8e, 0);
    add_idt_entry(30,(uint64_t)&isr30, 0x8, 0x8e, 0);
    add_idt_entry(31,(uint64_t)&isr31, 0x8, 0x8e, 0);

    add_idt_entry(32, (uint64_t)&irq0,  0x08,  0x8E,  0);
    add_idt_entry(33, (uint64_t)&irq1,  0x08,  0x8E,  0);
    add_idt_entry(34, (uint64_t)&irq2,  0x08,  0x8E,  0);
    add_idt_entry(35, (uint64_t)&irq3,  0x08,  0x8E,  0);
    add_idt_entry(36, (uint64_t)&irq4,  0x08,  0x8E,  0);
    add_idt_entry(37, (uint64_t)&irq5,  0x08,  0x8E,  0);
    add_idt_entry(38, (uint64_t)&irq6,  0x08,  0x8E,  0);
    add_idt_entry(39, (uint64_t)&irq7,  0x08,  0x8E,  0);
    add_idt_entry(40, (uint64_t)&irq8,  0x08,  0x8E,  0);
    add_idt_entry(41, (uint64_t)&irq9,  0x08,  0x8E,  0);
    add_idt_entry(42, (uint64_t)&irq10, 0x08,  0x8E,  0);
    add_idt_entry(43, (uint64_t)&irq11, 0x08,  0x8E,  0);
    add_idt_entry(44, (uint64_t)&irq12, 0x08,  0x8E,  0);
    add_idt_entry(45, (uint64_t)&irq13, 0x08,  0x8E,  0);
    add_idt_entry(46, (uint64_t)&irq14, 0x08,  0x8E,  0);
    add_idt_entry(47, (uint64_t)&irq15, 0x08,  0x8E,  0);
    
    for(uint8_t i = 0; i < 16; i++)
        pic_mask_irq(i);

    init_pic(0x20, 0x28);

    for(uint8_t i = 0; i < 16; i++)
        pic_unmask_irq(i);
    
    pointer.size = (uint16_t)sizeof(entries) - 1;
    pointer.base = (uintptr_t) &entries[0];

    load_idt(virt_to_phys((uint64_t)&pointer));

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