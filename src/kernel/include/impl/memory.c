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

uint64_t* get_page_table()
{
    return (uint64_t*)(&page_table_l2);
}

void map_address(uint64_t addr, uint64_t vaddr)
{   
    uint64_t* table = get_page_table();
    uint16_t i = (uint16_t)(vaddr * 2 >> 22);
    table[i] = (addr) | 0b10000011;

    __asm__("invlpg %0" : : "m"(i));
}