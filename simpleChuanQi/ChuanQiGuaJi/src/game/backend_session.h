#pragma once
#include "config.h"
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "input_stream.h"


class backend_session
{
	friend struct backend_session_init;
	struct bufferevent *socket;
	typedef bool (backend_session::*msg_proc)(input_stream& stream);
	static vector<msg_proc> msg_procs;
public:
	string ip;
	unsigned ip_ipv4;
	unsigned port;
public:
	backend_session(evutil_socket_t fd, struct sockaddr *a);
	~backend_session(void);
	void disconnect();
	void send(const char* buffer, size_t buffer_len);

	static void s_handling_error(struct bufferevent *bev, short what, void *void_client);
	static void s_handling_write(struct bufferevent *bev, void *arg);
	static void s_handling_read(struct bufferevent *bev, void* void_client);
	void handling_read();
	bool handling_protocal(input_stream& stream);
	bool charging(input_stream& stream);
	bool query_user_info(input_stream& stream);
	bool kick_user(input_stream& stream);
	bool broadcast(input_stream& stream);
	bool mail(input_stream& stream);
	bool close_user(input_stream& stream);
	bool deny_chat(input_stream& stream);
};

