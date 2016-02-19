#include "backend_session.h"
#include "service.h"
#include "backend_protocal.h"
#include "protocal.h"
#include "roleCtrl.h"
#include "output_stream.h"
#include "factionCtrl.h"
#include "broadCast.h"
#include "mailCtrl.h"


#define PKT_MAX_SIZE  (60 * 1024 )
#define HEAD_SIZE 4


vector<backend_session::msg_proc> backend_session::msg_procs;
struct backend_session_init
{
	backend_session_init()
	{
		backend_session::msg_procs.resize((size_t)backend_protocal::max_count);
		backend_session::msg_procs[(int)backend_protocal::charge] = &backend_session::charging;
		backend_session::msg_procs[(int)backend_protocal::query_roleinfo] = &backend_session::query_user_info;
		backend_session::msg_procs[(int)backend_protocal::kick_user] = &backend_session::kick_user;
		backend_session::msg_procs[(int)backend_protocal::broadcast] = &backend_session::broadcast;
		backend_session::msg_procs[(int)backend_protocal::mail] = &backend_session::mail;
		backend_session::msg_procs[(int)backend_protocal::close_user] = &backend_session::close_user;
		backend_session::msg_procs[(int)backend_protocal::deny_chat] = &backend_session::deny_chat;
	}
}backend_session_init_impl;



backend_session::backend_session(evutil_socket_t fd, struct sockaddr *addr)
{
	socket = bufferevent_socket_new(service::event_handler, fd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
	assert(socket);

	bufferevent_setcb(socket, s_handling_read, s_handling_write, s_handling_error, this);
	bufferevent_enable(socket, EV_READ|EV_WRITE);

	auto sa = (sockaddr_in*)addr;
	ip_ipv4 = sa->sin_addr.s_addr;
	ip = inet_ntoa(sa->sin_addr);
	port = sa->sin_port;
	cout << "backend be connected from " << ip << ":" << port << endl;
}

backend_session::~backend_session()
{
	disconnect();
}


void backend_session::disconnect()
{
	if (socket)
	{
		bufferevent_free(socket);
		socket = nullptr;
		cout << "disconnected from " << ip << ":" << port << endl;
		delete this;
	}
}

void backend_session::send(const char* buffer, size_t buffer_len)
{
	bufferevent_write(socket, buffer, buffer_len);
}


void backend_session::s_handling_read(struct bufferevent *bev, void* void_client)
{
	backend_session * client = (backend_session*)void_client;
	client->handling_read();
}

void backend_session::handling_read()
{
	struct evbuffer* input = bufferevent_get_input(socket);
	size_t total_len = evbuffer_get_length(input);

	while (1)
	{
		if (total_len < HEAD_SIZE)
		{
			return;
		}
		else
		{
			unsigned char *buffer = evbuffer_pullup(input, HEAD_SIZE);
			unsigned short pkt_len;
			
			if (NULL == buffer)
			{
				PUTS("evbuffer_pullup HEAD_SIZE failed!");
				disconnect();
				return;
			}
			
			pkt_len = *(unsigned short *)buffer;
			
			if (PKT_MAX_SIZE < pkt_len)
			{
				PUTS("len:%d > PKT_MAX_SIZE!", pkt_len);
				disconnect();
				return;
			}

			if (total_len < pkt_len)
				return;

			buffer = evbuffer_pullup(input, pkt_len);
			if (NULL == buffer)
			{
				PUTS("evbuffer_pullup msg_len failed!");
				disconnect();
				return;
			}
			input_stream stream(buffer+2, pkt_len-2);
			if(!handling_protocal(stream))
			{
				disconnect();
				return;
			}

			if (evbuffer_drain(input, pkt_len) < 0)
			{
				PUTS("evbuffer_drain failed!");
				disconnect();
				return;
			}

			total_len -= pkt_len;
		}
	}
}


bool backend_session::handling_protocal(input_stream& stream)
{
	u_short ptcl;
	if(!stream.read_ushort(ptcl) || ptcl >= msg_procs.size())
		return false;
	auto& fn = msg_procs[ptcl];
	if(fn == 0)
		return false;
	return (this->*fn)(stream);
}

bool backend_session::charging(input_stream& stream)
{
	unsigned roleid, ingot_count;
	bool res =false;
	string charge_account;

	output_stream out((u_short)backend_protocal::charge);

	if(!stream.read_uint(roleid) || !stream.read_uint(ingot_count) || !stream.read_string(charge_account))
	{
		out.write_bool(res);
	}
	else
	{
	    res = PROTOCAL::phpChargeRole(roleid, ingot_count, charge_account);
		out.write_bool(res);
	}
	//logwm("backend_session::charging(input_stream& stream) :res: %d %d %d",res, roleid, ingot_count);
	send(out.get_buffer(),  out.length());
	return true;
}

bool backend_session::query_user_info(input_stream& stream)
{
	string name;
	output_stream out((u_short)backend_protocal::query_roleinfo);
	if(!stream.read_string(name))
	{
		out.write_bool(false);
	}
	else
	{

		auto role = RoleMgr::getMe().getRoleByName(name);
		if(role!=nullptr)
		{
			out.write_bool(true);
			out.write_string(role->roleName);
			out.write_byte(role->byJob);
			out.write_ushort(role->wLevel);
			out.write_uint(role->dwExp);
			out.write_uint(CONFIG::getRoleUpgradeExp(role->wLevel));
			//out.write_string(role->faction==nullptr?"":role->faction->strName);
			out.write_ushort(role->wVipLevel);
			out.write_uint(role->dwCoin);
			out.write_uint(role->dwIngot);
			out.write_bool(PROTOCAL::getOnlineRole(role->dwRoleID)!=nullptr);
			out.write_uint(role->dwCharge);
		}
		else
		{
			out.write_bool(false);
		}
	}
	send(out.get_buffer(),  out.length());
	return true;
}

bool backend_session::kick_user(input_stream& stream)
{
	string name;
	output_stream out((u_short)backend_protocal::kick_user);
	if(!stream.read_string(name))
	{
		out.write_bool(false);
	}
	else
	{
		auto role = RoleMgr::getMe().getRoleByName(name);
		if(role==nullptr)
		{
			out.write_bool(false);
		}
		else if(role->client==nullptr)
		{
			out.write_bool(false);
		}
		else
		{
			PROTOCAL::onDisconnect(role->client);
			out.write_bool(true);
		}
	}
	send(out.get_buffer(),  out.length());
	return true;
}

bool backend_session::broadcast(input_stream& stream)
{
	string text;
	output_stream out((u_short)backend_protocal::broadcast);
	if(!stream.read_string(text))
	{
		out.write_bool(false);
	}
	else
	{
		BroadcastCtrl::systemBroadcast( 0, text );
		out.write_bool(true);
	}
	send(out.get_buffer(),  out.length());
	return true;
}

bool backend_session::mail(input_stream& stream)
{
	string roleids, type, content, items;
	output_stream out((u_short)backend_protocal::broadcast);
	if(!stream.read_string(roleids) || !stream.read_string(type) || !stream.read_string(content) || !stream.read_string(items))
	{
		out.write_bool(false);
	}
	else
	{
		vector<string> roleids_v;
		BASE::strtoken2Vec(roleids.c_str(), ",", roleids_v);

		vector<ITEM_INFO> vecItemInfo;
		CONFIG::parseItemInfoStr( items, vecItemInfo );

		for(auto& roleid : roleids_v)
		{
			MailCtrl::sendMail( atoi(roleid.c_str()), ATOB_(type), content, vecItemInfo );
		}
		out.write_bool(true);
	}
	send(out.get_buffer(),  out.length());
	return true;
}

bool backend_session::close_user(input_stream& stream)
{
	string name;
	unsigned valid_time;
	output_stream out((u_short)backend_protocal::kick_user);
	if(!stream.read_string(name) || !stream.read_uint(valid_time))
	{
		out.write_bool(false);
	}
	else
	{
		auto role = RoleMgr::getMe().getRoleByName(name);
		if(role==nullptr)
		{
			out.write_bool(false);
		}
		else
		{
			role->close_user_valid_time = valid_time;
			PROTOCAL::onDisconnect(role->client);
			out.write_bool(true);
		}
	}
	send(out.get_buffer(),  out.length());
	return true;
}

bool backend_session::deny_chat(input_stream& stream)
{
	string name;
	bool _switch;
	output_stream out((u_short)backend_protocal::kick_user);
	if(!stream.read_string(name) || !stream.read_bool(_switch))
	{
		out.write_bool(false);
	}
	else
	{
		auto role = RoleMgr::getMe().getRoleByName(name);
		if(role==nullptr)
		{
			out.write_bool(false);
		}
		else
		{
			role->deny_chat = _switch;
			out.write_bool(true);
		}
	}
	send(out.get_buffer(),  out.length());
	return true;
}


void backend_session::s_handling_error(struct bufferevent *bev, short what, void *void_client)
{
	if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR))
	{
		backend_session* client = (backend_session* )void_client;
		if (what & BEV_EVENT_ERROR)
		{
			PUTS("Client BEV_EVENT_ERROR.");
		}
		else
		{
			PUTS("Client BEV_EVENT_EOF.");

			/* Flush all pending data */
			s_handling_read(bev, client);
		}

		client->disconnect();
	}
}

void backend_session::s_handling_write(struct bufferevent *bev, void *arg)
{
	//struct evbuffer *output = bufferevent_get_output(bev);
	//PUTS("+++++++++++++:%u", evbuffer_get_length( bufferevent_get_output(bev) ));
}
