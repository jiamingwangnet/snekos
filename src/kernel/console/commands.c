#include "../include/console/commands.h"
#include "../include/stdlib/stdlib.h"
#include "../include/console/console.h"
#include "../include/graphics/graphics.h"
#include "../include/drivers/timer.h"
#include "../include/memory/kmalloc.h"
#include "../include/drivers/pci.h"
#include "apps/snake.h"
#include "../include/drivers/disk.h"

CREATE_COMMAND(hello, {
    // kprintf("Hello There!\n");
})

CREATE_COMMAND(add, {
    if(argc < 2)
    {
        kprintf("%hError: requires at least 2 numbers%h\n", RED, DEFAULT_FG);
        return;
    }

    int sum = atoi(argv[0]);
    for(int i = 1; i < argc; i++)
    {
        sum += atoi(argv[i]);
    }

    kprintf("%d\n", sum);
})

CREATE_COMMAND(sub, {
    if(argc < 2)
    {
        kprintf("%hError: requires at least 2 numbers%h\n", RED, DEFAULT_FG);
        return;
    }

    int sum = atoi(argv[0]);
    for(int i = 1; i < argc; i++)
    {
        sum -= atoi(argv[i]);
    }

    kprintf("%d\n", sum);
})

CREATE_COMMAND(mul, {
    if(argc < 2)
    {
        kprintf("%hError: requires at least 2 numbers%h\n", RED, DEFAULT_FG);
        return;
    }

    int sum = atoi(argv[0]);
    for(int i = 1; i < argc; i++)
    {
        sum *= atoi(argv[i]);
    }

    kprintf("%d\n", sum);
})

CREATE_COMMAND(div, {
    if(argc < 2)
    {
        kprintf("%hError: requires at least 2 numbers%h\n", RED, DEFAULT_FG);
        return;
    }

    int sum = atoi(argv[0]);
    for(int i = 1; i < argc; i++)
    {
        sum /= atoi(argv[i]);
    }
    
    kprintf("%d\n", sum);
})

CREATE_COMMAND(print, {
    for(int i = 0; i < argc; i++)
    {
        kprintf("%s ", argv[i]);
    }
    kprintf("\n");
})

extern uint32_t max_rows;
extern uint32_t y;
extern uint32_t line_pad;

CREATE_COMMAND(setrow, {
    uint32_t size = atoi(argv[0]);
    if(strcmp(argv[0], "fit") == 0)
    {
        PSF1_font *font = get_font();
        max_rows = (tagfb.common.framebuffer_height - y) / (font->charsize + line_pad) - 1;
        return;
    }
    else if(size > 0)
    {
        max_rows = size;
        return;
    }
    kprintf("%hRow must be a number greater than 0 or \"fit\"%h\n", RED, DEFAULT_FG);
})

extern uint32_t max_cols;
extern uint32_t x;
extern uint32_t col_pad;

CREATE_COMMAND(setcol, {
    uint32_t size = atoi(argv[0]);
    if(strcmp(argv[0], "fit") == 0)
    {
        max_cols = (tagfb.common.framebuffer_width - x) / (PSF1_WIDTH + col_pad) - 1;
        return;
    }
    else if(size > 0)
    {
        max_cols = size;
        return;
    }
    kprintf("%hColumn must be a number greater than 0 or \"fit\"%h\n", RED, DEFAULT_FG);
})

extern uint32_t* B_BUFFER;

CREATE_COMMAND(scrninfo, {
    kprintf("%h----Screen Information----%h\n", DODGERBLUE, DEFAULT_FG);

    kprintf("Framebuffer address: %h0x%x%h\n", ORANGE, tagfb.common.framebuffer_addr, DEFAULT_FG);
    kprintf("Backbuffer address: %h0x%x%h\n", ORANGE, (uint64_t)B_BUFFER - 0xffffffff80000000, DEFAULT_FG);

    kprintf("Width: %h%d\n%h", ORANGE, tagfb.common.framebuffer_width, DEFAULT_FG);

    kprintf("Height: %h%d\n%h", ORANGE, tagfb.common.framebuffer_height, DEFAULT_FG);

    kprintf("BPP: %h%d\n%h", ORANGE, tagfb.common.framebuffer_bpp, DEFAULT_FG);
})

CREATE_COMMAND(checksse, {
    uint32_t sse_enabled = 0;
    __asm__ volatile(
        "   mov eax, 0x1\n"
        "   cpuid\n"
        "   and edx, 1 << 25\n"
        "   mov %0, edx"
        : "=r"(sse_enabled)
    );
    
    if(sse_enabled)
        kprintf("SSE %his%h available.\n", GREEN, DEFAULT_FG);
    else
        kprintf("SSE %hnot%h available.\n", RED, DEFAULT_FG);
})

CREATE_COMMAND(clear, {
    clear();
})

CREATE_COMMAND(memdump, { // TODO: change to 64bit
    if(argc != 3)
    {
        kprintf("%hError: memdump requires 3 arguments%h\n", RED, DEFAULT_FG);
        return;
    }

    size_t bytes = atoi(argv[0]);
    size_t columns = atoi(argv[1]);
    uint32_t address = atoi(argv[2]);

    set_color(ORANGE);
    for(size_t b = 0; b < bytes; b++)
    {
        uint8_t byte = *(uint8_t*)(address + b * sizeof(uint8_t));
        char cbyte[3];
        itoa(byte, cbyte, 16);

        if(byte < 0x10)
        {
            cbyte[1] = cbyte[0];
            cbyte[0] = '0';
        }

        kprintf(cbyte);
        kprintf(" ");;

        if((b+1) % columns == 0)
        {
            kprintf("\n");
        }
    }
    set_color(DEFAULT_FG);
    kprintf("\n");
})

CREATE_COMMAND(timepit, {
    kprintf("The PIT time is: %h%d%h\n", ORANGE, get_time(), DEFAULT_FG);
})

CREATE_COMMAND(write, {
    if(argc != 3)
    {
        kprintf("%hMust have 3 arguments: size, data, destination%h\n", RED, DEFAULT_FG);
        return;
    }

    if(strcmp(argv[0], "byte"))
        *(uint8_t*)atoi(argv[2]) = (uint8_t)atoi(argv[1]);
    else if(strcmp(argv[0], "word"))
        *(uint16_t*)atoi(argv[2]) = (uint16_t)atoi(argv[1]);
    else if(strcmp(argv[0], "dword"))
        *(uint32_t*)atoi(argv[2]) = (uint32_t)atoi(argv[1]);
    else
    {
        kprintf("%hMust be a valid size below QWORD%h\n", RED, DEFAULT_FG);
        return;
    }

    kprintf("Data written to %h0x%x%h\n", ORANGE, atoi(argv[2]), DEFAULT_FG);
})

CREATE_COMMAND(malloc, {
    if(argc != 1)
    {
        kprintf("%hError: must have only 1 argument%h\n", RED, DEFAULT_FG);
        return;
    }

    uint64_t addr = (uint64_t)kmalloc(atoi(argv[0])) - 0xffffffff80000000;

    kprintf("Allocated memory at: %h%d%h (%h0x%x%h)\n", ORANGE, addr, DEFAULT_FG, ORANGE, addr, DEFAULT_FG);
})

CREATE_COMMAND(free, {
    if(argc != 1)
    {
        kprintf("%hError: must have only 1 argument%h\n", RED, DEFAULT_FG);
        return;
    }

    void *addr = (void*)atoi(argv[0]);
    kfree(addr);

    kprintf("Freed memory at %h0x%x%h\n", ORANGE, addr, DEFAULT_FG);
})

CREATE_COMMAND(call, {
    if(argc != 1)
    {
        kprintf("%hError: must have only 1 argument%h\n", RED, DEFAULT_FG);
        return;
    }
    
    uint64_t addr = atoi(argv[0]) + 0xffffffff80000000;
    __asm__ volatile("call %0" : : "g"(addr));
})

CREATE_COMMAND(help, {
    kprintf(
        // BASELINE:                      |
        "%hhello%h\t                          Prints out \"Hello There!\".\n"
        "\n"
        "%hadd%h [nums ...]%h\t                 Adds the specified numbers together.\n"
        "%hsub %h[nums ...]%h\t                 Subtracts from the first number.\n"
        "%hmul %h[nums ...]%h\t                 Multiplies the specified numbers together.\n"
        "%hdiv %h[nums ...]%h\t                 Divides the numbers.\n"
        "\n"
        "%hprint %h[str ...]%h\t                Prints the specified text.\n"
        "%hclear%h\t                          Clears the console.\n"
        "\n"
        "%hsetrow %h[row] %h| %hfit%h\t             Sets the console max rows to the specified number, \"fit\" adjusts the console to fit the screen.\n"
        "%hsetcol %h[col] %h| %hfit%h\t             Sets the console max columns to the specified number, \"fit\" adjusts the console to fit the screen.\n"
        "\n"
        "%hscrninfo%h\t                       Prints information about the screen.\n"
        "%hchecksse%h\t                       Checks whether SSE is available.\n"
        "%htimepit%h\t                        Prints out the amount of time the Programmable Interrupt Timer has fired.\n"
        "%hlogpci %h[num]%h\t                   Logs all the PCI devices. The \"num\" argument specifies the number of devices to print.\n"
        "\n"
        "%hmemdump %h[bytes] [col] [addr]%h\t   Dumps the memory at the specified address.\n"
        "%hwrite %h[size] [data] [addr]%h\t     Writes the specified data to the address.\n"
        "%hmalloc %h[bytes]%h\t                 Allocates the specified size of memory and prints the address.\n"
        "%hfree %h[addr]%h\t                    Frees the allocated memory at the specified address.\n"
        "%hcall %h[addr]%h\t                    Calls the function at the specified address.\n"
        "\n"
        // BASELINE:                      |
        "%hrdisk %h[sector] [columns]%h\t       Reads the disk at the specified sector.\n"
        "%hwdisk %h[byte] [sector] [offset]%h\t Writes the byte to the disk at the specified sector + offset in bytes.\n"
        "\n"
        "%hsnake%h\t                          Starts the snake game.\n"
        "\n"
        "%hhelp%h\t                           Displays this help message.\n",
    DODGERBLUE, DEFAULT_FG,

    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,

    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, DEFAULT_FG,

    DODGERBLUE, ORANGE, DEFAULT_FG, PURPLE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG, PURPLE, DEFAULT_FG,

    DODGERBLUE, DEFAULT_FG,
    DODGERBLUE, DEFAULT_FG,
    DODGERBLUE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,

    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,

    DODGERBLUE, ORANGE, DEFAULT_FG,
    DODGERBLUE, ORANGE, DEFAULT_FG,

    GREEN, DEFAULT_FG,

    DODGERBLUE, DEFAULT_FG
    );
})

extern pci_common_t device_list[1024];
extern pci_common_t *list_ptr;
CREATE_COMMAND(logpci, {
    if(argc > 1)
    {
        kprintf("%hMust have 1 or no arguments%h\n", RED, DEFAULT_FG);
        return;
    }

    pci_common_t *device_iter = device_list;

    size_t max;
    size_t i;
    if(argc == 1)
    {
       max = atoi(argv[0]);
        i = 0;
    }
    
    while(device_iter != list_ptr)
    {
        kprintf("%h%s%h\n", DODGERBLUE, pci_get_device_name(device_iter->class_code, device_iter->subclass), DEFAULT_FG);

        kprintf("Vendor ID: %h0x%x%h\n", ORANGE, device_iter->vendor_id, DEFAULT_FG);

        kprintf("Device ID: %h0x%x%h\n", ORANGE, device_iter->device_id, DEFAULT_FG);

        kprintf("Header Type: %h0x%x%h\n", ORANGE, device_iter->header_type, DEFAULT_FG);

        kprintf("Class & Subclass Code: %h0x%x%h\n", ORANGE, PCI_COMBINE_CLASS(device_iter->class_code, device_iter->subclass), DEFAULT_FG);

        if(device_iter->header_type == 0)
        {
            uint16_t line = pci_read_word(device_iter->location.bus, 
                               device_iter->location.device,
                               device_iter->location.function, 0x3C);

            kprintf("Interrupt line: %h0x%x%h\n", ORANGE, line, DEFAULT_FG);
            kprintf("Interrupt pin: %h0x%x%h\n", ORANGE, (line & 0xff00) >> 8, DEFAULT_FG);
        }

        kprintf("\n");

        device_iter++;

        if(argc == 1)
        {   
            i++;
            if(i >= max) break;
        }
    }
})

CREATE_COMMAND(snake, {
    main();
    kprintf("Score: %h%d%h\n", ORANGE, get_score(), DEFAULT_FG);
})

CREATE_COMMAND(rdisk, {
    if(argc != 2)
    {
        kprintf("%hMust have 2 arguments for sector and columns.%h\n", RED, DEFAULT_FG);
        return;
    }

    uint32_t sector = atoi(argv[0]);
    size_t columns = atoi(argv[1]);

    uint8_t buffer[ATA_SECTOR_BYTES];
    if(disk_read(0, sector, 1, buffer))
    {
        kprintf("%hDisk read error.%h\n", RED, DEFAULT_FG);
        return;
    }

    set_color(ORANGE);
    for(size_t b = 0; b < ATA_SECTOR_BYTES; b++)
    {
        uint8_t byte = buffer[b];
        char cbyte[3];
        itoa(byte, cbyte, 16);

        if(byte < 0x10)
        {
            cbyte[1] = cbyte[0];
            cbyte[0] = '0';
        }

        kprintf(cbyte);
        kprintf(" ");;

        if((b+1) % columns == 0)
        {
            kprintf("\n");
        }
    }
    set_color(DEFAULT_FG);
    kprintf("\n");
})

CREATE_COMMAND(wdisk, {
    if(argc != 3)
    {
        kprintf("%hMust have 3 arguments for the byte, sector and offset.%h\n", RED, DEFAULT_FG);
        return;
    }

    uint32_t sector = (uint32_t)atoi(argv[1]);
    size_t offset = (size_t)atoi(argv[2]);
    uint8_t byte = (uint8_t)atoi(argv[0]);

    uint8_t buffer[ATA_SECTOR_BYTES];
    if(disk_read(0, sector, 1, buffer))
    {
        kprintf("%hDisk read error.%h\n", RED, DEFAULT_FG);
        return;
    }

    buffer[offset] = byte;

    if(disk_write(0, sector, 1, buffer))
    {
        kprintf("%hDisk write error.%h\n", RED, DEFAULT_FG);
        return;
    }

    kprintf("%hSuccessfully written byte to disk.%h\n", GREEN, DEFAULT_FG);
})

CREATE_COMMAND(wipesect, {
    if(argc != 2)
    {
        kprintf("Must specify sector to wipe and count.\n");
        return;
    }

    uint8_t buffer[ATA_SECTOR_BYTES] = {0};
    uint32_t sector = (uint32_t)atoi(argv[0]);
    uint32_t count = (uint32_t)atoi(argv[1]);

    for(uint32_t i = 0; i < count; i++)
    {
        if(disk_write(0, sector + i, 1, buffer))
        {
            kprintf("Disk write error.\n");
            return;
        }
    }

    kprintf("Successfully wiped sector(s).\n");
})

CREATE_COMMAND(wipedisk, {
    uint8_t buffer[ATA_SECTOR_BYTES] = {0};
    size_t size = 0;

    if(get_nports() > 0) // prioritise ahci over ide
    {
        HBA_PORT *port = active_ports()[0].port;
        device_info_t device;
        ahci_identify(port, &device);

        size = device.size;
    }
    else if(ide_devices[0].reserved)
    {
       size = ide_devices[0].size;
    }
    else
    {
        kprintf("No drives.\n");
        return;
    }

    kprintf("Wipe started...\n");

    uint64_t start_time = get_time();
    for(uint32_t i = 0; i < size; i++)
    {
        if(disk_write(0, i, 1, buffer))
        {
            kprintf("Disk write error on sector %d.\n", i);
            return;
        }
    }
    uint64_t end_time = get_time();

    kprintf("Successfully wiped %d sector(s) (%d KiB) in %d ms.\n", size, size / 2, end_time - start_time);
})

void init_commands()
{
    ADDCMD(hello, 0)
    ADDCMD(add, 1)
    ADDCMD(sub, 2)
    ADDCMD(mul, 3)
    ADDCMD(div, 4)
    ADDCMD(print, 5)
    ADDCMD(setrow, 6)
    ADDCMD(setcol, 7)
    ADDCMD(scrninfo, 8)
    ADDCMD(checksse, 9)
    ADDCMD(clear, 10)
    ADDCMD(memdump, 11)
    ADDCMD(timepit, 12)
    ADDCMD(write, 13)
    ADDCMD(malloc, 14)
    ADDCMD(free, 15)
    ADDCMD(call, 16)
    ADDCMD(help, 17)
    ADDCMD(logpci, 18)
    ADDCMD(snake, 19)
    ADDCMD(rdisk, 20)
    ADDCMD(wdisk, 21)
    ADDCMD(wipesect, 22)
    ADDCMD(wipedisk, 23)
}