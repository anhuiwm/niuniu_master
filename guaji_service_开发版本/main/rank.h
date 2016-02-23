#ifndef RANK_H
#define RANK_H

#include "protocal.h"
#include "config.h"

#define  MAX_RANK_NUM  30
//#define  MAX_RANK_LIST  100
#define  SUB_TYPE_WEAPON  1

#pragma pack(push, 1)
class levelRankInfo
{
public:
	char  szName[NAME_MAX_LEN];
	char  szGuildName[NAME_MAX_LEN];
	DWORD dwID;
	BYTE  bySex;
	BYTE  byJob;
	WORD  wLevel;
	DWORD dwFightValue;
	ZERO_CONS(levelRankInfo);
	levelRankInfo(ROLE * role);
};

class WeaponRankInfo
{
public:
	char  szRoleName[NAME_MAX_LEN];
	char  szWeaponName[NAME_MAX_LEN];
	DWORD dwID;
	BYTE  bySex;
	BYTE  byJob;
	WORD  wLevel;
	DWORD dwWeaponFightValue;
	ZERO_CONS(WeaponRankInfo);
	WeaponRankInfo(ROLE * role, CONFIG::ITEM_CFG *item, DWORD battle_point = 0);
};
#pragma pack(pop)

namespace RANK
{
	int getRanks( ROLE* role, unsigned char * data, size_t dataLen, WORD cmd );
	int getRankByLevel( ROLE* role, unsigned char * data, size_t dataLen );
	int getRankByFightvalue( ROLE* role, unsigned char * data, size_t dataLen );
	int getRankByWeaponscore( ROLE* role, unsigned char * data, size_t dataLen );
	int getRankByMatch( ROLE* role, unsigned char * data, size_t dataLen );
	void initDB2Ranks();
	void insertWeaponListRank(ROLE * role, ITEM_CACHE* itemCache);
	void eraseWeaponListRank(DWORD roleID, WORD weaponID);
	void saveRank();
	void insertRanks(ROLE * role);
	void insertLevelRanks(ROLE *role);
	void insertFightvalueRanks(const ROLE *role);

	uint get_level_top_id();
	uint get_fightvalue_top_id();
	uint get_weapon_top_id();
}





#endif