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

enum class e_top_type : byte
{
	e_faction = 1,//门派排行：天下第一帮<帮会名称>的帮主玩家名称，重返武林，福泽天下
	e_fightvalue = 2,//战力排行：武林第一高手玩家名称，来到 < 热血大侠>
	e_level = 3,//等级排行：花名册等级第一的高手玩家名称，来到 < 热血大侠>
	e_weapon = 4,//武器排行：兵器谱第一神兵的主人玩家名称，来到 < 热血大侠>
	e_arena = 5,//比武场排行：实战榜比武第一高手玩家名称，来到 < 热血大侠>
	e_lastkill = 6,//连杀榜排行：连杀榜第一杀手玩家名称，来到 < 热血大侠>
	e_kill = 7,//杀戮榜排行：杀戮榜第一杀手玩家名称，来到 < 热血大侠 >
	e_none,//无
};

enum e_broadcast_type : byte
{
	text, // 文本广播
	arena_rank = 1,// 竞技场
	get_title = 2,// 取得称号
	activity_switch = 3, // 世界活动开关
	map_open = 4, // 打开新地图
	level_up = 5, // 升级
	rank_changed = 6, // 排行榜改变
	vip_up = 7, // VIP提升
	enhance_compose = 8, // 合成/强化
	new_skill = 9,// 领悟新技能
	guild_create = 10, // 公会创建
	shop_get_special_item = 11, // 商店购买到特殊物品
	meet_pk_combo = 12, // 偶遇PK连杀超数量
	meet_break_combo = 13, // 偶遇中断连杀
	meet_pk_successful = 14, // 偶遇PK成功
	elitemap_done = 15, // 精英地图完成
	treasure_first = 16, // 宝藏第一广播
	meet_pk_failed = 17, // 偷袭失败广播
	rank_top_login = 18, // 排行榜第一名登陆
	bodyguard_loot_successful = 19, // 镖局 打劫成功
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
