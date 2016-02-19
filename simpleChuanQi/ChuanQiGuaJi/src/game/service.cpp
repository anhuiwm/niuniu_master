#include "protocal.h"
#include "db.h"
#include "factionCtrl.h"
#include "itemCtrl.h"
#include "rank.h"
#include "arena.h"
#include "roleCtrl.h"
#include "WorldBoss.h"
#include "logs_db.h"
#include "backend_session.h"
#include "../../base/base.h"
#include <algorithm>
#include "config.h"
#include "event2/thread.h"
#include <fstream>
#include <sstream>

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Rpcrt4")

#else
#include "j2.h"
#endif

#include "service.h"
#include "tinyxml/tinyxml.h"

#define PKT_MAX_SIZE  (60 * 1024 )
#include <iostream>
using namespace std;
#define THROW_EXCEPTION(x) {cout << x << endl;\
	exit(9); }


bool read_xml_node_value(TiXmlNode* node, string& value)
{
	if(node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if(temp == nullptr)
		return false;
	value = temp->Value();
	return true;
}

bool read_xml_node_value(TiXmlNode* node, int& value)
{
	if(node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if(temp == nullptr)
		return false;
	value = atoi(temp->Value());
	return true;
}

bool read_xml_node_value(TiXmlNode* node, float& value)
{
	if(node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if(temp == nullptr)
		return false;
	value = (float)atof(temp->Value());
	return true;
}

bool read_xml_node_value(TiXmlNode* node, unsigned& value)
{
	if(node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if(temp == nullptr)
		return false;
	value = (unsigned)atoi(temp->Value());
	return true;
}

bool read_xml_node_value(TiXmlNode* node, double& value)
{
	if(node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if(temp == nullptr)
		return false;
	value = atof(temp->Value());
	return true;
}

unsigned service::gm_no_need_name = 0;
unsigned service::account = 0;
unsigned service::id = 0;
string service::ip;
unsigned service::port = 0;
string service::backend_ip;
unsigned service::backend_port = 0;
set<unsigned> service::backend_limited_ips;
string service::http_address;
unsigned service::http_port = 0;
set<unsigned> service::http_limited_ips;
service::db_config service::main_db_config;
service::db_config service::logs_db_config;
event_base* service::event_handler = nullptr;//event_base_new();
evhttp* service::http = nullptr;
unsigned service::min_meet_pk_value = 0;
list<meet_pk_rank_item> service::meet_pk_rank;
//list<worldboss_staff> service::worldboss_rank;
map<uint16_t, list<unsigned>> service::map_to_players;


service::service(void)
{
#ifdef _WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
}


service::~service(void)
{
	//if(event_handler)
	//{
	//	event_base_free(event_handler);
	//	event_handler = nullptr;
	//}
}

bool safety_http_ip(unsigned ip)
{
	if(service::http_limited_ips.find(ip) == service::http_limited_ips.end())
		return false;
	return true;
}

void write_json_header(evhttp_request* request)
{
	auto output_headers = evhttp_request_get_output_headers(request);

	evhttp_add_header(output_headers, "Server", "guaji");
	evhttp_add_header(output_headers, "Content-Type", "application/json");
	evhttp_add_header(output_headers, "Cache-Control","no-cache");
	evhttp_add_header(output_headers, "Connection", "close");
}

void write_html_header(evhttp_request* request)
{
	auto output_headers = evhttp_request_get_output_headers(request);

	evhttp_add_header(output_headers, "Server", "guaji");
	evhttp_add_header(output_headers, "Content-Type", "text/html; charset=gb2312");
	evhttp_add_header(output_headers, "Cache-Control","no-cache");
	evhttp_add_header(output_headers, "Connection", "close");
}

void write_html_content(evhttp_request* request, const char* content)
{
	struct evbuffer *buf = evbuffer_new();
	evbuffer_add_printf(buf, content);
	evhttp_send_reply(request, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

void http_request_save(evhttp_request* request, void* v)
{
	auto conn = evhttp_request_get_connection(request);
	char* addr;
	unsigned short port;
	evhttp_connection_get_peer(conn, &addr, &port);
	if(!safety_http_ip(inet_addr(addr)))return;
	xLog("Info_", "save data from http (host:%s).", addr);
	write_json_header(request);
	saveAllToDb(true);
	write_html_content(request, "{\"result\":0}");
}

void http_request_stop(evhttp_request* request, void* v)
{
	auto conn = evhttp_request_get_connection(request);
	char* addr;
	unsigned short port;
	evhttp_connection_get_peer(conn, &addr, &port);
	if(!safety_http_ip(inet_addr(addr)))return;
	xLog("Info_", "stop server from http (host:%s).", addr);
	write_json_header(request);
	write_html_content(request, "{\"result\":0}");
	//exit(0);

	event_base_loopbreak(service::event_handler);
}

void http_request_get_online_count(evhttp_request* request, void* v)
{
	auto conn = evhttp_request_get_connection(request);
	char* addr;
	unsigned short port;
	evhttp_connection_get_peer(conn, &addr, &port);
	if(!safety_http_ip(inet_addr(addr)))return;
	xLog("Info_", "get_online_count from http (host:%s).", addr);

	auto output_headers = evhttp_request_get_output_headers(request);
	write_json_header(request);
	struct evbuffer *buf = evbuffer_new();
	evbuffer_add_printf(buf, "{\"count\":%u}", PROTOCAL::get_online_count());
	evhttp_send_reply(request, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

void http_request_root(evhttp_request* request, void* v)
{
	auto conn = evhttp_request_get_connection(request);
	char* addr;
	unsigned short port;
	evhttp_connection_get_peer(conn, &addr, &port);
	if(!safety_http_ip(inet_addr(addr)))return;

	string default_file = string( getProgDir() )+ STR_FSEP + "default.html";

	ifstream file(default_file, ios::in);
	if(file)
	{
		write_html_header(request);
		stringstream ss;
		ss << file.rdbuf();
		write_html_content(request, ss.str().c_str());
	}
}
event* e;
void event_test(intptr_t, short, void*)
{
	event_del(e);
}

int service::run(int argc, char **argv)
{
	init();
	srand( getpid() );
	srand( time(NULL) );
	PROTOCAL::refreshTime();
	load_config();
	BASE::writeLastPktFile(0xffffffff, "", 0);

	e = event_new(event_handler, -1, EV_SIGNAL, event_test, 0);

	thread t([](void* v){
		while(1)
		{
			Sleep(5000);
			event_active(e, 0, EV_SIGNAL);
		}
	}, nullptr);
	t.detach();

	//struct timeval five_seconds = {0,0};
	//event_add(e, &five_seconds);

	struct evconnlistener *listener = create_listener(ip, port);
	if ( listener == NULL )
		THROW_EXCEPTION("can't create listener at " + ip + ":" + to_string(port));
	
	struct evconnlistener *bk_listener = create_backend_listener(backend_ip, backend_port);
	if ( bk_listener == NULL )
		THROW_EXCEPTION("can't create listener at " + backend_ip + ":" + to_string(backend_port));

	// HTTP
	if(evhttp_bind_socket(http, http_address.c_str(), http_port))
		THROW_EXCEPTION("can't evhttp_bind_socket " + ip + ":" + to_string(port));

	evhttp_set_cb(http, "/", http_request_root, 0);
	evhttp_set_cb(http, "/save", http_request_save, 0);
	evhttp_set_cb(http, "/stop", http_request_stop, 0);
	evhttp_set_cb(http, "/online_count", http_request_get_online_count, 0);

	if ( -1 == PROTOCAL::netInit() )
		THROW_EXCEPTION("can't PROTOCAL::netInit at " + ip + ":" + to_string(port));
	
	evhttp_free(http);
	evconnlistener_free(listener);
	evconnlistener_free(bk_listener);
	return 0;
}


void service::init()
{
#ifdef WIN32
	evthread_use_windows_threads();
	struct event_config *cfg = event_config_new();
	if(cfg == nullptr)
		THROW_EXCEPTION("event_config_new failed.");

	event_config_set_flag( cfg, EVENT_BASE_FLAG_STARTUP_IOCP );

	event_handler = event_base_new_with_config(cfg);
	if(event_handler == nullptr)
		THROW_EXCEPTION("event_base_new_with_config failed.");
#else
	event_handler = event_base_new();
	if(event_handler == nullptr)
		THROW_EXCEPTION("event_base_new failed.");
#endif
	http = evhttp_new(event_handler);
	if(http == nullptr)
		THROW_EXCEPTION("evhttp_new failed.");
}


void service::load_config()
{
	load_server_config();
	DBCtrl::dbInit();
	load_meet_pk_rank();
	load_map_to_players_data();
	WorldBoss::getMe().load_from_db();
	CONFIG::initCfg();
	item::load_config();
	FactionCtrl::loadFaction();
	RANK::initDB2Ranks();
	ARENA::initArenaRoles();
	ItemCtrl::init();
	RoleMgr::getMe().init();
	WorldBoss::getMe().init();

	logFATAL("load_config() ok ");
}


void service::load_server_config()
{
	string config_file = string( getProgDir() )+ STR_FSEP + "config.xml";

	TiXmlDocument doc;
	if(!doc.LoadFile(config_file.c_str()))
		THROW_EXCEPTION("can't load config.xml");

	auto root = doc.RootElement();

	if(!read_xml_node_value(root->FirstChild("id"), id))
		THROW_EXCEPTION("can't read server id");

	if(!read_xml_node_value(root->FirstChild("ip"), ip))
		THROW_EXCEPTION("can't read server ip");

	if(!read_xml_node_value(root->FirstChild("port"), port))
		THROW_EXCEPTION("can't read server port");

	if(!read_xml_node_value(root->FirstChild("backend_ip"), backend_ip))
		THROW_EXCEPTION("can't read server backend_ip");

	if(!read_xml_node_value(root->FirstChild("backend_port"), backend_port))
		THROW_EXCEPTION("can't read server backend_port");

	string backend_limit_ip_list_str;
	if(!read_xml_node_value(root->FirstChild("backend_limited_ip"), backend_limit_ip_list_str))
		THROW_EXCEPTION("can't read server backend_limited_ip");
	vector<string> backend_limit_ip_list;
	BASE::strtoken2Vec(backend_limit_ip_list_str.c_str(), ";", backend_limit_ip_list);
	for(auto& ipstr : backend_limit_ip_list)
		backend_limited_ips.insert(inet_addr(ipstr.c_str()));

	if(!read_xml_node_value(root->FirstChild("http_address"), http_address))
		THROW_EXCEPTION("can't read server http_address");

	if(!read_xml_node_value(root->FirstChild("http_port"), http_port))
		THROW_EXCEPTION("can't read server http_port");

	string http_limit_ip_list_str;
	if(!read_xml_node_value(root->FirstChild("http_limited_ip"), http_limit_ip_list_str))
		THROW_EXCEPTION("can't read server http_limited_ip");
	vector<string> http_limited_ip_list;
	BASE::strtoken2Vec(http_limit_ip_list_str.c_str(), ";", http_limited_ip_list);
	for(auto& ipstr : http_limited_ip_list)
		http_limited_ips.insert(inet_addr(ipstr.c_str()));

	if(!read_xml_node_value(root->FirstChild("db_ip"), main_db_config.ip))
		THROW_EXCEPTION("can't read main db ip");
	if(!read_xml_node_value(root->FirstChild("db_schema"), main_db_config.schema))
		THROW_EXCEPTION("can't read main db schema");
	if(!read_xml_node_value(root->FirstChild("db_user"), main_db_config.user))
		THROW_EXCEPTION("can't read main db user");
	if(!read_xml_node_value(root->FirstChild("db_pwd"), main_db_config.pwd))
		THROW_EXCEPTION("can't read main db pwd");
	if(!read_xml_node_value(root->FirstChild("db_port"), main_db_config.port))
		THROW_EXCEPTION("can't read main db port");

	if(!read_xml_node_value(root->FirstChild("logdb_ip"), logs_db_config.ip))
		THROW_EXCEPTION("can't read log db ip");
	if(!read_xml_node_value(root->FirstChild("logdb_schema"), logs_db_config.schema))
		THROW_EXCEPTION("can't read log db schema");
	if(!read_xml_node_value(root->FirstChild("logdb_user"), logs_db_config.user))
		THROW_EXCEPTION("can't read log db user");
	if(!read_xml_node_value(root->FirstChild("logdb_pwd"), logs_db_config.pwd))
		THROW_EXCEPTION("can't read log db pwd");
	if(!read_xml_node_value(root->FirstChild("logdb_port"), logs_db_config.port))
		THROW_EXCEPTION("can't read log db port");

	if(!read_xml_node_value(root->FirstChild("check_account"), account))
		THROW_EXCEPTION("can't check_account sign");
	if(!read_xml_node_value(root->FirstChild("gm_no_need_name"), gm_no_need_name))
		THROW_EXCEPTION("can't check_account sign");

	logFATAL("config.xml ok ");
}


void service::load_meet_pk_rank()
{
	SQLSelect sqlx( DBCtrl::getSQL(), "select * from meet_pk_rank");

	while( MYSQL_ROW row = sqlx.getRow() )
	{
		meet_pk_rank_item item;
		item.id = atoi(row[0]);
		strcpy(item.name, row[1]);
		item.job = atoi(row[2]);
		item.sex = atoi(row[3]);
		item.level = atoi(row[4]);
		item.pkvalue = atoi(row[5]);
		meet_pk_rank.push_back(item);
	}
	if(meet_pk_rank.size() >= max_meet_pk_rank_count)
		min_meet_pk_value = meet_pk_rank.back().pkvalue;
}


//void service::load_worldboss_rank()
//{
//	SQLSelect sqlx( DBCtrl::getSQL(), "select * from worldboss_rank");
//
//	while( MYSQL_ROW row = sqlx.getRow() )
//	{
//		worldboss_staff item;
//		item.roleid = atoi(row[0]);
//		item.cooldown = atoi(row[1]);
//		item.hurt = atoi(row[2]);
//		item.enchance = atoi(row[3]);
//		worldboss_rank.push_back(item);
//	}
//}


void service::save_data()
{
	save_meet_pk_rank();
}


void service::save_meet_pk_rank()
{
	execSQL(DBCtrl::getSQL(), "truncate table meet_pk_rank");

	for(auto & item : meet_pk_rank)
	{
		execSQL(DBCtrl::getSQL(), "insert into meet_pk_rank values(%u, '%s', %u, %u, %u, %u)",
			item.id, item.name, item.job, item.sex, item.level, item.pkvalue);
	}
}


void service::load_map_to_players_data()
{
	for(int i=1;i<=33;i++)
	{
		SQLSelect sqlx( DBCtrl::getSQL(), "select id from roleinfo where mapid=%d limit 100",i);

		auto& ls = map_to_players[i];
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			ls.push_back(atoi(row[0]));
		}
	}
}

void service::client_connected(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int slen, void *void_cliend_cb)
{
	new session(fd, addr);
}

void service::backend_connected(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int slen, void *void_cliend_cb)
{
	auto temp = new backend_session(fd, addr);
	if(backend_limited_ips.find(temp->ip_ipv4) == backend_limited_ips.end())
		temp->disconnect();
}


evconnlistener * service::create_listener(const string& ip, unsigned port)
{
	struct sockaddr_in listen_addr = {0};
	int socklen = sizeof(listen_addr);
	
	//if (evutil_parse_sockaddr_port(ip_port, (struct sockaddr*)&listen_addr, &socklen)<0)
	//	return NULL;
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	listen_addr.sin_port = ntohs(port);

	return evconnlistener_new_bind(event_handler, client_connected, 0,
					LEV_OPT_CLOSE_ON_FREE|LEV_OPT_CLOSE_ON_EXEC|LEV_OPT_REUSEABLE,
					-1, (struct sockaddr*)&listen_addr, socklen);
}


evconnlistener * service::create_backend_listener(const string& ip, unsigned port)
{
	struct sockaddr_in listen_addr = {0};
	int socklen = sizeof(listen_addr);
	
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	listen_addr.sin_port = ntohs(port);

	return evconnlistener_new_bind(event_handler, backend_connected, 0,
					LEV_OPT_CLOSE_ON_FREE|LEV_OPT_CLOSE_ON_EXEC|LEV_OPT_REUSEABLE,
					-1, (struct sockaddr*)&listen_addr, socklen);
}


void service::player_enter_map(ROLE* role, uint16_t old_map_id)
{
	if(old_map_id == role->wLastFightMapID)
		return;

	auto old_iter = map_to_players.find(old_map_id);
	if(old_iter != map_to_players.end())
		map_to_players[old_map_id].remove(role->dwRoleID);

	auto& ls = map_to_players[role->wLastFightMapID];
	auto iter = find(ls.begin(), ls.end(), role->dwRoleID);
	if(iter == ls.end())
	{
		ls.push_back(role->dwRoleID);
		if(ls.size() > 100)
			ls.pop_front();
	}
}


void service::get_map_rand_players(output_stream& stream, ROLE* role)
{
	auto iter = map_to_players.find(role->wLastFightMapID);
	if(iter == map_to_players.end())
	{
		stream.write_byte(0);
		return;
	}

	auto& ls = map_to_players[role->wLastFightMapID];
	if(ls.size() <= 4)
	{
		auto pos = stream.length();
		stream.write_byte((byte)0);
		
		byte count = 0;
		for(auto& item : ls)
		{
			auto target = RoleMgr::getMe().getRoleByID(item);
			if(target->dwRoleID == role->dwRoleID)continue;
			if(target == nullptr)continue;

			int reward_pkvalue;

			if(target->wLevel >= role->wLevel)
				reward_pkvalue = 10 + (target->wLevel - role->wLevel) * 2;
			else
			{
				reward_pkvalue = 10 - (role->wLevel - target->wLevel);
				if(reward_pkvalue < 1)reward_pkvalue = 1;
			}

			auto record = role->get_meet_pk_result(item, false);

			stream.write_uint(item);
			stream.write_string(target->roleName);
			stream.write_byte(target->byJob);
			stream.write_byte(target->bySex);
			stream.write_byte((byte)target->wLevel);
			stream.write_uint(target->dwFightValue);
			stream.write_byte(reward_pkvalue);
			stream.write_byte(record?(byte)record->result:0);
			count++;
		}
		stream.write_data((const char*)&count, 1, pos);
	}
	else
	{
		auto pos = stream.length();
		stream.write_byte((byte)0);
		set<int> used;
		int count = 0;
		while(1)
		{
			int index = 0;
			do
			{
				index = rand() % ls.size();
				if(used.find(index) == used.end())break;
			}while(1);

			used.insert(index);

			unsigned roleid = 0;
			auto iter = ls.begin();
			for(int i=0;i<index;i++)iter++;
			

			auto target = RoleMgr::getMe().getRoleByID(*iter);
			if(target->dwRoleID == role->dwRoleID)continue;
			if(target == nullptr)continue;
			auto record = target->get_meet_pk_result(target->dwRoleID, false);

			int reward_pkvalue;

			if(target->wLevel >= role->wLevel)
				reward_pkvalue = 10 + (target->wLevel - role->wLevel) * 2;
			else
			{
				reward_pkvalue = 10 - (role->wLevel - target->wLevel);
				if(reward_pkvalue < 1)reward_pkvalue = 1;
			}

			stream.write_uint(*iter);
			stream.write_string(target->roleName);
			stream.write_byte(target->byJob);
			stream.write_byte(target->bySex);
			stream.write_byte((byte)target->wLevel);
			stream.write_uint(target->dwFightValue);
			stream.write_byte(reward_pkvalue);
			stream.write_byte(record?(byte)record->result:0);
			count++;

			if(count>=4)break;
		}
		stream.write_data((const char*)&count, 1, pos);
	}
}


void service::meet_pk_rank_update(ROLE* role, unsigned pkvalue)
{
	if(pkvalue < min_meet_pk_value)
		return;

	for(auto iter = meet_pk_rank.begin(); iter != meet_pk_rank.end();iter++)
	{
		if(iter->id == role->dwRoleID)
		{
			iter = meet_pk_rank.erase(iter);
			break;
		}
	}

	for(auto iter = meet_pk_rank.begin(); iter != meet_pk_rank.end();iter++)
	{
		if(pkvalue > iter->pkvalue)
		{
			meet_pk_rank_item item;
			item.pkvalue = pkvalue;
			item.id = role->dwRoleID;
			item.job = role->byJob;
			item.level = (byte)role->wLevel;
			strcpy(item.name, role->roleName.c_str());
			item.sex = role->bySex;
			meet_pk_rank.insert(iter, item);

			if(meet_pk_rank.size() > max_meet_pk_rank_count)
			{
				meet_pk_rank.pop_back();
				min_meet_pk_value = meet_pk_rank.back().pkvalue;
			}
			return;
		}
	}

	if(meet_pk_rank.size() < max_meet_pk_rank_count)
	{
		meet_pk_rank_item item;
		item.pkvalue = pkvalue;
		item.id = role->dwRoleID;
		item.job = role->byJob;
		item.level = (byte)role->wLevel;
		strcpy(item.name, role->roleName.c_str());
		item.sex = role->bySex;
		meet_pk_rank.push_back(item);
	}
}