#include "../include/stdlib/stdlib.h"

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

// https://www.techiedelight.com/implement-atoi-function-c-iterative-recursive/
int atoi(const char *str)
{
    int num = 0;
    int i = 0, sign = 1;

    // skip white space characters
    while (str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
        i++;
    }

    // note sign of the number
    if (str[i] == '+' || str[i] == '-')
    {
        if (str[i] == '-') {
            sign = -1;
        }
        i++;
    }

    // run till the end of the string is reached, or the
    // current character is non-numeric
    while (str[i] && (str[i] >= '0' && str[i] <= '9'))
    {
        num = num * 10 + (str[i] - '0');
        i++;
    }

    return sign * num;
}

void *memcpy(void *dest, const void *src, size_t size)
{
    char *tmp = (char*)dest;
    char *s = (char*)src;

    while(size--)
        *tmp++ = *s++;
    return dest;
}

void *memset(void *dest, int ch, size_t count)
{
    uint8_t *tmp = (uint8_t*)dest;
    while(count--)
        *tmp++ = ch;
    return dest;
}

// https://stackoverflow.com/questions/20004458/optimized-strcmp-implementation
int strcmp(const char *s1, const char *s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// void *memcpy(void *dest, const void *src, size_t count)
// {
//          if(count % 64 == 0) return memcpy64(dest, src, count);
//     else if(count % 32 == 0) return memcpy32(dest, src, count);
//     else if(count % 16 == 0) return memcpy16(dest, src, count);
//     else                     return memcpy8 (dest, src, count);
// }