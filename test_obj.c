#include "lighttest2/lighttest2.h"
#include "env.h"

TEST(test_create_int, {
    lisp_obj *i = lisp_int(42);
    ASSERT(i->type == INT);
    ASSERT(i->value.i == 42);
    destroy_obj(i);
})

TEST(test_add_ints, {
    lisp_obj *i = lisp_int(27);
    lisp_obj *j = lisp_int(15);

    lisp_obj *args[2];
    args[0] = i;
    args[1] = j;

    lisp_obj *sum = add(2, args);
    ASSERT(sum->type == INT);
    ASSERT(sum->value.i == 42);
    
    destroy_obj(sum);
    destroy_obj(i);
    destroy_obj(j);
})

TEST(test_add_intfloat, {
    lisp_obj *i = lisp_int(27);
    lisp_obj *j = lisp_float(15);

    lisp_obj *args[2];
    args[0] = i;
    args[1] = j;

    lisp_obj *sum = add(2, args);
    ASSERT(sum->type == FLOAT);
    ASSERT(sum->value.f == 42.0);
    
    destroy_obj(sum);
    destroy_obj(i);
    destroy_obj(j);
})

TEST(test_add_floats, {
    lisp_obj *i = lisp_float(27);
    lisp_obj *j = lisp_float(15);

    lisp_obj *args[2];
    args[0] = i;
    args[1] = j;

    lisp_obj *sum = add(2, args);
    ASSERT(sum->type == FLOAT);
    ASSERT(sum->value.f == 42.0);
    
    destroy_obj(sum);
    destroy_obj(i);
    destroy_obj(j);
})

SUITE(
    ADDTEST(test_create_int),
    ADDTEST(test_add_ints),
    ADDTEST(test_add_floats),
    ADDTEST(test_add_intfloat))

