#include "pch.h"
#include "log.h"
#include "base.h"
#include "protocal.h"
#include "itemCtrl.h"
#include "WorldBoss.h"
#include "role.h"
#include "roleCtrl.h"
#include "arena.h"
#include "TimerMgr.h"
#include "mailCtrl.h"
#include "fightCtrl.h"
#include "db.h"
#include "service.h"


static uint getTodayNowSecond( time_t time)//��������
{
	struct tm tmNow;
	LOCAL_TIME(time, tmNow);
	return ( tmNow.tm_hour * 60 * 60 + tmNow.tm_min * 60 + tmNow.tm_sec );
}

void WorldBoss::init()
{
	ResetBossInfo();

	_bCalBossAttrs = calBossAttributes();

	set_timer();
}

void WorldBoss::load_config()
{
	string config_file = string(getProgDir()) + STR_FSEP + "config/worldboss.xml";

	TiXmlDocument doc;
	if (!doc.LoadFile(config_file.c_str()))
	{
		cout << "can't load worldboss.xml" << endl;
		logFATAL("can't load worldboss.xml");
		return;
	}

	auto root = doc.RootElement();

	//string debug;
	//if (!tixml_value(root->FirstChild("debug"), debug))
	//{
	//	cout << "can't read debug value" << endl;
	//	logFATAL("can't read debug value");
	//	return;
	//}

	string signin_begin;
	if (!tixml_value(root->FirstChild("signin_begin"), signin_begin))
	{
		cout << "can't read signin_begin value" << endl;
		logFATAL("can't read signin_begin value");
		return;
	}

	string signin_end;
	if (!tixml_value(root->FirstChild("signin_end"), signin_end))
	{
		cout << "can't read signin_begin value" << endl;
		logFATAL("can't read signin_begin value");
		return;
	}

	string battle_begin;
	if (!tixml_value(root->FirstChild("battle_begin"), battle_begin))
	{
		cout << "can't read signin_begin value" << endl;
		logFATAL("can't read signin_begin value");
		return;
	}

	string battle_end;
	if (!tixml_value(root->FirstChild("battle_end"), battle_end))
	{
		cout << "can't read signin_begin value" << endl;
		logFATAL("can't read signin_begin value");
		return;
	}

	//if (stricmp(debug.c_str(), "true") == 0)
	//{
	//	is_debug = true;
	//}

	vector<string> timestr;
	BASE::strtoken2Vec(signin_begin.c_str(), ":", timestr);
	timeconfig[0][0] = atol(timestr[0].c_str());
	timeconfig[0][1] = atol(timestr[1].c_str());
	timestr.clear();
	BASE::strtoken2Vec(signin_end.c_str(), ":", timestr);
	timeconfig[1][0] = atol(timestr[0].c_str());
	timeconfig[1][1] = atol(timestr[1].c_str());

	timestr.clear();
	BASE::strtoken2Vec(battle_begin.c_str(), ":", timestr);
	timeconfig[2][0] = atol(timestr[0].c_str());
	timeconfig[2][1] = atol(timestr[1].c_str());
	timestr.clear();
	BASE::strtoken2Vec(battle_end.c_str(), ":", timestr);
	timeconfig[3][0] = atol(timestr[0].c_str());
	timeconfig[3][1] = atol(timestr[1].c_str());
}


void WorldBoss::set_timer()
{
	auto now = time(0) - service::tzone;
	auto escape = now % (24 * 3600);// �����ȥ��ʱ��
/*
	if (is_debug)
	{
		if (_worldboss_status == EWBS_DEFAULT)
		{
			service::set_timer(120, SignUpStartTime);
		}
		if (_worldboss_status == EWBS_SIGNUP)
		{
			service::set_timer(120, SignUpEndTime);
		}
		if (_worldboss_status == EWBS_SIGNUP_END)
		{
			service::set_timer(120, BattleStartTime);
		}
		if (_worldboss_status == EWBS_BATTLE)
		{
			service::set_timer(120, BattleEndTime);
		}
	}
	else*/
	{
		if (escape >= timeconfig[0][0] * 3600 + timeconfig[0][1] * 60)
		{
			if (escape >= timeconfig[1][0] * 3600 + timeconfig[1][1] * 60)
			{
				if (escape >= timeconfig[2][0] * 3600 + timeconfig[2][1] * 60)
				{
					if (escape >= timeconfig[3][0] * 3600 + timeconfig[3][1] * 60)
					{
						_worldboss_status = EWBS_DEFAULT;
						service::set_timer((24 * 3600 - escape) + (timeconfig[0][0] * 3600 + timeconfig[0][1] * 60), SignUpStartTime);// 24Сʱ��ȥ�����ȥ��ʱ��+12Сʱ=����12��
					}
					else
					{
						_worldboss_status = EWBS_BATTLE;
						service::set_timer(timeconfig[3][0] * 3600 + timeconfig[3][1] * 60 - escape, BattleEndTime);
					}
				}
				else
				{
					_worldboss_status = EWBS_SIGNUP_END;
					service::set_timer(timeconfig[2][0] * 3600 + timeconfig[2][1] * 60 - escape, BattleStartTime);
				}
			}
			else
			{
				_worldboss_status = EWBS_SIGNUP;
				service::set_timer(timeconfig[1][0] * 3600 + timeconfig[1][1] * 60 - escape, SignUpEndTime);
			}
		}
		else
		{
			_worldboss_status = EWBS_DEFAULT;
			service::set_timer(timeconfig[0][0] * 3600 + timeconfig[0][1] * 60 - escape, SignUpStartTime);
		}
	}
	//// ������ʼʱ��12��
	//TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::SignUpStartTime,this), 12*3600);
	//// ��������ʱ��20��
	//TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::SignUpEndTime,this), 20*3600);
	//// ս����ʼʱ��20��10��
	//TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::BattleStartTime,this), 20*3600 + 10 * 60);
	//// ս������ʱ��20��40��
	//TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::BattleEndTime,this), 20*3600 + 40 * 60);
	//// ��������BOSS����ʱ��8��
	//TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::CalBossAttrTime,this), 20*3600);
}
void WorldBoss::set_status(int status)
{
	if (status >= E_WORLDBOSS_STATUS::EWBS_DEFAULT && status <= E_WORLDBOSS_STATUS::EWBS_BATTLE)
		_worldboss_status = (E_WORLDBOSS_STATUS)status;
}

// ����BOSS����
bool WorldBoss::calBossAttributes()
{
	uint role_count = 0;
	uint boss_level = 0;
	std::set<uint> setRoleIds = ARENA::getArenaTop10RoleIds();
	for (auto itr=setRoleIds.begin(); itr!=setRoleIds.end(); itr++)
	{
		uint roleid = *itr;
		ROLE* role = RoleMgr::getMe().getRoleByID( roleid );
		if (NULL != role)
		{
			role_count++;

			boss_level += role->wLevel;

			_bossAttr[E_ATTR_HP_MAX2] += role->roleAttr[E_ATTR_HP_MAX2];//�������ֵ
			_bossAttr[E_ATTR_ATK_MAX2] += role->roleAttr[E_ATTR_ATK_MAX2];//�������ֵ
			_bossAttr[E_ATTR_ATK_MIN2] += role->roleAttr[E_ATTR_ATK_MIN2];//������Сֵ
			_bossAttr[E_ATTR_SPELLATK_MAX] += role->roleAttr[E_ATTR_SPELLATK_MAX];//���ܹ������ֵ
			_bossAttr[E_ATTR_SPELLATK_MIN] += role->roleAttr[E_ATTR_SPELLATK_MIN];//���ܹ�����Сֵ
			//E_ATTR_SOL_MAX = 9,//������������
			//E_ATTR_SOL_MIN = 10,//������������
			_bossAttr[E_ATTR_DEF_MAX] += role->roleAttr[E_ATTR_DEF_MAX];//�������ֵ
			_bossAttr[E_ATTR_DEF_MIN] += role->roleAttr[E_ATTR_DEF_MIN];//������Сֵ
			_bossAttr[E_ATTR_SPELLDEF_MAX] += role->roleAttr[E_ATTR_SPELLDEF_MAX];//���ܷ������ֵ
			_bossAttr[E_ATTR_SPELLDEF_MIN] += role->roleAttr[E_ATTR_SPELLDEF_MIN];//���ܷ�����Сֵ
			_bossAttr[E_ATTR_DODGE] += role->roleAttr[E_ATTR_DODGE];//����ֵ
			//E_ATTR_MAG_DODGE = 16,//ħ������
			//E_ATTR_SOL_DODGE = 17,//��������
			//E_ATTR_SPEED = 18,//�����ٶ�
			//E_ATTR_MP_REC2		 =	19,//ÿ�غ�ħ���ָ�ֵ
			//E_ATTR_HP_REC2		 =	20,//ÿ�غ������ָ�ֵ
			_bossAttr[E_ATTR_HIT] += role->roleAttr[E_ATTR_HIT];//����ֵ
			//E_ATTR_STAMINA	     =  22,//����
			//E_ATTR_STRENGTH	     =  23,//����
			//E_ATTR_AGILITY	     =  24,//����
			//E_ATTR_INTELLECT     =  25,//�ǻ�
			//E_ATTR_SPIRIT		 =  26,//����
			_bossAttr[E_ATTR_CRIT] += role->roleAttr[E_ATTR_CRIT];//����ֵ
			_bossAttr[E_ATTR_CRIT_RES] += role->roleAttr[E_ATTR_CRIT_RES];//�����ֿ�ֵ
			//E_ATTR_CRIT_INC	     =	29,//�����˺�����ֵ
			//E_ATTR_CRIT_RED	     =	30,//�����˺�����ֵ
			//E_ATTR_ATT_HEM		 =	31,//������Ѫ
			//E_ATTR_ATT_MANA	     =	32,//��������
			//E_ATTR_EQU_DROP	     =	33,//װ������
			//E_ATTR_MONEY_DROP	 =	34,//��ҵ���
			//E_ATTR_LUCK		     =	35,//����
			//E_LUCK_DEF			 =  36,//���˷���
			//E_ATTR_MAX = 37,//�����������
		}
	}

	if (role_count > 0)
	{
		for (uint i=0; i<E_ATTR_MAX; i++)
		{
			_bossAttr[i] = (uint)((float)_bossAttr[i]/(float)role_count);
		}
		_boss_level = (WORD)boss_level / 10;
		_bossAttr[E_ATTR_HP_MAX2] = _bossAttr[E_ATTR_HP_MAX2] * 500 * 30;

		if ( _bossAttr[E_ATTR_HP_MAX2] > VALUE_MAX )
		{
			_bossAttr[E_ATTR_HP_MAX2] = VALUE_MAX;
		}

		_dwBossCurHp = _bossAttr[E_ATTR_HP_MAX2]; // �������ֵ
		return true;
	}

	return false;
}

// ��ȡ����BOSS״̬
int WorldBoss::clientGetWorldBossStatus(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	string strData;

	S_APPEND_BYTE(strData, _worldboss_status);
	stBossItem& boss_item = getBossItem(role->dwRoleID);
	S_APPEND_BYTE(strData, boss_item.byEnchance);

	if (boss_item.roleid == 0)
	{
		BYTE signup = 0;
		S_APPEND_BYTE(strData, signup);
	}
	else
	{
		BYTE signup = 1;
		S_APPEND_BYTE(strData, signup);
	}
	
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_WORLDBOSS_STATUS, strData) );

	return 0;
}

int WorldBoss::clientSignUpWorldBoss(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	if (_worldboss_status == EWBS_DEFAULT)
	{
		string strData;
		BYTE byResult = 2;
		S_APPEND_BYTE(strData, byResult);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SIGNUP_WORLDBOSS, strData) );
		return 0;
	}

	string strData;
	BYTE byResult = 0;
	S_APPEND_BYTE(strData, byResult);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SIGNUP_WORLDBOSS, strData) );

	auto itr=_mapBossItems.find(role->dwRoleID);
	if (itr == _mapBossItems.end())
	{
		stBossItem item(role->dwRoleID);
		_mapBossItems.insert(std::make_pair(role->dwRoleID, item));
	}

	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_WORLD_BOSS);
	return 0;
}

BYTE WorldBoss::isSignUpWorldBoss(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}
	auto itr=_mapBossItems.find(role->dwRoleID);
	if (itr == _mapBossItems.end())
	{
		return 0;
	}

	return 1;
}
// ��������BOSS
int WorldBoss::clientEmbraveWorldBoss(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	stBossItem& item = getBossItem(role->dwRoleID);
	if (item.byEnchance == 1)
	{
		return 0;
	}

	const uint need_gold = 200; // ��������200Ԫ��
	if (!role->hasEnoughGold(need_gold))
	{
		string strData;
		BYTE byResult = 2;
		S_APPEND_BYTE(strData, byResult);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EMBRAVE_WORLDBOSS, strData) );
		return 0;
	}

	role->subGold(need_gold);
	item.byEnchance++;

	// ����ɹ�
	string strData;
	BYTE byResult = 0;
	S_APPEND_BYTE(strData, byResult);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EMBRAVE_WORLDBOSS, strData) );
	return 0;
}

// ��������BOSS
int WorldBoss::clientAttackWorldBoss(ROLE* role)
{
	enum result : byte
	{
		successful,// �ɹ�
		not_signin,// δע��
		not_enough_gold,// Ԫ������
		not_battle_state,// ��ս��״̬
	};

	if (nullptr == role)
	{
		return 0;
	}

	if (_mapBossItems.find(role->dwRoleID) == _mapBossItems.end())
	{
		output_stream stream(S_ATTACK_WORLDBOSS);
		stream.write_byte(result::not_signin);
		role->send(stream);
		return 0;
	}

	if (EWBS_BATTLE != _worldboss_status)
	{
		output_stream stream(S_ATTACK_WORLDBOSS);
		stream.write_byte(result::not_battle_state);
		role->send(stream);
		return 0;
	}

	const uint need_gold = 20; // ����20Ԫ��
	if (!role->hasEnoughGold(need_gold))
	{
		output_stream stream(S_ATTACK_WORLDBOSS);
		stream.write_byte(result::not_enough_gold);
		role->send(stream);
		return 0;
	}
	role->subGold(need_gold);

	stBossItem& boss_item = getBossItem(role->dwRoleID);
	startBattle(role, boss_item.byEnchance, true);

	if (_dwBossCurHp == 0)
	{
		kill_boss_rolename = role->roleName;
		onBossEnd();
	}

	return 0;
}

WorldBoss::stBossItem& WorldBoss::getBossItem(uint roleid)
{
	static stBossItem s_bossitem;

	auto itr=_mapBossItems.find(roleid);
	if (itr == _mapBossItems.end())
	{
		return s_bossitem;
	}

	return itr->second;
}

bool WorldBoss::isLuckAttack(uint total_hp, uint cur_hp, uint remain_hp, uint roleid, uint& remain_hp_per)
{
	float fCur = (float)cur_hp / (float)total_hp * 10.0f;
	//uint dwCur = (uint)fCur;
	float fRemain = (float)remain_hp / (float)total_hp * 10.0f;
	//uint dwRemain = (uint)fRemain;

	bool bResult = false;
	for (uint i=9; i<=0; i--)
	{
		if (_arrLuckRoleIds[i] != 0)
		{
			continue;
		}

		if (fCur>=i && fRemain<i)
		{
			_arrLuckRoleIds[i] = roleid;
			bResult = true;
			remain_hp_per = i*10;
		}
	}

	if (remain_hp == 0)
	{
		_arrLuckRoleIds[0] = roleid;
		remain_hp_per = 0;
		bResult = true;
	}

	return bResult;
}

static bool sortHurt(const WorldBoss::stBossItem& left, const WorldBoss::stBossItem& right)
{
	return left.hurt > right.hurt;
}

// ��ǰʱ��
void WorldBoss::updateTime(uint currenttime)
{
	if (EWBS_BATTLE != _worldboss_status)
	{
		return;
	}

	for (auto itr=_mapBossItems.begin(); itr!=_mapBossItems.end(); itr++)
	{
		stBossItem& item = itr->second;
		ROLE* role = RoleMgr::getMe().getRoleByID( item.roleid );
		if (NULL == role)
		{
			continue;
		}

		if (currenttime >= item.cool_time)
		{
			stBossItem& boss_item = getBossItem(role->dwRoleID);
			startBattle(role, boss_item.byEnchance, false);
			item.cool_time = currenttime + 30;
		}
		if (_dwBossCurHp == 0)
		{
			kill_boss_rolename = role->roleName;
			break;
		}
	}

	if (EWBS_BATTLE == _worldboss_status)
	{
		std::vector<stBossItem> vecItems;
		for (auto itr=_mapBossItems.begin(); itr!=_mapBossItems.end(); itr++)
		{
			vecItems.push_back(itr->second);
		}
		std::sort(vecItems.begin(),vecItems.end(), sortHurt);
		_vecSortItems = vecItems;
		sorted_items_changed = true;
	}

	if (_dwBossCurHp == 0)
	{
		onBossEnd();
	}
}

void WorldBoss::startBattle(ROLE* role, BYTE byEnchance,  bool inst_battle)
{
	if (NULL == role)
	{
		return;
	}

	float fAdd = 1.0f;
	if (1 == byEnchance)
	{
		fAdd += 0.05f;
	}
	if (inst_battle)
	{
		fAdd += 0.3f;
	}

	//uint tmpAttr[E_ATTR_MAX];
	//memcpy(tmpAttr,roleAttr,sizeof(roleAttr));
	//tmpAttr[E_ATTR_HP_MAX2] = (uint)((float)tmpAttr[E_ATTR_HP_MAX2] * fAdd);//�������ֵ
	//tmpAttr[E_ATTR_ATK_MAX2] = (uint)((float)tmpAttr[E_ATTR_ATK_MAX2] * fAdd);//�������ֵ
	//tmpAttr[E_ATTR_ATK_MIN2] = (uint)((float)tmpAttr[E_ATTR_ATK_MIN2] * fAdd);//������Сֵ

	bool bChance = byEnchance == 1 ? true : false;

	uint remain_hp = 0;
	FightCtrl::fightWorldBoss(role, _bossAttr, bChance, inst_battle,  _boss_level, BOSS_SKILLID, _dwBossCurHp, remain_hp);
	//remain_hp = _dwBossCurHp - 1000;
	uint remain_hp_per = 0;
	BYTE byLuck = 0;
	if (isLuckAttack(_bossAttr[E_ATTR_HP_MAX2],_dwBossCurHp, remain_hp, role->dwRoleID, remain_hp_per))
	{
		byLuck = 1;
	}
	uint hurt = 0;
	if (_dwBossCurHp >= remain_hp)
	{
		hurt = _dwBossCurHp - remain_hp;
		_dwBossCurHp = remain_hp;
	}

	stBossItem& boss_item =  getBossItem(role->dwRoleID);
	boss_item.hurt += hurt;

	string strData;
	BYTE byError = 0;
	S_APPEND_BYTE(strData, byError);
	S_APPEND_DWORD(strData, hurt);
	S_APPEND_BYTE(strData, byLuck);
	S_APPEND_DWORD(strData, remain_hp_per);

	uint rank_count = 0;
	for (size_t i=0; i < _vecSortItems.size(); i++)
	{
		const stBossItem& item = _vecSortItems[i];
		ROLE* role = RoleMgr::getMe().getRoleByID( item.roleid );
		if (NULL == role)continue;

		S_APPEND_NBYTES( strData, role->roleName.c_str(), 22 );
		rank_count++;
		if (rank_count>=3)break;
	}

	if (rank_count < 3)
	{
		for (uint i=rank_count; i<3; i++)
		{
			std::string name;
			S_APPEND_NBYTES( strData, name.c_str(), 22 );
		}
	}

	role->role_worldboss.attack_index++;
	S_APPEND_DWORD(strData, role->role_worldboss.attack_index);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ATTACK_WORLDBOSS, strData) );
}

WorldBoss::WorldBoss()
{
	_worldboss_status = EWBS_DEFAULT;
	memset( _bossAttr, 0, sizeof(_bossAttr) );
	_bCalBossAttrs = false;
	uint _arrLuckRoleIds[MAX_LUCK_AWARDS];
	memset(_arrLuckRoleIds,0, sizeof(_arrLuckRoleIds));
}

// ������ʼʱ��
void WorldBoss::SignUpStartTime()
{
	logINFO("world boss enter signup stage.");
	WorldBoss::getMe().ResetBossInfo();
	WorldBoss::getMe()._worldboss_status = EWBS_SIGNUP; // ����ʱ�����
	WorldBoss::getMe().set_timer();
}

// ��������ʱ��
void WorldBoss::SignUpEndTime()
{
	logINFO("world boss enter signup end stage.");
	WorldBoss::getMe().CalBossAttrTime();
	WorldBoss::getMe()._worldboss_status = EWBS_SIGNUP_END; // ����ʱ�����
	WorldBoss::getMe().set_timer();
}

// ս����ʼʱ��
void WorldBoss::BattleStartTime()
{
	logINFO("world boss enter battle start stage.");
	WorldBoss::getMe()._worldboss_status = EWBS_BATTLE; // ����ʱ�����
	WorldBoss::getMe()._vecSortItems.clear();
	WorldBoss::getMe().save_to_db();
	WorldBoss::getMe().set_timer();
}

// ս������ʱ��
void WorldBoss::BattleEndTime()
{
	logINFO("world boss battle end. enter default stage.");
	WorldBoss::getMe()._worldboss_status = EWBS_DEFAULT;
	WorldBoss::getMe().onBossEnd();
	WorldBoss::getMe().set_timer();
}

// ��������BOSS����
void WorldBoss::CalBossAttrTime()
{
	_bCalBossAttrs = calBossAttributes();
}

/*
void resetSignup(RolePtr pRole)
{
	if (NULL == pRole)
	{
		return;
	}

	pRole->role_worldboss.resetSignup();
}
*/

/*
��������		
ÿ����������	���	Ӷ�������飨Ӷ�����ǣ�
1	2000000	4��������*3
2	1800000	4��������*2
3	1600000	4��������*1
4	1500000	3��������*2
5	1400000	3��������*2
6-10	1300000	3��������*2
11-20	1200000	3��������*1
21-50	1100000	3��������*1
51-100	1000000	3��������*1
101-200	900000	2��������*2
201-500	800000	2��������*1
501-1000	700000	1��������*2
1001-2000	600000	1��������*2
2001-5000	500000	1��������*1
5000+	400000	1��������*1

901
902
903
904
905
*/

static void getHurtRankAwards(uint rank, uint& gold_count, WORD& itemid, uint& itemcount)
{
	if (1 == rank)
	{
		gold_count = 2000000;
		itemid = 904;
		itemcount = 3;
	}
	else if (2 == rank)
	{
		gold_count = 1800000;
		itemid = 904;
		itemcount = 2;
	}
	else if (3 == rank)
	{
		gold_count = 1600000;
		itemid = 903;
		itemcount = 3;
	}
	else if (4 == rank)
	{
		gold_count = 1500000;
		itemid = 903;
		itemcount = 2;
	}
	else if (5 == rank)
	{
		gold_count = 1400000;
		itemid = 903;
		itemcount = 2;
	}
	else if (rank>=6 && rank<=10)
	{
		gold_count = 1300000;
		itemid = 903;
		itemcount = 1;
	}
	else if (rank>=11 && rank<=20)
	{
		gold_count = 1200000;
		itemid = 903;
		itemcount = 1;
	}
	else if (rank>=21 && rank<=50)
	{
		gold_count = 1100000;
		itemid = 903;
		itemcount = 1;
	}
	else if (rank>=51 && rank<=100)
	{
		gold_count = 1000000;
		itemid = 903;
		itemcount = 1;
	}
	else if (rank>=101 && rank<=200)
	{
		gold_count = 900000;
		itemid = 902;
		itemcount = 2;
	}
	else if (rank>=201 && rank<=500)
	{
		gold_count = 800000;
		itemid = 902;
		itemcount = 1;
	}
	else if (rank>=501 && rank<=1000)
	{
		gold_count = 700000;
		itemid = 901;
		itemcount = 2;
	}
	else if (rank>=1001 && rank<=2000)
	{
		gold_count = 600000;
		itemid = 901;
		itemcount = 2;
	}
	else if (rank>=2001 && rank<=5000)
	{
		gold_count = 500000;
		itemid = 901;
		itemcount = 2;
	}
	else
	{
		gold_count = 400000;
		itemid = 901;
		itemcount = 2;
	}
}

/*
ʣ��Ѫ��	��������	����
90%	���˽���	��װ��Ƭ*2
80%	���˽���	��װ��Ƭ*2
70%	���˽���	��װ��Ƭ*2
60%	���˽���	��װ��Ƭ*2
50%	���˽���	��װ��Ƭ*2
40%	���˽���	��װ��Ƭ*2
30%	���˽���	��װ��Ƭ*2
20%	���˽���	��װ��Ƭ*2
10%	���˽���	��װ��Ƭ*2
0%	���һ��	��װ��Ƭ*2
*/



void WorldBoss::onBossEnd()
{
	output_stream stream(S_ATTACK_WORLDBOSS);
	stream.write_byte(1);
	for (auto itr=_mapBossItems.begin(); itr!=_mapBossItems.end(); itr++)
	{
		auto role = RoleMgr::getMe().getRoleByID(itr->second.roleid);
		if (nullptr == role)continue;
		role->role_worldboss.attack_index = 0;
		role->send(stream);
	}

	//RoleMgr::getMe().ExecAllRoles(std::bind(resetSignup,std::placeholders::_1));
	// BOSS�˺���������
	std::vector<stBossItem> vecItems;
	for (auto itr=_mapBossItems.begin(); itr!=_mapBossItems.end(); itr++)
	{
		vecItems.push_back(itr->second);
	}
	std::sort(vecItems.begin(),vecItems.end(), sortHurt);
	for (uint rank=1; rank<=vecItems.size(); rank++)
	{
		//uint rank = 1;
		char szMsg[256];
		memset(szMsg, 0, sizeof(szMsg));

		uint gold_count = 0;
		uint hurt = vecItems[rank-1].hurt;
		WORD itemid = 0;
		uint itemcount = 0;
		getHurtRankAwards(rank, gold_count, itemid, itemcount);

		std::string item_name;
		ITEM_INFO iteminfo;
		iteminfo.itemIndex = itemid;
		iteminfo.itemNums = itemcount;
		std::vector<ITEM_INFO> vecItemInfos;
		vecItemInfos.push_back(iteminfo);

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(itemid);
		if ( itemCfg != NULL )
		{	
			item_name = itemCfg->name;
		}

		std::string strMsg = CONFIG:: getMsgBossHurtRank();
		sprintf( szMsg,strMsg.c_str(), hurt,rank, gold_count,item_name.c_str(), itemcount);
		MailCtrl::sendMail(vecItems[rank-1].roleid, E_MAIL_TYPE_PRIZE, szMsg, vecItemInfos);
	}

	_vecSortItems = vecItems;
	sorted_items_changed = true;

	const uint SUIT_PATCH_ID = 403;

	// BOSS���˽���
	for (uint i=0; i<MAX_LUCK_AWARDS; i++)
	{
		uint roleid = _arrLuckRoleIds[i];
		if (0 == roleid)
		{
			continue;
		}

		char szMsg[256];
		memset(szMsg, 0, sizeof(szMsg));
		uint itemcount = 0;
		std::string item_name;
		ITEM_INFO iteminfo;
		iteminfo.itemIndex = SUIT_PATCH_ID;
		iteminfo.itemNums = itemcount;
		std::vector<ITEM_INFO> vecItemInfos;
		vecItemInfos.push_back(iteminfo);

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(SUIT_PATCH_ID);
		if ( itemCfg != NULL )
		{	
			item_name = itemCfg->name;
		}

		sprintf( szMsg,(CONFIG:: getMsgBossLastHurt()).c_str(), item_name.c_str(), itemcount);
		MailCtrl::sendMail(roleid, E_MAIL_TYPE_PRIZE, szMsg, vecItemInfos);
	}

	ResetBossInfo();
}

void WorldBoss::ResetBossInfo()
{
	_worldboss_status = EWBS_DEFAULT; // ����BOSS״̬

	memset(_bossAttr,0,sizeof(_bossAttr));
	_boss_level = 1;

	_bCalBossAttrs = false;
	
	_mapBossItems.clear();

	memset(_arrLuckRoleIds,0,sizeof(_arrLuckRoleIds));

	_dwBossCurHp = 0; // BOSS��ǰѪ��

	kill_boss_rolename.clear();
}

// ����˺�����
int WorldBoss::clientGetWorldBossRank(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	string strData;
	S_APPEND_NBYTES(strData, kill_boss_rolename.c_str(), 22);

	bool found = false;
	for (uint rank=1; rank<=_vecSortItems.size(); rank++)
	{
		const stBossItem& item = _vecSortItems[rank-1];
		if(item.roleid != role->dwRoleID)
			continue;

		ROLE* role = RoleMgr::getMe().getRoleByID( item.roleid );
		if (NULL == role)
		{
			continue;
		}

		S_APPEND_DWORD(strData,rank);
		S_APPEND_NBYTES( strData, role->roleName.c_str(), 22 );
		S_APPEND_DWORD(strData, item.hurt);
		found = true;
		break;
	}
	if (!found)
	{
		S_APPEND_DWORD(strData,0);
		S_APPEND_NBYTES( strData, "", 22 );
		S_APPEND_DWORD(strData, 0);
	}
	
	for (uint rank=1; rank<=_vecSortItems.size(); rank++)
	{
		const stBossItem& item = _vecSortItems[rank-1];
		ROLE* role = RoleMgr::getMe().getRoleByID( item.roleid );
		if (NULL == role)
		{
			continue;
		}

		S_APPEND_DWORD(strData,rank);
		S_APPEND_NBYTES( strData, role->roleName.c_str(), 22 );
		S_APPEND_DWORD(strData, item.hurt);

		if (rank>=10)
		{
			break;
		}
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_WORLDBOSS_RANK, strData) );

	return 0;
}

// �����ȴʱ��
int WorldBoss::clientGetWorldBossCoolTime(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	string strData;
	stBossItem& boss_item = getBossItem(role->dwRoleID);
	BYTE remain_second = 0;
	if (boss_item.cool_time > (uint)time(nullptr))
	{
		remain_second = (BYTE)(boss_item.cool_time - time(nullptr));
	}
	
	S_APPEND_BYTE(strData, remain_second);

	uint rank_count = 0;
	for (uint rank=1; rank<=_vecSortItems.size(); rank++)
	{
		const stBossItem& item = _vecSortItems[rank-1];
		if(item.roleid != role->dwRoleID)
			continue;

		ROLE* role = RoleMgr::getMe().getRoleByID( item.roleid );
		if (NULL == role)
		{
			continue;
		}

		S_APPEND_NBYTES( strData, role->roleName.c_str(), 22 );
		rank_count++;
		if (rank_count>=3)
		{
			break;
		}
	}

	if (rank_count < 3)
	{
		for (uint i=rank_count; i<3; i++)
		{
			std::string name;
			S_APPEND_NBYTES( strData, name.c_str(), 22 );
		}
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_WORLDBOSS_COOLTIME, strData) );

	return 0;
}


void WorldBoss::save_to_db()
{
	if (!sorted_items_changed)
		return;

	execSQL( DBCtrl::getSQL(), "truncate table worldboss_rank");

	for(auto & item : _vecSortItems)
	{
		execSQL(DBCtrl::getSQL(), "insert into worldboss_rank values(%u, %u, %u, %u)",
			item.roleid, item.cool_time, item.hurt, item.byEnchance);
	}
	sorted_items_changed = false;
}


void WorldBoss::load_from_db()
{
	SQLSelect sqlx( DBCtrl::getSQL(), "select * from worldboss_rank");

	while( MYSQL_ROW row = sqlx.getRow() )
	{
		stBossItem item;
		item.roleid = atoi(row[0]);
		item.cool_time = atoi(row[1]);
		item.hurt = atoi(row[2]);
		item.byEnchance = atoi(row[3]);
		_vecSortItems.push_back(item);
	}
}