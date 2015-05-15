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

/* (display [objs ...]) -> number of printed objects */
extern const lisp_obj lisp_display;

/* (< number number [numbers ...]) > #t/#f */
extern const lisp_obj lisp_lt;

/* (dmp env) -> #n Dump environment of given lambda*/
extern const lisp_obj lisp_dump_env;

/* (real number) -> FLOAT */
extern const lisp_obj lisp_real;

#endif
