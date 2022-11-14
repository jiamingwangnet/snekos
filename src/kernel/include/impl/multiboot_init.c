#include "../multiboot_init.h"

extern uint32_t multiboot_info;
extern uint32_t page_table_l2;
extern uint32_t framebuffer;
extern uint32_t screen_size;
extern void map_framebuffer(void);

uint32_t get_info_addr() 
{
    if (multiboot_info & 7)
    {
        return -1;
    }

    return multiboot_info & 0xffffffff;
}

framebuffer_tag* get_framebuffer_tag()
{
    uint32_t addr = get_info_addr();
    for(LOOP_TAGS(tag, addr))
    {
        if(tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER)
        {
            return (framebuffer_tag*) tag;
        }
    }
    return 0;
}

void init_framebuffer(framebuffer_tag* fbtag)
{
    uint32_t addr = get_info_addr();
    if(fbtag != 0)
    {
        framebuffer = fbtag->common.framebuffer_addr;
        screen_size = fbtag->common.framebuffer_width * fbtag->common.framebuffer_height * fbtag->common.framebuffer_bpp;
        map_framebuffer();
    }
}