#include "utils.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

char *duplicate_string(const char *string, int maxsize)
{
    assert(string != NULL);

    int len = 0;
    while (string[len] && (maxsize < 0 || len < maxsize)){
        len++;
    }

    char *res = calloc(len+1, sizeof(char));
    memcpy(res, string, len);
    return res;
}
