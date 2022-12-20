#include "../stdlib.h"

int itoa(int value, char *sp, int radix)
{
    char tmp[16];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);    
    if (sign)
        v = -value;
    else
        v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix;
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign) 
    {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    *sp = '\0';

    return len;
}

void *memcpy(void *dest, const void *src, size_t count)
{
         if(count % 64 == 0) return memcpy64(dest, src, count);
    else if(count % 32 == 0) return memcpy32(dest, src, count);
    else if(count % 16 == 0) return memcpy16(dest, src, count);
    else                     return memcpy8 (dest, src, count);
}