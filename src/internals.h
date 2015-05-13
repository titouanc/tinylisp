#ifndef DEFINE_INTERNALS_HEADER
#define DEFINE_INTERNALS_HEADER

#include "obj.h"

/* Internal functions */

/* (+ operand [operands ...]) */
extern lisp_obj lisp_add;

/* (- operand [operands ...]) */
extern lisp_obj lisp_sub;

/* (* operand [operands ...]) */
extern lisp_obj lisp_mul;

/* (refcount obj) -> number of context that reference obj */
extern lisp_obj lisp_refcount;

#endif
