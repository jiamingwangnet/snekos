#include "../include/multiboot/multiboot_init.h"
#include "../include/memory/memory.h"

extern uint64_t multiboot_info;
extern framebuffer_tag* tagfb;

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
        default:
            break;
        }
    }
}

void map_framebuffer(uint64_t framebuffer, uint64_t screen_size)
{
    for(uint64_t i = 0; i < screen_size; i += 0x200000)
    {
        map_address((void*)(framebuffer + i), (void*)(FRAMEBUFFER + i));
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