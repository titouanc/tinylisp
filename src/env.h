#ifndef DEFINE_ENV_HEADER
#define DEFINE_ENV_HEADER

#include "obj.h"

#define LISP_MAX_NAME_SIZE 200

lisp_env *create_env(lisp_env *parent);

lisp_obj *set_env(lisp_env *env, const char *name, lisp_obj *value);

lisp_env *destroy_env(lisp_env *env);

lisp_obj *lookup(lisp_env *env, const char *name);

#endif
