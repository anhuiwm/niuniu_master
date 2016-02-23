#ifndef __WORLDBOSS_H__
#define __WORLDBOSS_H__

#include "typedef.h"

using namespace std;

enum E_WORLDBOSS_STATUS
{
	EWBS_DEFAULT = 0, // 0Ĭ��״̬ 
	EWBS_SIGNUP = 1,// 1����״̬ 
	EWBS_SIGNUP_END = 2, //2�������� 
	EWBS_BATTLE = 3, //3ս��״̬ 
	//EWBS_BATTLE_END = 4, //4ս������
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

	// ��ȡ����BOSS״̬
	int clientGetWorldBossStatus(ROLE* role);
	// ����
	int clientSignUpWorldBoss(ROLE* role);

	// �Ƿ���
	BYTE isSignUpWorldBoss(ROLE* role);

	// ��������BOSS
	int clientEmbraveWorldBoss(ROLE* role);
	// ��������BOSS
	int clientAttackWorldBoss(ROLE* role);
	// ����˺�����
	int clientGetWorldBossRank(ROLE* role);
	// �����ȴʱ��
	int clientGetWorldBossCoolTime(ROLE* role);

	// ��ǰʱ��
	void updateTime(uint currenttime);

	// ����BOSS����
	bool calBossAttributes();

	// ������ʼʱ��
	static void SignUpStartTime();
	// ��������ʱ��
	static void SignUpEndTime();
	// ս����ʼʱ��
	static void BattleStartTime();
	// ս������ʱ��
	static void BattleEndTime();
	// ��������BOSS����
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
		BYTE byEnchance; // �Ƿ���� 0���� 1��
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

	E_WORLDBOSS_STATUS _worldboss_status; // ����BOSS״̬

	uint _bossAttr[E_ATTR_MAX];
	WORD _boss_level;

	bool _bCalBossAttrs;

	std::map<uint, stBossItem> _mapBossItems;
	std::vector<stBossItem> _vecSortItems;
	bool sorted_items_changed = false;

	static const WORD BOSS_SKILLID = 1; //�����ս

	static const uint MAX_LUCK_AWARDS = 10;

	uint _arrLuckRoleIds[MAX_LUCK_AWARDS];
	uint _dwBossCurHp; // BOSS��ǰѪ��

	string kill_boss_rolename; // ���һ����ɫ��

	void ResetBossInfo();

	bool isLuckAttack(uint total_hp, uint cur_hp, uint remain_hp, uint roleid, uint& remain_hp_per);
	void set_timer();
};

#endif		//__WORLDBOSS_H__
