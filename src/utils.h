#ifndef DEFINE_UTILS_HEADER
#define DEFINE_UTILS_HEADER

#include <stdbool.h>

extern const bool enable_debug;

char *duplicate_string(const char *string, int maxsize);

void DEBUG(const char *fmt, ...);

void ERROR(const char *fmt, ...);

#endif
