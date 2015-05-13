#include "eval.h"
#include "utils.h"
#include <string.h>
#include <assert.h>

typedef enum {
    SELFEVAL,
    APPLICATION
} lisp_expr_type;

typedef struct lisp_expr_t lisp_expr;

typedef struct {
    lisp_expr *proc;
    size_t nparams;
    lisp_expr **params;
} lisp_expr_application;

typedef struct {
    lisp_obj *value;
} lisp_expr_selfeval;

struct lisp_expr_t {
    lisp_expr_type type;
    union {
        lisp_expr_selfeval selfeval;
        lisp_expr_application application;
    } value;
};

static lisp_obj *eval_internal(
    const char *str, 
    const char **endptr, 
    lisp_env *env, 
    lisp_err *err
);

static inline const char *ignore(const char *s)
{
    assert(s != NULL);
    while (*s != '\0' && strchr(" \t\r\n", *s)) s++;
    return s;
}

static lisp_obj *eval_number(
    const char *str, 
    const char **endptr, 
    lisp_env *env, 
    lisp_err *err
){
    double val = strtod(str, (char**) endptr);

    DEBUG(" => NUMBER %lf", val);

    if (val == (double) ((int) val)){
        return lisp_int(val);
    } 
    else {
        return lisp_float(val);
    }
}

static lisp_obj *eval_application(
    const char *str, 
    const char **endptr, 
    lisp_env *env, 
    lisp_err *err
){
    DEBUG(" => Application");

    lisp_obj *res = NULL;
    lisp_obj *func = eval_internal(str+1, endptr, env, err);
    lisp_obj *params[1000];
    int i;

    str = ignore(*endptr);
    for (i=0; i<1000 && *str != ')'; i++){
        if (*str == '\0')
            ERROR("Unexpected end of string");
        params[i] = eval_internal(str, endptr, env, err);
        str = ignore(*endptr);
    }
    endptr = &str;

    if (func->type == PROC){
        res = func->value.p(i, params);
        while (i > 0){
            i--;
            destroy_obj(params[i]);
        }
    }
    else if (func->type == LAMBDA) {
        lisp_env *locals = create_env(env);
        size_t nArgs = func->value.l.n_params;
        char **names = func->value.l.param_names;

        for (size_t j=0; j<nArgs; j++){
            set_env(locals, names[j], params[j]);
        }
        res = func->value.l.proc(locals);
        destroy_env(locals);
    }
    return res;
}

static lisp_obj *eval_constant(
    const char *str, 
    const char **endptr, 
    lisp_env *env, 
    lisp_err *err
){
    DEBUG(" => Constant");
    if (str[1] == 't'){
        *endptr = str+2;
        return TRUE;
    }
    else if (str[1] == 'f'){
        *endptr = str+2;
        return FALSE;
    }
    else if (str[1] == 'n'){
        *endptr = str+2;
        return NIL;
    }
    else if (err){
        *err = UNEXPECTED_TOKEN;
    }
    return NULL;
}

#define is_name_char(x) (! strchr(" \r\n\t()", (x)))
static lisp_obj *eval_lookup(
    const char *str, 
    const char **endptr, 
    lisp_env *env, 
    lisp_err *err
){
    char name[LISP_MAX_NAME_SIZE];
    size_t i;
    for (i=0; i<sizeof(name)-1 && str[i] != '\0' && is_name_char(str[i]); i++){
        name[i] = str[i];
    }
    name[i] = '\0';
    *endptr = str+i;

    DEBUG(" => lookup \"%s\"", name);
    return lookup(env, name);
}

static lisp_obj *eval_internal(
    const char *str, 
    const char **endptr, 
    lisp_env *env, 
    lisp_err *err
){
    DEBUG("EVAL %s", str);
    str = ignore(str);
    if (*str == '-' || ('0' <= *str && *str <= '9')){
        return eval_number(str, endptr, env, err);
    }
    else if (*str == '('){
        return eval_application(str, endptr, env, err);
    }
    else if (*str == '#'){
        return eval_constant(str, endptr, env, err);
    }
    else {
        return eval_lookup(str, endptr, env, err);
    }

    DEBUG(" => NOT FOUND !!!");
    return NULL;
}

lisp_obj *eval(const char *str, lisp_env *env, lisp_err *err)
{
    const char *end = str;
    return eval_internal(str, &end, env, err);
}
