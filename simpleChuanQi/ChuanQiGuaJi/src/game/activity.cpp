#include "activity.h"
#include "jsonpacket.h"
#include "algorithm"
#include "skill.h"
#include "roleCtrl.h"
#include "mailCtrl.h"
#include "itemCtrl.h"
#include <deque>
#include <algorithm>

USE_EXTERN_CONFIG;
//using namespace rapidjson;
//
//#define   MAX_BOSSFIGHT_TIME   30000
//#define   HELPER_NUM		   6
//
//namespace ACTIVITY
//{ 
//	enum E_NOTICE_STATUS
//	{
//		E_NOTICE_STATUS_NO,//0无通知
//		E_NOTICE_STATUS_REGISTER,//1报名通知
//		E_NOTICE_STATUS_START,//2战斗开始
//		E_NOTICE_STATUS_END,//3战斗结束
//	};
//
//	static E_NOTICE_STATUS _bossFightStatus = E_NOTICE_STATUS_NO;
//
//
//
//	//1物理 2魔法 3道术
//	enum E_ATK
//	{
//		E_SKILL_ATK_TYPE_WARRIOR = 1,
//		E_SKILL_ATK_TYPE_MAGIC = 2,
//		E_SKILL_ATK_TYPE_MONK = 3,
//	};
//
//	struct SKILL_INFO
//	{
//		WORD  wID;			//技能id
//		BYTE  type;			//技能类型
//		DWORD cooldown;		//冷却时间
//		DWORD coolLeft;		//剩余冷却时间
//		WORD  priority;		//优先级
//		BYTE  skillType;	//属性类型
//		WORD  skillAtt;		//属性项目加成值
//		float skillCoff;	//属性值系数
//		SKILL_INFO(WORD  wID, BYTE  type, DWORD cooldown, WORD priority, BYTE  skillType, WORD skillAtt, float skillCoff)
//		{
//			this->wID=wID;		
//			this->cooldown=cooldown;
//			this->type = type;
//			this->priority=priority;
//			this->skillType = skillType;
//			this->skillAtt=skillAtt;
//			this->skillCoff=skillCoff;
//			this->coolLeft = cooldown;
//		}
//	};
//
//	struct  BOSSHURT
//	{
//		DWORD dwRoleID;
//		DWORD dwHurt;
//		ZERO_CONS(BOSSHURT);
//		BOSSHURT(DWORD dwRoleID, DWORD dwHurt)
//		{
//			this->dwRoleID = dwRoleID;
//			this->dwHurt = dwHurt;
//		}
//		bool operator == (const DWORD& dst)const
//		{
//			return ( this->dwRoleID == dst );
//		}
//	};
//
//	static vector< BOSSHURT > _vecBossHurt;
//	static deque<DWORD> _dequeWarriors;
//	static deque<DWORD> _dequeMagics;
//	static deque<DWORD> _dequeMonks;
//	static DWORD _bossFightHP;
//	//static bool _bBossFight = false;
//	static WORD _bossFightID;
//	//static DWORD _RankHonour[TOP_THREE] = {100, 80, 60, 10};
//	static DWORD _lastFightID;
//	int insertBossfightArmy( ROLE* role);
//	int insertArmyByJob( ROLE* role, deque<DWORD>& dequeArmy);
//	int bossFightPre( ROLE* role, unsigned char * data, size_t dataLen );
//	void getArmy( ROLE* role, vector<DWORD>& vecArmy);
//	DWORD getRandIDs(  ROLE* role, deque<DWORD> dequeIDs, size_t nums, vector<DWORD>& vecArmy);
//	//int clientGetBossFightInfos( ROLE* role );
//	//int clientBossFightInfos( ROLE* role, vector<DWORD>& vecArmy, int nTag = 0 );
//	WORD getFightSkill(vector<SKILL_INFO>&  SkillsInfo);
//	//WORD initFightSkills(const vector<WORD>& FightSkills, vector<SKILL_INFO>&  SkillsInfo);
//	//DWORD getMonAtkValue( ROLE*role, CONFIG::MONSTER_CFG* monCfg, CONFIG::SKILL_CFG2* monSkillCfg, int& intMonHurt);
//	//DWORD getRoleAtkValue( ROLE*role,CONFIG::MONSTER_CFG* monCfg,DWORD& comID,SKILL_INFO skillInfo,int& intHurt,DWORD& dwDelay);
//	//void setClothWeaponSkillLucky( ROLE* comRole,WORD& cloth, WORD& weapon, WORD& lucky, vector<SKILL_INFO>& SkillsInfo);
//	
//	//void initHelpers( ROLE* role );//找伙伴
//	void countBossFightHurt( ROLE* role, DWORD nHurt );
//	void registerBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg, const DWORD& nowSecond );
//	void startBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg, const DWORD& nowSecond );
//	void endBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg );
//	void offerBossFightReward( );
//	void sortHurtRank( );
//	void getArmyByJob( ROLE* role, const BYTE& job );
//	void getArmyByAllJob( ROLE* role, vector<vector<DWORD>>& vvecArmy);
//	DWORD getOneHelpByJob( ROLE* role, const BYTE& job, const int& nPos);
//}
//
//void ACTIVITY::sortHurtRank( )
//{
//	sort(_vecBossHurt.begin(), _vecBossHurt.end(), [](BOSSHURT one, BOSSHURT two)
//	{
//		return 	one.dwHurt > two.dwHurt;	
//	}
//	);
//}
//
DWORD ACTIVITY::getTodayNowSecond( const time_t now)//今天秒数
{
	struct tm tmNow;
	LOCAL_TIME(now, tmNow);
	DWORD nowSecond = tmNow.tm_hour * 60 * 60 + tmNow.tm_min * 60 + tmNow.tm_sec;
	return nowSecond;
}
//
////void ACTIVITY::setClothWeaponSkillLucky( ROLE* comRole,WORD& cloth, WORD& weapon, WORD& lucky, vector<SKILL_INFO>& SkillsInfo)//设置默认人物信息
////{
////	WORD fightSkill;
////	vector<WORD>  FightSkills;
////	CONFIG::ITEM_CFG* itemTempCfg = NULL;
////	ITEM_CACHE* itemTempCache = comRole->vBodyEquip[BODY_INDEX_WEAPON];//穿武器
////
////	if ( itemTempCache != NULL )
////	{
////		itemTempCfg = itemTempCache->itemCfg;
////		if ( itemTempCfg != NULL )
////		{
////			weapon = itemTempCfg->id;
////		}
////	}
////
////	itemTempCache = comRole->vBodyEquip[BODY_INDEX_CLOTH];//穿衣服
////	if ( itemTempCache != NULL )
////	{
////		itemTempCfg = itemTempCache->itemCfg;
////		if (itemTempCfg != NULL)
////		{
////			cloth = itemTempCfg->id;
////		}
////		else
////		{
////			cloth = (comRole->bySex == E_SEX_MALE)?E_MALE_CLOTH:E_FMALE_CLOTH;
////		}
////	}
////	else
////	{
////		cloth = (comRole->bySex == E_SEX_MALE)?E_MALE_CLOTH:E_FMALE_CLOTH;
////	}
////
////	for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)//提出快捷技能ids
////	{
////		if ( (fightSkill = SKILL::getSkillIDByIndex(comRole, comRole->byFightSkill[i])) != 0)
////		{
////			FightSkills.push_back(fightSkill);
////		}
////	}
////
////	for ( int i=0; i<BODY_GRID_NUMS; i++)//幸运值
////	{
////		itemTempCache = comRole->vBodyEquip[i];
////		if ( itemTempCache == NULL )
////		{
////			continue;
////		}
////		itemTempCfg = itemTempCache->itemCfg;
////		if ( itemTempCfg == NULL )
////		{
////			continue;
////		}
////		
////		lucky += itemTempCfg->luck;		
////		if ( i == BODY_INDEX_WEAPON )
////			lucky += itemTempCache->byLuckPoint;
////	}
////
////	if (comRole->byJob == E_JOB_WARRIOR)//特殊技能
////	{
////		FightSkills.push_back( FIGHT_SKILL_WARRIOR );
////	}
////	if (comRole->byJob == E_JOB_MAGIC)
////	{
////		FightSkills.push_back( FIGHT_SKILL_MAGIC );
////	}
////	if (comRole->byJob == E_JOB_MONK)
////	{
////		FightSkills.push_back( FIGHT_SKILL_MONK );
////	}
////	initFightSkills( FightSkills, SkillsInfo);
////}
//
//WORD ACTIVITY::getBossFightID()//get bossfight monster id
//{
//	return _bossFightID;
//}
//
//void ACTIVITY::setBossFightID(WORD bossID)//set bossfight monster id
//{
//	_bossFightID = bossID;
//}
//
////DWORD ACTIVITY::getMonAtkValue( ROLE*role, CONFIG::MONSTER_CFG* monCfg, CONFIG::SKILL_CFG2* monSkillCfg, int& intMonHurt)//get monster attack hurt value 
////{
////	float fHurt = 0.0f;
////	if ( monSkillCfg->type == E_SKILL_TYPE_ATTACK )
////	{
////		if ( monSkillCfg->skillPara.skillType == E_SKILL_ATK_TYPE_WARRIOR )
////		{
////			//fHurt = -( BASE::randBetween( monCfg->atk_min, monCfg->atk_max+1) +  monSkillCfg->skillPara.skillAtt)*(monSkillCfg->skillPara.skillCoff/100.0f) - BASE::randBetween( role->roleAttr[E_ATTR_PHYDEF_MIN],role->roleAttr[E_ATTR_PHYDEF_MAX]+1);
////		}
////		else if ( monSkillCfg->skillPara.skillType == E_SKILL_ATK_TYPE_MAGIC )
////		{
////			//fHurt = -( BASE::randBetween( monCfg->mag_min, monCfg->mag_max+1) + monSkillCfg->skillPara.skillAtt)*(monSkillCfg->skillPara.skillCoff/100.0f) - BASE::randBetween( role->roleAttr[E_ATTR_MAGDEF_MIN], role->roleAttr[E_ATTR_MAGDEF_MAX]+1);
////		}
////		else if ( monSkillCfg->skillPara.skillType == E_SKILL_ATK_TYPE_MONK )
////		{
////			//fHurt = -( BASE::randBetween( monCfg->sol_min, monCfg->sol_max+1) + monSkillCfg->skillPara.skillAtt)*(monSkillCfg->skillPara.skillCoff/100.0f) - BASE::randBetween( role->roleAttr[E_ATTR_MAGDEF_MIN], role->roleAttr[E_ATTR_MAGDEF_MAX]+1);
////		}
////	}
////	//else if ( monSkillCfg->type == E_SKILL_TYPE_TREAT ) //暂定怪物无治疗
////	//{
////	//	//道术治疗		
////	//	//最终治疗值＝（攻击者道术攻击值÷3＋技能加成值）×（技能系数÷100）		
////	//	//攻击者道术攻击值=上限与下限的
////
////	//	fHurt = ( BASE::randBetween( monCfg->sol_min, monCfg->sol_max+1) / 3.0f + monSkillCfg->skillPara.skillAtt ) * (monSkillCfg->skillPara.skillCoff / 100.0f);
////	//}
////
////	intMonHurt = (int)fHurt;
////
////	return 0;
////}
//
////DWORD ACTIVITY::getRoleAtkValue(ROLE*role,CONFIG::MONSTER_CFG* monCfg,DWORD& comID,SKILL_INFO skillInfo,int& intHurt, DWORD& dwDelay)//计算技能伤害
////{
////
////	float fHurt = 0.0f;
////	if ( skillInfo.type == E_SKILL_TYPE_ATTACK )
////	{
////		if ( skillInfo.skillType == E_SKILL_ATK_TYPE_WARRIOR )
////		{
////			//fHurt = -( BASE::randBetween( role->roleAttr[E_ATTR_ATK_MIN], role->roleAttr[E_ATTR_ATK_MAX]+1) + skillInfo.skillAtt)*(skillInfo.skillCoff/100.0f) - BASE::randBetween( monCfg->phydef_min, monCfg->phydef_max+1);
////		}
////		else if ( skillInfo.skillType == E_SKILL_ATK_TYPE_MAGIC )
////		{
////			//fHurt = -( BASE::randBetween( role->roleAttr[E_ATTR_MAG_MIN], role->roleAttr[E_ATTR_MAG_MAX]+1) + skillInfo.skillAtt)*(skillInfo.skillCoff/100.0f) - BASE::randBetween( monCfg->magdef_min, monCfg->magdef_max+1);
////		}
////		else if ( skillInfo.skillType == E_SKILL_ATK_TYPE_MONK )
////		{
////			fHurt = -( BASE::randBetween( role->roleAttr[E_ATTR_SOL_MIN], role->roleAttr[E_ATTR_SOL_MAX]+1) + skillInfo.skillAtt)*(skillInfo.skillCoff/100.0f) - BASE::randBetween( monCfg->magdef_min, monCfg->magdef_max+1);
////		}
////	}
////	else if ( skillInfo.type == E_SKILL_TYPE_SUMMON )
////	{
////		comID = role->dwRoleID;
////	}
////	else if ( skillInfo.type == E_SKILL_TYPE_PASSIVE )
////	{
////		comID = role->dwRoleID;
////	}
////	else if ( skillInfo.type == E_SKILL_TYPE_TREAT )
////	{
//////道术治疗		
//////最终治疗值＝（攻击者道术攻击值÷3＋技能加成值）×（技能系数÷100）		
//////攻击者道术攻击值=上限与下限的
////
////		fHurt = ( BASE::randBetween( role->roleAttr[E_ATTR_SOL_MIN], role->roleAttr[E_ATTR_SOL_MAX]+1) / 3.0f + skillInfo.skillAtt ) * (skillInfo.skillCoff / 100.0f);
////		comID = role->dwRoleID;
////	}
////
////	fHurt += fHurt * (role->byEnhanceTime) * 0.1f;
////	intHurt = (int)fHurt;
////	//dwDelay += skillInfo.coolLeft;
////	//skillInfo.coolLeft = skillInfo.cooldown;
////	return 0;
////}
//
////WORD ACTIVITY::initFightSkills(const vector<WORD>& FightSkills, vector<SKILL_INFO>&  SkillsInfo)//找到技能
////{
////	for (auto it : FightSkills)
////	{
////		CONFIG::SKILL_CFG2 *skillCfg = CONFIG::getSkillCfg(it);
////		if (skillCfg == NULL)
////		{
////			continue;
////		}
////		SkillsInfo.push_back( SKILL_INFO(it,skillCfg->type,skillCfg->cooldown,skillCfg->priority,skillCfg->skillPara.skillType,skillCfg->skillPara.skillAtt,skillCfg->skillPara.skillCoff));
////	}
////	return 0;
////}
//
//WORD ACTIVITY::getFightSkill(vector<SKILL_INFO>&  SkillsInfo)//找到优先进攻技能
//{
//	std::sort(SkillsInfo.begin(), SkillsInfo.end(), 
//				[](SKILL_INFO one, SKILL_INFO two)
//				{
//					if ( one.coolLeft == two.coolLeft )
//					{
//						return one.priority < two.priority;
//					}
//					else
//					{
//						return one.coolLeft < two.coolLeft;
//					}
//				}
//			);
//	return 0;
//}
//
//
//DWORD ACTIVITY::getRandIDs( ROLE* role, deque<DWORD> dequeIDs, size_t nums, vector<DWORD>& vecArmy)
//{
//	deque<DWORD> dequeTempIDs( dequeIDs );
//	vecArmy.clear();
//	auto it = std::find(dequeTempIDs.begin(), dequeTempIDs.end(), role->dwRoleID);
//	if (it != dequeTempIDs.end())
//	{
//		dequeTempIDs.erase(it);
//	}
//	for (size_t i = 0; i < nums && dequeTempIDs.size() > 0; ++i )
//	{
//		size_t pos = rand() % dequeTempIDs.size();
//		DWORD id = dequeTempIDs[ pos ];
//		dequeTempIDs.erase( dequeTempIDs.begin() + pos );
//		vecArmy.push_back(id);
//	}
//
//	return 0;
//}
//
//
//DWORD ACTIVITY::getOneHelpByJob( ROLE* role, const BYTE& job, const int& nPos)
//{
//	if ( job < E_JOB_WARRIOR || job > E_JOB_MONK)
//	{
//		return 0;
//	}
//	vector<DWORD> vecIDs( role->vvecFightSourceHelpers[job - 1] );
//
//	role->vecBossFightHelpers[ nPos - 1 ] = 0;
//
//	for (size_t i = 0; i < HELPER_NUM; i++)
//	{
//		auto it = std::find(vecIDs.begin(), vecIDs.end(), role->vecBossFightHelpers[i]);
//		if (it != vecIDs.end())
//		{
//			vecIDs.erase(it);
//		}
//	}
//
//	if ( vecIDs.empty() )
//	{
//		return 0;
//	}
//	size_t pos = rand() % vecIDs.size();
//	return vecIDs[ pos ];
//}
//
//void ACTIVITY::getArmyByJob( ROLE* role, const BYTE& job)
//{
//	if ( job < E_JOB_WARRIOR || job > E_JOB_MONK)
//	{
//		return;
//	}
//	if (role->vvecFightSourceHelpers[job-1].size() >= 10)
//	{
//		return;
//	}
//	size_t min_nums;
//
//	switch ( job )
//	{
//	case E_JOB_WARRIOR:
//		min_nums = std::min<size_t>( _dequeWarriors.size(), 10 );
//		getRandIDs( role, _dequeWarriors, min_nums, role->vvecFightSourceHelpers[ job-1 ]);
//		break;
//
//	case E_JOB_MAGIC:
//		min_nums = std::min<size_t>( _dequeMagics.size(), 10 );
//		getRandIDs( role, _dequeMagics, min_nums, role->vvecFightSourceHelpers[ job-1 ]);
//		break;
//	case E_JOB_MONK:
//		min_nums = std::min<size_t>( _dequeMonks.size(), 10 );
//		getRandIDs( role,_dequeMonks, min_nums, role->vvecFightSourceHelpers[ job-1 ]);
//		break;
//	}
//}
//
//void ACTIVITY::getArmyByAllJob( ROLE* role, vector<vector<DWORD>>& vvecArmy)
//{
//	vector<DWORD> vecArmyWarriorr, vecArmyMagic, vecArmyMonk;
//	size_t min_nums_warriorr = std::min<size_t>( _dequeWarriors.size(), 10 );
//	size_t min_nums_magic = std::min<size_t>( _dequeMagics.size(), 10 );
//	size_t min_nums_monk = std::min<size_t>( _dequeMonks.size(), 10 );
//	getRandIDs( role, _dequeWarriors, min_nums_warriorr, vecArmyWarriorr);
//	getRandIDs( role, _dequeMagics, min_nums_magic, vecArmyMagic);
//	getRandIDs( role,_dequeMonks, min_nums_monk, vecArmyMonk);
//	vvecArmy.push_back(vecArmyWarriorr);
//	vvecArmy.push_back(vecArmyMagic);
//	vvecArmy.push_back(vecArmyMonk);
//}
//
//
//void ACTIVITY::getArmy( ROLE* role, vector<DWORD>& vecArmy)
//{
//	size_t min_nums_warriorr = std::min<size_t>( _dequeWarriors.size(), 10 );
//	size_t min_nums_magic = std::min<size_t>( _dequeMagics.size(), 10 );
//	size_t min_nums_monk = std::min<size_t>( _dequeMonks.size(), 10 );
//	getRandIDs( role, _dequeWarriors, min_nums_warriorr, vecArmy);
//	getRandIDs( role, _dequeMagics, min_nums_magic, vecArmy);
//	getRandIDs( role,_dequeMonks, min_nums_monk, vecArmy);
//}
//
//int ACTIVITY::insertArmyByJob( ROLE* role, deque<DWORD>& dequeArmy)
//{
//	if (std::find(dequeArmy.begin(), dequeArmy.end(),role->dwRoleID) != dequeArmy.end())
//	{
//		return ALREADY_ENLIST;
//	}
//
//	dequeArmy.push_back(role->dwRoleID);
//	return ACTIVITY_SUCCESS;
//}
//
//int ACTIVITY::insertBossfightArmy( ROLE* role)
//{
//	switch (role->byJob)
//	{
//	case E_JOB_WARRIOR:
//		return insertArmyByJob(role, _dequeWarriors);
//	case E_JOB_MAGIC:
//		return insertArmyByJob(role, _dequeMagics);
//	case E_JOB_MONK:
//		return insertArmyByJob(role, _dequeMonks);
//	default: return ACTIVITY_NO_EXSIST;
//	}
//}
//
//
///*
//@//#define	    S_GET_ACTIVITY		0x0800		//get activities list
//*/
int ACTIVITY::clientGetActivities( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;
	DWORD nowSecond = getTodayNowSecond( PROTOCAL::getCurTime() );
	for (auto &it : mapActivityInfoCfg)
	{
		if (role->wLevel >= it.second.open_level && nowSecond >= it.second.advance_time && nowSecond < it.second.end_time)
		{
			S_APPEND_WORD( strData, it.first );
			//logwm("get activities:%u",it.first);
		}
	}
	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_GET_ACTIVITY, strData));
	return 0;
}

/////*
////@参加活动#define	    S_JOIN_ACTIVITY			0x0801		//join activity
////*/
////int ACTIVITY::registerActivity( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	BYTE byRet = ACTIVITY_NO_EXSIST;
////	WORD actID;
////	string strData;
////	if ( !BASE::parseWORD( data, dataLen, actID) )
////		return -1;
////	CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg(actID);
////	if ( activityCfg == NULL || role->wLevel < activityCfg->open_level)
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		S_APPEND_BYTE( strData, byRet );
////		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_JOIN_ACTIVITY, strData));
////		return 0;
////	}
////
////	DWORD nowSecond = getTodayNowSecond( PROTOCAL::getCurTime() );
////
////	if (nowSecond < activityCfg->advance_time || nowSecond >= activityCfg->end_time)//不在报名战斗时间区
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		S_APPEND_BYTE( strData, byRet );
////		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_JOIN_ACTIVITY, strData));
////	}
////	else if (nowSecond < activityCfg->begin_time)//报名
////	{
////		if ( actID == WORLD_BOSS_FIGHT )
////		{
////			byRet = insertBossfightArmy( role );
////		}
////		byRet = ACTIVITY_SUCCESS;
////		S_APPEND_BYTE( strData, byRet );
////		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_JOIN_ACTIVITY, strData));
////	}
////	//else if (nowSecond < activityCfg->end_time)
////	//{	
////		//insertBossfightArmy( role );
////		//clientGetBossFightInfos( role );//boss 战斗  wm这里只有报名
////	//}
////	return 0;
////}
//
/////*
////@获取战斗序列信息
////*/
////int ACTIVITY::clientGetBossFightInfos( ROLE* role )
////{
////	if (role  == NULL)//到点通知用
////	{
////		return 0;
////	}
////	if (role->client == NULL)//到点通知用
////	{
////		return 0;
////	}
////	vector<DWORD> vecArmy;
////	vecArmy.push_back(role->dwRoleID);//自己在第一个
////	getArmy(role, vecArmy);
////	clientBossFightInfos( role, vecArmy);
////	return 0;
////	//while (vecArmy.size() < 7)
////	//{
////	//	vecArmy.push_back(role->dwRoleID);
////	//}
////	//CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );
////	//if ( activityCfg == NULL )
////	//{
////	//	return 0;
////	//}
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(activityCfg->monster_id);
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( _bossFightID );
////	//if ( monCfg == NULL )
////	//{
////	//	return 0;
////	//}
////
////	//CONFIG::SKILL_CFG2* monSkillCfg = CONFIG::getSkillCfg(monCfg->skill_id);
////	//if (monSkillCfg == NULL)
////	//{
////	//	return 0;
////	//}
////
////	//BYTE byFinal = 0;
////	//BYTE byRet = 0;
////	//DWORD dwFightTime = 0;
////	//WORD wRoleHP = role->roleAttr[E_ATTR_HP_MAX];
////	//char xxx[1024*20]={0};
////	//char xxxMon[10240]={0};
////	//char* p = xxx;
////	//char* pMon = xxxMon;
////	//if (_bossFightHP == 0)
////	//{
////	//	byRet = ACTIVITY_BOSS_DIE;
////	//}
////	//
////	//p += sprintf( p, "{\"error\":%u,", byRet );
////	//p += sprintf( p, "\"m\":%u,", monCfg->id );
////	//p += sprintf( p, "\"mb\":%u,", _bossFightHP );
////	//p += sprintf( p, "\"hf\":[");
////	//for (size_t roleIndex = 0; roleIndex < vecArmy.size(); ++roleIndex)
////	//{
////	//	DWORD id = vecArmy[roleIndex];
////
////	//	ROLE* comRole = RoleMgr::getMe().getRoleByID( id );
////	//	if (comRole == NULL)
////	//	{
////	//		continue;
////	//	}
////
////	//	WORD cloth = 0;
////	//	WORD weapon = 0;
////	//	WORD lucky = 0;
////	//	DWORD dwAllDelay = 0;
////	//	DWORD dwOneDelay = 0;
////	//	DWORD comID;
////	//	int intHurt;
////	//	vector<SKILL_INFO> SkillsInfo;
////	//	setClothWeaponSkillLucky( comRole, cloth,  weapon,  lucky, SkillsInfo );
////
////	//	p += sprintf( p, "{");
////	//	p += sprintf( p, "\"w\":%u,", weapon );
////	//	p += sprintf( p, "\"c\":%u,", cloth );
////	//	p += sprintf( p, "\"j\":%u,", comRole->byJob );
////	//	p += sprintf( p, "\"s\":%u,", comRole->bySex );
////	//	p += sprintf( p, "\"id\":%u,", comRole->dwRoleID );
////	//	p += sprintf( p, "\"q\":[");
////
////	//	if (roleIndex == 0 )//self
////	//	{
////	//		DWORD dwTempAllDelay = 0;
////	//		int intMonHurt = 0;
////
////	//		while (wRoleHP > 0 && _bossFightHP > 0 && dwAllDelay <=MAX_BOSSFIGHT_TIME && SkillsInfo.size() > 0 )
////	//		{
////	//			//self fight sequence
////	//			getFightSkill(SkillsInfo);
////	//			comID = monCfg->id;//治疗的时候可能会变成自己
////	//			getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//			p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////
////	//			dwOneDelay = SkillsInfo[0].coolLeft;
////	//			dwAllDelay += dwOneDelay;
////
////	//			for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//			{
////	//				if (i == 0)//使用的
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//				}
////	//				else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//				{
////	//					SkillsInfo[i].coolLeft = 0;
////	//				}
////	//				else
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//				}
////	//			}
////
////	//			if (comID == monCfg->id && intHurt >= 0)//role hurt monster
////	//			{
////	//				if ( _bossFightHP <= (DWORD)intHurt)
////	//				{
////	//					_bossFightHP = 0;//怪物被打死 记下  dwAllDelay
////	//					byFinal = 1;
////	//				}
////	//				else
////	//				{
////	//					_bossFightHP -= (DWORD)intHurt;
////	//				}
////	//			}
////
////	//			//moster fight sequence
////	//			if (dwAllDelay >= MAX_BOSSFIGHT_TIME)
////	//			{
////	//				intMonHurt = -(int)wRoleHP;
////	//				wRoleHP = 0;
////	//				pMon += sprintf( pMon, "[%u,%u,%u,%d,%u],", monCfg->id,4,role->dwRoleID,intMonHurt,dwAllDelay);//4 is kill skill
////	//				break;
////	//			}
////
////	//			dwTempAllDelay += dwOneDelay;
////	//			size_t count = dwTempAllDelay/3000;
////
////	//			for (size_t i = count; i > 0; --i)
////	//			{
////	//				getMonAtkValue( role, monCfg, monSkillCfg, intMonHurt);
////	//						
////	//				pMon += sprintf( pMon, "[%u,%u,%u,%d,%u],", monCfg->id,monSkillCfg->id,role->dwRoleID,intMonHurt, (dwAllDelay - i*3000));
////
////	//				if (intMonHurt > 0 )
////	//				{
////	//					wRoleHP -= (WORD)intMonHurt;
////	//				}
////	//				dwTempAllDelay -= 3000;
////	//			}
////	//		}
////	//		dwFightTime = dwAllDelay;
////	//	}
////	//	else//other  fight sequence
////	//	{
////	//		while ( dwAllDelay <=dwFightTime && SkillsInfo.size() > 0 && _bossFightHP > 0)
////	//		{
////	//			getFightSkill(SkillsInfo);
////
////	//			comID = monCfg->id;//治疗的时候可能会变成自己
////	//			getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//			p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////
////	//			dwOneDelay = SkillsInfo[0].coolLeft;
////	//			dwAllDelay += dwOneDelay;
////
////	//			for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//			{
////	//				if (i == 0)//使用的
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//				}
////	//				else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//				{
////	//					SkillsInfo[i].coolLeft = 0;
////	//				}
////	//				else
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//				}
////	//			}
////	//		}
////
////	//	}
////	//	*(--p) = 0;
////	//	p += sprintf( p, "]},");
////	//}
////	//*(--p) = 0;
////	//p += sprintf( p, "],");
////	//*(--pMon) = 0;
////	//pMon = xxxMon;
////	//p += sprintf( p, "\"mf\":[{\"q\":[%s]}]",pMon);
////	//p += sprintf( p, "\"f\":%u}",byFinal);
////	//logplan("bossfight:\n %s",xxx);
////
////	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_FIGHT, xxx) );
////	//return 0;
////}
//
/////*
////@boss战 鼓舞#define	    S_ENHANCE_BOSS_FIGHT		0x0803		//
////*/
////int ACTIVITY::enhanceBossFightActtivity( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	//WORD wID;
////	//if ( !BASE::parseWORD( data, dataLen, wID) )
////	//	return -1;
////
////	BYTE byRet = ACTIVITY_SUCCESS;
////	string strData;
////
////	if (role->byEnhanceTime >= 10)
////	{
////		byRet = BOSSFIGHT_ENHANCE_ENOUGH;
////	}
////	else if (role->dwIngot < 20)
////	{
////		byRet = COLD_NOT_ENOUGH;
////	}
////	else
////	{
////		role->byEnhanceTime++;
////	}
////	
////	S_APPEND_BYTE( strData, byRet);
////	S_APPEND_WORD( strData, (WORD)role->byEnhanceTime * 10);
////	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_ENHANCE_BOSS_FIGHT, strData));
////
////	return 0;
////}
//
//*
//@#define	    S_BOSS_TIME_LEFT		0x0804		//获取活动开始否
//*/
int ACTIVITY::getActtivityLeftTime( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byRet;
	WORD actID;
	string strData;
	if ( !BASE::parseWORD( data, dataLen, actID) )
		return 0;
	CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg(actID);
	if ( activityCfg == NULL )
	{
		byRet = ACTIVITY_NOT_OPEN;
		S_APPEND_BYTE( strData, byRet );
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_BOSS_TIME_LEFT, strData));
		return 0;
	}

	DWORD nowSecond = getTodayNowSecond( PROTOCAL::getCurTime() );

	if (nowSecond < activityCfg->advance_time || nowSecond >= activityCfg->end_time)//还未开放
	{
		byRet = ACTIVITY_NOT_OPEN;
	    S_APPEND_BYTE( strData, byRet );
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_BOSS_TIME_LEFT, strData));
	}
	else if (nowSecond < activityCfg->begin_time)//可以报名
	{
		//role->wBossFightActivityID = actID;
		byRet = ACTIVITY_WAITING;
		S_APPEND_BYTE( strData, byRet );
		S_APPEND_DWORD( strData, activityCfg->begin_time - nowSecond);
		S_APPEND_WORD( strData, (WORD)role->byEnhanceTime * 10);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_BOSS_TIME_LEFT, strData));
	}
	else if (nowSecond < activityCfg->end_time)//可以参加
	{
		//role->wBossFightActivityID = actID;
		byRet = ACTIVITY_SUCCESS;
		S_APPEND_BYTE( strData, byRet );
		S_APPEND_WORD( strData, (WORD)role->byEnhanceTime * 10);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_BOSS_TIME_LEFT, strData));
	}

	return 0;
}
//
//
//////new define	    S_BOSS_FIGHT				0x0802		//Boss战斗
////int ACTIVITY::clientBossFight2( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	rapidjson::Document document;
////	if (document.Parse<0>( string( (char*)data, dataLen ).c_str() ).HasParseError())
////		return 0;
////
////	int nTag = JSON::getJsonValueInt( document, "tag" );
////	if (nTag <= 0)
////	{
////		logwm("5nTag = %d",nTag);
////		//return 0;
////	}
////	return getBossFightHelperMe( role, nTag );
////}
//
//////new S_BOSS_GET_HELPER			0x0805		//补人
////int ACTIVITY::getBossFightHelper2( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	rapidjson::Document document;
////	if (document.Parse<0>( string( (char*)data, dataLen ).c_str() ).HasParseError())
////		return 0;
////
////	int nTag = JSON::getJsonValueInt( document, "tag" );
////	int nPos = JSON::getJsonValueInt( document, "pos" );
////	if ( nTag <= 0 )
////	{
////		logwm("helper error nTag = %d",nTag);
////		return 0;
////	}
////	//if ( nPos <= 0 || (WORD)nPos > role->vecBossFightHelpers.size() )
////	//{
////	//	logwm("helper error nPos = %d",nPos);
////	//	return 0;
////	//}
////
////
////	BYTE job;
////	switch (nPos)
////	{
////	case 1:
////	case 2:
////		job = E_JOB_WARRIOR;break;
////	case 3:
////	case 4:
////		job = E_JOB_MAGIC;break;
////	case 5:
////	case 6:
////		job = E_JOB_MONK;break;
////	default:
////		return 0;
////	}
////
////	getArmyByJob(role, job);
////
////	DWORD helpID = getOneHelpByJob(role, job, nPos);
////	if (helpID != 0)
////	{
////		role->vecBossFightHelpers[ nPos - 1 ] = helpID;
////		getBossFightHelperOther( role, helpID, nTag);
////	}
////	
////	return 0;
////}
//
//
//
/////*
////@old define	    S_BOSS_GET_HELPER		0x0805		//补人
////*/
////int ACTIVITY::getBossFightHelper( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	rapidjson::Document document;
////	if (document.Parse<0>( string( (char*)data, dataLen ).c_str() ).HasParseError())
////		return 0;
////	DWORD dwHelpID;
////	if ( ( dwHelpID = JSON::getJsonValueInt( document, "id" )) <= 0 )
////	{
////		logwm("helper: %u",dwHelpID);
////		return 0;
////	}
////
////	if (role->dwRoleID == dwHelpID)//补自己
////	{
////		getBossFightHelperMe( role );
////	}
////	else
////	{
////		getBossFightHelperOther( role, dwHelpID );
////	}
////	return 0;
////	//BYTE byJob;
////	//if ( !BASE::parseBYTE( data, dataLen, byJob) )
////	//	return -1;
////	//switch (byJob)
////	//{
////	//case E_JOB_WARRIOR:
////	//	getRandIDs( role, _dequeWarriors, 1, vecArmy);break;
////	//case E_JOB_MAGIC:
////	//	getRandIDs( role, _dequeMagics, 1, vecArmy);break;
////	//case E_JOB_MONK:
////	//	getRandIDs( role,_dequeMonks, 1, vecArmy);break;
////	//default: return 0;
////	//}
////
////	//WORD actID = role->wBossFightActivityID;
////	//CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );
////	//if ( activityCfg == NULL )
////	//{
////	//	return 0;
////	//}
////
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(activityCfg->monster_id);
////	//if ( monCfg == NULL )
////	//{
////	//	return 0;
////	//}
////
////	//BYTE byFinal = 0;
////	//BYTE byRet = 0;
////	//DWORD monFightTime = 0;
////	//char xxx[2048]={0};
////	//char* p = xxx;
////	//if (_bossFightHP == 0)
////	//{
////	//	byRet = ACTIVITY_BOSS_DIE;
////	//}
////
////	//ROLE* comRole = RoleMgr::getMe().getRoleByID( dwRoleID );
////	//if (comRole == NULL)
////	//{
////	//	return 0;
////	//}
////
////	//WORD cloth = 0;
////	//WORD weapon = 0;
////	//WORD lucky = 0;
////	//int intHurt;
////	//DWORD dwAllDelay = 0;
////	//DWORD dwOneDelay = 0;
////	//DWORD comID;
////	//vector<SKILL_INFO> SkillsInfo;
////	//setClothWeaponSkillLucky( comRole, cloth,  weapon,  lucky, SkillsInfo );
////	//p += sprintf( p, "{\"error\":%u,", byRet );
////	//p += sprintf( p, "\"member\":{");
////	//p += sprintf( p, "\"w\":%u,", weapon );
////	//p += sprintf( p, "\"c\":%u,", cloth );
////	//p += sprintf( p, "\"j\":%u,", comRole->byJob );
////	//p += sprintf( p, "\"s\":%u,", comRole->bySex );
////	//p += sprintf( p, "\"id\":%u,", comRole->dwRoleID );
////	//p += sprintf( p, "\"q\":[");
////
////	//while ( dwAllDelay <=MAX_BOSSFIGHT_TIME && SkillsInfo.size() > 0 && _bossFightHP>0 )
////	//{
////	//	getFightSkill(SkillsInfo);
////	//	comID = monCfg->id;//治疗的时候可能会变成自己
////	//	getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//	p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////	//	dwOneDelay = SkillsInfo[0].coolLeft;
////	//	dwAllDelay += dwOneDelay;
////	//	for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//	{
////	//		if (i == 0)
////	//		{
////	//			SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//		}
////	//		else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//		{
////	//			SkillsInfo[i].coolLeft = 0;
////	//		}
////	//		else
////	//		{
////	//			SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//		}
////	//	}
////	//}
////	//*(--p) = 0;
////	//p += sprintf( p, "]}}");
////	//logplan("buren:  %s",xxx);
////	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//return 0;
////}
//
/////*
////@//补自己 相当于重新战斗
////*/
////int ACTIVITY::getBossFightHelperMe( ROLE* role, int nTag )
////{
////	BYTE byRet ;
////	string strData;
////	DWORD nowSecond;
////	CONFIG::ACTIVITY_INFO_CFG* activityCfg;
////	vector<DWORD> vecArmy;
////	char xxx[1024];
////	if ( _bossFightStatus != E_NOTICE_STATUS_START )//未开始
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		goto end;
////	}
////
////	activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );
////	if ( activityCfg == NULL || role->wLevel < activityCfg->open_level)
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		goto end;
////	}
////
////	nowSecond = getTodayNowSecond( PROTOCAL::getCurTime() );
////
////	if( nowSecond <= role->dwFinalWorldBossTime + MAX_BOSSFIGHT_TIME/1000 )
////	{
////		byRet = NO_FIGHT_COLD_ENOUGH;
////		goto end;
////	}
////	if (nowSecond < activityCfg->begin_time || nowSecond >= activityCfg->end_time)//不在战斗时间区
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		goto end;
////	}
////	if ( _bossFightHP == 0 )
////	{
////		byRet = ACTIVITY_BOSS_DIE;
////		goto end;
////	}
////
////	vecArmy.push_back(role->dwRoleID);
////	clientBossFightInfos( role, vecArmy, nTag);
////	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_WORLD_BOSS);
////	//initHelpers( role );//找伙伴
////	insertBossfightArmy( role );
////	sortHurtRank();
////	return 0;
////
////end:
////	//sprintf(xxx, "{\"error\":%u}", byRet );
////	sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, byRet );
////	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_FIGHT, xxx) );
////	return 0;
////}
/////*
////@//补其他人
////*/
////int ACTIVITY::getBossFightHelperOther( ROLE* role, DWORD& helperID, int nTag)
////{
////
////	//BYTE byRet = 0;
////	//char xxx[20480]={0};
////	//CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );
////	//if ( activityCfg == NULL  || role->wLevel < activityCfg->open_level)
////	//{
////	//	byRet = ACTIVITY_NO_EXSIST;
////	//	sprintf( xxx, "{\"tag\":%d,\"error\":%u}", nTag, byRet );
////	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//	return 0;
////	//}
////
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(activityCfg->monster_id);
////	//if ( monCfg == NULL )
////	//{
////	//	byRet = ACTIVITY_NO_EXSIST;
////	//	sprintf( xxx, "{\"tag\":%d,\"error\":%u}", nTag, byRet );
////	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//	return 0;
////	//}
////
////	//BYTE byFinal = 0;
////	//
////	//DWORD monFightTime = 0;
////
////	//char* p = xxx;
////	//if (_bossFightHP == 0)
////	//{
////	//	byRet = ACTIVITY_BOSS_DIE;
////	//	sprintf( xxx, "{\"tag\":%d,\"error\":%u}", nTag, byRet );
////	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//	return 0;
////	//}
////
////	//ROLE* comRole = RoleMgr::getMe().getRoleByID( helperID );
////	//if (comRole == NULL)
////	//{
////	//	byRet = ACTIVITY_NO_EXSIST;
////	//	sprintf( xxx, "{\"tag\":%d,\"error\":%u}", nTag, byRet );
////	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//	return 0;
////	//}
////
////	//WORD cloth = 0;
////	//WORD weapon = 0;
////	//WORD lucky = 0;
////	//int intHurt;
////	//DWORD dwAllDelay = 0;
////	//DWORD dwOneDelay = 0;
////	//DWORD comID;
////	//vector<SKILL_INFO> SkillsInfo;
////	//setClothWeaponSkillLucky( comRole, cloth,  weapon,  lucky, SkillsInfo );
////	//p += sprintf( p, "{ \"tag\":%d,\"error\":%u,", nTag, byRet );
////	//p += sprintf( p, "\"member\":{");
////	//p += sprintf( p, "\"n\":\"%s\",", comRole->roleName.c_str());
////	//p += sprintf( p, "\"w\":%u,", weapon );
////	//p += sprintf( p, "\"c\":%u,", cloth );
////	//p += sprintf( p, "\"j\":%u,", comRole->byJob );
////	//p += sprintf( p, "\"s\":%u,", comRole->bySex );
////	//p += sprintf( p, "\"id\":%u,", comRole->dwRoleID );
////	//p += sprintf( p, "\"q\":[");
////
////	//while ( dwAllDelay <=MAX_BOSSFIGHT_TIME && SkillsInfo.size() > 0 && _bossFightHP>0 )
////	//{
////	//	getFightSkill(SkillsInfo);
////	//	comID = monCfg->id;//治疗的时候可能会变成自己
////	//	getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//	dwOneDelay = SkillsInfo[0].coolLeft;
////	//	dwAllDelay += dwOneDelay;
////	//	p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////	//	for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//	{
////	//		if (i == 0)
////	//		{
////	//			SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//		}
////	//		else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//		{
////	//			SkillsInfo[i].coolLeft = 0;
////	//		}
////	//		else
////	//		{
////	//			SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//		}
////	//	}
////	//}
////	//*(--p) = 0;
////	//p += sprintf( p, "]}}");
////	//logwm("buren:\n  %s",xxx);
////	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	return 0;
////
////}
//
/////*
////@战斗序列信息
////*/
////int ACTIVITY::clientBossFightInfos( ROLE* role, vector<DWORD>& vecArmy, int nTag )
////{
////	//if (role  == NULL)//到点通知用
////	//{
////	//	return 0;
////	//}
////	//if (role->client == NULL)//到点通知用
////	//{
////	//	return 0;
////	//}
////
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( _bossFightID );
////	//if ( monCfg == NULL )
////	//{
////	//	return 0;
////	//}
////
////	//CONFIG::SKILL_CFG2* monSkillCfg = CONFIG::getSkillCfg(monCfg->skill_id);
////	//if (monSkillCfg == NULL)
////	//{
////	//	return 0;
////	//}
////
////	//BYTE byFinal = 0;
////	//BYTE byRet = 0;
////	//DWORD dwFightTime = 0;
////	//DWORD dwRoleHP = role->roleAttr[E_ATTR_HP_MAX2];
////	//char xxx[1024*20]={0};
////	//char xxxMon[10240]={0};
////	//char* p = xxx;
////	//char* pMon = xxxMon;
////	//if (_bossFightHP == 0)
////	//{
////	//	byRet = ACTIVITY_BOSS_DIE;
////	//}
////
////	////p += sprintf( p, "{\"error\":%u,", byRet );
////	//p += sprintf(p, "{\"tag\":%d,\"error\":%u",nTag, byRet );
////	//p += sprintf( p, "\"m\":%u,", monCfg->id );
////	//p += sprintf( p, "\"mb\":%u,", _bossFightHP );
////	//p += sprintf( p, "\"hf\":[ ");
////	//for (size_t roleIndex = 0; roleIndex < vecArmy.size(); ++roleIndex)
////	//{
////	//	DWORD id = vecArmy[roleIndex];
////
////	//	ROLE* comRole = RoleMgr::getMe().getRoleByID( id );
////	//	if (comRole == NULL)
////	//	{
////	//		continue;
////	//	}
////
////	//	WORD cloth = 0;
////	//	WORD weapon = 0;
////	//	WORD lucky = 0;
////	//	DWORD dwAllDelay = 0;
////	//	DWORD dwOneDelay = 0;
////	//	DWORD comID;
////	//	int intHurt;
////	//	vector<SKILL_INFO> SkillsInfo;
////	//	setClothWeaponSkillLucky( comRole, cloth,  weapon,  lucky, SkillsInfo );
////
////	//	p += sprintf( p, "{");
////	//	p += sprintf( p, "\"w\":%u,", weapon );
////	//	p += sprintf( p, "\"c\":%u,", cloth );
////	//	p += sprintf( p, "\"j\":%u,", comRole->byJob );
////	//	p += sprintf( p, "\"s\":%u,", comRole->bySex );
////	//	p += sprintf( p, "\"id\":%u,", comRole->dwRoleID );
////	//	p += sprintf( p, "\"q\":[ ");
////
////	//	if (roleIndex == 0 )//self
////	//	{
////	//		DWORD dwTempAllDelay = 0;
////	//		int intMonHurt = 0;
////
////	//		while (dwRoleHP > 0 && _bossFightHP > 0 && dwAllDelay <=MAX_BOSSFIGHT_TIME && SkillsInfo.size() > 0 )
////	//		{
////	//			//self fight sequence
////	//			getFightSkill(SkillsInfo);
////	//			comID = monCfg->id;//治疗的时候可能会变成自己
////	//			getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//			
////	//			dwOneDelay = SkillsInfo[0].coolLeft;
////	//			dwAllDelay += dwOneDelay;
////	//			p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////
////	//			
////	//			for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//			{
////	//				if (i == 0)//使用的
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//				}
////	//				else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//				{
////	//					SkillsInfo[i].coolLeft = 0;
////	//				}
////	//				else
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//				}
////	//			}
////
////	//			if (comID == monCfg->id && intHurt < 0)//role hurt monster
////	//			{
////	//				if ( _bossFightHP <= (DWORD)(-intHurt) )
////	//				{
////	//					_bossFightHP = 0;//怪物被打死 记下  dwAllDelay
////	//					byFinal = 1;
////	//					_lastFightID = role->dwRoleID;
////	//				}
////	//				else
////	//				{
////	//					_bossFightHP -= (DWORD)(-intHurt);
////	//				}
////	//
////	//				countBossFightHurt( role, -(intHurt) );
////	//			}
////
////	//			//moster fight sequence
////	//			if (dwAllDelay >= MAX_BOSSFIGHT_TIME)
////	//			{
////	//				intMonHurt = -(int)dwRoleHP;
////	//				dwRoleHP = 0;
////	//				pMon += sprintf( pMon, "[%u,%u,%u,%d,%u],", monCfg->id,4,role->dwRoleID,intMonHurt,dwAllDelay);//4 is kill skill
////	//				break;
////	//			}
////
////	//			dwTempAllDelay += dwOneDelay;
////	//			size_t count = dwTempAllDelay/3000;
////
////	//			for (size_t i = count; i > 0; --i)
////	//			{
////	//				getMonAtkValue( role, monCfg, monSkillCfg, intMonHurt);
////
////	//				pMon += sprintf( pMon, "[%u,%u,%u,%d,%u],", monCfg->id,monSkillCfg->id,role->dwRoleID,intMonHurt, (dwAllDelay - i*3000));
////
////	//				if (intMonHurt > 0 )
////	//				{
////	//					dwRoleHP -= (WORD)intMonHurt;
////	//				}
////	//				dwTempAllDelay -= 3000;
////	//			}
////	//		}
////	//		dwFightTime = dwAllDelay;
////	//	}
////	//	else//other  fight sequence
////	//	{
////	//		while ( dwAllDelay <=dwFightTime && SkillsInfo.size() > 0 && _bossFightHP > 0)
////	//		{
////	//			getFightSkill(SkillsInfo);
////
////	//			comID = monCfg->id;//治疗的时候可能会变成自己
////	//			getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//			dwOneDelay = SkillsInfo[0].coolLeft;
////	//			dwAllDelay += dwOneDelay;
////	//			p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);			
////	//			for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//			{
////	//				if (i == 0)//使用的
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//				}
////	//				else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//				{
////	//					SkillsInfo[i].coolLeft = 0;
////	//				}
////	//				else
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//				}
////	//			}
////	//		}
////
////	//	}
////	//	*(--p) = 0;
////	//	p += sprintf( p, "]},");
////	//}
////	//*(--p) = 0;
////	//p += sprintf( p, "],");
////
////	//if (pMon != xxxMon)
////	//{
////	//	*(--pMon) = 0;
////	//}
////	//
////	////pMon = xxxMon;
////	//p += sprintf( p, "\"mf\":[{\"q\":[%s]}]",xxxMon);
////	//p += sprintf( p, "\"f\":%u}",byFinal);
////	//logwm("bossfight:\n %s",xxx);
////
////	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_FIGHT, xxx) );
////	return 0;
////}
//
///*
//@战斗到点开始和关闭
//*/
//void ACTIVITY::startTodaySec(const time_t& curTime)
//{
//	//	E_NOTICE_STATUS_NO,
//	//	E_NOTICE_STATUS_REGISTER,
//	//	E_NOTICE_STATUS_START,
//
//	DWORD nowSecond = getTodayNowSecond( curTime );
//
//	CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );//现在只有世界boss战
//	if ( activityCfg == NULL )
//	{
//		return;
//	}
//
//	if ( _bossFightStatus == E_NOTICE_STATUS_NO && nowSecond >= activityCfg->advance_time )
//	{
//		registerBossFight(activityCfg, nowSecond);
//	}
//	
//	if ( _bossFightStatus == E_NOTICE_STATUS_REGISTER && nowSecond >= activityCfg->begin_time )
//	{
//		startBossFight(activityCfg, nowSecond);
//	}
//
//	if ( _bossFightStatus == E_NOTICE_STATUS_START && nowSecond >= activityCfg->end_time )//CONFIG:: 怪物 打死id增加1  不死id加1
//	{
//		endBossFight(activityCfg);
//	}
//}
//void ACTIVITY::notifyRoleActivityStatus( ROLE* role )
//{
//	DWORD nowSecond = getTodayNowSecond( PROTOCAL::getCurTime() );
//
//	CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );//boss战
//
//	if ( _bossFightStatus == E_NOTICE_STATUS_REGISTER || _bossFightStatus == E_NOTICE_STATUS_START )
//	{
//		notifyActivityStatus( role, 2, activityCfg->id );
//	}
//}
//
//void ACTIVITY::notifyActivityStatus( ROLE* role, const BYTE& status, const WORD& actID, DWORD dwTime )
//{
//	if ( role->client == NULL )
//	{
//		return;
//	}
//	
//	string strData;
//	S_APPEND_BYTE( strData, 3 );//活动模块
//	S_APPEND_BYTE( strData, status );//该活动状态通知
//	S_APPEND_BYTE( strData, (BYTE)actID );//活动ID
//	S_APPEND_DWORD( strData, dwTime );//活动ID
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FUNCTION_NOTIFY, strData) );
//}
//
//void ACTIVITY::judgeNewActivity( ROLE* role, const WORD& wOldLevel )//新活动解锁
//{
//	for (auto &it : mapActivityInfoCfg)
//	{
//		if ( wOldLevel < it.second.open_level && role->wLevel >= it.second.open_level )
//		{
//			notifyActivityStatus( role, 1, it.first );
//		}
//	}
//}
//
////void ACTIVITY::initHelpers( ROLE* role )//找伙伴
////{
////	//getArmy(role, role->vecBossFightHelpers);
////	//getArmyByJob(role, role->vvecFightSourceHelpers);
////	
////	//size_t job_num = min<size_t>(role->vvecFightSourceHelpers.size(), 3);
////	//for( size_t i = 0; i < job_num; i++ )
////	//{
////	//	size_t help_num = min<size_t>(role->vvecFightSourceHelpers[i].size(), 2);
////	//	for (size_t j = 0; j < help_num; j++)
////	//	{
////	//		role->vvecFightSourceHelpers[i][j];
////	//	}
////	//}
////
////	//DWORD id = 1;
////	//while ( role->vecBossFightHelpers.size() < HELPER_NUM )
////	//{
////	//	if (role->dwRoleID != id && find(role->vecBossFightHelpers.begin(), role->vecBossFightHelpers.end(), id) == role->vecBossFightHelpers.end())
////	//	{
////	//		role->vecBossFightHelpers.push_back(id);
////	//	}
////	//	id++;
////	//}
////}
//
//void ACTIVITY::countBossFightHurt( ROLE* role, DWORD nHurt )//
//{
//	auto it = find(_vecBossHurt.begin(), _vecBossHurt.end(), role->dwRoleID);
//	if ( it == _vecBossHurt.end() )
//	{
//		_vecBossHurt.push_back( BOSSHURT(role->dwRoleID,nHurt));
//	}
//	else
//	{
//		it->dwHurt += nHurt;
//	}
//}
//
//void ACTIVITY::registerBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg, const DWORD& nowSecond )
//{
//	DWORD dwTimeLeft = activityCfg->begin_time >= nowSecond ? (activityCfg->begin_time - nowSecond) : 0;
//	ROLE * role;
//	PROTOCAL::forEachOnlineRole( [ & ] ( const std::pair<DWORD, ROLE_CLIENT> &x )
//	{
//		if ((role = x.second.role) != NULL)
//		{
//			notifyActivityStatus( role, 2, activityCfg->id, dwTimeLeft );
//		}	
//	}
//	);
//	_bossFightStatus = E_NOTICE_STATUS_REGISTER;
//}
//
//void ACTIVITY::startBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg, const DWORD& nowSecond )
//{
//	CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( _bossFightID );
//	if (monCfg == NULL)
//	{
//		return;
//	}
//	_bossFightHP = monCfg->hp_max;
//	_vecBossHurt.clear();
//	_lastFightID = 0;
//	DWORD dwTimeLeft = activityCfg->end_time >= nowSecond ? (activityCfg->end_time - nowSecond) : 0;
//	ROLE * role;
//
//	for (auto it:_dequeWarriors)
//	{
//		role = RoleMgr::getMe().getRoleByID(it);
//		notifyActivityStatus( role, 2, activityCfg->id, dwTimeLeft );
//	}
//	for (auto it:_dequeMagics)
//	{
//		role = RoleMgr::getMe().getRoleByID(it);
//		notifyActivityStatus( role, 2, activityCfg->id, dwTimeLeft );
//	}
//	for (auto it:_dequeMonks)
//	{
//		role = RoleMgr::getMe().getRoleByID(it);
//		notifyActivityStatus( role, 2, activityCfg->id, dwTimeLeft );
//	}
//
//	_bossFightStatus = E_NOTICE_STATUS_START;
//}
//
//void ACTIVITY::endBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg )
//{
//	if ( _bossFightHP == 0 && CONFIG::getMonsterCfg( _bossFightID+1 ) != NULL )
//	{
//		++_bossFightID;
//	}
//	else if( CONFIG::getMonsterCfg( _bossFightID-1 ) != NULL )
//	{
//		--_bossFightID;
//	}
//
//	offerBossFightReward( );
//
//	ROLE * role;
//	PROTOCAL::forEachOnlineRole( [ & ] ( const std::pair<DWORD, ROLE_CLIENT> &x )
//	{
//		if ((role = x.second.role) != NULL)
//		{
//			notifyActivityStatus( role, 0, activityCfg->id );
//		}	
//	}
//	);
//	_bossFightStatus = E_NOTICE_STATUS_END;
//
//	_bossFightHP = 0;
//	_dequeWarriors.clear();
//	_dequeMagics.clear();
//	_dequeMonks.clear();
//	_vecBossHurt.clear();
//}
//
//#define   TOP_THREE			   3
//
//#define   LAST_HURT_REWARD	   3
//
//#define   CHEST_REWARD		   0
//#define   CHEST_KEY_REWARD	   1
//#define   HONOUR_REWARD		   2
//
//#define   REWARD_ITEM_NUMS	   3
//
//#define   JOIN_HONOUR_REWARD   30
//
//void ACTIVITY::offerBossFightReward( )
//{
//	CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( _bossFightID );
//	if (monCfg == NULL || monCfg->hp_max < 100)
//	{
//		return;
//	}
//	CONFIG::WORLD_BOSS_REWARD_CFG* worldBossRewardCfg = CONFIG::getWorldBossRewardCfg( _bossFightID );
//	if ( worldBossRewardCfg == NULL )
//	{
//		return;
//	}
//
//	DWORD id = 0;
//	DWORD honour = 0;
//	DWORD coin = 0;
//	DWORD coinOrig  = 0;
//	ROLE* role = NULL;
//	DWORD bossHP = monCfg->hp_max;
//	size_t nums = std::min<size_t>( _vecBossHurt.size(), TOP_THREE );
//	vector<ITEM_INFO> vecItemInfo;
//	CONFIG::ITEM_CFG* itemCfg0, *itemCfg1;
//	ITEM_INFO* pVItemReward[REWARD_ITEM_NUMS];
//
//	for (size_t i = 0; i < nums; i++)//top three
//	{
//		pVItemReward[CHEST_REWARD] = &worldBossRewardCfg->vvecReward[i][CHEST_REWARD];//宝箱
//		pVItemReward[CHEST_KEY_REWARD] = &worldBossRewardCfg->vvecReward[i][CHEST_KEY_REWARD];//钥匙
//		pVItemReward[HONOUR_REWARD] = &worldBossRewardCfg->vvecReward[i][HONOUR_REWARD];//声望
//		itemCfg0 = CONFIG::getItemCfg(pVItemReward[0]->itemIndex);
//		itemCfg1 = CONFIG::getItemCfg(pVItemReward[1]->itemIndex);
//		if (itemCfg1==NULL || itemCfg1==NULL )
//		{
//			continue;
//		}
//		id = _vecBossHurt[i].dwRoleID;
//		char szMsg[256];
//		sprintf( szMsg,(CONFIG:: getMsgBossHurtRank()).c_str(), i+1, pVItemReward[HONOUR_REWARD]->itemNums,itemCfg0->name.c_str(), pVItemReward[CHEST_REWARD]->itemNums,itemCfg1->name.c_str(), pVItemReward[CHEST_KEY_REWARD]->itemNums);
//		MailCtrl::sendMail(id, E_MAIL_TYPE_PRIZE, szMsg, worldBossRewardCfg->vvecReward[i]);
//		logwm("世界boss:%s",szMsg);
//	}
//
//	if ( _lastFightID != 0 )
//	{
//		vector<ITEM_INFO>& vecLastHurtItem = worldBossRewardCfg->vvecReward[LAST_HURT_REWARD];
//		pVItemReward[CHEST_REWARD] = &vecLastHurtItem[CHEST_REWARD];//宝箱
//		pVItemReward[CHEST_KEY_REWARD] = &vecLastHurtItem[CHEST_KEY_REWARD];//钥匙
//		pVItemReward[HONOUR_REWARD] = &vecLastHurtItem[HONOUR_REWARD];//声望
//		itemCfg0 = CONFIG::getItemCfg(pVItemReward[0]->itemIndex);
//		itemCfg1 = CONFIG::getItemCfg(pVItemReward[1]->itemIndex);
//		if (itemCfg1 != NULL || itemCfg1 != NULL )
//		{
//			char szMsg[256];
//			sprintf( szMsg,(CONFIG:: getMsgBossLastHurt()).c_str(), pVItemReward[HONOUR_REWARD]->itemNums,itemCfg0->name.c_str(), pVItemReward[CHEST_REWARD]->itemNums,itemCfg1->name.c_str(), pVItemReward[CHEST_KEY_REWARD]->itemNums);//暂用
//			MailCtrl::sendMail(id, E_MAIL_TYPE_PRIZE, szMsg, vecLastHurtItem );
//					logwm("世界boss:%s",szMsg);
//		}
//	}
//
//	for ( auto it : _vecBossHurt )
//	{
//		id = it.dwRoleID;
//		role = RoleMgr::getMe().getRoleByID( id );
//		if (role == NULL)
//		{
//			continue;
//		}
//
//		vecItemInfo.clear();
//		vecItemInfo.push_back( ITEM_INFO(ITEM_ID_HONOUR, JOIN_HONOUR_REWARD ) );
//
//		if ( !vecItemInfo.empty() )
//		{
//			char szMsg[256];
//			sprintf( szMsg,(CONFIG::getMsgBossFightJoin()).c_str(), JOIN_HONOUR_REWARD );
//			MailCtrl::sendMail(id, E_MAIL_TYPE_PRIZE, szMsg, vecItemInfo);
//					logwm("世界boss:%s",szMsg);
//		}
//
//		coin = DWORD( it.dwHurt/(bossHP * 0.0001f) * (1000.0f + 2000.0f * pow(role->wLevel,2.22f) / (role->wLevel + 1030.0f)) );
//		if (coin > 0)
//		{
//			ADDUP(role->dwCoin, (int)coin);
//			notifyTypeValuebyRoleID(role->dwRoleID, ROLE_PRO_COIN, role->dwCoin);
//			vecItemInfo.clear();
//			char szMsg2[256];
//			sprintf( szMsg2,(CONFIG::getMsgBossHurt()).c_str(),it.dwHurt,coin );
//			MailCtrl::sendMail(id, E_MAIL_TYPE_MSG, szMsg2, vecItemInfo);
//					logwm("世界boss:%s",szMsg2);
//		}
//	}
//}
//
//void ACTIVITY::setBossFightStatusNO( )
//{
//	_bossFightStatus = E_NOTICE_STATUS_NO;
//}
//
/////*
////@#define	 S_BOSS_GET_HURT_RANK		0x0806		//伤害排行
////*/
////int ACTIVITY::getBossFightRank( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	string strData;
////	DWORD selfRank = 0;
////	DWORD selfHurt = 0;
////	auto it = find( _vecBossHurt.begin(), _vecBossHurt.end(), role->dwRoleID );//bool opeartor == (const DWORD&	id)const{}
////	if ( it != _vecBossHurt.end() )
////	{
////		selfRank = it - _vecBossHurt.begin();
////		selfHurt = it->dwHurt;
////	}
////
////	S_APPEND_DWORD( strData, selfRank);
////	S_APPEND_DWORD( strData, selfHurt);
////
////	size_t nums = min<size_t>( 5, _vecBossHurt.size() );
////	for ( size_t i = 0; i < nums; i++ )
////	{
////		ROLE* rankOther = RoleMgr::getMe().getRoleByID( _vecBossHurt[i].dwRoleID );
////		if ( rankOther != NULL )
////		{
////			S_APPEND_DWORD( strData, rankOther->dwRoleID );
////			S_APPEND_DWORD( strData, _vecBossHurt[i].dwHurt );
////			S_APPEND_BYTE( strData, rankOther->bySex );
////			S_APPEND_BYTE( strData, rankOther->byJob );
////			S_APPEND_BSTR( strData, rankOther->roleName );
////		}
////	}
////
////	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HURT_RANK, strData) );
////	return 0;
////}