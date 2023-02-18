#include "include/stdlib/types.h"
#include "include/io/serial.h"
#include "include/stdlib/stdlib.h"
#include "include/multiboot/multiboot_init.h"
#include "include/graphics/graphics.h"
#include "include/interrupts/idt.h"
#include "include/drivers/keyboard.h"
#include "include/drivers/timer.h"
#include "include/graphics/font.h"
#include "include/memory/kmalloc.h"
#include "include/drivers/pci.h"
#include "console/apps/snake.h"

void kernel_main()
{
    init_multiboot();

    init_serial();
    init_idt();
    init_font();

    init_timer();
    init_heap();

    init_graphics();
    init_keyboard();

    pci_check_all_busses();

    while(true)
    {
        main();
    }
}