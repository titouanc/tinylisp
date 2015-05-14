#include "lisp.h"
#include "internals.h"
#include <assert.h>

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

    /* Lisp plumbery */
    lisp_obj *res = NULL;
    res = eval("(define cons (lambda (head tail)"
               "    (lambda (closure) (closure head tail))))", env, NULL);
    assert(res == NIL);

    res = eval("(define car (lambda (closure)"
               "    (closure (lambda (head tail) head))))", env, NULL);
    assert(res == NIL);

    res = eval("(define cdr (lambda (closure)"
               "    (closure (lambda (head tail) tail))))", env, NULL);
    assert(res == NIL);

    res = eval("(define cadr (lambda (lst) (car (cdr lst))))", env, NULL);
    assert(res == NIL);

    res = eval("(define range (lambda (min max)"
               "    (if (< min max)"
               "        (cons min (range (+ 1 min) max))"
               "        #n)))", env, NULL);
    assert(res == NIL);

    res = eval("(define map (lambda (proc lst)"
               "    (if lst"
               "        (cons (proc (car lst)) (map proc (cdr lst)))"
               "        #n)))", env, NULL);
    assert(res == NIL);

    res = eval("(define reduce (lambda (proc lst initial)"
               "    (if lst"
               "        (proc (car lst) (reduce proc (cdr lst) initial))"
               "        initial)))", env, NULL);
    assert(res == NIL);
}
