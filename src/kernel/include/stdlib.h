#pragma once

#include "types.h"

int itoa(int value, char *sp, int radix);
int atoi(const char *str);
int strcmp (const char* str1, const char* str2);
void* memcpy(void* dest, const void* src, size_t count);
void *memset(void *dest, int ch, size_t count);


// optimisations
extern void *memcpy64(void *dest, const void *src, size_t size);
extern void *memcpy32(void *dest, const void *src, size_t size);
extern void *memcpy16(void *dest, const void *src, size_t size);
extern void *memcpy8 (void *dest, const void *src, size_t size);