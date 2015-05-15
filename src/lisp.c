#include "lisp.h"
#include "internals.h"
#include <assert.h>

lisp_obj *exit_proc(size_t argc, lisp_obj **argv)
{
    exit(0);
    return NIL;
}

lisp_obj lisp_exit = {.refcount=-42, .type=PROC, .value.p=exit_proc};

/* Some stubs, faster to write directly in lisp */
static const char *lisp_stubs[] = {
    "(define (cons head tail)"
    "  (lambda (pair) (pair head tail)))",

    "(define (car pair)"
    "  (pair (lambda (head tail) head)))",

    "(define (cdr pair)"
    "  (pair (lambda (head tail) tail)))",

    "(define (cadr seq) (car (cdr seq)))",

    "(define (caddr seq) (cadr (cdr seq)))",

    "(define (cadddr seq) (caddr (cdr seq)))",

    "(define (range from to)"
    "  (if (< from to)"
    "    (cons from (range (+ 1 from) to))"
    "    #n))",

    "(define (list-get i seq)"
    "  (if (< i 1) (car seq) (list-get (- i 1) (cdr seq))))",

    "(define (map proc seq)"
    "  (if seq"
    "    (cons (proc (car seq)) (map proc (cdr seq)))"
    "    #n))",

    "(define (reduce proc initial seq)"
    "  (if seq"
    "    (proc (car seq) (reduce proc initial (cdr seq)))"
    "    initial))",

    "(define (apply proc seq) (reduce proc (car seq) (cdr seq)))"
};

void stdenv(lisp_env *env)
{
    /* Arithmetic, logical */
    set_env(env, "+", (lisp_obj*) &lisp_add);
    set_env(env, "-", (lisp_obj*) &lisp_sub);
    set_env(env, "*", (lisp_obj*) &lisp_mul);
    set_env(env, "<", (lisp_obj*) &lisp_lt);
    set_env(env, "real", (lisp_obj*) &lisp_real);

    /* Repl, i/o */
    set_env(env, "display", (lisp_obj*) &lisp_display);
    set_env(env, "exit", (lisp_obj*) &lisp_exit);

    /* Debug, internal */
    set_env(env, "refcount", (lisp_obj*) &lisp_refcount);
    set_env(env, "dmp", (lisp_obj*) &lisp_dump_env);

    /* Lisp plumbery */
    lisp_obj *res = NULL;
    for (size_t i=0; i<sizeof(lisp_stubs)/sizeof(lisp_stubs[0]); i++){
        res = eval(lisp_stubs[i], env, NULL);
        assert(res == NIL);
    }
}
