#include "lisp.h"
#include "internals.h"

lisp_obj *lisp_exit(size_t argc, lisp_obj *argv)
{
    exit(0);
    return NIL;
}

void stdenv(lisp_env *env)
{
    set_env(env, "+", &lisp_add);
    set_env(env, "-", &lisp_sub);
    set_env(env, "exit", lisp_internal(lisp_exit));
}
