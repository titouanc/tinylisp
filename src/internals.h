#ifndef DEFINE_INTERNALS_HEADER
#define DEFINE_INTERNALS_HEADER

#include "obj.h"

/* Internal functions */

/* (+ operand [operands ...]) */
extern const lisp_obj lisp_add;

/* (- operand [operands ...]) */
extern const lisp_obj lisp_sub;

/* (* operand [operands ...]) */
extern const lisp_obj lisp_mul;

/* (refcount obj) -> number of context that reference obj */
extern const lisp_obj lisp_refcount;

/* (display [objs ...]) -> print objs to stdout */
extern const lisp_obj lisp_display;

extern const lisp_obj lisp_lt;

extern const lisp_obj lisp_dump_env;

#endif
