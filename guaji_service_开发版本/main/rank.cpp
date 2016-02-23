#include "pch.h"
#include "rank.h"
#include "roleCtrl.h"
#include "mysqlCtrl.h"
#include "db.h"
#include "arena.h" 
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

	for (auto it:listRankLevel)
	{
		if ( ! EXEC_SQL( "insert into  " TABLE_RANK " (roleid, level) values('%u', '%u') ",it.dwRoleID, it.wLevel) )
		{
			return ;
		}
	}

	for (auto it:listRankFightvalue)
	{
		if ( ! EXEC_SQL( "insert into  " TABLE_RANK " (roleid, rolevalue) values('%u', '%u') ",it.dwRoleID, it.dwFightValue) )
		{
			return ;
		}
	}
	for (auto it:listRankWeaponscore)
	{
		if ( ! EXEC_SQL( "insert into  " TABLE_RANK " (roleid, itemid, itemvalue) values('%u','%u', '%u') ",it.dwRoleID, it.wItemID, it.dwFightValue) )
		{
			return ;
		}
	}
}

/*
@初始化列表，服务器启动时候用
*/
void RANK::initDB2Ranks()
{		
	{
		DWORD roleid;
		WORD  level;
		listRankLevel.clear();
		SQLSelect sqlx( DBCtrl::getSQL(), "select roleid, level from " TABLE_RANK " where level>0 order by level desc  limit %d ", MAX_RANK_NUM );
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			BREAK_COND(listRankLevel.size() >= MAX_RANK_NUM);
			roleid = ATOUL(row[0]);
			level = ATOW(row[1]);
			listRankLevel.push_back( LEVELID(roleid, level) );
		}
	}


	{
		DWORD roleid;
		DWORD  rolevalue;
		listRankFightvalue.clear();
		SQLSelect sqlx( DBCtrl::getSQL(), "select roleid, rolevalue  from " TABLE_RANK " where rolevalue>0 order by rolevalue desc  limit %u ", MAX_RANK_NUM );
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			BREAK_COND( listRankFightvalue.size() >= MAX_RANK_NUM );
			roleid = ATOUL(row[0]);
			rolevalue = ATOW(row[1]);
			listRankFightvalue.push_back( FIGHTID(roleid, rolevalue));
		}
	}


	{
		listRankWeaponscore.clear();
		SQLSelect sqlx( DBCtrl::getSQL(), "select roleid,itemid,itemvalue from " TABLE_RANK " where itemvalue>0 order by itemvalue desc  limit %u ", MAX_RANK_NUM );
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			BREAK_COND(listRankWeaponscore.size() >= MAX_RANK_NUM)
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
		memcpy(this->szGuildName, role->faction->name.c_str(), sizeof(this->szGuildName));
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
	return 0;
}

/*
@客户端请求等级排行榜
*/
int RANK::getRankByLevel( ROLE* role, unsigned char * data, size_t dataLen )
{
	size_t count = 0;
	string strData;
	size_t nums = std::min<size_t>(listRankLevel.size(), MAX_RANK_NUM);
	for (auto it: listRankLevel)
	{
		ROLE* levelRole = RoleMgr::getMe().getRoleByID( it.dwRoleID );
		if (levelRole == NULL)
		{
			continue;
		}
		levelRankInfo levelRankInfos(levelRole);
		S_APPEND_NBYTES(strData, (char*)&levelRankInfos, sizeof(levelRankInfo));
		if (count++ >= nums)
		{
			break;
		}
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ROLE_RANK_LEVEL, strData) );
	return 0;
}


/*
@客户端请求战力排行榜
*/
int RANK::getRankByFightvalue( ROLE* role, unsigned char * data, size_t dataLen )
{
	size_t count = 0;
	size_t nums = std::min<size_t>(listRankFightvalue.size(), MAX_RANK_NUM);
	string strData;
	for (auto it: listRankFightvalue)
	{
		ROLE* levelRole = RoleMgr::getMe().getRoleByID( it.dwRoleID );
		if (levelRole == NULL)
		{
			continue;
		}
		levelRankInfo fightRankInfos(levelRole);
		S_APPEND_NBYTES(strData, (char*)&fightRankInfos, sizeof(levelRankInfo));
		if (count++ >= nums)
		{
			break;
		}
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ROLE_RANK_FIGHTVALUE, strData) );
	return 0;
}

/*
@客户端请求武器排行榜
*/
int RANK::getRankByWeaponscore( ROLE* role, unsigned char * data, size_t dataLen )
{
	size_t count = 0;
	string strData;
	size_t nums = std::min<size_t>(listRankWeaponscore.size(), MAX_RANK_NUM);
	for (auto it: listRankWeaponscore)
	{
		ROLE* levelRole = RoleMgr::getMe().getRoleByID( it.dwRoleID );
		CONFIG::ITEM_CFG * item = CONFIG::getItemCfg( it.wItemID );
		if (levelRole == NULL || item == NULL)
		{
			continue;
		}
		WeaponRankInfo weaponRankInfos(levelRole, item, (DWORD)it.dwFightValue);
		S_APPEND_NBYTES(strData, (char*)&weaponRankInfos, sizeof(WeaponRankInfo));
		if ( count++ >= nums )
		{
			break;
		}
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ROLE_WEAPON, strData) );
	return 0;
}


/*
@客户端请求竞技场排行榜
*/
int RANK::getRankByMatch( ROLE* role, unsigned char * data, size_t dataLen )
{
	vector<levelRankInfo*> matchRankInfos;
	list<uint> listRankMatch;

	ARENA::getArenaRankID(listRankMatch, MAX_RANK_NUM);
	//size_t nums = std::min<size_t>(listRankMatch.size(), MAX_RANK_NUM);
	string strData;
	for (auto it: listRankMatch)
	{
		ROLE* matchRole = RoleMgr::getMe().getRoleByID( it );
		if (matchRole == NULL)
		{
			continue;
		}
		levelRankInfo matchRankInfos(matchRole);
		S_APPEND_NBYTES(strData, (char*)&matchRankInfos, sizeof(levelRankInfo));
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ROLE_RANK_MATCH, strData) );
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

	if (role->gm_account )
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
			goto end;
		}
	}

	if ( NOTFULL( listRankWeaponscore ) )
	{
		listRankWeaponscore.push_back(WEAPONID(role->dwRoleID, itemCfg->id, battle_point));
		goto end;
	}


	for (auto it = listRankWeaponscore.begin(); it != listRankWeaponscore.end(); ++it)
	{
		if (battle_point > it->dwFightValue)
		{
			listRankWeaponscore.insert(it, WEAPONID(role->dwRoleID, itemCfg->id, battle_point));

			if (listRankWeaponscore.size() > MAX_RANK_NUM)
			{
				listRankWeaponscore.pop_back();
			}
			goto end;
		}
	}

end:
	listRankWeaponscore.sort([](WEAPONID one, WEAPONID two)
	{
		return one.dwFightValue > two.dwFightValue;
	}
	);//降序
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

	if ( role->gm_account || role->wLevel > 99 )
	{
		return;
	}

	for (auto it = listRankLevel.begin(); it != listRankLevel.end(); ++it)//同一个人被重复插入
	{
		if (role->dwRoleID == it->dwRoleID)
		{
			it->wLevel = role->wLevel;
			goto end;
		}
	}

	if ( NOTFULL( listRankLevel ) )
	{
		listRankLevel.push_back(LEVELID(role->dwRoleID, role->wLevel));
		goto end;
	}


	for (auto it = listRankLevel.begin(); it != listRankLevel.end(); ++it)
	{
		if (role->wLevel > it->wLevel)
		{
			listRankLevel.insert(it, LEVELID(role->dwRoleID, role->wLevel));
			if (listRankLevel.size() > MAX_RANK_NUM)
			{
				listRankLevel.pop_back();
			}
			goto end;
		}
	}

end:
	listRankLevel.sort([](LEVELID one, LEVELID two)
	{
		return one.wLevel > two.wLevel;
	}
	);//降序
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

	if (role->gm_account)
	{
		return;
	}

	for (auto it = listRankFightvalue.begin(); it != listRankFightvalue.end(); it++)
	{
		if (role->dwRoleID == it->dwRoleID)
		{
			it->dwFightValue = role->dwFightValue;
			goto end;
		}
	}

	if ( NOTFULL( listRankFightvalue ) )
	{
		listRankFightvalue.push_back(FIGHTID(role->dwRoleID, role->dwFightValue));
		goto end;
	}


	for (auto it = listRankFightvalue.begin(); it != listRankFightvalue.end(); ++it)
	{
		if (role->dwFightValue > it->dwFightValue)
		{
			listRankFightvalue.insert(it, FIGHTID(role->dwRoleID, role->dwFightValue));
			if (listRankFightvalue.size() > MAX_RANK_NUM)
			{
				listRankFightvalue.pop_back();
			}
			goto end;
		}
	}

end:
	listRankFightvalue.sort([](FIGHTID one, FIGHTID two)
	{
		return one.dwFightValue > two.dwFightValue;
	}
	);//降序
}


uint RANK::get_level_top_id()
{
	return listRankLevel.empty() ? 0 : (listRankLevel.front().dwRoleID);
}
uint RANK::get_fightvalue_top_id()
{
	return listRankFightvalue.empty() ? 0 : (listRankFightvalue.front().dwRoleID);
}
uint RANK::get_weapon_top_id()
{
	return listRankWeaponscore.empty() ? 0 : (listRankWeaponscore.front().dwRoleID);
}
