#define _GNU_SOURCE
#include "expr.h"
#include "utils.h"
#include "env.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#define LISP_MAX_PARAMS 10

void raise_error(lisp_err *err, lisp_err_type type, const char *fmt, ...)
{
    if (err != NULL){
        va_list args;
        va_start(args, fmt);
        err->type = type;
            if (err->description != NULL){
            free((void*) err->description);
            err->description = NULL;
        }
        int res = vasprintf((char **) &(err->description), fmt, args);
        if (res == -1){
            err->description = NULL;
        }
        va_end(args);
    }
}

void cleanup_error(lisp_err *err)
{
    if (err != NULL){
        if (err->description != NULL){
            free((void *)err->description);
        }
    }
}

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

    if (enable_debug){
        printf("DESTROY \033[34mEXPRESSION\033[0m id=%p ", expr);
        dump_expr(expr);
        printf("\n");
    }

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
        case DEFINE:
            free((void*) expr->value.define.name);
            release_expr(expr->value.define.expr);
            break;
        case MKLAMBDA:
            release_expr(expr->value.mklambda.body);
            for (size_t i=0; i<expr->value.mklambda.nparams; i++){
                free(expr->value.mklambda.param_names[i]);
            }
            free(expr->value.mklambda.param_names);
            break;
        case CONDITION:
            release_expr(expr->value.condition.condition);
            release_expr(expr->value.condition.consequence);
            release_expr(expr->value.condition.alternative);
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
    printf("APPLY(");
    dump_expr(app->proc);
    for (size_t i=0; i<app->nparams; i++){
        printf(" ");
        dump_expr(app->params[i]);
    }
    printf(")");
}

static void dump_lambda(lisp_expr_lambda *lambda)
{
    printf("(");
    for (size_t i=0; i<lambda->nparams; i++){
        if (i > 0) printf(" ");
        printf("%s", lambda->param_names[i]);
    }
    printf(") -> {");
    dump_expr(lambda->body);
    printf("}");
}

void dump_expr(lisp_expr *expr)
{
    assert(expr != NULL);

    switch (expr->type){
        case MKLAMBDA:
            dump_lambda(&expr->value.mklambda);
            break;
        case SELFEVAL:
            printf("SELFEVAL[");
            lisp_print(expr->value.selfeval.value);
            printf("]");
            break;
        case APPLICATION:
            dump_application(&(expr->value.application));
            break;
        case LOOKUP:
            printf("LOOKUP[%s]", (expr->value.lookup.name));
            break;
        case DEFINE:
            printf("(%s %s ", expr->value.define.overwrite ? "set!" : "define", expr->value.define.name);
            dump_expr(expr->value.define.expr);
            printf(")");
            break;
        case CONDITION:
            dump_expr(expr->value.condition.condition);
            printf(" ? <");
            dump_expr(expr->value.condition.consequence);
            printf("> : <");
            dump_expr(expr->value.condition.alternative);
            printf(">");
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
    if (res->value.application.proc == NULL){
        release_expr(res);
        return NULL;
    }
    
    lisp_expr *params[LISP_MAX_PARAMS];

    int i;

    str = ignore(*endptr);
    for (i=0; i<LISP_MAX_PARAMS && *str != ')'; i++){
        if (*str == '\0'){
            raise_error(err, UNTERMINATED_EXPRESSION, "Unexpected end of string");
            return NULL;
        }
        params[i] = analyze(str, endptr, err);
        if (params[i] == NULL){
            for (int j=0; j<i; j++){
                release_expr(params[j]);
            }
            return NULL;
        }
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
        res->value.selfeval.value = TRUE;
    }
    else if (str[1] == 'f'){
        res->value.selfeval.value = FALSE;
    }
    else if (str[1] != 'n'){
        raise_error(err, UNEXPECTED_TOKEN, "Unexpected token '%c' for constant value", str[1]);
    }
    *endptr = str + 2;
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
        raise_error(err, UNTERMINATED_EXPRESSION, "Unterminated argument names list");
        return NULL;
    }

    lisp_expr *body = analyze(str+1, endptr, err);
    if (body == NULL){
        return NULL;
    }
    *endptr = *endptr + 1;

    lisp_expr *res = create_expr(MKLAMBDA);
    res->value.mklambda.body = body;
    res->value.mklambda.nparams = n;
    res->value.mklambda.param_names = calloc(n, sizeof(char*));
    for (int i=0; i<n; i++){
        res->value.mklambda.param_names[i] = duplicate_string(param_names[i], LISP_MAX_NAME_SIZE);
    }
    return res;
}

static lisp_expr *analyze_define(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    char name[LISP_MAX_NAME_SIZE];
    int i;
    str = ignore(str);

    /* (define (func args) body) syntax */
    bool is_lambda = false;
    if (*str == '('){
        is_lambda = true;
        str++;
    }

    for (i=0; i<LISP_MAX_NAME_SIZE-1 && is_name_char(str[i]); i++){
        name[i] = str[i];
    }
    name[i] = '\0';
    str = ignore(str+i);

    lisp_expr *expr = is_lambda ? 
        analyze_lambda(str, endptr, err):
        analyze(str, endptr, err);

    if (expr == NULL){
        return NULL;
    }

    lisp_expr *res = create_expr(DEFINE);
    res->value.define.expr = expr;
    res->value.define.name = duplicate_string(name, LISP_MAX_NAME_SIZE);
    res->value.define.overwrite = false;

    return res;
}

static lisp_expr *analyze_condition(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    lisp_expr *cond = analyze(str, endptr, err);
    if (! cond)
        return NULL;

    lisp_expr *cons = analyze(*endptr, endptr, err);
    if (! cons){
        release_expr(cond);
        return NULL;
    }

    lisp_expr *alt = NULL;

    /* No alternative */
    if (**endptr == ')'){
        alt = create_expr(SELFEVAL);
        alt->value.selfeval.value = NIL;
    } else {
        alt = analyze(*endptr, endptr, err);
        if (alt == NULL){
            release_expr(cond);
            release_expr(cons);
            return NULL;
        }
    }

    *endptr = *endptr+1;

    lisp_expr *res = create_expr(CONDITION);
    res->value.condition.condition = cond;
    res->value.condition.consequence = cons;
    res->value.condition.alternative = alt;
    return res;
}

static lisp_expr *analyze_string(
    const char *str,
    const char **endptr,
    lisp_err *err
){
    char string[LISP_MAX_STRING_SIZE];
    str++;

    int i;
    for (i=0; i<LISP_MAX_NAME_SIZE-1 && str[i] != '"'; i++){
        string[i] = str[i];
    }
    string[i] = '\0';
    *endptr = str+i+1;

    lisp_expr *res = create_expr(SELFEVAL);
    res->value.selfeval.value = lisp_string(string);
    return res;
}

#define is_number(x) ('0' <= x && x <= '9')
lisp_expr *analyze(const char *str, const char **endptr, lisp_err *err)
{
    lisp_expr *res = NULL;
    str = ignore(str);
    if (is_number(*str) || (*str == '-' && is_number(str[1]))){
        res = analyze_number(str, endptr, err);
    }

    /* Special form or application */
    else if (*str == '('){
        if (strncmp(str+1, "lambda ", 7) == 0){
            str = ignore(str+8);
            if (*str != '('){
                raise_error(err, MISSING_ARGNAMES, "Missing argument list");
            } else {
                res = analyze_lambda(str + 1, endptr, err);
            }
        }
        else if (strncmp(str+1, "define ", 7) == 0){
            res = analyze_define(str+8, endptr, err);
        }
        else if (strncmp(str+1, "if ", 3) == 0){
            res = analyze_condition(str+3, endptr, err);
        }
        else {
            res = analyze_application(str, endptr, err);
        }
    }
    else if (*str == '"'){
        res = analyze_string(str, endptr, err);
    }
    else if (*str == '#'){
        res = analyze_constant(str, endptr, err);
    }
    else if (*str == '\0'){
        raise_error(err, NO_EXPRESSION, "No expression given");
    }
    else {
        res = analyze_lookup(str, endptr, err);
    }

    return res;
}
