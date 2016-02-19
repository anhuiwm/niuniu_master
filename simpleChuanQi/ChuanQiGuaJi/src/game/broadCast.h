#ifndef __BROADCAST_H__
#define __BROADCAST_H__


#include "typedef.h"
#include "config.h"

using namespace std;

//#include "role.h"
class ROLE;
class ROLE_BLOB;
struct PlayerInfo;


#pragma pack(push, 1)
				
#pragma pack(pop)

#define	S_SYSTEM_BROADCAST	0x000F
//	S:[BYTE, INFO]
//		[eType, info]
//		//系统广播通知, INFO:不定长度类型,根据广播类型不同,定义不同数据格式
//		eType:广播类型.

enum E_BC_TYPE_EQUIP
{
	E_BC_ENHANCE = 1,		//1:强化
	E_BC_COMPOSE = 2,		//2:合成
};


enum E_BC_TYPE_FACTION
{
	E_BC_CREATE = 1,				//1:创建
	E_BC_RANK_CHANGE = 2,		//2:排行改变
};

	
namespace BroadcastCtrl
{
	void systemBroadcast( ROLE* role, const string& strContent );

	void arenaRank( ROLE* role, int rank );

	void getTitle( ROLE* role, WORD titleId );


	void mapOpen( ROLE* role, WORD mapId );

	void levelUpgrade( ROLE* role );
	
	void enhanceCompose( ROLE* role, E_BC_TYPE_EQUIP bcType, BYTE quality, BYTE doLvel, WORD equipIndex );

	void faction( E_BC_TYPE_FACTION bcType, int rank, const string& playerName, const string& factionName );

};

#endif		//__BROADCAST_H__
