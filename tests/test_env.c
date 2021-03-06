#include "lighttest2.h"
#include "env.h"

TEST(test_create_env, {
    lisp_env *env = create_env(NULL);
    ASSERT(env != NULL);
    
    lisp_env *res = release_env(env);
    ASSERT(res == NULL);
})

TEST(test_env_content, {
    lisp_env *env = create_env(NULL);
    lisp_obj *obj1 = lisp_int(42);
    lisp_obj *obj2 = lisp_int(22);

    release(set_env(env, "key1", obj1));
    release(set_env(env, "key2", obj2));

    ASSERT(lookup(env, "key1") == obj1);
    ASSERT(lookup(env, "key2") == obj2);

    release_env(env);
})

TEST(test_overwrite_name, {
    lisp_env *env = create_env(NULL);
    lisp_obj *obj1 = lisp_int(42);
    lisp_obj *obj2 = lisp_int(22);

    release(set_env(env, "key", obj1));
    release(set_env(env, "key", obj2));

    ASSERT(lookup(env, "key") != obj1);
    ASSERT(lookup(env, "key") == obj2);

    release_env(env);
})

TEST(test_lookup_parent, {
    lisp_env *parent = create_env(NULL);
    release(set_env(parent, "key", lisp_int(42)));
    
    lisp_env *env = create_env(parent);

    ASSERT(lookup(env, "key") != NULL);
    release_env(env);

    ASSERT(lookup(parent, "key") != NULL);
    release_env(parent);
})

TEST(test_shadow_parent, {
    lisp_env *parent = create_env(NULL);
    lisp_obj *obj = lisp_int(42);
    release(set_env(parent, "key", obj));


    lisp_env *env = create_env(parent);
    lisp_obj *obj2 = lisp_float(3.14);
    release(set_env(env, "key", obj2));

    ASSERT(lookup(env, "key") == obj2);
    ASSERT(lookup(parent, "key") == obj);
    release_env(env);
    release_env(parent);
})

SUITE(
    ADDTEST(test_create_env),
    ADDTEST(test_env_content),
    ADDTEST(test_overwrite_name),
    ADDTEST(test_lookup_parent),
    ADDTEST(test_shadow_parent))

