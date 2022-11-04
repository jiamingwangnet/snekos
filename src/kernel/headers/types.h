#pragma once

#ifndef __SIZE_TYPE__
    #define __SIZE_TYPE__ unsigned long long
#endif

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef __SIZE_TYPE__ size_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;
