#include "lighttest2.h"
#include "lisp.h"

#define EVAL(str) eval(str, env, NULL)
static lisp_env *env = NULL;

static inline void before()
{
    env = create_env(NULL);
    stdenv(env);
}

static inline void after()
{
    clear_env(env);
    env = release_env(env);
}

#define ENVTEST(name, statements) TEST(name, {before(); statements; after(); ASSERT(env == NULL);})

ENVTEST(test_eval_car, 
    ASSERT(EVAL("(car (cons #t #f))") == TRUE))

ENVTEST(test_eval_cdr, 
    ASSERT(EVAL("(cdr (cons #t #f))") == FALSE))

ENVTEST(test_eval_cadr, 
    ASSERT(EVAL("(cadr (cons #f (cons #t #f)))") == TRUE))

ENVTEST(test_eval_car_range, {
    lisp_obj *res = EVAL("(car (range 0 10))");
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 0);
    release(res);
})

ENVTEST(test_eval_cadr_range, {
    lisp_obj *res = EVAL("(cadr (range 0 10))");
    ASSERT(res != NULL);
    ASSERT(res->type == INT);
    ASSERT(res->value.i == 1);
    release(res);
})

ENVTEST(test_eval_null_cdr_range,
    ASSERT(EVAL("(cdr (range 0 1))") == NIL))

SUITE(
    ADDTEST(test_eval_car),
    ADDTEST(test_eval_cdr),
    ADDTEST(test_eval_cadr),
    ADDTEST(test_eval_car_range),
    ADDTEST(test_eval_cadr_range),
    ADDTEST(test_eval_null_cdr_range))
