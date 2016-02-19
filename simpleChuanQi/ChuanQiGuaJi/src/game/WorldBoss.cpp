#include "log.h"

#include "base.h"
#include "protocal.h"

#include <algorithm>
#include "itemCtrl.h"
#include "WorldBoss.h"
#include "role.h"
#include "roleCtrl.h"
#include "arena.h"
#include "TimerMgr.h"
#include "mailCtrl.h"
#include "fightCtrl.h"
#include "output_stream.h"
#include "db.h"

static DWORD getTodayNowSecond( time_t time)//今天秒数
{
	struct tm tmNow;
	LOCAL_TIME(time, tmNow);
	DWORD nowSecond = tmNow.tm_hour * 60 * 60 + tmNow.tm_min * 60 + tmNow.tm_sec;
	return nowSecond;
}

void WorldBoss::init()
{
	ResetBossInfo();

	_bCalBossAttrs = calBossAttributes();

	time_t second = getTodayNowSecond(PROTOCAL::getCurTime());
	if (second <= 12*3600)
	{
		_worldboss_status = EWBS_DEFAULT;
	}
	else if (second <= 20 * 3600)
	{
		_worldboss_status = EWBS_SIGNUP;
	}
	else if (second <= (20*3600+10*60))
	{
		_worldboss_status = EWBS_SIGNUP_END;
	}
	else if (second <= (20*3600+40*60))
	{
		_worldboss_status = EWBS_BATTLE;
	}
	else
	{
		_worldboss_status = EWBS_DEFAULT;
	}

	// 报名开始时间12点
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::SignUpStartTime,this), 12*3600);
	// 报名结束时间20点
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::SignUpEndTime,this), 20*3600);
	// 战斗开始时间20点10分
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::BattleStartTime,this), 20*3600 + 10 * 60);
	// 战斗结束时间20点40分
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::BattleEndTime,this), 20*3600 + 40 * 60);
	// 计算世界BOSS属性时间8点
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&WorldBoss::CalBossAttrTime,this), 20*3600);
}

// 计算BOSS属性
bool WorldBoss::calBossAttributes()
{
	DWORD role_count = 0;
	DWORD boss_level = 0;
	std::set<DWORD> setRoleIds = ARENA::getArenaTop10RoleIds();
	for (auto itr=setRoleIds.begin(); itr!=setRoleIds.end(); itr++)
	{
		DWORD roleid = *itr;
		ROLE* role = RoleMgr::getMe().getRoleByID( roleid );
		if (NULL != role)
		{
			role_count++;

			boss_level += role->wLevel;

			_bossAttr[E_ATTR_HP_MAX2] += role->roleAttr[E_ATTR_HP_MAX2];//最大生命值
			_bossAttr[E_ATTR_ATK_MAX2] += role->roleAttr[E_ATTR_ATK_MAX2];//攻击最大值
			_bossAttr[E_ATTR_ATK_MIN2] += role->roleAttr[E_ATTR_ATK_MIN2];//攻击最小值
			_bossAttr[E_ATTR_SPELLATK_MAX] += role->roleAttr[E_ATTR_SPELLATK_MAX];//技能攻击最大值
			_bossAttr[E_ATTR_SPELLATK_MIN] += role->roleAttr[E_ATTR_SPELLATK_MIN];//技能攻击最小值
			//E_ATTR_SOL_MAX = 9,//道术攻击上限
			//E_ATTR_SOL_MIN = 10,//道术攻击下限
			_bossAttr[E_ATTR_DEF_MAX] += role->roleAttr[E_ATTR_DEF_MAX];//防御最大值
			_bossAttr[E_ATTR_DEF_MIN] += role->roleAttr[E_ATTR_DEF_MIN];//防御最小值
			_bossAttr[E_ATTR_SPELLDEF_MAX] += role->roleAttr[E_ATTR_SPELLDEF_MAX];//技能防御最大值
			_bossAttr[E_ATTR_SPELLDEF_MIN] += role->roleAttr[E_ATTR_SPELLDEF_MIN];//技能防御最小值
			_bossAttr[E_ATTR_DODGE] += role->roleAttr[E_ATTR_DODGE];//闪避值
			//E_ATTR_MAG_DODGE = 16,//魔法闪避
			//E_ATTR_SOL_DODGE = 17,//道术闪避
			//E_ATTR_SPEED = 18,//攻击速度
			//E_ATTR_MP_REC2		 =	19,//每回合魔法恢复值
			//E_ATTR_HP_REC2		 =	20,//每回合生命恢复值
			_bossAttr[E_ATTR_HIT] += role->roleAttr[E_ATTR_HIT];//命中值
			//E_ATTR_STAMINA	     =  22,//耐力
			//E_ATTR_STRENGTH	     =  23,//力量
			//E_ATTR_AGILITY	     =  24,//敏捷
			//E_ATTR_INTELLECT     =  25,//智慧
			//E_ATTR_SPIRIT		 =  26,//精神
			_bossAttr[E_ATTR_CRIT] += role->roleAttr[E_ATTR_CRIT];//暴击值
			_bossAttr[E_ATTR_CRIT_RES] += role->roleAttr[E_ATTR_CRIT_RES];//暴击抵抗值
			//E_ATTR_CRIT_INC	     =	29,//暴击伤害增加值
			//E_ATTR_CRIT_RED	     =	30,//暴击伤害减免值
			//E_ATTR_ATT_HEM		 =	31,//攻击吸血
			//E_ATTR_ATT_MANA	     =	32,//攻击吸蓝
			//E_ATTR_EQU_DROP	     =	33,//装备掉率
			//E_ATTR_MONEY_DROP	 =	34,//金币掉率
			//E_ATTR_LUCK		     =	35,//幸运
			//E_LUCK_DEF			 =  36,//幸运防御
			//E_ATTR_MAX = 37,//最大属性限制
		}
	}

	if (role_count > 0)
	{
		for (DWORD i=0; i<E_ATTR_MAX; i++)
		{
			_bossAttr[i] = (DWORD)((float)_bossAttr[i]/(float)role_count);
		}
		_boss_level = (WORD)boss_level / 10;
		_bossAttr[E_ATTR_HP_MAX2] = _bossAttr[E_ATTR_HP_MAX2] * 500 * 30;

		if ( _bossAttr[E_ATTR_HP_MAX2] > VALUE_MAX )
		{
			_bossAttr[E_ATTR_HP_MAX2] = VALUE_MAX;
		}

		_dwBossCurHp = _bossAttr[E_ATTR_HP_MAX2]; // 最大生命值
		return true;
	}

	return false;
}

// 获取世界BOSS状态
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
// 鼓舞世界BOSS
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

	const DWORD need_gold = 200; // 鼓舞消耗200元宝
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

	// 鼓舞成功
	string strData;
	BYTE byResult = 0;
	S_APPEND_BYTE(strData, byResult);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EMBRAVE_WORLDBOSS, strData) );
	return 0;
}

// 攻击世界BOSS
int WorldBoss::clientAttackWorldBoss(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	if (EWBS_BATTLE != _worldboss_status)
	{
		output_stream stream(S_ATTACK_WORLDBOSS);
		stream.write_byte(1);
		bufferevent_write(role->client->client_bev, stream.get_buffer(), stream.length());
		return 0;
	}

	const DWORD need_gold = 200; // 消耗200元宝
	if (!role->hasEnoughGold(need_gold))
	{
		string strData;
		BYTE byError = 2; //元宝不足
		S_APPEND_BYTE(strData, byError);
		//S_APPEND_DWORD(strData, hurt);
		//S_APPEND_BYTE(strData, byLuck);
		//S_APPEND_DWORD(strData, remain_hp_per);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ATTACK_WORLDBOSS, strData) );
		return 0;
	}
	role->subGold(need_gold);

	stBossItem& boss_item = getBossItem(role->dwRoleID);
	startBattle(role, boss_item.byEnchance, true);

	if (_dwBossCurHp == 0)
	{
		onBossEnd();
	}

	return 0;
}

WorldBoss::stBossItem& WorldBoss::getBossItem(DWORD roleid)
{
	static stBossItem s_bossitem;

	auto itr=_mapBossItems.find(roleid);
	if (itr == _mapBossItems.end())
	{
		return s_bossitem;
	}

	return itr->second;
}

bool WorldBoss::isLuckAttack(DWORD total_hp, DWORD cur_hp, DWORD remain_hp, DWORD roleid, DWORD& remain_hp_per)
{
	float fCur = (float)cur_hp / (float)total_hp * 10.0f;
	DWORD dwCur = (DWORD)fCur;
	float fRemain = (float)remain_hp / (float)total_hp * 10.0f;
	DWORD dwRemain = (DWORD)fRemain;

	bool bResult = false;
	for (DWORD i=9; i<=0; i--)
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

// 当前时间
void WorldBoss::updateTime(DWORD currenttime)
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

	//DWORD tmpAttr[E_ATTR_MAX];
	//memcpy(tmpAttr,roleAttr,sizeof(roleAttr));
	//tmpAttr[E_ATTR_HP_MAX2] = (DWORD)((float)tmpAttr[E_ATTR_HP_MAX2] * fAdd);//最大生命值
	//tmpAttr[E_ATTR_ATK_MAX2] = (DWORD)((float)tmpAttr[E_ATTR_ATK_MAX2] * fAdd);//攻击最大值
	//tmpAttr[E_ATTR_ATK_MIN2] = (DWORD)((float)tmpAttr[E_ATTR_ATK_MIN2] * fAdd);//攻击最小值

	bool bChance = byEnchance == 1 ? true : false;

	DWORD remain_hp = 0;
	FightCtrl::fightWorldBoss(role, _bossAttr, bChance, inst_battle,  _boss_level, BOSS_SKILLID, _dwBossCurHp, remain_hp);
	//remain_hp = _dwBossCurHp - 1000;
	DWORD remain_hp_per  = 0;
	BYTE byLuck = 0;
	if (isLuckAttack(_bossAttr[E_ATTR_HP_MAX2],_dwBossCurHp, remain_hp, role->dwRoleID, remain_hp_per))
	{
		byLuck = 1;
	}
	DWORD hurt = 0;
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

	DWORD rank_count = 0;
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
		for (DWORD i=rank_count; i<3; i++)
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
	DWORD _arrLuckRoleIds[MAX_LUCK_AWARDS];
	memset(_arrLuckRoleIds,0, sizeof(_arrLuckRoleIds));
}

// 报名开始时间
void WorldBoss::SignUpStartTime()
{
	ResetBossInfo();
	_worldboss_status = EWBS_SIGNUP; // 报名时间开始
}

// 报名结束时间
void WorldBoss::SignUpEndTime()
{
	_worldboss_status = EWBS_SIGNUP_END; // 报名时间结束
}

// 战斗开始时间
void WorldBoss::BattleStartTime()
{
	_vecSortItems.clear();
	save_to_db();
	_worldboss_status = EWBS_BATTLE;
}

// 战斗结束时间
void WorldBoss::BattleEndTime()
{
	_worldboss_status = EWBS_DEFAULT;
	onBossEnd();
}

// 计算世界BOSS属性
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
排名奖励		
每日排名奖励	金币	佣兵升级书（佣兵升星）
1	2000000	4级升级书*3
2	1800000	4级升级书*2
3	1600000	4级升级书*1
4	1500000	3级升级书*2
5	1400000	3级升级书*2
6-10	1300000	3级升级书*2
11-20	1200000	3级升级书*1
21-50	1100000	3级升级书*1
51-100	1000000	3级升级书*1
101-200	900000	2级升级书*2
201-500	800000	2级升级书*1
501-1000	700000	1级升级书*2
1001-2000	600000	1级升级书*2
2001-5000	500000	1级升级书*1
5000+	400000	1级升级书*1

901
902
903
904
905
*/

static void getHurtRankAwards(DWORD rank, DWORD& gold_count, WORD& itemid, DWORD& itemcount)
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
剩余血量	奖励名称	奖励
90%	幸运奖励	套装碎片*2
80%	幸运奖励	套装碎片*2
70%	幸运奖励	套装碎片*2
60%	幸运奖励	套装碎片*2
50%	幸运奖励	套装碎片*2
40%	幸运奖励	套装碎片*2
30%	幸运奖励	套装碎片*2
20%	幸运奖励	套装碎片*2
10%	幸运奖励	套装碎片*2
0%	最后一击	套装碎片*2
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
		if(role->client)bufferevent_write(role->client->client_bev, stream.get_buffer(), stream.length());
	}

	//RoleMgr::getMe().ExecAllRoles(std::bind(resetSignup,std::placeholders::_1));
	// BOSS伤害排名奖励
	std::vector<stBossItem> vecItems;
	for (auto itr=_mapBossItems.begin(); itr!=_mapBossItems.end(); itr++)
	{
		vecItems.push_back(itr->second);
	}
	std::sort(vecItems.begin(),vecItems.end(), sortHurt);
	for (DWORD rank=1; rank<=vecItems.size(); rank++)
	{
		//DWORD rank = 1;
		char szMsg[256];
		memset(szMsg, 0, sizeof(szMsg));

		DWORD gold_count = 0;
		DWORD hurt = vecItems[rank-1].hurt;
		WORD itemid = 0;
		DWORD itemcount = 0;
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

	const DWORD SUIT_PATCH_ID = 403;

	// BOSS幸运奖励
	for (DWORD i=0; i<MAX_LUCK_AWARDS; i++)
	{
		DWORD roleid = _arrLuckRoleIds[i];
		if (0 == roleid)
		{
			continue;
		}

		char szMsg[256];
		memset(szMsg, 0, sizeof(szMsg));
		DWORD itemcount = 0;
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
	_worldboss_status = EWBS_DEFAULT; // 世界BOSS状态

	memset(_bossAttr,0,sizeof(_bossAttr));
	_boss_level = 1;

	_bCalBossAttrs = false;
	
	_mapBossItems.clear();

	memset(_arrLuckRoleIds,0,sizeof(_arrLuckRoleIds));

	_dwBossCurHp = 0; // BOSS当前血量

	_last_rolename.clear();
}

// 获得伤害排行
int WorldBoss::clientGetWorldBossRank(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	string strData;
	S_APPEND_NBYTES( strData, _last_rolename.c_str(), 22 );

	for (DWORD rank=1; rank<=_vecSortItems.size(); rank++)
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
		break;
	}
	
	for (DWORD rank=1; rank<=_vecSortItems.size(); rank++)
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

// 获得冷却时间
int WorldBoss::clientGetWorldBossCoolTime(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	string strData;
	stBossItem& boss_item = getBossItem(role->dwRoleID);
	BYTE remain_second = 0;
	if (boss_item.cool_time > (DWORD)PROTOCAL::getCurTime())
	{
		remain_second = (BYTE)(boss_item.cool_time - PROTOCAL::getCurTime());
	}
	
	S_APPEND_BYTE(strData, remain_second);

	DWORD rank_count = 0;
	for (DWORD rank=1; rank<=_vecSortItems.size(); rank++)
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
		for (DWORD i=rank_count; i<3; i++)
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
	SQLSelect sqlx( DBCtrl::getSQL(), "truncate table worldboss_rank");

	for(auto & item : _vecSortItems)
	{
		SQLSelect sqlx( DBCtrl::getSQL(), "insert into worldboss_rank values(%u, %u, %u, %u)",
			item.roleid, item.cool_time, item.hurt, item.byEnchance);
	}
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