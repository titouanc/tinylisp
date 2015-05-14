#include "lighttest2.h"
#include "env.h"
#include "eval.h"
#include "internals.h"

static lisp_env *env = NULL;

static inline void before(){env = create_env(NULL);}
static inline void after(){clear_env(env); env = release_env(env);}

#define ENVTEST(name, statements) TEST(name, {before(); statements; after();})


ENVTEST(test_eval_int, {
    lisp_obj *res = eval("3", env, NULL);

    PRINT("test_eval_int 1");

    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 3);

    PRINT("test_eval_int 2");
    release(res);
    PRINT("test_eval_int 3");
})

ENVTEST(test_eval_float, {
    lisp_obj *res = eval("3.14", env, NULL);

    ASSERT(res != NULL);
    ASSERT(res->type == FLOAT);
    ASSERT(res->value.f == 3.14);
    release(res);
})

ENVTEST(test_eval_const, {
    ASSERT(eval("#t", env, NULL) == TRUE);
    ASSERT(eval("#f", env, NULL) == FALSE);
    ASSERT(eval("#n", env, NULL) == NIL);
})

ENVTEST(test_env_lookup, {
    lisp_obj *obj = lisp_int(42);
    release(set_env(env, "key", obj));

    lisp_obj *res = eval("key", env, NULL);
    ASSERT(res == obj);
    release(res);
})

ENVTEST(test_eval_application, {
    set_env(env, "+", (lisp_obj*) &lisp_add);

    lisp_obj *res = eval("(+ 27 15)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 42);

    release(res);
})

ENVTEST(test_eval_lambda, {
    lisp_obj *obj = eval("(lambda (x) x)", env, NULL);

    PRINT("lambda should not be destroyed here");

    ASSERT(obj->type == LAMBDA);

    lisp_expr_lambda *lambda = &(obj->value.l.declaration->value.mklambda);
    ASSERT(lambda->nparams == 1);
    ASSERT(streq(lambda->param_names[0], "x"));
    release(obj);
})

ENVTEST(test_eval_call_lambda, {
    lisp_obj *obj = eval("((lambda (x) x) 3)", env, NULL);

    PRINT("obj should not be destroyed here, but lambda should");

    ASSERT(obj != NULL);
    ASSERT(obj->type == INT);
    ASSERT(obj->value.i == 3);
    release(obj);
})

ENVTEST(test_eval_long_sum, {
    set_env(env, "+", (lisp_obj*) &lisp_add);
    lisp_obj *obj = eval("(+ 1 2 (+ 3 4) 5 6)", env, NULL);

    ASSERT(obj != NULL);
    ASSERT(obj->type == INT);
    ASSERT(obj->value.i == 21);

    release(obj);
})

ENVTEST(test_eval_define, {
    lisp_obj *res = eval("(define a 3)", env, NULL);
    ASSERT(res == NIL);

    lisp_obj *a = lookup(env, "a");
    ASSERT(a != NULL);
    ASSERT(a->type == INT);
    ASSERT(a->value.i == 3);
})

ENVTEST(test_eval_define_and_call, {
    set_env(env, "*", (lisp_obj*) &lisp_mul);
    lisp_obj *res = eval("(define sq (lambda (x) (* x x)))", env, NULL);
    ASSERT(res == NIL);

    ASSERT(lookup(env, "sq") != NULL);

    res = eval("(sq 3)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 9);
    release(res);
})

ENVTEST(test_eval_higher_order, {
    set_env(env, "+", (lisp_obj*) &lisp_add);

    /* plus : x -> (y -> x+y) */
    lisp_obj *res = eval("(define plus (lambda (x) (lambda (y) (+ x y))))", env, NULL);
    ASSERT(res == NIL);

    res = eval("(define +3 (plus 3))", env, NULL);
    ASSERT(res == NIL);

    res = eval("(+3 39)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 42);
    release(res);
})

ENVTEST(test_eval_condition, {
    lisp_obj *res = eval("(if #t 1 2)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 1);
    release(res);

    res = eval("(if #f 1 2)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 2);
    release(res);
})

ENVTEST(test_eval_double_condition, {
    lisp_obj *res = eval("(if #t (if #t 1 2) 3)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 1);
    release(res);

    res = eval("(if #t (if #f 1 2) 3)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 2);
    release(res);

    res = eval("(if #f (if #f 1 2) 3)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 3);
    release(res);
})

ENVTEST(test_eval_string, {
    lisp_obj *res = eval("\"Hello world\"", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == STRING);
    PRINT("String is \"%s\"", res->value.s);
    ASSERT(streq(res->value.s, "Hello world"));
    release(res);
})

SUITE(
    ADDTEST(test_eval_int),
    ADDTEST(test_eval_float),
    ADDTEST(test_eval_const),
    ADDTEST(test_env_lookup),
    ADDTEST(test_eval_application),
    ADDTEST(test_eval_lambda),
    ADDTEST(test_eval_call_lambda),
    ADDTEST(test_eval_long_sum),
    ADDTEST(test_eval_define),
    ADDTEST(test_eval_define_and_call),
    ADDTEST(test_eval_higher_order),
    ADDTEST(test_eval_condition),
    ADDTEST(test_eval_double_condition),
    ADDTEST(test_eval_string))

