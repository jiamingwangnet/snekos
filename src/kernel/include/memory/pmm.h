#pragma once

#include "../stdlib/types.h"

#define BLOCK_SIZE 0x1000
#define SETBIT(i) bitmap[(i) / 8] |= (1 << ((i) % 8))
#define CLEARBIT(i) bitmap[(i) / 8] &= ~(1 << ((i) % 8))
#define ISSET(i) ((bitmap[(i) / 8] >> ((i) % 8)) & 0x1)

extern uint64_t total_memory;

void init_pmm();
size_t get_next_block();
void *allocate_block();
void *allocate_size(size_t size);
// void *allocate_size(size_t size); // returns the address of the first block and sets others to in use
void free_block(size_t block);
void free_multi(void *paddr, size_t size);
size_t memory_used();
size_t memory_available();