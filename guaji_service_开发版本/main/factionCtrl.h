#pragma once
#include "config.h"
#include "role.h"


class ROLE_BLOB;

enum E_FACTION_JOB
{
	E_NONE = 0,			//û�м������
	E_MEMBER = 1,		//��ͨ����
	E_LEADER = 2,		//����
	E_LEADER2 = 3,		//������
	E_GUADIAN_LEFT = 4,	//�󻤷�
	E_GUADIAN_RIGHT = 5,	//�һ���
};


#define MAX_BUILDER_LEVEL (30)


enum E_BUILDER_TYPE
{
	E_BUILDER_TYPE_MAIN = 1,		//1�л�	�������
	E_BUILDER_TYPE_WELFARE = 2,		//2	����	��������,����
	E_BUILDER_TYPE_TEMPLE = 3,		//3	ʥ��	0
	E_BUILDER_TYPE_HP = 4,			//4	����ֵ	����hp_max��ֵ
	E_BUILDER_TYPE_MP = 5,			//5	����ֵ	����mp_max��ֵ
	E_BUILDER_TYPE_ATK = 6,			//6	������	������������ֵ����ʽ���﹥����,�﹥����
	E_BUILDER_TYPE_MAGIC_ATK = 7,	//7	ħ������	����ħ������ֵ����ʽ��ħ������,ħ������
	E_BUILDER_TYPE_SOL_ATK = 8,		//8	��������	���ӵ�������ֵ����ʽ����������,��������
	E_BUILDER_TYPE_DEF = 9,			//9	�������	������������ֵ����ʽ���������,�������
	E_BUILDER_TYPE_MAG_DEF = 10,	//10	ħ������	������������ֵ����ʽ��ħ������,ħ������
	E_BUILDER_NUMS = 10,
};

class guild_member
{
public:
	unsigned roleid;
	string name;
	byte vocation = 0;
	uint16_t level = 0;
	E_FACTION_JOB job = E_MEMBER;
	unsigned contribute = 0;
	size_t contribute_rank = 1;//��������
	unsigned contribute_count = 0;
};

typedef guild_member* guild_member_ptr;

// ����
class guild
{
	class contribute_reward
	{
	public:
		unsigned coin;
		unsigned gold;
		unsigned guild_exp;
		unsigned person_contribute;
	};

	class level_info
	{
	public:
		unsigned max_member = 10;
		unsigned exp = 0;
		unsigned reward_coin = 0;
		unsigned reward_gold = 0;
	};

	struct chat
	{
		time_t time;
		E_FACTION_JOB job;
		unsigned roleid;
		string name;
		string content;
	};

	static map<unsigned, guild*> guilds_id_index;// ����ID����
	static map<string, guild*> guilds_name_index;// ������������
	static map<unsigned, guild*> guilds_roleid_index;// �����ɫID����
	static vector<guild*> guilds;// �����б�
	static list<guild*> sorted_guilds;// ��������
	static bool _needSortFaction;

	static void add(guild* value);
	static void remove(guild* value);

public:
	unsigned id = 0;
	string name;
	string leader_name;
	unsigned leader_roleid = 0;
	string leader2_name;
	unsigned leader2_roleid = 0;
	string admin1_name;//�󻤷�
	unsigned admin1_roleid = 0;
	string admin2_name;//�һ���
	unsigned admin2_roleid = 0;

	size_t rank;	//��������

	unsigned level = 0;
	unsigned exp = 0;
	unsigned contribute = 0;
	string notify;//���ɹ���
	unsigned join_condition = 0;
	uint16 shrine_level = 1;
	DWORD shrine_exp = 0;//ʥ�ؾ���
	uint16 wordship_shrine_times = 0;//ʥ���Ѿ�Ĥ�ݴ���
	bool need_save = false;

	map<unsigned, guild_member_ptr> members;// �����Ա����
	list<guild_member_ptr> sorted_members;// ����Ĺ����Ա����
	deque<chat> chats;

	void addChat(ROLE* role, const string& strContent);
	void add_member(ROLE* role);
	void remove_member(guild_member_ptr member);
	//	bool upgradeBuilder( E_BUILDER_TYPE e );
	unsigned getBuilderUpgradeExp(uint16_t wLevel, E_BUILDER_TYPE eType);
	unsigned get_max_member();
	void sort_contribute();
	guild_member_ptr get_member(unsigned roleid);
	size_t get_member_count();
	void do_contribute(ROLE* value, byte type);
	void shrine_wordship(ROLE* role);
	void get_wordship_info(ROLE* role);
	static float get_shrine_addcoin_coff(ROLE* role);
	bool full();

private:
	static vector<contribute_reward> contribute_rewards;
	static vector<level_info> level_exps;

public:
	guild(const string& name);
	static void load_config();
	static contribute_reward* get_contribute_reward(unsigned count);
	static level_info* get_level_info(unsigned id);
	static unsigned get_max_level();

	static int rpc_get_recommend_list(ROLE* role, unsigned char * data, size_t dataLen);
	static int rpc_get_rank_list(ROLE* role, unsigned char * data, size_t dataLen);
	static int rpc_find(ROLE* role, unsigned char * data, size_t dataLen);
	static int rpc_exchange(ROLE* role, input_stream& stream);

	static DECLARE_CLIENT_FUNC(rpc_create);
	static DECLARE_CLIENT_FUNC(rpc_getinfo);
	static DECLARE_CLIENT_FUNC(rpc_create_content);
	static DECLARE_CLIENT_FUNC(rpc_get_member_list);
	static DECLARE_CLIENT_FUNC(rpc_get_mem_equip);
	static DECLARE_CLIENT_FUNC(rpc_add_friend);
	static DECLARE_CLIENT_FUNC(rpc_appoint_job);
	static DECLARE_CLIENT_FUNC(rpc_kick_member);
	static DECLARE_CLIENT_FUNC(rpc_quit);
	static DECLARE_CLIENT_FUNC(rpc_get_chatlist);
	
	static void load();
	static guild* create(ROLE* role, const string& factionName);
	static guild* get_guild(unsigned dwFactionID);
	static guild* get_guild(const string& name);
	static guild* get_guild_from_roleid(unsigned roleid);
	void contribute_up(guild_member* member);
	static void broadcast(guild* faction, output_stream& stream);
	static uint get_first_leader_id();// ��ȡ��һ����᳤
	static void save_guilds();
	static void daily_reset_guilds();
};
