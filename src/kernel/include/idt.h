#pragma once

#include "types.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI 0x20

#define ICW1_ICW4 0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE 0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08		/* Level triggered (edge) mode */
#define ICW1_INIT 0x10		/* Initialization - required! */

#define ICW4_8086 0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C		/* Buffered mode/master */
#define ICW4_SFNM 0x10		/* Special fully nested (not) */

struct IDTEntry // 64bit
{
    // offset is the address of the handler
    uint16_t offset_1;        // offset bits 0..15
    uint16_t selector;        // a code segment selector in GDT or LDT
    uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2;        // offset bits 16..31
    uint32_t offset_3;        // offset bits 32..63
    uint32_t zero;            // reserved
} __attribute__((packed));

struct IDTPtr
{
    uint16_t size;
    uint64_t base;
} __attribute__((packed));

typedef struct 
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    
    uint64_t interrupt_number;
    uint64_t error_code;
    
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) reg_status_t;

void add_idt_entry(uint8_t index, void* offset, uint16_t sel, uint8_t attr, uint8_t ist);
void init_idt();
void isr_handler(reg_status_t status);

// configure PIC (interrupt controller)
// from https://wiki.osdev.org/Interrupts
/*
    "
    Alert readers will notice that the CPU has reserved interrupts 0-31, yet IRQs 0-7 are set to interrupts 08-0Fh. 
    Now the reserved interrupts are called when, for example, a dreadful error has occurred that the OS must handle. 
    Now when the computer first starts up, most errors of this type won't occur. 
    However, when you enter protected mode (and every OS should use protected mode, real mode is obsolete), 
    these errors may occur at any time, and the OS needs to be able to handle them. 
    How's the OS going to tell the difference between 
    INT 9, Exception: Coprocessor segment overrun, and INT 9: IRQ 1? Well, it can ask the device whether there is really an interrupt for that device.
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 
    But this is slow, and hackish, and not all devices are able to do this type of thing. 
    The best way to do it is to tell the PIC to map the IRQs to different interrupts, such as INT 78h-7Fh. 
    For information on this, see the PIC FAQ. 
    Note that IRQs can only be mapped to INTs that are multiples of 08h: 00h-07h, 08h-0Fh, 10h-17h, 17h-1Fh. 
    And you probably want to use 20h-27h, or greater, since 00h-1Fh are reserved by the CPU. Also, each PIC has to be programmed separately. 
    You can tell the Master PIC to map IRQs 0-7 to INTs 20h-27h, but IRQs 8-F will still be INTs 70h-77h, 
                                            ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    unless you tell the Slave PIC to put them elsewhere as well. 
    "
*/
void init_pic(uint8_t offset1, uint8_t offset2);
void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();