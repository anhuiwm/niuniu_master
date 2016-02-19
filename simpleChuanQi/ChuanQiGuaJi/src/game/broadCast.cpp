#include "log.h"
#include "base.h"
#include "protocal.h"
#include "db.h"

#include "zlibapi.h"
#include "roleCtrl.h"
#include "factionCtrl.h"
#include "itemCtrl.h"

#include "skill.h"
#include "broadCast.h"

#include "arena.h"

#include <algorithm>
#include  <numeric> 



namespace BroadcastCtrl
{



}

#if 1



//			3.	����boss�����/�ر�
//			
//			info:{BYTE, BYTE, BSTR}
//				{bossType, activeId, playerName}
//					bossType:boss��㲥����.
//						1:��ʼǰ5���ӹ㲥
//						2:BOSS�����㲥
//						3:BOSS���ܹ㲥

//					activeId:�Index

//				�㲥���������ʼǰ5����
//				�㲥���ݣ�����BOSS��xxx������ÿ��ġ�19:55:00�����������λ��ʿ��ǰ����׼��

//				����BOSS����
//				�㲥���ݣ�����һ������������BOSS��xxx�����ڵ��£���YYY����BOSS�������һ��

//				����BOSS����
//				�㲥���ݣ�����BOSS��xxx��Я�������Ӵܵ���Ӱ���٣����λ��ʿ�´��ٽ�����



//			6.	���а���
//				info:{BYTE, BYTE, BYTE, WORD, BSTR}
//				{rankType, rank, equipQa, equipIdx, playerName}
//					rankType:���а�����.
//						1:�ȼ�����
//						2:ս������
//						3:��������
//					rank:������
//					equipQa:װ��Ʒ��
//					equipIdx:װ��index
//					playerName:�������

//				�ȼ���
//				�㲥���������λ������ʱ�������������ǰ20��
//				�㲥���ݣ���ϲ��xxx��ȡ�õȼ���ڡ�xxx���ĺóɼ�

//				ս����
//				�㲥���������λ������ʱ�������������ǰ20��
//				�㲥���ݣ���ϲ��xxx��ȡ��ս����ڡ�xxx���ĺóɼ�


//				������
//				�㲥���������λ������ʱ�������������ǰ20��
//				�㲥���ݣ���ϲ��xxx����װ����xxx��ȡ��������ڡ�xxx���ĺóɼ�


//			7.	Vip���
//				�㲥���������VIp�ȼ������仯ʱ
//				�㲥���ݣ���ϲ��xxx����Ϊ��vip x��
//			info:{WORD, BSTR}
//				{viplevel, playerName}
//				viplevel:�ﵽ��vip�ȼ�


//			9.	������
//				�㲥������ʹ�ü�����ѧ�Ἴ��
//				�㲥���ݣ���ϲ��xxx�������¼��ܡ�xxx��
//			
//			info:{WORD, BSTR}
//				{skillId, playerName}
//				skillId:����Index



//			11.	�̵깺��������Ʒ
//				�㲥����������
//				�㲥���ݣ���ϲ��xxx���Ӻ����л�ñ��yyy��

//			info:{WORD, BSTR}
//				{itemIdx, playerName}
//				itemIdx:����Index


#endif



#define BROAD_CAST(strData)		\
	do {\
		string strPkt = PROTOCAL::cmdPacket(S_SYSTEM_BROADCAST, strData);\
		PROTOCAL::forEachOnlineRole( [ &strPkt ] ( const std::pair<DWORD, ROLE_CLIENT> &x )\
			{\
				PROTOCAL::sendClient( x.second.client, strPkt );\
			}\
		);\
	}while(0)


void BroadcastCtrl::systemBroadcast( ROLE* role, const string& strContent )
{

//			0.	ϵͳ�㲥
//				info:{BSTR}
//					{content}
//					content:�㲥����


	string strData;

	S_APPEND_BYTE( strData, 0);//eType

	S_APPEND_BSTR( strData, strContent );

	BROAD_CAST(strData);

}

void BroadcastCtrl::arenaRank( ROLE* role, int rank )
{

//	S:[BYTE, INFO]
//		[eType, info]
//		//ϵͳ�㲥֪ͨ, INFO:������������,���ݹ㲥���Ͳ�ͬ,���岻ͬ���ݸ�ʽ
//		eType:�㲥����.
//			
//			1.	����������
//				�㲥���������λ������ʱ�������������ǰ20��
//				�㲥���ݣ���ϲ��xxx����Ʒ��������þ���������x���ĺóɼ�
//			info:{BYTE, BSTR}
//				{rank, playerName}


	string strData;

	S_APPEND_BYTE( strData, 1);//eType

	S_APPEND_BYTE( strData, rank);
	S_APPEND_BSTR( strData, role->roleName);

	BROAD_CAST(strData);

}

void BroadcastCtrl::getTitle( ROLE* role, WORD titleId )
{
//			2.	��óƺ�
//				�㲥����������һ��ϡ�жȡ�3�ĳƺ�ʱ
//				�㲥���ݣ���ϲ��ҡ�XXX����óƺţ���YYY��
//			info:{WORD, BSTR}
//				{titleId, playerName}

	string strData;

	S_APPEND_BYTE( strData, 2);//eType

	S_APPEND_WORD( strData, titleId);
	S_APPEND_BSTR( strData, role->roleName);

	BROAD_CAST(strData);
}


void BroadcastCtrl::mapOpen( ROLE* role, WORD mapId )
{
	if ( mapId < 10 )
		return;

//			4.	������ͼ
//				�㲥�����������µ�ͼ���ҵ�ͼid��10
//				�㲥���ݣ���ϲ��xxx�������µ�ͼ��yyy��
//			info:{WORD, BSTR}
//				{mapId, playerName}

	string strData;

	S_APPEND_BYTE( strData, 4);//eType

	S_APPEND_WORD( strData, mapId);
	S_APPEND_BSTR( strData, role->roleName);

	BROAD_CAST(strData);
}


void BroadcastCtrl::levelUpgrade( ROLE* role )
{

	if ( role->wLevel < 20 )
		return;

//			5.	�ȼ�����
//				�㲥�������������������ҵȼ���20
//				�㲥���ݣ���ϲ��xxx���ﵽyyy�����귨��½��ʵ���ּ�ǿ�ˡ�
//			info:{WORD, BSTR}
//				{level, playerName}
//				level:�ﵽ�ĵȼ�

	string strData;

	S_APPEND_BYTE( strData, 5 );//eType

	S_APPEND_WORD( strData, role->wLevel);
	S_APPEND_BSTR( strData, role->roleName);

	BROAD_CAST(strData);
}

void BroadcastCtrl::enhanceCompose( ROLE* role, E_BC_TYPE_EQUIP bcType, BYTE quality, BYTE doLvel, WORD equipIndex )
{

//			8.	�ϳ�/ǿ��
//			info:{BYTE, BYTE, BYTE, WORD, BSTR}
//				{eType, equipQa, equipAdd, equipIdx, playerName}
//					eType:����
//						1:ǿ��
//						2:�ϳ�
//					equipQa:װ��Ʒ��
//					equipAdd:ǿ���ȼ�
//					equipIdx:װ��index
//					playerName:�������

//				�㲥������ǿ���ɹ�ʱ��ǿ���ȼ���7
//				�㲥���ݣ���ϲ��xxx���ɹ��Ľ���xxx��ǿ������+ x��

//				�㲥�������ϳɺ󣬺ϳɵȼ��ݢ�
//				�㲥���ݣ���ϲ��xxx��������иŬ�����ϳɳ��ˡ�xxx��

	if ( bcType ==E_BC_ENHANCE )
	{
		if ( doLvel < 7 )
			return;
	}
	else if ( bcType ==E_BC_COMPOSE )
	{
		if ( doLvel < 5 )
			return;
	}
	else
		return;


	string strData;

	S_APPEND_BYTE( strData, 8);//eType

	S_APPEND_BYTE( strData, bcType );
	S_APPEND_BYTE( strData, quality );
	S_APPEND_BYTE( strData, doLvel);
	S_APPEND_WORD( strData, equipIndex);
	S_APPEND_BSTR( strData, role->roleName);

	BROAD_CAST(strData);

}

void BroadcastCtrl::faction( E_BC_TYPE_FACTION bcType, int rank, const string& playerName, const string& factionName )
{

//			10.	�л�
//				�лᴴ��
//				�㲥���������лᴴ��
//				�㲥���ݣ���ϲ��xxx�������л᡾xxx����ʢ���귨��ʿ����

//			info:{BYTE, BYTE, BSTR, BSTR}
//				{type, rank, playerName, factionName}
//				type:�л�֪ͨ����
//					1:����
//					2:���иı�
//				rank:�л�����

//				factionName:�л�����

//				�л�����
//				�㲥�������л���������ʱ������������������ǰ10��
//				�㲥���ݣ��л᡾xxx���ڻ᳤��xxx�����ڳ�Ա��Ŭ���£���Ϊ�л����С���3��

	string strData;

	S_APPEND_BYTE( strData, 10);//eType

	S_APPEND_BYTE( strData, bcType );
	S_APPEND_BYTE( strData, rank );
	S_APPEND_BSTR( strData, playerName);
	S_APPEND_BSTR( strData, factionName);

	BROAD_CAST(strData);

}

