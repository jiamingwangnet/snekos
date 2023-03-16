#pragma once

#include "../stdlib/types.h"

#define KERNEL_VOFFSET 0xFFFFFFFF80000000
#define TABLE_OFFSET 0x40000000
#define PAGE_ALIGN(addr) (addr & 0xFFC00000)
#define ALIGN_ADDR(addr, bound) ((addr + bound - 1) & ~(bound - 1))

void* phys_to_virt(void* addr);
void* virt_to_phys(void* addr);

uint64_t* get_page_table();
void *request_page(void *addr);
void map_address(void* addr, void* vaddr);



