#include "eval.h"
#include "lisp.h"
#include <stdio.h>
#include <string.h>

int main(int argc, const char **argv)
{
    char buffer[4096];
    lisp_env *env = create_env(NULL);
    stdenv(env);

    printf("(lisp) > "); fflush(stdout);

    while (fgets(buffer, sizeof(buffer)-1, stdin)){
        char *endl;
        while ((endl = strrchr(buffer, '\n')) != NULL) *endl = '\0';

        lisp_obj *res = eval(buffer, env, NULL);
        printf(" => ");
        lisp_print(res);
        printf("\n");
        destroy_obj(res);
        printf("(lisp) > "); fflush(stdout);
    }
    printf("\n");

    destroy_env(env);
    return 0;
}
