#include "env.h"
#include "utils.h"

#include <assert.h>
#include <string.h>

#define LISP_ENV_SIZE 10
struct lisp_env_t {
    lisp_env *next;
    lisp_env *parent;
    size_t used;
    char *names[LISP_ENV_SIZE];
    lisp_obj *values[LISP_ENV_SIZE];
};

lisp_env *create_env(lisp_env *parent)
{
    lisp_env *res = calloc(1, sizeof(lisp_env));
    assert(res != NULL);
    res->parent = parent;
    return res;
}

void set_env(lisp_env *env, const char *name, lisp_obj *value)
{
    assert(env != NULL);
    assert(name != NULL);
    assert(value != NULL);

    /* Hold ref for this obj if not owned by someone else */
    if (value->env == NULL){
        value->env = (void *) env;
    }

    /* If an object already has this name in this env, drop ref and detroy, 
       then replace by new object */
    for (size_t i=0; i<env->used; i++){
        if (strcmp(env->names[i], name) == 0){
            if (env->values[i]->env == env){
                env->values[i]->env = NULL;
                destroy_obj(env->values[i]);
            }
            env->values[i] = value;
            return;
        }
    }

    /* If not found, and there are subsequent env buckets set in following */
    if (env->next){
        set_env(env->next, name, value);
    } 

    /* Otherwise insert here */
    else {
        if (env->used == LISP_ENV_SIZE){
            env->next = create_env(env->parent);
            env = env->next;
        }

        env->names[env->used] = duplicate_string(name, LISP_MAX_NAME_SIZE);
        env->values[env->used] = value;
        env->used ++;
    }
}

lisp_env *destroy_env(lisp_env *env)
{
    assert(env != NULL);

    if (env->next){
        destroy_env(env->next);
    }

    lisp_env *parent = env->parent;
    for (size_t i=0; i<env->used; i++){
        free(env->names[i]);
        if (env->values[i]->env == env){
            env->values[i]->env = NULL;
            destroy_obj(env->values[i]);
        }
    }
    free(env);
    return parent;
}

lisp_obj *lookup(lisp_env *env, const char *name)
{
    for (size_t i=0; i<env->used; i++){
        if (strcmp(env->names[i], name) == 0){
            return env->values[i];
        }
    }

    lisp_obj *res = NULL;

    if (env->next){
        res = lookup(env->next, name);
    }
    if (! res && env->parent){
        res = lookup(env->parent, name);
    }
    return res;
}
