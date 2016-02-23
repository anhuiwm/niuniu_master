#include "pch.h"
//#include "resource.h"
//#include "zlibapi.h"
#if 0
#include "base.h"
#include "log.h"
//#include "cgimain.h"
#include "protocal.h"
#include "config.h"
using namespace BASE;
#include "config.h"
#include "legendFight.h"

USE_EXTERN_CONFIG;


FILE* _fightMsgFile =  NULL;

#if 1

#define debugPrintf(fmt, ...) do { if (_fightMsgFile) fprintf( _fightMsgFile, fmt, ##__VA_ARGS__); }while(0)
//#define printMatch(player, fmt, ...)	 if (_fightMsgFile) fprintf( _fightMsgFile, "%c:" fmt "\n", PLAYER_CHAR( (PLAYER*)player), ##__VA_ARGS__)

#else
//#define debugPrintf( ARGS... ) do { ((void)0) ; } while(0)

#define debugPrintf(fmt, ...) printf( fmt, ##__VA_ARGS__)
#define printMatch(player, fmt, ...)	 printf( "%c:" fmt "\n", PLAYER_CHAR( (PLAYER*)player), ##__VA_ARGS__)

#endif


FIGHT_PRO* FIGHT_PRO::createRoleFightPro(char id, const string & showName, WORD job, WORD level)
{
	FIGHT_PRO* fp = new FIGHT_PRO();
	fp->initCommon(id, showName, job, level);
	fp->initRolePro(job, level);
	return fp;
}

FIGHT_PRO* FIGHT_PRO::createMonsterFightPro(char id, CONFIG::MONSTER_CFG* monCfg )
{
//怪物永远只有一个技能:攻击技能
	auto monsterSkillIt = mapSkillCfg.find( monCfg->skill_id );
	if ( monsterSkillIt == mapSkillCfg.end() )
		return NULL;
	
	FIGHT_PRO* fp = new FIGHT_PRO();
	fp->initCommon(id, monCfg->name, monCfg->id, monCfg->level);
	fp->initMonsterPro(monCfg);
	fp->atkSkillVec.push_back( &monsterSkillIt->second );
	
	return fp;
}

void FIGHT_PRO::initCommon(char id, const string & showName, WORD job, WORD level)
{
	this->id = id;
	this->showName = showName;
	this->jobOrMonsterID = job;
	this->level = level;

	this->round = 0;
	this->faint = 0;

	this->curSkillIndex = 0;
}


void FIGHT_PRO::initRolePro(WORD job, WORD level)
{
	CONFIG::ATTR_UPGRADE_CFG* attrCfg = CONFIG::getAttrCfg( job, level );
	if ( attrCfg == NULL )
		return;

	rp.wHP = 
	rp.wHPMax = attrCfg->hp_max;

	rp.wMP = 
	rp.wMPMax = attrCfg->mp_max;

	rp.wAtkMax = attrCfg->atk_max;			//物理攻击上限	
	rp.wAtkMin = attrCfg->atk_min;			//物理攻击下限	

	rp.wMagicAtkMax = attrCfg->mag_max;		//魔法攻击上限	
	rp.wMagicAtkMin = attrCfg->mag_min;		//魔法攻击下限	

	rp.wSolAtkMax = attrCfg->sol_max;		//道术攻击上限	
	rp.wSolAtkMin = attrCfg->sol_min;			//道术攻击下限	

	rp.wDefMax = attrCfg->phydef_max;			//物理防御上限	
	rp.wDefMin = attrCfg->phydef_min;			//物理防御下限	

	rp.wMagicDefMax = attrCfg->magdef_max;		//魔法防御上限	
	rp.wMagicDefMin = attrCfg->magdef_min;		//魔法防御下限

	//装备
	//rp.wPhyDodge;			//物理回避率	受到物理攻击时，闪避的概率
	//rp.wMagicDodge;		//魔法回避率	受到魔法攻击时，闪避的概率
	//rp.wSolDodge;			//道术回避率	受到道术攻击时，闪避的概率

	//rp.wAtkSpeed;			//攻击速度	"初始攻速为0为1时，每4的倍数回合多一次攻击机会为2时，每3的倍数回合多一次攻击机会为3时，每2的倍数回合多一次攻击机会为4时，每回合多一次攻击机会"
	//rp.wHPRecover;		//生命恢复	每2回合结束，自动回复的生命值
	//rp.wMPRecover;		//魔法恢复	每2回合结束，自动回复的生命值

	//技能,装备
	//rp.wHit;				//命中率

}

void FIGHT_PRO::initMonsterPro(CONFIG::MONSTER_CFG* monsterCfg)
{
	rp.wHP = 
//	rp.wHPMax = monsterCfg->hp_max;

	rp.wMP = 
	rp.wMPMax = monsterCfg->mp_max;

	rp.wAtkMax = monsterCfg->atk_max;			//物理攻击上限	
	rp.wAtkMin = monsterCfg->atk_min;			//物理攻击下限	

	rp.wMagicAtkMax = monsterCfg->mag_max;		//魔法攻击上限	
	rp.wMagicAtkMin = monsterCfg->mag_min;		//魔法攻击下限	

	rp.wSolAtkMax = monsterCfg->sol_max;		//道术攻击上限	
	rp.wSolAtkMin = monsterCfg->sol_min;			//道术攻击下限	

	rp.wDefMax = monsterCfg->phydef_max;			//物理防御上限	
	rp.wDefMin = monsterCfg->phydef_min;			//物理防御下限	

	rp.wMagicDefMax = monsterCfg->magdef_max;		//魔法防御上限	
	rp.wMagicDefMin = monsterCfg->magdef_min;		//魔法防御下限

	//装备
	rp.wPhyDodge = monsterCfg->phy_dodge;			//物理回避率	受到物理攻击时，闪避的概率
	rp.wMagicDodge = monsterCfg->mag_dodge;		//魔法回避率	受到魔法攻击时，闪避的概率
	rp.wSolDodge = monsterCfg->sol_dodge;			//道术回避率	受到道术攻击时，闪避的概率

	rp.wAtkSpeed = monsterCfg->speed;			//攻击速度	"初始攻速为0为1时，每4的倍数回合多一次攻击机会为2时，每3的倍数回合多一次攻击机会为3时，每2的倍数回合多一次攻击机会为4时，每回合多一次攻击机会"
	rp.wHPRecover = monsterCfg->hp_rec;		//生命恢复	每2回合结束，自动回复的生命值
	rp.wMPRecover = monsterCfg->mp_rec;		//魔法恢复	每2回合结束，自动回复的生命值

	//技能,装备
	rp.wHit = monsterCfg->phy_hit;				//命中率

}

CONFIG::SKILL_CFG2* FIGHT_PRO::getAtkSkill()
{
	if ( atkSkillVec.empty() )
		return NULL;//0号技能不存在

	if ( curSkillIndex == atkSkillVec.size() )
		curSkillIndex = 0;

	//策划保证必有可以出手的技能
	if ( rp.wMP < atkSkillVec[curSkillIndex]->mp_cost )
	{
		curSkillIndex++;
		return getAtkSkill();
	}

	rp.wMP -= atkSkillVec[curSkillIndex]->mp_cost;		 
	return atkSkillVec[curSkillIndex++];
}


//死亡后,不会复活,可以直接清出队列

#define R_BETWEEN(min, max) ( (DWORD )BASE::randBetween(min, max) )

#define GET_ATK(attacker, fieldMin, fieldMax, skillCfg)	\
	( ( R_BETWEEN( attacker->rp.fieldMin, attacker->rp.fieldMax) + skillCfg->skillPara.atkSkill.atkHurt ) * skillCfg->skillPara.atkSkill.atkCoff / 100 )

#define GET_DEF(defender, fieldMin, fieldMax)	\
	( R_BETWEEN( defender->rp.fieldMin, defender->rp.fieldMax) )

int getDamage( FIGHT_PRO* attacker, FIGHT_PRO* defender, const CONFIG::SKILL_CFG2* skillCfg )
{

//◆命中率公式			
//	◇物理回避		
//		最终命中率＝50＋命中率－目标物理回避率	
//			
//	◇魔法回避		
//		最终命中率＝100－目标魔法回避率	
//			
//	◇道术回避		
//		最终命中率＝100－目标道术回避率	

	int r = (int) RAND(100);
	
	int atk = 0;
	int def = 0;
	
	//if ( skillCfg->skillPara.atkSkill.atkType == E_ATK_TYPE_PHY )
	//{
	//	int hit = max<int>( 50 + attacker->rp.wHit - defender->rp.wPhyDodge, 0);
	//	//debugPrintf( "attack hit:%d, rand hit:%d\n", hit, r );
	//	
	//	if ( r >  hit )
	//		return -1;

	//	atk = GET_ATK( attacker, wAtkMin, wAtkMax, skillCfg );
	//	def = GET_DEF( defender, wDefMin, wDefMax);
	//	
	//}
	//else if ( skillCfg->skillPara.atkSkill.atkType == E_ATK_TYPE_MAGIC )
	//{
	//	int hit = max<int>( 100 - defender->rp.wMagicDodge, 0);
	//	//debugPrintf( "attack hit:%d, rand hit:%d\n", hit, r );

	//	if ( r >  hit )
	//		return -1;
	//	
	//	atk = GET_ATK( attacker, wMagicAtkMin, wMagicAtkMax, skillCfg );
	//	def = GET_DEF( defender, wMagicDefMin, wMagicDefMax);
	//	
	//}
	//else //if ( E_ATK_TYPE_TAO )
	//{
	//	int hit = max<int>( 100 - defender->rp.wSolDodge, 0);
	//	//debugPrintf( "attack hit:%d, rand hit:%d\n", hit, r );

	//	if ( r >  hit )
	//		return -1;
	//	
	//	atk = GET_ATK( attacker, wSolAtkMin, wSolAtkMax, skillCfg );
	//	def = GET_DEF( defender, wMagicDefMin, wMagicDefMax);
	//}

	//if ( atk <= def )
		return 1;

	//return (atk - def);

//◆伤害计算公式：			
//	◇物理伤害		
//		最终伤害值＝（攻击者物理攻击值＋技能加成值）×（技能系数÷100）－受击者物理防御值	
//		物理攻击值是物理攻击上限与下限之间随机的一个值	
//		物理防御值与物理攻击值同理	
//			
//	◇魔法伤害		
//		最终伤害值＝（攻击者魔法攻击值＋技能加成值）×（技能系数÷100）－受击者魔法防御值	
//			
//			
//	◇道术伤害		
//		最终伤害值＝（攻击者道术攻击值＋技能加成值）×（技能系数÷100）－受击者魔法防御值	
//			
//			
//		附注：最终伤害值小于1时，直接强制为1	
//			

}



#define AREA_STR(area)	\
	( area == E_ATK_AREA_ONE ? "E_ATK_AREA_ONE" :\
			area == E_ATK_AREA_LINE ? "E_ATK_AREA_LINE" :\
			area == E_ATK_AREA_ROW ? "E_ATK_AREA_ROW" :\
			area == E_ATK_AREA_ALL ? "E_ATK_AREA_ALL" : "E_ATK_AREA_ERROR" )

#define PRINT_AREA(area)	debugPrintf("====%s====\n", AREA_STR(area) )


int doHurt( WORD area, FIGHT_PRO* attacker, const CONFIG::SKILL_CFG2* skillCfg, TEAM& tgtTeam,  int index)
{
	FIGHT_PRO* defender = tgtTeam.member[ index ];
	if ( defender == NULL || defender->rp.wHP == 0 )
		return 0;

	int damage = getDamage( attacker, defender, skillCfg );

	if ( damage == -1 )
	{
		debugPrintf("%c---->%c\t\t[%s], miss\n", attacker->id, defender->id, AREA_STR(area) );
		return -1;
	}
	else
		debugPrintf("%c---->%c\t\t[%s], hurt:%d\n", attacker->id, defender->id, AREA_STR(area), damage );

	if ( defender->rp.wHP > (WORD)damage )
	{
		defender->rp.wHP -= (WORD)damage;
	}
	else
	{
		assert( tgtTeam.leftNum > 0 );

		delete defender;
		tgtTeam.member[index] = NULL;
		tgtTeam.leftNum --;
	}

	return damage;
}

#if 0
			1	4	7
玩家		0	3	6
			2	5	8
			
8	5	2		1	4	7
6	3	0	VS	0	3	6
7	4	1		2	5	8

#endif

void PRINT_MEMBER(const TEAM& t, int i)
{
	if ( t.member[i] )
	{
		//debugPrintf( "%c(%06u)\t", t.member[i]->id, t.member[i]->rp.wHP );
		debugPrintf( "%c(%u)\t\t", t.member[i]->id, t.member[i]->rp.wHP );
		return;
	}

	//debugPrintf( "%c(%06u)\t", '0', 0 );
	//debugPrintf( "%c(%u)\t\t", '0', 0 );
	debugPrintf( "   +\t\t");
}

void putsForm( const TEAM& t1, const TEAM& t2, const char* prefixTab, const char* fmt, ...  )
{
	char title[512];
	va_list	ap;
	va_start(ap, fmt);	
	vsnprintf(title, sizeof(title), fmt, ap);
	va_end(ap);

	debugPrintf("%s\n", title);


	debugPrintf("%s", prefixTab);
	PRINT_MEMBER(t1, 8);
	PRINT_MEMBER(t1, 5);
	PRINT_MEMBER(t1, 2);
	
	PRINT_MEMBER(t2, 1);
	PRINT_MEMBER(t2, 4);
	PRINT_MEMBER(t2, 7);
	debugPrintf("\n");

	debugPrintf("%s", prefixTab);
	PRINT_MEMBER(t1, 6);
	PRINT_MEMBER(t1, 3);
	PRINT_MEMBER(t1, 0);
	
	PRINT_MEMBER(t2, 0);
	PRINT_MEMBER(t2, 3);
	PRINT_MEMBER(t2, 6);
	debugPrintf("\n");

	debugPrintf("%s", prefixTab);
	PRINT_MEMBER(t1, 7);
	PRINT_MEMBER(t1, 4);
	PRINT_MEMBER(t1, 1);
	
	PRINT_MEMBER(t2, 2);
	PRINT_MEMBER(t2, 5);
	PRINT_MEMBER(t2, 8);
	debugPrintf("\n");
	
	debugPrintf("\n");

}

#if 0

#define MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam, index )\
	do {\
		int damage = doHurt( atkArea, attacker, skillCfg, tgtTeam,  index);\
		if ( damage == 0 )\
			break;\
		APPEND_BYTE( pkt, tgtTeam.member[index]->id );\
		APPEND_WORD( pkt, damage==-1?0:damage);\
		count++;\
	}while(0)

#else


#define MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam, index )		MK_HURT_STR2( pkt, count, atkArea, attacker, skillCfg, tgtTeam, index )

void MK_HURT_STR2( char*& pkt, int& count, WORD atkArea, FIGHT_PRO* attacker, const CONFIG::SKILL_CFG2* skillCfg, TEAM& tgtTeam,  int index)
{

		char id = '0';
	
		FIGHT_PRO* defender = tgtTeam.member[ index ];
		if ( defender )
			id = defender->id;
		
		int damage = doHurt( atkArea, attacker, skillCfg, tgtTeam,  index);
		if ( damage == 0 )
			return;

		APPEND_BYTE( pkt, id );
		APPEND_WORD( pkt, damage==-1?0:damage);

		count++;
		
}



#endif



void attack( char*& pkt, FIGHT_PRO* attacker, TEAM& tgtTeam )
{

	CONFIG::SKILL_CFG2* skillCfg = attacker->getAtkSkill();
	if ( skillCfg == NULL )
		return;

	WORD atkArea = skillCfg->skillPara.atkSkill.atkArea;

//attacker_fight_id(BYTE), skillid(WORD), attack_area:击打范围(BYTE), 

APPEND_BYTE( pkt, attacker->id );
APPEND_WORD( pkt, skillCfg->id );

//hurt_nums:受击人口数量(BYTE), list[ defender_fight_id(BYTE), damage(WORD,0表示闪避)  ]

char* pCount = pkt;

int count = 0;
APPEND_BYTE( pkt, 0 );

	switch ( atkArea )
	{
		case E_ATK_AREA_ONE:

			//APPEND_BYTE( pkt, E_AREA_ONE );

			MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  0);
			break;
			
		case E_ATK_AREA_LINE:
			{
				int x = E_AREA_LINE_MID;
				auto r = RAND(10000);

//8	5	2		1	4	7
//6	3	0	VS	0	3	6
//7	4	1		2	5	8

				if ( tgtTeam.member[1] )//左线
				{
					if ( tgtTeam.member[2] )//右线
					{
						x =  r < 10000/3 ? E_AREA_LINE_LEFT : r < 10000/3*2 ? E_AREA_LINE_MID : E_AREA_LINE_RIGHT;
					}
					else
					{
						x =  r < 10000/2 ? E_AREA_LINE_LEFT : E_AREA_LINE_MID;
					}
				}
				else if ( tgtTeam.member[2] )//右线
				{
					x =  r < 10000/2 ? E_AREA_LINE_MID : E_AREA_LINE_RIGHT;
				}

				if ( x == E_AREA_LINE_LEFT )
				{
					//APPEND_BYTE( pkt, E_AREA_LINE_LEFT );

					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  1);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  4);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  7);
				}
				else if ( x == E_AREA_LINE_MID )
				{
					//APPEND_BYTE( pkt, E_AREA_LINE_MID );

					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  0);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  3);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  6);
				}
				else
				{
					//APPEND_BYTE( pkt, E_AREA_LINE_RIGHT );

					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  2);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  5);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  8);
				}

				break;
			}
		case E_ATK_AREA_ROW:
			{
				int x = E_AREA_ROW_MID;
				auto r = RAND(10000);

				if ( tgtTeam.member[4] || tgtTeam.member[3] || tgtTeam.member[5] )//中间一排
				{
					if ( tgtTeam.member[7] || tgtTeam.member[6] || tgtTeam.member[8] )//后排
					{
						x =  r < 10000/3 ? E_AREA_ROW_FORE : r < 10000/3*2 ? E_AREA_ROW_MID : E_AREA_ROW_BACK;
					}
					else
					{
						x =  r < 10000/2 ? E_AREA_ROW_FORE : E_AREA_ROW_MID;
					}
				}
				else if ( tgtTeam.member[7] || tgtTeam.member[6] || tgtTeam.member[8] )//后排
				{
					x =  r < 10000/2 ? E_AREA_ROW_BACK : E_AREA_ROW_MID;
				}

				if ( x == E_AREA_ROW_FORE )
				{
					//APPEND_BYTE( pkt, E_AREA_ROW_FORE );
	
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  1);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  0);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  2);
				}
				else if ( x == E_AREA_ROW_MID )
				{
					//APPEND_BYTE( pkt, E_AREA_ROW_MID );

					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  4);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  3);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  5);
				}
				else
				{
					//APPEND_BYTE( pkt, E_AREA_ROW_BACK );

					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  7);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  6);
					MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  8);
				}

				break;
			}

		case E_ATK_AREA_ALL:

			//APPEND_BYTE( pkt, E_AREA_ALL );

			for ( int i=0; i<TEAM_MAX_NUM; i++)
			{
				MK_HURT_STR( pkt, atkArea, attacker, skillCfg, tgtTeam,  i);
			}
			
			break;
			
		default:
			break;

	}


	APPEND_BYTE( pCount, count );

}

#define ASSIGN_NUMS(p,num)	*(BYTE*) (p) = (BYTE)num

FIGHT_PRO* getAttacker( TEAM& team, int curRound )
{
	
	for ( int i=0; i<TEAM_MAX_NUM; i++)
	{
		FIGHT_PRO* fp = team.member[i];

		if ( fp && fp->round < curRound )
			return fp;
	}
	
	return NULL;
}

#define MOVE_MEMBER( t, src, tgt) \
	do {\
		if ( t.member[src] && t.member[tgt] == NULL  ) {\
			APPEND_BYTE( pkt, t.member[src]->id);\
			APPEND_BYTE( pkt, src);\
			APPEND_BYTE( pkt, tgt);\
			t.member[tgt] = t.member[src];\
			t.member[src] = NULL;\
			moveCount++;\
		}\
	}while(0)


int moveDefender( char*& pkt, TEAM& t )
{
	//move_nums:防守方移动数量(BYTE), list [ defender_fight_id(BYTE), from(BYTE),  to(BYTE) ]
	int moveCount =0;
	
	if ( t.member[0] == NULL )
	{
		vector<FIGHT_PRO*> v;
		if ( t.member[1] )		v.push_back( t.member[1] );
		if ( t.member[2] )		v.push_back( t.member[2] );
		if ( t.member[3] )		v.push_back( t.member[3] );

		if ( !v.empty() )
		{
			auto p = v[ RAND( v.size() ) ];
			int index = p == t.member[1] ? 1 :
						p == t.member[2] ? 2:
						3;
			MOVE_MEMBER( t, index, 0);
		}			
	}
	
//			1	4	7
//玩家	0	3	6
//			2	5	8

	MOVE_MEMBER( t, 4, 1);
	//MOVE_MEMBER( t, 3, 0);
	MOVE_MEMBER( t, 5, 2);
	
	MOVE_MEMBER( t, 7, 4);
	MOVE_MEMBER( t, 6, 3);
	MOVE_MEMBER( t, 8, 5);

	MOVE_MEMBER( t, 4, 1);
	MOVE_MEMBER( t, 3, 0);
	MOVE_MEMBER( t, 5, 2);

	return moveCount;

}


int fightRound(char*& pkt, TEAM& t1, TEAM& t2, int curRound )
{
	bool attackAgain = false;

int attackCount = 0;

	while(1)
	{
		FIGHT_PRO* attacker = getAttacker( t1, curRound );

		if ( attacker == NULL )
			return attackCount;

		if ( attacker->faint )
		{
			attacker->faint--;
			attacker->round++;
			continue;
		}

ATTACK_AGAIN:
		//攻击者不会因为攻击导致自己受伤害
		attack(pkt, attacker, t2 );
		attackCount++;

		char* pCount = pkt;
		APPEND_BYTE(pkt, 0);


		if ( t2.leftNum == 0 )
			return attackCount;
		
		int count = moveDefender( pkt, t2 );
		if ( count )
		{
			APPEND_BYTE(pCount, count);
		}

	
		//初始攻速为0
		//为1时，每4的倍数回合多一次攻击机会
		//为2时，每3的倍数回合多一次攻击机会
		//为3时，每2的倍数回合多一次攻击机会
		//为4时，每回合多一次攻击机会
		if ( !attackAgain &&
			( (attacker->rp.wAtkSpeed == 1 && curRound % 4 == 0 ) 
				|| ( attacker->rp.wAtkSpeed == 2 && curRound % 3 == 0  )
				|| ( attacker->rp.wAtkSpeed == 3 && curRound % 2 == 0  )
				|| ( attacker->rp.wAtkSpeed == 4 ) ) )
		{
			debugPrintf("attack again\n");
			
			attackAgain = true;
			goto ATTACK_AGAIN;
		}

		attacker->round++;
	
	}

	return attackCount;

}



void mkFormStr(char*& pkt, TEAM& t1, TEAM& t2)
{
	APPEND_BYTE(pkt, t1.leftNum+t2.leftNum );

	int count = 0;

	for ( int i=0; i<TEAM_MAX_NUM; i++)
	{
		FIGHT_PRO* fp = t1.member[i];
		if ( fp )
		{			
			APPEND_WORD(pkt, fp->jobOrMonsterID );
			APPEND_BYTE(pkt, fp->id );
			APPEND_WORD(pkt, fp->rp.wHP);
			APPEND_WORD(pkt, fp->rp.wMP);
			APPEND_BYTE(pkt, i);

			count++;
		}
	}

	for ( int i=0; i<TEAM_MAX_NUM; i++)
	{
		FIGHT_PRO* fp = t2.member[i];
		if ( fp )
		{			
			APPEND_WORD(pkt, fp->jobOrMonsterID );
			APPEND_BYTE(pkt, fp->id );
			APPEND_WORD(pkt, fp->rp.wHP);
			APPEND_WORD(pkt, fp->rp.wMP);
			APPEND_BYTE(pkt, i);

			count++;
		}
	}
	
	assert(count==t1.leftNum+t2.leftNum);

}
//死亡后,不会复活
#define isAllDead(t)		( t.leftNum == 0 )


//-1输, 0平, 1赢
int vs( char*& pkt, TEAM& t1, TEAM& t2, int &round )
{

	putsForm( t1, t2, "", "INIT POSITION" );

	char*tmp;

	//APPEND_BYTE(pkt, 2);

//round_count(BYTE),list:

	//left_attack_nums:左边出手数,list:
		//attacker_fight_id(BYTE), skillid(WORD), 
		//hurt_nums:受击人口数量(BYTE), list[ defender_fight_id(BYTE), damage(WORD,0表示闪避)  ]
		//move_nums:防守方移动数量(BYTE), list [ defender_fight_id(BYTE), from(BYTE),  to(BYTE) ]

	//right_attack_nums:右边出手数,list:
		//attacker_fight_id(BYTE), skillid(WORD), 
		//hurt_nums:受击人口数量(BYTE), list[ defender_fight_id(BYTE), damage(WORD,0表示闪避)  ]
		//move_nums:防守方移动数量(BYTE), list [ defender_fight_id(BYTE), from(BYTE),  to(BYTE) ]

	//restore_nums:恢复人口数量(BYTE), list [ fight_id(BYTE), attr(byte), value_add(WORD) ]

	while(1)
	{
		debugPrintf("=====================================%d=========================================\n", round);

tmp=pkt;
APPEND_BYTE( pkt, 0);
		int attackCount1 = fightRound(pkt, t1, t2, round);
		putsForm( t1, t2, "", "" );
APPEND_BYTE(tmp, attackCount1);


tmp=pkt;
APPEND_BYTE( pkt, 0);
		int attackCount2 = fightRound(pkt, t2, t1, round);
		putsForm( t1, t2, "", "" );
APPEND_BYTE(tmp, attackCount2);

		//restore_nums:恢复人口数量(BYTE), list [ fight_id(BYTE), attr(BYTE), value_add(WORD) ]

		int count = 0;

tmp = pkt;
APPEND_BYTE( pkt, 0);
		
		//每2回合结束，自动回复的生命值
		if ( round % 2 == 0 )
		{
			count += t1.restoreHpMp(pkt);
			count += t2.restoreHpMp(pkt);

			putsForm( t1, t2, "", "restore HP, round:%d", round );
		}

APPEND_BYTE(tmp, count);

		//mkFormStr( pkt, t1, t2 );

		if ( isAllDead(t1 ) )
			return -1;

		if ( isAllDead(t2 ) )
			return 1;

		round++;

	}
	
}


void TEAM::initRoleTeam( FORM_LEFT_RIGHT formLeftRight, const string& roleName, WORD job, WORD roleLevel, WORD skillIndexVec[], size_t skillNums )
{
	char id = formLeftRight == E_FORM_LEFT ? 'A' :'a';
	
	FIGHT_PRO* fp = FIGHT_PRO::createRoleFightPro(id++, roleName, job, roleLevel);

	this->member[0] = fp;
	this->leftNum = 1;
	this->formLeftRight = formLeftRight;

	for( size_t i=0; i<skillNums; i++)
	{
		
		auto sillIt = mapSkillCfg.find( skillIndexVec[i] );
		if ( sillIt == mapSkillCfg.end() )
			continue;

		CONFIG::SKILL_CFG2* skillCfg = &sillIt->second;
		if ( skillCfg->type == E_SKILL_TYPE_ATTACK )
		{
			fp->atkSkillVec.push_back( skillCfg );

		}
		else if ( skillCfg->type == E_SKILL_TYPE_SUMMON )
		{
			//策划保证只有一个召唤技能
			//召唤出来的怪物永远在1号位置(base 0)

			auto monIt = mapMonsterCfg.find( skillCfg->skillPara.summonSkill.monIndex );
			if ( monIt == mapMonsterCfg.end() )
				continue;

			FIGHT_PRO* fpMon = FIGHT_PRO::createMonsterFightPro(id, &monIt->second);
			this->member[1] = fpMon;
			this->leftNum += 1;
	
		}
		else if ( skillCfg->type == E_SKILL_TYPE_PASSIVE )
		{
		
			if ( skillCfg->skillPara.passiveSkill.attrIndex == E_ATTR_PHY_HIT ) 
			{
				ADDUP3( fp->rp.wHit, skillCfg->skillPara.passiveSkill.attrValue, 60000 );
			}

		}
			
	}

	
}


void TEAM::initMonsterTeam( FORM_LEFT_RIGHT formLeftRight, WORD monIndexVec[], size_t monNums )
{
	char id = formLeftRight == E_FORM_LEFT ? 'A' :'a';
	
	this->formLeftRight = formLeftRight;
	this->leftNum = 0;

	int memberIndex = 0;

	assert( monNums<=9 );

	for( size_t i=0; i<monNums; i++)
	{

		auto monIt = mapMonsterCfg.find( monIndexVec[i] );
		if ( monIt == mapMonsterCfg.end() )
			continue;

		FIGHT_PRO* fpMon = FIGHT_PRO::createMonsterFightPro(id++, &monIt->second);
		this->member[memberIndex] = fpMon;
		this->leftNum += 1;
		memberIndex++;

	}

}

void TEAM::freeMember( )
{
	for ( int i=0; i<TEAM_MAX_NUM; i++)
	{
		delete this->member[i];
	}
}

int TEAM::restoreHpMp( char*& pkt )
{

int count = 0;

	for ( int i=0; i<TEAM_MAX_NUM; i++)
	{
		FIGHT_PRO* fp = this->member[i];
		if ( fp == NULL )
			continue;

		//restore_nums:恢复人口数量(BYTE), list [ fight_id(BYTE), attr(byte), value_add(WORD) ]

		if ( fp->rp.wHPRecover )
		{
			ADDUP3( fp->rp.wHP, fp->rp.wHPRecover, fp->rp.wHPMax );

			APPEND_BYTE( pkt, fp->id );
			APPEND_BYTE( pkt, E_ATTR_HP_REC );
			APPEND_WORD( pkt, fp->rp.wHPRecover );

			count++;
		}
		
		if ( fp->rp.wMPRecover )
		{
			ADDUP3( fp->rp.wMP, fp->rp.wMPRecover, fp->rp.wMPMax );
			
			APPEND_BYTE( pkt, fp->id );
			APPEND_BYTE( pkt, E_ATTR_MP_REC );
			APPEND_WORD( pkt, fp->rp.wMPRecover );
			
			count++;
		}
		
	}

	return count;
	
}

#if 0

void clientLegendFight( PROTOCAL::PROTO* proto, ROLE* role, map<string,string>& queryStrMap )
{

	_fightMsgFile =  fopen("fight.debug.txt", "wb");


	char* pktCur = proto->data;


	TEAM t1;
	WORD skillIndexVec1[] = { 23,1 };
	size_t skillNums1 = sizeof(skillIndexVec1) / sizeof(WORD);
	t1.initRoleTeam( E_FORM_LEFT, "roleName1", 1, 10, skillIndexVec1, skillNums1 );

	TEAM t2;
	WORD skillIndexVec2[] = { 1001, 2003 };
	size_t skillNums2 = sizeof(skillIndexVec2) / sizeof(WORD);
	t2.initRoleTeam( E_FORM_LEFT, "roleName2", 3, 10, skillIndexVec2, skillNums2 );


	TEAM tMonster;
	WORD monIndexVec[] = { 101, 102, 103, 101,102,103,101,102,103 };
	size_t monNums = sizeof(monIndexVec) / sizeof(WORD);
	tMonster.initMonsterTeam( E_FORM_RIGHT, monIndexVec, monNums );

	int result = 0;
	char* pResult = pktCur;
	APPEND_BYTE(pktCur, result );

	mkFormStr( pktCur, t2, tMonster );


	int round = 1;
	char* pRound = pktCur;
	APPEND_BYTE(pktCur, round );

	//vs( t1, t2 );
	//vs( t1, tMonster );
	result = vs( pktCur, t2, tMonster, round );


	APPEND_BYTE(pRound, round );
	APPEND_BYTE(pResult, result );


	proto->dataLen += pktCur-proto->data;

//X2( proto->data, pktCur - proto->data );


		
	t1.freeMember( );
	t2.freeMember( );
	tMonster.freeMember( );

	fclose(_fightMsgFile);

}

#endif

//8	5	2		1	4	7
//6	3	0	VS	0	3	6
//7	4	1		2	5	8
//'A'--'I'
//'a'--'i'

//form_info = nums, list [ job_or_monster_id(WORD), fight_id(char), hp(WORD), mp(WORD), pos(BYTE) ] 

//result(BYTE,1,0,-1)

//form_info(双方初始站位)

//round_count(BYTE),list:

	//left_attack_nums:左边出手数,list:
		//attacker_fight_id(BYTE), skillid(WORD), 
		//hurt_nums:受击人口数量(BYTE), list[ defender_fight_id(BYTE), damage(WORD,0表示闪避)  ]
		//move_nums:防守方移动数量(BYTE), list [ defender_fight_id(BYTE), from(BYTE),  to(BYTE) ]

	//right_attack_nums:右边出手数,list:
		//attacker_fight_id(BYTE), skillid(WORD), 
		//hurt_nums:受击人口数量(BYTE), list[ defender_fight_id(BYTE), damage(WORD,0表示闪避)  ]
		//move_nums:防守方移动数量(BYTE), list [ defender_fight_id(BYTE), from(BYTE),  to(BYTE) ]

	//restore_nums:恢复人口数量(BYTE), list [ fight_id(BYTE), attr(BYTE), value_add(WORD) ]


#endif



