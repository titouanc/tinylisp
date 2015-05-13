#include "lisp.h"
#include <stdio.h>

int main(int argc, const char **argv)
{
    char buffer[(2<<20)]; //1Mo buffer
    char *bufptr = buffer;
    size_t n = 0;

    while ((n = fread(bufptr, sizeof(char), sizeof(buffer)-n, stdin)) > 0){
        bufptr += n;
        while (bufptr[-1] == '\n'){
            bufptr--;
            *bufptr = '\0';
        }
    }
    *bufptr = '\0';

    if (bufptr > buffer){
        const char *endptr = buffer;
        while (*endptr != '\0'){
            lisp_expr *expr = analyze(endptr, &endptr, NULL);
            endptr++;
            dump_expr(expr);
            printf("\n");
            release_expr(expr);

        }
    }

    return 0;
}
