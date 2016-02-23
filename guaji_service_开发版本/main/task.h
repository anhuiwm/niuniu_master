#ifndef TASK_H
#define TASK_H

#include "protocal.h"
#include "config.h"

USE_EXTERN_CONFIG;

namespace TASK
{
	enum E_TASK
	{
		TASK_SUCCESS = 0,//�ɹ�
		ITEM_FULL = 1,//������
		TASK_ERROR_ID = 2,//�����ڵ�
		NOT_ENOUGH_LEVEL = 3,//�ȼ�����
		NOT_COMPELETE = 4,//δ���
		ALREADY_GET = 5,//�Ѿ���ȡ
	};
		
	enum e_platform_task
	{
		e_download = 1,//��һ�������ذ�װ��Ϸ
		e_up_level = 2,//�ڶ������ﵽ25��
		e_get_level= 3,//���������ﵽ35��
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

	void init_tencent_tasks(ROLE* role);
	bool get_tencent_task_reward(ROLE* role, const uint8 step);
	bool judge_tencent_task(ROLE* role, const uint8 step);
	bool php_get_platform_award(const string& phone_imei, const uint8 step);
	BYTE isCanActivityReward( const ROLE* role );
	BYTE isCanTaskReward( const ROLE* role );
	DWORD getDayFirstTime(const time_t& give_time);
}

#endif//TASK_H

