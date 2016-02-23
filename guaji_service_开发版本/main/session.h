#pragma once
#include "config.h"
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

class session
{
public:
	bool closed = false;
	struct bufferevent *buffer = nullptr;
//	ROLE* role;
	DWORD role_id;

	string ip;
	unsigned port;
	uint connect_time;

	struct event timer;
public:
	session(evutil_socket_t fd, struct sockaddr *addr);
	~session(void);
	bool send(const void* data, size_t len);

private:
	static void error_client_cb(struct bufferevent *bev, short what, void *void_client);
	static void write_client_cb(struct bufferevent *bev, void *arg);
	static void read_client_cb(struct bufferevent *bev, void *void_client);
};

