#include "pch.h"
#include "session.h"
#include "protocal.h"
#include "service.h"



#define PKT_MAX_SIZE  (60 * 1024 )
#define HEAD_SIZE 4

session::session(evutil_socket_t fd, struct sockaddr *addr):role_id(0)
{
	buffer = bufferevent_socket_new(service::event_handler, fd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
	assert(buffer);
	connect_time = time(nullptr);

	auto sa = (sockaddr_in*)addr;
	ip = inet_ntoa(sa->sin_addr);
	port = sa->sin_port;

	PROTOCAL::onAccept(this, 1);
	
	bufferevent_setcb(buffer, read_client_cb, write_client_cb, error_client_cb, this);
	bufferevent_enable(buffer, EV_READ | EV_WRITE);

	//auto today = (time(nullptr) + 8) % (24 * 3600);
	//auto hort = today / 3600;
	//auto minute = 
	auto host = gethostbyaddr((char*)&sa->sin_addr, 4, PF_INET);
	if (host)
		cout << host->h_name << " connected from " << ip << ":" << port << endl;
	else
		cout << "connected from " << ip << ":" << port << endl;
}


session::~session(void)
{
	cout << "disconnected from " << ip << ":" << port << endl;
}


bool session::send(const void* data, size_t len)
{
	if (buffer == nullptr)
		return false;
	return bufferevent_write(buffer, data, len) == 0;
}


void session::error_client_cb(struct bufferevent *bev, short what, void *void_client)
{
	if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR))
	{
		session* client = (session* )void_client;
		if (what & BEV_EVENT_ERROR)
		{
			logINFO("Client BEV_EVENT_ERROR.");
		}
		else
		{
			logINFO("Client BEV_EVENT_EOF.");
			read_client_cb(bev, client);
		}

		service::close_session(client);
	}
}

void session::write_client_cb(struct bufferevent *bev, void *arg)
{
	session* _this = (session*)arg;
	struct evbuffer *output = bufferevent_get_output(bev);
	if (_this->closed && evbuffer_get_length(output) == 0 && _this->buffer)
	{
		bufferevent_free(_this->buffer);
		_this->buffer = nullptr;
		delete _this;
	}
}

void session::read_client_cb(struct bufferevent *bev, void* void_client)
{
	session * client = (session*)void_client;

	struct evbuffer* input = bufferevent_get_input(bev);
	size_t total_len = evbuffer_get_length(input);

	while (1)
	{
		if (total_len < HEAD_SIZE)
		{
			goto conti;
		}
		else
		{
			unsigned char *buffer = evbuffer_pullup(input, HEAD_SIZE);
			unsigned short pkt_len;
			
			if (NULL == buffer)
			{
				PUTS("evbuffer_pullup HEAD_SIZE failed!");
				goto err;
			}
			
			pkt_len = *(unsigned short *)buffer;
			

			if (PKT_MAX_SIZE < pkt_len)
			{
				PUTS("len:%d > PKT_MAX_SIZE!", pkt_len);
				goto err;
			}

			if (total_len < pkt_len)
				goto conti;

			buffer = evbuffer_pullup(input, pkt_len);
			if (NULL == buffer)
			{
				PUTS("evbuffer_pullup msg_len failed!");
				goto err;
			}

			/* TODO frequency limit */

			/* callback */
			//if (client->cb->execPacket)
			{
				string src((char*)buffer, pkt_len);
				int ret = PROTOCAL::execPacket(client, buffer, pkt_len);
				for (uint i = 0; i < pkt_len;i++)
					if (buffer[i] != (byte)src[i])
					{
						cout << "not eq" << endl;
					}
				if ( ret == -1 )
				{
					goto err;
				}
			}
			

			if (evbuffer_drain(input, pkt_len) < 0)
			{
				PUTS("evbuffer_drain failed!");
				goto err;
			}

			total_len -= pkt_len;
		}
	}

	return;

err:
	PUTS("close socket!");
	service::close_session(client);
	return;

conti:
	//bufferevent_enable(bev, EV_READ);
	
	return;

}