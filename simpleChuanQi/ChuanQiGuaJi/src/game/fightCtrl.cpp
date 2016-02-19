#include "log.h"
#include "base.h"
#include "protocal.h"
#include "db.h"
#include "itemCtrl.h"
#include "activity.h"
#include "fightCtrl.h"
#include "skill.h"
#include "arena.h"
#include "broadCast.h"
#include "roleCtrl.h"
#include "mercenary.h"
#include "arena.h"
#include "service.h"


#define  MAX_ARMY_NUM     3

#define  MAX_ROUND		  30

#define  NEW_MAP_OPEN     1



void TFightDrop::resetDrop()
{
	this->lstDropMonster.clear();
	this->byDropType = E_DROP_NONE;
	this->wDropMap = 0;
	this->wBossID = 0;
}

void TFightDrop::setDrop( list<WORD>& lstMons, const BYTE dropType, const WORD map, const WORD boss )
{
	this->lstDropMonster.assign( lstMons.begin(), lstMons.end() );
	this->byDropType = dropType;
	this->wDropMap = map;
	this->wBossID = boss;
}

namespace FightCtrl
{

	enum eEFFSTATUS
	{
		E_EFF_OFF = 0,		  
		E_EFF_CONTINUE = 1,		
		E_EFF_ON = 2,		  
	};

	enum eFIGHT
	{
		ENTER_MAP_SUCCESS = 0,     //成功
		ENTER_MAP_NO_EXISTS = 1,   //地图不存在
		ENTER_MAP_NOT_OPEN = 2,    //地图未开放
		ENTER_MAP_LOW_LEVEL = 3,   //角色等级未达到
		FIGHT_MON_NO_COOL = 4,     //冷却时间未到
		FIGHT_MON_NO_EXISTS = 5,   //本地图怪物不存在
		DROP_EQUIP_FULL = 6,   	   //装备背包已满
		DROP_PROPS_FULL = 7,        //道具背包已满
		ARENA_ROLE_NO_EXISTS = 8,  //竞技场对手不存在
		FIGHT_LESS_TIME = 9,  //次数不足
	};

	enum eTARGETYPE
	{
		E_TARGET_ONE = 1,//单体
		E_TARGET_ALL = 2,//全体

		E_TARGET_SELF = 1,//自己
		E_TARGET_ATK = 2,//攻击对象
	};


	struct SKILLCOOL
	{
		WORD    id;
		BYTE   cool;

		SKILLCOOL( const SKILLCOOL& dst )
		{
			this->id = dst.id;
			this->cool = dst.cool;
		}

		SKILLCOOL& operator = ( const SKILLCOOL& dst )
		{
			if ( this == &dst )
			{
				return *this;
			}

			this->id = dst.id;
			this->cool = dst.cool;

			return *this;
		}

		SKILLCOOL( WORD id, BYTE cool )
		{
			this->id = id;
			this->cool = cool;
		}
	};


	struct FIGHT_ATTR 
	{
		int	  attackLevel			;
		int   defendLevel			;
		float attackHit				;
		float attackCrit			;
		float attackLuck			;
		float defendDodge			;
		float defendCritres			;
		float defendLuck			;
		DWORD attackAttackMaxValue  ;
		DWORD attackAttackMinValue  ;
		DWORD defendDefMaxValue     ;
		DWORD defendDefMinValue     ;
		DWORD attackSkillAttackValue;
		DWORD defendSkillDefValue   ;
		
		ZERO_CONS(FIGHT_ATTR);
	};
	

	void  initSkills( ROLE* roleCfg, list< SKILLCOOL >& lstSkills )
	{
		for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)//提出快捷技能ids
		{
			auto& it = roleCfg->role_skill.vecIndexSkills[i];
			if (it.byStatus == 0)
			{
				continue;
			}
			
			CONFIG::SKILL_CFG2* skillCfg = CONFIG::getSkillCfg(it.skillid);
			if (skillCfg != NULL )
			{
				lstSkills.push_back( SKILLCOOL( it.skillid, 0/*(BYTE)skillCfg->cooldown*/ ));
			}
		}
	}

	void  initSkills( CMercenary* mercenaryCfg, list< SKILLCOOL >& lstSkills )
	{
		for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)//提出快捷技能ids
		{
			auto& it = mercenaryCfg->vIndexSkills[i];
			if (it.byStatus == 0)
			{
				continue;
			}

			CONFIG::SKILL_CFG2* skillCfg = CONFIG::getSkillCfg(it.skillid);
			if (skillCfg != NULL )
			{
				lstSkills.push_back( SKILLCOOL( it.skillid, 0/*(BYTE)skillCfg->cooldown */));
			}
		}
	}
	
	template <class ATTR_CFG>
	void  initAttr( DWORD* eleAttr, ATTR_CFG* attrCfg )
	{
		eleAttr[ E_ATTR_STAMINA	    ] = attrCfg->stamina	   ;
		eleAttr[ E_ATTR_STRENGTH	    ] = attrCfg->strength	   ;
		eleAttr[ E_ATTR_AGILITY	    ] = attrCfg->agility	   ;
		eleAttr[ E_ATTR_INTELLECT    ] = attrCfg->intellect	;
		eleAttr[ E_ATTR_SPIRIT		] = attrCfg->spirit		;
		eleAttr[ E_ATTR_HP_MAX2		] = attrCfg->hp_max		;
		eleAttr[ E_ATTR_ATK_MAX2	    ] = attrCfg->atk_max	   ;
		eleAttr[ E_ATTR_ATK_MIN2	    ] = attrCfg->atk_min	   ;
		eleAttr[ E_ATTR_DEF_MAX	    ] = attrCfg->def_max	   ;
		eleAttr[ E_ATTR_DEF_MIN	    ] = attrCfg->def_min	   ;
		eleAttr[ E_ATTR_SPELLATK_MAX ] = attrCfg->spellatk_max;
		eleAttr[ E_ATTR_SPELLATK_MIN ] = attrCfg->spellatk_min;
		eleAttr[ E_ATTR_SPELLDEF_MAX ] = attrCfg->spelldef_max;
		eleAttr[ E_ATTR_SPELLDEF_MIN ] = attrCfg->spelldef_min;
		eleAttr[ E_ATTR_MP_MAX2		] = attrCfg->mp_max		;
		eleAttr[ E_ATTR_HIT			] = attrCfg->hit		   ;
		eleAttr[ E_ATTR_DODGE		] = attrCfg->dodge		;
		eleAttr[ E_ATTR_CRIT		    ] = attrCfg->crit		   ;
		eleAttr[ E_ATTR_CRIT_RES	    ] = attrCfg->crit_res	   ;
		eleAttr[ E_ATTR_CRIT_INC	    ] = attrCfg->crit_inc	   ;
		eleAttr[ E_ATTR_CRIT_RED	    ] = attrCfg->crit_red	   ;
		eleAttr[ E_ATTR_MP_REC2		] = attrCfg->mp_rec		;
		eleAttr[ E_ATTR_HP_REC2		] = attrCfg->hp_rec		;
		eleAttr[ E_ATTR_LUCK		    ] = attrCfg->luck		   ;
		eleAttr[ E_ATTR_ATT_HEM		] = attrCfg->att_hem	   ;
		eleAttr[ E_ATTR_ATT_MANA	    ] = attrCfg->att_mana	   ;
		eleAttr[ E_LUCK_DEF			] = attrCfg->luck_def	;
	}

	class CFightEle
	{
	public:

		CFightEle( const CFightEle& dst )
		{
			clone( dst );
		}

		CFightEle( ROLE* role, const DWORD& hp_max, const DWORD& mp_max, const BYTE& pos )//人物
		{
			commonConstracter( role, hp_max, mp_max, pos );
			this->job = role->byJob;
			this->eleID = role->dwRoleID;
			this->level = role->wLevel;
			this->hp_rec = role->roleAttr[ E_ATTR_HP_REC2] ;
			this->mp_rec = role->roleAttr[ E_ATTR_MP_REC2]  ;
			memcpy( this->eleAttr, role->roleAttr, sizeof(this->eleAttr) );
			initSkills( role, this->lstSkills);
		}

		CFightEle( CONFIG::MONSTER_CFG* monCfg, const DWORD& hp_max, const DWORD& mp_max, const BYTE& pos )//小怪 守关boss
		{
			commonConstracter( monCfg, hp_max, mp_max, pos );
			this->job = 4;//暂定
			this->eleID = monCfg->id;
			this->level = monCfg->level;
			this->hp_rec = monCfg->hp_rec;
			this->mp_rec = monCfg->mp_rec;
			memset( this->eleAttr, 0, sizeof(this->eleAttr) );
			initAttr( eleAttr, monCfg);
			CONFIG::SKILL_CFG2* skillCfg = CONFIG::getSkillCfg(monCfg->skill_id);
			if (skillCfg != NULL )
			{
				this->lstSkills.push_back( SKILLCOOL( monCfg->skill_id, 0 ));
			}
		}

		CFightEle( CMercenary* mercenaryCfg, const DWORD& hp_max, const DWORD& mp_max, const BYTE& pos )//佣兵
		{
			commonConstracter( mercenaryCfg, hp_max, mp_max, pos );
			this->job = mercenaryCfg->byJob;
			this->eleID = mercenaryCfg->wID;
			this->level = mercenaryCfg->wLevel;
			this->hp_rec = mercenaryCfg->merAttr[ E_ATTR_HP_REC2] ;
			this->mp_rec = mercenaryCfg->merAttr[ E_ATTR_MP_REC2]  ;
			memcpy( this->eleAttr, mercenaryCfg->merAttr, sizeof(this->eleAttr) );
			initSkills( mercenaryCfg, this->lstSkills);
		}

		CFightEle( DWORD roleAttr[E_ATTR_MAX], DWORD boss_cur_hp, DWORD& boss_remain_hp, WORD level, WORD skill_id )//世界boss
		{
			this->bNotAtk = false;
			this->bCommonAtk = false;
			this->bDoubleAtk = false;
			this->job = 0;
			this->hp_max = boss_cur_hp;
			this->mp_max = 0;
			this->mp_cost = 0;
			this->pos = pos;
			this->eleID = 0;
			this->level = level;
			this->dwFightValue = 0;
			this->hp_rec = 0;
			this->mp_rec = 0;
			memcpy( this->eleAttr, roleAttr, sizeof(this->eleAttr) );

			CONFIG::SKILL_CFG2* skillCfg = CONFIG::getSkillCfg( skill_id );
			if (skillCfg != NULL )
			{
				this->lstSkills.push_back( SKILLCOOL( skill_id, 0/*(BYTE)skillCfg->cooldown*/ ));
			}
		}

		CFightEle& operator = ( const CFightEle& dst )
		{
			if ( this == &dst )
			{
				return *this;
			}

			clone( dst );

			return *this;
		}

		DWORD eleID;
		DWORD eleAttr[E_ATTR_MAX];
		DWORD hp_max;//剩余血量
		DWORD mp_max;//剩余魔法值
		DWORD hp_rec;
		DWORD mp_rec;
		DWORD mp_cost;
		DWORD dwFightValue;
		BYTE  pos;
		BYTE  job;
		WORD level;
		bool bNotAtk;
		bool bCommonAtk;
		bool bDoubleAtk;
		list< EFFECT_ROUND > lstEffRound;//进攻时候需要检查的buf
		list< EFFECT_ROUND > lstDefEffRound;//防守时候需要检查的buf
		list< SKILLCOOL > lstSkills;
		vector< SKILL_EFFECT > vecEffectAttr;
		vector< SKILL_EFFECT > vecDoubleEffectAttr;

	private:
		template<class  T>
		void commonConstracter(T* ele, const DWORD& hp_max, const DWORD& mp_max, const BYTE& pos )
		{
			this->bNotAtk = false;
			this->bCommonAtk = false;
			this->bDoubleAtk = false;
			this->hp_max = hp_max;
			this->mp_max = mp_max;
			this->mp_cost = 0;
			this->pos = pos;
			this->dwFightValue = ele->dwFightValue;
		}

		void clone( const CFightEle& dst )
		{
			this->eleID = dst.eleID;
			memcpy( this->eleAttr, dst.eleAttr, sizeof( this->eleAttr ) );
			this->hp_max = dst.hp_max;
			this->mp_max = dst.mp_max;
			this->hp_rec = dst.hp_rec;
			this->mp_rec = dst.mp_rec;
			this->mp_cost = dst.mp_cost;
			this->dwFightValue = dst.dwFightValue;
			this->pos = dst.pos;
			this->job = dst.job;
			this->level = dst.level;
			this->bNotAtk = dst.bNotAtk;
			this->bCommonAtk = dst.bCommonAtk;
			this->bDoubleAtk = dst.bDoubleAtk;
			this->lstEffRound = dst.lstEffRound;
			this->lstDefEffRound = dst.lstDefEffRound;
			this->lstSkills = dst.lstSkills;
			this->vecEffectAttr = dst.vecEffectAttr;
			this->vecDoubleEffectAttr = dst.vecDoubleEffectAttr;
		}
	};



	BYTE clientEnterMap( ROLE* role, const WORD& wMapIndex );

	BYTE clientFightMap( ROLE* role, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_SEQ>& fightSeqs, BYTE& byResult, WORD wMonID = 0 );

	BYTE clientFightPlayer( ROLE* role, int type, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_SEQ>& fightSeqs, BYTE& byResult, DWORD dwComID );

	BYTE meet_battle( ROLE* role, int type, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_SEQ>& fightSeqs, BYTE& byResult, DWORD dwComID );

	BYTE clientStartFight( vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef );

	BYTE clientStartFight( vector<CFightEle> vecAtk, vector<CFightEle> vecDef, vector<FIGHT_SEQ>& fightSeqs, BYTE& byResult );

	void recHpMpEachRound( vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef );

	void recHpMpEachRoundOneSide( vector<CFightEle>& vecAtk );

	BYTE clientReportMap( ROLE* role, vector<DROP_SEQ>& dropSeqs, const WORD& mapID, const BYTE& dropType, DWORD* pvAdd );

	void initFightTeam( ROLE* role, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, DWORD com_id );

	void initFightTeam( ROLE* role, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, WORD wMonID );

	void initFightTeam( ROLE* role, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, DWORD roleAttr[E_ATTR_MAX], DWORD boss_cur_hp, DWORD& boss_remain_hp, WORD level, WORD skill_id  );

	int getAtkValue( CFightEle atk, CFightEle def, CONFIG::SKILL_CFG2* skillCfg, BYTE& hit, BYTE&crit, BYTE&luck, BYTE& luckDefend );

	void reduceSkillsCool( list<SKILLCOOL>& lstSkills );

	int atkAtkDef( CFightEle& attack, CONFIG::SKILL_CFG2* skillCfg,  vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_HURT>& victimSeq );

	int getAtkValue( BYTE& hit, BYTE&crit, BYTE&luck, BYTE& luckDefend, const FIGHT_ATTR& stFightAttr, CONFIG::SKILL_CFG2* skillCfg );

	void setSkillEffect( CFightEle& attack, CONFIG::SKILL_CFG2* skillCfg );

	void getAtkEffect( CFightEle& attack, vector<SELF_EFFECT_NET>& vecSelfEff );

	void getDefEffect( CFightEle& def, int& atkValue, vector<EFFECT_NET>& vecEff );

	int getTreatValue( const DWORD& dwSkillAtk, CONFIG::SKILL_CFG2* skillCfg );

	
	CONFIG::SKILL_CFG2* getFightSkill( CFightEle& attack, bool bComSkill = false )
	{
		CONFIG::SKILL_CFG2* skillCfg = NULL;

		WORD skillID = 0;

		bool bSkill = false;

		if ( !bComSkill )
		{
			for ( auto& it : attack.lstSkills )
			{
				skillCfg = CONFIG::getSkillCfg( it.id );
				if ( skillCfg != NULL && skillCfg->mp_cost <= attack.mp_max && it.cool == 0 )
				{
					attack.mp_cost = skillCfg->mp_cost;

					bSkill = true;

					it.cool = (BYTE)skillCfg->cooldown;

					break;
				}
			}
		}


		if ( !bSkill )
		{
			if (attack.job == E_JOB_WARRIOR)//特殊技能
			{
				skillID = FIGHT_SKILL_WARRIOR;
			}
			if (attack.job  == E_JOB_MAGIC)
			{
				skillID = FIGHT_SKILL_MAGIC;
			}
			if (attack.job == E_JOB_MONK)
			{
				skillID = FIGHT_SKILL_MONK;
			}

			skillCfg = CONFIG::getSkillCfg( skillID );
			if ( skillCfg != NULL  )
			{
				attack.mp_cost = 0;
			}
		}

		return skillCfg;
	}
}

int FightCtrl::getTreatValue( const DWORD& dwSkillAtk, CONFIG::SKILL_CFG2* skillCfg )
{
	//加血血量=技能攻击/10*（技能等级*3+1）
	int nAtkValue = int(dwSkillAtk * ( skillCfg->level * 3 + 1 ) / 10);
	return int( nAtkValue * skillCfg->skillPara.skillCoff + skillCfg->skillPara.skillAtt );
}


//#define S_ENTER_MAP				0x0050//--进地图协议
int FightCtrl::clientEnterMap( ROLE* role, unsigned char * data, size_t dataLen )
{
	WORD wMapIndex;
	if ( !BASE::parseWORD( data, dataLen, wMapIndex) )
		return 0;

	RETURN_COND( wMapIndex > role->wCanFightMaxMapID, 0 );//精英关卡  //上面条件判断

	string strData;

	BYTE byRet = clientEnterMap( role, wMapIndex );

	S_APPEND_BYTE(strData, byRet);

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ENTER_MAP, strData) );
	return 0;
}


//#define S_FIGHT_END_REPORT		0x0056//战斗结束
int FightCtrl::clientFightEnd( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byType;

	if ( !BASE::parseBYTE( data, dataLen, byType) )
		return 0;

	string strData;

	vector<DROP_SEQ> dropSeqs;
	DWORD vAdd[3] = {0};
	if (byType == E_FIGHT_ARENA_PLAYER)
	{
		ARENA::fight_report( role, role->dwComID, role->byArenaResult, vAdd);
	}
	else if (byType == E_FIGHT_MEET_PLAYER)
	{
		role->fight_report_for_meet(role->meet_pk_target_roleid, role->byArenaResult, dropSeqs);
	}
	else
	{
		clientReportMap( role, dropSeqs, role->stFightDrop.wDropMap, role->stFightDrop.byDropType, vAdd );
	}

	S_APPEND_NBYTES(strData, vAdd, sizeof(vAdd) );

	S_APPEND_BYTE(strData, dropSeqs.size());

	for (auto it : dropSeqs )
	{
		S_APPEND_NBYTES(strData, &it, sizeof(DROP_SEQ) );
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_END_REPORT, strData) );

	role->stFightDrop.resetDrop();
	
	return 0;
}


//#define S_FIGHT_PROCESS  		0x0051//注意原来的需要改掉
int FightCtrl::clientFightRequest( ROLE* role, unsigned char * data, size_t dataLen )
{
	//enum eFIGHT
	//{
	//	ENTER_MAP_SUCCESS = 0,     //成功
	//	ENTER_MAP_NO_EXISTS = 1,   //地图不存在
	//	ENTER_MAP_NOT_OPEN = 2,    //地图未开放
	//	ENTER_MAP_LOW_LEVEL = 3,   //角色等级未达到
	//	FIGHT_MON_NO_COOL = 4,     //冷却时间未到
	//	FIGHT_MON_NO_EXISTS = 5,   //本地图怪物不存在
	//	DROP_ITEM_FULL = 6,   	   //背包已满
	//	DROP_MINE_FULL = 7,        //矿石背包已满
	//};


	BYTE byType;
	WORD wMapID;//地图ID
	DWORD dwComID;//竞技场对手

	if ( !BASE::parseBYTE( data, dataLen, byType) )
		return 0;

	if ( !BASE::parseDWORD( data, dataLen, dwComID) )
		return 0;

	string strData;
	BYTE byRet;
	BYTE byResult = E_MATCH_STATUS_LOSS;
	WORD wTime = 0;
	vector<CFightEle> vecAtk;
	vector<CFightEle> vecDef;
	vector<FIGHT_SEQ> fightSeqs;
	
	DWORD tNow = (DWORD)PROTOCAL::getCurTime();
	
	switch ( byType )
	{
		case E_FIGHT_MEET_PLAYER:
			{
				wTime = 5;//客户端需要的
				byRet = meet_battle( role, byType, vecAtk, vecDef, fightSeqs, byResult, dwComID );
				role->byArenaResult = byResult;
			}
			break;

		case E_FIGHT_ARENA_PLAYER://无掉落 无冷却
			{
				RETURN_COND(MAX_ARENA_USE_NUMS + role->byBuyArenaDayCount <= role->byArenaDayCount, 0);
				wTime = 5;//客户端需要的
				byRet = clientFightPlayer( role, byType, vecAtk, vecDef, fightSeqs, byResult, dwComID );
				role->byArenaResult = byResult;
			}
			break;


		case E_FIGHT_MON:
			{
				COND_BREAK( tNow < role->dwCanFightMapTime && role->dwCanFightMapTime != 0, byRet, FIGHT_MON_NO_COOL );

				CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg( role->wLastFightMapID );

				COND_BREAK( mapCfg == NULL, byRet, ENTER_MAP_NO_EXISTS);

				wTime = (WORD)mapCfg->cool_down;

				role->dwCanFightMapTime = tNow + mapCfg->cool_down;

				role->lstMonsterIndex.clear();//重新刷小怪

				int nums = BASE::randBetween( mapCfg->dwMin, mapCfg->dwMax+1 );
	
				for ( int i=0; i<nums; i++ )
				{
					RETURN_COND( mapCfg->vecMonsterRandSum.empty() , 0 );

					int index = BASE::getIndexOfVectorSum( mapCfg->vecMonsterRandSum );

					WORD monsIndex = mapCfg->vecMonsterIndex[index];

					role->lstMonsterIndex.push_back( monsIndex );
				}

				byRet = clientFightMap( role, vecAtk, vecDef, fightSeqs, byResult, (WORD)dwComID );

				if ( byResult == E_MATCH_STATUS_WIN )
				{
					role->stFightDrop.setDrop( role->lstMonsterIndex, E_DROP_MON, role->wLastFightMapID );
				}
			}
			break;

		case E_FIGHT_HERO:
		case E_FIGHT_BOSS:
			{
				wMapID = (WORD)dwComID;//传地图ID

				WORD wWinMaxID = 0;

				CONFIG::MAP_CFG* mapCfg  = CONFIG::getMapCfg( wMapID );

				COND_BREAK( mapCfg == NULL, byRet, ENTER_MAP_NO_EXISTS);

				COND_BREAK( tNow < role->dwCanFightMapTime && role->dwCanFightMapTime != 0, byRet, FIGHT_MON_NO_COOL );

				if(byType == E_FIGHT_HERO)
				{
					COND_BREAK( role->byAllBuyHeroTimes + role->bySendFightHeroTimes == 0, byRet, FIGHT_MON_NO_COOL );

					COND_BREAK( mapCfg->level_limit > role->wLevel, byRet, ENTER_MAP_NOT_OPEN );

					auto it = role->mapElite.find( mapCfg->group_id );

					COND_BREAK( it == role->mapElite.end(), byRet, ENTER_MAP_NO_EXISTS);

					COND_BREAK( wMapID > it->second.wBase, byRet, ENTER_MAP_NO_EXISTS);

					wWinMaxID = it->second.wExtra;
				}
				else
				{
					COND_BREAK( role->byAllBuyBossTimes + role->bySendFightBossTimes == 0, byRet, FIGHT_MON_NO_COOL );

					wWinMaxID = role->wWinMaxMapID;
				}

				wTime = (WORD)mapCfg->cool_down;

				role->dwCanFightMapTime = tNow + mapCfg->cool_down;

				byRet = clientFightMap( role, vecAtk, vecDef, fightSeqs, byResult, mapCfg->boss_id );

				if ( byResult == E_MATCH_STATUS_WIN )
				{
					BYTE byDropType = wMapID <= wWinMaxID ? E_DROP_BOSS : E_DROP_FIRST_BOSS;

					role->stFightDrop.setDrop( role->lstMonsterIndex, byDropType, wMapID, mapCfg->boss_id );
				}
			}
			break;

		default: 
			return 0;
	}

	S_APPEND_BYTE(strData, byRet);

	S_APPEND_BYTE(strData, byType);

	S_APPEND_BYTE(strData, byResult);

	S_APPEND_WORD(strData, wTime);

	S_APPEND_WORD(strData, vecAtk.size());
	for (size_t i = 0; i < vecAtk.size(); i++)
	{
		S_APPEND_DWORD(strData, vecAtk[i].eleID);
		S_APPEND_BYTE(strData, vecAtk[i].pos);
		S_APPEND_WORD(strData, 1);
		S_APPEND_DWORD(strData, vecAtk[i].hp_max);
		S_APPEND_DWORD(strData, vecAtk[i].mp_max);
		S_APPEND_DWORD(strData, vecAtk[i].dwFightValue);
		S_APPEND_DWORD(strData, vecAtk[i].hp_rec);
		S_APPEND_DWORD(strData, vecAtk[i].mp_rec);
	}

	S_APPEND_WORD(strData, vecDef.size());
	for (size_t i = 0; i < vecDef.size(); i++ )
	{
		S_APPEND_DWORD(strData, vecDef[i].eleID);
		S_APPEND_BYTE(strData, vecDef[i].pos);
		S_APPEND_WORD(strData, 1);
		S_APPEND_DWORD(strData, vecDef[i].hp_max);
		S_APPEND_DWORD(strData, vecDef[i].mp_max);
		S_APPEND_DWORD(strData, vecDef[i].dwFightValue);
		S_APPEND_DWORD(strData, vecDef[i].hp_rec);
		S_APPEND_DWORD(strData, vecDef[i].mp_rec);
	}

	S_APPEND_BYTE(strData, fightSeqs.size());

	for (auto& it : fightSeqs )
	{
		S_APPEND_BYTE(strData, it.pos);

		S_APPEND_BYTE(strData, it.vecSelfEff.size());
		for ( auto& selfeff : it.vecSelfEff )
		{
			S_APPEND_NBYTES(strData, &selfeff, sizeof( SELF_EFFECT_NET ) );
		}

		S_APPEND_WORD(strData, it.skillID);
		S_APPEND_BYTE(strData, it.round);
		S_APPEND_DWORD(strData, it.leftHp);
		S_APPEND_DWORD(strData, it.leftMp);

		S_APPEND_BYTE(strData, it.victimSeq.size());
		for ( auto& fight : it.victimSeq )
		{
			S_APPEND_BYTE(strData, fight.pos);
			S_APPEND_DWORD(strData, fight.leftHp);
			S_APPEND_DWORD(strData, fight.leftMp);
			S_APPEND_INT32(strData, fight.hurtValue);
			S_APPEND_INT32(strData, fight.mpValue);
			S_APPEND_BYTE(strData, fight.hit);
			S_APPEND_BYTE(strData, fight.crit);
			S_APPEND_BYTE(strData, fight.luck);
			S_APPEND_BYTE(strData, fight.bydouble);
			S_APPEND_BYTE(strData, fight.vecEff.size());
			for ( auto& eff : fight.vecEff )
			{
				S_APPEND_NBYTES(strData, &eff, sizeof( EFFECT_NET ) );

			}
		}
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_PROCESS, strData) );
	return 0;

}


BYTE FightCtrl::clientEnterMap( ROLE* role, const WORD& wMapIndex/*, vector<WORD>& monIDs*/ )
{
	BYTE byRet = (BYTE )ItemCtrl::setMapMonster( role, wMapIndex );	
	if (  byRet != 0  )
	{
		return byRet;
	}

	//进地图清 战斗统计用
	memset(role->predictEarning, 0, sizeof( role->predictEarning ));

	role->byNewMapKillMonNum = 0;

	return ENTER_MAP_SUCCESS;
}



void FightCtrl::initFightTeam( ROLE* role, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, DWORD roleAttr[E_ATTR_MAX], DWORD boss_cur_hp, DWORD& boss_remain_hp, WORD level, WORD skill_id  )
{
	BYTE pos = 0;

	vecAtk.push_back( CFightEle(role, role->roleAttr[E_ATTR_HP_MAX2], role->roleAttr[E_ATTR_MP_MAX2], pos) );
	for (auto& it : role->cMerMgr.m_vecCMercenary )
	{
		if ( it.byFight == E_FIGHT_ON )
		{
			vecAtk.push_back( CFightEle( &it, it.merAttr[E_ATTR_HP_MAX2], it.merAttr[E_ATTR_MP_MAX2], pos + 1));
		}
	}

	pos = MAX_ARMY_NUM;


	vecDef.push_back( CFightEle( roleAttr,  boss_cur_hp, boss_remain_hp, level, skill_id  ) );

}

void FightCtrl::initFightTeam( ROLE* role, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, DWORD com_id )
{
	BYTE pos = 0;

	vecAtk.push_back( CFightEle(role, role->roleAttr[E_ATTR_HP_MAX2], role->roleAttr[E_ATTR_MP_MAX2], pos) );
	for (auto& it : role->cMerMgr.m_vecCMercenary )
	{
		if ( it.getFightStatus() == E_FIGHT_ON )
		{
			vecAtk.push_back( CFightEle( &it, it.merAttr[E_ATTR_HP_MAX2], it.merAttr[E_ATTR_MP_MAX2], pos + 1));
		}
	}

	pos = MAX_ARMY_NUM;

	ROLE* comRole = RoleMgr::getMe().getRoleByID( com_id );

	if (comRole != NULL )
	{
		vecDef.push_back( CFightEle(comRole, comRole->roleAttr[E_ATTR_HP_MAX2], comRole->roleAttr[E_ATTR_MP_MAX2], pos) );
		for (auto& it : comRole->cMerMgr.m_vecCMercenary )
		{
			if ( it.getFightStatus() == E_FIGHT_ON )
			{
				vecDef.push_back( CFightEle( &it, it.merAttr[E_ATTR_HP_MAX2], it.merAttr[E_ATTR_MP_MAX2], pos + 1));
			}
		}
	}

}

void FightCtrl::initFightTeam( ROLE* role, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, WORD wMonID )
{
	BYTE pos = 0;

	vecAtk.push_back( CFightEle(role, role->roleAttr[E_ATTR_HP_MAX2], role->roleAttr[E_ATTR_MP_MAX2], pos));

	for (auto& it : role->cMerMgr.m_vecCMercenary )
	{
		if ( it.getFightStatus() == E_FIGHT_ON )
		{
			vecAtk.push_back( CFightEle( &it, it.merAttr[E_ATTR_HP_MAX2], it.merAttr[E_ATTR_MP_MAX2], pos + 1));
		}
	}

	pos = MAX_ARMY_NUM;

	list<WORD> lstMon( role->lstMonsterIndex );

	if ( wMonID != 0 )//boss
	{
		lstMon.clear();
		lstMon.push_back( wMonID );
	}
	
	for ( auto& it : lstMon )
	{
		CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( it );

		if ( monCfg != NULL )
		{
			vecDef.push_back( CFightEle( monCfg, monCfg->hp_max, monCfg->mp_max, pos ) );

			pos++;

			if( (pos > MAX_ARMY_NUM*2) )
			{
				break;
			}
		}
	}
}

#define  COND_DIE( hp, vecArmy, index, victimNum ) \
	if( hp == 0 )\
	{\
		vecArmy.erase( vecArmy.begin() + index );\
		i--;\
		REDUCEUNSIGNED(victimNum, 1);\
	}


////敌人序列不会为空
//int FightCtrl::roleAtkRole( ROLE* role, vector< CMercenary >& vecEnemy, CONFIG::SKILL_CFG2* skillCfg, vector<FIGHT_HURT>& victimSeq )
//{
//	BYTE victimNum = 1;
//	//if ( skillCfg->type = 3 )
//	//{
//	//	victimNum = vecEnemy.size();
//	//}
//
//	for (size_t i = 0; i < victimNum; i++)
//	{
//		BYTE hit = 0;
//		BYTE crit = 0;
//		BYTE luckAttack = 0;
//		BYTE luckDefend = 0;
//		int attackValue = 0;
//		int mpValue = 0;
//		BYTE statu = 0;
//		BYTE round = 0;
//
//		CMercenary& defend = vecEnemy[ i ];//打最后一个
//
//		attackValue =  getRoleAtkRoleValue(role, defend.roleCfg, skillCfg,  hit, crit, luckAttack, luckDefend );//+ - 血量值
//		victimSeq.push_back( FIGHT_HURT( defend.pos, attackValue, mpValue, hit, crit, luckAttack, statu, round ) );
//
//		ADDUP(defend.hp_max, attackValue);
//
//		COND_DIE(defend.hp_max, vecEnemy, i);
//	}
//
//	return 0;
//}

//敌人序列不会为空
int FightCtrl::atkAtkDef( CFightEle& attack, CONFIG::SKILL_CFG2* skillCfg, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_HURT>& victimSeq )
{
	RETURN_COND( vecDef.empty(), 0 );

	size_t victimNum = 1;//目标数量
	size_t startVictim = 0;//开始目标位置
	BYTE hit = 0;
	BYTE crit = 0;
	BYTE luckAttack = 0;
	BYTE luckDefend = 0;
	int  attackValue = 0;
	int  mpValue = 0;

	if ( skillCfg->skillPara.skillTarget == E_TARGET_ALL )//全体目标
	{
		victimNum = vecDef.size();
	}
	else if ( skillCfg->skillPara.skillTarget == E_TARGET_ONE )//只打一个随机选一个目标
	{
		startVictim = BASE::randTo( vecDef.size() );

		victimNum = startVictim + 1;
	}

	
	if ( skillCfg->target == E_TARGET_SELF )//给自身添加效果
	{
		setSkillEffect( attack, skillCfg );
	}


	if ( skillCfg->type == E_SKILL_TYPE_TREAT )//治疗
	{
		attackValue = getTreatValue( attack.eleAttr[E_ATTR_SPELLATK_MAX], skillCfg );

		for ( /*int i = 0*/; startVictim < victimNum; startVictim++)
		{
			CFightEle& defend = vecAtk[ startVictim ];

			vector< EFFECT_NET > vecEff;

			if ( skillCfg->target == E_TARGET_ATK )
			{
				setSkillEffect( defend, skillCfg );
			}

			ADDUP(defend.hp_max, attackValue);

			victimSeq.push_back( FIGHT_HURT( defend.pos, defend.hp_max, defend.mp_max, attackValue, mpValue, hit, crit, luckAttack, vecEff ) );
		}
	}
	else//攻击
	{
		for ( /*int i = 0*/; startVictim < victimNum; startVictim++)
		{
			CFightEle& defend = vecDef[ startVictim ];

			vector< EFFECT_NET > vecEff;

			if ( skillCfg->target == E_TARGET_ATK )
			{
				setSkillEffect( defend, skillCfg );
			}

			attackValue =  getAtkValue(attack, defend, skillCfg,  hit, crit, luckAttack, luckDefend );//+ - 血量值

			getDefEffect( defend, attackValue, vecEff );

			if ( skillCfg->type == E_SKILL_TYPE_LESSMP )//攻击吸蓝类型
			{
				mpValue = int(attack.mp_max * skillCfg->skillPara.skillMpCoff + skillCfg->skillPara.skillMpAtt);
				ADDUP( attack.mp_max, mpValue );
				ADDUP( defend.mp_max, -mpValue );
			}

			ADDUP(defend.hp_max, attackValue);

			victimSeq.push_back( FIGHT_HURT( defend.pos, defend.hp_max, defend.mp_max, attackValue, mpValue, hit, crit, luckAttack, vecEff ) );

			if ( attack.bDoubleAtk )
			{
				vecEff.clear();
				attackValue = int(attackValue * skillCfg->extra_coff);
				victimSeq.push_back( FIGHT_HURT(  defend.pos, defend.hp_max, defend.mp_max, attackValue, mpValue, hit, crit, luckAttack, vecEff , 1 ) );
			}
		}
	}


	for ( auto it = vecDef.begin(); it != vecDef.end(); )//判断是否有死掉的
	{
		if (it->hp_max == 0)
		{
			it = vecDef.erase(it);
		}
		else
		{
			it++;
		}
	}

	return 0;
}

////敌人序列不会为空
//int FightCtrl::atkOnceAtkDef( CFightEle& attack, CONFIG::SKILL_CFG2* skillCfg, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_HURT>& victimSeq )
//{
//	vector< EFFECT_NET > vecEff;
//
//	setSkillEffect( attack,  skillCfg );
//
//	//攻击目标数
//	WORD victimNum = 1;//单体
//	if ( skillCfg->skillPara.skillTarget == E_TARGET_ALL )//全体
//	{
//		victimNum = vecDef.size();
//	}
//
//	BYTE hit = 0;
//	BYTE crit = 0;
//	BYTE luckAttack = 0;
//	BYTE luckDefend = 0;
//	int attackValue = 0;
//	int mpValue = 0;
//
//	//技能类型
//	if ( skillCfg->type == E_SKILL_TYPE_TREAT )
//	{
//		attackValue = getTreatValue( attack.eleAttr[E_ATTR_SPELLATK_MAX], skillCfg );
//
//		for (size_t i = 0; i < victimNum; i++)
//		{
//			CFightEle& defend = vecAtk[ i ];
//
//			victimSeq.push_back( FIGHT_HURT( defend.pos, attackValue, mpValue, hit, crit, luckAttack, vecEff ) );
//
//			ADDUP(defend.hp_max, attackValue);
//		}
//	}
//	else
//	{
//		for (size_t i = 0; i < victimNum; i++)
//		{
//			CFightEle& defend = vecDef[ i ];
//
//			attackValue =  getAtkValue(attack, defend, skillCfg,  hit, crit, luckAttack, luckDefend );//+ - 血量值
//
//			if ( skillCfg->type == E_SKILL_TYPE_LESSMP )
//			{
//				mpValue = int(attack.mp_max * skillCfg->skillPara.skillMpCoff + skillCfg->skillPara.skillMpAtt);
//				ADDUP( attack.mp_max, mpValue );
//				ADDUP( defend.mp_max, mpValue );
//			}
//
//			victimSeq.push_back( FIGHT_HURT( defend.pos, attackValue, mpValue, hit, crit, luckAttack, vecEff ) );
//
//			ADDUP(defend.hp_max, attackValue);
//
//			COND_DIE(defend.hp_max, vecDef, i, victimNum);
//
//		}
//	}
//
//	return 0;
//}


int FightCtrl::getAtkValue( BYTE& hit, BYTE&crit, BYTE&luck, BYTE& luckDefend, const FIGHT_ATTR& stFightAttr, CONFIG::SKILL_CFG2* skillCfg )
{
	int	  attackLevel			 = stFightAttr.attackLevel			;
	int   defendLevel			 = stFightAttr.defendLevel			;
	float attackHit				 = stFightAttr.attackHit			;
	float attackCrit			 = stFightAttr.attackCrit			;
	float attackLuck			 = stFightAttr.attackLuck			;
	float defendDodge			 = stFightAttr.defendDodge			;
	float defendCritres			 = stFightAttr.defendCritres		;
	float defendLuck			 = stFightAttr.defendLuck			 ;
	DWORD attackAttackMaxValue   = stFightAttr.attackAttackMaxValue  ;
	DWORD attackAttackMinValue   = stFightAttr.attackAttackMinValue  ;
	DWORD defendDefMaxValue      = stFightAttr.defendDefMaxValue     ;
	DWORD defendDefMinValue      = stFightAttr.defendDefMinValue     ;
	DWORD attackSkillAttackValue = stFightAttr.attackSkillAttackValue;
	DWORD defendSkillDefValue    = stFightAttr.defendSkillDefValue   ;

	attackSkillAttackValue = DWORD(attackSkillAttackValue * skillCfg->skillPara.skillCoff) /*+ skillCfg->skillPara.skillAtt*/;//技能攻击的附加值和系数

	int hurtValue = 0;
	int levelDiff = attackLevel - defendLevel;
	if ( levelDiff < -10 )
	{
		levelDiff = -10;
	}
	if ( levelDiff > 10 )
	{
		levelDiff = 10;
	}
	float attackHitRate = attackHit/(100.0f + attackLevel * 150.0f);//A命中率
	float defDodgeRate = defendDodge/(100.0f + defendLevel * 150.0f);//B闪避率
	float succRatio = ( levelDiff * 0.02f + (attackHitRate - defDodgeRate) + 0.90f );//最终命中率
	if (succRatio < 0.3f)
	{
		succRatio = 0.3f;
	}
	if (succRatio > 1.0f)
	{
		succRatio = 1.0f;
	}	
	if ( RAND_HIT( succRatio ) )//是否命中
	{
		hit = 1;	
		float attackCritRate = attackCrit/(100.0f + attackLevel * 150.0f);//A暴击率
		float defCritDefRate = defendCritres/(100.0f + defendLevel * 150.0f);//B暴击抗率
		float critRatio = levelDiff * 0.02f + (attackCritRate - defCritDefRate);//暴击率
		if (critRatio < 0.05f)
		{
			critRatio = 0.05f;
		}
		if (critRatio > 0.75f)
		{
			critRatio = 0.75f;
		}

		if ( RAND_HIT( critRatio) )//是否暴击
		{
			crit = 1;
		}

		float* pLuckRatio = CONFIG::getLuckCfg( (DWORD)attackLuck );
		float luckRatio = ( pLuckRatio != NULL ) ? (*pLuckRatio) : 0.0f;
		if ( RAND_HIT( luckRatio) )//幸运攻击
		{
			luck = 1;
		}

		float* pluckDefRatio = CONFIG::getLuckCfg( (DWORD)defendLuck );
		float luckDefRatio = ( pluckDefRatio != NULL ) ? (*pluckDefRatio) : 0.0f;
		if ( RAND_HIT( luckDefRatio) )//幸运防御
		{
			luckDefend = 1;
		}

		DWORD attackAttackValue = attackAttackMaxValue;//普通攻击值
		DWORD defendDefValue = defendDefMaxValue;//普通防御值

		if (luck == 0)
		{
			attackAttackValue = (BASE::randBetween( attackAttackMinValue, attackAttackMaxValue + 1 ));
		}

		if (luckDefend == 0)
		{
			defendDefValue = (BASE::randBetween(defendDefMinValue, defendDefMaxValue + 1 ));
		}

		if ( skillCfg->type == E_SKILL_TYPE_GENERAL )//普通攻击
		{
			attackSkillAttackValue = 0;
			defendSkillDefValue = 0;
		}
		else
		{
			attackAttackValue = DWORD(attackAttackValue * skillCfg->skillPara.skillCoff) + skillCfg->skillPara.skillAtt;//技能攻击的附加值和系数
		}

		if (crit == 1)
		{
			attackAttackValue = DWORD( (float)attackAttackValue * 1.5f );
			attackSkillAttackValue = DWORD( (float)attackSkillAttackValue * 1.5f );
		}

		int nHurtValue = 0;

		int nAtkValue = int( ( attackAttackValue >= defendDefValue ) ? ( attackAttackValue - defendDefValue ) : 0 );

		int nSkillAtkValue = int( ( attackSkillAttackValue - defendSkillDefValue ) ? ( attackSkillAttackValue - defendSkillDefValue ) : 0 );

		if ( ( nAtkValue + nSkillAtkValue ) < ( attackLevel * 2 + 2 ) )//保证最小伤害
		{
			nHurtValue = attackLevel * 2 + 2;
		}
		else
		{
			nHurtValue = nAtkValue + nSkillAtkValue;
		}

		hurtValue = -nHurtValue;
	}

	return hurtValue;
}

int FightCtrl::getAtkValue( CFightEle atk, CFightEle def, CONFIG::SKILL_CFG2* skillCfg, BYTE& hit, BYTE&crit, BYTE&luck, BYTE& luckDefend )
{
	FIGHT_ATTR stFightAttr;

	stFightAttr.attackLevel			  = (int)atk.level;
	stFightAttr.defendLevel			  = (int)def.level;
	stFightAttr.attackHit			  = (float)atk.eleAttr[E_ATTR_HIT];
	stFightAttr.attackCrit			  = (float)atk.eleAttr[E_ATTR_CRIT];
	stFightAttr.attackLuck			  = (float)atk.eleAttr[E_ATTR_LUCK];
	stFightAttr.defendDodge			  = (float)def.eleAttr[E_ATTR_DODGE];
	stFightAttr.defendCritres		  = (float)def.eleAttr[E_ATTR_CRIT_RES];
	stFightAttr.defendLuck			  = (float)def.eleAttr[E_LUCK_DEF];
	stFightAttr.attackAttackMaxValue  = atk.eleAttr[E_ATTR_ATK_MAX2];
	stFightAttr.attackAttackMinValue  = atk.eleAttr[E_ATTR_ATK_MIN2];
	stFightAttr.defendDefMaxValue     = def.eleAttr[E_ATTR_DEF_MAX];
	stFightAttr.defendDefMinValue     = def.eleAttr[E_ATTR_DEF_MIN];
	stFightAttr.attackSkillAttackValue= atk.eleAttr[E_ATTR_SPELLATK_MAX];
	stFightAttr.defendSkillDefValue   = def.eleAttr[E_ATTR_SPELLDEF_MAX];

	int nAtkValue =  getAtkValue( hit, crit, luck, luckDefend, stFightAttr, skillCfg );
	
	return nAtkValue;
	//return int( nAtkValue * skillCfg->skillPara.skillCoff + skillCfg->skillPara.skillAtt );
}

//int FightCtrl::getRoleAtkRoleValue( ROLE* role, ROLE* comRole, CONFIG::SKILL_CFG2* skillCfg, BYTE& hit, BYTE&crit, BYTE&luck, BYTE& luckDefend )
//{
//	FIGHT_ATTR stFightAttr;
//
//	stFightAttr.attackLevel			  = (int)role->wLevel;
//	stFightAttr.defendLevel			  = (int)comRole->wLevel;
//	stFightAttr.attackHit			  = (float)role->roleAttr[E_ATTR_HIT];
//	stFightAttr.attackCrit			  = (float)role->roleAttr[E_ATTR_CRIT];
//	stFightAttr.attackLuck			  = (float)role->roleAttr[E_ATTR_LUCK];
//	stFightAttr.defendDodge			  = (float)comRole->roleAttr[E_ATTR_DODGE];
//	stFightAttr.defendCritres		  = (float)comRole->roleAttr[E_ATTR_CRIT_RES];
//	stFightAttr.defendLuck			  = (float)comRole->roleAttr[E_LUCK_DEF];
//	stFightAttr.attackAttackMaxValue  = role->roleAttr[E_ATTR_ATK_MAX2];
//	stFightAttr.attackAttackMinValue  = role->roleAttr[E_ATTR_ATK_MIN2];
//	stFightAttr.defendDefMaxValue     = comRole->roleAttr[E_ATTR_DEF_MAX];
//	stFightAttr.defendDefMinValue     = comRole->roleAttr[E_ATTR_DEF_MIN];
//	stFightAttr.attackSkillAttackValue= role->roleAttr[E_ATTR_SPELLATK_MAX];
//	stFightAttr.defendSkillDefValue   = comRole->roleAttr[E_ATTR_SPELLDEF_MAX];
//
//	return getAtkValue( hit, crit, luck, luckDefend, stFightAttr );
//}

//int FightCtrl:: getRoleAtkMonValue( ROLE* role, CONFIG::MONSTER_CFG* monCfg, CONFIG::SKILL_CFG2* skillCfg, BYTE& hit, BYTE&crit, BYTE&luck, BYTE& luckDefend )
//{
//	FIGHT_ATTR stFightAttr;
//
//	stFightAttr.attackLevel			  = (int)role->wLevel;
//	stFightAttr.defendLevel			  = (int)monCfg->level;
//	stFightAttr.attackHit			  = (float)role->roleAttr[E_ATTR_HIT];
//	stFightAttr.attackCrit			  = (float)role->roleAttr[E_ATTR_CRIT];
//	stFightAttr.attackLuck			  = (float)role->roleAttr[E_ATTR_LUCK];
//	stFightAttr.defendDodge			  = (float)monCfg->dodge;
//	stFightAttr.defendCritres		  = (float)monCfg->crit_res;
//	stFightAttr.defendLuck			  = (float)monCfg->luck_def;
//	stFightAttr.attackAttackMaxValue  = role->roleAttr[E_ATTR_ATK_MAX2];
//	stFightAttr.attackAttackMinValue  = role->roleAttr[E_ATTR_ATK_MIN2];
//	stFightAttr.defendDefMaxValue     = monCfg->def_max;
//	stFightAttr.defendDefMinValue     = monCfg->def_min;
//	stFightAttr.attackSkillAttackValue= role->roleAttr[E_ATTR_SPELLATK_MAX];
//	stFightAttr.defendSkillDefValue   = monCfg->spelldef_max;
//
//	return getAtkValue( hit, crit, luck, luckDefend, stFightAttr );
//	//int hpValue = 0;
//	//if ( skillCfg->skillPara.skillType == E_SKILL_TYPE_ATTACK )
//	//{
//	//	hpValue = getAtkValue( hit, crit, luck, luckDefend, stFightAttr );
//	//}
//
//	//if ( skillCfg->skillPara.skillType == E_SKILL_TYPE_TREAT )
//	//{
//	//	//道术治疗		
//	//	//最终治疗值＝（攻击者道术攻击值÷3＋技能加成值）×（技能系数÷100）		
//	//	//攻击者道术攻击值=上限与下限的
//
//	//	fHurt = ( BASE::randBetween( role->roleAttr[E_ATTR_SOL_MIN], role->roleAttr[E_ATTR_SOL_MAX]+1) / 3.0f + skillInfo.skillAtt ) * (skillInfo.skillCoff / 100.0f);
//	//	comID = role->dwRoleID;
//	//}
//	//
//	//return hpValue;
//
//	//int hurtValue = 0;
//	//int levelDiff = (int)role->wLevel - (int)monCfg->level;
//	//if ( levelDiff < -10 )
//	//{
//	//	levelDiff = -10;
//	//}
//	//if ( levelDiff > 10 )
//	//{
//	//	levelDiff = 10;
//	//}
//	//float attackHitRate = role->roleAttr[E_ATTR_HIT]/(50.0f + role->wLevel * 50.0f);
//	//float defDodgeRate = monCfg->dodge/(50.0f + monCfg->level * 50.0f);
//	//float succRatio = ( levelDiff * 0.02f + (attackHitRate - defDodgeRate) + 0.85f )/*/100*/;
//	//if (succRatio < 0.3f)
//	//{
//	//	succRatio = 0.3f;
//	//}
//	//if (succRatio > 1.0f)
//	//{
//	//	succRatio = 1.0f;
//	//}	
//	//if ( RAND_HIT( succRatio ) )
//	//{
//	//	hit = 1;	
//	//	float attackCritRate = role->roleAttr[E_ATTR_CRIT]/(50.0f + role->wLevel * 50.0f);
//	//	float defCritDefRate = monCfg->crit_res/(50.0f + monCfg->level * 50.0f);
//	//	float critRatio = levelDiff * 0.02f + (attackCritRate - defCritDefRate);
//	//	if (critRatio < 0.05f)
//	//	{
//	//		critRatio = 0.05f;
//	//	}
//	//	if (critRatio > 0.75f)
//	//	{
//	//		critRatio = 0.75f;
//	//	}
//
//	//	if ( RAND_HIT( critRatio) )
//	//	{
//	//		crit = 1;
//	//	}
//
//	//	float luckRatio = 0.15f + role->roleAttr[E_ATTR_LUCK]*0.05f;
//	//	if ( RAND_HIT( luckRatio) )
//	//	{
//	//		luck = 1;
//	//	}
//
//	//	BYTE luckDef = 0;
//	//	float luckDefRatio = 0.15f + monCfg->luck * 0.05f;
//	//	if ( RAND_HIT( luckDefRatio) )
//	//	{
//	//		luckDef = 1;
//	//	}
//
//	//	DWORD attackAttackValue = role->roleAttr[E_ATTR_ATK_MAX2];
//	//	DWORD defendDefValue = monCfg->def_max;
//
//	//	DWORD attackSkillAttackValue = role->roleAttr[E_ATTR_SPELLATK_MAX];
//	//	DWORD defendSkillDefValue = monCfg->spelldef_max;
//
//	//	if (luck == 0)
//	//	{
//	//		attackAttackValue = (BASE::randBetween(role->roleAttr[E_ATTR_ATK_MIN2], role->roleAttr[E_ATTR_ATK_MAX2] + 1 ));
//	//		attackSkillAttackValue = ( BASE::randBetween( role->roleAttr[E_ATTR_SPELLATK_MIN], role->roleAttr[E_ATTR_SPELLATK_MAX]+1 ) );
//	//	}
//
//	//	if (luckDef == 0)
//	//	{
//	//		defendDefValue = (BASE::randBetween(monCfg->def_min, monCfg->def_max + 1 ));
//	//		defendSkillDefValue = ( BASE::randBetween( monCfg->spelldef_min, monCfg->spelldef_max+1));
//	//	}
//	//	
//	//	if (crit == 1)
//	//	{
//	//		attackAttackValue = DWORD( (float)attackAttackValue * 1.5f );
//	//		attackSkillAttackValue = DWORD( (float)attackSkillAttackValue * 1.5f );
//	//	}
//	//	
//	//	int nHurtValue = 0;
//	//	if ( attackAttackValue <= defendDefValue )
//	//	{
//	//		nHurtValue = 1;
//	//	}
//	//	else
//	//	{
//	//		nHurtValue = int(attackAttackValue - defendDefValue);
//	//	}
//	//	
//	//	hurtValue = (int)(-nHurtValue);
//	//}
//
//	//return hurtValue;
//}

//int FightCtrl::getMonAtkRoleValue( ROLE* comRole, CONFIG::MONSTER_CFG* monCfg, CONFIG::SKILL_CFG2* skillCfg, BYTE& hit, BYTE&crit, BYTE&luck, BYTE& luckDefend )
//{
//	FIGHT_ATTR stFightAttr;
//
//	stFightAttr.attackLevel			  = (int)monCfg->level;
//	stFightAttr.defendLevel			  = (int)comRole->wLevel;
//	stFightAttr.attackHit			  = (float)monCfg->hit;
//	stFightAttr.attackCrit			  = (float)monCfg->crit;
//	stFightAttr.attackLuck			  = (float)monCfg->luck;
//	stFightAttr.attackCrit			  = (float)monCfg->crit;
//	stFightAttr.defendDodge			  = (float)comRole->roleAttr[E_ATTR_DODGE];
//	stFightAttr.defendCritres		  = (float)comRole->roleAttr[E_ATTR_CRIT_RES];
//	stFightAttr.defendLuck			  = (float)comRole->roleAttr[E_LUCK_DEF];
//	stFightAttr.attackAttackMaxValue  = monCfg->atk_max;
//	stFightAttr.attackAttackMinValue  = monCfg->atk_min;
//	stFightAttr.defendDefMaxValue     = comRole->roleAttr[E_ATTR_DEF_MAX];
//	stFightAttr.defendDefMinValue     = comRole->roleAttr[E_ATTR_DEF_MIN];
//	stFightAttr.attackSkillAttackValue= monCfg->spellatk_max;
//	stFightAttr.defendSkillDefValue   = comRole->roleAttr[E_ATTR_SPELLDEF_MAX];
//
//	return getAtkValue( hit, crit, luck, luckDefend, stFightAttr );
//
//	//int hurtValue = 0;
//	//int levelDiff =  (int)monCfg->level - (int)comRole->wLevel;
//	//if ( levelDiff < -10 )
//	//{
//	//	levelDiff = -10;
//	//}
//	//if ( levelDiff > 10 )
//	//{
//	//	levelDiff = 10;
//	//}
//
//
//	//float attackHitRate = monCfg->hit/(50.0f + monCfg->level * 50.0f);
//	//float defDodgeRate = comRole->roleAttr[E_ATTR_DODGE]/(50.0f + comRole->wLevel * 50.0f);
//	//float succRatio = ( levelDiff * 0.02f + (attackHitRate - defDodgeRate) + 0.85f )/*/100*/;
//
//	//if (succRatio < 0.3f)
//	//{
//	//	succRatio = 0.3f;
//	//}
//	//if (succRatio > 1.0f)
//	//{
//	//	succRatio = 1.0f;
//	//}
//
//
//	//if ( RAND_HIT( succRatio ) )
//	//{
//	//	hit = 1;
//	//	float attackCritRate = monCfg->crit/(50.0f + monCfg->level * 50.0f);
//	//	float defCritDefRate = comRole->roleAttr[E_ATTR_CRIT_RES]/(50.0f + comRole->wLevel * 50.0f);
//	//	float critRatio = levelDiff * 0.02f + (attackCritRate - defCritDefRate);
//	//	if (critRatio < 0.05f)
//	//	{
//	//		critRatio = 0.05f;
//	//	}
//	//	if (critRatio > 0.75f)
//	//	{
//	//		critRatio = 0.75f;
//	//	}
//
//	//	if ( RAND_HIT( critRatio) )
//	//	{
//	//		crit = 1;
//	//	}
//
//	//	float luckRatio = 0.15f + monCfg->luck*0.05f;
//	//	if ( RAND_HIT( luckRatio) )
//	//	{
//	//		luck = 1;
//	//	}
//
//	//	BYTE luckDef = 0;
//	//	float luckDefRatio = 0.15f + comRole->roleAttr[E_ATTR_LUCK] * 0.05f;
//	//	if ( RAND_HIT( luckDefRatio) )
//	//	{
//	//		luckDef = 1;
//	//	}
//
//	//	DWORD attackAttackValue = monCfg->atk_max;
//	//	DWORD defendDefValue = comRole->roleAttr[E_ATTR_DEF_MAX];
//
//	//	DWORD attackSkillAttackValue = monCfg->spellatk_max;
//	//	DWORD defendSkillDefValue = comRole->roleAttr[E_ATTR_SPELLDEF_MAX];
//
//	//	if (luck == 0)
//	//	{
//	//		attackAttackValue = (BASE::randBetween(monCfg->atk_min, monCfg->atk_max + 1 ));
//	//		attackSkillAttackValue = ( BASE::randBetween( monCfg->spellatk_min, monCfg->spellatk_max + 1 ) );
//	//	}
//
//	//	if (luckDef == 0)
//	//	{
//	//		defendDefValue = (BASE::randBetween( comRole->roleAttr[E_ATTR_DEF_MIN], comRole->roleAttr[E_ATTR_DEF_MAX] + 1 ));
//	//		defendSkillDefValue = ( BASE::randBetween( comRole->roleAttr[E_ATTR_SPELLDEF_MIN], comRole->roleAttr[E_ATTR_SPELLDEF_MAX] + 1 ));
//	//	}
//
//	//	if (crit == 1)
//	//	{
//	//		attackAttackValue = DWORD( (float)attackAttackValue * 1.5f );
//	//		attackSkillAttackValue = DWORD( (float)attackSkillAttackValue * 1.5f );
//	//	}
//
//	//	
//	//	if ( skillCfg->type != 5 )
//	//	{
//	//		attackAttackValue += attackSkillAttackValue;
//	//		defendDefValue +=  defendSkillDefValue;
//	//	}
//	//	
//	//	int nHurtValue = 0;
//	//	if ( attackAttackValue <= defendDefValue )
//	//	{
//	//		nHurtValue = 1;
//	//	}
//	//	else
//	//	{
//	//		nHurtValue = int( attackAttackValue - defendDefValue ) ;
//	//	}
//
//	//	hurtValue = (int)(-nHurtValue);
//	//}
//	//return hurtValue;
//}

void FightCtrl::reduceSkillsCool( list<SKILLCOOL>& lstSkills )
{
	for (auto& it : lstSkills )
	{
		REDUCEUNSIGNED(it.cool, 1);
	}
}

#define  ADDKEEPMAX( container, ele )\
	do \
	{\
		container.push_back(ele);\
	} while ( 0 );
	
	
void FightCtrl::getAtkEffect( CFightEle& attack, vector<SELF_EFFECT_NET>& vecSelfEff )
{
	attack.vecEffectAttr.clear();
	attack.bCommonAtk = false;
	attack.bNotAtk = false;
	attack.bDoubleAtk = false;

	int nHp = 0;
	int nMp = 0;

	auto& lstEff = attack.lstEffRound; 

	for ( auto it = lstEff.begin(); it != lstEff.end();/* it++*/ )
	{
		CONFIG::SKILL_EFF_CFG* effCfg = getSkillEffCfg2( it->type, it->level );
		if ( effCfg == NULL )
		{
			it++;
			continue;
		}
		
		switch ( it->type )
		{
			case	E_SKILL_EFFECT_ZHUOSAO		:  //灼烧   
			case	E_SKILL_EFFECT_ZHONGDU		:  //中毒
			case	E_SKILL_EFFECT_CHUNFENG		:  //春风

					for ( auto& oneEff : effCfg->vecEffectAttr )
					{
						if( oneEff.attr == E_ATTR_HP_MAX2 || oneEff.attr == E_ATTR_HP_MAX2 )
						{
							WORD dwValue = oneEff.value + WORD(oneEff.coff*attack.eleAttr[oneEff.attr]);

							int nExtraValue = oneEff.addType == E_ATTR_REDUCE ? -(int)dwValue : (int)dwValue;

							if (oneEff.attr == E_ATTR_HP_MAX2 )
							{
								nHp += nExtraValue;
								ADDUP( attack.hp_max, nExtraValue );
							}
							else
							{
								nMp += nExtraValue;
								ADDUP( attack.mp_max, nExtraValue );
							}
						}
						else
						{
							attack.vecEffectAttr.push_back( oneEff );
						}
					}
					break;//自己掉血  改变属性

			case    E_SKILL_EFFECT_POFANG		:  //破防
			case	E_SKILL_EFFECT_KUANGBAO		:  //狂暴
			case	E_SKILL_EFFECT_QIANNENG		:  //无力
			case	E_SKILL_EFFECT_SAOLAN		:  //烧蓝
			case	E_SKILL_EFFECT_ZHENQI		:  //真气

					for ( auto& oneEff : effCfg->vecEffectAttr )
					{
						COND_CONTINUE( oneEff.attr == E_ATTR_HP_MAX2 || oneEff.attr == E_ATTR_HP_MAX2 );

						attack.vecEffectAttr.push_back( oneEff );
					}
					break;//改变属性

			case	E_SKILL_EFFECT_MABI			:  //麻痹
			case	E_SKILL_EFFECT_XUANYUN		:  //诱惑
			case	E_SKILL_EFFECT_SHUFU		:  //束缚
					attack.bNotAtk = true;
					break;//不能进攻

			case	E_SKILL_EFFECT_CHENGMO		:  //沉默
					attack.bCommonAtk = true;
					break;//只能用普通攻击

			default: 
					it = lstEff.erase( it );
					continue;
		}
		
		vecSelfEff.push_back( SELF_EFFECT_NET( it->type, it->round, nHp, nMp ) );

		REDUCEUNSIGNED(it->round, 1);

		if ( it->round == 0 )
		{
			it = lstEff.erase( it );
		}
		else
		{
			it++;
		}
	}

	for ( auto it = attack.lstDefEffRound.begin(); it != attack.lstDefEffRound.end(); it++ )
	{
		vecSelfEff.push_back( SELF_EFFECT_NET( it->type, it->round, 0, 0 ) );
	}
}

void FightCtrl::getDefEffect( CFightEle& def, int& atkValue, vector<EFFECT_NET>& vecEff )
{
	int nExtraValue = 0;

	auto& lstEff = def.lstDefEffRound; 

	for ( auto it = lstEff.begin(); it != lstEff.end(); )
	{
		CONFIG::SKILL_EFF_CFG* effCfg = getSkillEffCfg2( it->type, it->level );
		if ( effCfg == NULL )
		{
			it++;
			continue;
		}

		switch ( it->type )
		{
			case	E_SKILL_EFFECT_MOFADUN		:  //魔法盾
			case	E_SKILL_EFFECT_XURUO		:  //虚弱
					for ( auto& oneEff : effCfg->vecEffectAttr )
					{
						atkValue = (int)oneEff.value + int( oneEff.coff * atkValue );
					}
					break;//自己掉血  改变属性

			default: 
					it = lstEff.erase( it );
					continue;

		}

		vecEff.push_back( EFFECT_NET( it->type, it->round ) );

		REDUCEUNSIGNED(it->round, 1);

		if ( it->round == 0 )
		{
			it = lstEff.erase( it );
		}
		else
		{
			it++;
		}
	}

	for ( auto it = def.lstEffRound.begin(); it != def.lstEffRound.end(); it++ )
	{
		vecEff.push_back( EFFECT_NET( it->type, it->round ) );
	}
}


void FightCtrl::setSkillEffect( CFightEle& attack, CONFIG::SKILL_CFG2* skillCfg  )
{
	if ( skillCfg->eff_type == E_SKILL_EFFECT_MONATTACKTWO )
	{
		attack.bDoubleAtk = true;
	}
	else
	{
		EFFECT_ROUND effRound( skillCfg->level, skillCfg->eff_type, skillCfg->round );

		if ( skillCfg->eff_type == E_SKILL_EFFECT_MOFADUN ||  skillCfg->eff_type == E_SKILL_EFFECT_XURUO )
		{
			ADDKEEPMAX( attack.lstDefEffRound, effRound );//防守型buf
		}
		else
		{
			ADDKEEPMAX( attack.lstEffRound, effRound );//进攻型buf
		}
	}
}

BYTE FightCtrl::clientStartFight( vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef )//world boss fight
{
	BYTE byResult;

	BYTE count = 0;

	while ( !vecAtk.empty() && !vecDef.empty() )
	{
		count++;

		COND_BREAK( count > MAX_ROUND, byResult, E_MATCH_STATUS_LOSS );

		//攻击
		for (size_t i = 0; i < vecAtk.size(); i++ )
		{
			CFightEle& attack = vecAtk[ i ];

			CONFIG::SKILL_CFG2* skillCfg = getFightSkill( attack );
			if ( skillCfg != NULL )
			{
				vector<FIGHT_HURT> victimSeq;

				atkAtkDef(attack, skillCfg, vecAtk, vecDef, victimSeq);

				REDUCEUNSIGNED( attack.mp_max, attack.mp_cost );
			}

			reduceSkillsCool( attack.lstSkills );

			COND_BREAK( vecDef.empty(), byResult, E_MATCH_STATUS_WIN );

		}

		//防守
		for (size_t i = 0; i < vecDef.size(); i++ )
		{
			CFightEle& attack = vecDef[ i ];

			CONFIG::SKILL_CFG2* skillCfg = getFightSkill( attack );
			if ( skillCfg != NULL )
			{
				vector<FIGHT_HURT> victimSeq;

				atkAtkDef(attack, skillCfg, vecDef, vecAtk, victimSeq);

				REDUCEUNSIGNED( attack.mp_max, attack.mp_cost );	
			}

			reduceSkillsCool( attack.lstSkills );

			COND_BREAK( vecAtk.empty(), byResult, E_MATCH_STATUS_LOSS );
		}	
	}
	return ENTER_MAP_SUCCESS;
}

BYTE FightCtrl::clientStartFight( vector<CFightEle> vecAtk, vector<CFightEle> vecDef, vector<FIGHT_SEQ>& fightSeqs, BYTE& byResult )//arena and common boss 
{

	BYTE count = 0;

	while ( !vecAtk.empty() && !vecDef.empty() )
	{
		count++;

		COND_BREAK( count > MAX_ROUND, byResult, E_MATCH_STATUS_LOSS );

		recHpMpEachRound( vecAtk, vecDef );//回血 回魔

		//攻击
		for ( auto atkit = vecAtk.begin(); atkit != vecAtk.end(); )
		{
			CFightEle& attack = *atkit;
			vector<SELF_EFFECT_NET> vecSelfEff;//自身附加效果
			vector<FIGHT_HURT> victimSeq;

			getAtkEffect( attack, vecSelfEff );

			if ( attack.hp_max == 0 )//die
			{
				fightSeqs.push_back( FIGHT_SEQ( attack.pos, 0, count, attack.hp_max, attack.mp_max, vecSelfEff, victimSeq ) );
				atkit = vecAtk.erase( atkit );
				COND_BREAK( vecAtk.empty(), byResult, E_MATCH_STATUS_LOSS );
				continue;
			}

			if ( attack.bNotAtk )//can't attack
			{
				fightSeqs.push_back( FIGHT_SEQ( attack.pos, 0, count, attack.hp_max, attack.mp_max, vecSelfEff, victimSeq ) );
			}
			else
			{
				CONFIG::SKILL_CFG2* skillCfg = NULL;
				if ( attack.bCommonAtk )//only common skill
				{
					skillCfg =  getFightSkill(  attack, true );
				}
				else
				{
					skillCfg = getFightSkill( attack );
				}

				if ( skillCfg != NULL )
				{
					atkAtkDef(attack, skillCfg, vecAtk, vecDef, victimSeq);

					REDUCEUNSIGNED( attack.mp_max, attack.mp_cost );

					fightSeqs.push_back( FIGHT_SEQ( attack.pos, skillCfg->id, count, attack.hp_max, attack.mp_max, vecSelfEff, victimSeq ) );
				}
			}

			reduceSkillsCool( attack.lstSkills );

			COND_BREAK( vecDef.empty(), byResult, E_MATCH_STATUS_WIN );
			
			 atkit++ ;
		}

		//防守
		for (auto defit = vecDef.begin(); defit != vecDef.end(); )
		{
			CFightEle& attack = *defit;
			vector<SELF_EFFECT_NET> vecSelfEff;//自身附加效果
			vector<FIGHT_HURT> victimSeq;

			getAtkEffect( attack, vecSelfEff );

			if ( attack.hp_max == 0 )//die
			{
				fightSeqs.push_back( FIGHT_SEQ( attack.pos, 0, count, attack.hp_max, attack.mp_max, vecSelfEff, victimSeq ) );
				defit = vecDef.erase( defit);
				COND_BREAK( vecDef.empty(), byResult, E_MATCH_STATUS_WIN );
				continue;
			}

			if ( attack.bNotAtk )//can't attack
			{
				fightSeqs.push_back( FIGHT_SEQ( attack.pos, 0, count, attack.hp_max, attack.mp_max, vecSelfEff, victimSeq ) );
			}
			else
			{
				CONFIG::SKILL_CFG2* skillCfg = NULL;
				if ( attack.bCommonAtk )
				{
					skillCfg =  getFightSkill(  attack, true );
				}
				else
				{
					skillCfg = getFightSkill( attack );
				}

				if ( skillCfg != NULL )
				{
					atkAtkDef(attack, skillCfg, vecDef, vecAtk, victimSeq);

					REDUCEUNSIGNED( attack.mp_max, attack.mp_cost );

					fightSeqs.push_back( FIGHT_SEQ( attack.pos, skillCfg->id, count, attack.hp_max, attack.mp_max, vecSelfEff, victimSeq ) );
				}
			}

			reduceSkillsCool( attack.lstSkills );

			COND_BREAK( vecAtk.empty(), byResult, E_MATCH_STATUS_LOSS );

			defit++;
		}	
	}
	return ENTER_MAP_SUCCESS;
}

void FightCtrl::recHpMpEachRound( vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef )
{
	recHpMpEachRoundOneSide( vecAtk );

	recHpMpEachRoundOneSide( vecDef );
}


void FightCtrl::recHpMpEachRoundOneSide( vector<CFightEle>& vecAtk )
{
	for ( auto& atk : vecAtk )
	{
		if ( atk.hp_max + atk.hp_rec < atk.eleAttr[ E_ATTR_HP_MAX2 ] )
		{
			atk.hp_max += atk.hp_rec;
		}
		else
		{
			atk.hp_max = atk.eleAttr[ E_ATTR_HP_MAX2 ];
		}
	}
}

//竞技场战斗
BYTE FightCtrl::clientFightPlayer( ROLE* role, int type, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_SEQ>& fightSeqs, BYTE& byResult, DWORD dwComID )
{
	if ( role->dwComID != dwComID )
	{
		return ARENA_ROLE_NO_EXISTS;
	}

	initFightTeam( role, vecAtk, vecDef, dwComID);

	if ( vecAtk.empty() || vecDef.empty() )
	{
		return ARENA_ROLE_NO_EXISTS;
	}

	return clientStartFight( vecAtk, vecDef, fightSeqs, byResult );//不能引用 传出进攻防守序列用

}

//竞技场战斗
BYTE FightCtrl::meet_battle( ROLE* role, int type, vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_SEQ>& fightSeqs, BYTE& byResult, DWORD dwComID )
{
	role->update_meet_pk_count();
	if(role->pk_count == 0)
		return FIGHT_LESS_TIME;

	if ( role->meet_pk_target_roleid != dwComID )
	{
		return ARENA_ROLE_NO_EXISTS;
	}

	initFightTeam( role, vecAtk, vecDef, dwComID );

	//lognew("arena fight initFightTeam");
	if ( vecAtk.empty() || vecDef.empty() )
	{
		return ARENA_ROLE_NO_EXISTS;
	}

	//lognew("arena fight initFightTeam end");
	return clientStartFight( vecAtk, vecDef, fightSeqs, byResult );//不能引用 传出进攻防守序列用

}

BYTE FightCtrl::clientFightMap( ROLE* role,vector<CFightEle>& vecAtk, vector<CFightEle>& vecDef, vector<FIGHT_SEQ>& fightSeqs, BYTE& byResult, WORD wMonID )
{

	initFightTeam( role, vecAtk, vecDef, wMonID );

	if ( vecAtk.empty() || vecDef.empty() )
	{
		return FIGHT_MON_NO_EXISTS;
	}

	return clientStartFight( vecAtk, vecDef, fightSeqs, byResult );//不能引用 传出进攻防守序列用
}

BYTE FightCtrl::clientReportMap( ROLE* role, vector<DROP_SEQ>& dropSeqs, const WORD& mapID, const BYTE& dropType, DWORD* pvAdd )
{

	CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg( mapID );

	RETURN_COND( mapCfg == NULL, ENTER_MAP_NO_EXISTS );

	BYTE byRet = 0;

	if ( dropType == E_DROP_MON )//推图
	{
		ITEM_INFO para( role->wLastFightMapID, role->stFightDrop.lstDropMonster.size() );
		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_KILL_MON, &para);
	}
	else//挑战boss和精英关卡
	{
		if ( IS_ELITE_MAP(mapID) )
		{
			auto it = role->mapElite.find( mapCfg->group_id );//本组
			if ( it != role->mapElite.end() )
			{
				WORD& wCanFightMaxHeroID = it->second.wBase;

				WORD& wWinMaxHeroID = it->second.wExtra;

				CONFIG::MAP_CFG* nextMapCfg = CONFIG::getMapCfg( wCanFightMaxHeroID + 1 );

				if ( wCanFightMaxHeroID <= mapID )
				{
					if ( nextMapCfg != NULL && nextMapCfg->group_id == mapCfg->group_id )//开精英关卡 
					{
						wCanFightMaxHeroID++;
					}
				}

				if ( wWinMaxHeroID <= mapID )
				{
					wWinMaxHeroID = mapID;
				}
			}

			if ( role->bySendFightHeroTimes > 0 )
			{
				role->bySendFightHeroTimes--;
			}
			else if ( role->byAllBuyHeroTimes > 0 )
			{
				role->byAllBuyHeroTimes--;
			}

			RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_FIGHT_ELITE_TIMES );
		}
		else //守关boss
		{
			if ( role->wCanFightMaxMapID == mapID )//最大地图
			{
				if ( CONFIG::getMapCfg( role->wCanFightMaxMapID + 1 ) != NULL )//开地图 
				{
					role->wCanFightMaxMapID++;
					notifyMaxMap(role);
					//ItemCtrl::setMapMonster( role, role->wCanFightMaxMapID );	//设置新挂机地图
					FightCtrl::clientEnterMap( role, role->wCanFightMaxMapID);

					BroadcastCtrl::mapOpen( role, role->wCanFightMaxMapID );
				}
			}

			if ( role->wWinMaxMapID <= mapID )
			{
				role->wWinMaxMapID = mapID;
			}

			if ( role->wLevel > 7 )//策划要求<8级不要减次数
			{
				if ( role->bySendFightBossTimes > 0 )
				{
					role->bySendFightBossTimes--;
				}
				else if ( role->byAllBuyBossTimes > 0 )
				{
					role->byAllBuyBossTimes--;
				}
			}

			RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_FIGHT_BOSS_TIMES );

			ITEM_INFO para(mapID, mapCfg->boss_id);
			RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_FIGHT_BOSS, &para);
		}
	}

	list<ITEM_CACHE*> lstEquipNotify, lstPropsNotify, lstAutoSellItem, lstNoSellItem;
	ItemCtrl::killMapMonster( role, mapID, dropType, &lstEquipNotify, &lstPropsNotify, &lstAutoSellItem, &lstNoSellItem, pvAdd );


	if ( role->m_packmgr.lstEquipPkg.size() >= role->m_packmgr.curEquipPkgLimit )
	{
		byRet = DROP_EQUIP_FULL;
	}

	if ( role->m_packmgr.lstItemPkg.size() >= role->m_packmgr.curItemPkgLimit )
	{
		byRet = DROP_PROPS_FULL;
	}

	//1自动卖出 2未卖
	for ( auto& it : lstAutoSellItem )
	{
		if ( it != NULL)
		{
			dropSeqs.push_back( DROP_SEQ( it->itemCfg->id,  (WORD)it->itemNums, it->byQuality, 1, (it->itemNums * it->itemCfg->price)) );
			CC_SAFE_DELETE(it);
		}

	}
	for ( auto& it : lstNoSellItem )
	{
		if (it != NULL )
		{
			dropSeqs.push_back( DROP_SEQ( it->itemCfg->id,  (WORD)it->itemNums, it->byQuality, 2, (it->itemNums * it->itemCfg->price)) );
			CC_SAFE_DELETE(it);
		}
	}


	ItemCtrl::notifyClientItem(role, lstEquipNotify, PKG_TYPE_EQUIP);
	ItemCtrl::notifyClientItem(role, lstPropsNotify, PKG_TYPE_ITEM);

	return byRet;
}

//BYTE FightCtrl::clientReportMap( ROLE* role, vector<DROP_SEQ>& dropSeqs, BYTE monType, const vector<DWORD>& monIDs )
//{
	//for (auto wMonIndex : monIDs )
	//{
	//	CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( (WORD)wMonIndex );
	//	if ( monCfg == NULL )
	//	{
	//		continue;
	//	}
	//	
	//	if ( monType == E_FIGHT_MON )//推图
	//	{

	//	}
	//	else//守关boss
	//	{
	//		if ( role->byUseFightBossTimes >= role->byBuyFightBossTimes + role->bySendFightBossTimes)
	//		{
	//			return 0;
	//		}

	//		if (  wMonIndex == role->wBossID )
	//		{
	//			if ( role->wCanFightMaxMapID <= role->wLastFightMapID )
	//			{
	//				role->wCanFightMaxMapID ++;
	//				notifyMaxMap(role);
	//				BroadcastCtrl::mapOpen( role, role->wCanFightMaxMapID );
	//			}

	//			if ( role->wWinMaxMapID <= role->wLastFightMapID )
	//			{
	//				role->wWinMaxMapID = role->wLastFightMapID;
	//			}

	//			role->byUseFightBossTimes++;

	//			ITEM_INFO para(role->wLastFightMapID, wMonIndex);
	//			RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_FIGHT_BOSS, &para);
	//		}
	//	}

	//	//list<ITEM_CACHE*> lstItemNotify, lstAutoSellItem, lstNoSellItem;
	//	//ItemCtrl::killMonster( role, (WORD)wMonIndex, 1, &lstItemNotify, &lstAutoSellItem, &lstNoSellItem );

	//	list<ITEM_CACHE*> lstEquipNotify, lstPropsNotify, lstAutoSellItem, lstNoSellItem;
	//	ItemCtrl::killMonster( role, (WORD)wMonIndex, 1, &lstEquipNotify, &lstPropsNotify, &lstAutoSellItem, &lstNoSellItem );

	//	ITEM_INFO para(role->stFightDrop.wDropMap, 1);
	//	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_KILL_MON, &para);

	//	ItemCtrl::notifyClientItem(role, lstEquipNotify, PKG_TYPE_EQUIP);
	//	ItemCtrl::notifyClientItem(role, lstPropsNotify, PKG_TYPE_ITEM);

	//	BYTE byRet = !!(role->m_packmgr.lstEquipPkg.size() >= role->m_packmgr.curEquipPkgLimit);

	//	//1自动卖出 2未卖
	//	for ( auto it : lstAutoSellItem )
	//	{
	//		dropSeqs.push_back( DROP_SEQ( it->itemCfg->id,  (WORD)it->itemNums, it->byQuality, 1, (it->itemNums * it->itemCfg->price)) );
	//		delete it;
	//	}
	//	for ( auto it : lstNoSellItem )
	//	{
	//		dropSeqs.push_back( DROP_SEQ( it->itemCfg->id,  (WORD)it->itemNums, it->byQuality, 2, (it->itemNums * it->itemCfg->price)) );
	//		delete it;
	//	}
	//}
//	return 0;
//}

void FightCtrl::fightWorldBoss(ROLE* role, DWORD roleAttr[E_ATTR_MAX], bool bChance, bool inst_battle,  WORD boss_level, WORD skillid, DWORD boss_cur_hp, DWORD& boss_remain_hp)
{
	vector<CFightEle> vecAtk;
	vector<CFightEle> vecDef;

	initFightTeam( role, vecAtk, vecDef,  roleAttr, boss_cur_hp, boss_remain_hp , boss_level, skillid );

	if ( vecAtk.empty() || vecDef.empty() )
	{
		return ;
	}

	vector<FIGHT_SEQ> fightSeqs;

	clientStartFight( vecAtk, vecDef );//需要引用

	if ( vecDef.empty() )
	{
		boss_remain_hp = 0;
	}
	else
	{
		boss_remain_hp = vecDef[0].hp_max;
	}

}

