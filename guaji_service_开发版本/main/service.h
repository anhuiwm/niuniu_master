#pragma once
#include "config.h"
#include <event2/http.h>



#pragma pack(push, 1)
// ż��PK���а���Ŀ
class rank_item
{
public:
	unsigned id;
	char name[NAME_MAX_LEN];
	byte  job;
	byte  sex;
	byte  level;
	unsigned value;

	rank_item()
	{
		memset(name, 0, NAME_MAX_LEN);
	}
};


// �����Ϲ�ս��Ա
//class worldboss_staff
//{
//public:
//	unsigned roleid;
//	unsigned cooldown;// ��ȴʱ��
//	unsigned hurt;// �˺�
//	byte enchance; // �Ƿ���� 0���� 1��
//};
#pragma pack(pop)

class service
{
	class db_config
	{
	public:
		string ip;
		string schema;
		string user;
		string pwd;
		unsigned port;
	};

	class timer
	{
		event* e;
		void (*final_triger)();
	public:
		timer(time_t timeout, void (*f)())
		{
			e = evtimer_new(service::event_handler, triger, this);
			//event_base_set(service::event_handler, e);
			struct timeval t = { timeout, 0 };
			evtimer_add(e, &t);
			final_triger = f;
		}

		static void triger(evutil_socket_t fd, short s, void* arg)
		{
			timer* _this = (timer*)arg;
			_this->final_triger();
			evtimer_del(_this->e);
			event_free(_this->e);
			delete _this;
		}
	};
public:
	static const int max_meet_pk_rank_count = 30;
	static unsigned gm_no_need_name;
	static unsigned account;
	static unsigned id;
	static string ip;
	static unsigned port;
	static string backend_ip;
	static unsigned backend_port;
	static set<unsigned> backend_limited_ips;
	static string http_address;
	static unsigned http_port;
	static set<unsigned> http_limited_ips;

	static db_config main_db_config;
	static db_config logs_db_config;

	static event_base* event_handler;
	static evhttp* http;

public:
	static unsigned min_meet_pk_value;
	static unsigned min_meet_combo_value;
	static uint open_time;
	static long tzone;
	static list<rank_item> meet_pk_rank;// ż��PK��
	static list<rank_item> meet_combo_rank;// ż����ɱ��
	static map<uint16_t, list<unsigned>> map_to_players;// ��ͼ��Ӧ�����
	static bool elitemap_first_killer_changed;
	static map<uint16_t, pair<unsigned, string>> elitemap_first_killer;// ��ͼ��ɱ��
	static map<string, string> global_config;
	//static list<worldboss_staff> worldboss_rank;// �����Ϲ�ս���а�
	static map<uint, ROLE_CLIENT> online_players;
	static deque<string> chats;

public:
	service(void);
	~service(void);

	int run(int argc, char **argv);
	static void save_data( bool bForce = false );
	static void on_signal_terminate(evutil_socket_t fd, short event, void *arg);
	uint get_days_of_open();// ��ȡ�����ڼ���
private:
	void init();
	void load_config();
	void load_server_config();// ���ط���������
	void load_server_config_from_db();// ���ط���������
	void load_map_to_players_data();// ����ż����ͼ����������б�
	static void load_elitemap_first_killer_data();// ���ؾ�Ӣ��ͼ��ɱ���а�
	static void load_meet_pk_rank();// ����ż��PK������Ϣ
	static void load_meet_combo_rank();// ����ż����ɱ������Ϣ
	static void save_meet_pk_rank(rank_item& item);// ����ż��PK������Ϣ
	static void save_meet_combo_rank(rank_item& item);// ����ż����ɱ������Ϣ
	static void save_elitemap_first_killer();// ���澫Ӣ��ͼ��ɱ���а�
	//static void load_worldboss_rank();// ����ż��PK������Ϣ
	//static void save_worldboss_rank();// ����ż��PK������Ϣ
	static void client_connected(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int slen, void *void_cliend_cb);
	static void backend_connected(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int slen, void *void_cliend_cb);
	static evconnlistener * create_listener(const string& ip, unsigned port);
	static evconnlistener * create_backend_listener(const string& ip, unsigned port);

	static void save_roles();
	bool process_cmd(int argc, char* argv[]);

public:
	static void player_enter_map(ROLE* role, uint16_t old_map_id);// ��ҽ����ͼ
	static void get_map_rand_players(output_stream& stream, ROLE* role);// ��ȡ���ڵ�ͼ��������
	static void update_meet_pk_rank(ROLE* role);// ����ż�����а�
	static void update_meet_combo_rank(ROLE* role);// ����ż�����а�
	static void elitemap_done(ROLE* role, uint16_t mapid, uint16_t bossid);// ���һ�ξ�Ӣ����ս
	static bool get_config(const string& key, string& value);
	void set_config(string key, string value);
	static bool get_elitemap_first_killer(uint16_t mapid, unsigned& roleid, string& name);
	static void broadcast(output_stream& stream);
	static int rpc_get_meet_rank(ROLE* role, input_stream& stream);

	static uint64_t get_counter();
	static uint64_t get_cpu_performance();
	static void set_timer(time_t second, void (*func)());

	static uint get_online_count();
	static void attach_role_and_session(ROLE* role, session* client);
	static void close_session(session* client, int immediate = 0);
	static void log_online_info();
	static void notify_all_close_gs();

	static session* get_online_session(uint roleid);
	static ROLE* get_online_role(uint roleid);
	static uint get_first_meet_pk_id();// ��ȡż��PK�������ID������0��ʾ������
	static uint get_first_meet_combo_id();// ��ȡż����ɱ�������ID������0��ʾ������
};