#include "lighttest2.h"
#include "utils.h"
#include <stdlib.h>

TEST(test_duplicate_string, {
    char *s = duplicate_string("haha", -1);
    ASSERT(strcmp(s, "haha") == 0);
    free(s);

    s = duplicate_string("haha", 2);
    ASSERT(strcmp(s, "ha") == 0);
    free(s);
})

SUITE(
    ADDTEST(test_duplicate_string))
