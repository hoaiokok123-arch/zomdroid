#ifndef ZOMDROID_MATH_COMPAT_H
#define ZOMDROID_MATH_COMPAT_H

#if defined(__APPLE__)
#include <math.h>
#include <stddef.h>
#if __has_include(<alloca.h>)
#include <alloca.h>
#else
void* alloca(size_t size);
#endif

#ifdef __cplusplus
extern "C" {
#endif
int zomdroid_isnanf(float x);
int zomdroid_isinff(float x);
void zomdroid_sincos(double x, double* s, double* c);
void zomdroid_sincosf(float x, float* s, float* c);
#ifdef __cplusplus
}
#endif

#define isnanf zomdroid_isnanf
#define isinff zomdroid_isinff
#define sincos zomdroid_sincos
#define sincosf zomdroid_sincosf
#endif

#endif
