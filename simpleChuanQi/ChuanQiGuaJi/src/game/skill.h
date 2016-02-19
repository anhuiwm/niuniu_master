#ifndef SKILL_H
#define SKILL_H
#include "protocal.h"
#include "config.h"

USE_EXTERN_CONFIG;

#define ZERO_TIME				0
#define SKILL_BASE_LEVEL		1
#define FIGHT_SKILL_INDEXS		4
#define OPEN_SKILL_LEVEL		1
#define MAX_SKILL_NUM			10
#define SIXTY(value)    ((value)*60)
#define NO_LIMIT_JOB			5
#define SPECIAL_SKILL_MIN_ID	60000


namespace SKILL
{
	enum eSKILL
	{
		SKILL_SUCCESS = 0,//�ɹ�
		SKILL_ON_UPING = 1,//��������
		SKILL_NO_EXISTS = 2,//�޴˼���
		COLD_NOT_ENOUGH = 3,//Ԫ������
		UP_SKILL_SUCCESS = 4,//�����ɹ�
		MAX_SKILL_LEVEL = 5,//�Ѿ�������
		SET_SKILL_REPEAT = 6,//�ظ�����
		LEVEL_NOT_ENOUGH = 7,//�ȼ�����
		SKILL_LEVEL_FULL = 8,//���ܵȼ�����
		VIP_LEVEL_NOT_ENOUGH = 9,//�ȼ�����
	};

//	//void autoUnlockSkills( ROLE* role );
//
//	//void leanHignSkill(ROLE* role, WORD id, WORD level);
//
//	//void onLevelUp(ROLE* role, WORD wOldLevel);
//
//	BYTE pauseUpSkill(ROLE *role, WORD id);
//
//	DWORD upSkill(ROLE *role, WORD id, BYTE& byErrorCode);
//	//WORD buySkill(ROLE *role, WORD id,BYTE&  byCode );
//	WORD upSkillOK(ROLE *role, WORD id);
//	void InitRoleSkills(ROLE *role, WORD* skills, BYTE* fightSkills);
//	BYTE setFightSkills(ROLE *role, BYTE* idIndexs);
//	BYTE getIndexBySkillID(ROLE *role, WORD id);
//	WORD getSkillIDByIndex(ROLE *role, BYTE index);
//	void getSkillsInfo(ROLE *role, WORD* skills, DWORD* times, BYTE* fightSkills);
//	WORD getGroupByID(WORD id);
//	//WORD getIDByGroup(WORD group);
//	void timeUpdateSkills(ROLE *role);
//	void levelUpdateSkills(ROLE *role, vector<CONFIG::SKILL_CFG2>& vecNewSkill);
//	DWORD buySkillCost(ROLE *role, WORD id);
//	int unlockSkillByItem( ROLE* role, CONFIG::ITEM_CFG* itemCfg );
//	int dealSkill(ROLE *role, unsigned char * data, size_t dataLen  ,WORD cmd);

	DECLARE_CLIENT_FUNC( clientGetSkillsIndexs );
	DECLARE_CLIENT_FUNC( clientSkillMoveUp );
	DECLARE_CLIENT_FUNC( clientGetUnlockSkillsList );
	DECLARE_CLIENT_FUNC( clientSaveSkillsIndexs );
	DECLARE_CLIENT_FUNC( clientUpSkillLevel );
}

#endif // SKILL_H