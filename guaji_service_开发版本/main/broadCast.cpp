#include "pch.h"
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
#include "service.h"


#if 0
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

//18	��¼�㲥
//1�������У����µ�һ��<�������>�İ���������ƣ��ط����֣���������
//2ս�����У����ֵ�һ����������ƣ����� < ��Ѫ����>
//3�ȼ����У�������ȼ���һ�ĸ���������ƣ����� < ��Ѫ����>
//4�������У������׵�һ���������������ƣ����� < ��Ѫ����>
//5���䳡���У�ʵս������һ����������ƣ����� < ��Ѫ����>
//6��ɱ�����У���ɱ���һɱ��������ƣ����� < ��Ѫ����>
//7ɱ¾�����У�ɱ¾���һɱ��������ƣ����� < ��Ѫ���� >
//
//info:{BYTE, DATA}
//				{type, data}
//			data:
//			type : 1
//			{BSTR, BSTR}
//					{factionName, playerName}
//				type:2
//				{BSTR}
//					{playerName}
//				type:3
//				{BSTR}
//					{playerName}
//				type:4
//				{BSTR}
//					{playerName}
//				type:5
//				{BSTR}
//					{playerName}
//				type:6
//				{BSTR}
//					{playerName}
//				type:7
//				{BSTR}
//					{playerName}


#endif




void BroadcastCtrl::systemBroadcast(const string& strContent)
{
	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::text);//eType
	out.write_string(strContent);
	service::broadcast(out);
}

void BroadcastCtrl::arenaRank(ROLE* role, int rank)
{
	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::arena_rank);//eType
	out.write_byte(rank);
	out.write_string(role->roleName);

	service::broadcast(out);
}

void BroadcastCtrl::getTitle(ROLE* role, WORD titleId)
{
	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::get_title);
	out.write_ushort(titleId);
	out.write_string(role->roleName);

	service::broadcast(out);
}


void BroadcastCtrl::mapOpen(ROLE* role, WORD mapId)
{
	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::map_open);//eType
	out.write_ushort(mapId);
	out.write_string(role->roleName);
	service::broadcast(out);
}


void BroadcastCtrl::levelUpgrade(ROLE* role)
{
	if (role->wLevel < 20)
		return;
	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::level_up);//eType
	out.write_ushort(role->wLevel);
	out.write_string(role->roleName);
	service::broadcast(out);
}

void BroadcastCtrl::enhanceCompose(ROLE* role, E_BC_TYPE_EQUIP bcType, BYTE quality, BYTE doLvel, WORD equipIndex)
{
	if (bcType == E_BC_ENHANCE)
	{
		if (doLvel < 7)
			return;
	}
	else if (bcType == E_BC_COMPOSE)
	{
		if (doLvel < 5)
			return;
	}
	else
		return;

	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::enhance_compose);//eType
	out.write_byte(bcType);
	out.write_byte(quality);
	out.write_byte(doLvel);
	out.write_ushort(equipIndex);
	out.write_string(role->roleName);
	service::broadcast(out);
}

void BroadcastCtrl::faction(E_BC_TYPE_FACTION bcType, int rank, const string& playerName, const string& factionName)
{
	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::guild_create);
	out.write_byte(bcType);
	out.write_byte(rank);
	out.write_string(playerName);
	out.write_string(factionName);
	service::broadcast(out);
}

void BroadcastCtrl::rank_top_login(const e_top_type type, ROLE* role)
{
	output_stream out(S_SYSTEM_BROADCAST);
	out.write_byte(e_broadcast_type::rank_top_login);
	out.write_byte(byte(type));
	if (e_top_type::e_faction == type )
	{
		if (role->faction == nullptr)
		{
			return;
		}
		out.write_string( role->faction->name );
	}
	out.write_string(role->roleName);
	service::broadcast(out);
}

