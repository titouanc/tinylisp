#include "internals.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

static lisp_obj *add_proc(size_t argc, lisp_obj **argv)
{
    assert(argc > 0);
    assert(argv[0] != NULL);

    lisp_obj *res = create_obj(argv[0]->type, argv[0]->value);

    for (size_t i=1; i<argc; i++){
        lisp_obj *arg = argv[i];

        switch (res->type){
            case INT:
                switch (arg->type){
                    /* int + int -> int */
                    case INT:
                        res->value.i += arg->value.i;
                        break;

                    /* int + float -> float */
                    case FLOAT:
                        res->value.f = res->value.i + arg->value.f;
                        res->type = FLOAT;
                        break;

                    default:
                        ERROR("Cannot add int and non-number");
                }
                break;

            case FLOAT:
                switch (arg->type){
                    /* float + int -> float */
                    case INT:
                        res->value.f += arg->value.i;
                        break;

                    /* float + float -> float */
                    case FLOAT:
                        res->value.f += arg->value.f;
                        break;

                    default:
                        ERROR("Cannot add float and non-number");
                }
                break;
            default:
                break;
        }
    }

    return res;
}

static lisp_obj *sub_proc(size_t argc, lisp_obj **argv)
{
    assert(argc > 0);
    assert(argv[0] != NULL);

    lisp_obj *res = create_obj(argv[0]->type, argv[0]->value);

    /* Unary operator '-' */
    if (argc == 1){
        if (res->type == FLOAT)
            res->value.f *= -1;
        else if (res->type == INT)
            res->value.i *= -1;
        else
            ERROR("Unknow unary operator - on non-number");
        return res;
    }

    /* n-ary operator '-' */
    for (size_t i=1; i<argc; i++){
        lisp_obj *arg = argv[i];

        switch (res->type){
            case INT:
                switch (arg->type){
                    /* int - int -> int */
                    case INT:
                        res->value.i -= arg->value.i;
                        break;

                    /* int - float -> float */
                    case FLOAT:
                        res->value.f = res->value.i - arg->value.f;
                        res->type = FLOAT;
                        break;

                    default:
                        ERROR("Cannot add int and non-number");
                }
                break;

            case FLOAT:
                switch (arg->type){
                    /* float - int -> float */
                    case INT:
                        res->value.f -= arg->value.i;
                        break;

                    /* float - float -> float */
                    case FLOAT:
                        res->value.f -= arg->value.f;
                        break;

                    default:
                        ERROR("Cannot add float and non-number");
                }
                break;
            default:
                break;
        }
    }

    return res;
}

static lisp_obj *mul_proc(size_t argc, lisp_obj **argv)
{
    assert(argc > 0);
    assert(argv[0] != NULL);

    lisp_obj *res = create_obj(argv[0]->type, argv[0]->value);

    for (size_t i=1; i<argc; i++){
        lisp_obj *arg = argv[i];

        switch (res->type){
            case INT:
                switch (arg->type){
                    /* int - int -> int */
                    case INT:
                        res->value.i *= arg->value.i;
                        break;

                    /* int - float -> float */
                    case FLOAT:
                        res->value.f = res->value.i * arg->value.f;
                        res->type = FLOAT;
                        break;

                    default:
                        ERROR("Cannot add int and non-number");
                }
                break;

            case FLOAT:
                switch (arg->type){
                    /* float - int -> float */
                    case INT:
                        res->value.f *= arg->value.i;
                        break;

                    /* float - float -> float */
                    case FLOAT:
                        res->value.f *= arg->value.f;
                        break;

                    default:
                        ERROR("Cannot add float and non-number");
                }
                break;
            default:
                break;
        }
    }

    return res;
}

static lisp_obj *refcount_proc(size_t argc, lisp_obj **argv)
{
    assert(argc >= 1);

    lisp_obj *res = lisp_int(argv[0]->refcount);
    return res;
}

static lisp_obj *display_proc(size_t argc, lisp_obj **argv)
{
    for (size_t i=0; i<argc; i++){
        if (i > 0)
            printf(" ");
        lisp_print(argv[i]);
    }
    printf("\n");
    return lisp_int(argc);
}

static lisp_obj *lt_proc(size_t argc, lisp_obj ** argv)
{
    assert(argc >= 2);

    double prev=0, cur=0;
    if (argv[0]->type == INT){cur = argv[0]->value.i;}
    else if (argv[0]->type == FLOAT){cur = argv[0]->value.f;}
    else {ERROR("Cannot compare non-number");}

    for (size_t i=1; i<argc; i++){
        prev = cur;
        if (argv[i]->type == INT){cur = argv[i]->value.i;}
        else if (argv[i]->type == FLOAT){cur = argv[i]->value.f;}
        else {ERROR("Cannot compare non-number");}
        if (! (prev < cur)){
            return FALSE;
        }
    }

    return TRUE;
}

static lisp_obj *dump_env_proc(size_t argc, lisp_obj **argv)
{
    for (size_t i=0; i<argc; i++){
        if (argv[i]->type == LAMBDA){
            dump_env(argv[i]->value.l.context);
        }
    }
    return NIL;
}

#define INTERNAL_PROCEDURE(func) {.refcount=-42, .type=PROC, .value.p=func}

const lisp_obj lisp_add = INTERNAL_PROCEDURE(add_proc);
const lisp_obj lisp_sub = INTERNAL_PROCEDURE(sub_proc);
const lisp_obj lisp_mul = INTERNAL_PROCEDURE(mul_proc);
const lisp_obj lisp_refcount = INTERNAL_PROCEDURE(refcount_proc);
const lisp_obj lisp_display = INTERNAL_PROCEDURE(display_proc);
const lisp_obj lisp_lt = INTERNAL_PROCEDURE(lt_proc);
const lisp_obj lisp_dump_env = INTERNAL_PROCEDURE(dump_env_proc);
