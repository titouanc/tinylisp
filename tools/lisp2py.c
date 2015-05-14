#include "lisp.h"
#include <stdio.h>

void pydump(lisp_expr *expr);

void pydump_obj(lisp_obj *obj)
{
    if (obj == NIL){printf("None");}
    else if (obj == TRUE){printf("True");}
    else if (obj == FALSE){printf("False");}
    else if (obj->type == INT){printf("%ld", obj->value.i);}
    else if (obj->type == FLOAT){printf("%lf", obj->value.f);}
}

void pydump_def(const char *name, lisp_expr_lambda *lambda)
{
    printf("def %s(", name);
    for (size_t i=0; i<lambda->nparams; i++){
        printf("%s%s", (i>0) ? ", " : "", lambda->param_names[i]);
    }
    printf("):\n    return ");
    pydump(lambda->body);
}

void pydump(lisp_expr *expr)
{
    if (expr->type == SELFEVAL){
        pydump_obj(expr->value.selfeval.value);
    }

    else if (expr->type == LOOKUP){
        printf("%s", expr->value.lookup.name);
    }

    else if (expr->type == APPLICATION){
        pydump(expr->value.application.proc);
        printf("(");
        for (size_t i=0; i<expr->value.application.nparams; i++){
            if (i>0) printf(", ");
            pydump(expr->value.application.params[i]);
        }
        printf(")");
    }

    else if (expr->type == DEFINE){
        const char *name = expr->value.define.name;

        if (expr->value.define.expr->type == MKLAMBDA){
            lisp_expr_lambda *lambda = &(expr->value.define.expr->value.mklambda);
            pydump_def(name, lambda);
        } 

        else if (expr->value.define.expr->type == APPLICATION &&
                 expr->value.define.expr->value.application.nparams == 1 &&
                 expr->value.define.expr->value.application.params[0]->type == MKLAMBDA){
            lisp_expr_application *app = &(expr->value.define.expr->value.application);
            lisp_expr_lambda *lambda = &(app->params[0]->value.mklambda);
            printf("@");
            pydump(app->proc);
            printf("\n");
            pydump_def(name, lambda);
        }

        else {
            printf("%s = ", name);
            pydump(expr->value.define.expr);
        }
    }

    else if (expr->type == MKLAMBDA){
        printf("lambda");
        for (size_t i=0; i<expr->value.mklambda.nparams; i++){
            printf("%s%s", (i==0) ? " " : ", ", expr->value.mklambda.param_names[i]);
        }
        printf(": ");
        pydump(expr->value.mklambda.body);
    }
}

int main(int argc, const char **argv)
{
    for (int i=1; i<argc; i++){
        lisp_err err = {.type=OK};

        const char *endptr;
        lisp_expr *expr = analyze(argv[i], &endptr, &err);

        if (err.type != OK){
            printf("Error: %s\n", err.description);
        }
        else {
            pydump(expr);
            printf("\n\n");
        }

        if (expr){
            release_expr(expr);
        }
    }
}
