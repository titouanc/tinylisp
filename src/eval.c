#include "eval.h"
#include "utils.h"
#include <assert.h>

lisp_obj *apply(lisp_expr_application *app, lisp_env *env, lisp_err *err)
{
    lisp_obj *proc = eval_expression(app->proc, env, err);
    assert(proc != NULL);

    lisp_obj *res = NIL;

    if (proc->type == PROC){
        /* Eval args */
        lisp_obj **args = calloc(app->nparams, sizeof(lisp_obj*));
        for (size_t i=0; i<app->nparams; i++){
            args[i] = eval_expression(app->params[i], env, err);
        }

        /* Eval internal */
        res = proc->value.p(app->nparams, args);
        
        /* Free args */
        for (size_t i=0; i<app->nparams; i++){
            destroy_obj(args[i]);
        }
        free(args);
    }
    else if (proc->type == LAMBDA){
        lisp_lambda *lambda = &(proc->value.l);
        if (app->nparams != lambda->nparams){
            ERROR("Arity error ! Expected %d params, got %d", lambda->nparams, app->nparams);
        }

        /* Extend env */
        lisp_env *locals = create_env(env);
        for (size_t i=0; i<lambda->nparams; i++){
            set_env(locals, lambda->param_names[i], eval_expression(app->params[i], env, err));
        }

        /* Eval body */

        /* cleanup env */
        destroy_env(locals);
    }
    else {
        ERROR("CANNOT CALL obj %p", proc);
        lisp_print(proc);
    }

    return res;
}

lisp_obj *eval_expression(lisp_expr *expr, lisp_env *env, lisp_err *err)
{
    lisp_obj *value = NULL;
    assert(expr != NULL);
    switch (expr->type){
        case SELFEVAL:
            value = expr->value.selfeval.value;
            return (value->_static) ? value : create_obj(value->type, value->value);
        case LOOKUP:
            return lookup(env, expr->value.lookup.name);
        case APPLICATION:
            return apply(&(expr->value.application), env, err);
        default:
            return NIL;
    }
}

lisp_obj *eval(const char *str, lisp_env *env, lisp_err *err)
{
    const char *end = str;
    lisp_expr *expr = analyze(str, &end, err);
    lisp_obj *res = eval_expression(expr, env, err);
    destroy_expr(expr);
    return res;
}
