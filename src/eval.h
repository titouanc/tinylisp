#ifndef DEFINE_EXPRESSION_HEADER
#define DEFINE_EXPRESSION_HEADER

#include "obj.h"
#include "env.h"
#include "expr.h"

lisp_obj *eval_expression(lisp_expr *expr, lisp_env *env, lisp_err *err);

lisp_obj *eval(const char *str, lisp_env *env, lisp_err *err);

#endif