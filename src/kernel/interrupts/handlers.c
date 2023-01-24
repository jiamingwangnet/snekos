#include "../include/interrupts/handlers.h"
#include "../include/drivers/keyboard.h"
#include "../include/io/io.h"
#include "../include/io/serial.h"

handler_t handlers[256] = {};
// #define DEBUG_LOG_IRQ
void irq_handler(reg_status_t status)
{
    #ifdef DEBUG_LOG_IRQ
    serial_str("\n\nIrq Handled ");
    char c_id[15];
    itoa(id, c_id, 10);
    serial_str(c_id);
    serial_char('\n');
    #endif

    handler_t handler = handlers[status.interrupt_number];
    if(handler) handler();

    if(status.interrupt_number >= 40) // IRQ 8
        out(PIC2_COMMAND, PIC_EOI);

    out(PIC1_COMMAND, PIC_EOI);
}

void add_handler(uint8_t index, handler_t hdlr)
{
    handlers[index] = hdlr;
}