#ifndef CSLACK_H
#define CSLACK_H

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <stdbool.h>

#include "epoll.h"
#include "log.h"

typedef struct msg 
{
    char *channel;
    char *username;
    char *icon_emogi;
    char *text;
} Msg;

typedef struct url
{
    char *urla;
    char *urlb;
    char *urlc;
} Url;


#define setUrl(url,a,b,c) url = &(Url) {a, b, c} 
#define setMsg(msg,ch,usr,icon) msg = &(Msg) {ch, usr, icon}

#define MSG_FMT "POST /services/%s/%s/%s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nContent-Length: %d\r\nContent-type: x-www-form-urlencoded\r\n\r\n%s"
#define BODY_FMT "{\"channel\": \"%s\", \"username\": \"%s\", \"text\": \"%s\", \"icon_emoji\": \"%s\" }"

#define PORT "443"
#define HOST "hooks.slack.com"
#define BODY_MAX_SIZE 256
#define REQUEST_MAX_SIZE 1024
#define RESPONSE_MAX_SIZE 1024
#define MAX_EPOLL_EVENTS 10

int slack_msg_request (Url *u, Msg *m, char *message);

#endif /* CSLACK_H */
