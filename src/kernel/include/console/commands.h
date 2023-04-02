#pragma once

typedef void (*cmd_func)(int argc, char *argv[]);

typedef struct
{
    const char *name;
    cmd_func func;
} command_t;

#define CREATE_COMMAND(cname, body) \
    command_t cmd_##cname = { \
        .name = #cname, \
        .func = cmd_##cname##_func \
    }; \
    void cmd_##cname##_func(int argc, char *argv[]) body

#define DECL_CMDFUNC(cname) \
    void cmd_##cname##_func(int argc, char *argv[]);

#define N_COMMANDS 26
command_t commands[N_COMMANDS];

#define ADDCMD(cname, index) commands[index] = cmd_##cname;

void init_commands();

DECL_CMDFUNC(hello)
DECL_CMDFUNC(add)
DECL_CMDFUNC(sub)
DECL_CMDFUNC(mul)
DECL_CMDFUNC(div)
DECL_CMDFUNC(print)
DECL_CMDFUNC(setrow)
DECL_CMDFUNC(setcol)
DECL_CMDFUNC(scrninfo)
DECL_CMDFUNC(checksse)
DECL_CMDFUNC(clear)
DECL_CMDFUNC(memdump)
DECL_CMDFUNC(timepit)
DECL_CMDFUNC(write)
DECL_CMDFUNC(malloc)
DECL_CMDFUNC(free)
DECL_CMDFUNC(call)
DECL_CMDFUNC(help)
DECL_CMDFUNC(logpci)
DECL_CMDFUNC(snake)
DECL_CMDFUNC(rdisk)
DECL_CMDFUNC(wdisk)
DECL_CMDFUNC(wipesect)
DECL_CMDFUNC(wipedisk)
DECL_CMDFUNC(meminfo)
DECL_CMDFUNC(alloclog)