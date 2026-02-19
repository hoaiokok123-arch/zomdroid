#include <dlfcn.h>

/*
 * iOS/macOS dyld interpose table.
 * For Apple targets we route dlopen/dlsym through Zomdroid linker hooks so
 * JNI native resolution can be redirected to box64-backed libraries.
 */

#if defined(__APPLE__)

void* zomdroid_linker_dlopen(const char* filename, int flags);
void* zomdroid_linker_dlsym(void* handle, const char* symbol);

static void* zomdroid_dlopen_interposed(const char* filename, int flags) {
    return zomdroid_linker_dlopen(filename, flags);
}

static void* zomdroid_dlsym_interposed(void* handle, const char* symbol) {
    return zomdroid_linker_dlsym(handle, symbol);
}

#define DYLD_INTERPOSE(_replacement, _replacee) \
    __attribute__((used)) static struct { \
        const void* replacement; \
        const void* replacee; \
    } _interpose_##_replacee \
    __attribute__((section("__DATA,__interpose"))) = { \
        (const void*)(unsigned long)&_replacement, \
        (const void*)(unsigned long)&_replacee \
    }

DYLD_INTERPOSE(zomdroid_dlopen_interposed, dlopen);
DYLD_INTERPOSE(zomdroid_dlsym_interposed, dlsym);

#endif

