#include "../multiboot_init.h"
#include "../memory.h"

extern uint64_t multiboot_info;
extern uint64_t page_table_l2;

void map_framebuffer(uint64_t framebuffer, uint64_t screen_size)
{
    for(uint64_t i = 0; i * 0x200000 < screen_size; i++)
    {
        map_address(framebuffer + i * 0x200000, 488 + i);
    }
}

uint64_t get_info_addr() 
{
    if (multiboot_info & 7)
    {
        return -1;
    }

    return multiboot_info & 0xffffffff;
}

framebuffer_tag* get_framebuffer_tag()
{
    uint64_t addr = get_info_addr();
    for(LOOP_TAGS(tag, addr))
    {
        if(tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER)
        {
            return (framebuffer_tag*) tag;
        }
    }
    return 0;
}

void init_framebuffer()
{
    tagfb = get_framebuffer_tag();
    uint64_t addr = get_info_addr();

    if(tagfb != 0)
    {
        SCRN_HEIGHT = tagfb->common.framebuffer_height;
        SCRN_WIDTH = tagfb->common.framebuffer_width;

        map_framebuffer(tagfb->common.framebuffer_addr, 
            tagfb->common.framebuffer_width * tagfb->common.framebuffer_height * tagfb->common.framebuffer_bpp);
    }
}