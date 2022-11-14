#include "../multiboot_init.h"

extern uint32_t multiboot_info;
extern uint32_t page_table_l2;

void map_framebuffer(uint32_t framebuffer, uint32_t screen_size)
{
    framebuffer |= 0b10000011;
    for(uint32_t i = 0; i * 0x200000 < screen_size; i++)
    {
        *(uint32_t*)((uint32_t)&page_table_l2 + 8 * (488 + i)) = framebuffer + i * 0x200000;
    }
    __asm__("invlpg [0]");
}

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
        map_framebuffer(fbtag->common.framebuffer_addr, 
            fbtag->common.framebuffer_width * fbtag->common.framebuffer_height * fbtag->common.framebuffer_bpp);
    }
}