#pragma once

#ifndef __SIZE_TYPE__
    #define __SIZE_TYPE__ unsigned long long
#endif

#include <stdint.h>
#include "multiboot2.h"

typedef __SIZE_TYPE__ size_t;
typedef struct multiboot_tag_framebuffer framebuffer_tag;
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;
