#include "pch.h"
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
#include "config.h"
#include "activity.h"
#include "broadCast.h"
#include "service.h"


#ifdef _WIN32

#else
#include "j2.h"
#endif


#define PKT_MAX_SIZE  (60 * 1024 )

#define THROW_EXCEPTION(x) {cout << x << endl;\
	exit(9); }


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
unsigned service::min_meet_combo_value = 0;
uint service::open_time = 0;
long service::tzone = 0;
list<rank_item> service::meet_pk_rank;
list<rank_item> service::meet_combo_rank;
//list<worldboss_staff> service::worldboss_rank;
map<uint16_t, list<unsigned>> service::map_to_players;
map<uint16_t, pair<unsigned, string>> service::elitemap_first_killer;
bool service::elitemap_first_killer_changed = false;
map<string, string> service::global_config;
map<uint, ROLE_CLIENT> service::online_players;
deque<string> service::chats;


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

void write_html_content(evhttp_request* request, const char* content)
{
	struct evbuffer *buf = evbuffer_new();
	evbuffer_add_printf(buf, content);
	evhttp_send_reply(request, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
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

bool verify_client_ip(evhttp_request* request)
{
	auto conn = evhttp_request_get_connection(request);
	char* addr;
	unsigned short port;
	evhttp_connection_get_peer(conn, &addr, &port);

	auto uri = evhttp_request_get_uri(request);
	cout << uri << " from:" << addr << endl;
	logINFO("access uri %s from client(%s:%u)", uri, addr, port);

	if (service::http_limited_ips.find(inet_addr(addr)) == service::http_limited_ips.end())
	{
		write_html_header(request);
		write_html_content(request, "<html><head><title>禁止访问</title></head><body>禁止访问</body></html>");
		return false;
	}
	return true;
}

void http_request_any(evhttp_request* request, void* v)
{
	if (!verify_client_ip(request))return;
}

void http_request_save(evhttp_request* request, void* v)
{
	if (!verify_client_ip(request))return;
	xLog("Info_", "save data.");
	write_json_header(request);
	PROTOCAL::saveAllToDb(true);
	write_html_content(request, "{\"result\":0}");
}

void http_request_stop(evhttp_request* request, void* v)
{
	if (!verify_client_ip(request))return;
	xLog("Info_", "stop server.");
	write_json_header(request);
	write_html_content(request, "{\"result\":0}");
	//exit(0);

	PROTOCAL::saveAllToDb(true);
	event_base_loopbreak(service::event_handler);
}

void http_request_get_online_count(evhttp_request* request, void* v)
{
	if (!verify_client_ip(request))return;
	xLog("Info_", "get_online_count.");

	write_json_header(request);
	struct evbuffer *buf = evbuffer_new();
	evbuffer_add_printf(buf, "{\"count\":%u}", service::get_online_count());
	evhttp_send_reply(request, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

void http_request_get_cache_count(evhttp_request* request, void* v)
{
	if (!verify_client_ip(request))return;
	xLog("Info_", "get_cache_count.");

	write_json_header(request);
	struct evbuffer *buf = evbuffer_new();
	evbuffer_add_printf(buf, "{\"count\":%u}", RoleMgr::getMe().get_cached_role_count());
	evhttp_send_reply(request, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

void http_request_online_players(evhttp_request* request, void* v)
{
	if (!verify_client_ip(request))return;
	xLog("Info_", "http_request_online_players.");

	write_json_header(request);
	auto uri = evhttp_request_get_uri(request);
	evkeyvalq kvp;
	evhttp_parse_query(uri, &kvp);
	auto str_size = evhttp_find_header(&kvp, "size");
	auto str_page = evhttp_find_header(&kvp, "page");
	if (str_size == nullptr || str_page == nullptr)
	{
		write_html_content(request, "{\"result\":1}");
		return;
	}

	auto size = atoi(str_size);
	auto page = atoi(str_page);
	if (size < 10)size = 10;
	if (size > 200)size = 200;
	if (page < 1)page = 1;
	if (page > 1000)page = 1000;

	struct evbuffer *buf = evbuffer_new();
	evbuffer_add_printf(buf, "{\"result\":0,\"online_count\":%u,\"size\":%u,\"page\":%u,\"players\":[", service::get_online_count(), size, page);

	page--;
	int cur = 0, count = 0;
	for (auto& item : service::online_players)
	{
		if (cur < page * size)
		{
			cur++;
			continue;
		}
		if (count > size)continue;
		auto host = gethostbyname(item.second.client->ip.c_str());
		auto host_ip = item.second.client->ip;
		if (host)
			host_ip = host->h_name + string("(") + host_ip + ")";
		if (count > 0)
			evbuffer_add_printf(buf, ",{\"id\":%u,\"name\":\"%s\",\"level\":%u,\"ip\":\"%s\",\"logintime\":%u}",
			item.second.role->dwRoleID, item.second.role->roleName.c_str(),
			item.second.role->wLevel, host_ip.c_str(), item.second.client->connect_time);
		else
			evbuffer_add_printf(buf, "{\"id\":%u,\"name\":\"%s\",\"level\":%u,\"ip\":\"%s\",\"logintime\":%u}",
			item.second.role->dwRoleID, item.second.role->roleName.c_str(),
			item.second.role->wLevel, host_ip.c_str(), item.second.client->connect_time);
		count++;
	}
	evbuffer_add_printf(buf, "]}");
	evhttp_send_reply(request, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

void http_request_broadcast(evhttp_request* request, void* v)
{
	if (!verify_client_ip(request))return;
	xLog("Info_", "http_request_broadcast.");

	auto uri = evhttp_request_get_evhttp_uri(request);
	auto content = evhttp_uri_get_query(uri);

	int result = 0;
	if (content == nullptr || content[0] == 0)
		result = 1;
	else
	{
		content = evhttp_decode_uri(content);
		BroadcastCtrl::systemBroadcast(content);
	}

	write_json_header(request);
	struct evbuffer *buf = evbuffer_new();
	evbuffer_add_printf(buf, "{\"result\":%d}", result);
	evhttp_send_reply(request, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

void http_request_root(evhttp_request* request, void* v)
{
	if (!verify_client_ip(request))return;

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


bool service::process_cmd(int argc, char* argv[])
{
	if (argc > 1)
	{
		map<string, string> cmds;
		for (int i = 1; i < argc; i++)
		{
			vector<string> v;
			BASE::strtoken2Vec(argv[i], "=", v);
			if (v.size() == 1)
				cmds.insert(make_pair(v[0], ""));
			else
				cmds.insert(make_pair(v[0], v[1]));
		}

		if (cmds.count("sqlfile"))
		{
			load_server_config();
			database db;
			db.init(main_db_config.ip, main_db_config.schema, main_db_config.user, main_db_config.pwd, main_db_config.port);
			fstream file(cmds["sqlfile"]);
			if (file)
			{
				file.seekg(0, ios::end);
				auto len = file.tellg();
				string buffer;
				file.seekg(0, ios::beg);
				buffer.resize((size_t)len);
				file.read(&buffer[0], buffer.size());
				try
				{
					db.exe(buffer);
				}
				catch (exception& exp)
				{
					cout << exp.what() << endl;
				}
			}
			return true;
		}
	}
	return false;
}
void service::on_signal_terminate(evutil_socket_t fd, short event, void *arg)
{
	//return;//不能存数据 可能产生未知错误

	//service::notify_all_close_gs();

	event_base_loopbreak(service::event_handler);

	PROTOCAL::saveAllToDb(true);

	logonline( "pkill and save time: %u",time(0) );
}


int service::run(int argc, char **argv)
{
	if (process_cmd(argc, argv))
		return 0;
	srand(getpid());
	srand(time(NULL));
	BASE::writeLastPktFile(0xffffffff, "", 0);
	init();
	load_config();
	//e = event_new(event_handler, -1, EV_SIGNAL, event_test, 0);

	//thread t([](void* v){
	//	while(1)
	//	{
	//		Sleep(5000);
	//		event_active(e, 0, EV_SIGNAL);
	//	}
	//}, nullptr);
	//t.detach();

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
	evhttp_set_cb(http, "/cache_count", http_request_get_cache_count, 0);
	evhttp_set_cb(http, "/broadcast", http_request_broadcast, 0);
	evhttp_set_cb(http, "/online_players", http_request_online_players, 0);
	evhttp_set_gencb(http, http_request_any, 0);

	PROTOCAL::init_timers();

	logs_db_thread::singleton();

	// 添加CtrlC事件
	struct event ev;
#ifdef _WIN32

	//SIGINT
	evsignal_set(&ev, 2, service::on_signal_terminate, &ev);
#else

	//SIGTERM
	evsignal_set(&ev, 15, service::on_signal_terminate, &ev);

#endif
	event_base_set(service::event_handler, &ev);

	evsignal_add(&ev, NULL);


	event_base_dispatch(service::event_handler);
	
	evhttp_free(http);
	evconnlistener_free(listener);
	evconnlistener_free(bk_listener);
	return 0;
}


uint service::get_days_of_open()
{
	auto escape = (uint)time(nullptr) - open_time;
	if (escape == 0)
		return 1;
	return escape / 24 * 3600 + 1;
}


void service::init()
{
//#ifdef WIN32
//	evthread_use_windows_threads();
//	struct event_config *cfg = event_config_new();
//	if(cfg == nullptr)
//		THROW_EXCEPTION("event_config_new failed.");
//
//	event_config_set_flag( cfg, EVENT_BASE_FLAG_STARTUP_IOCP );
//
//	event_handler = event_base_new_with_config(cfg);
//	if(event_handler == nullptr)
//		THROW_EXCEPTION("event_base_new_with_config failed.");
//#else
	event_handler = event_base_new();
	if(event_handler == nullptr)
		THROW_EXCEPTION("event_base_new failed.");
//#endif
	http = evhttp_new(event_handler);
	if(http == nullptr)
		THROW_EXCEPTION("evhttp_new failed.");
}


void service::load_config()
{
	load_server_config();
	DBCtrl::dbInit();
	load_meet_pk_rank();
	load_meet_combo_rank();
	load_map_to_players_data();
	load_elitemap_first_killer_data();
	load_server_config_from_db();
	guild::load_config();
	WorldBoss::getMe().load_from_db();
	CONFIG::initCfg();
	item::load_config();
	guild::load();
	RANK::initDB2Ranks();
	ARENA::initArenaRoles();
	ItemCtrl::init();

	string temp_open_time;
#ifdef __linux__
	time_t o = 0;
	tm t;
	localtime_r(&o, &t);
	tzone = t.tm_gmtoff;
#else
	tzone = ::timezone;
#endif
	tzone = -abs(tzone);
	WorldBoss::getMe().load_config();
	WorldBoss::getMe().init();
	cout << "time zone:" << tzname[0] << " zone value:" << tzone / 3600 << " utc time:" << time(nullptr) << endl;
	if (!get_config("open_time", temp_open_time))
	{
		auto now = time(nullptr);
		set_config("open_time", to_string(now));
		now = now - tzone;
		open_time = now - (now % (3600 * 24)) + tzone;
	}
	else
	{
		auto temp = (uint)strtoul(temp_open_time.c_str(), 0, 10) - tzone;
		open_time = temp - (temp % (3600 * 24)) + tzone;
	}
	cout << "open_time:" << open_time << endl;
	activity_top_upgrade::load();
	activity_taobao::load();
	treasure::load();
	bodyguard::load();
	bodyguard::load_data();

	RoleMgr::getMe().init();

	logFATAL("load_config() ok ");
}


void service::load_server_config()
{
	string config_file = string( getProgDir() )+ STR_FSEP + "config.xml";

	TiXmlDocument doc;
	if(!doc.LoadFile(config_file.c_str()))
		THROW_EXCEPTION("can't load config.xml");

	auto root = doc.RootElement();

	if(!tixml_value(root->FirstChild("id"), id))
		THROW_EXCEPTION("can't read server id");

	if (!tixml_value(root->FirstChild("ip"), ip))
		THROW_EXCEPTION("can't read server ip");

	if (!tixml_value(root->FirstChild("port"), port))
		THROW_EXCEPTION("can't read server port");

	if (!tixml_value(root->FirstChild("backend_ip"), backend_ip))
		THROW_EXCEPTION("can't read server backend_ip");

	if (!tixml_value(root->FirstChild("backend_port"), backend_port))
		THROW_EXCEPTION("can't read server backend_port");

	string backend_limit_ip_list_str;
	if (!tixml_value(root->FirstChild("backend_limited_ip"), backend_limit_ip_list_str))
		THROW_EXCEPTION("can't read server backend_limited_ip");
	vector<string> backend_limit_ip_list;
	BASE::strtoken2Vec(backend_limit_ip_list_str.c_str(), ";", backend_limit_ip_list);
	for(auto& ipstr : backend_limit_ip_list)
		backend_limited_ips.insert(inet_addr(ipstr.c_str()));

	if (!tixml_value(root->FirstChild("http_address"), http_address))
		THROW_EXCEPTION("can't read server http_address");

	if (!tixml_value(root->FirstChild("http_port"), http_port))
		THROW_EXCEPTION("can't read server http_port");

	string http_limit_ip_list_str;
	if (!tixml_value(root->FirstChild("http_limited_ip"), http_limit_ip_list_str))
		THROW_EXCEPTION("can't read server http_limited_ip");
	vector<string> http_limited_ip_list;
	BASE::strtoken2Vec(http_limit_ip_list_str.c_str(), ";", http_limited_ip_list);
	for(auto& ipstr : http_limited_ip_list)
		http_limited_ips.insert(inet_addr(ipstr.c_str()));

	if (!tixml_value(root->FirstChild("db_ip"), main_db_config.ip))
		THROW_EXCEPTION("can't read main db ip");
	if (!tixml_value(root->FirstChild("db_schema"), main_db_config.schema))
		THROW_EXCEPTION("can't read main db schema");
	if (!tixml_value(root->FirstChild("db_user"), main_db_config.user))
		THROW_EXCEPTION("can't read main db user");
	if (!tixml_value(root->FirstChild("db_pwd"), main_db_config.pwd))
		THROW_EXCEPTION("can't read main db pwd");
	if(!tixml_value(root->FirstChild("db_port"), main_db_config.port))
		THROW_EXCEPTION("can't read main db port");

	if(!tixml_value(root->FirstChild("logdb_ip"), logs_db_config.ip))
		THROW_EXCEPTION("can't read log db ip");
	if(!tixml_value(root->FirstChild("logdb_schema"), logs_db_config.schema))
		THROW_EXCEPTION("can't read log db schema");
	if(!tixml_value(root->FirstChild("logdb_user"), logs_db_config.user))
		THROW_EXCEPTION("can't read log db user");
	if(!tixml_value(root->FirstChild("logdb_pwd"), logs_db_config.pwd))
		THROW_EXCEPTION("can't read log db pwd");
	if(!tixml_value(root->FirstChild("logdb_port"), logs_db_config.port))
		THROW_EXCEPTION("can't read log db port");

	if(!tixml_value(root->FirstChild("check_account"), account))
		THROW_EXCEPTION("can't check_account sign");
	if(!tixml_value(root->FirstChild("gm_no_need_name"), gm_no_need_name))
		THROW_EXCEPTION("can't check_account sign");

	logINFO("load config.xml done.");
}


void service::load_server_config_from_db()
{
	SQLSelect sqlx( DBCtrl::getSQL(), "select * from global_config");

	while( MYSQL_ROW row = sqlx.getRow() )
	{
		global_config.insert(make_pair(row[0], row[1]));
	}
}


void service::set_config(string key, string value)
{
	if(global_config.find(key) == global_config.end())
	{
		execSQL(DBCtrl::getSQL(), "insert into global_config values('%s', '%s')", key.c_str(), value.c_str());
	}
	else
	{
		execSQL(DBCtrl::getSQL(), "update global_config set value='%s' where name='%s'", value.c_str(), key.c_str());
	}
	global_config[key] = value;
}


void service::load_meet_pk_rank()
{
	SQLSelect sqlx( DBCtrl::getSQL(), "select * from meet_pk_rank order by pkvalue desc");

	while( MYSQL_ROW row = sqlx.getRow() )
	{
		rank_item item;
		item.id = atoi(row[0]);
		auto len = strlen(row[1]);
		memcpy(item.name, row[1], min<size_t>(len, NAME_MAX_LEN));
		item.name[NAME_MAX_LEN - 1] = 0;
		item.job = atoi(row[2]);
		item.sex = atoi(row[3]);
		item.level = atoi(row[4]);
		item.value = atoi(row[5]);
		meet_pk_rank.push_back(item);
	}
	if(meet_pk_rank.size() >= max_meet_pk_rank_count)
		min_meet_pk_value = meet_pk_rank.back().value;
}


void service::load_meet_combo_rank()
{
	SQLSelect sqlx( DBCtrl::getSQL(), "select * from rank_meet_combo order by combo desc");

	while( MYSQL_ROW row = sqlx.getRow() )
	{
		rank_item item;
		item.id = atoi(row[0]);
		auto len = strlen(row[1]);
		memcpy(item.name, row[1], min<size_t>(len, NAME_MAX_LEN));
		item.name[NAME_MAX_LEN - 1] = 0;
		item.job = atoi(row[2]);
		item.sex = atoi(row[3]);
		item.level = atoi(row[4]);
		item.value = atoi(row[5]);
		meet_combo_rank.push_back(item);
	}
	if (meet_combo_rank.size() >= max_meet_pk_rank_count)
		min_meet_combo_value = meet_combo_rank.back().value;
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


void service::save_data( bool bForce )
{
	save_elitemap_first_killer();
	//auto s = get_counter();
	RoleMgr::getMe().save_data( bForce );
	//auto e = get_counter();
	//cout << (e - s) / get_cpu_performance() << endl;
	bodyguard::save_data();
}


void service::save_meet_pk_rank(rank_item& item)
{
	//char name[NAME_MAX_LEN * 2 + 1];
	//for (auto & item : meet_pk_rank)
	//{
	//	item.name[NAME_MAX_LEN - 1] = 0;
	//	mysql_escape_string(name, item.name, strlen(item.name));
	//	execSQL(DBCtrl::getSQL(), "insert into meet_pk_rank values(%u, '%s', %u, %u, %u, %u)",
	//		item.id, name, item.job, item.sex, item.level, item.value);
	//}
	MYSQL_ESCAPE_N(rolename, item.name, min<size_t>(sizeof(item.name), ROLE_NAME_LEN), ROLE_NAME_LEN);
	if (!execSQL(DBCtrl::getSQL(), "insert into meet_pk_rank(id, name, job, sex, level, pkvalue) value(%u, '%s', %u, %u, %u, %u) \
								   		on duplicate key update name='%s', job=%u, sex=%u, level=%u, pkvalue=%u",
										item.id, rolename, item.job, item.sex, item.level, item.value, rolename, item.job, item.sex, item.level, item.value))
	{
		logFATAL("save_meet_pk_rank error %s", mysql_error(DBCtrl::getSQL()));
	}
}


void service::save_meet_combo_rank(rank_item& item)
{
	MYSQL_ESCAPE_N(rolename, item.name, min<size_t>(sizeof(item.name), ROLE_NAME_LEN), ROLE_NAME_LEN);
	if (!execSQL(DBCtrl::getSQL(), "insert into rank_meet_combo(id, name, job, sex, level, combo) value(%u, '%s', %u, %u, %u, %u) \
								   		on duplicate key update name='%s', job=%u, sex=%u, level=%u, combo=%u",
										item.id, rolename, item.job, item.sex, item.level, item.value, rolename, item.job, item.sex, item.level, item.value))
	{
		logFATAL("save_meet_combo_rank error %s", mysql_error(DBCtrl::getSQL()));
	}
}


void service::save_elitemap_first_killer()
{
	if(!elitemap_first_killer_changed)
		return;
	execSQL(DBCtrl::getSQL(), "truncate table rank_elite_first_killer");

	for(auto & item : elitemap_first_killer)
	{
		if (!execSQL(DBCtrl::getSQL(), "insert into rank_elite_first_killer(mapid, roleid, rolename) values(%u, %u, '%s')",
			item.first, item.second.first, item.second.second.c_str()))
		{
			logERROR("insert into rank_elite_first_killer %s", mysql_error(DBCtrl::getSQL()));
		}
	}
	elitemap_first_killer_changed = false;
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


void service::load_elitemap_first_killer_data()
{
	SQLSelect sqlx( DBCtrl::getSQL(), "select mapid, roleid, rolename from rank_elite_first_killer");

	while( MYSQL_ROW row = sqlx.getRow() )
	{
		elitemap_first_killer.insert(make_pair((uint16)atoi(row[0]), make_pair((unsigned)atoi(row[1]), row[2])));
	}
}


void service::client_connected(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int slen, void *void_cliend_cb)
{
	new session(fd, addr);
}

void service::backend_connected(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int slen, void *void_cliend_cb)
{
	new backend_session(fd, addr);
	//if(backend_limited_ips.find(temp->ip_ipv4) == backend_limited_ips.end())
	//	temp->disconnect();
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


void service::save_roles()
{
	auto now = time(0);
	for (auto it : online_players)
	{
		if (now - it.second.role->tLastSaveTime > 60)
		{
			it.second.role->tLastSaveTime = now;
			it.second.role->tLogoutTime = now;		//退出时间,防止断线
			it.second.role->roleSave2DB();
		}
	}
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
			if(target == nullptr)continue;
			if(target->dwRoleID == role->dwRoleID)continue;

			int reward_pkvalue;

			if(target->wLevel >= role->wLevel)
				reward_pkvalue = 10 + (target->wLevel - role->wLevel) * 2;
			else
			{
				reward_pkvalue = 10 - (role->wLevel - target->wLevel);
				if(reward_pkvalue < 1)reward_pkvalue = 1;
			}

			//auto record = role->get_meet_pk_result(item, false);// 获取针对item的PK记录

			stream.write_uint(item);
			stream.write_string(target->roleName);
			stream.write_byte(target->byJob);
			stream.write_byte(target->bySex);
			stream.write_byte((byte)target->wLevel);
			stream.write_uint(target->dwFightValue);
			stream.write_byte(reward_pkvalue);
			stream.write_byte(/*record?(byte)record->result:*/0);
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
			if (used.size() >= ls.size())
			{
				logERROR("get_map_rand_players rand error");
				return;
			}

			//unsigned roleid = 0;
			auto iter = ls.begin();
			for(int i=0;i<index;i++)iter++;
			
			auto target = RoleMgr::getMe().getRoleByID(*iter);
			if(target == nullptr)continue;
			if(target->dwRoleID == role->dwRoleID)continue;
			//auto record = role->get_meet_pk_result(target->dwRoleID, false);// 获取针对目标的PK记录

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
			stream.write_byte(/*record?(byte)record->result:*/0);
			count++;

			if(count>=4)break;
		}
		stream.write_data((const char*)&count, 1, pos);
	}
}


void service::update_meet_pk_rank(ROLE* role)
{
	if (role->pkvalue < 5 || role->pkvalue < min_meet_pk_value)
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
		if (role->pkvalue > iter->value)
		{
			rank_item item;
			item.value = role->pkvalue;
			item.id = role->dwRoleID;
			item.job = role->byJob;
			item.level = (byte)role->wLevel;
			strcpy(item.name, role->roleName.c_str());
			item.sex = role->bySex;
			meet_pk_rank.insert(iter, item);

			if(meet_pk_rank.size() > max_meet_pk_rank_count)
			{
				meet_pk_rank.pop_back();
				min_meet_pk_value = meet_pk_rank.back().value;
			}
			save_meet_pk_rank(item);
			return;
		}
	}

	if(meet_pk_rank.size() < max_meet_pk_rank_count)
	{
		rank_item item;
		item.value = role->pkvalue;
		item.id = role->dwRoleID;
		item.job = role->byJob;
		item.level = (byte)role->wLevel;
		strcpy(item.name, role->roleName.c_str());
		item.sex = role->bySex;
		meet_pk_rank.push_back(item);
		save_meet_pk_rank(item);
	}
}


void service::update_meet_combo_rank(ROLE* role)
{
	if (role->meet_pk_combo_count < min_meet_combo_value)
		return;

	for (auto iter = meet_combo_rank.begin(); iter != meet_combo_rank.end(); iter++)
	{
		if(iter->id == role->dwRoleID)
		{
			iter = meet_combo_rank.erase(iter);
			break;
		}
	}

	for (auto iter = meet_combo_rank.begin(); iter != meet_combo_rank.end(); iter++)
	{
		if (role->meet_pk_combo_count > iter->value)
		{
			rank_item item;
			item.value = role->meet_pk_combo_count;
			item.id = role->dwRoleID;
			item.job = role->byJob;
			item.level = (byte)role->wLevel;
			strcpy(item.name, role->roleName.c_str());
			item.sex = role->bySex;
			meet_combo_rank.insert(iter, item);

			if (meet_combo_rank.size() > max_meet_pk_rank_count)
			{
				meet_combo_rank.pop_back();
				min_meet_combo_value = meet_combo_rank.back().value;
			}
			save_meet_combo_rank(item);
			return;
		}
	}

	if (meet_combo_rank.size() < max_meet_pk_rank_count)
	{
		rank_item item;
		item.value = role->meet_pk_combo_count;
		item.id = role->dwRoleID;
		item.job = role->byJob;
		item.level = (byte)role->wLevel;
		strcpy(item.name, role->roleName.c_str());
		item.sex = role->bySex;
		meet_combo_rank.push_back(item);
		save_meet_combo_rank(item);
	}

}


void service::elitemap_done(ROLE* role, uint16_t mapid, uint16_t bossid)
{
	if(elitemap_first_killer.find(mapid) != elitemap_first_killer.end())
		return;

	elitemap_first_killer[mapid] = make_pair(role->dwRoleID, role->roleName);
	elitemap_first_killer_changed = true;

	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::elitemap_done);
	out.write_string(role->roleName);
	out.write_ushort(bossid);
	broadcast(out);
}

bool service::get_config(const string& key, string& value)
{
	if(global_config.find(key) == global_config.end())
		return false;

	value = global_config[key];
	return true;
}


bool service::get_elitemap_first_killer(uint16_t mapid, unsigned& roleid, string& name)
{
	auto iter = elitemap_first_killer.find(mapid);
	if(iter == elitemap_first_killer.end())
		return false;

	roleid = iter->second.first;
	name = iter->second.second;
	return true;
}


void service::broadcast(output_stream& stream)
{
	for (auto& item : online_players)
	{
		if (item.second.client)
			item.second.client->send(stream.get_buffer(), stream.length());
	}
}


uint64_t service::get_counter()
{
#ifdef WIN32
	return __rdtsc();
#else
	return 0;
#endif
}


uint64_t service::get_cpu_performance()
{
#ifdef WIN32
	LARGE_INTEGER bigint;
	QueryPerformanceFrequency(&bigint);
	return bigint.QuadPart;
#else
	return 0;
#endif
}


void service::set_timer(time_t second, void(*func)())
{
	new timer(second, func);
}

uint service::get_online_count()
{
	return online_players.size();
}

void service::attach_role_and_session(ROLE* role, session* client)
{
	auto it = online_players.find(role->dwRoleID);
	if (it != online_players.end())
	{
		close_session(it->second.client,1);
	}

	role->client = client;
	client->role_id = role->dwRoleID;
	online_players.insert(make_pair(role->dwRoleID, ROLE_CLIENT(role, client)));
}

void service::close_session(session* client, int immediate)
{
	if (client == nullptr)
		return;

	output_stream out(S_BREAK);
	out.write_short(immediate);
	client->send(out.get_buffer(), out.length());

	if (client->role_id)
	{
		ROLE* role = RoleMgr::getMe().getMe().getRoleByID(client->role_id);
		role->client = nullptr;
		role->tLogoutTime = time(nullptr);
		role->roleSave2DB();
		online_players.erase(client->role_id);
	}

	if ( immediate == 0 )
	{
		bufferevent_free(client->buffer);
		client->buffer = nullptr;
		delete client;
	}
	else//need to notify client
	{
		client->closed = true;
	}
}


void service::log_online_info()
{
	for (auto& item : online_players)
	{
		DWORD id = item.second.role->dwRoleID;
		string name = item.second.role->roleName;
		logonline("id=%d name=%s", id, name.c_str());
	}
}

void service::notify_all_close_gs()
{
	for (auto& player : online_players)
	{
		session* client = player.second.client;
		if (client == nullptr)
			continue;

		string strData;
		S_APPEND_WORD(strData, 0);
		output_stream out(S_BREAK);
		client->send(out.get_buffer(), out.length());
	}
}


session* service::get_online_session(uint roleid)
{
	auto iter = online_players.find(roleid);
	if (iter == online_players.end())
		return nullptr;

	return iter->second.client;
}


ROLE* service::get_online_role(uint roleid)
{
	auto iter = online_players.find(roleid);
	if (iter == online_players.end())
		return nullptr;

	return iter->second.role;
}

uint service::get_first_meet_pk_id()
{
	if (meet_pk_rank.empty())
		return 0;

	return meet_pk_rank.front().id;
}

uint service::get_first_meet_combo_id()
{
	if (meet_combo_rank.empty())
		return 0;

	return meet_combo_rank.front().id;
}


int service::rpc_get_meet_rank(ROLE* role, input_stream& stream)
{
	byte type;
	if (!stream.read_byte(type))
		return 0;

	if (type == 0)
	{
		output_stream out(S_MEET_PK_RANK);
		out.write_byte(0);
		out.write_byte(service::meet_pk_rank.size());
		for (auto& item : service::meet_pk_rank)
		{
			auto temp = RoleMgr::getMe().getRoleByID(item.id);
			if (temp)
			{
				item.level = (byte)temp->wLevel;
			}
			out.write_data((char*)&item, sizeof(item));
		}
		role->send(out);
	}
	else if (type == 1)
	{
		output_stream out(S_MEET_PK_RANK);
		out.write_byte(1);
		out.write_uint(role->meet_pk_max_combo_count);
		out.write_byte(service::meet_combo_rank.size());
		for (auto& item : service::meet_combo_rank)
		{
			auto temp = RoleMgr::getMe().getRoleByID(item.id);
			if (temp)
			{
				item.level = (byte)temp->wLevel;
			}
			out.write_data((char*)&item, sizeof(item));
		}
		role->send(out);
	}
	return 0;
}