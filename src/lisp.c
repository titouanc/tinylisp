#include "lisp.h"
#include "internals.h"

lisp_obj *exit_proc(size_t argc, lisp_obj **argv)
{
    exit(0);
    return NIL;
}

lisp_obj lisp_exit = {.refcount=-42, .type=PROC, .value.p=exit_proc};

void stdenv(lisp_env *env)
{
    set_env(env, "+", &lisp_add);
    set_env(env, "-", &lisp_sub);
    set_env(env, "*", &lisp_mul);
    set_env(env, "refcount", &lisp_refcount);
    set_env(env, "exit", &lisp_exit);
}
