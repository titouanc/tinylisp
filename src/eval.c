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
        lisp_expr_lambda *lambda_expr = &(lambda->declaration->value.mklambda);

        if (app->nparams != lambda_expr->nparams){
            raise_error(err, WRONG_ARITY, "Arity error ! Expected %d params, got %d",
                lambda_expr->nparams, app->nparams);
            return NULL;
        }

        /* Extend env */
        lisp_env *locals = create_env(lambda->context);
        for (size_t i=0; i<lambda_expr->nparams; i++){
            DEBUG("Extend env with %s\n", lambda_expr->param_names[i]);
            release(set_env(locals, lambda_expr->param_names[i], 
                eval_expression(app->params[i], env, err)));
        }

        if (enable_debug){
            printf("\033[1mCALL\033[0m ");
            dump_expr(lambda_expr->body);
            printf(" with env\n");
            dump_env(locals);
        }

        /* Eval body */
        res = eval_expression(lambda_expr->body, locals, err);

        /* cleanup env */
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

lisp_obj *eval_condition(lisp_expr_condition *expr, lisp_env *env, lisp_err *err)
{
    lisp_obj *cond = eval_expression(expr->condition, env, err);
    if (cond == NULL){
        return NULL;
    }

    lisp_obj *res = eval_expression(
        (cond != NIL && cond != FALSE) ? expr->consequence : expr->alternative,
        env, err);
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
            value = eval_expression(expr->value.define.expr, env, err);
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
    lisp_expr *expr = analyze(str, &end, err);
    if (expr){
        lisp_obj *res = eval_expression(expr, env, err);
        release_expr(expr);
        return res;
    }
    return NULL;
}
