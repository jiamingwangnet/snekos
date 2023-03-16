#include "../include/multiboot/multiboot_init.h"
#include "../include/memory/memory.h"

#ifdef DEBUG_LOG
#include "../include/io/serial.h"
#include "../include/stdlib/stdlib.h"
#endif

extern uint64_t multiboot_info;
extern framebuffer_tag* tagfb;

uint32_t mem_lower;
uint32_t mem_upper;

inline uint64_t get_info_addr() 
{
    if (multiboot_info & 7)
    {
        return -1;
    }

    return multiboot_info & 0xffffffff;
}

void init_multiboot()
{
    #ifdef DEBUG_LOG
    serial_str("Multiboot Addr: 0x");
    char caddr[16];
    uint32_t tagaddr = (uint32_t)&multiboot_info;
    itoa(tagaddr >= 0x80000000 ? tagaddr - 0x80000000 : tagaddr, caddr, 16);
    serial_str(caddr);
    serial_char('\n');
    #endif

    uint64_t addr = get_info_addr();
    for(LOOP_TAGS(tag, addr))
    {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
        {
            tagfb = (framebuffer_tag*)tag;
            init_framebuffer();
            break;
        }        
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
        {
            mem_lower = ((struct multiboot_tag_basic_meminfo *) tag)->mem_lower;
            mem_upper = ((struct multiboot_tag_basic_meminfo *) tag)->mem_upper;

            char clower[16];
            char cupper[16];

            itoa(mem_lower, clower, 10);
            itoa(mem_upper, cupper, 10);

            serial_str("Lower: ");
            serial_str(clower);
            serial_str("Kb  Upper: ");
            serial_str(cupper);
            serial_str("Kb\n");

            break;
        }
        case MULTIBOOT_TAG_TYPE_MMAP:
        {
            multiboot_memory_map_t *mmap;

            for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
                 (multiboot_uint8_t *) mmap 
                   < (multiboot_uint8_t *) tag + tag->size;
                 mmap = (multiboot_memory_map_t *) 
                   ((unsigned long) mmap
                    + ((struct multiboot_tag_mmap *) tag)->entry_size))
            {
                #ifdef DEBUG_LOG
                char cbaseaddr[16];
                char clen[16];
                char ctype[16];
                itoa(mmap->addr, cbaseaddr, 16);
                itoa(mmap->len, clen, 10);
                itoa(mmap->type, ctype, 10);

                serial_str("Mmap Entry\n");
                serial_str("base addr: 0x");
                serial_str(cbaseaddr);
                serial_str("\nlength: ");
                serial_str(clen);
                serial_str("\ntype: ");
                serial_str(ctype);
                serial_str("\n\n");
                #endif
            }
            break;
        }
        default:
            break;
        }
    }
}

void map_framebuffer(uint64_t framebuffer, uint64_t screen_size)
{
    for(uint64_t i = 0; i < screen_size; i += 0x200000)
    {
        void *addr = request_page((void*)(framebuffer + i));
        if(i == 0) FRAMEBUFFER = (uint64_t)addr;
    }
}

void init_framebuffer()
{
    if(tagfb != 0)
    {
        SCRN_HEIGHT = tagfb->common.framebuffer_height;
        SCRN_WIDTH = tagfb->common.framebuffer_width;

        map_framebuffer(tagfb->common.framebuffer_addr, 
            tagfb->common.framebuffer_width * tagfb->common.framebuffer_height * (tagfb->common.framebuffer_bpp / 8));
    }
}