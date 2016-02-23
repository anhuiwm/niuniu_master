#ifndef __WORLDBOSS_H__
#define __WORLDBOSS_H__

#include "typedef.h"

using namespace std;

enum E_WORLDBOSS_STATUS
{
	EWBS_DEFAULT = 0, // 0默认状态 
	EWBS_SIGNUP = 1,// 1报名状态 
	EWBS_SIGNUP_END = 2, //2报名结束 
	EWBS_BATTLE = 3, //3战斗状态 
	//EWBS_BATTLE_END = 4, //4战斗结束
};

class WorldBoss
{
public:
	array<array<long, 2>, 4> timeconfig{};
	static WorldBoss& getMe()
	{
		static WorldBoss s_inst;
		return s_inst;
	}

	WorldBoss();

	void init();
	void load_config();
	void set_status(int status);

	// 获取世界BOSS状态
	int clientGetWorldBossStatus(ROLE* role);
	// 报名
	int clientSignUpWorldBoss(ROLE* role);

	// 是否报名
	BYTE isSignUpWorldBoss(ROLE* role);

	// 鼓舞世界BOSS
	int clientEmbraveWorldBoss(ROLE* role);
	// 攻击世界BOSS
	int clientAttackWorldBoss(ROLE* role);
	// 获得伤害排行
	int clientGetWorldBossRank(ROLE* role);
	// 获得冷却时间
	int clientGetWorldBossCoolTime(ROLE* role);

	// 当前时间
	void updateTime(uint currenttime);

	// 计算BOSS属性
	bool calBossAttributes();

	// 报名开始时间
	static void SignUpStartTime();
	// 报名结束时间
	static void SignUpEndTime();
	// 战斗开始时间
	static void BattleStartTime();
	// 战斗结束时间
	static void BattleEndTime();
	// 计算世界BOSS属性
	void CalBossAttrTime();

	void onBossEnd();
	
	void startBattle(ROLE* role, BYTE bEnchance,  bool inst_battle);

	void save_to_db();
	void load_from_db();

	struct stBossItem
	{
		uint roleid;
		uint cool_time;
		uint hurt;
		BYTE byEnchance; // 是否鼓舞 0不是 1是
		stBossItem()
		{
			roleid = 0;
			cool_time = 0;
			hurt = 0;
			byEnchance = 0;
		}

		stBossItem(uint roleid)
		{
			this->roleid = roleid;
			cool_time = 0;
			hurt = 0;
			byEnchance = 0;
		}
	};

	stBossItem& getBossItem(uint roleid);
protected:

	E_WORLDBOSS_STATUS _worldboss_status; // 世界BOSS状态

	uint _bossAttr[E_ATTR_MAX];
	WORD _boss_level;

	bool _bCalBossAttrs;

	std::map<uint, stBossItem> _mapBossItems;
	std::vector<stBossItem> _vecSortItems;
	bool sorted_items_changed = false;

	static const WORD BOSS_SKILLID = 1; //物理近战

	static const uint MAX_LUCK_AWARDS = 10;

	uint _arrLuckRoleIds[MAX_LUCK_AWARDS];
	uint _dwBossCurHp; // BOSS当前血量

	string kill_boss_rolename; // 最后一击角色名

	void ResetBossInfo();

	bool isLuckAttack(uint total_hp, uint cur_hp, uint remain_hp, uint roleid, uint& remain_hp_per);
	void set_timer();
};

#endif		//__WORLDBOSS_H__
