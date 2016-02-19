#ifndef TASK_H
#define TASK_H

#include "protocal.h"
#include "config.h"

USE_EXTERN_CONFIG;

namespace TASK
{
	enum E_TASK
	{
		TASK_SUCCESS = 0,//成功
		ITEM_FULL = 1,//背包满
		TASK_ERROR_ID = 2,//不存在的
		NOT_ENOUGH_LEVEL = 3,//等级不足
		NOT_COMPELETE = 4,//未完成
		ALREADY_GET = 5,//已经领取
	};
	void autoUnlockTask( ROLE* role, WORD wCompeleteID = 0 );
	int clientGetTasksList( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGetTaskReward( ROLE* role, unsigned char * data, size_t dataLen );
	void dailySetTask( ROLE* role );
	WORD getGroupByID( const WORD& wTaskID);
	void notifyRoleTaskStatus( ROLE* role, CONFIG::MISSION_CFG* taskCfg, const BYTE& status );
	void loginNotifyRoleTaskStatus( ROLE* role);

	void autoUnlockOpenActivitiyIDs( ROLE* role );
	int clientGetOpenActivitiyIDs( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGetOpenActivitiyIDsReward( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGetOpenActivitiyInfo( ROLE* role, unsigned char * data, size_t dataLen );
	void clearRepeatOpenActivityDaily( ROLE* role );

	BYTE isCanActivityReward( const ROLE* role );
	BYTE isCanTaskReward( const ROLE* role );
}

#endif//TASK_H

