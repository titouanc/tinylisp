#ifndef DEFINE_EXPRESSION_HEADER
#define DEFINE_EXPRESSION_HEADER

#include "obj.h"
#include "env.h"

typedef enum {
    UNEXPECTED_TOKEN,
    TOO_MUCH_ARGS,
    NOT_ENOUGH_ARGS
} lisp_err;

lisp_obj *eval(const char *str, lisp_env *env, lisp_err *err);

#endif