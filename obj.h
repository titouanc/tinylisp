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

struct lisp_lambda_t {
    size_t n_params;
    char **param_names;
    lisp_obj *(*proc)(lisp_env *);
};


#define IS_NUMBER(x)   ((x)&(FLOAT|INT))
#define IS_CALLABLE(x) ((x)&(PROC|LAMBDA))
typedef enum {
    FLOAT  = 0x01,
    INT    = 0x02,
    STRING = 0x04,
    LAMBDA = 0x08,
    PROC   = 0x10
} lisp_obj_type;

typedef union {
    double f;
    long int i;
    char *s;
    lisp_lambda l;
    lisp_proc p;
} lisp_obj_val;

struct lisp_obj_t {
    bool _static;
    lisp_obj_type type;
    lisp_obj_val value;
};

#define lisp_int(val) create_obj(INT, (lisp_obj_val)((long int)(val)))
#define lisp_float(val) create_obj(FLOAT, (lisp_obj_val)((double)(val)))
#define lisp_internal(val) create_obj(PROC, (lisp_obj_val)((lisp_proc)(val)))

lisp_obj *create_obj(lisp_obj_type type, lisp_obj_val value);

void destroy_obj(lisp_obj *obj);

void lisp_print(lisp_obj *obj);

#endif
