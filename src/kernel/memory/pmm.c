#include "../include/memory/pmm.h"
#include "../include/memory/memory.h"
#include "../include/memory/kmalloc.h"
#include "../include/stdlib/stdlib.h"
#include "../include/multiboot/multiboot_init.h"

#include "../include/console/console.h"

extern uint32_t mem_lower;
extern uint64_t _kernel_start;
extern uint64_t _kernel_end;
uint64_t base_addr;
uint8_t *bitmap;
size_t total_blocks;
mmap_entry_t entry;

size_t next_block = 0;

uint64_t total_memory;

void init_pmm()
{
    for(size_t i = 0; i < mmap_len; i++)
    {
        entry = memory_map[i];
        if((entry.end - entry.base) / 1024 > mem_lower && entry.available) // find a available entry that is not below the min memory limit
            break;
    }

    entry.base = ALIGN_ADDR(entry.base, 0x10);
    entry.end = ALIGN_ADDR(entry.end, 0x10);

    size_t size = (entry.end - entry.base) / BLOCK_SIZE;
    if(size * BLOCK_SIZE < entry.end - entry.base) size++;
    size = size / 8 + (size_t)(size % 8 < 8);

    bitmap = (uint8_t*)ALIGN_ADDR(((uint64_t)&_kernel_end + 0x10), 0x10);
    memset((void*)bitmap, 0, size);

    base_addr = entry.base;

    total_blocks = size * 8;

    // set kernel code blocks to taken
    uint64_t kstart = (uint64_t)virt_to_phys((void*)&_kernel_start);
    uint64_t kend = (uint64_t)virt_to_phys((void*)&_kernel_end);

    for(; kstart < kend; kstart += BLOCK_SIZE)
    {
        SETBIT((kstart - entry.base) / BLOCK_SIZE);
    }

    for(size_t mbstart = get_info_addr(), mbend = mbstart + header_size; mbstart < mbend; mbstart += BLOCK_SIZE)
    {
        SETBIT((mbstart - entry.base) / BLOCK_SIZE);
    }

    for(; next_block < total_blocks; next_block++)
    {
        if(!ISSET(next_block))
            break;
    }

    total_memory = entry.end - entry.base;

    // for(uint64_t bitstart = (uint64_t)bitmap, bitend = bitstart + size; bitstart < bitend; bitstart += BLOCK_SIZE) // bitmap
    // {
    //     SETBIT((bitstart - entry.base) / BLOCK_SIZE);
    // }

    // for(size_t i = 0; i < total_blocks / 8; i++)
    // {
    //     char cbyte[9];
    //     itoa(bitmap[i], cbyte, 2);
    //     serial_str(cbyte);
    //     serial_char(' ');
    // }
}
/*
TODO: faster allocator
keep track of the next block using a number
increase the number when a new block is allocated
    if the next block is set, keep looping until a free block is found
when freeing memory, check if the freed block's number is smaller than the tracker number
    if it is, then set the tracter number to the freed block

Best case: O(1)
Worst case: O(n)
*/
size_t get_next_block()
{
    for(; next_block < total_blocks; next_block++)
    {
        if(!ISSET(next_block))
            break;
    }

    if(next_block < total_blocks) return next_block;

    kprintf("%hOut of memory.%h\n", RED, DEFAULT_FG);
    serial_str("OUT OF MEMORY\n");
    return (size_t)-1;
}

size_t memory_used() // in bytes
{
    size_t blocks = 0;

    size_t block = 0;
    for(; block < total_blocks; block++)
    {
        if(ISSET(block))
            blocks++;
    }
    return blocks * BLOCK_SIZE;
}

size_t memory_available() // in bytes
{
    size_t blocks = 0;

    size_t block = 0;
    for(; block < total_blocks; block++)
    {
        if(!ISSET(block))
            blocks++;
    }
    return blocks * BLOCK_SIZE;
}

/*
TODO:

Put flags in free memory areas and search for a flag that is big enough to fit the required size
*/
void *allocate_size(size_t size) // TODO: Make faster, very inefficient
{
    size_t blocks = size / BLOCK_SIZE + 1;

    size_t free_counter = 0;
    for(size_t b = get_next_block(); b < total_blocks; b++)
    {
        if(ISSET(b))
            free_counter = 0;
        else
            free_counter++;
        
        if(free_counter == blocks)
        {
            for(size_t c = b - (blocks - 1); c < blocks + (b - (blocks - 1)); c++)
            {
                SETBIT(c);
            }

            return (void*)(b * BLOCK_SIZE + entry.base);
        }
    }

    kprintf("%hOut of memory.%h\n", RED, DEFAULT_FG);
    serial_str("OUT OF MEMORY\n");
    return (size_t)-1;
}

void *allocate_block() // TODO: implement fast multiple block allocation
{
    size_t next = get_next_block();
    if(next == -1) return -1;
    SETBIT(next);

    return (void*)(next * BLOCK_SIZE + entry.base);
}

void free_block(size_t block) // TODO: implement fast clearing for many blocks
{
    CLEARBIT(block);

    if(block < next_block)
        next_block = block;
}

void free_multi(void *paddr, size_t size)
{
    size_t blocks = size / BLOCK_SIZE + 1;
    size_t block = (size_t)(((uint64_t)paddr - entry.base) / BLOCK_SIZE);

    for(size_t b = 0; b < blocks; b++)
    {
        free_block(b + block);
    }
}