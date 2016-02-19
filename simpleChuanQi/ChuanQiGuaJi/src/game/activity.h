#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "config.h"
#include "protocal.h"

namespace ACTIVITY
{
	//	enum E_ACTIVITY_ID
	//	{
	//		WORLD_BOSS_FIGHT		 = 1,//����bossս
	//		CHANGSHA_BOSS_FIGHT		 = 2,//ɳ������ս
	//		HANGHUI_BOSS_FIGHT		 = 3,//�л�����ս
	//		CHANGBANPO_BOSS_FIGHT	 = 4,//Ѫս������
	//	};

	enum E_ACTIVITY_RETURN
	{
		ACTIVITY_SUCCESS = 0,//�Ѿ���ʼ
		ACTIVITY_WAITING = 1,//������
		ACTIVITY_NOT_OPEN = 2, //��δ��ʼ
		BOSSFIGHT_ENHANCE_ENOUGH = 3,//�����������
		COLD_NOT_ENOUGH = 4,//Ԫ������
		ALREADY_ENLIST = 5,//�Ѿ�����
		ACTIVITY_NO_EXSIST = 6,//������
		ACTIVITY_START_FIGHT = 7,//ս��
		ACTIVITY_BOSS_DIE = 8,//�����Ѿ�����
		NO_FIGHT_COLD_ENOUGH = 9,//Ϊ����30��
	};

	//	enum E_BOSS_FIGHT_USE
	//	{
	//		E_MALE_CLOTH        = 10009,//Ĭ�������·�
	//		E_FMALE_CLOTH       = 10010,//Ĭ��Ů���·�
	//		FIGHT_SKILL_MONK    = 65533,//��ʿĬ�ϼ���
	//		FIGHT_SKILL_MAGIC   = 65534,//Ĭ��
	//		FIGHT_SKILL_WARRIOR = 65535,//Ĭ��
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
