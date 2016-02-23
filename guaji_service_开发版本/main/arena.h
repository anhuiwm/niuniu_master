#pragma once
#include "protocal.h"
#include "config.h"

#define RANK_MAX_ROLES_NUMS			(5000)		//榜单最多留数据
#define MAX_ARENA_USE_NUMS			(5)			//竞技场每日最多挑战次
#define ARENA_NEED_LEVEL			(1)			//1级可以打竞技场
#define ARENA_COMPETITOR_NUMS		(4)			//竞技场匹配对手数量
#define ARENA_NO_FIGHT   0  //不在战斗
#define TOP_TANK   1  //第一

enum eFIGHTRESULT
{
	//0:输  1:赢  2:没有比
	E_MATCH_STATUS_LOSS = 0,//被动输了
	E_MATCH_STATUS_WIN = 1,//主动赢了
	E_MATCH_STATUS_LOSS_PASSIVE = 2,//被动赢了
	E_MATCH_STATUS_WIN_PASSIVE  = 3,//主动输了
	E_MATCH_STATUS_NONE = 4,//没打
};

enum eFIGHT
{
	E_MALE_CLOTH        = 10009,//默认男性衣服
	E_FMALE_CLOTH       = 10010,//默认女孩衣服
	FIGHT_SKILL_MONK    = 65533,//道士默认技能
	FIGHT_SKILL_MAGIC   = 65534,//默认
	FIGHT_SKILL_WARRIOR = 65535,//默认
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
		ARENA_SUCCESS = 0,				//成功
		ARENA_NO_EXISTS = 1,			//不存在
		ARENA_ALREADY = 2,				//已经是
		ARENA_LEVEL_NOTENOUGH = 3,		//等级不足
		ARENA_BUY_FULL = 4,				//购买次数已满
		ARENA_NOT_NEED_BUY = 5,			//挑战次数已满
		GOLD_NOT_ENOUGH = 6,			//元宝不足
		ARENA_ALREADY_FIGHT = 7,		//挑战中
		ERROR_RESULT = 8,				//不可能的结果
		LOSE_RESULT = 9,				//输了
		MEET_PK_NOT_ATTACK = 10,				// 对方受保护
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
	std::set<uint> getArenaTop10RoleIds();// 获得竞技场前10名角色id
	uint getRankByRoleID(const uint id);
	int meetMapFightPre( ROLE* role, unsigned char * data, size_t dataLen );
	uint get_arena_top_id();
}


