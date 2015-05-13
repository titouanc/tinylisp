#include "expr.h"
#include "utils.h"
#include "env.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define LISP_MAX_PARAMS 1000

void destroy_expr(lisp_expr *expr)
{
    assert(expr != NULL);
    switch (expr->type){
        case SELFEVAL:
            destroy_obj(expr->value.selfeval.value);
            break;
        case APPLICATION:
            destroy_expr(expr->value.application.proc);
            for (size_t i=0; i<expr->value.application.nparams; i++){
                destroy_expr(expr->value.application.params[i]);
            }
            free(expr->value.application.params);
            break;
        case LOOKUP:
            free((void*) expr->value.lookup.name);
            break;
    }
    free(expr);
}

void dump_expr(lisp_expr *expr)
{
    assert(expr != NULL);
    switch (expr->type){
        case SELFEVAL:
            lisp_print(expr->value.selfeval.value);
            break;
        case APPLICATION:
            printf("(");
            dump_expr(expr->value.application.proc);
            for (size_t i=0; i<expr->value.application.nparams; i++){
                printf(" ");
                dump_expr(expr->value.application.params[i]);
            }
            printf(")");
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

    lisp_expr *res = calloc(1, sizeof(lisp_expr));
    res->type = SELFEVAL;

    if (val == (double) ((int) val)){
        res->value.selfeval.value = lisp_int(val);
        DEBUG(" -> INT %ld", (long int) val);
    } 
    else {
        res->value.selfeval.value = lisp_float(val);
        DEBUG(" -> FLOAT %lf", val);
    }

    return res;
}

static lisp_expr *analyze_application(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    lisp_expr *res = calloc(1, sizeof(lisp_expr));
    res->type = APPLICATION;
    res->value.application.proc = analyze(ignore(str+1), endptr, err);
    
    lisp_expr *params[LISP_MAX_PARAMS];

    int i;

    str = ignore(*endptr);
    for (i=0; i<1000 && *str != ')'; i++){
        if (*str == '\0'){
            ERROR("Unexpected end of string");
        }
        params[i] = analyze(str, endptr, err);
        str = ignore(*endptr);
    }
    endptr = &str;

    res->value.application.nparams = i;
    res->value.application.params = calloc(i, sizeof(lisp_expr));
    memcpy(res->value.application.params, params, i*sizeof(lisp_expr));

    DEBUG(" -> Application (%d params)", res->value.application.nparams);

    return res;
}

static lisp_expr *analyze_constant(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    lisp_expr *res = calloc(1, sizeof(lisp_expr));
    res->type = SELFEVAL;
    res->value.selfeval.value = NIL;

    DEBUG(" -> CONSTANT");

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

    lisp_expr *res = calloc(1, sizeof(lisp_expr));
    res->type = LOOKUP;
    res->value.lookup.name = duplicate_string(name, LISP_MAX_NAME_SIZE);

    DEBUG(" -> LOOKUP \"%s\"", name);
    return res;
}

#define is_number(x) ('0' <= x && x <= '9')
lisp_expr *analyze(const char *str, const char **endptr, lisp_err *err)
{
    DEBUG("ANALYZE \"%s\"", str);
    str = ignore(str);
    if (is_number(*str) || (*str == '-' && is_number(str[1]))){
        return analyze_number(str, endptr, err);
    }
    else if (*str == '('){
        return analyze_application(str, endptr, err);
    }
    else if (*str == '#'){
        return analyze_constant(str, endptr, err);
    }
    else {
        return analyze_lookup(str, endptr, err);
    }

    DEBUG(" -> NOT FOUND !!!");
    return NULL;
}
