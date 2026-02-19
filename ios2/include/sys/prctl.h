#ifndef ZOMDROID_IOS_PRCTL_STUB_H
#define ZOMDROID_IOS_PRCTL_STUB_H

#include <errno.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PR_SET_NAME
#define PR_SET_NAME 15
#endif
#ifndef PR_SET_SECCOMP
#define PR_SET_SECCOMP 22
#endif

static inline int prctl(int option, ...)
{
    (void)option;
    errno = ENOSYS;
    return -1;
}

#ifdef __cplusplus
}
#endif

#endif
