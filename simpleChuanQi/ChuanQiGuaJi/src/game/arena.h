#ifndef ARENA_H
#define ARENA_H

#include "protocal.h"
#include "config.h"
#include <set>

#define RANK_MAX_ROLES_NUMS			(5000)		//�����������

#define MAX_ARENA_USE_NUMS			(5)			//������ÿ�������ս��

#define ARENA_NEED_LEVEL			(1)			//1�����Դ򾺼���

#define ARENA_COMPETITOR_NUMS		(4)			//������ƥ���������

#define ARENA_NO_FIGHT   0  //����ս��

enum eFIGHTRESULT
{
	//0:��  1:Ӯ  2:û�б�
	E_MATCH_STATUS_LOSS = 0,//��������
	E_MATCH_STATUS_WIN = 1,//����Ӯ��
	E_MATCH_STATUS_LOSS_PASSIVE = 2,//����Ӯ��
	E_MATCH_STATUS_WIN_PASSIVE  = 3,//��������
	E_MATCH_STATUS_NONE = 4,//û��
};

enum eFIGHT
{
	E_MALE_CLOTH        = 10009,//Ĭ�������·�
	E_FMALE_CLOTH       = 10010,//Ĭ��Ů���·�
	FIGHT_SKILL_MONK    = 65533,//��ʿĬ�ϼ���
	FIGHT_SKILL_MAGIC   = 65534,//Ĭ��
	FIGHT_SKILL_WARRIOR = 65535,//Ĭ��
};

namespace ARENA
{
	enum eARENA
	{
		ARENA_SUCCESS = 0,				//�ɹ�
		ARENA_NO_EXISTS = 1,			//������
		ARENA_ALREADY = 2,				//�Ѿ���
		ARENA_LEVEL_NOTENOUGH = 3,		//�ȼ�����
		ARENA_BUY_FULL = 4,				//�����������
		ARENA_NOT_NEED_BUY = 5,			//��ս��������
		GOLD_NOT_ENOUGH = 6,			//Ԫ������
		ARENA_ALREADY_FIGHT = 7,		//��ս��
		ERROR_RESULT = 8,				//�����ܵĽ��
		LOSE_RESULT = 9,				//����
	};

	void offerArenaReward();
	int getArenaRecommend( ROLE* role, unsigned char * data, size_t dataLen );
	int arenaFightPre( ROLE* role, unsigned char * data, size_t dataLen );
	int buyArenaFightCount( ROLE* role, unsigned char * data, size_t dataLen );
	int buyArenaCountGold( ROLE* role, unsigned char * data, size_t dataLen );
	int requireArenaRecord( ROLE* role, unsigned char * data, size_t dataLen );
	int initArenaRoles();
	void writeArenaRank2DB();
	void insertArenaRank(ROLE* role);
	void swapArenaRank(ROLE* role, ROLE* comRole, BYTE byResult);
	void toClienCompetitors(ROLE* role, bool bExchange = false);
	void getArenaRankID(list<DWORD>& listRankMatch, size_t nums);
	std::set<DWORD> getArenaTop10RoleIds();// ��þ�����ǰ10����ɫid
	int fight_report( ROLE* role, const DWORD target_role_id, const BYTE byResult, DWORD* pvAdd);
	DWORD getRankByRoleID(const DWORD id);

	int meetMapFightPre( ROLE* role, unsigned char * data, size_t dataLen );
}











#endif //ARENA_H
