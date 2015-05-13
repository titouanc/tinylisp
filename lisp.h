#ifndef DEFINE_TINYLISP_HEADER
#define DEFINE_TINYLISP_HEADER

#include "env.h"

lisp_obj *lisp_exit(size_t argc, lisp_obj *argv);

void stdenv(lisp_env *env);

#endif
