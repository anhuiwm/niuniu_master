#pragma once
#include "config.h"
#include "protocal.h"
#include "fightCtrl.h"

// 活动 - 升级新人王
class activity_top_upgrade
{
	static uint end_time;
	static unsigned min_rank_level;// 榜上最小等级值
	static list<pair<unsigned short, pair<unsigned, string>>> rank;// 榜
public:
	
	static void load();
	static void save();
	static void update(ROLE* role);// 升级榜
	static void send_data(ROLE* role, byte* pkt, size_t len);

	static void send_award();
};


// 活动 - 基金
class activity_fund
{
	enum class e_status
	{
		none,
		l10 = 0x0001,//10级	500
		l20 = 0x0002,//20级	1000
		l30 = 0x0004,//30级	1500
		l40 = 0x0008,//40级	2000
		l50 = 0x0010,//50级	2500
		l55 = 0x0020,//55级	2500
	};
	static void add_user_deliver(unsigned roleid, e_status deliver);
public:
	static bool get_user_deliver(unsigned roleid, int& deliver);
	static void buy(ROLE* role);
	static void get(ROLE* role, input_stream& stream);
};


// 活动 - 淘宝
class activity_taobao
{
	enum class e_status
	{
		none,
		t50 = 0x0001, // 累计淘宝50次		500万铜钱
		t100 = 0x0002, // 累计淘宝100次		黑铁矿 * 50
		t200 = 0x0004, // 累计淘宝200次		5级红宝石 * 1
		t500 = 0x0008, // 累计淘宝500次		套装碎片 * 50
		t1000 = 0x0010, // 累计淘宝1000次		强化石 * 500
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


// 宝藏系统
class treasure
{
	struct record
	{
		uint roleid;// 角色ID
		uint count;// 点击数量
		uint hurt;// 伤害值
		uint coin;// 金币数
		string name;// 名字
		byte job;// 职业
		byte sex;// 性别
		uint16 item;// 道具
		byte quality;// 品质
	};
	typedef shared_ptr<record> record_ptr;

	struct info
	{
		list<pair<uint, uint>> history;
		int rank = -1;
	};
	static const uint max_rank_count = 200;
	static unsigned min_value;// 最小宝藏排名数值
	static list<record_ptr> rank;// 宝藏排行, roleid, 点击次数，伤害数值，COIN, 名字
	static map<uint, info> records;// 记录 roleid, [count, hurt]
	//static void on_begin_timer();
	static void on_end_timer();

public:
	static bool check;

	static void treasure_done(ROLE* role, input_stream& stream);// 完成一次
	static void get_record(ROLE* role, input_stream& stream);// 获取记录
	static void get_rank(ROLE* role, input_stream& stream);// 获取排行
	static void cost(ROLE* role, input_stream& stream);// 消费
	static uint treasure_ranking(record_ptr record);// 处理排序
	static byte get_remain(ROLE* role);// 获取剩余次数
	static void load();// 加载宝藏排行
	static void update(record_ptr record);// 更新宝藏排行
	static void dump_rank(ROLE* role);
};


// 镖局系统
class bodyguard
{
	enum e_state : byte
	{
		not_start,// 未开始
		inprogress_1,// 进行中，第一次
		inprogress_2,// 进行中，第二次
		inprogress_3,// 进行中，第三次
		today_done,// 完成
		history_done,// 昨天以前有完成
	};

	// 单次运镖信息
	struct single
	{
		uint reward_item_id = 0;
		uint reward_item_count = 1;
		bool looted_item = false;// 是否被抢道具
		bool got = false;// 领取状态
		array<uint, 2> looters{};// 抢劫者ID

		uint looted_count()// 被抢次数
		{
			return int(looters[0] > 0) + int(looters[1] > 0);
		}
		uint can_loot()// 可以被劫
		{
			if (looters[0] == 0)
				return true;
			if (looters[1] == 0)
				return true;
			return false;
		}
	};
	// 个人信息
	struct info
	{
		// 历史条目
#pragma pack(push, 1)
		struct history_item
		{
			time_t time;// 时间
			byte order;// 镖次
			uint looterid;// 劫匪ID
			uint looted_itemid;// 道具ID
			history_item(byte order, uint looterid, uint looted_itemid) :order(order), looterid(looterid), looted_itemid(looted_itemid)
			{
				time = ::time(nullptr);
			}
		};
#pragma pack(pop)
		time_t start_time = 0;// 开始时间
		uint coin = 0;// 奖励金币
		uint exp = 0;// 奖励经验
		bool item_plus = false;// 道具提升
		bool hidden = false;// 隐藏，不显示在列表中
		array<single, 3> guards{};// 镖的详细信息
		int divination = -1;
		list<history_item> history;// 被劫记录
		bool data_changed = false;

		e_state state();// 获取状态
		bool add_looter(uint index, uint looterid, uint looted_itemid);// 添加劫匪
		void reset();
	};
	// 奖励配置
	struct reward_config
	{
		uint item_id;
		float probability;// 概率
		float probability_plus;// 增强概率
	};
	// 吉凶占卜配置
	struct divination_config
	{
		float coin_up;// 对获取金币的提升值
		float exp_up;// 对获取经验的提升值
		float item_up;// 对道具命中概率的提升值
		float probability;// 命中此卦的概率
	};
	// VIP劫镖次数奖励配置
	struct vip_loot_count_config
	{
		uint count = 0;
		uint ingod = 0;
	};

	static time_t round_time;// 一次运镖时间(单位：小时)
	static uint hidden_price;// 在列表隐藏的价格
	static uint item_plus_price;// 道具提升价格
	static uint divination_price;// 占卜价格
	static uint list_count;// 列表数量
	static float loot_item_probability;// 抢劫道具概率
	static uint double_start_time, double_end_time;// 双倍奖励开始、结束时间(单位：小时)
	static vector<reward_config> reward_configs;// 奖励配置
	static vector<divination_config> divination_configs;// 占卜配置
	static vector<vip_loot_count_config> vip_loot_count_configs;// VIP奖励抢劫次数配置
	static map<uint, info> records;// 记录

	static void send_reward(ROLE* role, info& reward, single& item);// 发送单次奖励

public:
	static int been_loot(ROLE* role, ROLE* looter, const byte result);// 被劫镖
	static int rpc_start(ROLE* role, input_stream& stream);// 开始保镖
	static int rpc_get_reward(ROLE* role, input_stream& stream);// 获取运镖奖励
	static int rpc_get_status(ROLE* role, input_stream& stream);// 获取运镖奖励信息
	static int rpc_buy_loot_count(ROLE* role, input_stream& stream);// 购买抢劫次数
	static int rpc_buy_divination(ROLE* role, input_stream& stream);// 卜一卦
	static int rpc_list(ROLE* role, input_stream& stream);// 列表
	static int rpc_been_loot_record(ROLE* role, input_stream& stream);// 列表

	// 遇打劫信息
	enum pre_loot_info : byte
	{
		ok,
		target_can_not_loot,// 目标不能再被打劫
		not_running,// 目标没有在运镖
	};
	static pre_loot_info get_state(uint roleid, uint targetid);// 抢劫
	static void get_info(ROLE* role, byte& state, byte& remain_receive, time_t& start_time);// 可领取次数
	static void debug_set_time(ROLE* role, time_t time = 0);// 调试功能 设置镖车时间，默认清0（未开始状态），单位小时

	static void save_data();// 保存数据
	static void load_data();// 加载数据
	static void load();// 加载配置
};


namespace ACTIVITY
{
	//	enum E_ACTIVITY_ID
	//	{
	//		WORLD_BOSS_FIGHT		 = 1,//世界boss战
	//		CHANGSHA_BOSS_FIGHT		 = 2,//沙城争夺战
	//		HANGHUI_BOSS_FIGHT		 = 3,//行会争夺战
	//		CHANGBANPO_BOSS_FIGHT	 = 4,//血战长坂坡
	//	};

	enum E_ACTIVITY_RETURN
	{
		ACTIVITY_SUCCESS = 0,//已经开始
		ACTIVITY_WAITING = 1,//报名中
		ACTIVITY_NOT_OPEN = 2, //还未开始
		BOSSFIGHT_ENHANCE_ENOUGH = 3,//鼓舞次数已满
		COLD_NOT_ENOUGH = 4,//元宝不足
		ALREADY_ENLIST = 5,//已经报名
		ACTIVITY_NO_EXSIST = 6,//不存在
		ACTIVITY_START_FIGHT = 7,//战斗
		ACTIVITY_BOSS_DIE = 8,//怪物已经死掉
		NO_FIGHT_COLD_ENOUGH = 9,//为超过30秒
	};

	//	enum E_BOSS_FIGHT_USE
	//	{
	//		E_MALE_CLOTH        = 10009,//默认男性衣服
	//		E_FMALE_CLOTH       = 10010,//默认女孩衣服
	//		FIGHT_SKILL_MONK    = 65533,//道士默认技能
	//		FIGHT_SKILL_MAGIC   = 65534,//默认
	//		FIGHT_SKILL_WARRIOR = 65535,//默认
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

