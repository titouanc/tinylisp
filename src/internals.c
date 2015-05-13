#include "internals.h"
#include <assert.h>
#include <string.h>
#include "utils.h"

lisp_obj *add_proc(size_t argc, lisp_obj **argv);

lisp_obj lisp_add = {._static=true, .type=PROC, .value.p=add_proc};

lisp_obj *add_proc(size_t argc, lisp_obj **argv)
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