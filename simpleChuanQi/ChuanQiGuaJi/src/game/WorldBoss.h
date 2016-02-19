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
	static WorldBoss& getMe()
	{
		static WorldBoss s_inst;
		return s_inst;
	}

	WorldBoss();

	void init();

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
	void updateTime(DWORD currenttime);

	// 计算BOSS属性
	bool calBossAttributes();

	// 报名开始时间
	void SignUpStartTime();
	// 报名结束时间
	void SignUpEndTime();
	// 战斗开始时间
	void BattleStartTime();
	// 战斗结束时间
	void BattleEndTime();
	// 计算世界BOSS属性
	void CalBossAttrTime();

	void onBossEnd();
	
	void startBattle(ROLE* role, BYTE bEnchance,  bool inst_battle);

	void save_to_db();
	void load_from_db();

	struct stBossItem
	{
		DWORD roleid;
		DWORD cool_time;
		DWORD hurt;
		BYTE byEnchance; // 是否鼓舞 0不是 1是
		stBossItem()
		{
			roleid = 0;
			cool_time = 0;
			hurt = 0;
			byEnchance = 0;
		}

		stBossItem(DWORD roleid)
		{
			this->roleid = roleid;
			cool_time = 0;
			hurt = 0;
			byEnchance = 0;
		}
	};

	stBossItem& getBossItem(DWORD roleid);
protected:

	E_WORLDBOSS_STATUS _worldboss_status; // 世界BOSS状态

	DWORD _bossAttr[E_ATTR_MAX];
	WORD _boss_level;

	bool _bCalBossAttrs;

	std::map<DWORD, stBossItem> _mapBossItems;
	std::vector<stBossItem> _vecSortItems;

	static const WORD BOSS_SKILLID = 1; //物理近战

	static const DWORD MAX_LUCK_AWARDS = 10;

	DWORD _arrLuckRoleIds[MAX_LUCK_AWARDS];
	DWORD _dwBossCurHp; // BOSS当前血量

	std::string _last_rolename; // 最后一击角色名

	void ResetBossInfo();

	bool isLuckAttack(DWORD total_hp, DWORD cur_hp, DWORD remain_hp, DWORD roleid, DWORD& remain_hp_per);
};

#endif		//__WORLDBOSS_H__
