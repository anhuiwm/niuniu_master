#pragma once
#include "protocal.h"
#include "config.h"

#define RANK_MAX_ROLES_NUMS			(5000)		//�����������
#define MAX_ARENA_USE_NUMS			(5)			//������ÿ�������ս��
#define ARENA_NEED_LEVEL			(1)			//1�����Դ򾺼���
#define ARENA_COMPETITOR_NUMS		(4)			//������ƥ���������
#define ARENA_NO_FIGHT   0  //����ս��
#define TOP_TANK   1  //��һ

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


class ROLE;
#pragma pack(push, 1)
struct TEnemyInfo
{
	char  szName[22];
	uint dwID;
	BYTE  bySex;
	BYTE  byJob;
	WORD  wLevel;
	//uint dwFightValue;
	ZERO_CONS(TEnemyInfo);
	TEnemyInfo(ROLE* role);
};
#pragma pack(pop)

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
		MEET_PK_NOT_ATTACK = 10,				// �Է��ܱ���
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
	void getArenaRankID(list<uint>& listRankMatch, size_t nums);
	std::set<uint> getArenaTop10RoleIds();// ��þ�����ǰ10����ɫid
	uint getRankByRoleID(const uint id);
	int meetMapFightPre( ROLE* role, unsigned char * data, size_t dataLen );
	uint get_arena_top_id();
}


