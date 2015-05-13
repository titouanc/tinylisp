#include "obj.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const lisp_obj null_obj = {.refcount=-42};
const lisp_obj true_obj = {.refcount=-42};
const lisp_obj false_obj = {.refcount=-42};

lisp_obj *create_obj(lisp_obj_type type, lisp_obj_val value)
{
    lisp_obj *res = calloc(1, sizeof(lisp_obj));
    assert(res != NULL);
    res->type = type;
    res->value = value;
    res->refcount = 0;
    return res;
}

lisp_obj *create_empty_obj(lisp_obj_type type)
{
    lisp_obj *res = calloc(1, sizeof(lisp_obj));
    res->type = type;
    res->refcount = 0;
    return res;
}

static void destroy_obj(lisp_obj *obj)
{
    assert(obj != NULL);
    assert(obj->refcount == 0);

    if (obj->type == LAMBDA){
        for (size_t i=0; i<obj->value.l.nparams; i++){
            free(obj->value.l.param_names[i]);
        }
        free(obj->value.l.param_names);
        release_expr(obj->value.l.body);
    }

    /* Clean area */
    memset(obj, 0, sizeof(lisp_obj));

    free(obj);
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

lisp_obj *retain(lisp_obj *obj)
{
    assert(obj != NULL);
    if (obj->refcount >= 0)
        obj->refcount++;
    return obj;
}

lisp_obj *release(lisp_obj *obj)
{
    assert(obj != NULL);
    if (obj->refcount == 0){
        destroy_obj(obj);
        return NULL;
    }
    else if (obj->refcount > 0){
        obj->refcount--;
    }
    return obj;
}
