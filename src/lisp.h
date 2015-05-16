#ifndef DEFINE_TINYLISP_HEADER
#define DEFINE_TINYLISP_HEADER

/*
 * The main interpreter header
 */

#include "env.h"
#include "eval.h"

/* Exit procedure */
extern lisp_obj lisp_exit;

/*
 * Populate env with standard functions
 */
bool stdenv(lisp_env *env);

#endif
