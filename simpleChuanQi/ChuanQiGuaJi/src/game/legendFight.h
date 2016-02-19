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
	WORD wHP;				//��ǰ����ֵ	
	WORD wHPMax;			//����ֵ����	

	WORD wMP;				//��ǰħ��ֵ	
	WORD wMPMax;			//ħ��ֵ����	

	WORD wAtkMax;			//����������	
	WORD wAtkMin;			//����������	

	WORD wMagicAtkMax;		//ħ����������	
	WORD wMagicAtkMin;		//ħ����������	

	WORD wSolAtkMax;		//������������	
	WORD wSolAtkMin;			//������������	

	WORD wDefMax;			//�����������	
	WORD wDefMin;			//�����������	

	WORD wMagicDefMax;		//ħ����������	
	WORD wMagicDefMin;		//ħ����������	

	WORD wPhyDodge;			//����ر���	�ܵ�������ʱ�����ܵĸ���
	WORD wMagicDodge;		//ħ���ر���	�ܵ�ħ������ʱ�����ܵĸ���
	WORD wSolDodge;			//�����ر���	�ܵ���������ʱ�����ܵĸ���

	WORD wAtkSpeed;			//�����ٶ�	"��ʼ����Ϊ0Ϊ1ʱ��ÿ4�ı����غ϶�һ�ι�������Ϊ2ʱ��ÿ3�ı����غ϶�һ�ι�������Ϊ3ʱ��ÿ2�ı����غ϶�һ�ι�������Ϊ4ʱ��ÿ�غ϶�һ�ι�������"
	WORD wHPRecover;		//�����ָ�	ÿ2�غϽ������Զ��ظ�������ֵ
	WORD wMPRecover;		//ħ���ָ�	ÿ2�غϽ������Զ��ظ�������ֵ

	WORD wHit;				//������

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
//�������룺1��ս��2Զ��
	E_ATK_DISTANCE_NEAR = 1, 
	E_ATK_DISTANCE_REMOTE = 2, 

//�˺����ͣ�1����2ħ����3����
	E_ATK_TYPE_PHY = 1, 
	E_ATK_TYPE_MAGIC = 2, 
	E_ATK_TYPE_TAO = 3, 

//������Χ��1���壬2ֱ�ߣ�3���ߣ�4ȫ��
	E_ATK_AREA_ONE = 1, 		
	E_ATK_AREA_LINE = 2, 	//��
	E_ATK_AREA_ROW = 3, 	//��
	E_ATK_AREA_ALL = 4, 

};

enum
{

	E_AREA_ONE = 0,			//����

	E_AREA_LINE_LEFT = 11,	//���һ����
	E_AREA_LINE_MID = 12,	//�м�һ����
	E_AREA_LINE_RIGHT = 13,	//�ұ�һ����

	E_AREA_ROW_FORE = 21,	//ǰ��
	E_AREA_ROW_MID = 22,	//����
	E_AREA_ROW_BACK = 23,	//����

	E_AREA_ALL = 100,			//����
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
