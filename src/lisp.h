#ifndef DEFINE_TINYLISP_HEADER
#define DEFINE_TINYLISP_HEADER

/*
 * The main lisp header
 */

#include "env.h"
#include "eval.h"

/*
 * Lisp procedure to quit the interpreter
 */
lisp_obj *lisp_exit(size_t argc, lisp_obj *argv);

/*
 * Populate env with standard functions
 */
void stdenv(lisp_env *env);

#endif
