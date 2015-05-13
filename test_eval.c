#include "lighttest2/lighttest2.h"
#include "env.h"
#include "eval.h"
#include "internals.h"

TEST(test_eval_int, {
    lisp_env *env = create_env(NULL);
    lisp_obj *res = eval("3", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 3);
    destroy_obj(res);
    destroy_env(env);
})

TEST(test_eval_float, {
    lisp_env *env = create_env(NULL);
    lisp_obj *res = eval("3.14", env, NULL);
    ASSERT(res != NULL);
    ASSERT(res->type == FLOAT);
    ASSERT(res->value.f == 3.14);
    destroy_obj(res);
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
    set_env(env, "key", obj);

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

    destroy_obj(res);
    destroy_env(env);
})

SUITE(
    ADDTEST(test_eval_int),
    ADDTEST(test_eval_float),
    ADDTEST(test_eval_const),
    ADDTEST(test_env_lookup),
    ADDTEST(test_eval_application))

