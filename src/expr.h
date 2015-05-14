#ifndef DEFINE_EXPR_HEADER
#define DEFINE_EXPR_HEADER

#include <stdlib.h>

typedef struct lisp_expr_t lisp_expr;

/* A lambda expression */
typedef struct {
    size_t nparams;
    char **param_names;
    lisp_expr *body;
} lisp_expr_lambda;

#include "obj.h"

typedef enum {
    OK=0,
    UNEXPECTED_TOKEN,
    UNKNOW_IDENTIFIER,
    WRONG_ARITY,
    MISSING_ARGNAMES,
    UNTERMINATED_EXPRESSION,
    NOT_CALLABLE
} lisp_err_type;

typedef struct {
    lisp_err_type type;
    const char *description;
} lisp_err;

void raise_error(lisp_err *err, lisp_err_type type, const char *fmt, ...);

/* Expression ADT */
typedef enum {
    SELFEVAL,
    APPLICATION,
    LOOKUP,
    DEFINE,
    CONDITION,
    MKLAMBDA   /* A lambda declaration */
} lisp_expr_type;

/* An application expression (func call) */
typedef struct {
    lisp_expr *proc;
    size_t nparams;
    lisp_expr **params;
} lisp_expr_application;

/* A self-evaluating expression (constant) */
typedef struct {
    lisp_obj *value;
} lisp_expr_selfeval;

/* An env lookup expression */
typedef struct {
    const char *name;
} lisp_expr_lookup;

/* A define/set expression */
typedef struct {
    bool   overwrite; /* overwrite in parent env (!set) if true */
    const char *name; /* symbol to be defined */
    lisp_expr  *expr; /* value to assign */
} lisp_expr_define;

/* An if expression */
typedef struct {
    lisp_expr *condition;
    lisp_expr *consequence;
    lisp_expr *alternative;
} lisp_expr_condition;

/* A lisp expression has a type and a value */
struct lisp_expr_t {
    int refcount;
    lisp_expr_type type;
    union {
        lisp_expr_selfeval       selfeval;
        lisp_expr_application application;
        lisp_expr_lookup           lookup;
        lisp_expr_define           define;
        lisp_expr_condition     condition;
        lisp_expr_lambda         mklambda;
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
