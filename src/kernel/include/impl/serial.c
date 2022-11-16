#include "../serial.h"

void serial_char(char c)
{
    out(COM1, c);
}

void serial_str(char* str)
{
    for(; *str != 0; str++)
    {
        out(COM1, *str);
    }
}