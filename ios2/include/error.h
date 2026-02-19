#ifndef ZOMDROID_IOS_ERROR_STUB_H
#define ZOMDROID_IOS_ERROR_STUB_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void error(int status, int errnum, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    if (errnum) {
        fprintf(stderr, ": %s", strerror(errnum));
    }
    fputc('\n', stderr);
    va_end(ap);
    if (status) {
        exit(status);
    }
}

static inline void error_at_line(int status, int errnum, const char* filename, unsigned int linenum, const char* format, ...)
{
    va_list ap;
    if (filename) {
        fprintf(stderr, "%s:%u: ", filename, linenum);
    }
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    if (errnum) {
        fprintf(stderr, ": %s", strerror(errnum));
    }
    fputc('\n', stderr);
    va_end(ap);
    if (status) {
        exit(status);
    }
}

#ifdef __cplusplus
}
#endif

#endif
