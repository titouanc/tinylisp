#ifndef DEFINE_EXPR_HEADER
#define DEFINE_EXPR_HEADER

typedef struct lisp_expr_t lisp_expr;

#include "obj.h"

typedef enum {
    NO_ERR=0,
    UNEXPECTED_TOKEN
} lisp_err;

/* Expression ADT */
typedef enum {
    SELFEVAL,
    APPLICATION,
    LOOKUP
} lisp_expr_type;

/* An application expression */
typedef struct {
    lisp_expr *proc;
    size_t nparams;
    lisp_expr **params;
} lisp_expr_application;

/* A self-evaluating expression */
typedef struct {
    lisp_obj *value;
} lisp_expr_selfeval;

/* An env lookup expression */
typedef struct {
    const char *name;
} lisp_expr_lookup;

/* A lisp expression has a type and a value */
struct lisp_expr_t {
    int refcount;
    lisp_expr_type type;
    union {
        lisp_expr_selfeval    selfeval;
        lisp_expr_application application;
        lisp_expr_lookup      lookup;
    } value;
};

/* Create a new expression of given type */
lisp_expr *create_expr(lisp_expr_type type);

/* Free an entire expression tree */
lisp_expr *release_expr(lisp_expr *expr);

/* Retain an entire expression tree */
lisp_expr *retain_expr(lisp_expr *expr);

/* Writes out expression in lisp format to stdout */
void dump_expr(lisp_expr *expr);

/* Main method: str -> lisp exression */
lisp_expr *analyze(const char *str, const char **endptr, lisp_err *err);

#endif
