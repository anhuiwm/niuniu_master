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
//			3.	世界boss活动开启/关闭
//			
//			info:{BYTE, BYTE, BSTR}
//				{bossType, activeId, playerName}
//					bossType:boss活动广播类型.
//						1:开始前5分钟广播
//						2:BOSS死亡广播
//						3:BOSS逃跑广播

//					activeId:活动Index

//				广播条件：活动开始前5分钟
//				广播内容：世界BOSS【xxx】将于每天的【19:55:00】开启，请各位勇士提前做好准备

//				世界BOSS死亡
//				广播内容：伴随一声哀嚎，世界BOSS【xxx】终于倒下，【YYY】对BOSS造成致命一击

//				世界BOSS逃跑
//				广播内容：世界BOSS【xxx】携带宝物逃窜的无影无踪，请各位勇士下次再接再厉



//			6.	排行榜变更
//				info:{BYTE, BYTE, BYTE, WORD, BSTR}
//				{rankType, rank, equipQa, equipIdx, playerName}
//					rankType:排行榜类型.
//						1:等级排行
//						2:战力排行
//						3:武器排行
//					rank:榜单排行
//					equipQa:装备品质
//					equipIdx:装备index
//					playerName:玩家名称

//				等级榜
//				广播条件：名次获得提升时，且提升后进入前20名
//				广播内容：恭喜【xxx】取得等级榜第【xxx】的好成绩

//				战力榜
//				广播条件：名次获得提升时，且提升后进入前20名
//				广播内容：恭喜【xxx】取得战力榜第【xxx】的好成绩


//				武器榜
//				广播条件：名次获得提升时，且提升后进入前20名
//				广播内容：恭喜【xxx】的装备【xxx】取得武器榜第【xxx】的好成绩


//			7.	Vip变更
//				广播条件：玩家VIp等级发生变化时
//				广播内容：恭喜【xxx】成为【vip x】
//			info:{WORD, BSTR}
//				{viplevel, playerName}
//				viplevel:达到的vip等级


//			9.	领悟技能
//				广播条件：使用技能书学会技能
//				广播内容：恭喜【xxx】领悟新技能【xxx】
//			
//			info:{WORD, BSTR}
//				{skillId, playerName}
//				skillId:技能Index



//			11.	商店购买到特殊物品
//				广播条件：待定
//				广播内容：恭喜【xxx】从黑市中获得宝物【yyy】

//			info:{WORD, BSTR}
//				{itemIdx, playerName}
//				itemIdx:道具Index

//18	登录广播
//1门派排行：天下第一帮<帮会名称>的帮主玩家名称，重返武林，福泽天下
//2战力排行：武林第一高手玩家名称，来到 < 热血大侠>
//3等级排行：花名册等级第一的高手玩家名称，来到 < 热血大侠>
//4武器排行：兵器谱第一神兵的主人玩家名称，来到 < 热血大侠>
//5比武场排行：实战榜比武第一高手玩家名称，来到 < 热血大侠>
//6连杀榜排行：连杀榜第一杀手玩家名称，来到 < 热血大侠>
//7杀戮榜排行：杀戮榜第一杀手玩家名称，来到 < 热血大侠 >
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

