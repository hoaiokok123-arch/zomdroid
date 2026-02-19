#ifndef ZOMDROID_IOS_EPOLL_STUB_H
#define ZOMDROID_IOS_EPOLL_STUB_H

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union epoll_data {
    void* ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

struct epoll_event {
    uint32_t events;
    epoll_data_t data;
};

#ifndef EPOLLIN
#define EPOLLIN 0x001
#endif
#ifndef EPOLLPRI
#define EPOLLPRI 0x002
#endif
#ifndef EPOLLOUT
#define EPOLLOUT 0x004
#endif
#ifndef EPOLLERR
#define EPOLLERR 0x008
#endif
#ifndef EPOLLHUP
#define EPOLLHUP 0x010
#endif
#ifndef EPOLLRDNORM
#define EPOLLRDNORM 0x040
#endif
#ifndef EPOLLRDBAND
#define EPOLLRDBAND 0x080
#endif
#ifndef EPOLLWRNORM
#define EPOLLWRNORM 0x100
#endif
#ifndef EPOLLWRBAND
#define EPOLLWRBAND 0x200
#endif
#ifndef EPOLLMSG
#define EPOLLMSG 0x400
#endif
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif
#ifndef EPOLLONESHOT
#define EPOLLONESHOT (1u << 30)
#endif
#ifndef EPOLLET
#define EPOLLET (1u << 31)
#endif

#ifndef EPOLL_CTL_ADD
#define EPOLL_CTL_ADD 1
#endif
#ifndef EPOLL_CTL_DEL
#define EPOLL_CTL_DEL 2
#endif
#ifndef EPOLL_CTL_MOD
#define EPOLL_CTL_MOD 3
#endif

static inline int epoll_create(int size)
{
    (void)size;
    errno = ENOSYS;
    return -1;
}

static inline int epoll_create1(int flags)
{
    (void)flags;
    errno = ENOSYS;
    return -1;
}

static inline int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event)
{
    (void)epfd;
    (void)op;
    (void)fd;
    (void)event;
    errno = ENOSYS;
    return -1;
}

static inline int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout)
{
    (void)epfd;
    (void)events;
    (void)maxevents;
    (void)timeout;
    errno = ENOSYS;
    return -1;
}

static inline int epoll_pwait(int epfd, struct epoll_event* events, int maxevents, int timeout, const sigset_t* sigmask)
{
    (void)epfd;
    (void)events;
    (void)maxevents;
    (void)timeout;
    (void)sigmask;
    errno = ENOSYS;
    return -1;
}

static inline int epoll_pwait2(int epfd, struct epoll_event* events, int maxevents, const struct timespec* timeout, const sigset_t* sigmask)
{
    (void)epfd;
    (void)events;
    (void)maxevents;
    (void)timeout;
    (void)sigmask;
    errno = ENOSYS;
    return -1;
}

#ifdef __cplusplus
}
#endif

#endif
