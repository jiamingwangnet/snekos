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