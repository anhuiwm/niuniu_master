
#include <algorithm>

#include "protocal.h"
#include "roleCtrl.h"
#include "itemCtrl.h"
#include "factionCtrl.h"
#include "mailCtrl.h"
#include "mineCtrl.h"
#include "config.h"
#include "skill.h"
#include "friend.h"
#include "arena.h"
#include "item.h"

#include "rank.h"
#include "sign.h"

#include "base.h"
#include "db.h"
#include "jsonpacket.h"
#include "activity.h"
#include "shop.h"

#include "broadCast.h"
#include "task.h"
#include "quickfight.h"
#include "fightCtrl.h"
#include "common.h"
#include "TimerMgr.h"
#include "WorldBoss.h"
#include "logs_db.h"
#include "service.h"
#include "output_stream.h"

using namespace BASE;
//using namespace CONFIG;


namespace PROTOCAL
{
	static map<DWORD, ROLE_CLIENT> _mapOnlineRoleClient;//只有在线角色,key:roleid


	static time_t _curTime;

	static deque<string> deqChat;


}

//#define NEED_REFRESH(role, i, now) ( role->vMiner[i].dwRoleID == 0 && now >= role->vMiner[i].startTime + MINE_FREE_SECS )

//每次刷新数据时，判定矿点闲置时间，若超过10分钟，则为该矿点分配一个随机的陪同玩家

bool isRoleNameValid(const string & roleName)
{
	return !CONFIG::isInnerSieldWord(roleName);
}

void PROTOCAL::sendClient(session* client, const string& str)
{
	if (NULL == client)
	{
		return;
	}

	bufferevent_write(client->client_bev, str.data(), str.size() );
}


#define WriteClient3(client, cstr, len)		bufferevent_write(client->client_bev, cstr, len )

string& PROTOCAL::makePkt3(string& strPKT, WORD wCmd, const string& strInfo)
{
	PKT_DATA stcHead;
	memset(&stcHead, 0, HEAD_SIZE);
	stcHead.cmd = wCmd;

	stcHead.len = HEAD_SIZE + strInfo.size();
	strPKT.append((char*)&stcHead, HEAD_SIZE);
	if (!strInfo.empty())
		strPKT.append(strInfo);

	return strPKT;
}

string PROTOCAL::cmdPacket(WORD wCmd, const string& strInfo)
{

	string strPKT; 

	PKT_DATA stcHead;
	memset(&stcHead, 0, HEAD_SIZE);
	stcHead.cmd = wCmd;

	stcHead.len = HEAD_SIZE + strInfo.size();

	strPKT.append((char*)&stcHead, HEAD_SIZE);
	if (!strInfo.empty())
		strPKT.append(strInfo);

	return strPKT;
}

#define SEND4( client, cmd, errcode, str) \
	do {\
	string strData;\
	S_APPEND_WORD( strData, errcode );\
	strData.append(str);\
	sendClient( client, cmdPacket( cmd, strData ) );\
	}while(0)

#define ADD_CLIENT( roleID, role, client)	\
	client->role_id = roleID;\
	role->client = client;\
	_mapOnlineRoleClient.insert( make_pair( roleID, ROLE_CLIENT(role,client) ) )


void roleSave(time_t tNow, bool forceSave)
{
	for ( auto it : PROTOCAL::_mapOnlineRoleClient )
	{
		if ( forceSave || tNow - it.second.role->tLastSaveTime > 60 )
		{
			it.second.role->tLastSaveTime = tNow;
			it.second.role->tLogoutTime = tNow;		//退出时间,防止断线
			it.second.role->roleSave2DB();
		}
	}
}

void saveAllToDb( bool bForce )
{
	time_t tNow = PROTOCAL::getCurTime();
	roleSave( tNow, bForce );
	JSON::writeGsTime();
	RANK::saveRank();
	service::save_data();
	WorldBoss::getMe().save_to_db();
}

void logOnlineClient()
{
	PROTOCAL::forEachOnlineRole([](const std::pair<DWORD, ROLE_CLIENT> &x)
	{
		DWORD id = x.second.role->dwRoleID;
		string name = x.second.role->roleName;
		logonline( "id=%d name=%s", id,name.c_str() );
	});
}
//#define loginSendChat(role)		\
//	do {\
//		for ( auto it : deqChat )	sendClient( role->client, it );\
//	}while(0)


int PROTOCAL::roleLogin( session *client, unsigned char * data, size_t dataLen )
{
	//	C:[BSTR]
	//		[uuid]
	//	S:[WORD, PlayerInfo]
	//		[ERROR, playerInfo]
	//			ERROR:
	//				0:成功
	//				1:角色不存在
	//			playerInfo:角色基本信息

	string uuid;
	if ( !BASE::parseBSTR( data, dataLen, uuid) )
		return 0;

	//wmevtimer_del(&client->timer);


	//某条连接重复登陆
	if ( client->role_id )
	{
		SEND4( client, S_LOGIN, 3, "");
		return 0;
	}

	ROLE* role = RoleMgr::getMe().findRoleByUUID(uuid);
	if ( role == NULL)
	{
		SEND4( client, S_LOGIN, 1, "");
		return 0;
	}

	// 封停账号处理
	if(role->close_user_valid_time != 0 && role->close_user_valid_time > time(0))
	{
		onDisconnect(client);
		return 0;
	}

	//已登陆玩家开另一个客户端登陆
	auto it = _mapOnlineRoleClient.find( role->dwRoleID );
	if ( it != _mapOnlineRoleClient.end() )
	{
		onDisconnect(it->second.client);
	}

	ADD_CLIENT(role->dwRoleID, role, client);

	WORD wOldLevel = role->wLevel;

	PlayerInfo playerInfo(role);

	string strData;
	S_APPEND_WORD( strData, 0 );
	S_APPEND_SZ( strData, (char*)&playerInfo, sizeof(PlayerInfo) );
	sendClient( role->client, cmdPacket( S_LOGIN, strData ) );

	//XP2( strPKT.data(), strPKT.size() );

	MailCtrl::loadMailFromDB( role );
	MailCtrl::onlineNotifyMail( role );


	if ( wOldLevel != role->wLevel )
		BroadcastCtrl::levelUpgrade( role );

	TASK::autoUnlockTask( role );

	RoleMgr::getMe().timerDailyClean(role, PROTOCAL::getCurTime() );

	role->roleZone = RoleMgr::getMe().loadZoneInfoFromDB( role );

	role->online();

	ITEM_INFO para1(0, role->getVipLevel());
	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_VIP_LOGIN, &para1);

	string sql = "insert into log_login values(0," + to_string(service::id) + ",'" +
		role->roleName+"', "+to_string(time(0))+", "+to_string(role->dwRoleID)+
		", "+to_string(role->wLevel)+", "+to_string(role->dwExp)+", "+
		to_string(role->dwCoin)+", "+to_string(role->dwIngot)+", "+
		to_string(role->dwCharge)+",'"+client->ip+"')";
	logs_db_thread::singleton().push(sql);

	return 0;
}



string mkChatPkt(ROLE* role, BYTE channel, const string& strContent )
{
	string strData;

	//	S:[WORD, DWORD, BYTE, BYTE, BYTE, BYTE, BYTE, DWORD, BSTR, BSTR, BSTR]
	//		[error, playerId, channel, job, sex, vipLv, titleId, factionId, playeName, factionName, content]


	S_APPEND_WORD( strData, 0 );
	S_APPEND_DWORD( strData, role->dwRoleID );
	S_APPEND_BYTE( strData, channel );


	S_APPEND_BYTE( strData, role->byJob );
	S_APPEND_BYTE( strData, role->bySex );
	S_APPEND_BYTE( strData, role->getVipLevel() );
	S_APPEND_BYTE( strData, role->byTitleId );

	S_APPEND_DWORD( strData, role->faction ? role->faction->dwFactionID : 0  );

	S_APPEND_BSTR( strData, role->roleName );

	if ( role->faction == NULL )
		S_APPEND_BSTR_NULL( strData);
	else
		S_APPEND_BSTR( strData, role->faction->strName );
	//string newContent = strContent;
	//newContent.append(role->roleZone);
	S_APPEND_BSTR( strData, role->roleZone );
	S_APPEND_BSTR( strData, strContent );
	//S_APPEND_BSTR( strData, newContent );

	return PROTOCAL::cmdPacket( S_GAME_CHAT, strData );
}

int gmMakeItem(ROLE*role, WORD itemIndex, size_t itemNum, BYTE byQua = 1)
{
	//	WORD itemIndex = ATOW_( vecString[1]  );
	//	WORD itemNum = 1;

	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(itemIndex);
	if ( itemCfg == NULL )
		return 0;

	list<ITEM_CACHE*> lstItemCachePkg, lstItemCacheMine;

	//if ( IS_MINE(itemCfg ) )
	//{
	//	if ( E_SUCC == role->m_packmgr.makeItem( itemCfg, itemNum, &lstItemCacheMine) )
	//	{
	//		ItemCtrl::notifyClientItem(role, lstItemCacheMine, PKG_TYPE_MINE);
	//		return 0;		
	//	}
	//	return -1;
	//}

	if ( IS_PROPS(itemCfg) )
	{
		if ( E_SUCC == role->m_packmgr.makeItem( itemCfg, itemNum, &lstItemCachePkg, false, byQua ) )
		{
			ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_ITEM);
			return 0;		
		}
		return 0;
	}
	else if (IS_DIVINE(itemCfg))
	{
		if ( E_SUCC == role->m_packmgr.makeItem( itemCfg, itemNum, &lstItemCachePkg ) )
		{
			ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_FUWEN);
			return 0;		
		}
	}
	else if (IS_EQUIP(itemCfg) )
	{
		if ( E_SUCC == role->m_packmgr.makeItem( itemCfg, itemNum, &lstItemCachePkg, false, byQua ) )
		{
			ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_EQUIP);
			return 0;		
		}
	}


	return -1;
}


int gmSendMail( const string& roleName, const string& mailType, const string& content, const string& prize )
{
	DWORD dwRoleID = 0;

	ROLE* role2 = RoleMgr::getMe().getCacheRoleByName(roleName);
	if ( role2 )
	{
		dwRoleID = role2->dwRoleID;
	}
	else
	{
		MYSQL_ESCAPE_N(szRoleName, roleName.data(), min<size_t>(roleName.size(), ROLE_NAME_LEN), ROLE_NAME_LEN );

		SQLSelect sqlx( DBCtrl::getSQL(), "select id from " TABLE_ROLE_INFO  " where rolename='%s' ", szRoleName );

		MYSQL_ROW row = sqlx.getRow();
		if ( row==NULL )
			return -1;

		dwRoleID = ATOUL(row[0]);
	}

	vector<ITEM_INFO> vecItemInfo;
	CONFIG::parseItemInfoStr( prize, vecItemInfo );

	MailCtrl::sendMail( dwRoleID, ATOB_(mailType), content, vecItemInfo );

	return 0;

}

int PROTOCAL::gm(ROLE* role, const string& strContent)
{
	//return 0;
	//"$item index num attr1 value1 attr2 value2"

	vector<string> vecString;
	strtoken2Vec( strContent.c_str(), SPLIT_SPACE, vecString, NULL, NULL);

	if ( !vecString.empty() )
	{
		logonline("usegm:%d  %s",role->dwRoleID,vecString[0].c_str());
	}
	
	if ( service::gm_no_need_name == 0 )
	{
		if ( vecString[0] == "$reloadgmnamecfg" )
		{
			if ( vecString.size() < 2 )
				return 0;
			return CONFIG::readCfgGmName();
		}

		if ( !CONFIG::isGmName(role->roleName) )
		{
			return 0;
		}
	}

	if ( vecString[0] == "$reload" )
	{
		if ( vecString.size() < 2 )
			return 0;
		return CONFIG::reloadCfg(vecString[1]);

	}
	else if ( vecString[0] == "$item" )
	{
		if ( vecString.size() < 2 )
			return 0;

		return gmMakeItem( role, ATOW_( vecString[1]), ( vecString.size() >= 3 ? ATOUL_(vecString[2]):1 ), BYTE( vecString.size() >= 4 ? ATOUL_(vecString[3]):1 ) );

	}
	else if (vecString[0] == "$clean")// 竞技场次数清零
	{
		RoleMgr::getMe().timerDailyClean( role, PROTOCAL::getCurTime() ,true);
	}
	else if (vecString[0] == "$signtimes")
	{
		if ( vecString.size() < 2 )
			return -1;
		role->bySignTimes = ATOB_( vecString[1]);
	}
	else if (vecString[0] == "$charge")//竞技场次数清零
	{
		if ( vecString.size() < 2 )
			return -1;
		DWORD dwChargeNum = ATOUL_( vecString[1] );
		PROTOCAL::chargeRole( NULL, 0, dwChargeNum, role, true );
	}
	else if (vecString[0] == "$level")//等级上升
	{
		if ( vecString.size() < 2  || ATOW_( vecString[1]) > 79 )
			return -1;

		role->wLevel = ATOW_( vecString[1]);
		RoleMgr::getMe().onLevelUP(role,role->wLevel);
		notifyLevel(role);		
	}
	else if (vecString[0] == "$viplevel")
	{
		if ( vecString.size() < 2 || ATOW_( vecString[1]) > 10)
			return -1;

		role->wVipLevel = ATOW_( vecString[1]);
		notifywVipLevel(role);
	}
	else if (vecString[0] == "$repute")
	{

		if ( vecString.size() < 2 )
			return -1;
		ADDUP(role->dwHonour, ATOUL_( vecString[1]));
		notifyHonour(role);
	}
	else if (vecString[0] == "$smelt")
	{

		if ( vecString.size() < 2 )
			return -1;
		ADDUP(role->dwSmeltValue, ATOUL_( vecString[1]));
		notifySmeltValue(role);
	}
	else if (vecString[0] == "$money" || vecString[0] == "$coin" )//wm 加钱
	{	
		if ( vecString.size() < 2 )
			return -1;
		auto value = ATOUL_( vecString[1]);
		ADDUP(role->dwCoin, value);
		//logs_db_thread::singleton.consume(service::id, role->roleName, role->dwRoleID, value, 0, purpose::gm, 0);
		notifyCoin(role);	
	}
	else if (vecString[0] == "$gold"|| vecString[0] == "$ingot")//wm 加元宝
	{

		if ( vecString.size() < 2 )
			return -1;
		auto value = ATOUL_( vecString[1]);
		ADDUP(role->dwIngot, value);
		//logs_db_thread::singleton.consume(service::id, role->roleName, role->dwRoleID, 0, value, purpose::gm, 0);
		notifyIngot(role);	
	}
	else if (vecString[0] == "$task")//人物清空
	{
		role->mapTask.clear();
		TASK::autoUnlockTask(role);
	}
	else if (vecString[0] == "$mail")//$mail rolename mailtype content prize
	{
		if ( (vecString.size() >= 2 && vecString[1]=="-h" )|| vecString.size()==1 )
		{
			string strData = mkChatPkt( role, 1, "$mail rolename mailtype content prize" );
			sendClient( role->client, strData );
			return 0;
		}

		if ( vecString.size() < 5 )
			return -1;

		const string& roleName = vecString[1];
		const string& mailType = vecString[2];
		const string& content = vecString[3];
		const string& prize = vecString[4];

		gmSendMail( roleName, mailType, content, prize );
	}
	else if (vecString[0] == "$other" || vecString[0] == "$otherid")
	{
		if ( vecString.size() < 4 )
		{
			return 0;
		}

		ROLE* roleOther = NULL;
		if ( vecString[0] == "$other" )
		{
			roleOther = RoleMgr::getMe().getRoleByName( vecString[1] );
		}
		else if ( vecString[0] == "$otherid" )
		{
			roleOther = RoleMgr::getMe().getRoleByID( ATOUL_(vecString[1]) );
		}
		if(roleOther == NULL)
		{
			return 0;
		}

		if ( vecString[2] == "item" )
		{
			if ( vecString.size() < 2 )
				return -1;

			return gmMakeItem( roleOther, ATOW_( vecString[3]), vecString.size() >= 5 ? ATOUL_(vecString[4]):1 );

		}
		else if (vecString[2] == "clean")//竞技场次数清零
		{
			RoleMgr::getMe().timerDailyClean( roleOther, PROTOCAL::getCurTime() ,true);
		}
		else if (vecString[2] == "level")//等级上升
		{
			if ( vecString.size() < 4 || ATOW_( vecString[3]) > 50 )
				return -1;

			roleOther->wLevel = ATOW_( vecString[3]);
			RoleMgr::getMe().onLevelUP(roleOther,roleOther->wLevel);
		}
		else if (vecString[2] == "viplevel")
		{
			if ( vecString.size() < 4  || ATOW_( vecString[3]) > 15 )
				return -1;

			roleOther->wVipLevel = ATOW_( vecString[3]);
		}
		else if (vecString[2] == "money" || vecString[2] == "coin" )//加钱
		{	
			if ( vecString.size() < 4 )
				return -1;
			auto value = ATOUL_( vecString[3]);
			ADDUP(roleOther->dwCoin, value);
			//logs_db_thread::getMe().consume(service::id, role->roleName, role->dwRoleID, value, 0, purpose::gm, 0);
		}
		else if (vecString[2] == "gold"|| vecString[2] == "ingot")//加元宝
		{

			if ( vecString.size() < 4 )
				return -1;
			auto value = ATOUL_( vecString[3]);
			ADDUP(roleOther->dwIngot, value);
			//logs_db_thread::getMe().consume(service::id, role->roleName, role->dwRoleID, 0, value, purpose::gm, 0);
		}


	}
	else if (vecString[0] == "$bc")
	{
		if (vecString.size() >= 2 )
			BroadcastCtrl::systemBroadcast( role, vecString[1] );
	}
	else if (vecString[0] == "$save_data")
	{
		if (vecString.size() >= 2 )
		{
			if ( vecString[1] == "all" )
			{
				saveAllToDb( true );
			}
			else if ( vecString[1] == "online" )
			{
				logOnlineClient();
			}
		}
	}

	return -1;//
}



int PROTOCAL::clientChat( ROLE* role, unsigned char * data, size_t dataLen )
{
	// 禁言
	if(role->deny_chat)
		return 0;

	//	C:[BYTE, BSTR]
	//		[channel, content]
	//		channel:聊天频道. 道具展示(1bit)|频道(bit)
	//			最高位标记是否道具展示(0x80)|低七位作为频道号
	//			1:世界频道
	//			2:公会频道
	//		content:聊天内容

	//	S:[WORD, DWORD, BYTE, BYTE, BYTE, BYTE, BYTE, DWORD, BSTR, BSTR, BSTR]
	//		[error, playerId, channel, job, sex, vipLv, titleId, factionId, playeName, factionName, content]
	//		error:错误码
	//			0:成功
	//			1:时间限制
	//			2:被禁言
	//			3:频道不存在

	//		playerId:说话者ID
	//		channel:聊天频道, 同上(有道具展示, 聊天内容不截断)
	//		job:职业
	//		sex:性别
	//		vipLv:vip等级
	//		titleId:称号ID
	//		factionId:公会ID
	//		playeName:玩家名称
	//		factionName:公会名称
	//		content:聊天内容, 20个汉字长度,多余的截断

	enum
	{
		E_CHAT_TOO_FAST = 1,
		E_CHAT_BAN = 2,
		E_CHAT_NO_SUCH_CHANNEL = 3,

		E_GM_NO_SUCH_CMD = 11,
		E_GM_NO_SUCH_INDEX = 12,
		E_GM_INPUT = 13,
	};

	BYTE channel;
	string strContent;

	if ( !BASE::parseBYTE( data, dataLen, channel) )
		return 0;

	if ( !BASE::parseBSTR( data, dataLen, strContent) )
		return 0;

	if ( CONFIG::isInnerSieldWord(strContent) )
	{
		return 0;
	}

	if ( !strContent.empty() && strContent[0] == '$' )
	{
		gm( role, strContent );
		return 0;
	}

	channel &= 0x7f;
	if ( channel !=1 && channel != 2 )
		return 0;


	string strData = mkChatPkt( role, channel, strContent );

	if ( channel == 1 )
	{
		forEachOnlineRole( [ &strData ] ( const std::pair<DWORD, ROLE_CLIENT> &x )
		{
			sendClient( x.second.client, strData );
		}
		);


		deqChat.push_back(strData);
		if ( deqChat.size() >5 )
		{
			deqChat.pop_front();
		}

	}
	else if ( channel == 2 )
	{
		if ( role->faction == NULL )
			return 0;

		role->faction->addChat( role, strContent );

		FactionCtrl::broadCast( role->faction, strData );
	}

	//sendClient( role->client, strData );

	return 0;
}

//int PROTOCAL::chargeRole( session *client, unsigned char * data, size_t dataLen )
//{
//
//	//C:[  WORD,   WORD,  DWORD, DWORD ]
//	//	[协议长度,协议号,人物ID,元宝数 ]
//	//S:[  WORD,   WORD,  BYTE  ]
//	//	[协议长度,协议号,还回码 ]
//	//
//	//	协议号：0x0008
//	//		还回码：0成功 1失败
//
//	DWORD dwChargeRoleID, dwChargeNum;
//	if ( !BASE::parseDWORD( data, dataLen, dwChargeRoleID) )
//		return 0;
//	if ( !BASE::parseDWORD( data, dataLen, dwChargeNum) )
//		return 0;
//	return chargeRole( client, dwChargeRoleID, dwChargeNum, NULL, false );
//
//}

bool PROTOCAL::phpChargeRole( DWORD roleid, DWORD dwChargeNum, string charge_account )
{
	BYTE byRet = 0;

	ROLE* role = RoleMgr::getMe().getRoleByID(roleid);
	
	RETURN_COND( role == NULL, false);

	RETURN_COND( role->charge_account == charge_account, false);

	int dwRealNum = dwChargeNum;

	if ( std::find(role->vecFirstCharge.begin(),role->vecFirstCharge.end(),dwChargeNum) == role->vecFirstCharge.end() )
	{
		role->vecFirstCharge.push_back( dwChargeNum );
		dwChargeNum *= 3;//first charge 3 mutiple
	}

	byRet = role->roleGetCharge( (int)dwChargeNum, (int)dwRealNum );

	string strClientData;
	S_APPEND_BYTE( strClientData, byRet );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_REQUEST_BUY_INGOT, strClientData ) );

	if ( byRet == E_SUCC )
	{
		if (role->byRechargeState == E_TASK_UNCOMPLETE )
		{
			role->byRechargeState = E_TASK_COMPLETE_CAN_GET_PRIZE;
		}
	}

	role->charge_account = charge_account;

	return true;
}

//int PROTOCAL::clientActive( session *client, unsigned char * data, size_t dataLen )
//{
//	BYTE phone_type;
//
//	BYTE platform_id;
//
//	string serial;
//
//	if ( !BASE::parseBYTE( data, dataLen, phone_type) )
//		return 0;
//
//	if ( !BASE::parseBYTE( data, dataLen, platform_id) )
//		return 0;
//
//	if ( !BASE::parseBSTR( data, dataLen, serial) )
//		return 0;
//
//	string sql = "insert into log_connect( ip,time,phone_type,platform_id,serial ) values('"+client->ip+"',"+to_string(time(NULL))+","+to_string(phone_type)+","+to_string(platform_id)+"',"+serial+"')";
//	logs_db_thread::singleton().push(sql);
//}

int PROTOCAL::chargeRole( session *client, const DWORD dwChargeRoleID, DWORD dwChargeNum, ROLE* role, bool bGm )
{
	BYTE byRet = 0;
	if ( !bGm )
	{
		ROLE* role = RoleMgr::getMe().getRoleByID(dwChargeRoleID);
	}
	if ( role == NULL)
	{
		byRet = 1;
	}
	else
	{
		DWORD dwRealNum = dwChargeNum;
		if ( std::find(role->vecFirstCharge.begin(),role->vecFirstCharge.end(),dwChargeNum) == role->vecFirstCharge.end() )
		{
			role->vecFirstCharge.push_back( dwChargeNum );
			dwChargeNum *= 3;//first charge 3 mutiple
		}

		byRet = role->roleGetCharge( int(dwChargeNum), int(dwRealNum) );

		string strClientData;
		S_APPEND_BYTE( strClientData, byRet );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_REQUEST_BUY_INGOT, strClientData ) );

		if ( byRet == E_SUCC && role->byRechargeState == E_TASK_UNCOMPLETE )
		{
			role->byRechargeState = E_TASK_COMPLETE_CAN_GET_PRIZE;
		}
	}
	if ( !bGm )
	{
		string strData;
		S_APPEND_BYTE( strData, byRet );
		PROTOCAL::sendClient( client, PROTOCAL::cmdPacket( S_CHARGE_ROLE, strData ) );
	}
	return 0;
}

int PROTOCAL::roleCheckRole( session *client, unsigned char * data, size_t dataLen )
{
	//#define		S_CHECK_ROLE	0x0009
	//C:[BSTR]
	//C:[imei]
	//imei:平台帐号
	//S:[BSTR]
	//	[UUID]
	//UUID:用户UUID

	if (service::account != 1 )//1为有平台账号才继续
	{
		return 0;
	}

	BYTE platform_id;

	string serial, uuid;

	if ( !BASE::parseBYTE( data, dataLen, platform_id) )
		return 0;

	if ( !BASE::parseBSTR( data, dataLen, serial) )
		return 0;

	//evtimer_del(&client->timer);

	if ( !serial.empty() )
	{
		uuid = RoleMgr::getMe().findUuidBySerial( serial, platform_id );

		//string sql = "insert into log_active(ip,serial) values('"+client->ip+"''"+serial+"')";
		//logs_db_thread::singleton().push(sql);
	}

	string strData;
	S_APPEND_BSTR( strData, uuid );
	sendClient( client, cmdPacket(S_CHECK_ROLE, strData) );


	return 0;
}

int PROTOCAL::roleCreate( session *client, unsigned char * data, size_t dataLen )
{
	//	C:[BSTR, BSTR, BYTE, BYTE]
	//		[imei, playerName, job, sex]
	//	S:[WORD, BSTR, PlayerInfo]
	//		[ERROR, UUID, playerInfo]
	//		ERROR:
	//			0:成功
	//			1:名称不合法
	//			2:名称已存在
	//		UUID:角色唯一标识号
	//		playerInfo:
	//			角色基本信息

	ROLE* role = NULL;

	string serial, roleName;
	BYTE job=0, sex=0, phone_type=0, platform_id=0;

	if ( !BASE::parseBSTR( data, dataLen, serial) )
		return 0;

	if ( !BASE::parseBSTR( data, dataLen, roleName) )
		return 0;

	if ( !BASE::parseBYTE( data, dataLen, job) )
		return 0;

	if ( !BASE::parseBYTE( data, dataLen, sex) )
		return 0;

	if ( !BASE::parseBYTE( data, dataLen, phone_type) )
		return 0;

	if ( !BASE::parseBYTE( data, dataLen, platform_id) )
		return 0;

	//wmevtimer_del(&client->timer);

	if ( job==0 || job>3 )
	{
		SEND4( client, S_CREATE_ROLE, 3, "");
		return 0;
	}

	if ( roleName.empty() || !isRoleNameValid(roleName) || ( role = RoleMgr::getMe().newRole( serial, roleName, job, sex, phone_type, platform_id ) ) == NULL )
	{
		SEND4( client, S_CREATE_ROLE, 2, "" );
		return 0;
	}

	//某条连接重复登陆
	if ( client->role_id )
	{
		SEND4( client, S_CREATE_ROLE, 3, "" );
		return 0;
	}

	ADD_CLIENT(role->dwRoleID, role, client);
	service::player_enter_map(role, 0);


	PlayerInfo playerInfo(role);

	string strData;
	S_APPEND_WORD( strData, 0 );
	S_APPEND_BSTR( strData, role->uuid );
	S_APPEND_NBYTES( strData, (char*)&playerInfo, sizeof(PlayerInfo) );
	sendClient( role->client, cmdPacket(S_CREATE_ROLE, strData) );

	//loginSendChat(role);
	time_t now = PROTOCAL::getCurTime();
	role->tLastAccess = now;
	role->setFirstLoginTime( now );
	role->roleZone = RoleMgr::getMe().loadZoneInfoFromDB( role );
	role->role_skill.autoUnlockSkills();
	TASK::autoUnlockTask(role);
	TASK::autoUnlockOpenActivitiyIDs(role);

	string sql = "insert into log_register values(0,"+to_string(service::id)+",'"+role->roleName+"',"+to_string(time(0))+","+to_string(role->dwRoleID)+",'"+client->ip+"')";
	logs_db_thread::singleton().push(sql);

	return 0;
}


//道具摘要信息
//#define S_GET_ITEMS_BRIEF	0x0020
//	C:[BYTE]
//		[PkgType]
//		PkgType:包裹类型, 1:装备包裹; 2:普通包裹; 3:仓库包裹
//	S:[BYTE, WORD, BYTE, [ItemBreifInfo]]
//		[grids, nextCost, num, [itemInfo]]
//		grids:已经开启的格子数
//		nextCost:下次开启新格子消耗元宝
//		num:道具数量
//		itemInfo:道具数据

//道具详细信息
//#define S_GET_ITEM_DETAIL	0x0021
//	C:[DWORD]
//		[itemId]
//		itemId:道具唯一ID
//	S:[DWORD, [BYTE, WORD]]
//		[itemId, [attrType, attrVal]]
//		itemId:道具ID
//		attrType:属性类型
//		attrVal:属性值


#define CASE_CMD(cmd, func)	\
		case cmd:\
		return func( role, p->data, len-HEAD_SIZE )


int PROTOCAL::execPacket(session *client, unsigned char * pkt, size_t len)
{

	PKT_DATA *p = (PKT_DATA*) pkt;

	//if ( p->cmd == S_ACTIVE )
	//{
	//	return clientActive( client, p->data, len-HEAD_SIZE );
	//}

	if ( p->cmd == S_CREATE_ROLE )
	{
		return roleCreate( client, p->data, len-HEAD_SIZE );
	}

	if (p->cmd == S_CHECK_ROLE)
	{
		return roleCheckRole( client, p->data, len-HEAD_SIZE );
	}

	if ( p->cmd == S_LOGIN )
	{
		return roleLogin( client, p->data, len-HEAD_SIZE );
	}

	RETURN_COND( client->role_id == 0, -1);

	ROLE* role = RoleMgr::getMe().getRoleByID( client->role_id );

	RETURN_COND( NULL == role, -1);

	BASE::writeLastPktFile( role->dwRoleID, (char*)pkt, len);

	//PUTS( "connection  %u, cmd:%x\n", role->dwRoleID, p->cmd );

	switch ( p->cmd )
	{

	case S_GET_ITEMS_BRIEF:
		return ItemCtrl::clientGetItemBrief( role, p->data, len-HEAD_SIZE );

		CASE_CMD(S_GET_ITEM_DETAIL, ItemCtrl::clientGetItemDetail );

	case S_GAME_CHAT:
		return clientChat( role, p->data, len-HEAD_SIZE );

	case S_NEWBIE_GUIDE:
		return role->setNewbieStep( role, p->data, len-HEAD_SIZE );

	case S_EQUIP_AWARD_FETCH:
		return role->getLevelAward( role, p->data, len-HEAD_SIZE );

	case S_FETCH_RECHARGE_AWARD:
		return role->getRechargeAward( role, p->data, len-HEAD_SIZE);

	case S_TODAY_GUIDE:
		return role->getTodayGuide( role, p->data, len-HEAD_SIZE);

	case S_TIME_GUIDE:
		return role->getTimeGuide( role, p->data, len-HEAD_SIZE);

	case S_MODIFY_BIND_EMAIL:
		return role->setEmailAddress( role, p->data, len-HEAD_SIZE);

	case S_REQUEST_BIND_EMAIL:
		return role->getEmailAddress( role, p->data, len-HEAD_SIZE);

	case S_EXCHANGE_AWARDS_BY_KEY:
		return role->getCodeAward( role, p->data, len-HEAD_SIZE);
		
	case S_EQUIP_ATTACH:
		return ItemCtrl::clientEquipAttach( role, p->data, len-HEAD_SIZE );

	case S_SELL_ITEMS:
		return ItemCtrl::clientSellItems( role, p->data, len-HEAD_SIZE );

	case S_LOOK_OTHER_ITEM:
		return ItemCtrl::clientLookOthersItem( role, p->data, len-HEAD_SIZE );

	case S_LOOK_OTHER:
		return ItemCtrl::clientLookOther( role, p->data, len-HEAD_SIZE );

	CASE_CMD(S_REQUEST_EMAILS, MailCtrl::clientRequestMails );

	CASE_CMD(S_DELETE_EMAIL, MailCtrl::clientDelMail );

	case S_GUILD_LIST:
		return FactionCtrl::clientGetGuildRecommendList( role, p->data, len-HEAD_SIZE );

	case S_GET_GUILD_RANK_LIST:
		return FactionCtrl::clientGetGuildRankList( role, p->data, len-HEAD_SIZE );

	case S_GET_GUILD_req:
		return FactionCtrl::clientGetGuildReq( role, p->data, len-HEAD_SIZE );

	case S_GET_GUILD_find:
		return FactionCtrl::clientGuildFind( role, p->data, len-HEAD_SIZE );

	CASE_CMD(S_GUILD_add, FactionCtrl::clientGuildReqJoin );

	CASE_CMD(S_GUILD_create, FactionCtrl::clientGuildCreate );

	CASE_CMD(S_GUILD_INFO, FactionCtrl::clientGuildGetInfo );

	CASE_CMD(S_GUILD_create_content, FactionCtrl::clientGuildCreateContent );

	CASE_CMD(S_GUILD_INFO_MEMLIST, FactionCtrl::clientGuildGetMemList );

	CASE_CMD(S_GUILD_INFO_MEM_EQUIP, FactionCtrl::clientGuildGetMemEquip );

	CASE_CMD(S_GUILD_INFO_MEM_FRIEND, FactionCtrl::clientGuildAddFriend );

	CASE_CMD(S_GUILD_INFO_MEM_JOB, FactionCtrl::clientGuildAppointJob );

	CASE_CMD(S_GUILD_INFO_MEM_QUIT, FactionCtrl::clientGuildKickMember );

	CASE_CMD(S_GUILD_INFO_QUIT, FactionCtrl::clientGuildQuit );

	CASE_CMD(S_GUILD_INFO_REQ_LIST, FactionCtrl::clientGuildGetReqList );

	CASE_CMD(S_GUILD_INFO_REQ_YES, FactionCtrl::clientGuildReqListOp );

	CASE_CMD(S_GUILD_INFO_REQ_ALL, FactionCtrl::clientGuildReqAcceptALL );

	CASE_CMD(S_GUILD_INFO_SAY_LIST, FactionCtrl::clientGuildGetSayList );

	CASE_CMD(S_GUILD_INFO_TEACH_LIST, FactionCtrl::clientGuildGetTechList );

	CASE_CMD(S_GUILD_INFO_TEACH_UP_LIST, FactionCtrl::clientGuildDonateEquip);

	CASE_CMD(S_GUILD_INFO_TEACH_UP_GOLD, FactionCtrl::clientGuildDonateCoin);

	CASE_CMD(S_FIGHT_AWARDS, ItemCtrl::clientGetFightPredictAward);

		//进地图
	case S_ENTER_MAP:
		return FightCtrl::clientEnterMap( role, p->data, len-HEAD_SIZE );
		//战斗
	case S_FIGHT_PROCESS:
		return FightCtrl::clientFightRequest( role, p->data, len-HEAD_SIZE );

	case S_FIGHT_REQUEST:
		return ItemCtrl::clientFightRequest( role, p->data, len-HEAD_SIZE );

	case S_BUY_FIGHT:
		return ItemCtrl::clientBuyFight( role, p->data, len-HEAD_SIZE );

	case S_BUY_FIGHT_COST:
		return ItemCtrl::clientBuyFightCost( role, p->data, len-HEAD_SIZE );

	case S_GET_FIGHT_TIMES:
		return ItemCtrl::clientGetFightTimes( role, p->data, len-HEAD_SIZE );

		//战斗结束
	case S_FIGHT_END_REPORT:
		return FightCtrl::clientFightEnd( role, p->data, len-HEAD_SIZE );

	case S_REQUEST_ELITE_INFO:
		return QUICKFIGHT::clientEliteInfo( role, p->data, len-HEAD_SIZE );

	case S_REQUEST_SELECTED_ELITE_INFO:
		return QUICKFIGHT::clientSelectElitePass( role, p->data, len-HEAD_SIZE );

	case S_BUY_CHALLENGE_TIME:
		return QUICKFIGHT::clientBuyEliteTimes( role, p->data, len-HEAD_SIZE );

	case S_ELITE_CHALLENGE:
		return QUICKFIGHT::clientRequestFightElite( role, p->data, len-HEAD_SIZE );

	case S_QUICK_CHALLENGE:
		return QUICKFIGHT::clientQuickFightElite( role, p->data, len-HEAD_SIZE );

	CASE_CMD(S_DROPSET, ItemCtrl::clientGetAutoSellOption);

	CASE_CMD(S_REQUEST_DROPSET, ItemCtrl::clientSetAutoSellOption);

	CASE_CMD( S_GET_SKILLS_INDEXS, SKILL::clientGetSkillsIndexs );

	CASE_CMD( S_SKILL_MOVE_UP, SKILL::clientSkillMoveUp );

	CASE_CMD( S_GET_UNLOCK_SKILLS_LIST, SKILL::clientGetUnlockSkillsList );

	CASE_CMD( S_SAVE_SKILLS_INDEXS, SKILL::clientSaveSkillsIndexs );

	CASE_CMD( S_UP_SKILL_LEVEL, SKILL::clientUpSkillLevel );

	case S_GET_FRIENDS:
	case S_ADD_FRIEND:
	case S_DEL_FRIEND:
	case S_SEARCH_FRIEND:
	case S_RECOMMOND_FRIENDS:
	case S_FRIEND_INFO:
	case S_DIANZAN:
		return FRIEND::dealFriends(role, p->data, len-HEAD_SIZE , p->cmd);

	case S_GET_ROLE_RANK_LEVEL:
		return RANK::getRankByLevel(role, p->data, len-HEAD_SIZE);

	case S_GET_ROLE_RANK_FIGHTVALUE:
		return RANK::getRankByFightvalue(role, p->data, len-HEAD_SIZE);

	case S_GET_ROLE_WEAPON:
		return RANK::getRankByWeaponscore(role, p->data, len-HEAD_SIZE);

	case S_GET_ROLE_RANK_MATCH:
		return RANK::getRankByMatch(role, p->data, len-HEAD_SIZE);

	case S_GET_ARENA_RECOMMEND:
		return ARENA::getArenaRecommend(role, p->data, len-HEAD_SIZE);

	case S_ARENA_BUY_COUNT:
		return ARENA::buyArenaFightCount(role, p->data, len-HEAD_SIZE);

	case S_ARENA_BUY_GOLD:
		return ARENA::buyArenaCountGold(role, p->data, len-HEAD_SIZE);

	case S_GET_ARENA_RECORD:
		return ARENA::requireArenaRecord(role, p->data, len-HEAD_SIZE);

	case S_ARENA_FIGHT_PRE:
		return ARENA::arenaFightPre(role, p->data, len-HEAD_SIZE);

	case S_OUYU_FIGHT_PRE:
		return ARENA::meetMapFightPre(role, p->data, len-HEAD_SIZE);
		
	case S_GET_SIGN_INFO:
		return SIGN::getSignInfo(role, p->data, len-HEAD_SIZE);

	case S_CLIENT_SIGN:
		return SIGN::clientSign( role, p->data, len-HEAD_SIZE );

	case S_GET_ACTIVITY:
		return ACTIVITY::clientGetActivities( role, p->data, len-HEAD_SIZE );

	case S_BOSS_TIME_LEFT:
		return ACTIVITY::getActtivityLeftTime( role, p->data, len-HEAD_SIZE );

	case S_GET_GOODS_LIST:
		return role->cMarMgr.cMarJew.ClientGetCoodsList( role, p->data, len-HEAD_SIZE );
	case S_REQUEST_BUY:
		return role->cMarMgr.cMarJew.ClientBuyCoods( role, p->data, len-HEAD_SIZE );
	case S_REQUEST_BUY_ALL:
		return role->cMarMgr.cMarJew.clientBuyAllCood( role, p->data, len-HEAD_SIZE );
	case S_REQUEST_UPDATE_COST:
		return role->cMarMgr.cMarJew.clientRequestUpdateCost( role, p->data, len-HEAD_SIZE );
	case S_REQUEST_UPDATE:
		return role->cMarMgr.cMarJew.clientRequestUpdate( role, p->data, len-HEAD_SIZE );
	case S_USE_MONEYTREE:
		return role->cMarMgr.clientUseMoneyTree( role, p->data, len-HEAD_SIZE );
	case S_REQUEST_MONEYTREE:
		return role->cMarMgr.clientRequestMoneyTree( role, p->data, len-HEAD_SIZE ); 

	case S_REQUEST_MISTERY_SHOP:
		return role->cMarMgr.cMarMys.ClientGetCoodsList( role, p->data, len-HEAD_SIZE );
	case S_BUY_MISTERY_SHOP:
		return role->cMarMgr.cMarMys.ClientBuyCoods( role, p->data, len-HEAD_SIZE ); 

	case S_GET_TASK_LIST:
		return TASK::clientGetTasksList( role, p->data, len-HEAD_SIZE );
	case S_GET_TASK_GIFTS:
		return TASK::clientGetTaskReward( role, p->data, len-HEAD_SIZE );

	case S_OFFLINE_FIGHT_REPORT:
		return QUICKFIGHT::clientOfflineFightReport( role, p->data, len-HEAD_SIZE );
	case S_QUICK_FIGHT_REPORT:
		return QUICKFIGHT::clientQuickFightReport( role, p->data, len-HEAD_SIZE );
	case S_BUY_QUICK_FIGHT:
		return QUICKFIGHT::clientBuyQuickFight( role, p->data, len-HEAD_SIZE );
	case S_GET_QUICK_FIGHT_COST:
		return QUICKFIGHT::clientBuyQuickFightCost( role, p->data, len-HEAD_SIZE );
	case S_GET_QUICK_FIGHT_TIMES:
		return QUICKFIGHT::clientGetQuickFightTimes( role, p->data, len-HEAD_SIZE );
	case S_REQUES_MERCENARY_INFO:
		return NETMERCENARY::clientGetMerInfo( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_UP_DOWN:
		return NETMERCENARY::clientMerUpDown( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_SKILL_LIST:
		return NETMERCENARY::clientGetSkills( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_REFRESH_SKILLS:
		return NETMERCENARY::clientRefreshSkills( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_TRAIN_PREVIEW:
		return NETMERCENARY::clientPreTrainMercenary( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_TRAIN_SAVE:
		return NETMERCENARY::clientTrainMercenary( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_STAR_PRO_PREVIEW:
		return NETMERCENARY::clientPreViewMerStar( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_UPGRADE_STAR:
		return NETMERCENARY::clientConfirmMerStar( role, p->data, len-HEAD_SIZE );
	case S_REQUEST_COMBO_BOOKS_INFO:
		return NETMERCENARY::clientPreViewComposeBook( role, p->data, len-HEAD_SIZE );
	case S_COMBO_BOOK:
		return NETMERCENARY::clientConfirmComposeBook( role, p->data, len-HEAD_SIZE );
	case S_REQUEST_HALO_LIST:
		return NETMERCENARY::clientGetMerHaloList( role, p->data, len-HEAD_SIZE );
	case S_ACTIVE_HALO:
		return NETMERCENARY::clientActiveMerHalo( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_EQUIP_ATTACH:
		return NETMERCENARY::clientMerEquipAttach( role, p->data, len-HEAD_SIZE );
	case S_SLAVE_BASR_ATTR:
		return NETMERCENARY::clientGetTrainMercenaryBaseAttr( role, p->data, len-HEAD_SIZE );
		

	CASE_CMD(S_PACKAGE_EXTEND, ItemCtrl::clientPackageExtend);

	CASE_CMD(S_ITEM_USE, ItemCtrl::clientProcsUse);

	CASE_CMD(S_STRENGTHEN_PREVIEW, ItemCtrl::clientPreViewStrengthenInfo);//  强化

	CASE_CMD(S_CONFIRM_STRENGTHEN, ItemCtrl::clientConfirmStrengthenInfo);

	CASE_CMD(S_MAKE_HOLE, ItemCtrl::clientPunchHole);// 打孔镶嵌

	CASE_CMD(S_INJECT_DIAMOND_AT, ItemCtrl::clientInjectDiamond);//镶嵌宝石

	CASE_CMD(S_UNINJECT_DIAMOND_AT, ItemCtrl::clientUninjectDiamond);//卸载宝石

	CASE_CMD(S_UNINJECT_ALL_DIAMOND, ItemCtrl::clientUninjectAllDiamonds);

	CASE_CMD(S_EQUIP_RESET, ItemCtrl::clientEquipReset);

	CASE_CMD(S_SMELT_EQUIP_NEW, ItemCtrl::clientSmeltEquip);//熔炼

	CASE_CMD(S_REQUEST_FORGE_SUIT_NUM, ItemCtrl::clientForgeSuitNum);// 打造套装剩余次数

	CASE_CMD(S_FORGE_ITEM, ItemCtrl::clientForgeEquip);//打造装备

	CASE_CMD(S_EXCHANGE_EQUIP_NEW, ItemCtrl::clientExchangeEquip);//兑换装备

	case S_GET_FUWEN_STATUS:// 符文系统
		return role->role_rune.clientGetRuneStatus(p->data, len-HEAD_SIZE );
	case S_REQ_DIVINE:
		return role->role_rune.clientReqDivine(p->data, len-HEAD_SIZE );
	case S_PUT_ON_FUWEN:
		return role->role_rune.clientPutOnRune(p->data, len-HEAD_SIZE );
	case S_FUWEN_LEVELUP:
		return role->role_rune.clientRuneLevelUp(p->data, len-HEAD_SIZE );

	case S_GET_WORLDBOSS_STATUS:// 世界BOSS
		return role->role_worldboss.clientGetWorldBossStatus(p->data, len-HEAD_SIZE );
	case S_SIGNUP_WORLDBOSS:
		return role->role_worldboss.clientSignUpWorldBoss(p->data, len-HEAD_SIZE );
	case S_ATTACK_WORLDBOSS:
		return role->role_worldboss.clientAttackWorldBoss(p->data, len-HEAD_SIZE );
	case S_EMBRAVE_WORLDBOSS:
		return role->role_worldboss.clientEmbraveWorldBoss(p->data, len-HEAD_SIZE );
	case S_GET_WORLDBOSS_RANK:
		return role->role_worldboss.clientGetWorldBossRank(p->data, len-HEAD_SIZE );
	case S_GET_WORLDBOSS_COOLTIME:
		return role->role_worldboss.clientGetWorldBossCoolTime(p->data, len-HEAD_SIZE );

	case S_GET_OPEN_REWARD_LIST:
		return TASK::clientGetOpenActivitiyIDs( role, p->data, len-HEAD_SIZE );
	case S_GET_OPEN_REWARD_GIFTS:
		return TASK::clientGetOpenActivitiyIDsReward( role, p->data, len-HEAD_SIZE );
	case S_GET_OPEN_ACTIVITY_INFO:
		return TASK::clientGetOpenActivitiyInfo( role, p->data, len-HEAD_SIZE );
		
	case S_MEET_BUY_PK_TIMES:
		{
			auto role = RoleMgr::getMe().getRoleByID(client->role_id);
			output_stream out(S_MEET_BUY_PK_TIMES);
			if(role == nullptr)
			{
				out.write_byte(1);
			}
			else
			{
				unsigned price;
				if(role->pk_buy_count < 5)
					price = (role->pk_buy_count + 1) * 10;
				else
					price = 50;

				if(role->dwIngot < price)
				{
					out.write_byte(2);
				}
				else
				{
					role->dwIngot -= price;
					role->pk_buy_count++;
					role->pk_count++;
					out.write_byte(0);
					price += 10;
					if(price > 50)price = 50;
					out.write_uint(price);
				}
			}
			bufferevent_write(client->client_bev, out.get_buffer(), out.length());
		}
		break;
	case S_MEET_PLAYER:
		{
			auto role = RoleMgr::getMe().getRoleByID(client->role_id);
			output_stream out(S_MEET_PLAYER);
			if(role == nullptr)
			{
				out.write_bool(false);
			}
			else
			{
				role->update_meet_pk_count();
				out.write_bool(true);
				out.write_uint(role->pkvalue);
				out.write_byte(role->pk_count);
				unsigned price;
				if(role->pk_buy_count < 5)
					price = (role->pk_buy_count + 1) * 10;
				else
					price = 50;
				out.write_uint(price);
				out.write_ushort((uint16_t)((time(0) - role->last_pk_count_inc_time)  % (20 * 60)));
				service::get_map_rand_players(out, role);
			}
			bufferevent_write(client->client_bev, out.get_buffer(), out.length());
		}
		break;
	case S_MEET_PK_RECORD:
		{
			auto role = RoleMgr::getMe().getRoleByID(client->role_id);
			output_stream out(S_MEET_PK_RECORD);
			out.write_uint(time(0));
			out.write_uint(role->pk_dead_count);
			if(role == nullptr)
			{
				out.write_byte(0);
			}
			else
			{
				out.write_byte((byte)role->meet_pk_records.size());
				for(auto& item : role->meet_pk_records)
					out.write_data((char*)&item, sizeof(item));
			}
			bufferevent_write(client->client_bev, out.get_buffer(), out.length());
		}
		break;
	case S_MEET_PK_RANK:
		{
			output_stream out(S_MEET_PK_RANK);
			out.write_byte(service::meet_pk_rank.size());
			for(auto& item : service::meet_pk_rank)
					out.write_data((char*)&item, sizeof(item));
			bufferevent_write(client->client_bev, out.get_buffer(), out.length());
		}
		break;

	default:
		;//X2(pkt, len);
	}

	return 0;
}


void PROTOCAL::timerLongTimeNoLogin(int fd, short what, void *arg)
{
	//return;

	session *client = (session *)arg;

	PUTS("");
	onDisconnect(client);
}


//struct event ev_timer;

int PROTOCAL::onAccept(session *client, int succ)
{
//	struct timeval tv = { 90, 0};
//
//#if 0
//	struct event *ev_timer = (struct event *)malloc(sizeof(struct event));
//
//	evtimer_set(ev_timer, timerLongTimeNoLogin, client );
//	event_base_set(client->cb->ebase, ev_timer);
//	evtimer_add(ev_timer, &tv);
//#else
//	evtimer_set(&client->timer, timerLongTimeNoLogin, client );
//	event_base_set(service::event_handler, &client->timer);
//	evtimer_add(&client->timer, &tv);
//
//#endif

	//PUTS( "Accepted connection from %s, bev:%u\n", client->ip.c_str(), (DWORD)client->client_bev );

	//string sql = "insert into log_connect values( 0, '" + client->ip + "'," + to_string( time(NULL)) + ")";
	//logs_db_thread::singleton().push(sql);

	return 0;

}

int PROTOCAL::onDisconnect(session *client)
{
	RETURN_COND( client == nullptr, 0);


	string strData;
	S_APPEND_WORD( strData, 0 );
	sendClient( client, cmdPacket( S_BREAK, strData ) );



	PUTS( "bev:%u\n", (DWORD)client->client_bev );
	if (client->client_bev)
		bufferevent_free(client->client_bev);

	if ( client->role_id )
	{
		ROLE* role = RoleMgr::getMe().getMe().getRoleByID(client->role_id);
		role->client = NULL;
		role->tLogoutTime = PROTOCAL::getCurTime();
		role->roleSave2DB();
		_mapOnlineRoleClient.erase(client->role_id);
	}

	PUTS("free client:%x, roleid:%u, IP:%s", (DWORD)client, client->role_id, client->ip.c_str() );

	//wmevtimer_del(&client->timer);

	delete client;
	return 0;
}

session* PROTOCAL::getOnlineClient(DWORD dwRoleID)
{
	auto it = _mapOnlineRoleClient.find(dwRoleID);
	if ( it ==  _mapOnlineRoleClient.end() )
		return NULL;

	return it->second.client;

}

ROLE* PROTOCAL::getOnlineRole(DWORD dwRoleID)
{
	auto it = _mapOnlineRoleClient.find(dwRoleID);
	if ( it ==  _mapOnlineRoleClient.end() )
		return NULL;

	return it->second.role;

}

ROLE_CLIENT* PROTOCAL::getOnlineRoleClient(DWORD dwRoleID)
{
	auto it = _mapOnlineRoleClient.find(dwRoleID);
	if ( it ==  _mapOnlineRoleClient.end() )
		return NULL;

	return &it->second;

}

void PROTOCAL::forEachOnlineRole( std::function<void ( const std::pair<DWORD, ROLE_CLIENT> &x )> func )
{
	for_each( _mapOnlineRoleClient.begin(), _mapOnlineRoleClient.end(), func);
}

///*
//@查找修炼完成技能
//*/
//void PROTOCAL::selectSkillUpOk( )
//{
//	for (auto OnlineClient : PROTOCAL::_mapOnlineRoleClient)
//	{
//		if (OnlineClient.second.role->wOnUpSkill != 0)
//		{
//			SKILL::timeUpdateSkills(OnlineClient.second.role);
//		}	
//	}
//}

/*
@找在线玩家,推荐好友用
*/
void PROTOCAL::getOnlineID(ROLE* role, vector<DWORD>& vecIDs, WORD num)
{
	DWORD nums = PROTOCAL::_mapOnlineRoleClient.size();
	int n = rand() % nums;
	while (vecIDs.size() < num)
	{ 	
		vecIDs.clear();
		auto it = PROTOCAL::_mapOnlineRoleClient.begin();
		std::advance( it, n );

		for ( ; it != PROTOCAL::_mapOnlineRoleClient.end(); ++it )
		{
			if ( it->first == role->dwRoleID )
				continue;

			ROLE* roleFriend = (it->second).role;
			if ( roleFriend->roleName.empty())
				continue;
			vector<DWORD>::iterator itdword = find(role->vecFriends.begin(), role->vecFriends.end(), it->first);
			if (itdword != role->vecFriends.end())
			{
				continue;
			}
			vecIDs.push_back(it->first);
			if (vecIDs.size() >=  num)
			{
				return;
			}
		}
		n -= 30 + 6 +1;
		if (n < 0)
		{
			break;
		}
	}
}





//static CLIENT_CALLBACK cb =
//{
//	event_base_new(),//	NULL,
//	PROTOCAL::execPacket , PROTOCAL::onAccept, PROTOCAL::onDisconnect
//};


static void onSignalTerm(evutil_socket_t fd, short event, void *arg)
{
	//return;//不能存数据 可能产生未知错误

	event_base_loopbreak(service::event_handler);

	saveAllToDb(true);
	//time_t tNow = PROTOCAL::getCurTime();

	//roleSave( tNow, true );
	//JSON::writeGsTime();
	//RANK::saveRank();
	//service::save_data();
	//WorldBoss::getMe().save_to_db();

	FactionCtrl::saveApplyList();

}

/*
struct TIMER
{
struct event* timeout;
struct timeval tv;

void* data;
};


static void timer1(int fd, short event, void* arg)
{
TIMER* timer = (TIMER*)arg;
evtimer_add(timer->timeout, &timer->tv);

}

*/
//========================================================================
class timer;

typedef void* (*tm_handler_t)(void* arg);

typedef map<unsigned int, timer*> map_timer_t;

map_timer_t g_maptimers;

#define SET_TIMER(id, elapse, func, flag)	\
	do{													\
	static timer aTimer((id), (elapse), func, (flag));	\
	g_maptimers[(id)] = &aTimer;						\
	aTimer.set_timer();								\
	}while(0)

enum 
{
	E_TIMER_1_SECOND,
	E_TIMER_24_SECOND,
	E_TIMER_60_SECOND,
	E_TIMER_300_SECOND,

	E_TIMER_1800_SECOND,
	E_TIMER_3600_SECOND,

};

class timer
{
private:
	int tm_flag;		// timeout flag, 0 is one-shot, 1 is forever
	struct event* tm_ev; // timeout event
	struct timeval tm_tv;  // timeout value used to re-add this event
	unsigned int tm_id;	// timeout id, used to call handler
	tm_handler_t tm_handler; // timeout handler when timeout

public:
	// constructor
	timer(unsigned int tmid, unsigned int tmelapse/* in milliseconds */, tm_handler_t tmfunc, int tmflag = 1);
	~timer();

	// add this event to libevent
	bool set_timer();
	void del_timer();
	void* call_handler();

private:

	timer(const timer& tm);
	timer& operator=(const timer& tm);

	static void timeout_cb(int fd, short event_cmd, void *arg);

};


void timer::timeout_cb(int fd, short event_cmd, void *arg)
{
	timer* ptimer = (timer*)arg;
	if( ptimer )
	{
		ptimer->call_handler();
	}
}

//void* timer30(void* arg)
//{
//
//	//MineCtrl::doMinining( );
//
//	//return arg;
//
//}
void* timer60(void* arg)
{
	if ( JSON::isNewDay() )
	{
		ARENA::offerArenaReward();

		JSON::setLastAccess( PROTOCAL::getCurTime() );
	}
	RoleMgr::getMe().DailyCleanAll( );

	auto num = PROTOCAL::_mapOnlineRoleClient.size();
	string sql = "insert into log_online_count values(0,'"+to_string(service::id)+"','"+to_string(time(0))+"','"+to_string(num)+"')";//wmsql
	logs_db_thread::singleton().push(sql);
	return arg;
}

void* timerSave(void* arg)
{
	saveAllToDb(false);
	//time_t tNow = PROTOCAL::getCurTime();
	//roleSave( tNow, false );
	//JSON::writeGsTime();
	//RANK::saveRank();
	//service::save_data();
	//WorldBoss::getMe().save_to_db();
	return arg;
}

void* timerSaveFaction(void* arg)
{
	FactionCtrl::saveApplyList();
	return arg;
}

void* timer3600(void* arg)
{
	RoleMgr::getMe().timerClearRoleCache( PROTOCAL::getCurTime() );
	return arg;
}

void* timer1800(void* arg)
{
	//RoleMgr::getMe().timerClearRoleCache( PROTOCAL::getCurTime() );
	RoleMgr::getMe().addOneDivineTime();
	return arg;
}

void* timer1(void* arg)
{
	PROTOCAL::_curTime = time(NULL);
	//ACTIVITY::startTodaySec(PROTOCAL::_curTime);
	TimerMgr::getMe().Update();
	//SHOP::startTodaySec(PROTOCAL::_curTime);
	WorldBoss::getMe().updateTime(PROTOCAL::_curTime);
	return arg;
}

time_t PROTOCAL::getCurTime()
{
	return _curTime;
}

void PROTOCAL::refreshTime()
{
	_curTime = time(NULL);
}

timer::timer(unsigned int tmid, unsigned int tmelapse, tm_handler_t tmfunc, int tmflag /* = 1 */)
{
	tm_id = tmid;
	tm_flag = tmflag;

	tm_ev = (struct event*)malloc(sizeof(struct event));

	long lsec = (long)tmelapse / 1000l;
	long lms  = (long)tmelapse % 1000l;
	evutil_timerclear(&tm_tv);
	tm_tv.tv_sec = lsec;
	tm_tv.tv_usec = lms * 1000l;

	tm_ev  = evtimer_new(service::event_handler, timeout_cb, this );
	evtimer_add(tm_ev, &tm_tv);

	tm_handler = tmfunc;
}

timer::~timer()
{
	if( tm_ev )
	{
		free( tm_ev );
	}
}

// let this event awarded by libevent, and to be dispatched
bool timer::set_timer()
{
	assert( tm_ev != NULL );
	if( tm_ev == NULL )
		return false;

	int ret = event_add(tm_ev, &tm_tv);
	return ret == 0;
}

// prevent this event from dispatched
void timer::del_timer()
{
	tm_flag = 0;
	if( tm_ev )
	{
		event_del( tm_ev );
	}
}

void* timer::call_handler()
{
	void* ret = NULL;
	if( tm_handler )
		ret = (tm_handler)( NULL );

	if( tm_flag == 1 )
	{
		// re-schedule this timer
		set_timer();
	}

	return ret;
}

//========================================================================

int PROTOCAL::netInit()
{

	_curTime = time(NULL);

	//#define SET_TIMER(id, elapse, func, flag)	\
	//	do{													\
	//	static timer aTimer((id), (elapse), func, (flag));	\
	//	g_maptimers[(id)] = &aTimer;						\
	//	aTimer.set_timer();								\
	//	}while(0)
	SET_TIMER(E_TIMER_300_SECOND, 60*1000, timer60, 1);
	SET_TIMER(E_TIMER_60_SECOND, 5*61*1000, timerSave, 1);
	SET_TIMER(E_TIMER_1_SECOND, 1*1000, timer1, 1);
	//SET_TIMER(E_TIMER_24_SECOND, 30*1000, timer30, 1);

	SET_TIMER(E_TIMER_3600_SECOND, 3602*1000, timerSaveFaction, 1);
	SET_TIMER(E_TIMER_3600_SECOND, 3600*1000, timer3600, 1);
	SET_TIMER(E_TIMER_1800_SECOND, 1800*1000, timer1800, 1);

	struct event ev;

#ifdef _WIN32

	//SIGINT
	evsignal_set(&ev, 2, onSignalTerm, &ev);
#else

	//SIGTERM
	evsignal_set(&ev, 15, onSignalTerm, &ev);

#endif
	event_base_set(service::event_handler, &ev);

	evsignal_add(&ev, NULL);

	event_base_dispatch(service::event_handler);

	return 0;
}

size_t PROTOCAL::get_online_count()
{
	return _mapOnlineRoleClient.size();
}


