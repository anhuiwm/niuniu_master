#pragma once
#include "config.h"
#include "protocal.h"
#include "fightCtrl.h"

// � - ����������
class activity_top_upgrade
{
	static uint end_time;
	static unsigned min_rank_level;// ������С�ȼ�ֵ
	static list<pair<unsigned short, pair<unsigned, string>>> rank;// ��
public:
	
	static void load();
	static void save();
	static void update(ROLE* role);// ������
	static void send_data(ROLE* role, byte* pkt, size_t len);

	static void send_award();
};


// � - ����
class activity_fund
{
	enum class e_status
	{
		none,
		l10 = 0x0001,//10��	500
		l20 = 0x0002,//20��	1000
		l30 = 0x0004,//30��	1500
		l40 = 0x0008,//40��	2000
		l50 = 0x0010,//50��	2500
		l55 = 0x0020,//55��	2500
	};
	static void add_user_deliver(unsigned roleid, e_status deliver);
public:
	static bool get_user_deliver(unsigned roleid, int& deliver);
	static void buy(ROLE* role);
	static void get(ROLE* role, input_stream& stream);
};


// � - �Ա�
class activity_taobao
{
	enum class e_status
	{
		none,
		t50 = 0x0001, // �ۼ��Ա�50��		500��ͭǮ
		t100 = 0x0002, // �ۼ��Ա�100��		������ * 50
		t200 = 0x0004, // �ۼ��Ա�200��		5���챦ʯ * 1
		t500 = 0x0008, // �ۼ��Ա�500��		��װ��Ƭ * 50
		t1000 = 0x0010, // �ۼ��Ա�1000��		ǿ��ʯ * 500
	};
	static uint end_time;
	static map<unsigned, unsigned> counts;
	static void set_info(unsigned roleid, unsigned count, unsigned remain, unsigned reward, unsigned used_count);
public:
	static void load();
	static void save();
	static void charge(unsigned roleid, unsigned count, bool force = false);
	static bool get_count(unsigned roleid, unsigned& count, unsigned& remain, unsigned& reward, unsigned& used_count);
	static void tao(ROLE* role, input_stream& stream);
	static void get_result(ROLE* role, input_stream& stream);
	static void get_reward(ROLE* role, input_stream& stream);
	static void get_info(ROLE* role, input_stream& stream);
};


// ����ϵͳ
class treasure
{
	struct record
	{
		uint roleid;// ��ɫID
		uint count;// �������
		uint hurt;// �˺�ֵ
		uint coin;// �����
		string name;// ����
		byte job;// ְҵ
		byte sex;// �Ա�
		uint16 item;// ����
		byte quality;// Ʒ��
	};
	typedef shared_ptr<record> record_ptr;

	struct info
	{
		list<pair<uint, uint>> history;
		int rank = -1;
	};
	static const uint max_rank_count = 200;
	static unsigned min_value;// ��С����������ֵ
	static list<record_ptr> rank;// ��������, roleid, ����������˺���ֵ��COIN, ����
	static map<uint, info> records;// ��¼ roleid, [count, hurt]
	//static void on_begin_timer();
	static void on_end_timer();

public:
	static bool check;

	static void treasure_done(ROLE* role, input_stream& stream);// ���һ��
	static void get_record(ROLE* role, input_stream& stream);// ��ȡ��¼
	static void get_rank(ROLE* role, input_stream& stream);// ��ȡ����
	static void cost(ROLE* role, input_stream& stream);// ����
	static uint treasure_ranking(record_ptr record);// ��������
	static byte get_remain(ROLE* role);// ��ȡʣ�����
	static void load();// ���ر�������
	static void update(record_ptr record);// ���±�������
	static void dump_rank(ROLE* role);
};


// �ھ�ϵͳ
class bodyguard
{
	enum e_state : byte
	{
		not_start,// δ��ʼ
		inprogress_1,// �����У���һ��
		inprogress_2,// �����У��ڶ���
		inprogress_3,// �����У�������
		today_done,// ���
		history_done,// ������ǰ�����
	};

	// ����������Ϣ
	struct single
	{
		uint reward_item_id = 0;
		uint reward_item_count = 1;
		bool looted_item = false;// �Ƿ�������
		bool got = false;// ��ȡ״̬
		array<uint, 2> looters{};// ������ID

		uint looted_count()// ��������
		{
			return int(looters[0] > 0) + int(looters[1] > 0);
		}
		uint can_loot()// ���Ա���
		{
			if (looters[0] == 0)
				return true;
			if (looters[1] == 0)
				return true;
			return false;
		}
	};
	// ������Ϣ
	struct info
	{
		// ��ʷ��Ŀ
#pragma pack(push, 1)
		struct history_item
		{
			time_t time;// ʱ��
			byte order;// �ڴ�
			uint looterid;// �ٷ�ID
			uint looted_itemid;// ����ID
			history_item(byte order, uint looterid, uint looted_itemid) :order(order), looterid(looterid), looted_itemid(looted_itemid)
			{
				time = ::time(nullptr);
			}
		};
#pragma pack(pop)
		time_t start_time = 0;// ��ʼʱ��
		uint coin = 0;// �������
		uint exp = 0;// ��������
		bool item_plus = false;// ��������
		bool hidden = false;// ���أ�����ʾ���б���
		array<single, 3> guards{};// �ڵ���ϸ��Ϣ
		int divination = -1;
		list<history_item> history;// ���ټ�¼
		bool data_changed = false;

		e_state state();// ��ȡ״̬
		bool add_looter(uint index, uint looterid, uint looted_itemid);// ��ӽٷ�
		void reset();
	};
	// ��������
	struct reward_config
	{
		uint item_id;
		float probability;// ����
		float probability_plus;// ��ǿ����
	};
	// ����ռ������
	struct divination_config
	{
		float coin_up;// �Ի�ȡ��ҵ�����ֵ
		float exp_up;// �Ի�ȡ���������ֵ
		float item_up;// �Ե������и��ʵ�����ֵ
		float probability;// ���д��Եĸ���
	};
	// VIP���ڴ�����������
	struct vip_loot_count_config
	{
		uint count = 0;
		uint ingod = 0;
	};

	static time_t round_time;// һ������ʱ��(��λ��Сʱ)
	static uint hidden_price;// ���б����صļ۸�
	static uint item_plus_price;// ���������۸�
	static uint divination_price;// ռ���۸�
	static uint list_count;// �б�����
	static float loot_item_probability;// ���ٵ��߸���
	static uint double_start_time, double_end_time;// ˫��������ʼ������ʱ��(��λ��Сʱ)
	static vector<reward_config> reward_configs;// ��������
	static vector<divination_config> divination_configs;// ռ������
	static vector<vip_loot_count_config> vip_loot_count_configs;// VIP�������ٴ�������
	static map<uint, info> records;// ��¼

	static void send_reward(ROLE* role, info& reward, single& item);// ���͵��ν���

public:
	static int been_loot(ROLE* role, ROLE* looter, const byte result);// ������
	static int rpc_start(ROLE* role, input_stream& stream);// ��ʼ����
	static int rpc_get_reward(ROLE* role, input_stream& stream);// ��ȡ���ڽ���
	static int rpc_get_status(ROLE* role, input_stream& stream);// ��ȡ���ڽ�����Ϣ
	static int rpc_buy_loot_count(ROLE* role, input_stream& stream);// �������ٴ���
	static int rpc_buy_divination(ROLE* role, input_stream& stream);// ��һ��
	static int rpc_list(ROLE* role, input_stream& stream);// �б�
	static int rpc_been_loot_record(ROLE* role, input_stream& stream);// �б�

	// �������Ϣ
	enum pre_loot_info : byte
	{
		ok,
		target_can_not_loot,// Ŀ�겻���ٱ����
		not_running,// Ŀ��û��������
	};
	static pre_loot_info get_state(uint roleid, uint targetid);// ����
	static void get_info(ROLE* role, byte& state, byte& remain_receive, time_t& start_time);// ����ȡ����
	static void debug_set_time(ROLE* role, time_t time = 0);// ���Թ��� �����ڳ�ʱ�䣬Ĭ����0��δ��ʼ״̬������λСʱ

	static void save_data();// ��������
	static void load_data();// ��������
	static void load();// ��������
};


namespace ACTIVITY
{
	//	enum E_ACTIVITY_ID
	//	{
	//		WORLD_BOSS_FIGHT		 = 1,//����bossս
	//		CHANGSHA_BOSS_FIGHT		 = 2,//ɳ������ս
	//		HANGHUI_BOSS_FIGHT		 = 3,//�л�����ս
	//		CHANGBANPO_BOSS_FIGHT	 = 4,//Ѫս������
	//	};

	enum E_ACTIVITY_RETURN
	{
		ACTIVITY_SUCCESS = 0,//�Ѿ���ʼ
		ACTIVITY_WAITING = 1,//������
		ACTIVITY_NOT_OPEN = 2, //��δ��ʼ
		BOSSFIGHT_ENHANCE_ENOUGH = 3,//�����������
		COLD_NOT_ENOUGH = 4,//Ԫ������
		ALREADY_ENLIST = 5,//�Ѿ�����
		ACTIVITY_NO_EXSIST = 6,//������
		ACTIVITY_START_FIGHT = 7,//ս��
		ACTIVITY_BOSS_DIE = 8,//�����Ѿ�����
		NO_FIGHT_COLD_ENOUGH = 9,//Ϊ����30��
	};

	//	enum E_BOSS_FIGHT_USE
	//	{
	//		E_MALE_CLOTH        = 10009,//Ĭ�������·�
	//		E_FMALE_CLOTH       = 10010,//Ĭ��Ů���·�
	//		FIGHT_SKILL_MONK    = 65533,//��ʿĬ�ϼ���
	//		FIGHT_SKILL_MAGIC   = 65534,//Ĭ��
	//		FIGHT_SKILL_WARRIOR = 65535,//Ĭ��
	//	};
	
	int clientGetActivities( ROLE* role, unsigned char * data, size_t dataLen );
	//
	//	//int registerActivity( ROLE* role, unsigned char * data, size_t dataLen );
	//	
	//	//int clientBossFight( ROLE* role, unsigned char * data, size_t dataLen );
	//
	//	//int enhanceBossFightActtivity( ROLE* role, unsigned char * data, size_t dataLen );
	//
	int getActtivityLeftTime( ROLE* role, unsigned char * data, size_t dataLen );
	//
	//	//int getBossFightHelper( ROLE* role, unsigned char * data, size_t dataLen );
	//	//int getBossFightHelperMe( ROLE* role, int nTag = 0);
	//	//int getBossFightHelperOther( ROLE* role, DWORD& helperID, int nTag = 0);
	//	//int startActivity( WORD id);
	//
	//	//int clientGetBossFightInfos( ROLE* role );
	//
	//	void startTodaySec(const time_t& curTime);
	//
	//	WORD getBossFightID();
	//	void setBossFightID(WORD bossID);
	//
	//	//int clientBossFight2( ROLE* role, unsigned char * data, size_t dataLen );
	//	//int getBossFightHelper2( ROLE* role, unsigned char * data, size_t dataLen );
	//	void notifyActivityStatus( ROLE* role, const BYTE& status, const WORD& actID, DWORD dwTime = 0 );
	//	void judgeNewActivity( ROLE* role, const WORD& wOldLevel );
	//	void notifyRoleActivityStatus( ROLE* role );
	//	void setBossFightStatusNO( );
	//	//int getBossFightRank( ROLE* role, unsigned char * data, size_t dataLen );
	DWORD getTodayNowSecond( time_t now);
}

