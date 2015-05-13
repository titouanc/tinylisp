#include "obj.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const lisp_obj null_obj = {._static=true};
const lisp_obj true_obj = {._static=true};
const lisp_obj false_obj = {._static=true};

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
    if (! obj->_static){
        free(obj);
    }
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
