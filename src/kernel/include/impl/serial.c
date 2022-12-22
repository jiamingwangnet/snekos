#include "../serial.h"

int init_serial()
{
    out(COM1 + 1, 0x00);    // Disable all interrupts
    out(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    out(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    out(COM1 + 1, 0x00);    //                  (hi byte)
    out(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    out(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    out(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    out(COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
    out(COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if(in(COM1 + 0) != 0xAE) {
       return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    out(COM1 + 4, 0x0F);
    return 0;
}

int is_transmit_empty()
{
    return in(COM1 + 5) & 0x20;
}

int serial_received()
{
    return in(COM1 + 5) & 1;
}

void serial_char(char c)
{
    while(is_transmit_empty() == 0);
    out(COM1, c);
}

void serial_str(const char* str)
{
    for(; *str != 0; str++)
    {
        serial_char(*str);
    }
}

uint8_t iserial_char()
{
    while(serial_received() == 0);

    return in(COM1);
}