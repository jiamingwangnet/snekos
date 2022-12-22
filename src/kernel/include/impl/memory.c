#include "../memory.h"

extern uint64_t page_table_l2;

inline void* phys_to_virt(void* addr)
{
    if((uint64_t)addr > KERNEL_VOFFSET) return addr;
    return addr + KERNEL_VOFFSET;
}

inline void* virt_to_phys(void* addr)
{
    if((uint64_t)addr < KERNEL_VOFFSET) return addr;
    return addr - KERNEL_VOFFSET;
}

inline uint64_t* get_page_table()
{
    return &page_table_l2;
}

void map_address(void* addr, void* vaddr)
{   
    uint64_t* table = get_page_table();
    uint16_t i = (uint16_t)((uint64_t)vaddr * 2 >> 22);
    table[i] = ((uint64_t)addr) | 0b10000011;

    __asm__("invlpg %0" : : "m"(i));
}