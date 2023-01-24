#pragma once

#include "../stdlib/types.h"

// allocatable memory begins at the end of the kernel + padding
extern uint64_t _kernel_end;

#define MINIUM_SIZE 0x20
#define PADDING 0x1000
#define PAGE_SIZE 0x200000
#define INITIAL_SIZE 0x200000

#define ALIGNMENT 0x10

typedef struct mem_block_t
{
    uint64_t size; // includes header size
    bool is_free;
    // prev = left 
    // next = right
    struct mem_block_t *prev; // point to NULL if it is the first block
    struct mem_block_t *next;
} mem_block_t;

// sets the size to a multiple of a value (currently 16)
size_t align(size_t size);

void init_heap();

// internal block management functions

mem_block_t *split_block(mem_block_t *block, size_t size);
void merge_blocks(mem_block_t *target);
void expand_heap(size_t size);

// C allocation functions
void *kaligned_alloc(size_t alignment, size_t size);
void *kcalloc(size_t nmemb, size_t size);
void kfree(void *ptr);
void *kmalloc(size_t size);
void *krealloc(void *ptr, size_t size);

#ifdef DEBUG_LOG

void serial_print_blocks();

#endif