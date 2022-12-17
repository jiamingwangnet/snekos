#include "../memory.h"

extern uint64_t page_table_l2;

uint64_t phys_to_virt(uint64_t addr)
{
    if(addr > KERNEL_VOFFSET) return addr;
    return addr + KERNEL_VOFFSET;
}

uint64_t virt_to_phys(uint64_t addr)
{
    if(addr < KERNEL_VOFFSET) return addr;
    return addr - KERNEL_VOFFSET;
}

void map_address(uint64_t addr, uint16_t page)
{
    addr |= 0b10000011;
    *(uint64_t*)(phys_to_virt((uint64_t)&page_table_l2) + 8 * page) = addr;
    __asm__("invlpg [0]");
}