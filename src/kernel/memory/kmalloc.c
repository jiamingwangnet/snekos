#include "../include/memory/kmalloc.h"

mem_block_t *heap_start;
mem_block_t *heap_end;

// const uint8_t heap_start_addr[0x400000]; //  reserve memory

void init_heap()
{
    heap_start = (mem_block_t*)((uint64_t)&_kernel_end + PADDING);
    heap_start->is_free = true;
    heap_start->next = NULL;
    heap_start->prev = NULL;
    heap_start->size = INITIAL_SIZE + sizeof(mem_block_t);
    heap_end = heap_start;
}

void *kmalloc(size_t size)
{
    if(size == 0) return NULL;

    size = align(size);
    mem_block_t *current = heap_start;
    size_t real_size = size + sizeof(mem_block_t);

    while(current != NULL)
    {
        if(current->is_free && real_size <= current->size)
        {
            if(current->size - real_size > MINIUM_SIZE)
            {
                split_block(current, real_size);
                current->is_free = false;
                current->size = real_size;
            }
            else
            {
                current->is_free = false;
            }

            return (void*)current + sizeof(mem_block_t);
        }

        if(current == heap_end)
            expand_heap(real_size);

        current = current->next;
    }

    return NULL;
}

void kfree(void *ptr)
{
    if(ptr == NULL || ptr < heap_start || ptr > heap_end) return;

    mem_block_t *current = heap_start;
    while(current != NULL)
    {
        if((void*)current + sizeof(mem_block_t) == ptr)
        {
            current->is_free = true;
            merge_blocks(current);
            return;
        }
        current = current->next;
    }
}

size_t align(size_t size)
{
    return (size / ALIGNMENT + 1) * ALIGNMENT;
}

void expand_heap(size_t size)
{
    uint64_t real_heap_end = (uint64_t)heap_end + heap_end->size;
    size_t required_pages = size / PAGE_SIZE + 1;

    // TODO:if heap end is out of the mapped memory, map more pages
    // char addr[16];
    // itoa(real_heap_end, addr, 16);
    // serial_str(addr);

    mem_block_t *new_block = (mem_block_t*)real_heap_end;
    new_block->prev = heap_end;
    new_block->next = NULL;

    new_block->size = required_pages * PAGE_SIZE;
    new_block->is_free = true;

    heap_end->next = new_block;
    heap_end = new_block;
}

mem_block_t *split_block(mem_block_t *block, size_t size)
{
    mem_block_t* new_block = (mem_block_t*)((void*)block + size);
    new_block->is_free = true;
    new_block->size = block->size - size;

    new_block->next = block->next;
    new_block->prev = block;

    if(block->next != NULL)
    {
        block->next->prev = new_block;
    }
    block->next = new_block;

    block->size = size;

    if(block == heap_end)
        heap_end = new_block;

    return new_block;
}

void merge_blocks(mem_block_t *target)
{
    if(target->next == NULL && target->prev == NULL)      return;
    if(    target->next ? !target->next->is_free : true 
        && target->prev ? !target->prev->is_free : true )  return;
    
    if(target->prev != NULL && target->prev->is_free)
        target = target->prev;

    target->size = target->size + target->next->size;

    if(target->next->next != NULL)
    {
        target->next = target->next->next;
        target->next->prev = target;
    }
    else
    {
        target->next = NULL;
    }

    if(target->next != NULL && target->next->is_free)
    {
        merge_blocks(target);
    }
}

#ifdef DEBUG_LOG

void serial_print_blocks()
{
    char cheader[16];
    itoa(sizeof(mem_block_t), cheader, 10);
    serial_str("\n\nsize of header: ");
    serial_str(cheader);
    serial_char('\n');

    mem_block_t *it = heap_start;
    while(it != NULL)
    {
        char caddr[16];
        char csize[16];
        char cprev[16];
        char cnext[16];

        itoa(it->size, csize, 10);
        itoa((uint32_t)it > 0 ? (uint32_t)it - 0x80000000 : (uint32_t)it, caddr, 16);
        itoa((uint32_t)it->prev > 0 ? (uint32_t)it->prev - 0x80000000 : (uint32_t)it->prev, cprev, 16);
        itoa((uint32_t)it->next > 0 ? (uint32_t)it->next - 0x80000000 : (uint32_t)it->next, cnext, 16);
        
        serial_str("\naddr: 0x");
        serial_str(caddr);
        serial_char('\n');

        serial_str("size: ");
        serial_str(csize);
        serial_char('\n');

        serial_str("free: ");
        serial_str(it->is_free ? "true" : "false");
        serial_char('\n');

        serial_str("prev: 0x");
        serial_str(cprev);
        serial_char('\n');

        serial_str("next: 0x");
        serial_str(cnext);
        serial_char('\n');

        it = it->next;
    }
}

#endif