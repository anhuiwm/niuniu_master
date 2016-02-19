#include "log.h"
#include "base.h"
#include "protocal.h"
#include "db.h"

#include "zlibapi.h"

#include "roleCtrl.h"
#include "itemCtrl.h"
#include "broadCast.h"
#include "factionCtrl.h"

#include <algorithm>
#include  <numeric> 

namespace FactionCtrl
{

	static map<DWORD, FACTION*> _mapFaction;
	static map<string, FACTION*> _mapNameFaction;//key: faction name 

	static vector<FACTION*> _vecFaction;

	static map<DWORD, FACTION*> _mapRoleIDFaction;
	
	static multimap<DWORD, FACTION*> _mapRoleReqJoin;

	static bool _needSortFaction = false;


	static size_t getFacRank( FACTION* faction );


}

FACTION::FACTION(const string& strFactionName)//, const string& strLeaderName, DWORD dwLeaderID)
{
	dwLeaderID = dwLeaderID2 = dwGuardianLeftID = dwGuardianRightID = 0;

	//memset( builderLevel, 1, sizeof(builderLevel) );//BYTE

	for ( int i=0; i<E_BUILDER_NUMS; i++)
	{
		builderLevel[i]=1;
	}

	memset( builderExp, 0, sizeof(builderExp) );//

	wMemberLimit = 
	wWelfare = 
	wTemple = 
	wHp = 
	wMp = 
	wAtk_max = 
	wAtk_min = 
	wMagic_max = 
	wMagic_min = 
	wSol_max = 
	wSol_min = 
	wDef_max = 
	wDef_min = 
	wMdef_max = 
	wMdef_min = 0;

	dwContribute = 0;

	dwExp = 0;

//	this->dwLeaderID = dwLeaderID;
	this->strName = strFactionName;
//	this->strLeaderName = strLeaderName;

	facRank = 1;

}


void FACTION::addChat( ROLE* role, const string& strContent )
{

	FACTION_CHAT fc;
	fc.tTime =  PROTOCAL::getCurTime();
	fc.facJob = FactionCtrl::getRoleFacJob( role );	
	fc.dwRoleID = role->dwRoleID;
	fc.strRoleName = role->roleName;
	fc.strContent = strContent;
	
	deqFactionChat.push_back( fc );

	if ( deqFactionChat.size() > 30 )
	{
		deqFactionChat.pop_front();
	}
	
}

#define BUILDER_EXP(e)	( this->builderExp[ (int)e -1] )
#define BUILDER_LEVEL(e)	( this->builderLevel[ (int)e -1] )


static string BUILDER_FIELD[] = { "main", "welfare", "temple", "hp", "mp", "atk", "mag", "sol", "def", "magdef" };

#define GET_BUILDER_FIELD(e)	( BUILDER_FIELD[ (int)e -1] )
#define GET_BUILDER_EXP_FIELD(e)	( BUILDER_FIELD[ (int)e -1]+"_exp" )


DWORD FACTION::getBuilderUpgradeExp(WORD wLevel, E_BUILDER_TYPE eType)
{

	CONFIG::GUILD_BUILD_CFG* builderCfg = getGuildBuildCfg2(eType, wLevel );
	if ( builderCfg == NULL )
		return 0;

	return builderCfg->upgrade_exp;
}

DWORD FACTION::getMemberLimit()
{

	CONFIG::GUILD_BUILD_CFG* builderCfg = getGuildBuildCfg2(E_BUILDER_TYPE_MAIN, builderLevel[ E_BUILDER_TYPE_MAIN -1 ] );
	if ( builderCfg == NULL )
		return 0;

	return builderCfg->union_para.one_value;
	
}



bool FACTION::addBuilderExp( E_BUILDER_TYPE e, DWORD dwAddExp )
{
//每项科技均拥有自己的等级，默认均为Lv.1，上限为Lv.30
//每项科技均拥有自己的进度，进度积累满之后，若满足升级的需要条件，则该科技的等级+1
//进度可以通过捐献金币或捐献装备来积累

	WORD maxLevel = ( e == E_BUILDER_TYPE_TEMPLE) ? BUILDER_LEVEL(E_BUILDER_TYPE_MAIN) :
					( e == E_BUILDER_TYPE_MAIN) ? 30 : BUILDER_LEVEL(E_BUILDER_TYPE_TEMPLE);


	WORD wOrigLevel = BUILDER_LEVEL(e);
	DWORD dwOrigExp = BUILDER_EXP(e);
	
	CONFIG::addExp( BUILDER_LEVEL(e), BUILDER_EXP(e), dwAddExp, maxLevel, 
					std::bind( &FACTION::getBuilderUpgradeExp, this, std::placeholders::_1, e  ) );

	if ( wOrigLevel != BUILDER_LEVEL(e) || dwOrigExp != BUILDER_EXP(e) )
	{
		EXEC_SQL( "update " TABLE_FACTION" set %s=%u, %s=%u where id=%u ", 
					GET_BUILDER_FIELD(e).c_str(), BUILDER_LEVEL(e), 
					GET_BUILDER_EXP_FIELD(e).c_str(), BUILDER_EXP(e), 
					this->dwFactionID );
 
	}

	return true;
}


void FACTION::initBuilderPro( )
{
	
	for ( BYTE i=E_BUILDER_TYPE_MAIN; i<= E_BUILDER_TYPE_MAG_DEF; i++) 
	{
	
		CONFIG::GUILD_BUILD_CFG*	pBuilderCfg = getGuildBuildCfg2( i, builderLevel[ i-1 ] );
		if ( pBuilderCfg == NULL )
			continue;
		
		CONFIG::GUILD_BUILD_CFG& builderCfg = *pBuilderCfg;

		if ( i == E_BUILDER_TYPE_MAIN )
		{
			wMemberLimit = builderCfg.union_para.one_value;
		}
		else if ( i == E_BUILDER_TYPE_WELFARE )
		{

		}
		else if ( i == E_BUILDER_TYPE_TEMPLE )
		{
		}
		else if ( i == E_BUILDER_TYPE_HP )
		{
			wHp = builderCfg.union_para.one_value;
		}
		else if ( i == E_BUILDER_TYPE_MP )
		{
			wMp = builderCfg.union_para.one_value;
		}
		else if ( i == E_BUILDER_TYPE_ATK )
		{
			wAtk_max = builderCfg.union_para.min_max.max_value;
			wAtk_min = builderCfg.union_para.min_max.min_value;

		}
		else if ( i == E_BUILDER_TYPE_MAGIC_ATK )
		{
			wMagic_max = builderCfg.union_para.min_max.max_value;
			wMagic_min = builderCfg.union_para.min_max.min_value;

		}
		else if ( i == E_BUILDER_TYPE_SOL_ATK )
		{
			wSol_max = builderCfg.union_para.min_max.max_value;
			wSol_min = builderCfg.union_para.min_max.min_value;

		}
		else if ( i == E_BUILDER_TYPE_DEF )
		{
			wDef_max = builderCfg.union_para.min_max.max_value;
			wDef_min = builderCfg.union_para.min_max.min_value;

		}
		else if ( i == E_BUILDER_TYPE_MAG_DEF )
		{
			wMdef_max = builderCfg.union_para.min_max.max_value;
			wMdef_min = builderCfg.union_para.min_max.min_value;
		}
	}

}


size_t FACTION::getContributeRank( DWORD roleid )
{

	auto it = mapFactionMember.find( roleid );
	if ( it == mapFactionMember.end() )
		return 0;
	
	if ( !needSortContribute )
	{
		return it->second.contributeRank;
	}


	vector< map<DWORD, FACTION_MEMBER>::iterator > v;
	
	for ( auto it = mapFactionMember.begin(); it!= mapFactionMember.end(); ++it )
	{
		v.push_back( it );
	}

	std::sort( v.begin(), v.end(), 
				[] ( map<DWORD, FACTION_MEMBER>::iterator a, map<DWORD, FACTION_MEMBER>::iterator b ) 
				{			
					return a->second.dwContribute > b->second.dwContribute;					
				} );


	for ( size_t i=0; i<v.size(); i++ )
	{
		v[i]->second.contributeRank = i+1;
	}

	needSortContribute = false;

	return it->second.contributeRank;

}

size_t FactionCtrl::getFacRank( FACTION* faction )
{

	if ( !_needSortFaction )
	{
		return faction->facRank;
	}

	std::sort( _vecFaction.begin(), _vecFaction.end(), 
				[] ( FACTION*  a, FACTION* b ) 
				{			
					if (GET_FACTION_LEVEL(a) != GET_FACTION_LEVEL(b) )
						return GET_FACTION_LEVEL(a) > GET_FACTION_LEVEL(b);

					return a->dwContribute > b->dwContribute;
				} );

	for ( size_t i=0; i<_vecFaction.size(); i++ )
	{
		_vecFaction[i]->facRank = i+1;
	}

	_needSortFaction = false;

	return faction->facRank;

}



void FactionCtrl::broadCast( FACTION* faction, const string& strPkt)
{

	for_each( faction->mapFactionMember.begin(), faction->mapFactionMember.end(), [&strPkt]( const std::pair<DWORD, FACTION_MEMBER> &x ) 
		{
		
			session* client = PROTOCAL::getOnlineClient(x.first);
			if ( client == NULL )
				return;

			PROTOCAL::sendClient(client, strPkt );

		}

	);

}

void FactionCtrl::factionDisband( FACTION* faction )
{
	assert( faction );

	EXEC_SQL( "delete from " TABLE_FACTION_MEMBER  " where factionid=%u ", faction->dwFactionID );
	EXEC_SQL( "delete from " TABLE_FACTION  " where id=%u ", faction->dwFactionID );
		
	_mapFaction.erase( faction->dwFactionID );
	_mapNameFaction.erase( faction->strName );
	
	for ( auto it = _vecFaction.begin(); it!=_vecFaction.end();  ++it )
	{
		if ( *it == faction )
		{
			_vecFaction.erase(it);
			break;
		}
	}
	
	string strData;
	S_APPEND_BYTE( strData, ROLE_PRO_FACTION_JOB );
	S_APPEND_DWORD( strData, E_NONE);
	string strPkt = PROTOCAL::cmdPacket(S_UPDATE_ROLE_DATA, strData);

	for ( auto it = _mapRoleReqJoin.begin(); it!=_mapRoleReqJoin.end();  )
	{
		if ( it->second == faction )
		{
			_mapRoleReqJoin.erase(it++);
			continue;
		}
		++it;
	}

	for_each( faction->mapFactionMember.begin(), faction->mapFactionMember.end(), [&strPkt]( const std::pair<DWORD, FACTION_MEMBER> &x ) 
		{
			_mapRoleIDFaction.erase( x.first );

			ROLE* role = RoleMgr::getMe().getCacheRoleByID(x.second.dwRoleID);
			if ( role )
			{
				role->faction = NULL;
			}

			session* client = PROTOCAL::getOnlineClient(x.first);
			if ( client == NULL )
				return;

			PROTOCAL::sendClient(client, strPkt );
		}

	);

	CC_SAFE_DELETE(faction);
	
}


FACTION* FactionCtrl::getFactionByRoleID( DWORD dwRoleID )
{

	auto it = _mapRoleIDFaction.find( dwRoleID );
	if ( it == _mapRoleIDFaction.end())
		return NULL;

	return it->second;

}

FACTION* FactionCtrl::getFaction( DWORD dwFactionID )
{

	auto it = _mapFaction.find( dwFactionID );
	if ( it == _mapFaction.end())
		return NULL;

	return it->second;
}

#define ADD_FACTION(fac)	\
	do {\
		faction->facRank = _mapFaction.size()+1;\
		_mapNameFaction.insert( make_pair( fac->strName, fac ) );\
		_mapFaction.insert( make_pair( fac->dwFactionID, fac ) );\
		_vecFaction.push_back( fac );\
	}while(0)

		
FACTION* FactionCtrl::createFaction( ROLE* role, const string& factionName)
{
	assert( role->faction == NULL );

	MYSQL_ESCAPE_N(szFacName, factionName.data(), min<size_t>(factionName.size(), ROLE_NAME_LEN), ROLE_NAME_LEN );
//	MYSQL_ESCAPE_N(szRoleName, role->roleName.data(), min<size_t>(role->roleName.size(), ROLE_NAME_LEN), ROLE_NAME_LEN );

	if ( !EXEC_SQL( "insert into " TABLE_FACTION "(factionname) values('%s')", szFacName ) )
		return NULL;
	
	FACTION *faction = new FACTION( factionName);//, role->roleName, role->dwRoleID );
	faction->dwFactionID = (DWORD)mysql_insert_id( DBCtrl::getSQL() );
	faction->dwLeaderID = role->dwRoleID;
	faction->strLeaderName = role->roleName;

	faction->initBuilderPro( );

	if ( !EXEC_SQL( "insert into " TABLE_FACTION_MEMBER "(roleid, factionid, facjob) values(%u,  %u, %u)", 
							role->dwRoleID, faction->dwFactionID, E_LEADER) )
	{
		CC_SAFE_DELETE(faction);
		return NULL;
	}

	//EXEC_SQL( "update " TABLE_ROLE_INFO " set factionid=%u where roleid=%u", faction->dwFactionID, role->dwRoleID );


	FACTION_MEMBER& fm = faction->mapFactionMember[ role->dwRoleID ];
	fm.dwRoleID = role->dwRoleID;
	fm.strRoleName = role->roleName;
	fm.facJob = E_LEADER;

	ADD_FACTION(faction);

	role->faction = faction;
	//role->facJob = E_LEADER;

	return faction;
}

E_FACTION_JOB FactionCtrl::getRoleFacJob( ROLE* role )
{
	if ( role->faction == NULL )
		return E_NONE;
	
	auto it = role->faction->mapFactionMember.find( role->dwRoleID );
	return ( it != role->faction->mapFactionMember.end() ) ? it->second.facJob : E_NONE;
	
}

FACTION_MEMBER* FactionCtrl::getFacMember( ROLE* role )
{
	if ( role->faction == NULL )
		return NULL;
	
	auto it = role->faction->mapFactionMember.find( role->dwRoleID );
	return ( it != role->faction->mapFactionMember.end() ) ? &it->second: NULL;
	
}

FACTION_MEMBER* FactionCtrl::getFacMemberByRoleID( FACTION* faction, DWORD dwRoleID )
{	
	auto it = faction->mapFactionMember.find( dwRoleID );
	return ( it != faction->mapFactionMember.end() ) ? &it->second: NULL;
	
}



#define SET_LEADER(faction, id,name)		\
	do {\
		faction->dwLeaderID = id;\
		faction->strLeaderName = name;\
	}while(0)


#define SET_LEADER_2(faction, id,name)		\
	do {\
		faction->dwLeaderID2 = id;\
		faction->strLeaderName2 = name;\
	}while(0)
	
#define SET_GUARDIAN_LEFT(faction, id,name)		\
	do {\
		faction->dwGuardianLeftID = id;\
		faction->strGuardianLeftName = name;\
	}while(0)
	
#define SET_GUARDIAN_RIGHT(faction, id,name)		\
	do {\
		faction->dwGuardianRightID = id;\
		faction->strGuardianRightName = name;\
	}while(0)


void FactionCtrl::loadFaction( )
{

	SQLSelect sqlx( DBCtrl::getSQL(), "select id, factionname"//, leadername, leaderid, leader2name, leader2id"
								//",guardian_left_name, guardian_left_id, guardian_right_name, guardian_right_id, "
								",contribute"
								",main, main_exp, welfare, welfare_exp, temple,temple_exp"
								",hp, hp_exp, mp, mp_exp, atk, atk_exp, mag, mag_exp, sol, sol_exp, def, def_exp, magdef,magdef_exp"
								" from " TABLE_FACTION );

	while( MYSQL_ROW row = sqlx.getRow() )
	{
		FACTION *faction = new FACTION(row[1]);//, row[2], ATOUL(row[3]) );

		faction->dwFactionID = ATOUL(row[0]);
		
		faction->dwContribute = ATOUL(row[2]);

		const int COLS= 3;
		for ( int i=0; i< E_BUILDER_NUMS; i++) 
		{
			faction->builderLevel[ i ] = ATOB(row[COLS+i*2]);
			faction->builderExp[ i ] = ATOUL(row[COLS+i*2+1]);
		}

		faction->initBuilderPro( );

		ADD_FACTION(faction);

			
	}


	{

		SQLSelect sqlx( DBCtrl::getSQL(), " select roleid, rolename, job, level, factionid, facjob, contribute from " TABLE_FACTION_MEMBER
									" left join roleinfo on factionmember.roleid = roleinfo.id " );

		while( MYSQL_ROW row = sqlx.getRow() )
		{
			DWORD dwFactionID = ATOUL(row[4]);
			 
			auto it = _mapFaction.find(dwFactionID);
			if ( it == _mapFaction.end() )
			{
				continue;
			}

			FACTION *faction =it->second;
			
			FACTION_MEMBER fm;

			fm.dwRoleID = ATOUL(row[0]);
			fm.strRoleName = row[1];
			fm.byJob = ATOB(row[2]);
			fm.wLevel = ATOW(row[3]);

			fm.facJob = (E_FACTION_JOB)ATOUL(row[5]);

			fm.dwContribute = ATOUL(row[6]);

			if ( fm.facJob == E_LEADER )
			{
				SET_LEADER( faction, fm.dwRoleID, fm.strRoleName );
			}
			else if ( fm.facJob == E_LEADER2 )
			{
				SET_LEADER_2( faction, fm.dwRoleID, fm.strRoleName );
			}
			else if ( fm.facJob == E_GUADIAN_LEFT )
			{
				SET_GUARDIAN_LEFT( faction, fm.dwRoleID, fm.strRoleName );

			}
			else if ( fm.facJob == E_GUADIAN_RIGHT )
			{
				SET_GUARDIAN_RIGHT( faction, fm.dwRoleID, fm.strRoleName );
			}

			faction->mapFactionMember.insert( make_pair( fm.dwRoleID, fm) );

			_mapRoleIDFaction[ fm.dwRoleID ] = faction;
			
		}


	}

	{

		SQLSelect sqlx( DBCtrl::getSQL(), " select roleid, factionid, rolename, job, sex, level from " TABLE_FACTION_APPLY 
									" left join roleinfo on factionapply.roleid = roleinfo.id " );

		while( MYSQL_ROW row = sqlx.getRow() )
		{
			DWORD dwRoleID = ATOUL(row[0]);
			DWORD dwFactionID = ATOUL(row[1]);
			 
			auto it = _mapFaction.find(dwFactionID);
			if ( it == _mapFaction.end() )
			{
				continue;
			}
			
			_mapRoleReqJoin.insert( make_pair( dwRoleID, it->second ) );
			
			ROLE_SIMPLE_INFO rsi( dwRoleID, row[2], ATOW(row[5]), ATOB(row[3]), ATOB(row[4]) );	
			it->second->lstReqJoin.push_back( rsi );

		}

	}
	
	//每个帮派内部,成员帮贡排名
	for( auto it : _mapFaction )
	{
		it.second->needSortContribute = false;
		it.second->getContributeRank( it.second->dwLeaderID );
	}

	//帮派排名
	auto it = _mapFaction.begin();
	if ( it != _mapFaction.end() )
	{
		_needSortFaction = true;
		FactionCtrl::getFacRank( it->second );
	}


}



string& mkSimpleFacInfo(string & strData, ROLE* role, FACTION* faction)
{
	char name[ROLE_NAME_LEN]={0};
	strcpy( name, faction->strName.c_str() );

	
	S_APPEND_DWORD( strData, faction->dwFactionID );

	S_APPEND_NBYTES( strData, name, ROLE_NAME_LEN );
	S_APPEND_BYTE( strData, GET_FACTION_LEVEL(faction) );
	S_APPEND_WORD( strData, faction->mapFactionMember.size() );
	S_APPEND_WORD( strData, faction->getMemberLimit( ) );

	auto it = find_if( faction->lstReqJoin.begin(), faction->lstReqJoin.end(), [role](const ROLE_SIMPLE_INFO& rsi ){ return rsi.dwRoleID == role->dwRoleID; } );
	S_APPEND_BYTE( strData, !!( it!=faction->lstReqJoin.end()) );

	//S_APPEND_BYTE( strData, !!(FactionCtrl::_mapRoleReqJoin.find(role->dwRoleID) != FactionCtrl::_mapRoleReqJoin.end() ) );

//		  [ 行会id,名字, 等级, 当前人数, 人数上限, 是否已申请(0/1) ]n个

	return strData;
}


int FactionCtrl::clientGetGuildRecommendList( ROLE* role, unsigned char * data, size_t dataLen )
{

//获取当前推荐的公会信息列表
//		C:[] #只有协议头
//
//		S:[dword, char[22],BYTE,WORD,WORD BYTE,]n个
//		  [ 行会id,名字, 等级, 当前人数, 人数上限, 是否已申请(0/1) ]n个
//		注：不定长度，最长15个工会信息

logff("%u %s %u", role->dwRoleID, role->roleName.c_str(), _vecFaction.size() );

	const size_t MAX_RECOMM_NUMS = 15;

	random_shuffle( _vecFaction.begin(), _vecFaction.end() );

	size_t nums = min( _vecFaction.size(), MAX_RECOMM_NUMS );

	string strData;

	for ( size_t i=0; i<nums; i++)
	{
		FACTION* faction = 	_vecFaction[i];

		mkSimpleFacInfo( strData, role, faction );

	}

	//没有数据也返回
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_LIST, strData) );
	return 0;
}

#define S_APPEND_NAME(strData, nameStr)		\
	do {\
		char name[ROLE_NAME_LEN]={0};\
		strcpy( name, nameStr.c_str() );\
		S_APPEND_NBYTES( strData, name, ROLE_NAME_LEN );\
	}while(0)


#define S_APPEND_FACTION_NAME(strData, faction)				S_APPEND_NAME( strData, faction->strName)
#define S_APPEND_FACTION_LEADER_NAME(strData, faction)		S_APPEND_NAME( strData, faction->strLeaderName)




int FactionCtrl::clientGetGuildRankList( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[] #只有协议头
//		
//		S:[DWord,BYTE,char[22],DWORD,BYTE,char[22]]n个
//		  [行会id, 排行,名字, 帮贡, 等级, 会长名字 ]n个
//		注：不定长度，最长20个工会信息
	const size_t MAX_RANK_NUMS = 20;
	size_t nums = min( _vecFaction.size(), MAX_RANK_NUMS );
	
//	std::sort( _vecFaction.begin(), _vecFaction.end(), 
//				[] ( FACTION*  a, FACTION* b ) 
//				{
//			
//					if (GET_FACTION_LEVEL(a) != GET_FACTION_LEVEL(b) )
//						return GET_FACTION_LEVEL(a) > GET_FACTION_LEVEL(b);

//					return a->dwContribute > b->dwContribute;
//					
//				} );

	string strData;
	for ( size_t i=0; i<nums; i++)
	{
		FACTION* faction = 	_vecFaction[i];

		char name[ROLE_NAME_LEN]={0};
		strcpy( name, faction->strName.c_str() );

		char leaderName[ROLE_NAME_LEN]={0};
		strcpy( leaderName, faction->strLeaderName.c_str() );

		S_APPEND_DWORD( strData, faction->dwFactionID );

		S_APPEND_BYTE( strData, i+1 );
		S_APPEND_NBYTES( strData, name, ROLE_NAME_LEN );
		S_APPEND_DWORD( strData, faction->dwContribute );
		S_APPEND_BYTE( strData, GET_FACTION_LEVEL(faction) );	
		S_APPEND_NBYTES( strData, leaderName, ROLE_NAME_LEN );

	}

	//没有数据也返回
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_GUILD_RANK_LIST, strData) );
	return 0;

}

//点击搜索工会 点击搜索,显示的已请求申请的工会列表
int FactionCtrl::clientGetGuildReq( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[] #只有协议头
//		
//		S:[[DWord,char[22],BYTE,WORD,WORD BYTE,]n个
//		  [[行会id, 名字, 等级, 当前人数, 人数上限, 是否已申请(0/1) ]]3个
//		注：不定长度，最长3个工会信息

	string strData;

	auto beg = _mapRoleReqJoin.lower_bound( role->dwRoleID );
	auto end = _mapRoleReqJoin.upper_bound( role->dwRoleID );

	while( beg != end )
	{
		FACTION* faction = 	beg->second;
		mkSimpleFacInfo( strData,  role, faction);
		++beg;
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_GUILD_req, strData) );
	return 0;

}


//点击ok输入搜索的公会请求
int FactionCtrl::clientGuildFind( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[char[22]] #[行会id 或者行会名字]
//		
//		S:[DWord,char[22],BYTE,WORD,WORD BYTE,]n个
//		  [行会id, 名字, 等级, 当前人数, 人数上限, 是否已申请(0/1) ]n个
//		注：不定长度，最长15个工会信息


	string strNameOrID;


	if ( !BASE::parseNBYTE( data, dataLen, strNameOrID, ROLE_NAME_LEN-1 ) )
		return -1;

	FACTION* faction1 = NULL;
	FACTION* faction2 = NULL;

	auto it = _mapNameFaction.find( strNameOrID );
	if ( it != _mapNameFaction.end() )
		faction1 = it->second;

	DWORD dwFactionID = ATOUL_( strNameOrID );
	auto jt = _mapFaction.find(dwFactionID);
	if ( jt != _mapFaction.end() )
	{
		faction2 = jt->second;
	}

	string strData;
	if ( faction1 )
		mkSimpleFacInfo( strData, role, faction1 );
	
	if ( faction2 )
		mkSimpleFacInfo( strData, role, faction2 );

	//没有数据也返回
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_GUILD_find, strData) );
	return 0;

}

bool removeFromReqJoin(DWORD dwRoleID, FACTION* faction)
{

	auto beg = FactionCtrl::_mapRoleReqJoin.find( dwRoleID );
	
	while( beg != FactionCtrl::_mapRoleReqJoin.end() )
	{
		if ( beg->second == faction )
		{
			FactionCtrl::_mapRoleReqJoin.erase( beg++ );
			return true;
		}
		else
		{
			++beg;
		}
	}

	return false;

}

void FactionCtrl::saveApplyList()
{

	//static char szSql[2048]= "insert into  " TABLE_FACTION_APPLY " (roleid, factionid, rolename, job, sex, level) values ";
	static char szSql[1024]= "insert into  " TABLE_FACTION_APPLY " (roleid, factionid) values ";
	static char* head = szSql+strlen(szSql);

	EXEC_SQL( "TRUNCATE TABLE " TABLE_FACTION_APPLY);
	
	char* p = head;
//	p += sprintf( p, "insert into  " TABLE_FACTION_APPLY " (roleid, factionid, rolename, job, sex, level) values " );

	for ( auto &it : _mapFaction )
	{

		for ( auto &jt : it.second->lstReqJoin )
		{
			//MYSQL_ESCAPE_CPP( szName, jt.strRolename );

			//p += sprintf( p, "(%u,%u,'%s',%u,%u,%u),", jt.dwRoleID, it.second->dwFactionID, szName, jt.byJob, jt.bySex, jt.wLevel );
			p += sprintf( p, "(%u,%u),", jt.dwRoleID, it.second->dwFactionID  );

			if ( p - head >900  )
			{
				*(--p) = 0;
				EXEC_SQL( "%s", szSql );
				p = head;
			}
		}
	}

	if ( p != head )
	{
		*(--p) = 0;
		EXEC_SQL( "%s", szSql );
	}

}


//加入行会申请  对于已申请的就是撤销申请
int FactionCtrl::clientGuildReqJoin( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[DWord] #[行会id]
//		S:[byte(0/1),DWord(行会id)]

	string strData;

	DWORD dwFactionID;

	if ( !BASE::parseDWORD( data, dataLen, dwFactionID) )
		return -1;
	
	BYTE errCode = 0;

	do
	{
		FACTION* factionDst = getFaction( dwFactionID );
		COND_BREAK( factionDst == NULL , errCode, 1 );

		auto it = find_if( factionDst->lstReqJoin.begin(), factionDst->lstReqJoin.end(), [role](const ROLE_SIMPLE_INFO& rsi)
			{
				return ( rsi.dwRoleID == role->dwRoleID );
			}
		);

		//已经申请
		if ( it != factionDst->lstReqJoin.end() )
		{
			factionDst->lstReqJoin.erase(it);
			removeFromReqJoin( role->dwRoleID, factionDst );
			break;
		}

		ROLE_SIMPLE_INFO rsi(role);
		_mapRoleReqJoin.insert( make_pair( role->dwRoleID, factionDst ) );
		factionDst->lstReqJoin.push_back( rsi );


	} while(0);

	S_APPEND_BYTE( strData, errCode );
	S_APPEND_DWORD( strData, errCode == 0 ? dwFactionID : 0 );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_add, strData) );
	return 0;

}

//创建行会
int FactionCtrl::clientGuildCreate( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[char[22]] #[行会名字]
//		
//		S:[byte]
//		  [错误码(0成功,1是已存在,2是不合法)]

	string  strFactionName;
	if ( !BASE::parseBSTR( data, dataLen, strFactionName) )
		return -1;

	string strData;

	BYTE errCode = 0;

	do
	{	
		COND_BREAK( role->faction != NULL , errCode, 4 );

		COND_BREAK( strFactionName.empty(), errCode, 2 );

		COND_BREAK( strFactionName.size()>ROLE_NAME_LEN-1, errCode, 3 );

		COND_BREAK( _mapNameFaction.find( strFactionName ) != _mapNameFaction.end(), errCode, 1 );

		COND_BREAK( role->dwIngot<500, errCode, 6 );

		FACTION* faction = createFaction( role, strFactionName );
		
		COND_BREAK( faction==NULL, errCode, 5 );

		//ADDUP(role->dwIngot, -(int)500 );

	}while(0);

	S_APPEND_BYTE( strData, errCode );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_create, strData) );

	if ( errCode == 0 )
		BroadcastCtrl::faction( E_BC_CREATE, _mapFaction.size(), role->roleName, role->faction->strName );



	return 0;

}


//公会详情
int FactionCtrl::clientGuildGetInfo( ROLE* role, unsigned char * data, size_t dataLen )
{

	//只看自己的
	string strData;

	FACTION* faction = role->faction;

	if ( faction == NULL )
	{
		//没有数据也返回
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO, strData) );
		return 0;
	}
	
	char name[ROLE_NAME_LEN]={0};
	strcpy( name, faction->strName.c_str() );
//			
	char leaderName[ROLE_NAME_LEN]={0};
	strcpy( leaderName, faction->strLeaderName.c_str() );

//		S:[char[22](行会名字),BYTE(等级),dword(行会id),
//		dword(当前经验),char[22](行会会长),
//		word(排行),dword(贡献),WORD(当前人数),WORD(人数上限), BYTE(职位)
//		byte(贡献排名),char[30](公告)]

	S_APPEND_NBYTES( strData, name, ROLE_NAME_LEN );
	S_APPEND_BYTE( strData, GET_FACTION_LEVEL(faction) );
	S_APPEND_DWORD( strData, faction->dwFactionID );

	S_APPEND_DWORD( strData, faction->dwExp );
	S_APPEND_NBYTES( strData, leaderName, ROLE_NAME_LEN );

	S_APPEND_WORD( strData, getFacRank(faction) );//排行
	
	S_APPEND_DWORD( strData, faction->dwContribute );		
	S_APPEND_WORD( strData, faction->mapFactionMember.size() );
	S_APPEND_WORD( strData, faction->wMemberLimit );
	S_APPEND_BYTE( strData, getRoleFacJob(role) );

	S_APPEND_BYTE( strData, faction->getContributeRank( role->dwRoleID ) );		//(贡献排名),char[30](公告)]
	S_APPEND_BSTR( strData, faction->strNoti );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO, strData) );
	return 0;

}



//公告编辑
int FactionCtrl::clientGuildCreateContent( ROLE* role, unsigned char * data, size_t dataLen )
{

//		C:[char[30]] #[公告内容]
//		S:[byte(0/1)]
	
	
	string  strNoti;
	if ( !BASE::parseBSTR( data, dataLen, strNoti) )
		return -1;
	
	if ( strNoti.size() >= 256 )
		return 0;
	
	string strData;

	BYTE errCode = 0;
	do
	{
		COND_BREAK( role->faction == NULL , errCode, 1 );
		
		COND_BREAK( getRoleFacJob(role) == E_MEMBER , errCode, 1 );

		role->faction->strNoti.assign( strNoti );

		MYSQL_ESCAPE_CPP(szNotify, role->faction->strNoti);

		EXEC_SQL( "update " TABLE_FACTION_MEMBER  " set board='%s' where id=%u ", szNotify, role->faction->dwFactionID );
	
	}while(0);


	S_APPEND_BYTE( strData, errCode );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_create_content, strData) );
	return 0;

}



int FactionCtrl::clientGuildGetMemList( ROLE* role, unsigned char * data, size_t dataLen )
{
//		c:[]
//		s:[[
//		DWORD(成员id),BYTE(职位),char[22](成员名字),BYTE(职业),
//		BYTE(等级),dword(贡献),
//		]]

	string strData;

	if ( role->faction )
	{

		for ( auto it : role->faction->mapFactionMember )
		{
			S_APPEND_DWORD( strData, it.first );
			S_APPEND_BYTE( strData, it.second.facJob );
			S_APPEND_NAME( strData, it.second.strRoleName );

			S_APPEND_BYTE( strData, it.second.byJob );
			S_APPEND_BYTE( strData, it.second.wLevel );
			S_APPEND_DWORD( strData, it.second.dwContribute );
		}
	}
	
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_MEMLIST, strData) );
	return 0;

}

int FactionCtrl::clientGuildGetMemEquip( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[word(成员id)]
//		s:[
//		(这个应该是杨威的统一返回接口)
//		]

//	string strData;

//	BYTE errCode = 0;

//	S_APPEND_BYTE( strData, errCode );
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_create_content, strData) );

	return 0;

}

int FactionCtrl::clientGuildAddFriend( ROLE* role, unsigned char * data, size_t dataLen )
{

//	string strData;

//	BYTE errCode = 0;
//	
//	S_APPEND_BYTE( strData, errCode );
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_create_content, strData) );
//	
	return 0;
}

#define UPDATE_DB_FACJOB(roleid, facjob)		EXEC_SQL( "update " TABLE_FACTION_MEMBER  " set facjob=%u where roleid=%u ", facjob, roleid )

int FactionCtrl::clientGuildAppointJob( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[word(成员id),BYTE(职位id)]
//		s:[
//		byte(error返回值 0是成功)
//		]


//职位任命
//会长可以对除自己外的所有人进行职位任命，
//在弹出的职位任命窗口中，从4种职位中选择1种职位
//副会长、左护法、右护法 均只能有一个，将该职位任命给其他人后，之前的那个将自动变成成员职位

	DWORD dwRoleID;
	BYTE byFacJob;

	if ( !BASE::parseDWORD( data, dataLen, dwRoleID) )
		return -1;

	if ( !BASE::parseBYTE( data, dataLen, byFacJob) )
		return -1;

	E_FACTION_JOB dstJob = (E_FACTION_JOB)byFacJob;

	string strData;
	BYTE errCode = 0;

	do
	{

		//E_FACTION_JOB myJob = FactionCtrl::getRoleFacJob( role );


		//职位不合法
//		COND_BREAK( dstJob != E_LEADER2 && dstJob != E_GUADIAN_LEFT && dstJob != E_GUADIAN_RIGHT, errCode, 1 );
		COND_BREAK( dstJob == E_LEADER, errCode, 1 );
		
		//不是帮主
		COND_BREAK( !IS_LEADER(role), errCode, 2 );

		//委派自己
		COND_BREAK( role->dwRoleID == dwRoleID, errCode, 3 );

		//不是同一帮派
		FACTION_MEMBER* pFmDst = getFacMemberByRoleID( role->faction, dwRoleID );
		COND_BREAK( pFmDst == NULL, errCode, 4 );

		//同一职位
		COND_BREAK( pFmDst->facJob == dstJob, errCode, 5 );


		//需要把原职务的卸任
		if ( dstJob != E_MEMBER )
		{
			FACTION_MEMBER* pFmOrig = NULL;
			for ( auto & it : role->faction->mapFactionMember )
			{
				if ( it.second.facJob == dstJob )
				{
					pFmOrig = &it.second;
					break;
				}
			}

			if ( pFmOrig )
			{
				pFmOrig->facJob = E_MEMBER;			
				notifyTypeValuebyRoleID(pFmOrig->dwRoleID, ROLE_PRO_FACTION_JOB, E_MEMBER );
				UPDATE_DB_FACJOB(pFmOrig->dwRoleID, E_MEMBER);
			}

		}

		if ( pFmDst->facJob == E_LEADER2 )
		{
			SET_LEADER_2( role->faction, 0, "" );
		}
		else if ( pFmDst->facJob == E_GUADIAN_LEFT )
		{
			SET_GUARDIAN_LEFT( role->faction, 0, "" );
		}
		else if ( pFmDst->facJob == E_GUADIAN_RIGHT )
		{
			SET_GUARDIAN_RIGHT( role->faction, 0, "" );
		}


//		COND_BREAK( dstJob != E_LEADER2 && dstJob != E_GUADIAN_LEFT && dstJob != E_GUADIAN_RIGHT, errCode, 1 );

		pFmDst->facJob = dstJob;
		notifyTypeValuebyRoleID(dwRoleID, ROLE_PRO_FACTION_JOB, dstJob );
		UPDATE_DB_FACJOB(dwRoleID, dstJob);

	}while(0);
	
	
	S_APPEND_BYTE( strData, errCode );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_MEM_JOB, strData) );
	
	return 0;
}



#define DEL_ROLE_FROM_FACTION(faction, dwRoleID, strRoleName)	\
	do {\
		_mapRoleIDFaction.erase(dwRoleID);\
		faction->mapFactionMember.erase( dwRoleID );\
		EXEC_SQL( "delete from " TABLE_FACTION_MEMBER  " where roleid=%u ", dwRoleID );\
	}while(0)
		
//逐出行会
int FactionCtrl::clientGuildKickMember( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[word(成员id)]
//		s:[
//		byte(error返回值 0是成功)
//		]

//逐出行会
//行会中，拥有5种职位身份，分别为：会长、副会长、左护法、右护法、成员
//会长和副会长可以将行会中的普通成员逐出行会，当逐出的对象职位不为"成员"时
//提示：抱歉，只有职位为成员的可以被逐出

	DWORD dwRoleID;

	if ( !BASE::parseDWORD( data, dataLen, dwRoleID) )
		return -1;

	string strData;

	BYTE errCode = 0;
	
	do
	{

		//驱逐自己
		COND_BREAK( role->dwRoleID == dwRoleID, errCode, 1 );

		//不是帮主,副帮主
		COND_BREAK( !IS_LEADER(role) && !IS_VICE_LEADER(role), errCode, 2 );

		//不是同一帮派
		FACTION_MEMBER* pFmDst = getFacMemberByRoleID( role->faction, dwRoleID );
		COND_BREAK( pFmDst == NULL, errCode, 3 );

		//当逐出的对象职位不为"成员"时
		COND_BREAK( pFmDst->facJob != E_MEMBER, errCode, 4 );

		notifyTypeValuebyRoleID(dwRoleID, ROLE_PRO_FACTION_JOB, E_NONE );
	
		DEL_ROLE_FROM_FACTION(role->faction, pFmDst->dwRoleID, pFmDst->strRoleName);

	}while(0);


	
	S_APPEND_BYTE( strData, errCode );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_MEM_QUIT, strData) );
	
	return 0;
}

//退出行会
int FactionCtrl::clientGuildQuit( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[]
//		s:[
//		byte(error返回值 0是成功)
//		]

//退出行会
//点击退出行会按钮，将弹出对话框
//会长：离开行会后，会长之位将传给副会长，或者解散，是否继续？
//其他成员：是否确定要离开行会：行会名称？

//会长退出行会后，副会长直接变成会长，若无副会长，则左护法 > 右护法，变成会长，若副会长、左护法、右护法均没有，则直接解散行会
//行会解散后，行会中所有会员都将变成未入行会状态

	E_FACTION_JOB facJob = FactionCtrl::getRoleFacJob( role );
	if( facJob == E_NONE )
		return 0;

	if ( facJob == E_LEADER )
	{
		FACTION_MEMBER* pLeader2 = NULL;
		FACTION_MEMBER* pGuadianLeft = NULL;
		FACTION_MEMBER* pGuadianRight= NULL;

		for ( auto & it : role->faction->mapFactionMember )
		{
			if ( it.second.facJob == E_LEADER2 )
			{
				pLeader2 = &it.second;
				break;
			}
			else if ( it.second.facJob == E_GUADIAN_LEFT )
			{
				pGuadianLeft = &it.second;
			}
			else if ( it.second.facJob == E_GUADIAN_RIGHT )
			{
				pGuadianRight = &it.second;
			}				
		}

		FACTION_MEMBER* pNewLeader = pLeader2 ? pLeader2 :
										pGuadianLeft ? pGuadianLeft :
										pGuadianRight ? pGuadianRight : NULL;

		//帮主退出，没有继任者
		if ( pNewLeader == NULL )
		{
			factionDisband( role->faction );
		}
		else
		{
			//帮主退出，有继任者
			
			pNewLeader->facJob = E_LEADER;
			UPDATE_DB_FACJOB(pNewLeader->dwRoleID, E_LEADER);
			notifyTypeValuebyRoleID(pNewLeader->dwRoleID, ROLE_PRO_FACTION_JOB, E_LEADER );

			DEL_ROLE_FROM_FACTION(role->faction, role->dwRoleID, role->roleName);
			notifyTypeValue( role, ROLE_PRO_FACTION_JOB, E_NONE );

SET_LEADER( role->faction, pNewLeader->dwRoleID, pNewLeader->strRoleName );

if ( role->faction->dwLeaderID2 == pNewLeader->dwRoleID )
{
	SET_LEADER_2(role->faction, 0, "");
}
else if ( role->faction->dwGuardianLeftID == pNewLeader->dwRoleID )
{
	SET_GUARDIAN_LEFT(role->faction, 0, "");
}
else if ( role->faction->dwGuardianRightID == pNewLeader->dwRoleID )
{
	SET_GUARDIAN_RIGHT(role->faction, 0, "");
}



	
		}
	
	}
	else
	{
		//非帮主退出
		FACTION_MEMBER* pFM = FactionCtrl::getFacMember( role );
		assert( pFM );

		DEL_ROLE_FROM_FACTION(role->faction, pFM->dwRoleID, pFM->strRoleName);
		notifyTypeValue( role, ROLE_PRO_FACTION_JOB, E_NONE );

		role->faction=NULL;
	}


	string strData;
	S_APPEND_BYTE( strData, 0 );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_QUIT, strData) );

	return 0;
}


//行会审核列表
int FactionCtrl::clientGuildGetReqList( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[]
//		s:[
//		WORD(成员id),char[22](成员名字),BYTE(等级),BYTE(职业),
//		]

	E_FACTION_JOB facJob = FactionCtrl::getRoleFacJob( role );
	if ( facJob == E_NONE || facJob == E_MEMBER )
		return 0;
	
	string strData;

	for ( auto it : role->faction->lstReqJoin )
	{
		S_APPEND_DWORD( strData, it.dwRoleID );
		S_APPEND_NAME( strData, it.strRolename );
		S_APPEND_BYTE( strData, it.wLevel );
		S_APPEND_BYTE( strData, it.byJob );
	}
	
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_REQ_LIST, strData) );	
	return 0;
}

void addFacMember(FACTION* faction, DWORD dwDstRoleID)
{

	if ( EXEC_SQL( "insert into " TABLE_FACTION_MEMBER "(roleid, factionid, facjob) values(%u,  %u, %u)", 
					dwDstRoleID, faction->dwFactionID, E_MEMBER ) )
	{	
		notifyTypeValuebyRoleID(dwDstRoleID, ROLE_PRO_FACTION_JOB, E_MEMBER );

		ROLE* roleTgt = RoleMgr::getMe().getRoleByID( dwDstRoleID );
		if ( roleTgt )
		{
			roleTgt->faction = faction;
			FACTION_MEMBER& fm = faction->mapFactionMember[ dwDstRoleID ];
			fm.dwRoleID = roleTgt->dwRoleID;
			fm.strRoleName = roleTgt->roleName;
			fm.facJob = E_MEMBER;			
		}
	}

}

int acceptDenyFromReqList( FACTION* faction, DWORD dwDstRoleID, BYTE byAccept )
{

	auto it = find_if( faction->lstReqJoin.begin(), faction->lstReqJoin.end(), [dwDstRoleID](const ROLE_SIMPLE_INFO& rsi)
		{
			return ( rsi.dwRoleID == dwDstRoleID );
		}
	);
	if ( it != faction->lstReqJoin.end() )
	{
		removeFromReqJoin( (*it).dwRoleID, faction );
		faction->lstReqJoin.erase( it );

		if ( byAccept )
		{
			addFacMember( faction, dwDstRoleID );
		}
		
	}
	else
	{
		return 2;//不在列表中
	}

	return 0;
}



//同意或者拒绝id加入行会
int FactionCtrl::clientGuildReqListOp( ROLE* role, unsigned char * data, size_t dataLen )
{

//		C:[word(成员id),byte(0/1,0同意)]
//		s:[
//		byte(error返回值 0是成功)
//		]

//1: 操作者没有帮派或者不是领导职务
//2: 不在列表中
//3: 人数已满

	string strData;

	DWORD dwDstRoleID;
	BYTE byAccept;
	
	if ( !BASE::parseDWORD( data, dataLen, dwDstRoleID) )
		return -1;

	if ( !BASE::parseBYTE( data, dataLen, byAccept) )
		return -1;

	BYTE errCode = 0;

	E_FACTION_JOB facJob = FactionCtrl::getRoleFacJob( role );
	if ( facJob == E_NONE || facJob == E_MEMBER )
	{
		errCode = 1;
	}
	else if ( role->faction->mapFactionMember.size() >= role->faction->getMemberLimit() )
	{
		errCode = 3;
	}	
	else
	{
		acceptDenyFromReqList( role->faction, dwDstRoleID, byAccept );	
	}

	S_APPEND_BYTE( strData, errCode );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_REQ_YES, strData) );
	
	return 0;
}


//全部同意加入
int FactionCtrl::clientGuildReqAcceptALL( ROLE* role, unsigned char * data, size_t dataLen )
{

	BYTE errCode = 0;

	E_FACTION_JOB facJob = FactionCtrl::getRoleFacJob( role );
	if ( facJob == E_NONE || facJob == E_MEMBER )
	{
		errCode = 1;
	}

	auto memberLimit = role->faction->getMemberLimit();

	bool patialAdd = false;
	bool full = false;
	
	for( auto it = role->faction->lstReqJoin.begin(); it!=role->faction->lstReqJoin.end(); )
	{
		if ( role->faction->mapFactionMember.size() >= memberLimit )
		{
			full = true;
			break;
		}

		addFacMember( role->faction, (*it).dwRoleID );

		removeFromReqJoin( (*it).dwRoleID, role->faction );
		role->faction->lstReqJoin.erase(it++);

		patialAdd = true;
	}

//	if ( full && patialAdd )
//	{

//	}
	if ( full && !patialAdd )
	{
		errCode = 3;
	}

	string strData;
	S_APPEND_BYTE( strData, 0 );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_REQ_ALL, strData) );

	return 0;
}


int FactionCtrl::clientGuildGetSayList( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[]
//		S:[[
//		BYTE(职位),char[22](成员名字),dword(时间)
//		]] 30条

	string strData;

	if ( role->faction == NULL )
		return 0;

//	int i = 30;
	for ( deque<FACTION_CHAT>::reverse_iterator it = role->faction->deqFactionChat.rbegin(); 
			it != role->faction->deqFactionChat.rend(); ++it )
	{	
		S_APPEND_DWORD( strData, it->dwRoleID );
		S_APPEND_BYTE( strData, it->facJob );
		S_APPEND_NAME( strData, it->strRoleName );
		S_APPEND_DWORD( strData, it->tTime );
		S_APPEND_BSTR( strData, it->strContent );
		
//		i++;
//		if ( i == 30 )
//			break;

	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_SAY_LIST, strData) );
	return 0;
}



bool FactionCtrl::addRoleFacContribute(ROLE* role, DWORD dwAddContribute, FACTION_MEMBER* fm )
{
	if ( fm == NULL )
	{
		fm = FactionCtrl::getFacMember( role );
		if ( fm == NULL )
			return false;
	}

	role->faction->needSortContribute = true;
	_needSortFaction = true;
	
	ADDUP( fm->dwContribute, dwAddContribute );
	ADDUP( role->faction->dwContribute, dwAddContribute );
	
	notifyTypeValue( role, ROLE_PRO_FACTION_CONTRIBUTE, role->faction->dwContribute );

	return true;
}
	
//公会科技
int FactionCtrl::clientGuildGetTechList( ROLE* role, unsigned char * data, size_t dataLen )
{
//每项科技均拥有自己的等级，默认均为Lv.1，上限为Lv.30
//每项科技均拥有自己的进度，进度积累满之后，若满足升级的需要条件，则该科技的等级+1
//进度可以通过捐献金币或捐献装备来积累

//		C:[]
//		S:[[
//		BYTE(科技id),byte(科技等级),dword(当前科技经验)
//		]]

	E_FACTION_JOB facJob = FactionCtrl::getRoleFacJob( role );
	if ( facJob == E_NONE || facJob == E_MEMBER )
		return 0;

	string strData;

	for ( int i=0; i<E_BUILDER_NUMS; i++) 
	{
		S_APPEND_BYTE( strData, i+1 );
		S_APPEND_BYTE( strData, role->faction->builderLevel[ i ] );
		S_APPEND_DWORD( strData, role->faction->builderExp[ i ] );
	}

	
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_TEACH_LIST, strData) );
	return 0;
}


//捐献装备(科技升级)
int FactionCtrl::clientGuildDonateEquip( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[BYTE(科技id),[装备id列表]]
//		s:[
//		byte(error返回值 0是成功),byte(科技id),byte(科技等级),dword(当前科技经验)
//		]


//穿在身上的装备不能捐献

	string strData;

	BYTE byBuilderID;

	FACTION_MEMBER* fm = FactionCtrl::getFacMember( role );
	if ( fm == NULL )
		return 0;

	if ( !BASE::parseBYTE( data, dataLen, byBuilderID) )
		return -1;

	if ( !IS_VALID_BUILDER_ID(byBuilderID) )
		return 0;

	list<ITEM_CACHE*> rmList;
	
	int nums = dataLen / sizeof(DWORD);
	for( int i=0; i<nums; i++)
	{
		ITEM_CACHE* item;

		if ( !BASE::parseDWORD( data, dataLen, (*(DWORD*)&item)) )
			return -1;

		if ( find( role->m_packmgr.lstEquipPkg.begin(), role->m_packmgr.lstEquipPkg.end(), item ) == role->m_packmgr.lstEquipPkg.end() || !IS_EQUIP(item->itemCfg) )
		{
			S_APPEND_BYTE( strData, 1 );
			PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_TEACH_UP_LIST, strData) );
			return 0;
		}

		if ( find( rmList.begin(), rmList.end(), item ) == rmList.end() )
			rmList.push_back( item );
	}

	if ( rmList.empty() )
	{
		S_APPEND_BYTE( strData, 2 );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_TEACH_UP_LIST, strData) );
		return 0;
	}


	//DWORD dwFightValue = 0;
	DWORD dwDonatePoint = 0;
	for ( auto it : rmList )
	{
		//dwFightValue += it->wFightValue;
		dwDonatePoint += CONFIG::getEquipDonatePoint( it->byQuality, it->itemCfg->sub_type );

		//客户端负责刷新
		role->m_packmgr.rmItemInPkg(it);
	}


	//5点实力=1点进度点数=1帮贡点数
	DWORD dwAddExp;
	DWORD dwAddContribute;
	dwAddExp = dwAddContribute  = dwDonatePoint;
 
	if ( dwAddExp )
	{
		role->faction->addBuilderExp( (E_BUILDER_TYPE)byBuilderID, dwAddExp );

		addRoleFacContribute( role, dwAddContribute, fm );


	}

	S_APPEND_BYTE( strData, 0 );
	S_APPEND_BYTE( strData, byBuilderID );
	S_APPEND_BYTE( strData, role->faction->builderLevel[ byBuilderID -1] );
	S_APPEND_DWORD( strData, role->faction->builderExp[ byBuilderID -1] );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_TEACH_UP_LIST, strData) );
	
	return 0;
}

//[coin]
//#金币捐献   5000    等于    1   贡献
//coin = 5000

//[equip_donate]
//#装备捐献   ——根据装备的品质和类型决定贡献值

//#   品质    武器    衣服    头盔    腰带    鞋子    项链    手镯    戒指    勋章    宝石
//    1   3   2   2   1   1   1   1   1   2   2
//    2   6   5   4   3   3   3   3   3   4   4
//    3   10  8   6   5   5   4   4   4   6   6
//    4   13  10  8   7   7   6   6   6   8   8
//    5   17  13  10  9   9   7   8   8   10  10
//    6   20  16  12  11  10  9   9   9   12  12
//    7   23  18  14  13  12  10  11  11  14  14


//捐献金币
int FactionCtrl::clientGuildDonateCoin( ROLE* role, unsigned char * data, size_t dataLen )
{
//		C:[BYTE(科技id),DWORD(金币)]
//		s:[
//		byte(error返回值 0是成功),byte(科技id),byte(科技等级),dword(当前科技经验)
//		]
	
 	string strData;
	BYTE byBuilderID;
	DWORD dwCoinInput;

	if ( !BASE::parseBYTE( data, dataLen, byBuilderID) )
		return -1;
	
	if ( !BASE::parseDWORD( data, dataLen, dwCoinInput) )
		return -1;
	
	FACTION_MEMBER* fm = FactionCtrl::getFacMember( role );
	if ( fm == NULL )
		return 0;

	if ( !IS_VALID_BUILDER_ID(byBuilderID) )
		return 0;

	if ( role->dwCoin < dwCoinInput )
	{
		S_APPEND_BYTE( strData, 1 );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_TEACH_UP_GOLD, strData) );
		return 0;
	}
	

	DWORD eachCoin = CONFIG::getCoinDonateEach();

	DWORD dwAddExp;
	DWORD dwAddContribute;
	dwAddExp = dwAddContribute = dwCoinInput / eachCoin;

	DWORD dwCoinCost = dwCoinInput / eachCoin * eachCoin;
	
	if ( dwAddExp )
	{
		ADDUP( role->dwCoin, -(int)dwCoinCost );
		role->faction->addBuilderExp( (E_BUILDER_TYPE)byBuilderID, dwAddExp );

		addRoleFacContribute( role, dwAddContribute, fm );

		notifyCoin(role);
	
	}
	else
	{
		S_APPEND_BYTE( strData, 2 );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_TEACH_UP_GOLD, strData) );
		return 0;
	}

	S_APPEND_BYTE( strData, 0 );
	S_APPEND_BYTE( strData, byBuilderID );
	S_APPEND_BYTE( strData, role->faction->builderLevel[ byBuilderID -1] );
	S_APPEND_DWORD( strData, role->faction->builderExp[ byBuilderID -1] );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_TEACH_UP_GOLD, strData) );

	return 0;
}



