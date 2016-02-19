#ifndef __NET_UTIL_H
#define __NET_UTIL_H

#ifdef _WIN32
#pragma warning(disable : 4200)
#pragma warning(disable : 4996)
#endif


#include "base.h"
#include "log.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include <log-internal.h>

#ifdef _WIN32
#include <winsock2.h>
//#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#endif

#ifdef _WIN32
#include <event2/event-config.h>
#endif

#include <event.h>
#include <evutil.h>
#include <util-internal.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include "../game/session.h"


#define HEAD_SIZE 4

struct CLIENT;

typedef int (*FUNC_EXEC_PKT)(session *client, unsigned char *, size_t);
typedef int (*FUNC_ACCEPT)(session *client, int);
typedef int (*FUNC_DISCONNECT)(session *client);



struct CONNECTOR;

typedef void (*FUNC_CONNECTOR_EXEC_PKT)(struct CONNECTOR *client, unsigned char *, size_t);
typedef void (*FUNC_CONNECTOR_CONN)(struct CONNECTOR *client, int);
typedef void (*FUNC_CONNECTOR_DISCONN)(struct CONNECTOR *client);




enum
{
	STATE_NOT_CONNECTED = 0,
	STATE_CONNECTED = 1,
};



struct CLIENT_CALLBACK
{

	struct event_base *ebase;

	FUNC_EXEC_PKT execPacket;
	FUNC_ACCEPT onAccept;
	FUNC_DISCONNECT onDisconnect;

};


struct CONNECTOR_CALLBACK
{

	struct event_base *ebase;

	FUNC_CONNECTOR_EXEC_PKT conExecPkt;
	FUNC_CONNECTOR_CONN onConnect;
	FUNC_CONNECTOR_DISCONN onDisconnect;

};

struct CONNECTOR
{

	CONNECTOR_CALLBACK* cb;

	int keep_connect;

	int state;
	struct sockaddr *sa;

	int socklen;
	char addrtext[32];

	struct event timer;

	struct bufferevent *bev;

};

//class ROLE;

//
//struct CLIENT
//{
//	CLIENT_CALLBACK* cb;
//	struct bufferevent *client_bev;
////	ROLE* role;
//	DWORD role_id;
//
//	char ip[IP4_ADDRESS_STRING_LENGTH];
//
//	struct event timer;
//
//	CLIENT()
//	{
////		cb = NULL;
////		client_bev = NULL;
////		//role = NULL;
////		role_id = 0;
//
//		memset(this, 0, sizeof(*this) );
//	}
//
//};



#pragma pack(push, 1) 

struct PKT_DATA
{
	unsigned short len;
	unsigned short cmd;
	unsigned char data[0];
};

#pragma pack(pop)

unsigned int peerSock2BigendianIP(evutil_socket_t sock);

struct evconnlistener * listener_init(CLIENT_CALLBACK* client_cb, const char* ip, unsigned port);

CONNECTOR* connect_server(struct event_base* base, CLIENT_CALLBACK* cb, const char* ip, int port, int keep_connect);


#endif		//__NET_UTIL_H
