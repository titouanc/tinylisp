#include "eval.h"
#include "lisp.h"
#include <stdio.h>

int main(int argc, const char **argv)
{
    char buffer[4096];
    lisp_env *env = create_env(NULL);
    stdenv(env);

    while (fgets(buffer, sizeof(buffer)-1, stdin)){
        lisp_obj *res = eval(buffer, env, NULL);
        printf(" => ");
        lisp_print(res);
        printf("\n");
        destroy_obj(res);
    }

    destroy_env(env);
    return 0;
}
