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

enum class e_top_type : byte
{
	e_faction = 1,//�������У����µ�һ��<�������>�İ���������ƣ��ط����֣���������
	e_fightvalue = 2,//ս�����У����ֵ�һ����������ƣ����� < ��Ѫ����>
	e_level = 3,//�ȼ����У�������ȼ���һ�ĸ���������ƣ����� < ��Ѫ����>
	e_weapon = 4,//�������У������׵�һ���������������ƣ����� < ��Ѫ����>
	e_arena = 5,//���䳡���У�ʵս������һ����������ƣ����� < ��Ѫ����>
	e_lastkill = 6,//��ɱ�����У���ɱ���һɱ��������ƣ����� < ��Ѫ����>
	e_kill = 7,//ɱ¾�����У�ɱ¾���һɱ��������ƣ����� < ��Ѫ���� >
	e_none,//��
};

enum e_broadcast_type : byte
{
	text, // �ı��㲥
	arena_rank = 1,// ������
	get_title = 2,// ȡ�óƺ�
	activity_switch = 3, // ��������
	map_open = 4, // ���µ�ͼ
	level_up = 5, // ����
	rank_changed = 6, // ���а�ı�
	vip_up = 7, // VIP����
	enhance_compose = 8, // �ϳ�/ǿ��
	new_skill = 9,// �����¼���
	guild_create = 10, // ���ᴴ��
	shop_get_special_item = 11, // �̵깺��������Ʒ
	meet_pk_combo = 12, // ż��PK��ɱ������
	meet_break_combo = 13, // ż���ж���ɱ
	meet_pk_successful = 14, // ż��PK�ɹ�
	elitemap_done = 15, // ��Ӣ��ͼ���
	treasure_first = 16, // ���ص�һ�㲥
	meet_pk_failed = 17, // ͵Ϯʧ�ܹ㲥
	rank_top_login = 18, // ���а��һ����½
	bodyguard_loot_successful = 19, // �ھ� ��ٳɹ�
};

namespace BroadcastCtrl
{
	void systemBroadcast(const string& strContent );

	void arenaRank( ROLE* role, int rank );

	void getTitle( ROLE* role, WORD titleId );

	void mapOpen( ROLE* role, WORD mapId );

	void levelUpgrade( ROLE* role );
	
	void enhanceCompose( ROLE* role, E_BC_TYPE_EQUIP bcType, BYTE quality, BYTE doLvel, WORD equipIndex );

	void faction( E_BC_TYPE_FACTION bcType, int rank, const string& playerName, const string& factionName );

	void rank_top_login( const e_top_type type, ROLE* role );
};

#endif		//__BROADCAST_H__
