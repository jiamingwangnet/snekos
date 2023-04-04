#include "../include/console/console.h"
#include "../include/graphics/graphics.h"
#include "../include/stdlib/stdlib.h"
#include "../include/memory/kmalloc.h"
#include "../include/console/commands.h"
#include "../include/memory/memory.h"
#include "../include/io/serial.h"
#include <stdarg.h>

static uint32_t foreground = 0xffffff;
static uint32_t background = 0x000000;

uint32_t DEFAULT_FG = 0xffffff;
uint32_t DEFAULT_BG = 0x000000;

uint32_t x = 0;
uint32_t y = 0;

uint32_t max_rows = 30;
uint32_t max_cols = 120;

const uint32_t line_pad = 5;
const uint32_t col_pad = 1;

static const char* cmdtxt = "| KERNEL CMD :> ";
static bool input_mode = false;
static bool update_console = false;
static bool has_initalised = false;

struct pending_command_t {
    cmd_func command;
    int argc;
    char **argv;
} pending_command;
static bool hasCommand = false;

#define INPUT_BUFFER_SIZE 512
static char input_buffer[INPUT_BUFFER_SIZE];
static char *buffer_ptr = input_buffer;

#define TAB_WDITH 4

// stores the terminal text
static char *con_memory = NULL;
static char *mem_end = NULL;
static size_t con_memory_size = 0x200000; // TODO: implement this fully, this caused a lot of errors

// TODO: store color and character together in a single uint32_t: char * 0x1000000 + color   example: char: 0x64 color: 0x34e832 together: 0x6434e832
static uint32_t *color_memory = NULL; // TODO: make more memory efficient solution
static uint32_t *cmem_end = NULL;

inline void expand_cmem()
{
    char *tmp = kmalloc(con_memory_size*2);
    memcpy((void*)tmp, (void*)con_memory, con_memory_size);
    kfree(con_memory);
    con_memory = tmp;
    con_memory_size *= 2;
}

void enable_input()
{
    kprintf(cmdtxt);
    input_mode = true;
}

void clear()
{
    con_memory[0] = 0;
    con_memory[1] = 0;
    
    // while(*tmp)
    //     *tmp++ = 0;
    mem_end = con_memory;
    cmem_end = color_memory;
}

void init_console(uint32_t sx, uint32_t sy, uint32_t fg, uint32_t bg)
{
    x = sx;
    y = sy;
    foreground = fg;
    background = bg;

    DEFAULT_FG = fg;
    DEFAULT_BG = bg;

    attach_keyboard(console_keyboard);

    fill_screen(background);

    con_memory = (char*)kmalloc(con_memory_size); // sizeof(uint32_t) * con_memory_size works
    color_memory = (uint32_t*)kmalloc(sizeof(uint32_t) * con_memory_size);

    memset((void*)con_memory, 0, con_memory_size); // makes sure that con_memory will always be a null terminated string
    memset((void*)color_memory, 0, sizeof(uint32_t) * con_memory_size);

    mem_end = con_memory;
    cmem_end = color_memory;
    
    init_commands();

    // fit console
    PSF1_font *font = get_font();
    max_cols = (tagfb.common.framebuffer_width - x) / (PSF1_WIDTH + col_pad) - 1;
    max_rows = (tagfb.common.framebuffer_height - y) / (font->charsize + line_pad) - 1;

    console_loop();

    has_initalised = true;
}

void set_color(uint32_t color)
{
    foreground = color;
}

// free memory
void console_end()
{
    kfree((void*)con_memory);
}

void append_buffer(char c)
{
    *buffer_ptr = c;
    buffer_ptr++;
}

// WARNING: input buffer must be cleared after the command handling and the input buffer must not be used after command handling
void clear_buffer()
{
    for(size_t i = 0; i < INPUT_BUFFER_SIZE; i++)
    {
        input_buffer[i] = 0;
    }
    buffer_ptr = input_buffer;
}

void decrease_buffer()
{
    if(buffer_ptr == input_buffer) return;
    *--buffer_ptr = 0;
    update_console = true;
}

void backspace()
{
    if(!has_initalised)
    {
        serial_str("WARNING: mem_end accessed before console memory allocation. The function will now return.\n");
        return;
    }

    // PSF1_font* font = get_font();
    decrease_buffer();
    *--mem_end = 0;
    *--cmem_end = 0;
    // draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, PSF1_WIDTH + col_pad, font->charsize + line_pad, background);
}

void add_pending_command(cmd_func command, int argc, char **argv)
{
    pending_command.command = command;
    pending_command.argc = argc;

    char **alloc_argv = (char**)kmalloc(sizeof(char*) * argc);
    for(int i = 0; i < argc; i++)
    {
        char *arg = argv[i];
        size_t len = strlen(arg) + 1;

        char *alloc_arg = (char*)kmalloc(len); // REMEMBER TO FREE THE MEMORY
        strcpy(alloc_arg, arg);

        alloc_argv[i] = alloc_arg;
    }

    pending_command.argv = alloc_argv;
    hasCommand = true;
}

void run_pending_command()
{
    if(!hasCommand) return;
    pending_command.command(pending_command.argc, pending_command.argv);

    // free args
    for(int i = 0; i < pending_command.argc; i++)
    {
        kfree((void*)pending_command.argv[i]);
    }
    kfree((void*)pending_command.argv);
    hasCommand = false;

    enable_input();
}

// WARNING: input buffer must be cleared after the command handling and the input buffer must not be used after command handling
void handle_commands()
{
    if(!*input_buffer) 
    {
        enable_input();
        return;
    }

    char name[INPUT_BUFFER_SIZE]; // TODO: make dynamic and ignore leading space
    size_t i = 0;
    for(; input_buffer[i] != ' ' && input_buffer[i] != 0; i++)
        name[i] = input_buffer[i];
    name[i] = 0;

    for(size_t i = 0; i < N_COMMANDS; i++) // TODO: implement hash table for quicker search
    {
        if(strcmp(name, commands[i].name) == 0)
        {
            int argc = 0;

            // TODO: ignore leading space
            // count args
            char *tmp = input_buffer;
            while(*tmp)
            {
                if(*tmp == ' ' && *(tmp + 1) != ' ' && *(tmp + 1) != 0)
                {
                    argc++;
                }
                tmp++;
            }

            // replace spaces in input buffer
            // last arg will automatically have a 0 at the end
            char **argv = (char**)kmalloc(sizeof(char*) * argc);

            tmp = input_buffer;
            size_t argi = 0;
            while(*tmp)
            {
                if(*tmp == ' ')
                {
                    *tmp = 0;
                    if(*(tmp + 1) != ' ' && *(tmp + 1) != 0)
                    {
                        argv[argi] = tmp+1;
                        argi++;
                    }
                }
                tmp++;
            }

            add_pending_command(commands[i].func, argc, argv);

            kfree((void*)argv);
            return;
        }
    }
    kprintf("%hError: \"%s\" command not found.%h\n", RED, name, DEFAULT_FG);

    enable_input();
}

void enter()
{
    // PSF1_font* font = get_font();
    // clear cursor
    // draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, PSF1_WIDTH + col_pad, font->charsize + line_pad, background);
    kprintf("\n");
    input_mode = false;

    // run command
    handle_commands();
}

void console_keyboard(Key_Info info)
{
    // other interrupts do not work here (PIT will not fire)
    if(!input_mode) return;

    if(!info.release && !info.modifier)
    {
        const char keystr[2] = {info.key, 0};
        kprintf(keystr);
        append_buffer(info.key);
    }
    else if(!info.release && info.modifier && info.key == ENTER)
    {
        enter();

        // WARNING: input buffer must be cleared after the command handling and the input buffer must not be used after command handling
        clear_buffer();
    }
    else if(!info.release && info.modifier && info.key == BACKSPACE)
    {
        if(buffer_ptr == input_buffer) return;
        
        backspace();
    }
}

void kprintch(char c)
{
    serial_char(c);
    *mem_end++ = c;
    *cmem_end++ = foreground;
    update_console = true;
}

void kprintf(const char *str, ...) // only allows specifier character
{
    // safety check
    if(!has_initalised)
    {
        serial_str("WARNING: kprintf called before console memory allocation. The function will now return.\n");
        return;
    }

    va_list args;
    va_start(args, strlen(str));

    while(*str)
    {
        if(*str == '%' && str[1] != '\0')
        {
            switch(str[1])
            {
                case 'd':
                {
                    int num = va_arg(args, int);
                    char cnum[16];
                    char *pnum = cnum;

                    itoa(num, cnum, 10);

                    while(*pnum) kprintch(*pnum++);
                    break;
                }
                case 'x':
                {
                    int num = va_arg(args, int);
                    char cnum[16];
                    char *pnum = cnum;
                    itoa(num, cnum, 16);

                    while(*pnum) kprintch(*pnum++);
                    break;
                }
                case 's':
                {
                    const char *str = va_arg(args, const char*);
                    while(*str) kprintch(*str++);
                    break;
                }
                case 'h': // custom color formatting (h is for hue)
                {
                    uint32_t color = va_arg(args, uint32_t);
                    set_color(color);
                    break;
                }
                case '%':
                {
                    kprintch('%');
                    break;
                }
            }
            str+=2;
        }
        else
        {
            kprintch(*str++);
        }
    }

    va_end(args);
}

void goto_line_start()
{
    if(!has_initalised)
    {
        serial_str("WARNING: mem_end accessed before console memory allocation. The function will now return.\n");
        return;
    }

    if(mem_end == con_memory) return;
    mem_end--;
    cmem_end--;
    while(*(mem_end-1) != '\n') 
    {
        mem_end--;
        cmem_end--;
    }
}

void draw_cursor(uint32_t col, uint32_t row)
{
    PSF1_font* font = get_font();
    draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y + 10, PSF1_WIDTH, 5, foreground);
}

void console_loop()
{
    if(!update_console || !has_initalised) return;

    run_pending_command();
    PSF1_font* font = get_font();
    fill_screen(background);
    uint32_t col = 0; 
    uint32_t row = 0;
    
    // count back rows, ignore column wrap
    char *c = mem_end - 1;
    uint32_t *cl = cmem_end - 1;

    uint32_t row_count = 0;
    while(c != con_memory - 1)
    {
        if(*c == '\n')
        {
            row_count ++;
            if(row_count == max_rows)
                break;
        }
        c--;
        cl--;
    }
    c++;
    cl++;

    // handle column wrap here by rewriting lines
    char *start = c;
    uint32_t *cstart = cl;

    for(; c != mem_end; c++, cl++)
    {
        if(*c == '\t')
            col += TAB_WDITH;
        else
            draw_char(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, *cl, background, *c);

        col++;

        if(*c == '\n' || col == max_cols)
        {
            row++;
            col = 0;
            if(row == max_rows)
            {
                draw_rect(x, y, max_cols * (PSF1_WIDTH + col_pad), max_rows * (font->charsize + line_pad), background);

                // skip start by 1 line
                uint32_t col_count = 0;
                while(*start != '\n' && col_count != max_cols)
                {
                    start++;
                    cstart++;
                    col_count++;
                }
                c = ++start;
                cl = ++cstart;
                row = 0;
            }
        }
    }


    if(input_mode) draw_cursor(col, row);

    update_console = false;
}