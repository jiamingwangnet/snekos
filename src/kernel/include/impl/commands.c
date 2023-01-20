#include "../commands.h"
#include "../stdlib.h"
#include "../console.h"
#include "../graphics.h"
#include "../timer.h"
#include "../kmalloc.h"

CREATE_COMMAND(hello, {
    kprintf("Hello There!\n");
})

CREATE_COMMAND(add, {
    if(argc < 2)
    {
        kprintf("Error: requires at least 2 numbers\n");
        return;
    }

    int sum = atoi(argv[0]);
    for(int i = 1; i < argc; i++)
    {
        sum += atoi(argv[i]);
    }

    char res[16];
    itoa(sum, res, 10);
    
    kprintf(res);
    kprintch('\n');
})

CREATE_COMMAND(sub, {
    if(argc < 2)
    {
        kprintf("Error: requires at least 2 numbers\n");
        return;
    }

    int sum = atoi(argv[0]);
    for(int i = 1; i < argc; i++)
    {
        sum -= atoi(argv[i]);
    }

    char res[16];
    itoa(sum, res, 10);
    
    kprintf(res);
    kprintch('\n');
})

CREATE_COMMAND(mul, {
    if(argc < 2)
    {
        kprintf("Error: requires at least 2 numbers\n");
        return;
    }

    int sum = atoi(argv[0]);
    for(int i = 1; i < argc; i++)
    {
        sum *= atoi(argv[i]);
    }

    char res[16];
    itoa(sum, res, 10);
    
    kprintf(res);
    kprintch('\n');
})

CREATE_COMMAND(div, {
    if(argc < 2)
    {
        kprintf("Error: requires at least 2 numbers\n");
        return;
    }

    int sum = atoi(argv[0]);
    for(int i = 1; i < argc; i++)
    {
        sum /= atoi(argv[i]);
    }

    char res[16];
    itoa(sum, res, 10);
    
    kprintf(res);
    kprintch('\n');
})

CREATE_COMMAND(print, {
    for(int i = 0; i < argc; i++)
    {
        kprintf(argv[i]);
        kprintch(' ');
    }
    kprintch('\n');
})

extern uint32_t max_rows;
extern uint32_t y;
extern uint32_t line_pad;

CREATE_COMMAND(setrow, {
    uint32_t size = atoi(argv[0]);
    if(strcmp(argv[0], "fit") == 0)
    {
        PSF1_font *font = get_font();
        max_rows = (tagfb->common.framebuffer_height - y) / (font->charsize + line_pad) - 1;
        return;
    }
    else if(size > 0)
    {
        max_rows = size;
        return;
    }
    kprintf("Row must be a number greater than 0 or \"fit\"\n");
})

extern uint32_t max_cols;
extern uint32_t x;
extern uint32_t col_pad;

CREATE_COMMAND(setcol, {
    uint32_t size = atoi(argv[0]);
    if(strcmp(argv[0], "fit") == 0)
    {
        max_cols = (tagfb->common.framebuffer_width - x) / (PSF1_WIDTH + col_pad) - 1;
        return;
    }
    else if(size > 0)
    {
        max_cols = size;
        return;
    }
    kprintf("Column must be a number greater than 0 or \"fit\"\n");
})

extern uint32_t* B_BUFFER;

CREATE_COMMAND(scrninfo, {
    char cwidth[16];
    char cheight[16];
    char cbpp[16];
    char caddr[16];
    char cbaddr[16];
    itoa(tagfb->common.framebuffer_width, cwidth, 10);
    itoa(tagfb->common.framebuffer_height, cheight, 10);
    itoa(tagfb->common.framebuffer_bpp, cbpp, 10);
    itoa(tagfb->common.framebuffer_addr, caddr, 16);
    itoa((uint64_t)B_BUFFER - 0xffffffff80000000, cbaddr, 16);
    

    kprintf("----Screen Information----\n");
    kprintf("Framebuffer address: 0x");
    kprintf(caddr);
    kprintch('\n');

    kprintf("Backbuffer address: 0x");
    kprintf(cbaddr);
    kprintch('\n');

    kprintf("Width: ");
    kprintf(cwidth);
    kprintch('\n');

    kprintf("Height: ");
    kprintf(cheight);
    kprintch('\n');

    kprintf("BPP: ");
    kprintf(cbpp);
    kprintch('\n');
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
        kprintf("SSE is available.\n");
    else
        kprintf("SSE not available.\n");
})

CREATE_COMMAND(clear, {
    clear();
})

CREATE_COMMAND(memdump, { // TODO: change to 64bit
    if(argc != 3)
    {
        kprintf("Error: memdump requires 3 arguments\n");
        return;
    }

    size_t bytes = atoi(argv[0]);
    size_t columns = atoi(argv[1]);
    uint32_t address = atoi(argv[2]);

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
        kprintch(' ');

        if((b+1) % columns == 0)
        {
            kprintch('\n');
        }
    }
    kprintch('\n');
})

CREATE_COMMAND(timepit, {
    char ctime[32];
    itoa(get_time(), ctime, 10);
    kprintf("The PIT time is: ");
    kprintf(ctime);
    kprintch('\n');
})

CREATE_COMMAND(write, {
    if(argc != 3)
    {
        kprintf("Must have 3 arguments: size, data, destination\n");
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
        kprintf("Must be a valid size below QWORD\n");
        return;
    }

    char caddr16[16];
    itoa(atoi(argv[2]), caddr16, 16);

    kprintf("Data written to 0x");
    kprintf(caddr16);
    kprintch('\n');
})

CREATE_COMMAND(malloc, {
    if(argc != 1)
    {
        kprintf("Error: must have only 1 argument\n");
        return;
    }

    uint64_t addr = (uint64_t)kmalloc(atoi(argv[0])) - 0xffffffff80000000;

    char caddr[16];
    char caddr16[16];

    itoa((uint32_t)addr, caddr, 10);
    itoa((uint32_t)addr, caddr16, 16);

    kprintf("Allocated memory at: ");
    kprintf(caddr);
    kprintf(" (0x");
    kprintf(caddr16);
    kprintf(")\n");
})

CREATE_COMMAND(free, {
    if(argc != 1)
    {
        kprintf("Error: must have only 1 argument\n");
        return;
    }

    void *addr = (void*)atoi(argv[0]);
    kfree(addr);

    // TODO: add a conversion function
    char caddr16[16];
    itoa((uint32_t)addr, caddr16, 16);

    kprintf("Freed memory at 0x");
    kprintf(caddr16);
    kprintch('\n');
})

CREATE_COMMAND(call, {
    if(argc != 1)
    {
        kprintf("Error: must have only 1 argument\n");
        return;
    }
    
    uint64_t addr = atoi(argv[0]) + 0xffffffff80000000;
    __asm__ volatile("call %0" : : "g"(addr));
})

CREATE_COMMAND(help, {
    kprintf(
        "hello\t                          Prints out \"Hello There!\".\n"
        "add [nums ...]\t                 Adds the specified numbers together.\n"
        "sub [nums ...]\t                 Subtracts from the first number.\n"
        "mul [nums ...]\t                 Multiplies the specified numbers together.\n"
        "div [nums ...]\t                 Divides the numbers.\n"
        "print [str ...]\t                Prints the specified text.\n"
        "setrow [row] | fit\t             Sets the console max rows to the specified number, \"fit\" adjusts the console to fit the screen.\n"
        "setcol [col] | fit\t             Sets the console max columns to the specified number, \"fit\" adjusts the console to fit the screen.\n"
        "scrninfo\t                       Prints information about the screen.\n"
        "checksse\t                       Checks whether SSE is available.\n"
        "clear\t                          Clears the console.\n"
        "memdump [bytes] [col] [addr]\t   Dumps the memory at the specified address.\n"
        "timepit\t                        Prints out the amount of time the Programmable Interrupt Timer has fired.\n"
        "write [size] [data] [addr]\t     Writes the specified data to the address.\n"
        "malloc [bytes]\t                 Allocates the specified size of memory and prints the address.\n"
        "free [addr]\t                    Frees the allocated memory at the specified address.\n"
        "call [addr]\t                    Calls the function at the specified address.\n"
        "help\t                           Displays this help message.\n"
    );
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
}