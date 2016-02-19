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
//		//ϵͳ�㲥֪ͨ, INFO:������������,���ݹ㲥���Ͳ�ͬ,���岻ͬ���ݸ�ʽ
//		eType:�㲥����.

enum E_BC_TYPE_EQUIP
{
	E_BC_ENHANCE = 1,		//1:ǿ��
	E_BC_COMPOSE = 2,		//2:�ϳ�
};


enum E_BC_TYPE_FACTION
{
	E_BC_CREATE = 1,				//1:����
	E_BC_RANK_CHANGE = 2,		//2:���иı�
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
