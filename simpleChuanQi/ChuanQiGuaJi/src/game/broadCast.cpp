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

//			0.	系统广播
//				info:{BSTR}
//					{content}
//					content:广播内容


	string strData;

	S_APPEND_BYTE( strData, 0);//eType

	S_APPEND_BSTR( strData, strContent );

	BROAD_CAST(strData);

}

void BroadcastCtrl::arenaRank( ROLE* role, int rank )
{

//	S:[BYTE, INFO]
//		[eType, info]
//		//系统广播通知, INFO:不定长度类型,根据广播类型不同,定义不同数据格式
//		eType:广播类型.
//			
//			1.	竞技场名次
//				广播条件：名次获得提升时，且提升后进入前20名
//				广播内容：恭喜【xxx】人品爆发，夺得竞技场【第x】的好成绩
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
//			2.	获得称号
//				广播条件：当玩家获得稀有度≥3的称号时
//				广播内容：恭喜玩家【XXX】获得称号：【YYY】
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

//			4.	开启地图
//				广播条件：开启新地图，且地图id≥10
//				广播内容：恭喜【xxx】开启新地图【yyy】
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

//			5.	等级提升
//				广播条件：玩家升级，且玩家等级≥20
//				广播内容：恭喜【xxx】达到yyy级，玛法大陆的实力又加强了。
//			info:{WORD, BSTR}
//				{level, playerName}
//				level:达到的等级

	string strData;

	S_APPEND_BYTE( strData, 5 );//eType

	S_APPEND_WORD( strData, role->wLevel);
	S_APPEND_BSTR( strData, role->roleName);

	BROAD_CAST(strData);
}

void BroadcastCtrl::enhanceCompose( ROLE* role, E_BC_TYPE_EQUIP bcType, BYTE quality, BYTE doLvel, WORD equipIndex )
{

//			8.	合成/强化
//			info:{BYTE, BYTE, BYTE, WORD, BSTR}
//				{eType, equipQa, equipAdd, equipIdx, playerName}
//					eType:类型
//						1:强化
//						2:合成
//					equipQa:装备品质
//					equipAdd:强化等级
//					equipIdx:装备index
//					playerName:玩家名称

//				广播条件：强化成功时，强化等级≥7
//				广播内容：恭喜【xxx】成功的将【xxx】强化到【+ x】

//				广播条件：合成后，合成等级≥⑤
//				广播内容：恭喜【xxx】经过不懈努力，合成出了【xxx】

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

//			10.	行会
//				行会创建
//				广播条件：新行会创立
//				广播内容：恭喜【xxx】创建行会【xxx】，盛邀玛法勇士加入

//			info:{BYTE, BYTE, BSTR, BSTR}
//				{type, rank, playerName, factionName}
//				type:行会通知类型
//					1:创建
//					2:排行改变
//				rank:行会排名

//				factionName:行会名称

//				行会排行
//				广播条件：行会排名提升时，且提升后排名进入前10名
//				广播内容：行会【xxx】在会长【xxx】与众成员的努力下，成为行会排行【第3】

	string strData;

	S_APPEND_BYTE( strData, 10);//eType

	S_APPEND_BYTE( strData, bcType );
	S_APPEND_BYTE( strData, rank );
	S_APPEND_BSTR( strData, playerName);
	S_APPEND_BSTR( strData, factionName);

	BROAD_CAST(strData);

}

