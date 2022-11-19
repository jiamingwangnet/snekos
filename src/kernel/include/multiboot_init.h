#pragma once

#include "multiboot2.h"
#include "types.h"
#include "graphics.h"

#define LOOP_TAGS(tag, addr) struct multiboot_tag *tag = (struct multiboot_tag *)(addr + 8);tag->type != MULTIBOOT_TAG_TYPE_END; \
                    tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7))

void map_framebuffer(uint32_t addr, uint32_t screen_size);
uint32_t get_info_addr();
framebuffer_tag* get_framebuffer_tag();
void init_framebuffer();