#include "lighttest2.h"
#include "env.h"
#include "eval.h"
#include "internals.h"

TEST(test_eval_int, {
    lisp_env *env = create_env(NULL);
    lisp_obj *res = eval("3", env, NULL);

    PRINT("test_eval_int 1");

    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 3);

    PRINT("test_eval_int 2");
    release(res);
    PRINT("test_eval_int 3");

    destroy_env(env);
})

TEST(test_eval_float, {
    lisp_env *env = create_env(NULL);
    lisp_obj *res = eval("3.14", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == FLOAT);
    ASSERT(res->value.f == 3.14);
    release(res);
    destroy_env(env);
})

TEST(test_eval_const, {
    lisp_env *env = create_env(NULL);

    ASSERT(eval("#t", env, NULL) == TRUE);
    ASSERT(eval("#f", env, NULL) == FALSE);
    ASSERT(eval("#n", env, NULL) == NIL);

    destroy_env(env);
})

TEST(test_env_lookup, {
    lisp_env *env = create_env(NULL);
    lisp_obj *obj = lisp_int(42);
    release(set_env(env, "key", obj));

    lisp_obj *res = eval("key", env, NULL);
    ASSERT(res == obj);

    destroy_env(env);
})

TEST(test_eval_application, {
    lisp_env *env = create_env(NULL);
    set_env(env, "+", &lisp_add);

    lisp_obj *res = eval("(+ 27 15)", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 42);

    release(res);
    destroy_env(env);
})

TEST(test_eval_lambda, {
    lisp_env *env = create_env(NULL);
    lisp_obj *obj = eval("(lambda (x) x)", env, NULL);

    PRINT("lambda should not be destroyed here");

    ASSERT(obj->type == LAMBDA);
    ASSERT(obj->value.l.nparams == 1);
    ASSERT(streq(obj->value.l.param_names[0], "x"));
    release(obj);
    destroy_env(env);
})

TEST(test_eval_call_lambda, {
    lisp_env *env = create_env(NULL);
    lisp_obj *obj = eval("((lambda (x) x) 3)", env, NULL);

    PRINT("obj should not be destroyed here, but lambda should");

    ASSERT(obj != NULL);
    ASSERT(obj->type == INT);
    ASSERT(obj->value.i == 3);
    release(obj);
    destroy_env(env);
})


TEST(test_eval_long_sum, {
    lisp_env *env = create_env(NULL);
    set_env(env, "+", &lisp_add);
    lisp_obj *obj = eval("(+ 1 2 (+ 3 4) 5 6)", env, NULL);

    ASSERT(obj != NULL);
    ASSERT(obj->type == INT);
    ASSERT(obj->value.i == 21);

    release(obj);
    destroy_env(env);
})

SUITE(
    ADDTEST(test_eval_int),
    ADDTEST(test_eval_float),
    ADDTEST(test_eval_const),
    ADDTEST(test_env_lookup),
    ADDTEST(test_eval_application),
    ADDTEST(test_eval_lambda),
    ADDTEST(test_eval_call_lambda),
    ADDTEST(test_eval_long_sum))

