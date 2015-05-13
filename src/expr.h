#ifndef DEFINE_EXPR_HEADER
#define DEFINE_EXPR_HEADER

#include "obj.h"

typedef enum {
    UNEXPECTED_TOKEN
} lisp_err;

/* Expression ADT */
typedef struct lisp_expr_t lisp_expr;
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
    lisp_expr_type type;
    union {
        lisp_expr_selfeval    selfeval;
        lisp_expr_application application;
        lisp_expr_lookup      lookup;
    } value;
};

/* Free an entire expression tree */
void destroy_expr(lisp_expr *expr);

/* Writes out expression in lisp format to stdout */
void dump_expr(lisp_expr *expr);

/* Main method: str -> lisp exression */
lisp_expr *analyze(const char *str, const char **endptr, lisp_err *err);

#endif
