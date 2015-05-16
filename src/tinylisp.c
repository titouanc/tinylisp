#include "eval.h"
#include "lisp.h"
#include <stdio.h>
#include <string.h>

int main(int argc, const char **argv)
{
    char buffer[4096];
    lisp_env *env = create_env(NULL);
    lisp_err error = {.type=OK};

    stdenv(env);
    printf("(lisp) > "); fflush(stdout);

    while (fgets(buffer, sizeof(buffer)-1, stdin)){
        char *endl;
        while ((endl = strrchr(buffer, '\n')) != NULL) *endl = '\0';

        lisp_obj *res = eval(buffer, env, &error);
        if (error.type != OK){
            if (error.type != NO_EXPRESSION)
                printf("ERROR: %s\n", error.description);
        }
        else {
            printf(" => ");
            lisp_print(res);
            printf("\n");
            release(res);
        }
        error.type = OK;
        printf("(lisp) > "); fflush(stdout);
    }
    printf("\nGoodbye !\n");

    clear_env(env);
    release_env(env);
    cleanup_error(&error);
    
    return 0;
}
