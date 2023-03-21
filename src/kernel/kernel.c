#include "include/stdlib/types.h"
#include "include/io/serial.h"
#include "include/stdlib/stdlib.h"
#include "include/multiboot/multiboot_init.h"
#include "include/graphics/graphics.h"
#include "include/interrupts/idt.h"
#include "include/drivers/keyboard.h"
#include "include/drivers/timer.h"
#include "include/graphics/font.h"
#include "include/console/console.h"
#include "include/memory/kmalloc.h"
#include "include/drivers/pci.h"
#include "include/drivers/ata.h"
#include "include/drivers/ahci.h"
#include "include/drivers/disk.h"

void scrn_test();

extern uint32_t mem_lower;
extern uint32_t mem_upper;

void kernel_main()
{
    init_multiboot();

    init_serial();
    init_idt();

    init_keyboard(); // irq does not get fired if i type before this is initalised

    init_timer();
    init_heap();

    init_graphics();
    init_font();

    scrn_test();
    
    init_console(20, 20, 0xe0ffeb, 0x0f0f0f);

    serial_print_blocks();
    kprintf(
        "%h  ____             _     ___  ____  \n"
        " / ___| _ __   ___| | __/ _ \\/ ___| \n"
        " \\___ \\| '_ \\ / _ | |/ | | | \\___ \\ \n"
        "  ___) | | | |  __|   <| |_| |___) |\n"
        " |____/|_| |_|\\___|_|\\_\\\\___/|____/ \n\n%h",
    GREEN, DEFAULT_FG);

    kprintf("Lower Memory: %h%dKb%h  Upper Memory: %h%dKb%h\n\n", ORANGE, mem_lower, DEFAULT_FG, ORANGE, mem_upper, DEFAULT_FG);

    pci_check_all_busses();
    kprintf("\n");

    init_ahci();
    kprintf("\n");
    
    init_ata();
    kprintf("\n");
    
    // extern uint32_t *color_memory;
    // kprintf("0x%x\n", color_memory);

    enable_input();

    // for(int i = 0; i < 10000; i++) //133033984
    // {
    //     uint8_t *byte = (uint8_t*)0x100000;
    //     byte[i] = 0;
    // }
    
    bool right = true;
    for(uint32_t x = 0;;)
    {
        console_loop();
        // draw_rect_aligned(0, 10, SCRN_WIDTH, SCRN_HEIGHT - 10, 0x4da5d1);
        if(get_time() % (1000/60) == 0)
        {        
            // clear path
            draw_rect(0,0, SCRN_WIDTH, 10, 0x303030);
            draw_rect(x, 0, 10, 10, 0x28c77a);
            if(right)
                x++;
            else
                x--;
            if(x + 10 >= SCRN_WIDTH)
                right = false;
            else if(x <= 0)
                right = true;
           
            update_buffer();
        }
    }
}

void scrn_test()
{
    fill_screen(0xffffff);

    // draw smile

    // mouth
    draw_rect(SCRN_WIDTH/2 - 250, SCRN_HEIGHT/2 + 110, 500, 50, 0);
    draw_rect(SCRN_WIDTH/2 - 250, SCRN_HEIGHT/2 + 10, 50, 100, 0);
    draw_rect(SCRN_WIDTH/2 + 200, SCRN_HEIGHT/2 + 10, 50, 100, 0);

    // eyes
    draw_rect(SCRN_WIDTH/2 - 130, 200, 60, 120,0);
    draw_rect(SCRN_WIDTH/2 + 70, 200, 60, 120, 0);

    update_buffer();

    wait_ticks(250);

    fill_screen(0x0f0f0f);
    draw_str(20, 20, 0xe0ffeb,0xf0f0f,
                    "Hello, World!\n"
                     "TEST CMD!!!\n"
                     "The Quick Brown Fox Jumps Over The Lazy Dog\n"
                     "the quick brown fox fox jumps over the lazy dog.\n"
                     "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.\n"
                     "1234567890\n"
                     "~!@#$%^&*()_+<>[]{}");
    
    update_buffer();

    wait_ticks(150);
}