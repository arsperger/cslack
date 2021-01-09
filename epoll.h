#ifndef EPOLL_H
#define EPOLL_H

#include <sys/types.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <unistd.h>

#include "cslack.h"
#include "log.h"

int epoll_setup(void);
void epoll_cleanup(int epoll_fd);

void epoll_add(int epoll_fd, int fd, uint32_t events);
void epoll_mod(int epoll_fd, int fd, uint32_t events);
void epoll_del(int epoll_fd, int fd);

#endif /* EPOLL_H */
