#include "cslack.h"

static void setnonblocking(int sock) {
    int opts;
    if ((opts=fcntl(sock,F_GETFL))<0)
        die_errno(errno,"fcntl(sock,GETFL)");
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
        die_errno(errno,"fcntl(sock,SETFL,opts)");

    return;
}

static int create_noblock_sockfd (void){
    struct addrinfo hints, *result, *rp;
    int sockfd, rc;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_protocol = 0;

    if((rc = getaddrinfo(HOST, PORT, &hints, &result)) !=0) {
        gai_error(rc, "obtain_sockfd: getaddrinfo error");
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sockfd == -1) {
            continue;
        } else {
            setnonblocking(sockfd); 
            if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != 0) {
                if (errno != EINPROGRESS) {
                    goto fail;
                } else {
                    freeaddrinfo(result);
                    return sockfd;
                }
            }
        }

        close(sockfd);
    }

    freeaddrinfo(result);

    if (rp == NULL)
        goto fail;

    fail:
        freeaddrinfo(result);
        die_errno(errno,"Could not connect");
        return -1;
}

int slack_msg_request (Url *u, Msg *m, char *text) {
    char body[BODY_MAX_SIZE];
    char message[REQUEST_MAX_SIZE];
    int ssize = sprintf(body, BODY_FMT, m->channel, m->username, text, m->icon_emogi);
    int sockfd, epoll_fd;
    struct epoll_event events[MAX_EPOLL_EVENTS];

    /* compose message */
    sprintf(message, MSG_FMT, u->urla, u->urlb, u->urlc, HOST, ssize, body);
    wlog(HIGH,"Request: %s\n",message);

    sockfd = create_noblock_sockfd();
    epoll_fd = epoll_setup();

    u_int32_t event = EPOLLIN | EPOLLOUT | EPOLLET |EPOLLERR | EPOLLHUP;
    epoll_add(epoll_fd, sockfd, event);

     /* initialize OpenSSL */
    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    SSL *conn = SSL_new(ssl_ctx);
    if (conn == NULL) { die_errno(errno, "ERROR create SSL conn"); };
    SSL_set_fd(conn, sockfd);
    SSL_set_connect_state(conn);

    /* negotiate SSL connection */
    for(;;) {

        int ssl_con = SSL_connect(conn);

        if(ssl_con < 0) {
            int err = SSL_get_error(conn, ssl_con);
            if (err == SSL_ERROR_WANT_READ ||
                    err == SSL_ERROR_WANT_WRITE ||
                    err == SSL_ERROR_WANT_X509_LOOKUP) {
                continue;
            } else if(err == SSL_ERROR_ZERO_RETURN) {
                app_die("SSL_connect peer closed connect");
            } else {
                SSL_free(conn);
                close(sockfd);
                epoll_cleanup(epoll_fd);
                app_die("SSL_connect failed");
            }
        } else {
            wlog(NORMAL,"SSL has been negotiated on socket %d\n",sockfd);
            break;
        }
    }

    /* The event loop */
    while (1) {
        int n = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, -1);
        if (n < 0 && n == EINTR) {
            wlog(NORMAL,"epoll_wait System call interrupted. Continue.");
            continue;
        }

        for (int i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)
                    || (!(events[i].events & (EPOLLIN | EPOLLOUT)))) {
                /* An error has occurred on this socket we can close it and continue with the next
                but since we have only one sockfd added - terminate the process */
                SSL_free(conn);
                close(events[i].data.fd);
                epoll_cleanup(epoll_fd);
                app_die("epoll_wait error");

            } else if (events->events & (EPOLLIN | EPOLLHUP)) { // read
                int err;
                char response[RESPONSE_MAX_SIZE];
                memset(response,0,sizeof(response));
                err = SSL_read(conn, response, sizeof(response)-1);
                response[err] = '\0';
                wlog(HIGH,"Client Received %d chars - '%s'\n", err, response);

                if (err <= 0) {
                    if(err == SSL_ERROR_WANT_READ ||
                        err == SSL_ERROR_WANT_WRITE ||
                        err == SSL_ERROR_WANT_X509_LOOKUP) {
                        wlog(NORMAL,"Read could not complete. Try again");
                        break;
                    }
                    else if(err == SSL_ERROR_ZERO_RETURN) {
                        app_die("SSL_read: close notify received from peer");
                    }
                    else {
                        app_die("Error during SSL_read");
                    }
                }
                wlog(NORMAL, "SSL Read: %d bytes\n", err);
                exit(EXIT_SUCCESS);
            } else if (events->events & EPOLLOUT) { // write
                int err;
                err = SSL_write(conn, message, strlen(message));
                if (err <= 0) {
                    if(err == SSL_ERROR_WANT_READ ||
                        err == SSL_ERROR_WANT_WRITE ||
                        err == SSL_ERROR_WANT_X509_LOOKUP) {
                        wlog(NORMAL, "Write could not complete. Try again.");
                        break;
                    }
                    else if(err == SSL_ERROR_ZERO_RETURN) {
                        app_die("SSL_write: close notify received from peer");
                    }
                    else {
                        app_die("Error during SSL_write");
                    }
                }
                wlog(NORMAL, "SSL Write: %d bytes\n", err);
            }
        }
    }

    epoll_cleanup(epoll_fd);
    close(sockfd);

    return 0;
}
