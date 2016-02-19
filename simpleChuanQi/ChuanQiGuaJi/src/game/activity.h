#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "config.h"
#include "protocal.h"

namespace ACTIVITY
{
	//	enum E_ACTIVITY_ID
	//	{
	//		WORLD_BOSS_FIGHT		 = 1,//世界boss战
	//		CHANGSHA_BOSS_FIGHT		 = 2,//沙城争夺战
	//		HANGHUI_BOSS_FIGHT		 = 3,//行会争夺战
	//		CHANGBANPO_BOSS_FIGHT	 = 4,//血战长坂坡
	//	};

	enum E_ACTIVITY_RETURN
	{
		ACTIVITY_SUCCESS = 0,//已经开始
		ACTIVITY_WAITING = 1,//报名中
		ACTIVITY_NOT_OPEN = 2, //还未开始
		BOSSFIGHT_ENHANCE_ENOUGH = 3,//鼓舞次数已满
		COLD_NOT_ENOUGH = 4,//元宝不足
		ALREADY_ENLIST = 5,//已经报名
		ACTIVITY_NO_EXSIST = 6,//不存在
		ACTIVITY_START_FIGHT = 7,//战斗
		ACTIVITY_BOSS_DIE = 8,//怪物已经死掉
		NO_FIGHT_COLD_ENOUGH = 9,//为超过30秒
	};

	//	enum E_BOSS_FIGHT_USE
	//	{
	//		E_MALE_CLOTH        = 10009,//默认男性衣服
	//		E_FMALE_CLOTH       = 10010,//默认女孩衣服
	//		FIGHT_SKILL_MONK    = 65533,//道士默认技能
	//		FIGHT_SKILL_MAGIC   = 65534,//默认
	//		FIGHT_SKILL_WARRIOR = 65535,//默认
	//	};
	
	int clientGetActivities( ROLE* role, unsigned char * data, size_t dataLen );
	//
	//	//int registerActivity( ROLE* role, unsigned char * data, size_t dataLen );
	//	
	//	//int clientBossFight( ROLE* role, unsigned char * data, size_t dataLen );
	//
	//	//int enhanceBossFightActtivity( ROLE* role, unsigned char * data, size_t dataLen );
	//
	int getActtivityLeftTime( ROLE* role, unsigned char * data, size_t dataLen );
	//
	//	//int getBossFightHelper( ROLE* role, unsigned char * data, size_t dataLen );
	//	//int getBossFightHelperMe( ROLE* role, int nTag = 0);
	//	//int getBossFightHelperOther( ROLE* role, DWORD& helperID, int nTag = 0);
	//	//int startActivity( WORD id);
	//
	//	//int clientGetBossFightInfos( ROLE* role );
	//
	//	void startTodaySec(const time_t& curTime);
	//
	//	WORD getBossFightID();
	//	void setBossFightID(WORD bossID);
	//
	//	//int clientBossFight2( ROLE* role, unsigned char * data, size_t dataLen );
	//	//int getBossFightHelper2( ROLE* role, unsigned char * data, size_t dataLen );
	//	void notifyActivityStatus( ROLE* role, const BYTE& status, const WORD& actID, DWORD dwTime = 0 );
	//	void judgeNewActivity( ROLE* role, const WORD& wOldLevel );
	//	void notifyRoleActivityStatus( ROLE* role );
	//	void setBossFightStatusNO( );
	//	//int getBossFightRank( ROLE* role, unsigned char * data, size_t dataLen );
	DWORD getTodayNowSecond( time_t now);
}

#endif //ACTIVITY_H
