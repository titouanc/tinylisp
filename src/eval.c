#include "eval.h"
#include "utils.h"
#include <assert.h>

lisp_obj *apply(lisp_expr_application *app, lisp_env *env, lisp_err *err)
{
    lisp_obj *callable = eval_expression(app->proc, env, err);
    assert(callable != NULL);

    lisp_obj *res = NIL;

    /* Internal procedure */
    if (callable->type == PROC){
        /* Eval args */
        lisp_obj **args = calloc(app->nparams, sizeof(lisp_obj*));
        for (size_t i=0; i<app->nparams; i++){
            args[i] = eval_expression(app->params[i], env, err);
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
        if (app->nparams != lambda->nparams){
            ERROR("Arity error ! Expected %d params, got %d", lambda->nparams, app->nparams);
        }

        /* Extend env */
        lisp_env *locals = create_env(env);
        for (size_t i=0; i<lambda->nparams; i++){
            release(set_env(locals, lambda->param_names[i], 
                eval_expression(app->params[i], env, err)));
        }

        /* Eval body */
        res = eval_expression(lambda->body, locals, err);

        /* cleanup env */
        destroy_env(locals);
    }
    else {
        lisp_print(callable);
        ERROR("CANNOT CALL obj %p", callable);
    }

    release(callable);
    return res;
}

lisp_obj *eval_expression(lisp_expr *expr, lisp_env *env, lisp_err *err)
{
    lisp_obj *value = NULL;
    assert(expr != NULL);
    switch (expr->type){
        case SELFEVAL:
            value = expr->value.selfeval.value;
            return retain(value);
        case LOOKUP:
            value = lookup(env, expr->value.lookup.name);
            if (! value){
                ERROR("Unknow identifier %s", expr->value.lookup.name);
            }
            return retain(value);
        case APPLICATION:
            return apply(&(expr->value.application), env, err);
        case DEFINE:
            release(set_env(env, expr->value.define.name, eval_expression(
                expr->value.define.expr, env, err)));
        default:
            return NIL;
    }
}

lisp_obj *eval(const char *str, lisp_env *env, lisp_err *err)
{
    const char *end = str;
    lisp_expr *expr = analyze(str, &end, err);
    lisp_obj *res = eval_expression(expr, env, err);
    release_expr(expr);
    return res;
}
