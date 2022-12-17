#pragma once

#include "types.h"

#define KERNEL_VOFFSET 0xFFFFFFFF80000000

uint64_t phys_to_virt(uint64_t addr);
uint64_t virt_to_phys(uint64_t addr);

void map_address(uint64_t addr, uint16_t page);



