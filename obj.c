#include "obj.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const lisp_obj null_obj;
const lisp_obj true_obj;
const lisp_obj false_obj;

static inline void ERROR(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("\033[31;1m[ERROR]\033[0m ");
    vprintf(fmt, args);
    va_end(args);
    exit(1);
}

lisp_obj *create_obj(lisp_obj_type type, lisp_obj_val value)
{
    lisp_obj *res = calloc(1, sizeof(lisp_obj));
    assert(res != NULL);
    res->type = type;
    res->value = value;
    return res;
}

void destroy_obj(lisp_obj *obj)
{
    if (! (obj == NIL || obj == FALSE || obj == TRUE)){
        free(obj);
    }
}

lisp_obj *add(size_t argc, lisp_obj **argv)
{
    assert(argc > 0);

    lisp_obj *res = calloc(1, sizeof(lisp_obj));
    memcpy(res, argv[0], sizeof(lisp_obj));

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

void lisp_print(lisp_obj *obj)
{
    if (obj == NULL){
        return;
    }

    if (obj->type == INT){
        printf("%ld", obj->value.i);
    }
    else if (obj->type == FLOAT){
        printf("%lf", obj->value.f);
    }
    else if (obj->type == PROC){
        printf("#<Procedure>");
    }
    else if (obj->type == LAMBDA){
        printf("#<Lambda>");
    }
    else if (obj == TRUE){printf("#t");}
    else if (obj == FALSE){printf("#f");}
    else if (obj == NIL){printf("#n");}
}
