#pragma once
#include "config.h"
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

class session
{
public:
	struct bufferevent *client_bev;
//	ROLE* role;
	DWORD role_id;

	string ip;
	unsigned port;

	struct event timer;
public:
	session(evutil_socket_t fd, struct sockaddr *addr);
	~session(void);

private:
	static void error_client_cb(struct bufferevent *bev, short what, void *void_client);
	static void write_client_cb(struct bufferevent *bev, void *arg);
	static void read_client_cb(struct bufferevent *bev, void *void_client);
};

