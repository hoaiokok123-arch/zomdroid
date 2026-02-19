#ifndef ZOMDROID_IOS_VFS_STUB_H
#define ZOMDROID_IOS_VFS_STUB_H

#include <sys/mount.h>

#ifndef statfs64
#define statfs64 statfs
#endif
#ifndef fstatfs64
#define fstatfs64 fstatfs
#endif

#endif
