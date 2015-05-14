#ifndef DEFINE_ENV_HEADER
#define DEFINE_ENV_HEADER

/*
 * An lisp environment is a key:val association.
 * An environment can extend another one, and can be referenced by several
 * objects or environments.
 */

#include "obj.h"

#define LISP_MAX_NAME_SIZE 200

/* Create a new env, which extends parent (NULL if root env frame) */
lisp_env *create_env(lisp_env *parent);

/* 
 * Insert object in environment, return inserted object
 * IDIOM: release(set_env(env, "key", obj)): give responsibility of obj to env
 */
lisp_obj *set_env(lisp_env *env, const char *name, lisp_obj *value);

/* Decrement refcount if still referenced, otherwise destroy env frame */
lisp_env *release_env(lisp_env *env);

/* Increment refcount (mark as referenced by me) */
lisp_env *retain_env(lisp_env *env);

/* Lookup for name in env, return found object or NULL */
lisp_obj *lookup(lisp_env *env, const char *name);

/* release all env content */
void clear_env(lisp_env *env);

/* Prints out environment to stdout. Return the number of traversed frames */
int dump_env(lisp_env *env);

#endif
