#ifndef DEFINE_OBJ_HEADER
#define DEFINE_OBJ_HEADER

#include <stdlib.h>
#include <stdbool.h>

typedef struct lisp_obj_t lisp_obj;
typedef struct lisp_env_t lisp_env;
typedef struct lisp_lambda_t lisp_lambda;

#define NIL (lisp_obj*)(&(null_obj))
#define TRUE (lisp_obj*)(&(true_obj))
#define FALSE (lisp_obj*)(&(false_obj))

extern const lisp_obj null_obj;
extern const lisp_obj true_obj;
extern const lisp_obj false_obj;

typedef lisp_obj *(*lisp_proc)(size_t argc, lisp_obj **argv);

#include "expr.h"
#include "env.h"
struct lisp_lambda_t {
    lisp_expr *declaration;
    lisp_env      *context;
};

typedef enum {
    FLOAT  = 0x01,
    INT    = 0x02,
    STRING = 0x04,
    LAMBDA = 0x08,
    PROC   = 0x10
} lisp_obj_type;

/* MAX 64ko strings */
#define LISP_MAX_STRING_SIZE (1<<16)

typedef union {
    double      f;
    long int    i;
    const char *s;
    lisp_lambda l;
    lisp_proc   p;
} lisp_obj_val;

struct lisp_obj_t {
    int       refcount; /* Who references this object ?
                           Special values (constants) have refcount=-42 
                           Ohterwise refcount>=0 */
    lisp_obj_type type; /* What kind of object */
    lisp_obj_val value; /* Its actual value */
};

/* Quickly create any object */
#define lisp_int(val) create_obj(INT, (lisp_obj_val)((long int)(val)))
#define lisp_float(val) create_obj(FLOAT, (lisp_obj_val)((double)(val)))
#define lisp_internal(val) create_obj(PROC, (lisp_obj_val)((lisp_proc)(val)))

/* Quickly create a string */
lisp_obj *lisp_string(const char *val);

/* Allocate a new object with given type and value, and refcount=0 */
lisp_obj *create_obj(lisp_obj_type type, lisp_obj_val value);

/* Allocate a new empty object with given type, and refcount=0 */
lisp_obj *create_empty_obj(lisp_obj_type type);

/* Increment refcount (reference object) */
lisp_obj *retain(lisp_obj *obj);

/* Decrement refcount, potentially destroy (dereference object) */
lisp_obj *release(lisp_obj *obj);

/* Print lisp object to stdout */
void lisp_print(lisp_obj *obj);

#endif
