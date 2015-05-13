#include "internals.h"
#include <assert.h>
#include <string.h>
#include "utils.h"

lisp_obj *add_proc(size_t argc, lisp_obj **argv);
lisp_obj *sub_proc(size_t argc, lisp_obj **argv);

lisp_obj lisp_add = {.refcount=-42, .type=PROC, .value.p=add_proc};
lisp_obj lisp_sub = {.refcount=-42, .type=PROC, .value.p=sub_proc};

lisp_obj *add_proc(size_t argc, lisp_obj **argv)
{
    assert(argc > 0);

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

lisp_obj *sub_proc(size_t argc, lisp_obj **argv)
{
    assert(argc > 0);

    lisp_obj *res = create_obj(argv[0]->type, argv[0]->value);

    if (argc == 1){
        if (res->type == FLOAT)
            res->value.f *= -1;
        else if (res->type == INT)
            res->value.i *= -1;
        else
            ERROR("Unknow unary operator - on non-number");
        return res;
    }

    for (size_t i=1; i<argc; i++){
        lisp_obj *arg = argv[i];

        switch (res->type){
            case INT:
                switch (arg->type){
                    /* int + int -> int */
                    case INT:
                        res->value.i -= arg->value.i;
                        break;

                    /* int + float -> float */
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
                    /* float + int -> float */
                    case INT:
                        res->value.f -= arg->value.i;
                        break;

                    /* float + float -> float */
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
