#include "eval.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>

static lisp_obj *make_lambda(lisp_expr *expr, lisp_env *context)
{
    lisp_obj *res = create_empty_obj(LAMBDA);
    res->value.l.declaration = retain_expr(expr);
    res->value.l.context = retain_env(context);
    return res;
}

static lisp_obj *make_thunk(lisp_expr *expr, lisp_env *env)
{
    lisp_obj *res = create_empty_obj(THUNK);
    res->value.l.declaration = retain_expr(expr);
    res->value.l.context = retain_env(env);
    return res;
}

#define FORCE_VALUE(expr,env,err) trampoline(eval_expression((expr), (env), (err)), (err))

static inline lisp_obj *trampoline(lisp_obj *obj, lisp_err *err)
{
    int i = 0;
    while (obj && obj->type == THUNK){
        lisp_expr *body = obj->value.l.declaration;
        lisp_env *env = obj->value.l.context;
        lisp_obj *res = eval_expression(body, env, err);
        
        if (enable_debug && i > 0){
            printf("Trampolined [%d] -> ", i);
            lisp_print(res);
            printf("\n");
        }

        release(obj);
        obj = res;
        i++;
    }

    if (enable_debug && i > 0){
        printf("=== End of trampoline\n");
    }
    return obj;
}

static lisp_obj *apply(lisp_expr_application *app, lisp_env *env, lisp_err *err)
{
    lisp_obj *callable = FORCE_VALUE(app->proc, env, err);
    if (! callable){
        return NULL;
    }

    lisp_obj *res = NIL;

    /* Internal procedure */
    if (callable->type == PROC){
        /* Eval args */
        lisp_obj **args = calloc(app->nparams, sizeof(lisp_obj*));
        for (size_t i=0; i<app->nparams; i++){
            lisp_obj *arg = FORCE_VALUE(app->params[i], env, err);
            if (! arg){
                for (size_t j=0; j<i; j++){
                    release(args[j]);
                }
                free(args);
                return NULL;
            }
            args[i] = arg;
        }

        /* Eval internal */
        res = callable->value.p(app->nparams, args);
        
        /* Free args */
        for (size_t i=0; i<app->nparams; i++){
            release(args[i]);
        }
        free(args);
    }

    /* Lisp func */
    else if (callable->type == LAMBDA){
        lisp_lambda *lambda = &(callable->value.l);
        lisp_expr_lambda *lambda_expr = &(lambda->declaration->value.mklambda);

        /* Check arity */
        if (app->nparams != lambda_expr->nparams){
            raise_error(err, WRONG_ARITY, "Arity error ! Expected %d params, got %d",
                lambda_expr->nparams, app->nparams);
            return NULL;
        }

        /* Extend env */
        lisp_env *locals = create_env(lambda->context);
        for (size_t i=0; i<lambda_expr->nparams; i++){
            lisp_obj *param = eval_expression(app->params[i], env, err);
            if (! param){
                release_env(locals);
                return NULL;
            }
            DEBUG("Extend env with %s", lambda_expr->param_names[i]);
            release(set_env(locals, lambda_expr->param_names[i], param));
        }

        if (enable_debug){
            printf("\033[1mCALL\033[0m ");
            dump_expr(lambda_expr->body);
            printf(" with env\n");
            dump_env(locals);
        }

        /* Wrap in thunk for trampoline */
        res = make_thunk(lambda_expr->body, locals);
        release_env(locals);
    }
    else {
        lisp_print(callable);
        raise_error(err, NOT_CALLABLE, "CANNOT CALL obj %p", callable);
        return NULL;
    }

    release(callable);
    return res;
}

static lisp_obj *eval_condition(lisp_expr_condition *expr, lisp_env *env, lisp_err *err)
{
    lisp_obj *cond = FORCE_VALUE(expr->condition, env, err);
    if (cond == NULL){
        return NULL;
    }

    lisp_expr *next = (cond != NIL && cond != FALSE) ? expr->consequence : expr->alternative;
    lisp_obj *res = make_thunk(next, env);
    release(cond);
    return res;
}

lisp_obj *eval_expression(lisp_expr *expr, lisp_env *env, lisp_err *err)
{
    lisp_obj *value = NULL;
    assert(expr != NULL);

    switch (expr->type){
        case MKLAMBDA:
            return make_lambda(expr, env);

        case SELFEVAL:
            value = expr->value.selfeval.value;
            if (! value){
                return NULL;
            }
            return retain(value);

        case LOOKUP:
            value = lookup(env, expr->value.lookup.name);
            if (! value){
                raise_error(err, UNKNOW_IDENTIFIER, "Unknow identifier \"%s\"", expr->value.lookup.name);
                return NULL;
            }
            return retain(value);

        case APPLICATION:
            return apply(&(expr->value.application), env, err);

        case CONDITION:
            return eval_condition(&(expr->value.condition), env, err);

        case DEFINE:
            value = FORCE_VALUE(expr->value.define.expr, env, err);
            if (! value){
                return NULL;
            }
            release(set_env(env, expr->value.define.name, value));

        default:
            return NIL;
    }
}


lisp_obj *eval(const char *str, lisp_env *env, lisp_err *err)
{
    const char *end = str;
    DEBUG("EVAL \"%s\"", str);
    lisp_expr *expr = analyze(str, &end, err);
    if (expr){
        lisp_obj *res = FORCE_VALUE(expr, env, err);
        release_expr(expr);
        return res;
    }
    return NULL;
}
