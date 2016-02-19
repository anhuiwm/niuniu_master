#include "arena.h"
#include "algorithm"
#include "roleCtrl.h"
#include "mysqlCtrl.h"
#include "db.h"
#include "itemCtrl.h"
#include <cstdio>
#include "skill.h"
#include "mailCtrl.h"
#include "logs_db.h"
#include "service.h"


namespace ARENA
{
	static map<DWORD, DWORD> mapRankRoleID;
	static map<DWORD, DWORD> mapRoleIDRank;
	void refreshCompetitor(ROLE* role);
	static WORD _ArenaCost[COST_TIMES] = { 50,50,50,60,70,80,90,100,100,100 };
}



#pragma pack(push, 1)

struct CompetitorInfo
{
	char  szName[22];
	DWORD dwID;
	BYTE  bySex;
	BYTE  byJob;
	DWORD dwFightValue;
	DWORD dwRank;
	DWORD dwMoney;
	WORD  wLevel;
	WORD  wChipID;
	WORD  wChipNum;
	ZERO_CONS(CompetitorInfo);
	CompetitorInfo(ROLE* role)
	{
		memset(this, 0, sizeof(*this));
		RETURN_VOID(role == NULL);
		memcpy(this->szName, role->roleName.c_str(), sizeof(this->szName)-1);
		this->dwID =  role->dwRoleID;
		this->bySex = role->bySex;
		this->byJob = role->byJob;
		this->dwFightValue = role->dwFightValue;
		this->wLevel = role->wLevel;

		auto it = ARENA::mapRoleIDRank.find( role->dwRoleID );
		if ( it != ARENA::mapRoleIDRank.end() )
		{
			this->dwRank = it->second;
		}

		CONFIG::ARENA_BOUNS_CFG *arenaBounsCfg = CONFIG::getArenaBounsCfg(this->dwRank);
		if( arenaBounsCfg != NULL)
		{
			this->dwMoney = arenaBounsCfg->goldNum;
			this->wChipID = arenaBounsCfg->itemID;
			this->wChipNum = arenaBounsCfg->itemNum;
		}
	}
};


struct TEnemyInfo
{
	char  szName[22];
	DWORD dwID;
	BYTE  bySex;
	BYTE  byJob;
	WORD  wLevel;
	//DWORD dwFightValue;
	ZERO_CONS(TEnemyInfo);
	TEnemyInfo(ROLE* role)
	{
		strncpy(this->szName, role->roleName.c_str(), sizeof(this->szName)-1);
		this->dwID =  role->dwRoleID;
		this->bySex = role->bySex;
		this->byJob = role->byJob;
		this->wLevel = role->wLevel;
		//this->dwFightValue = role->dwFightValue;
	}
};

#pragma pack(pop)




/*
@获取推荐对手
*/
int ARENA::getArenaRecommend( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byExchange = 0;
	if ( !BASE::parseBYTE( data, dataLen, byExchange) )
		return 0;

	toClienCompetitors(role, !!byExchange);

	return 0;

}

/*
@战斗前
*/
int ARENA::arenaFightPre( ROLE* role, unsigned char * data, size_t dataLen )
{

	//#define	    S_ARENA_FIGHT_PRE			0x0505		//area fight pre
	//C:[DWORD]
	//	[对手ID]
	//
	//S:[ BYTE  + char[22],  BYTE, BYTE, DWORD, DWORD ]n个
	//	[换回吗 + 名字    ,  性别, 职业, hp ,    mp	]n个

	BYTE byRet = 0;
	string strData;

#if 0
	byRet = ARENA_NOT_NEED_BUY ;
	S_APPEND_BYTE( strData, byRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ARENA_FIGHT_PRE, strData) );
	return 0;
#endif

	if (role->dwComID != ARENA_NO_FIGHT)
	{
		byRet = ARENA_ALREADY_FIGHT ;
		S_APPEND_BYTE( strData, byRet);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ARENA_FIGHT_PRE, strData) );
		return 0;
	}

	DWORD dwID;
	if ( !BASE::parseDWORD( data, dataLen, dwID) )
		return 0;

	ROLE* comRole = RoleMgr::getMe().getRoleByID( dwID );
	if (comRole == NULL)
	{
		byRet = ARENA_NO_EXISTS ;
		S_APPEND_BYTE( strData, byRet);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ARENA_FIGHT_PRE, strData) );
		return 0;
	}


	if (MAX_ARENA_USE_NUMS + role->byBuyArenaDayCount <= role->byArenaDayCount)
	{
		byRet = ARENA_NOT_NEED_BUY ;
		S_APPEND_BYTE( strData, byRet);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ARENA_FIGHT_PRE, strData) );
		return 0;
	}

	S_APPEND_BYTE( strData, byRet);

	TEnemyInfo stEnemy( comRole );
	
	S_APPEND_NBYTES( strData, (char*)&stEnemy, sizeof(TEnemyInfo) );

	for ( auto& it : comRole->cMerMgr.m_vecCMercenary )
	{
		if ( it.byFight == E_FIGHT_ON )
		{
			S_APPEND_WORD( strData, it.wID);
		}
	}
	
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ARENA_FIGHT_PRE, strData) );

	role->dwComID = comRole->dwRoleID;

	return 0;
}

/*
@地图偶遇战斗前
*/
int ARENA::meetMapFightPre( ROLE* role, unsigned char * data, size_t dataLen )
{
//	--地图偶遇战斗预请求战斗( 同竞技场 S_ARENA_FIGHT_PRE(0x0505))
//#define	S_OUYU_FIGHT_PRE 0x005E
//C:[DWORD]
//	[对手ID]
//S:[ BYTE ] + [char[22],DWORD,  BYTE, BYTE,  WORD  ]1个 + [ WORD ]n个
//	  [还回吗] + [名字    ,  ID ,  性别, 职业, wLevel ]1个 + [  ID  ]n个
	
	BYTE byRet = 0;
	DWORD dwID;
	string strData;

	if ( !BASE::parseDWORD( data, dataLen, dwID) )
		return 0;

	if ( role->meet_pk_target_roleid != ARENA_NO_FIGHT )
	{
		byRet = ARENA_ALREADY_FIGHT ;
		S_APPEND_BYTE( strData, byRet);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_OUYU_FIGHT_PRE, strData) );
		return 0;
	}

	ROLE* comRole = RoleMgr::getMe().getRoleByID( dwID );
	if (comRole == NULL)
	{
		byRet = ARENA_NO_EXISTS ;
		S_APPEND_BYTE( strData, byRet);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_OUYU_FIGHT_PRE, strData) );
		return 0;
	}

	if ( role->pk_count <= 0 )
	{
		byRet = ARENA_NOT_NEED_BUY ;
		S_APPEND_BYTE( strData, byRet);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_OUYU_FIGHT_PRE, strData) );
		return 0;
	}

	S_APPEND_BYTE( strData, byRet);

	TEnemyInfo stEnemy( comRole );

	S_APPEND_NBYTES( strData, (char*)&stEnemy, sizeof(TEnemyInfo) );

	for ( auto& it : comRole->cMerMgr.m_vecCMercenary )
	{
		if ( it.byFight == E_FIGHT_ON )
		{
			S_APPEND_WORD( strData, it.wID);
		}
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_OUYU_FIGHT_PRE, strData) );

	role->meet_pk_target_roleid = comRole->dwRoleID;

	return 0;
}


/*
@购买比赛次数
*/
int ARENA::buyArenaFightCount( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byErrCode  = ARENA_SUCCESS ;
	
	WORD index = role->byBuyArenaDayCount;

	DWORD dwCostGold = index <= (COST_TIMES - 1) ? _ArenaCost[index] : _ArenaCost[COST_TIMES - 1];

	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg( role->getVipLevel() );
	if (vipCfg != NULL && vipCfg->match_buy > role->byBuyArenaDayCount)
	{
		if (role->dwIngot < dwCostGold )
		{
			byErrCode  = GOLD_NOT_ENOUGH ;
		}
		else
		{
			ADDUP(role->dwIngot, -(int)(dwCostGold) );
			role->byBuyArenaDayCount++;
			notifyIngot(role);
			logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID,
				0, -(int)dwCostGold, purpose::buy_arena_battle_count, 0);
		}
	}
	else
	{
		byErrCode  = ARENA_LEVEL_NOTENOUGH ;//vip等级不足
	}	


	string strData;
	S_APPEND_BYTE( strData, byErrCode);
	S_APPEND_BYTE( strData, (MAX_ARENA_USE_NUMS + role->byBuyArenaDayCount - role->byArenaDayCount));
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ARENA_BUY_COUNT, strData) );
	return 0;
}

int ARENA::buyArenaCountGold( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byTimes = 0;
	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
	if ( vipCfg != NULL && vipCfg->match_buy > role->byBuyArenaDayCount)
	{
		byTimes = vipCfg->match_buy - role->byBuyArenaDayCount;
	}	

	WORD index = role->byBuyArenaDayCount;

	DWORD dwGold = index <= (COST_TIMES - 1) ? _ArenaCost[index] : _ArenaCost[COST_TIMES - 1];

	string strData;
	S_APPEND_DWORD( strData, dwGold);
	S_APPEND_BYTE( strData, byTimes);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ARENA_BUY_GOLD, strData) );
	return 0;
}
/*
@请求竞技场记录
*/
int ARENA::requireArenaRecord( ROLE* role, unsigned char * data, size_t dataLen )
{

	vector< ARENA_RECORD > vecRecordInfos;

	size_t nums = std::min<size_t>(role->listRenaRecord.size(), MAX_ARENA_RECORD);

	if (role->wLevel < ARENA_NEED_LEVEL)
	{
		return 0;
	}

	for (auto it = role->listRenaRecord.begin(); it != role->listRenaRecord.end() && nums > 0; ++it)
	{
		vecRecordInfos.push_back( *it );
		nums--;
	}
	string strData;
	for (size_t i=0; i < vecRecordInfos.size(); i++)
	{
		S_APPEND_NBYTES( strData, (char*)&vecRecordInfos[i], sizeof(ARENA_RECORD) );
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_GET_ARENA_RECORD, strData ) );
	return 0;
}

/*
@刷新竞技场对手
*/
void ARENA::refreshCompetitor(ROLE* role)
{
	//srand(time(NULL) + getpid());
	vector<DWORD> vecIDs;
	role->vecReanaArmy.clear();

	auto it = mapRoleIDRank.find( role->dwRoleID );
	if ( it == mapRoleIDRank.end() )//找不到自己
	{
		for (auto it= mapRankRoleID.rbegin(); it != mapRankRoleID.rend(); ++it)
		{
			vecIDs.push_back(it->first);
			if (vecIDs.size() >= ARENA_COMPETITOR_NUMS)
			{
				break;
			}
		}
	}
	else
	{
		DWORD rank = it->second;

		if (mapRankRoleID.size() <= ARENA_COMPETITOR_NUMS)//竞技场不足四个人
		{
			for (auto idRank : mapRankRoleID)
			{
				if (role->dwRoleID == idRank.second)
				{
					continue;
				}
				vecIDs.push_back(idRank.second);
			}
		}
		else if (rank <= ARENA_COMPETITOR_NUMS)//排名前四
		{
			for (auto idRank : mapRankRoleID)
			{
				if (role->dwRoleID == idRank.second)
				{
					continue;
				}
				vecIDs.push_back(idRank.second);
				if (vecIDs.size() >= ARENA_COMPETITOR_NUMS)
				{
					break;
				}
			}
		}
		else if (rank <= 20)
		{
			vecIDs.push_back( mapRankRoleID[rank -1] );
			vecIDs.push_back (mapRankRoleID[rank -2] );
			vecIDs.push_back( mapRankRoleID[rank -3] );
			vecIDs.push_back (mapRankRoleID[rank -4] );
		}
		else if (rank <= 50)
		{
			vecIDs.push_back( mapRankRoleID[rank - 1 - rand()%3] );
			vecIDs.push_back (mapRankRoleID[rank - 4 - rand()%3] );
			vecIDs.push_back( mapRankRoleID[rank - 7 - rand()%3] );
			vecIDs.push_back (mapRankRoleID[rank - 10 - rand()%5] );
		}
		else if (rank <= 100)
		{
			vecIDs.push_back( mapRankRoleID[rank - 2 - rand()%5] );
			vecIDs.push_back (mapRankRoleID[rank - 7 - rand()%7] );
			vecIDs.push_back( mapRankRoleID[rank - 15 - rand()%11] );
			vecIDs.push_back (mapRankRoleID[rank - 27 - rand()%17] );
		}
		else if (rank <= 1000)
		{
			vecIDs.push_back( mapRankRoleID[rank - 1 - rand()%19] );
			vecIDs.push_back (mapRankRoleID[rank - 20 - rand()%21] );
			vecIDs.push_back( mapRankRoleID[rank - 49 - rand()%23] );
			vecIDs.push_back (mapRankRoleID[rank - 75 - rand()%11] );
		}
		else 
		{
			vecIDs.push_back( mapRankRoleID[rank - 5 - rand()%11] );
			vecIDs.push_back (mapRankRoleID[rank - 30 - rand()%41] );
			vecIDs.push_back( mapRankRoleID[rank - 150 - rand()%101] );
			vecIDs.push_back (mapRankRoleID[rank - 600 - rand()%201] );
		}
	}

	role->vecReanaArmy.assign(vecIDs.begin(),vecIDs.end());
}

int ARENA::initArenaRoles()
{
	if (mapRankRoleID.size() < RANK_MAX_ROLES_NUMS)
	{
		mapRoleIDRank.clear();
		mapRankRoleID.clear();
		DWORD ID;
		DWORD rank;
		SQLSelect sqlx( DBCtrl::getSQL(), "select roleid, arenarank from " TABLE_RANK " where arenarank>0 order by arenarank asc  limit %d ", RANK_MAX_ROLES_NUMS );
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			ID = ATOUL(row[0]);
			rank = ATOUL(row[1]);
			if (mapRoleIDRank.find(ID) == mapRoleIDRank.end())
			{
				mapRankRoleID.insert(make_pair(rank, ID));
				mapRoleIDRank.insert(make_pair(ID, rank));
			}
		}
	}
	return 0;
}

// 获得竞技场前10名角色id
std::set<DWORD> ARENA::getArenaTop10RoleIds()
{
	std::set<DWORD> setRoleIds;

	for (DWORD i=1; i<=10; i++)
	{
		auto itr = mapRankRoleID.find(i);
		if (itr != mapRankRoleID.end())
		{
			setRoleIds.insert(itr->second);
		}
	}

	return setRoleIds;
}

void ARENA::writeArenaRank2DB()
{
	for (auto it:mapRoleIDRank)
	{
		//logwm("id=%d  rank=%d\n",it.first,it.second);
		if ( ! EXEC_SQL( "insert into  " TABLE_RANK " (roleid, arenarank) values('%d', '%d') ", it.first, it.second) )//要改
		{
			return ;
		}

	}
}

void ARENA:: insertArenaRank(ROLE* role)
{
	if (role->wLevel >= ARENA_NEED_LEVEL)
	{
		DWORD rank = mapRoleIDRank.size() + 1;
		DWORD ID = role->dwRoleID;
		if (mapRoleIDRank.find(role->dwRoleID) == mapRoleIDRank.end() && rank <= RANK_MAX_ROLES_NUMS)
		{
			mapRankRoleID.insert( make_pair( rank, ID ) );
			mapRoleIDRank.insert( make_pair( ID, rank ) );
		}
	}
}

void ARENA:: swapArenaRank(ROLE* role, ROLE* comRole, BYTE byResult)
{
	if ( byResult != E_MATCH_STATUS_WIN )//不赢不要换排名
	{
		return;
	}
	auto itCom = mapRoleIDRank.find(comRole->dwRoleID);
	if (  itCom == mapRoleIDRank.end())//对手排名不在一千之内不换
	{
		return;
	}

	DWORD rankRole = 0;
	DWORD rankcomRole = mapRoleIDRank[comRole->dwRoleID];
	BYTE  byComResult = E_MATCH_STATUS_LOSS;//对手的结果

	if (mapRoleIDRank.find(role->dwRoleID) == mapRoleIDRank.end())//自己无排名,一千之外
	{

		mapRoleIDRank.erase(itCom);
		mapRoleIDRank.insert( make_pair(role->dwRoleID, rankcomRole) );
		
		mapRankRoleID[rankcomRole] = role->dwRoleID;

		return;
	}
	else
	{
		rankRole = mapRoleIDRank[role->dwRoleID];
		if ( (rankRole < rankcomRole) )
		{
			return;
		}

		mapRoleIDRank[role->dwRoleID] = rankcomRole;
		mapRoleIDRank[comRole->dwRoleID] = rankRole;

		mapRankRoleID[rankRole] = comRole->dwRoleID;
		mapRankRoleID[rankcomRole] = role->dwRoleID;
	}

	role->listRenaRecord.push_back( ARENA_RECORD( byResult, comRole->roleName.c_str(), rankcomRole, PROTOCAL::getCurTime(), comRole->byJob, comRole->bySex, comRole->wLevel, comRole->dwRoleID ));
	comRole->listRenaRecord.push_back( ARENA_RECORD( byComResult, role->roleName.c_str(), rankRole, PROTOCAL::getCurTime(), role->byJob, role->bySex, role->wLevel, role->dwRoleID ));

	if (role->listRenaRecord.size() > MAX_ARENA_RECORD)
	{
		role->listRenaRecord.pop_front();
	}
	if (comRole->listRenaRecord.size() > MAX_ARENA_RECORD)
	{
		comRole->listRenaRecord.pop_front();
	}
}

/*
@ 根据战斗流程会改
*/
void ARENA::toClienCompetitors(ROLE* role, bool bExchange)
{
	list< CompetitorInfo > listCompetitorInfos;

	if (role->wLevel >= ARENA_NEED_LEVEL)
	{
		if (bExchange)
		{
			refreshCompetitor(role);
		}
		size_t nums = std::min<size_t>(role->vecReanaArmy.size(), ARENA_COMPETITOR_NUMS);

		for (size_t i=0; i < nums; i++)
		{
			ROLE* comRole = RoleMgr::getMe().getRoleByID( role->vecReanaArmy[i] );
			if (comRole == NULL)
			{
				continue;
			}
			CompetitorInfo comInfo(comRole);
			if (comInfo.dwRank != 0)
			{
				listCompetitorInfos.push_back(comInfo);
			}
		}

		//自己信息已经在第一个,其他的按排名值从小到大
		listCompetitorInfos.sort(
			[](CompetitorInfo one, CompetitorInfo two)
		{
			return  (one.dwRank < two.dwRank);
		}
		);

	}
	listCompetitorInfos.push_front( CompetitorInfo(role) );

	string strData;
	S_APPEND_BYTE( strData,  ( MAX_ARENA_USE_NUMS + role->byBuyArenaDayCount - role->byArenaDayCount ));

	for (auto info : listCompetitorInfos)
	{
		S_APPEND_NBYTES( strData, (char*)&info, sizeof(CompetitorInfo) );
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_GET_ARENA_RECOMMEND, strData ) );

}

void ARENA::getArenaRankID(list<DWORD>& listRankMatch, size_t nums)//竞技场排行用
{
	nums = std::min<size_t>(nums, mapRankRoleID.size());
	for (auto it : mapRankRoleID)
	{
		if (listRankMatch.size() >= nums)
		{
			break;
		}
		listRankMatch.push_back(it.second);
	}
}

DWORD ARENA::getRankByRoleID(const DWORD id)
{
	DWORD dwRank = 0;
	auto it  = mapRoleIDRank.find( id );
	if (it != mapRoleIDRank.end() )
	{
		dwRank = it->second;
	}
	return dwRank;
}
/*
@竞技场战斗报告
*/
int ARENA::fight_report( ROLE* role, const DWORD target_role_id, const BYTE byResult, DWORD* pvAdd)
{

	RETURN_COND( byResult!=E_MATCH_STATUS_LOSS && byResult!=E_MATCH_STATUS_WIN, 0 );

	ROLE* comRole = RoleMgr::getMe().getRoleByID( target_role_id );

	RETURN_COND( comRole == NULL, 0 );

	DWORD dwRank = getRankByRoleID(target_role_id);

	RETURN_COND( dwRank == 0, 0 );

	const CONFIG::ARENA_REWARD_CFG* arenaRewardCfg = CONFIG::getArenaRewardCfg(dwRank);

	RETURN_COND( arenaRewardCfg == NULL, 0 );

	int nAddCoin = 0;
	int nAddGold = 0;
	bool bChange = false;

	role->dwComID = ARENA_NO_FIGHT;//战斗结束

	role->byArenaDayCount++;//次数增加

	if ( byResult == E_MATCH_STATUS_LOSS )//输了给钱
	{
		nAddCoin = arenaRewardCfg->lose_money;
		nAddGold = arenaRewardCfg->lose_gold;
	}
	else
	{
		swapArenaRank(role, comRole, byResult);

		nAddCoin = arenaRewardCfg->win_money;
		nAddGold = arenaRewardCfg->win_gold;
		bChange = true;
	}
	if (pvAdd)
	{
		pvAdd[0] = nAddCoin;
		pvAdd[1] = nAddGold;
	} 
	ADDUP(role->dwCoin, nAddCoin );
	ADDUP(role->dwIngot, nAddGold );
	logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID, nAddCoin, nAddGold,
		purpose::arena_battle_report, 0);
	notifyIngot( role );
	notifyCoin( role );

	toClienCompetitors(role, bChange);

	RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_ARENA );

	return 0;
}

/*
@竞技场掉落
*/
//bool ARENA::getArenaDrop( ROLE* role, DWORD dropID, list<ITEM_CACHE*>* pLstEquipCachePkg, list<ITEM_CACHE*>* pLstPropsCachePkg, list<ITEM_CACHE*>* pLstItemCacheMine )
//{
//	vector<ITEM_INFO> tryV;
//	if ( !CONFIG::getRandItem( dropID, tryV ) )
//		return false;
//
//	for ( auto ii : tryV )
//	{
//
//		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( ii.itemIndex );
//		if ( itemCfg==NULL )
//			continue;
//
//		if ( IS_MINE(itemCfg ) )
//		{
//			//ItemCtrl::makeMineItem( role, itemCfg, ii.itemNums, 0, pLstItemCacheMine );
//			ItemCtrl::makeItem( role, role->m_packmgr.lstMinePkg, role->m_packmgr.curMineLimit, itemCfg, ii.itemNums, pLstItemCacheMine, false, (BYTE)itemCfg->mp_max);
//		}
//		else if ( IS_EQUIP(itemCfg) )
//		{
//			ItemCtrl::makeItem( role, role->m_packmgr.lstItemPkg, role->m_packmgr.curItemPkgLimit, itemCfg, ii.itemNums, pLstEquipCachePkg );	
//		}
//		else
//		{
//			ItemCtrl::makeItem( role, role->m_packmgr.lstEquipPkg, role->m_packmgr.curEquipPkgLimit, itemCfg, ii.itemNums, pLstPropsCachePkg );			
//		}
//
//	}
//	return true;
//}

/*
@竞技场发放奖励
*/
void ARENA::offerArenaReward()
{
	DWORD rank = 0;
	DWORD id = 0;
	DWORD moneyNum = 0;
	WORD itemID = 0;
	string itemName;
	WORD mineNum = 0;
	BYTE mineQua = 0;//纯度
	DWORD honour = 0;
	CONFIG::ARENA_BOUNS_CFG *arenaBounsCfg = NULL;
	ROLE *role = NULL;
	vector<ITEM_INFO> vecItemInfo;
	
	for ( auto it : mapRankRoleID )
	{
		vecItemInfo.clear();
		rank = it.first;
		id = it.second;
		arenaBounsCfg = CONFIG::getArenaBounsCfg(rank);
		if (arenaBounsCfg == NULL)
		{
			continue;
		}

		if (arenaBounsCfg->goldNum > 0)
		{
			moneyNum = arenaBounsCfg->goldNum;
			vecItemInfo.push_back( ITEM_INFO(arenaBounsCfg->goldID, moneyNum) );
		}

		if (arenaBounsCfg->itemNum > 0)
		{
			CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( arenaBounsCfg->itemID );
			if ( itemCfg != NULL )
			{
				itemName = itemCfg->name;
				itemID = arenaBounsCfg->itemID;
				mineQua = (BYTE)itemCfg->hp_max;
				mineNum = arenaBounsCfg->itemNum;
				vecItemInfo.push_back( ITEM_INFO(itemID, mineNum) );
			}	
		}

		if ( !vecItemInfo.empty() )
		{
			char szMsg[256];
			sprintf(szMsg,(CONFIG::getMsgJingjichang()).c_str(), rank, moneyNum, itemName.c_str(), mineNum);
			//logplan("areanmail::%s",szMsg);
			MailCtrl::sendMail(id, E_MAIL_TYPE_PRIZE, szMsg, vecItemInfo);
		}
	}
}
