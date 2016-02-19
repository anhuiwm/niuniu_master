#include "event2/event-config.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/socket.h>
#include <signal.h>
#include <sys/resource.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _EVENT_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

#include <event.h>
#include <evutil.h>



void readcb(struct bufferevent *bev, void *ctx)
{
printf("%s %d\n", __FUNCTION__, __LINE__);
}


void writecb(struct bufferevent *bev, void *ctx)
{
printf("%s %d\n", __FUNCTION__, __LINE__);
}




void eventcb(struct bufferevent *bev, short events, void *ptr)
{
    if (events & BEV_EVENT_CONNECTED) {
         /* We're connected to 127.0.0.1:8080.   Ordinarily we'd do
            something here, like start reading or writing. */
    } else if (events & BEV_EVENT_ERROR) {
         /* An error occured while connecting. */
    }
}

int
main(int argc, char **argv)
{
	int i, c;
	struct timeval *tv;
	evutil_socket_t *cp;

#ifdef WIN32
	WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
#endif

    struct event_base *base;
    struct bufferevent *bev;
    struct sockaddr_in sin;

    base = event_base_new();

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */
    sin.sin_port = htons(9999); /* Port 8080 */

    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

//    bufferevent_setcb(bev, NULL, NULL, eventcb, NULL);
    bufferevent_setcb(bev, readcb, writecb, eventcb, NULL);

    if (bufferevent_socket_connect(bev,
        (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        /* Error starting connection */
        bufferevent_free(bev);
        return -1;
    }

    event_base_dispatch(base);
    return 0;

}
