#include "utils.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

static const bool enable_debug = false;

char *duplicate_string(const char *string, int maxsize)
{
    assert(string != NULL);

    int len = 0;
    while (string[len] && (maxsize < 0 || len < maxsize)){
        len++;
    }

    char *res = calloc(len+1, sizeof(char));
    memcpy(res, string, len);
    return res;
}

void ERROR(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\033[31;1m[ERROR]\033[0m ");
    vfprintf(stderr, fmt, args);
    if (! strchr(fmt, '\n'))
        fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

void DEBUG(const char *fmt, ...)
{
    if (enable_debug){
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "\033[34;1m[DEBUG]\033[0m ");
        vfprintf(stderr, fmt, args);
        if (! strchr(fmt, '\n'))
            fprintf(stderr, "\n");
        va_end(args);
    }
}
