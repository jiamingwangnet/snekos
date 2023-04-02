#include "../include/memory/memory.h"
#include "../include/memory/pmm.h"
#include "../include/console/console.h"
#include "../include/io/serial.h"

extern uint64_t page_table_l2;
uint64_t *table_end = NULL;

mmap_entry_t memory_map[32];
size_t mmap_len = 0;

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

void *get_phys_mapping(void *vaddr)
{
    uint64_t entry = (uint64_t)vaddr & 0xFFE00000;
    uint64_t paddr = get_page_table()[(entry - TABLE_OFFSET) / PAGE_SIZE];

    return (void*)((paddr & 0xFFE00000) + ((uint64_t)vaddr & ~(0xFFE00000)));
}

inline uint64_t* get_page_table()
{
    return (uint64_t*)((uint64_t)&page_table_l2 + 4096);
}

void *request_page(void *addr)
{
    if(table_end == NULL) table_end = get_page_table();

    if(((uint64_t)table_end - (uint64_t)get_page_table()) / 8 >= 512 * (L2_TABLES - 1)) 
    {
        kprintf("%hOut of pages.%h\n", RED, DEFAULT_FG);
        serial_str("OUT OF PAGES\n");
        return -1;
    }

    *table_end = ((uint64_t)addr) | 0b10000011;
    uint16_t i = (uint16_t)(((uint64_t)table_end - ((uint64_t)&page_table_l2 + 4096)) >> 3);

    __asm__("invlpg %0" : : "m"(i));

    table_end++; // TODO: FIXME: change whatever this is. Actually find the next free memory block instead of incrementing

    return TABLE_OFFSET + 0x200000 * i;
}

void *request_memory(size_t size) //  TODO: contiguous allocation
{
    void *vaddr = NULL;
    void *paddr = allocate_size(size);
    void *aligned = (void*)PAGE_ALIGN((uint64_t)paddr);
    if(paddr == -1) return -1;

    size_t cur_size;
    for(cur_size = 0; cur_size < size; cur_size += PAGE_SIZE)
    {
        void *page = request_page(aligned + cur_size); // change this to find contiguous free entries (2MiB each)
        if(page == -1) return -1;
        if(cur_size == 0) vaddr = page;
    }
    request_page(aligned + cur_size); // just in case the memory is out of bounds of the mapping
    
    return (void*)((uint64_t)vaddr + ((uint64_t)paddr & ~(0xFFC00000)));
}

void *request_memory_addr(void *paddr, size_t size)
{
    void *vaddr = NULL;
    void *aligned = (void*)PAGE_ALIGN((uint64_t)paddr);
    if(paddr == -1) return -1;

    size_t cur_size;
    for(cur_size = 0; cur_size < size; cur_size += PAGE_SIZE)
    {
        void *page = request_page(aligned + cur_size); // change this to find contiguous free entries (2MiB each)
        if(cur_size == 0) vaddr = page;
    }
    request_page(aligned + cur_size); // just in case the memory is out of bounds of the mapping

    return (void*)((uint64_t)vaddr + ((uint64_t)paddr & ~(0xFFC00000)));
}

// absolutely useless function i might not even need it
void free_memory(void *addr, size_t size)
{
    uint64_t aligned = PAGE_ALIGN((uint64_t)addr);
    size_t start = (size_t)(aligned - TABLE_OFFSET) / PAGE_SIZE;
    size_t lim = size / PAGE_SIZE + 1;
    uint64_t *table = get_page_table();
    free_multi(get_phys_mapping(addr), size);

    for(size_t i = 0; i < lim; i++)
    {
        table[i] = 0;
    }

    // TODO: update the next free entry pointer after adding contiguous allocation
}

void map_address(void* addr, void* vaddr)
{   
    if((uint64_t)vaddr < TABLE_OFFSET) return;

    uint64_t* table = get_page_table();
    uint16_t i = (uint16_t)((uint64_t)vaddr * 2 >> 22);
    table[i] = ((uint64_t)addr) | 0b10000011;

    __asm__("invlpg %0" : : "m"(i));
}