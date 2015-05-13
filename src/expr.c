#include "expr.h"
#include "utils.h"
#include "env.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define LISP_MAX_PARAMS 10

lisp_expr *create_expr(lisp_expr_type type)
{
    lisp_expr *res = calloc(1, sizeof(lisp_expr));
    res->type = type;
    res->refcount = 0;
    return res;
}

static void destroy_expr(lisp_expr *expr)
{
    assert(expr != NULL);
    assert(expr->refcount == 0);

    switch (expr->type){
        case SELFEVAL:
            release(expr->value.selfeval.value);
            break;
        case APPLICATION:
            release_expr(expr->value.application.proc);
            for (size_t i=0; i<expr->value.application.nparams; i++){
                release_expr(expr->value.application.params[i]);
            }
            free(expr->value.application.params);
            break;
        case LOOKUP:
            free((void*) expr->value.lookup.name);
            break;
    }
    memset(expr, 0, sizeof(lisp_expr));
    free(expr);
}

lisp_expr *release_expr(lisp_expr *expr)
{
    assert(expr != NULL);
    if (expr->refcount == 0){
        destroy_expr(expr);
        return NULL;
    }
    else if (expr->refcount > 0){
        expr->refcount--;
    }
    return expr;
}

lisp_expr *retain_expr(lisp_expr *expr)
{
    assert(expr != NULL);
    if (expr->refcount >= 0){
        expr->refcount++;
    }
    return expr;
}

static void dump_application(lisp_expr_application *app)
{
    printf("(");
    dump_expr(app->proc);
    for (size_t i=0; i<app->nparams; i++){
        printf(" ");
        dump_expr(app->params[i]);
    }
    printf(")");
}

static void dump_lambda(lisp_lambda *lambda)
{
    printf("(lambda (");
    for (size_t i=0; i<lambda->nparams; i++){
        if (i > 0) printf(" ");
        printf("%s", lambda->param_names[i]);
    }
    printf(") ");
    dump_expr(lambda->body);
    printf(")");
}

void dump_expr(lisp_expr *expr)
{
    assert(expr != NULL);
    switch (expr->type){
        case SELFEVAL:
            if (expr->value.selfeval.value->type == LAMBDA){
                dump_lambda(&(expr->value.selfeval.value->value.l));
            } else {
                lisp_print(expr->value.selfeval.value);
            }
            break;
        case APPLICATION:
            dump_application(&(expr->value.application));
            break;
        case LOOKUP:
            printf("%s", (expr->value.lookup.name));
            break;
    }
}

static inline const char *ignore(const char *s)
{
    assert(s != NULL);
    while (*s != '\0' && strchr(" \t\r\n", *s)) s++;
    return s;
}

static lisp_expr *analyze_number(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    double val = strtod(str, (char**) endptr);

    lisp_expr *res = create_expr(SELFEVAL);

    if (val == (double) ((int) val)){
        res->value.selfeval.value = lisp_int(val);
    } 
    else {
        res->value.selfeval.value = lisp_float(val);
    }

    return res;
}

static lisp_expr *analyze_application(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    lisp_expr *res = create_expr(APPLICATION);
    res->value.application.proc = analyze(ignore(str+1), endptr, err);
    
    lisp_expr *params[LISP_MAX_PARAMS];

    int i;

    str = ignore(*endptr);
    for (i=0; i<LISP_MAX_PARAMS && *str != ')'; i++){
        if (*str == '\0'){
            ERROR("Unexpected end of string");
        }
        params[i] = analyze(str, endptr, err);
        str = ignore(*endptr);
    }
    *endptr = str + 1;

    res->value.application.nparams = i;
    res->value.application.params = calloc(i, sizeof(lisp_expr));
    memcpy(res->value.application.params, params, i*sizeof(lisp_expr));

    return res;
}

static lisp_expr *analyze_constant(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    lisp_expr *res = create_expr(SELFEVAL);
    res->value.selfeval.value = NIL;

    if (str[1] == 't'){
        *endptr = str+2;
        res->value.selfeval.value = TRUE;
    }
    else if (str[1] == 'f'){
        *endptr = str+2;
        res->value.selfeval.value = FALSE;
    }
    else if (err){
        *err = UNEXPECTED_TOKEN;
    }
    return res;
}

#define is_name_char(x) (! strchr(" \r\n\t()", (x)))
static lisp_expr *analyze_lookup(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    char name[LISP_MAX_NAME_SIZE];
    size_t i;
    for (i=0; i<sizeof(name)-1 && str[i] != '\0' && is_name_char(str[i]); i++){
        name[i] = str[i];
    }
    name[i] = '\0';
    *endptr = str+i;

    lisp_expr *res = create_expr(LOOKUP);
    res->value.lookup.name = duplicate_string(name, LISP_MAX_NAME_SIZE);

    return res;
}

static lisp_expr *analyze_lambda(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    str = ignore(str);
    *endptr = str;

    if (*str != '('){
        ERROR("Missing lambda param list");
    }
    else {
        str++;
        char param_names[LISP_MAX_PARAMS][LISP_MAX_NAME_SIZE];
        int i, n;
        for (n=0; n<LISP_MAX_PARAMS-1 && *str != ')'; n++){
            for (i=0; i<LISP_MAX_NAME_SIZE && is_name_char(str[i]); i++){
                param_names[n][i] = str[i];
            }
            param_names[n][i] = '\0';
            *endptr = str+i;
            str = ignore(*endptr);
        }

        if (*str != ')'){
            ERROR("Unterminated argument names list");
        }

        lisp_expr *body = analyze(str+1, endptr, err);
        *endptr = *endptr + 1;

        lisp_obj *lambda = create_empty_obj(LAMBDA);
        lambda->value.l.nparams = n;
        lambda->value.l.param_names = calloc(n, sizeof(char*));
        for (i=0; i<n; i++){
            lambda->value.l.param_names[i] = duplicate_string(param_names[i], LISP_MAX_NAME_SIZE);
        }
        lambda->value.l.body = body;

        lisp_expr *res = create_expr(SELFEVAL);
        res->value.selfeval.value = lambda;
        return res;
    }
    return NULL;
}

#define is_number(x) ('0' <= x && x <= '9')
lisp_expr *analyze(const char *str, const char **endptr, lisp_err *err)
{
    lisp_expr *res = NULL;

    DEBUG("ANALYZE \"%s\"", str);
    str = ignore(str);
    if (is_number(*str) || (*str == '-' && is_number(str[1]))){
        res = analyze_number(str, endptr, err);
    }
    else if (*str == '('){
        if (strncmp(str+1, "lambda ", 7) == 0){
            res = analyze_lambda(str+8, endptr, err);
        } else {
            res = analyze_application(str, endptr, err);
        }
    }
    else if (*str == '#'){
        res = analyze_constant(str, endptr, err);
    }
    else {
        res = analyze_lookup(str, endptr, err);
    }

    return res;
}
