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
    /* Arithmetic, logical */
    set_env(env, "+", (lisp_obj*) &lisp_add);
    set_env(env, "-", (lisp_obj*) &lisp_sub);
    set_env(env, "*", (lisp_obj*) &lisp_mul);
    set_env(env, "<", (lisp_obj*) &lisp_lt);

    /* Repl, i/o */
    set_env(env, "display", (lisp_obj*) &lisp_display);
    set_env(env, "exit", (lisp_obj*) &lisp_exit);

    /* Debug, internal */
    set_env(env, "refcount", (lisp_obj*) &lisp_refcount);
    set_env(env, "dmp", (lisp_obj*) &lisp_dump_env);
}
