#include "../commands.h"
#include "../stdlib.h"
#include "../console.h"
#include "../graphics.h"

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