#include "log.h"

void die_errno(int code, char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(code));
    exit(EXIT_FAILURE);
}

void gai_error(int code, char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, gai_strerror(code));
    exit(EXIT_FAILURE);
}

void app_die(char *msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}

void wlog(int priority, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    if(priority & PRIO_LOG)
            vprintf(format, args);

    va_end(args);
}