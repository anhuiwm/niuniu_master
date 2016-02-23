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
		SKILL_SUCCESS = 0,//成功
		SKILL_ON_UPING = 1,//正在修炼
		SKILL_NO_EXISTS = 2,//无此技能
		COLD_NOT_ENOUGH = 3,//元宝不够
		UP_SKILL_SUCCESS = 4,//修炼成功
		MAX_SKILL_LEVEL = 5,//已经到满级
		SET_SKILL_REPEAT = 6,//重复设置
		LEVEL_NOT_ENOUGH = 7,//等级不足
		SKILL_LEVEL_FULL = 8,//技能等级已满
		VIP_LEVEL_NOT_ENOUGH = 9,//等级不足
	};


	DECLARE_CLIENT_FUNC( clientGetSkillsIndexs );
	DECLARE_CLIENT_FUNC( clientSkillMoveUp );
	DECLARE_CLIENT_FUNC( clientGetUnlockSkillsList );
	DECLARE_CLIENT_FUNC( clientSaveSkillsIndexs );
	DECLARE_CLIENT_FUNC( clientUpSkillLevel );
}

#endif // SKILL_H