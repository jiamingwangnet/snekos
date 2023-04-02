#pragma once

#include "../stdlib/types.h"

#define KERNEL_VOFFSET 0xFFFFFFFF80000000
#define TABLE_OFFSET 0x40000000
#define PAGE_ALIGN(addr) (addr & 0xFFC00000)
#define ALIGN_ADDR(addr, bound) ((addr + bound - 1) & ~(bound - 1))
#define PAGE_SIZE 0x200000
#define L2_TABLES 4

extern uint32_t mem_lower;
extern uint32_t mem_upper;

typedef struct 
{
    uint64_t base;
    uint64_t end;
    bool available;
} mmap_entry_t;

extern mmap_entry_t memory_map[32];
extern size_t mmap_len;

void* phys_to_virt(void* addr);
void* virt_to_phys(void* addr);
void *get_phys_mapping(void *vaddr);

uint64_t* get_page_table();
void *request_page(void *addr);
void *request_memory(size_t size);
void *request_memory_addr(void *paddr, size_t size);
void free_memory(void *addr, size_t size);
void map_address(void* addr, void* vaddr);



