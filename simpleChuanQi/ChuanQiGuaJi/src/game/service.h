#pragma once
#include "config.h"
#include <event2/event.h>
#include <event2/http.h>
#include <set>
#include "output_stream.h"


#pragma pack(push, 1)
// ż��PK���а���Ŀ
class meet_pk_rank_item
{
public:
	unsigned id;
	char name[NAME_MAX_LEN];
	byte  job;
	byte  sex;
	byte  level;
	unsigned pkvalue;
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
	static list<meet_pk_rank_item> meet_pk_rank;// ż��PK��
	static map<uint16_t, list<unsigned>> map_to_players;// ��ͼ��Ӧ�����
	//static list<worldboss_staff> worldboss_rank;// �����Ϲ�ս���а�

public:
	service(void);
	~service(void);

	int run(int argc, char **argv);
	static void save_data();
private:
	void init();
	void load_config();
	void load_server_config();// ���ط���������
	void load_map_to_players_data();// ����ż����ͼ����������б�
	static void load_meet_pk_rank();// ����ż��PK������Ϣ
	static void save_meet_pk_rank();// ����ż��PK������Ϣ
	//static void load_worldboss_rank();// ����ż��PK������Ϣ
	//static void save_worldboss_rank();// ����ż��PK������Ϣ
	static void client_connected(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int slen, void *void_cliend_cb);
	static void backend_connected(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int slen, void *void_cliend_cb);
	static evconnlistener * create_listener(const string& ip, unsigned port);
	static evconnlistener * create_backend_listener(const string& ip, unsigned port);

public:
	static void player_enter_map(ROLE* role, uint16_t old_map_id);// ��ҽ����ͼ
	static void get_map_rand_players(output_stream& stream, ROLE* role);// ��ȡ���ڵ�ͼ��������
	static void meet_pk_rank_update(ROLE* role, unsigned pkvalue);// ����ż�����а�
};

