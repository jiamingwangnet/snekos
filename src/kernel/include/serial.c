#include "serial.h"

unsigned char chr = '\0';

void serial_char(char c)
{
    chr = c;
    __asm__("mov eax, chr");
    serial_com1();
}

void serial_str(char* str)
{
    for(; *str != 0; str++)
    {
        chr = *str;
        __asm__("mov eax, chr");
        serial_com1();
    }
}