#include "rank.h"
#include <list>
#include <algorithm>
#include "roleCtrl.h"
#include "mysqlCtrl.h"
#include "db.h"
#include  "arena.h" 
#include "factionCtrl.h"
#define  NOTFULL(list)     (list.size() < MAX_RANK_NUM)
#define  RANK_LIMIT_LEVEL  4
#define  LOW_LEVEL_RETURN  
namespace RANK
{
	struct  WEAPONID
	{
		DWORD dwRoleID;
		WORD  wItemID;
		DWORD  dwFightValue;
		ZERO_CONS(WEAPONID);
		WEAPONID(DWORD dwRoleID, WORD  wItemID, DWORD  dwFightValue = 0)
		{
			this->dwRoleID = dwRoleID;
			this->wItemID = wItemID;
			this->dwFightValue = dwFightValue;
		}
		bool operator == (const WEAPONID& dst)const
		{
			return ( this->dwRoleID==dst.dwRoleID && this->wItemID==dst.wItemID );
		}
	};

	struct  LEVELID
	{
		DWORD dwRoleID;
		WORD  wLevel;
		ZERO_CONS(LEVELID);
		LEVELID(DWORD dwRoleID, WORD  wLevel)
		{
			this->dwRoleID = dwRoleID;
			this->wLevel = wLevel;
		}
		bool operator == (const LEVELID& dst)const
		{
			return ( this->dwRoleID==dst.dwRoleID && this->wLevel==dst.wLevel );
		}
	};

	struct  FIGHTID
	{
		DWORD dwRoleID;
		DWORD  dwFightValue;
		ZERO_CONS(FIGHTID);
		FIGHTID(DWORD dwRoleID, DWORD  dwFightValue)
		{
			this->dwRoleID = dwRoleID;
			this->dwFightValue = dwFightValue;
		}
		bool operator == (const FIGHTID& dst)const
		{
			return ( this->dwRoleID==dst.dwRoleID && this->dwFightValue==dst.dwFightValue );
		}
	};

	static list<LEVELID> listRankLevel; 
	static list<FIGHTID> listRankFightvalue;
	static list<WEAPONID> listRankWeaponscore;

	void rankList2DB();
}

void RANK::saveRank()
{
	EXEC_SQL( "TRUNCATE TABLE " TABLE_RANK);
	rankList2DB();
	ARENA::writeArenaRank2DB();
}

void RANK::rankList2DB()
{
	//size_t count = 0;
	for (auto it:listRankLevel)
	{
		if ( ! EXEC_SQL( "insert into  " TABLE_RANK " (roleid, level) values('%u', '%u') ",it.dwRoleID, it.wLevel) )
		{
			return ;
		}
		//if ( count++ >= MAX_RANK_NUM)
		//{
		//	break;
		//}
	}
	//count = 0;
	for (auto it:listRankFightvalue)
	{
		if ( ! EXEC_SQL( "insert into  " TABLE_RANK " (roleid, rolevalue) values('%u', '%u') ",it.dwRoleID, it.dwFightValue) )//要改
		{
			return ;
		}
		//if ( count++ >= MAX_RANK_NUM)
		//{
		//	break;
		//}
	}
	//count = 0;
	for (auto it:listRankWeaponscore)
	{
		if ( ! EXEC_SQL( "insert into  " TABLE_RANK " (roleid, itemid, itemvalue) values('%u','%u', '%u') ",it.dwRoleID, it.wItemID, it.dwFightValue) )//要改
		{
			return ;
		}
		//if ( count++ >= MAX_RANK_NUM)
		//{
		//	break;
		//}
	}
}

/*
@初始化列表，服务器启动时候用
*/
void RANK::initDB2Ranks()
{		
	if (listRankLevel.size() < MAX_RANK_NUM)
	{
		DWORD roleid;
		WORD  level;
		listRankLevel.clear();
		SQLSelect sqlx( DBCtrl::getSQL(), "select roleid, level from " TABLE_RANK " where level>0 order by level desc  limit %d ", MAX_RANK_NUM );
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			roleid = ATOUL(row[0]);
			level = ATOW(row[1]);
			listRankLevel.push_back( LEVELID(roleid, level) );
		}
	}

	if (listRankFightvalue.size() < MAX_RANK_NUM)
	{
		DWORD roleid;
		DWORD  rolevalue;
		listRankFightvalue.clear();
		SQLSelect sqlx( DBCtrl::getSQL(), "select roleid, rolevalue  from " TABLE_RANK " where rolevalue>0 order by rolevalue desc  limit %u ", MAX_RANK_NUM );
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			roleid = ATOUL(row[0]);
			rolevalue = ATOW(row[1]);
			listRankFightvalue.push_back( FIGHTID(roleid, rolevalue));
		}
	}

	if (listRankWeaponscore.size() < MAX_RANK_NUM)
	{
		listRankWeaponscore.clear();
		SQLSelect sqlx( DBCtrl::getSQL(), "select roleid,itemid,itemvalue from " TABLE_RANK " where itemvalue>0 order by itemvalue desc  limit %u ", MAX_RANK_NUM );
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			listRankWeaponscore.push_back(WEAPONID( ATOUL(row[0]), ATOW(row[1]), ATOW(row[2]) ));
		}
	}
}

levelRankInfo::levelRankInfo(ROLE * role)
{
	memset(this, 0, sizeof(*this));
	if (role == NULL)
	{
		return;
	}
	memcpy(this->szName, role->roleName.c_str(), sizeof(this->szName));

	if ( role->faction )
	{
		memcpy(this->szGuildName, role->faction->strName.c_str(), sizeof(this->szGuildName));
	}
	
	this->dwID =  role->dwRoleID;
	this->bySex = role->bySex;
	this->byJob = role->byJob;
	this->dwFightValue = role->dwFightValue;
	this->wLevel = role->wLevel;

}

WeaponRankInfo::WeaponRankInfo(ROLE * role, CONFIG::ITEM_CFG *item, DWORD battle_point)
{
	memset(this, 0, sizeof(*this));
	if (role == NULL || item ==NULL)
	{
		return;
	}
	memcpy(this->szRoleName, role->roleName.c_str(), sizeof(this->szRoleName));
	memcpy(this->szWeaponName, item->name.c_str(), sizeof(this->szWeaponName));
	this->dwID =  role->dwRoleID;
	this->bySex = role->bySex;
	this->byJob = role->byJob;
	this->dwWeaponFightValue = battle_point;
}

int RANK::getRanks( ROLE* role, unsigned char * data, size_t dataLen, WORD cmd )
{
	//if (cmd = S_GET_ROLE_RANK_LEVEL)
	//{
	//	return getRankByLevel( role, data, dataLen );
	//}
	//else if (cmd = S_GET_ROLE_RANK_FIGHTVALUE)
	//{
	//	return getRankByFightvalue( role, data, dataLen );
	//}
	//else if (cmd = S_GET_ROLE_WEAPON)
	//{
	//	return getRankByWeaponscore( role, data, dataLen );
	//}
	//else if (cmd = S_GET_ROLE_RANK_MATCH)
	//{
	//	return getRankByMatch( role, data, dataLen );
	//}

	return 0;
}

/*
@客户端请求等级排行榜
*/
int RANK::getRankByLevel( ROLE* role, unsigned char * data, size_t dataLen )
{
	vector<levelRankInfo*> levelRankInfos;
	size_t count = 0;
	for (auto it: listRankLevel)
	{
		ROLE* levelRole = RoleMgr::getMe().getRoleByID( it.dwRoleID );
		if (levelRole == NULL)
		{
			continue;
		}
		levelRankInfo *levelInfo = new levelRankInfo(levelRole);
		levelRankInfos.push_back(levelInfo);
		if ( count++ >= MAX_RANK_NUM)
		{
			break;
		}
	}

	//std::sort(levelRankInfos.begin(), levelRankInfos.end(),
	//			[](levelRankInfo *pOne, levelRankInfo *pTwo)
	//			{
	//				return (pOne->wLevel > pTwo->wLevel);
	//			}
	//		 );

	size_t nums = std::min<size_t>( levelRankInfos.size(), MAX_RANK_NUM );

	string strData;
	for (size_t i=0; i < nums; i++)
	{
		//logwm("roleid = %u get levelRankInfos[%d].dwID = %d ",role->dwRoleID,i,levelRankInfos[i]->dwID);
		S_APPEND_NBYTES( strData, levelRankInfos[i], sizeof(levelRankInfo) );
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ROLE_RANK_LEVEL, strData) );

	for (size_t i=0; i < nums; i++)
	{
		CC_SAFE_DELETE(levelRankInfos[i]);
	}
	return 0;
}


/*
@客户端请求战力排行榜
*/
int RANK::getRankByFightvalue( ROLE* role, unsigned char * data, size_t dataLen )
{
	vector<levelRankInfo*> fightRankInfos;
	size_t count = 0;
	for (auto it: listRankFightvalue)
	{
		ROLE* levelRole = RoleMgr::getMe().getRoleByID( it.dwRoleID );
		if (levelRole == NULL)
		{
			continue;
		}
		levelRankInfo *levelInfo = new levelRankInfo(levelRole);
		fightRankInfos.push_back(levelInfo);
		if ( count++ >= MAX_RANK_NUM)
		{
			break;
		}
	}

	//std::sort(fightRankInfos.begin(), fightRankInfos.end(),
	//			[](levelRankInfo *pOne, levelRankInfo *pTwo)
	//			{
	//				return (pOne->dwFightValue > pTwo->dwFightValue);
	//			}
	//		 );

	size_t nums = std::min<size_t>(fightRankInfos.size(), MAX_RANK_NUM);
	string strData;
	for (size_t i=0; i < nums; i++)
	{
		//logwm("roleid = %u get fightRankInfos[%d].dwID = %d dwFightValue=%u",role->dwRoleID,i,fightRankInfos[i]->dwID,fightRankInfos[i]->dwFightValue);
		S_APPEND_NBYTES( strData, (char*)fightRankInfos[i], sizeof(levelRankInfo) );

	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ROLE_RANK_FIGHTVALUE, strData) );

	for (size_t i=0; i < nums; i++)
	{
		CC_SAFE_DELETE(fightRankInfos[i]);
	}
	return 0;
}

/*
@客户端请求武器排行榜
*/
int RANK::getRankByWeaponscore( ROLE* role, unsigned char * data, size_t dataLen )
{
	vector<WeaponRankInfo*> weaponRankInfos;
	size_t count = 0;
	for (auto it: listRankWeaponscore)
	{
		ROLE* levelRole = RoleMgr::getMe().getRoleByID( it.dwRoleID );
		CONFIG::ITEM_CFG * item = CONFIG::getItemCfg( it.wItemID );
		if (levelRole == NULL || item == NULL)
		{
			continue;
		}
		WeaponRankInfo *weaponInfo =  new WeaponRankInfo(levelRole, item, (DWORD)it.dwFightValue);
		weaponRankInfos.push_back(weaponInfo);
		if ( count++ >= MAX_RANK_NUM )
		{
			break;
		}
	}

	//sort(weaponRankInfos.begin(), weaponRankInfos.end(),
	//		[](WeaponRankInfo* pone, WeaponRankInfo* ptwo)
	//		{
	//			return ( pone->dwWeaponFightValue > ptwo->dwWeaponFightValue);	
	//		}
	//	);

	size_t nums = std::min<size_t>(weaponRankInfos.size(), MAX_RANK_NUM);
	string strData;
	for (size_t i=0; i < nums; i++)
	{
		//logwm("roleid = %u get weaponRankInfos[%d].dwID = %d dwFightValue=%u",role->dwRoleID,i,weaponRankInfos[i]->dwID,weaponRankInfos[i]->dwWeaponFightValue);
		S_APPEND_NBYTES( strData, (char*)weaponRankInfos[i], sizeof(WeaponRankInfo) );

	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ROLE_WEAPON, strData) );

	for (size_t i=0; i < nums; i++)
	{
		CC_SAFE_DELETE(weaponRankInfos[i]);
	}
	return 0;
}


/*
@客户端请求竞技场排行榜
*/
int RANK::getRankByMatch( ROLE* role, unsigned char * data, size_t dataLen )
{
	vector<levelRankInfo*> matchRankInfos;
	list<DWORD> listRankMatch;

	ARENA::getArenaRankID(listRankMatch, MAX_RANK_NUM);

	for (auto it: listRankMatch)
	{
		ROLE* matchRole = RoleMgr::getMe().getRoleByID( it );
		if (matchRole == NULL)
		{
			continue;
		}
		levelRankInfo *levelInfo = new levelRankInfo(matchRole);
		matchRankInfos.push_back(levelInfo);
	}

	//sort(matchRankInfos.begin(), matchRankInfos.end(),
	//		[](levelRankInfo *pOne, levelRankInfo *pTwo)
	//		{
	//			return pOne->dwFightValue > pTwo->dwFightValue;	
	//		}
	//	);

	size_t nums = std::min<size_t>(matchRankInfos.size(), MAX_RANK_NUM);
	string strData;
	for (size_t i=0; i < nums; i++)
	{
		//logwm("get matchRankInfo[%d].dwID = %d",i,matchRankInfos[i]->dwID);
		S_APPEND_NBYTES( strData, (char*)matchRankInfos[i], sizeof(levelRankInfo) );
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ROLE_RANK_MATCH, strData) );

	for (size_t i=0; i < nums; i++)
	{
		CC_SAFE_DELETE(matchRankInfos[i]);
	}

	return 0;
}

/*
@武器排行榜添加
*/
void RANK::insertWeaponListRank(ROLE * role, ITEM_CACHE* itemCache)
{
	if (role == NULL || itemCache == NULL)
	{
		return;
	}
	
	CONFIG::ITEM_CFG *itemCfg = itemCache->itemCfg;
	if (itemCfg == NULL || itemCfg->type != SUB_TYPE_WEAPON || itemCfg->sub_type != SUB_TYPE_WEAPON )
	{
		return;
	}

	DWORD battle_point = itemCache->dwFightValue;

	for (auto it = listRankWeaponscore.begin(); it != listRankWeaponscore.end(); ++it)
	{
		if (role->dwRoleID == it->dwRoleID)
		{
			it->wItemID = itemCfg->id;
			it->dwFightValue = battle_point;
			return;
		}
	}

	if ( NOTFULL( listRankWeaponscore ) )
	{
		//logwm("notfull RoleID=%d, itemid=%d, battle_point=%d\n",role->dwRoleID, itemCfg->id, battle_point);
		listRankWeaponscore.push_back(WEAPONID(role->dwRoleID, itemCfg->id, battle_point));

		listRankWeaponscore.sort(	[](WEAPONID one, WEAPONID two)
		{
			return one.dwFightValue > two.dwFightValue;	
		}
		);//降序

		return;
	}

	listRankWeaponscore.sort(	[](WEAPONID one, WEAPONID two)
								{
									return one.dwFightValue > two.dwFightValue;	
								}
							);//降序

	for (auto it = listRankWeaponscore.begin(); it != listRankWeaponscore.end(); ++it)
	{
		if (battle_point > it->dwFightValue)
		{
			//logwm("full RoleID=%d, itemid=%d, battle_point=%d\n",role->dwRoleID, itemCfg->id, battle_point);
			listRankWeaponscore.insert(it, WEAPONID(role->dwRoleID, itemCfg->id, battle_point));
			//listRankWeaponscore.pop_back();//有可能排行榜中玩家卸掉装备
			if (listRankWeaponscore.size() > MAX_RANK_NUM)
			{
				listRankWeaponscore.pop_back();
			}
			break;
		}
	}	
}

/*
@武器排行榜删除
*/
void RANK::eraseWeaponListRank(DWORD roleID, WORD weaponID)
{
	//listRankWeaponscore.remove( WEAPONID(roleID, weaponID) );
}

/*
@插入排行榜
*/
void RANK::insertRanks(ROLE * role)
{
	//insertLevelRanks(role);
	//insertFightvalueRanks(role);
	//insertWeaponListRank( role, role->vBodyEquip[BODY_INDEX_WEAPON] );
}

/*
@插入等级排行榜
*/
void RANK::insertLevelRanks(ROLE * role)
{
	if (role == NULL)
	{
		return;
	}
	if (role ->wLevel > 99)
	{
		//logwm("id: %d ,level; %d ",role->dwRoleID, role->wLevel);
		return;
	}
	for (auto it = listRankLevel.begin(); it != listRankLevel.end(); ++it)// wm 同一个人被重复插入
	{
		if (role->dwRoleID == it->dwRoleID)
		{
			it->wLevel = role->wLevel;
			return;
		}
	}

	if ( NOTFULL( listRankLevel ) )
	{
		listRankLevel.push_back(LEVELID(role->dwRoleID, role->wLevel));
		listRankLevel.sort(	[](LEVELID one, LEVELID two)
		{
			return one.wLevel > two.wLevel;	
		}
		);//降序
		return;
	}

	listRankLevel.sort(	[](LEVELID one, LEVELID two)
						{
							return one.wLevel > two.wLevel;	
						}
					  );//降序

	for (auto it = listRankLevel.begin(); it != listRankLevel.end(); ++it)
	{
		if (role->wLevel > it->wLevel)
		{
			listRankLevel.insert(it, LEVELID(role->dwRoleID, role->wLevel));
			if (listRankLevel.size() > MAX_RANK_NUM)
			{
				listRankLevel.pop_back();
			}
			break;
		}
	}	
}

/*
@插入战力排行榜
*/
void RANK::insertFightvalueRanks(const ROLE * role)
{
	if (role == NULL)
	{
		return;
	}

	for (auto it = listRankFightvalue.begin(); it != listRankFightvalue.end(); it++)
	{
		if (role->dwRoleID == it->dwRoleID)
		{
			it->dwFightValue = role->dwFightValue;
			return;
		}
	}

	if ( NOTFULL( listRankFightvalue ) )
	{
		listRankFightvalue.push_back(FIGHTID(role->dwRoleID, role->dwFightValue));
		listRankFightvalue.sort(	[](FIGHTID one, FIGHTID two)
		{
			return one.dwFightValue > two.dwFightValue;	
		}
		);//降序
		return;
	}

	listRankFightvalue.sort(	[](FIGHTID one, FIGHTID two)
								{
									return one.dwFightValue > two.dwFightValue;	
								}
						   );//降序

	for (auto it = listRankFightvalue.begin(); it != listRankFightvalue.end(); ++it)
	{
		if (role->dwFightValue > it->dwFightValue)
		{
			listRankFightvalue.insert(it, FIGHTID(role->dwRoleID, role->dwFightValue));
			if (listRankFightvalue.size() > MAX_RANK_NUM)
			{
				listRankFightvalue.pop_back();
			}
			break;
		}
	}		
}