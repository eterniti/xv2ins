#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <QMessageBox>

#include <windows.h>

#include "debug.h"

#define DEBUG_BUFFER_SIZE	1324

static int debug_level = 0;

RedirectFunc fud, fuu;

int set_debug_level(int level)
{
    int ret = debug_level;

    debug_level = level;
    return ret;
}

int mod_debug_level(int mod_by)
{
    return set_debug_level(debug_level+mod_by);
}

int FORMAT_PRINTF UserPrintf(const char* fmt, ...)
{
    char *dbg;
    va_list ap;

    dbg = (char *)malloc(DEBUG_BUFFER_SIZE);
    if (!dbg)
        return 0;

    va_start(ap, fmt);
    size_t len = vsnprintf(dbg, DEBUG_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    if (fuu)
    {
        fuu(dbg);
    }
    else
    {
        QMessageBox::information(NULL, "xv2ins", dbg);
        //MessageBoxA(nullptr, dbg, "xv2ins", MB_ICONINFORMATION);
    }

    free(dbg);
    return (int)len;
}

int FORMAT_PRINTF DebugPrintf(const char* fmt, ...)
{
    if (debug_level <= 0)
        return 0;

    char *dbg;
    va_list ap;

    dbg = (char *)malloc(DEBUG_BUFFER_SIZE);
    if (!dbg)
        return 0;

    va_start(ap, fmt);
    size_t len = vsnprintf(dbg, DEBUG_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    if (fud)
    {
        fud(dbg);
    }
    else
    {
        if (debug_level == 1)
        {
            printf("%s", dbg);
        }
        else
        {
            MessageBoxA(nullptr, dbg, "xv2ins", MB_ICONWARNING);
        }
    }

    free(dbg);
    return (int)len;
}

void redirect_dprintf(RedirectFunc func)
{
    fud = func;
}

void redirect_uprintf(RedirectFunc func)
{
    fuu = func;
}
