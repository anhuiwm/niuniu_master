
//gcc -DHAVE_CONFIG_H -I. -I..  -I.. -I../compat -I../include -I../include   -g -O2 -Wall -fno-strict-aliasing  -c easy.c
//gcc -g -O2 -Wall -fno-strict-aliasing -o easy easy.o  ../.libs/libevent.a  -lcrypto -lrt
#include "net_util.h"
#include <time.h>

static void go_connecting(int fd, short what, void *arg)
{
	CONNECTOR *cr = (CONNECTOR *)arg;
	bufferevent_socket_connect(cr->bev, cr->sa, cr->socklen);
	PUTS("bufferevent_socket_connect %s", __FUNCTION__);

}

void conn_write_cb(struct bufferevent *bev, void *arg)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	size_t sz = evbuffer_get_length(output);
	if (sz > 0)
		bufferevent_enable(bev, EV_WRITE);

	PUTS("+++++++++++++");

}


void reconnect(CONNECTOR *cr, int delay_connect);
static void conn_event_cb2(struct bufferevent *bev, short what, void *arg)
{
	CONNECTOR *cr = (CONNECTOR *)arg;

	if (what & BEV_EVENT_EOF || what & BEV_EVENT_ERROR)
	{

		CONNECTOR_CALLBACK *cb = cr->cb;
		if (cb->onDisconnect)
		{
			(*(cb->onDisconnect))(cr);
		}

		if ( cr->keep_connect )
			reconnect(cr, 0);

	}
}
static void connecting_event_cb(struct bufferevent *bev, short what, void *arg)
{
	CONNECTOR *cr = (CONNECTOR *)arg;

	if (!(what & BEV_EVENT_CONNECTED))
	{
		PUTS("connecting failed!, %d", time(NULL) );
		if (0 == cr->keep_connect)
		{
			CONNECTOR_CALLBACK *cb = cr->cb;
			if ( cb->onConnect)
				(*(cb->onConnect))(cr, 0);
		}
		else
		{
			//delay_connecting(c);
			reconnect(cr, 5);
		}
	}
	else
	{

		CONNECTOR_CALLBACK *cb = cr->cb;
		int fd = bufferevent_getfd(bev);
		struct linger l;

		PUTS("connect %s success!%d", cr->addrtext, time(NULL));
		cr->state = STATE_CONNECTED;

		if (cb->onConnect)
			(*(cb->onConnect))(cr, 1);

		/* prevent CLOSE_WAIT */
		l.l_onoff = 1;
		l.l_linger = 0;
		setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&l, sizeof(l));
		bufferevent_setcb(bev, read_client_cb, conn_write_cb, conn_event_cb2, cr);
		bufferevent_enable(bev, EV_READ);

	}
}


void reconnect(CONNECTOR *cr, int delay_connect)
{
	struct timeval tv = { delay_connect, 0};

	if ( cr->bev )
		bufferevent_free(cr->bev);

	cr->bev = bufferevent_socket_new(cr->cb->ebase, -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS );
	
	assert(cr->bev);

	bufferevent_setcb(cr->bev, NULL, NULL, connecting_event_cb, cr);
	cr->state = STATE_NOT_CONNECTED;

	evtimer_set(&cr->timer, go_connecting, cr);
	event_base_set(cr->cb->ebase, &cr->timer);
	evtimer_add(&cr->timer, &tv);

}


int conn_write(CONNECTOR *cr, unsigned char *msg, size_t sz)
{
	if (STATE_CONNECTED == cr->state)
	{
		if (0 == bufferevent_write(cr->bev, msg, sz))
		{
			return 0;
		}
	}
	return -1;
}


CONNECTOR* connect_server(struct event_base* base, CONNECTOR_CALLBACK* cb , const char* ip, int port, int keep_connect)
{
	CONNECTOR *cr = (CONNECTOR *)calloc(1, sizeof(CONNECTOR));
	assert(cr);

	cr->sa = (struct sockaddr *)calloc(1, sizeof(struct sockaddr_in));
	assert(cr->sa);

	((struct sockaddr_in *)cr->sa)->sin_family = AF_INET;	
	((struct sockaddr_in *)cr->sa)->sin_port = htons(port);	
	((struct sockaddr_in *)cr->sa)->sin_addr.s_addr = inet_addr(ip);

//	cr->sa.sin_port = htons(port);
//	cr->sa.sin_addr.s_addr = inet_addr(ip);

	cr->keep_connect = keep_connect;

	cr->cb = cb;

	/*
	cr->cb.type = 'c';
	cr->cb.ebase = base;
	cr->cb.proto = NULL;
	cr->cb.connect = connected_cb;
	cr->cb.disconnect = NULL;
*/

	cr->socklen = sizeof(struct sockaddr_in);

	sprintf(cr->addrtext, "%s:%d", inet_ntoa(((struct sockaddr_in *)(cr->sa))->sin_addr),
						ntohs(((struct sockaddr_in *)(cr->sa))->sin_port));

	cr->bev = NULL;
	cr->state = STATE_NOT_CONNECTED;

	reconnect(cr, 0);

	return cr;

}


