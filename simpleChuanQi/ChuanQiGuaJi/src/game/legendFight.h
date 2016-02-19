#ifndef __LEGENDFIGHT_H_
#define __LEGENDFIGHT_H_


#include "base.h"
#include "typedef.h"

//#include "role.h"
//#include "proto.h"

#include <string>
#include <map>
#include <tuple>
using namespace std;


struct PRO
{
	WORD wHP;				//当前生命值	
	WORD wHPMax;			//生命值上限	

	WORD wMP;				//当前魔法值	
	WORD wMPMax;			//魔法值上限	

	WORD wAtkMax;			//物理攻击上限	
	WORD wAtkMin;			//物理攻击下限	

	WORD wMagicAtkMax;		//魔法攻击上限	
	WORD wMagicAtkMin;		//魔法攻击下限	

	WORD wSolAtkMax;		//道术攻击上限	
	WORD wSolAtkMin;			//道术攻击下限	

	WORD wDefMax;			//物理防御上限	
	WORD wDefMin;			//物理防御下限	

	WORD wMagicDefMax;		//魔法防御上限	
	WORD wMagicDefMin;		//魔法防御下限	

	WORD wPhyDodge;			//物理回避率	受到物理攻击时，闪避的概率
	WORD wMagicDodge;		//魔法回避率	受到魔法攻击时，闪避的概率
	WORD wSolDodge;			//道术回避率	受到道术攻击时，闪避的概率

	WORD wAtkSpeed;			//攻击速度	"初始攻速为0为1时，每4的倍数回合多一次攻击机会为2时，每3的倍数回合多一次攻击机会为3时，每2的倍数回合多一次攻击机会为4时，每回合多一次攻击机会"
	WORD wHPRecover;		//生命恢复	每2回合结束，自动回复的生命值
	WORD wMPRecover;		//魔法恢复	每2回合结束，自动回复的生命值

	WORD wHit;				//命中率

	ZERO_CONS(PRO);
};


struct FIGHT_PRO
{
	char id;
	WORD level;
	WORD jobOrMonsterID;

	string showName;//monster name, role name

	PRO rp;

	int round;
	int faint;

	size_t curSkillIndex;
	vector<CONFIG::SKILL_CFG2*> atkSkillVec;

	static FIGHT_PRO* createRoleFightPro(char id, const string & showName, WORD job, WORD level);
	static FIGHT_PRO* createMonsterFightPro(char id, CONFIG::MONSTER_CFG* monCfg );

	void initCommon(char id, const string & showName, WORD job, WORD level);

	void initRolePro(WORD job, WORD level);
	void initMonsterPro(CONFIG::MONSTER_CFG* monsterCfg);

	CONFIG::SKILL_CFG2* getAtkSkill();
	
};


#define TEAM_MAX_NUM 9

enum FORM_LEFT_RIGHT
{
	E_FORM_LEFT = 1,
	E_FORM_RIGHT = 2,
};



enum
{
//攻击距离：1近战，2远程
	E_ATK_DISTANCE_NEAR = 1, 
	E_ATK_DISTANCE_REMOTE = 2, 

//伤害类型：1物理，2魔法，3道术
	E_ATK_TYPE_PHY = 1, 
	E_ATK_TYPE_MAGIC = 2, 
	E_ATK_TYPE_TAO = 3, 

//攻击范围：1单体，2直线，3横线，4全体
	E_ATK_AREA_ONE = 1, 		
	E_ATK_AREA_LINE = 2, 	//列
	E_ATK_AREA_ROW = 3, 	//排
	E_ATK_AREA_ALL = 4, 

};

enum
{

	E_AREA_ONE = 0,			//单个

	E_AREA_LINE_LEFT = 11,	//左边一条线
	E_AREA_LINE_MID = 12,	//中间一条线
	E_AREA_LINE_RIGHT = 13,	//右边一条线

	E_AREA_ROW_FORE = 21,	//前排
	E_AREA_ROW_MID = 22,	//中排
	E_AREA_ROW_BACK = 23,	//后排

	E_AREA_ALL = 100,			//所有
};


struct TEAM
{
	FIGHT_PRO* member[TEAM_MAX_NUM];
	size_t leftNum;

	FORM_LEFT_RIGHT formLeftRight;

	ZERO_CONS(TEAM);

	void initRoleTeam( FORM_LEFT_RIGHT formLeftRight, const string& roleName, WORD job, WORD roleLevel, WORD skillIndexVec[], size_t skillNums );
	void initMonsterTeam( FORM_LEFT_RIGHT formLeftRight, WORD monIndexVec[], size_t monNums );

	int restoreHpMp(char*& pkt);

	void freeMember( );

};

//void clientLegendFight( PROTOCAL::PROTO* proto, ROLE* role, map<string,string>& queryStrMap );


#endif//__LEGENDFIGHT_H_
