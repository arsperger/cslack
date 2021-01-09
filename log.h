#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void die_errno(int code, char *msg);
void gai_error(int code, char *msg);
void app_die(char *msg);

#define EXITNULL(x) if ((x)==NULL) die_errno(errno,"exit NULL")
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors(stderr); exit(EXIT_FAILURE); }

//extern u_int8_t PRIO_LOG;

#define HIGH 0x0001
#define NORMAL 0x0002
#define LOW 0x0004

static const u_int8_t PRIO_LOG = NORMAL;

void wlog(int priority, const char *format, ...);

#endif /* LOG_H */