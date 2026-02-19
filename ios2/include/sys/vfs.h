#ifndef ZOMDROID_IOS_VFS_STUB_H
#define ZOMDROID_IOS_VFS_STUB_H

#include <sys/mount.h>

typedef struct statfs statfs64;

static inline int statfs64(const char* path, struct statfs* buf)
{
    return statfs(path, buf);
}

static inline int fstatfs64(int fd, struct statfs* buf)
{
    return fstatfs(fd, buf);
}

#endif
