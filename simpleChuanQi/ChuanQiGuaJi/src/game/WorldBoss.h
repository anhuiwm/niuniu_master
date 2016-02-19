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
	static WorldBoss& getMe()
	{
		static WorldBoss s_inst;
		return s_inst;
	}

	WorldBoss();

	void init();

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
	void updateTime(DWORD currenttime);

	// ����BOSS����
	bool calBossAttributes();

	// ������ʼʱ��
	void SignUpStartTime();
	// ��������ʱ��
	void SignUpEndTime();
	// ս����ʼʱ��
	void BattleStartTime();
	// ս������ʱ��
	void BattleEndTime();
	// ��������BOSS����
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
		BYTE byEnchance; // �Ƿ���� 0���� 1��
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

	E_WORLDBOSS_STATUS _worldboss_status; // ����BOSS״̬

	DWORD _bossAttr[E_ATTR_MAX];
	WORD _boss_level;

	bool _bCalBossAttrs;

	std::map<DWORD, stBossItem> _mapBossItems;
	std::vector<stBossItem> _vecSortItems;

	static const WORD BOSS_SKILLID = 1; //�����ս

	static const DWORD MAX_LUCK_AWARDS = 10;

	DWORD _arrLuckRoleIds[MAX_LUCK_AWARDS];
	DWORD _dwBossCurHp; // BOSS��ǰѪ��

	std::string _last_rolename; // ���һ����ɫ��

	void ResetBossInfo();

	bool isLuckAttack(DWORD total_hp, DWORD cur_hp, DWORD remain_hp, DWORD roleid, DWORD& remain_hp_per);
};

#endif		//__WORLDBOSS_H__
