#pragma once

#include "types.h"

#define KERNEL_VOFFSET 0xFFFFFFFF80000000

void* phys_to_virt(void* addr);
void* virt_to_phys(void* addr);

uint64_t* get_page_table();
void map_address(void* addr, void* vaddr);



