#include "../include/memory/memory.h"

extern uint64_t page_table_l2_high;
uint64_t *table_end = NULL;

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
    return &page_table_l2_high;
}

void *request_page(void *addr)
{
    if(table_end == NULL) table_end = get_page_table();

    *table_end = ((uint64_t)addr) | 0b10000011;
    uint16_t i = (uint16_t)(((uint64_t)table_end - (uint64_t)&page_table_l2_high) >> 3);

    __asm__("invlpg %0" : : "m"(i));

    table_end++;

    return TABLE_OFFSET + 0x200000 * i;
}

void map_address(void* addr, void* vaddr)
{   
    if((uint64_t)vaddr < TABLE_OFFSET) return;

    uint64_t* table = get_page_table();
    uint16_t i = (uint16_t)((uint64_t)vaddr * 2 >> 22);
    table[i] = ((uint64_t)addr) | 0b10000011;

    __asm__("invlpg %0" : : "m"(i));
}