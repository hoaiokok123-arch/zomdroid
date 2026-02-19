#ifndef ZOMDROID_IOS_PTRACE_STUB_H
#define ZOMDROID_IOS_PTRACE_STUB_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PTRACE_PEEKTEXT
#define PTRACE_PEEKTEXT 1
#endif
#ifndef PTRACE_PEEKDATA
#define PTRACE_PEEKDATA 2
#endif
#ifndef PTRACE_PEEKUSER
#define PTRACE_PEEKUSER 3
#endif
#ifndef PTRACE_POKETEXT
#define PTRACE_POKETEXT 4
#endif
#ifndef PTRACE_POKEDATA
#define PTRACE_POKEDATA 5
#endif
#ifndef PTRACE_POKEUSER
#define PTRACE_POKEUSER 6
#endif

long ptrace(int request, pid_t pid, void* addr, void* data);

#ifdef __cplusplus
}
#endif

#endif
