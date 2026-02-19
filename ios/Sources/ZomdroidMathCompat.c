#include "zomdroid_math_compat.h"

#if defined(__APPLE__)
#include <math.h>

int zomdroid_isnanf(float x) {
    return isnan((double)x);
}

int zomdroid_isinff(float x) {
    return isinf((double)x);
}

void zomdroid_sincos(double x, double* s, double* c) {
    if (s) {
        *s = sin(x);
    }
    if (c) {
        *c = cos(x);
    }
}

void zomdroid_sincosf(float x, float* s, float* c) {
    if (s) {
        *s = sinf(x);
    }
    if (c) {
        *c = cosf(x);
    }
}
#endif
