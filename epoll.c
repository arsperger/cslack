#include "epoll.h"

int epoll_setup(void) {
        int epoll_fd = epoll_create1(0);
        if (epoll_fd == -1)
            die_errno(errno,"epoll_create1 failed");
        wlog(NORMAL, "created epoll fd %d\n", epoll_fd);
        return epoll_fd;
}

void epoll_add(int epoll_fd, int fd, uint32_t events) {
        struct epoll_event epev = { .events = events, .data = { .fd = fd } };

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &epev) <0)
            die_errno(errno,"epoll_add failed");
}

void epoll_mod(int epoll_fd, int fd, uint32_t events) {
        struct epoll_event epev = { .events = events, .data = { .fd = fd } };
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &epev) <0)
            die_errno(errno,"epoll_add failed");
}

void epoll_del(int epoll_fd, int fd) {
       if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) <0)
            die_errno(errno,"epoll_del failed");
}

void epoll_cleanup(int epoll_fd) {
        close(epoll_fd);
}