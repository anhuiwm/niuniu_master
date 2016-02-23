#include "pch.h"

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
#include "team.h"

using namespace BASE;


bool isRoleNameValid(const string & roleName)
{
	//return !CONFIG::isInnerSieldWord(roleName);
	return true;
}

void PROTOCAL::sendClient(session* client, const string& str)
{
	if (NULL == client)
	{
		return;
	}

	client->send(str.data(), str.size() );
}


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


void PROTOCAL::saveAllToDb(bool bForce)
{
	RANK::saveRank();
	service::save_data( bForce );
	WorldBoss::getMe().save_to_db();
	guild::save_guilds();
}

#define loginSendChat(role)		\
	do {\
		for ( auto it : service::chats )	sendClient( role->client, it );\
		}while(0)


int PROTOCAL::set_phone_imei(ROLE* role, unsigned char * data, size_t dataLen)
{
	string str_imei;
	RETURN_COND(!BASE::parseBSTR(data, dataLen, str_imei), 0);
	role->phone_imei = str_imei;
	if (!role->phone_imei.empty())
	{
		RoleMgr::getMe().insert_imei_uuids(role);
	}
	return 0;
}

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
#ifdef WIN32
	auto tick = GetTickCount();
#endif
	string uuid;
	if ( !BASE::parseBSTR( data, dataLen, uuid) )
		return 0;

	logINFO("role login : %s(uuid), %u(datalen)", uuid.c_str(), dataLen);
	//wmevtimer_del(&client->timer);


	//某条连接重复登陆
	if ( client->role_id )
	{
		SEND4( client, S_LOGIN, 3, "");
		return 0;
	}

	ROLE* role = RoleMgr::getMe().findRoleByUUID(uuid);
	if ( role == nullptr)
	{
		SEND4( client, S_LOGIN, 1, "");
		return 0;
	}
	//logonline(" login: uuid=%s",uuid.c_str());
	// 封停账号处理
	if(role->close_user_valid_time != 0 && role->close_user_valid_time > time(0))
	{
		service::close_session(client);
		return 0;
	}

	//已登陆玩家开另一个客户端登陆
	service::attach_role_and_session(role, client);

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

	RoleMgr::getMe().timerDailyClean(role, time(nullptr) );

	role->roleZone = RoleMgr::getMe().loadZoneInfoFromDB( role );

	role->online();

	loginSendChat(role);

	ITEM_INFO para1(0, role->getVipLevel());
	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_VIP_LOGIN, &para1);

	string sql = "insert into log_login values(0," + to_string(service::id) + ",'" +
		role->roleName+"', "+to_string(time(0))+", "+to_string(role->dwRoleID)+
		", "+to_string(role->wLevel)+", "+to_string(role->dwExp)+", "+
		to_string(role->dwCoin)+", "+to_string(role->dwIngot)+", "+
		to_string(role->dwCharge)+",'"+client->ip+"')";
	logs_db_thread::singleton().push(sql);

	if (CONFIG::isGmName(role->roleName))
	{
		role->gm_account = true;
	}

	{
		// 发送系统信息
		output_stream out(S_SYSTEM_INFO);
		out.write_int(time(nullptr));
		role->send(out);

		role->send_activitys_info();
	}

#ifdef WIN32
	cout << "load user info use time:" << GetTickCount() - tick << "ms" << endl;
#endif
	return 0;
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


bool PROTOCAL::phpChargeRole( DWORD roleid, DWORD dwChargeNum, string charge_account )
{
	BYTE byRet = 0;

	ROLE* role = RoleMgr::getMe().getRoleByID(roleid);
	
	RETURN_COND( role == NULL, false);

	RETURN_COND( role->charge_account == charge_account, false);

	int dwRealNum = dwChargeNum;

	if (CONFIG::getChargeCfg(dwRealNum) == 0)
	{
		//不在充值档里面
	}
	else if ( std::find(role->vecFirstCharge.begin(),role->vecFirstCharge.end(),dwChargeNum) == role->vecFirstCharge.end() )
	{
		role->vecFirstCharge.push_back( dwChargeNum );
		dwChargeNum *= 3;//first charge 3 mutiple
	}

	byRet = role->roleGetCharge( (int)dwChargeNum, (int)dwRealNum );

	string strClientData;
	S_APPEND_BYTE( strClientData, byRet );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_REQUEST_BUY_INGOT, strClientData ) );

	RETURN_COND(byRet != E_SUCC, false);

	if (role->byRechargeState == E_TASK_UNCOMPLETE)
	{
		role->byRechargeState = E_TASK_COMPLETE_CAN_GET_PRIZE;
	}

	RoleMgr::getMe().judgeCompleteTypeOpenActivity(role, E_ACTIVITY_FIRST_CHARGE);

	role->send_firstcharge_list();
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

int PROTOCAL::chargeRole( DWORD dwChargeNum, ROLE* role, bool bGm )
{
	RETURN_COND(role == NULL, 0);

	DWORD dwRealNum = dwChargeNum;
	if ( CONFIG::getChargeCfg(dwRealNum) == 0 )
	{
		//不在充值档里面
	}
	else if ( std::find(role->vecFirstCharge.begin(),role->vecFirstCharge.end(),dwChargeNum) == role->vecFirstCharge.end() )
	{
		role->vecFirstCharge.push_back( dwChargeNum );
		dwChargeNum *= 3;//first charge 3 mutiple
	}

	BYTE byRet = role->roleGetCharge( int(dwChargeNum), int(dwRealNum), bGm );

	string strClientData;
	S_APPEND_BYTE( strClientData, byRet );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_REQUEST_BUY_INGOT, strClientData ) );

	RETURN_COND(byRet != E_SUCC, 0);

	RoleMgr::getMe().judgeCompleteTypeOpenActivity(role, E_ACTIVITY_FIRST_CHARGE);

	if ( role->byRechargeState == E_TASK_UNCOMPLETE )
	{
		role->byRechargeState = E_TASK_COMPLETE_CAN_GET_PRIZE;
	}

	role->send_firstcharge_list();

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

	//logonline("check: platform_id=%u   serial=%s  uuid=%s", platform_id, serial.c_str(), uuid.c_str() );
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

	//logonline("create: platform_id=%u   serial=%s  ", platform_id, serial.c_str());
	//某条连接重复登陆
	if ( client->role_id )
	{
		SEND4( client, S_CREATE_ROLE, 3, "" );
		return 0;
	}
	service::attach_role_and_session(role, client);
	service::player_enter_map(role, 0);


	PlayerInfo playerInfo(role);

	string strData;
	S_APPEND_WORD( strData, 0 );
	S_APPEND_BSTR( strData, role->uuid );
	S_APPEND_NBYTES( strData, (char*)&playerInfo, sizeof(PlayerInfo) );
	sendClient( role->client, cmdPacket(S_CREATE_ROLE, strData) );

	time_t now = time(nullptr);
	role->tLastAccess = now;
	role->setFirstLoginTime( now );
	role->roleZone = RoleMgr::getMe().loadZoneInfoFromDB( role );
	role->role_skill.autoUnlockSkills();
	TASK::autoUnlockTask(role);
	TASK::autoUnlockOpenActivitiyIDs(role);
	TASK::init_tencent_tasks(role);

	loginSendChat(role);
	string sql = "insert into log_register values(0,"+to_string(service::id)+",'"+role->roleName+"',"+to_string(time(0))+","+to_string(role->dwRoleID)+",'"+client->ip+"')";
	logs_db_thread::singleton().push(sql);


	if (CONFIG::isGmName(role->roleName))
	{
		role->gm_account = true;
	}

	{
		// 发送系统信息
		output_stream out(S_SYSTEM_INFO);
		out.write_int(time(nullptr) - service::tzone);
		role->send(out);
	}

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

	if (p->cmd == S_CHECK_ROLE)
	{
		return roleCheckRole(client, p->data, len - HEAD_SIZE);
	}

	if ( p->cmd == S_CREATE_ROLE )
	{
		return roleCreate( client, p->data, len-HEAD_SIZE );
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
	auto data_ptr = p->data;
	auto data_len = len - HEAD_SIZE;

	switch ( p->cmd )
	{

	case  S_SERVER_NOTI_MID:
		return PROTOCAL::set_phone_imei(role, p->data, len - HEAD_SIZE);

	case S_GET_ITEMS_BRIEF:
		return ItemCtrl::clientGetItemBrief( role, p->data, len-HEAD_SIZE );

		CASE_CMD(S_GET_ITEM_DETAIL, ItemCtrl::clientGetItemDetail );

	case S_GAME_CHAT:
	{
		input_stream stream(data_ptr, data_len);
		role->chat(stream);
		break;
	}

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
		return guild::rpc_get_recommend_list(role, p->data, len - HEAD_SIZE);

	case S_GET_GUILD_RANK_LIST:
		return guild::rpc_get_rank_list(role, p->data, len - HEAD_SIZE);

	case S_GET_GUILD_find:
	{
		input_stream stream(data_ptr, data_len);
		role->guild_find(stream);
		break;
	}
	case S_GUILD_add:
	{
		input_stream stream(data_ptr, data_len);
		role->guild_join_guild(stream);
		break;
	}
	case S_GUILD_SIGNIN:
	{
		input_stream stream(data_ptr, data_len);
		role->guild_signin(stream);
		break;
	}
	case S_GUILD_SIGEXCHANGE:
	{
		input_stream stream(data_ptr, data_len);
		guild::rpc_exchange(role, stream);
		break;
	}

	CASE_CMD(S_GUILD_create, guild::rpc_create);
	CASE_CMD(S_GUILD_INFO, guild::rpc_getinfo);
	CASE_CMD(S_GUILD_create_content, guild::rpc_create_content);
	CASE_CMD(S_GUILD_INFO_MEMLIST, guild::rpc_get_member_list);
	CASE_CMD(S_GUILD_INFO_MEM_EQUIP, guild::rpc_get_mem_equip);
	CASE_CMD(S_GUILD_INFO_MEM_FRIEND, guild::rpc_add_friend);
	CASE_CMD(S_GUILD_INFO_MEM_JOB, guild::rpc_appoint_job);
	CASE_CMD(S_GUILD_INFO_MEM_QUIT, guild::rpc_kick_member);
	CASE_CMD(S_GUILD_INFO_QUIT, guild::rpc_quit);
	CASE_CMD(S_GUILD_INFO_SAY_LIST, guild::rpc_get_chatlist);

	case S_GUILD_CONTRIBUTE:
	{
		input_stream stream(data_ptr, data_len);
		role->guild_contribute(stream);
		break;
	}
	case S_GUILD_GET_CONTRIBUTE:
	{
		input_stream stream(data_ptr, data_len);
		role->guild_get_contribute_info(stream);
		break;
	}
	case S_GUILD_SET_CONDITION:
	{
		input_stream stream(data_ptr, data_len);
		role->guild_set_join_condition(stream);
		break;
	}
	case S_GUILD_WORSHIPINFO:
		return role->get_wordship_info(data_ptr, data_len);
	case S_GUILD_WORSHIP	:
		return role->guild_wordship(data_ptr, data_len);

	CASE_CMD(S_FIGHT_AWARDS, ItemCtrl::clientGetFightPredictAward);
	case S_FIGHT_TIP:
	{
		input_stream stream(data_ptr, data_len);
		role->battle_tip(stream);
		break;
	}
		//进地图
	case S_ENTER_MAP:
		return FightCtrl::clientEnterMap( role, p->data, len-HEAD_SIZE );
		//战斗过程
	case S_FIGHT_PROCESS:
	{
		input_stream stream(data_ptr, data_len);
		return FightCtrl::clientFightRequest(role, stream);
	}
		//BOSS挑战请求（次数）
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

	case S_QUICK_FIGHT_BOSS:
		return QUICKFIGHT::clientQuickFightBoss(role, p->data, len - HEAD_SIZE);

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
		return 0;// ARENA::arenaFightPre(role, p->data, len - HEAD_SIZE);

	case S_OUYU_FIGHT_PRE:
		return 0;// ARENA::meetMapFightPre(role, p->data, len - HEAD_SIZE);
		
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

	//case S_OFFLINE_FIGHT_REPORT:
	//	return QUICKFIGHT::clientOfflineFightReport(role, p->data, len - HEAD_SIZE);
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

	CASE_CMD(S_STRENGTHEN_PREVIEW, ItemCtrl::clientPreViewStrengthenInfo);//  强化预览

	CASE_CMD(S_CONFIRM_STRENGTHEN, ItemCtrl::clientConfirmStrengthenInfo);//强化确认

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
		
	//守卫剑阁组队战
	CASE_CMD(TEAM_DEFENCE_STATE, jiange_team::getJiangeState);
	CASE_CMD(REQUEST_TEAM_LIST, jiange_team::getJiangeTeams);
	CASE_CMD(REQUEST_CREATE_TEAM, jiange_team::createJiangeTeam);
	CASE_CMD(REQUEST_APPLY_TEAM, jiange_team::joinJiangeTeam);
	CASE_CMD(REQUEST_MY_TEAM_INFO, jiange_team::getMyJiangeTeam);
	CASE_CMD(REQUEST_LEAVE_MY_TEAM, jiange_team::leaveJiangeTeam);
	CASE_CMD(KICK_OFF_TEAM_MATE, jiange_team::leaderKick);
	CASE_CMD(REQUEST_CHANGE_STRATEGY, jiange_team::setBuff);
	CASE_CMD(REQUEST_GO_FIGHTING, jiange_team::leaderStartfight);
	CASE_CMD(REQUEST_FIGHTING_LOG, jiange_team::getJiangeFightProcess);
	CASE_CMD(REQUEST_EETCH_DEFEND_AWARDS, jiange_team::getReward);
	CASE_CMD(REQUEST_DEFEND_FIGHT_RECORD, jiange_team::getJiangeFightHistory);
	CASE_CMD(REQUEST_DEFEND_RANK, jiange_team::getJiangeFightRank);

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
			role->send(out);
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
			role->send(out);
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
			role->send(out);
		}
		break;
	case S_MEET_PK_RANK:
		{
			auto stream = input_stream(data_ptr, data_len);
			service::rpc_get_meet_rank(role, stream);
		}
		break;
	case S_PRECIOUS_TRAIN_INFO:
	{
		auto role = RoleMgr::getMe().getRoleByID(client->role_id);
		if (role == nullptr)break;
		auto stream = input_stream(data_ptr, data_len);
		role->equip_get_forging_info(stream);
	}
		break;
	case S_PRECIOUS_FORGE:
	{
		auto role = RoleMgr::getMe().getRoleByID(client->role_id);
		if (role == nullptr)break;
		auto stream = input_stream(data_ptr, data_len);
		role->equip_forging(stream);
	}
		break;
	case S_PRECIOUS_UP_STAR:
	{
		auto role = RoleMgr::getMe().getRoleByID(client->role_id);
		if (role == nullptr)break;
		auto stream = input_stream(data_ptr, data_len);
		role->equip_star_up(stream);
	}
		break;
	case S_PRECIOUS_TRANSLATE:
	{
		auto role = RoleMgr::getMe().getRoleByID(client->role_id);
		if (role == nullptr)break;
		auto stream = input_stream(data_ptr, data_len);
		role->equip_inherit(stream);
	}
		break;

	case S_GET_LEVEL_PRISE_DATA:
		activity_top_upgrade::send_data(role, pkt + HEAD_SIZE, len);
		break;

	case S_REQUEST_FUND_STATE:
	{
		int status = 0;
		if (activity_fund::get_user_deliver(client->role_id, status))
		{
			output_stream out(S_REQUEST_FUND_STATE);
			out.write_byte(1);
			out.write_int(status);
			role->send(out);
		}
		else
		{
			output_stream out(S_REQUEST_FUND_STATE);
			out.write_byte(0);
			out.write_int(0);
			role->send(out);
		}
	}
		break;
	case S_REQUEST_TO_BUY_FUND:
		activity_fund::buy(role);
		break;
	case S_REQUEST_FUND_GETFUND:
	{
		auto stream = input_stream(data_ptr, data_len);
		activity_fund::get(role, stream);
		break;
	}
	case S_REQUEST_TAO_DRAW:
	{
		auto stream = input_stream(data_ptr, data_len);
		activity_taobao::tao(role, stream);
		break;
	}
	case S_REQUEST_TAO_RESULT:
	{
		auto stream = input_stream(data_ptr, data_len);
		activity_taobao::get_result(role, stream);
		break;
	}
	case S_REQUEST_TAO_ITEM:
	{
		auto stream = input_stream(data_ptr, data_len);
		activity_taobao::get_reward(role, stream);
		break;
	}
	case S_REQUEST_TAO_LEFTTIME:
	{
		auto stream = input_stream(data_ptr, data_len);
		activity_taobao::get_info(role, stream);
		break;
	}
	case S_REQUEST_BAO_INFO:
	{
		auto stream = input_stream(data_ptr, data_len);
		treasure::get_record(role, stream);
		break;
	}
	case S_REQUEST_BAO_RANK:
	{
		auto stream = input_stream(data_ptr, data_len);
		treasure::get_rank(role, stream);
		break;
	}
	case S_REQUEST_BAO_RESULT:
	{
		auto stream = input_stream(data_ptr, data_len);
		treasure::treasure_done(role, stream);
		break;
	}
	case S_REQUEST_BAO_COST:
	{
		auto stream = input_stream(data_ptr, data_len);
		treasure::cost(role, stream);
		break;
	}
	case S_BODYGUARD_BEEN_LOOT_RECORD:
	{
		auto stream = input_stream(data_ptr, data_len);
		bodyguard::rpc_been_loot_record(role, stream);
		break;
	}
	case S_BODYGUARD_BUY_LOOT_COUNT:
	{
		auto stream = input_stream(data_ptr, data_len);
		bodyguard::rpc_buy_loot_count(role, stream);
		break;
	}
	case S_BODYGUARD_DIVINATION:
	{
		auto stream = input_stream(data_ptr, data_len);
		bodyguard::rpc_buy_divination(role, stream);
		break;
	}
	case S_BODYGUARD_GET_REWARD:
	{
		auto stream = input_stream(data_ptr, data_len);
		bodyguard::rpc_get_reward(role, stream);
		break;
	}
	case S_BODYGUARD_LIST:
	{
		auto stream = input_stream(data_ptr, data_len);
		bodyguard::rpc_list(role, stream);
		break;
	}
	case S_BODYGUARD_LOOT_PRE:
	{
		return 0;
		//auto stream = input_stream(data_ptr, data_len);
		//bodyguard::rpc_loot_pre(role, stream);
		//break;
	}
	case S_BODYGUARD_START:
	{
		auto stream = input_stream(data_ptr, data_len);
		bodyguard::rpc_start(role, stream);
		break;
	}
	case S_BODYGUARD_STATUS:
	{
		auto stream = input_stream(data_ptr, data_len);
		bodyguard::rpc_get_status(role, stream);
		break;
	}

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
	service::close_session(client);
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


///*
//@查找修炼完成技能
//*/
//void PROTOCAL::selectSkillUpOk( )
//{
//	for (auto OnlineClient : service::online_players)
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
	DWORD nums = service::online_players.size();
	int n = rand() % nums;
	while (vecIDs.size() < num)
	{ 	
		vecIDs.clear();
		auto it = service::online_players.begin();
		std::advance( it, n );

		for ( ; it != service::online_players.end(); ++it )
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


//static void onSignalTerm(evutil_socket_t fd, short event, void *arg)
//{
//	//return;//不能存数据 可能产生未知错误
//
//	event_base_loopbreak(service::event_handler);
//
//	saveAllToDb(true);
//
//}

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
	RoleMgr::getMe().DailyCleanAll( );
	auto num = service::get_online_count();
	string sql = "insert into log_online_count values(0,'"+to_string(service::id)+"','"+to_string(time(0))+"','"+to_string(num)+"')";//wmsql
	logs_db_thread::singleton().push(sql);
	return arg;
}

void* timerSave(void* arg)
{
	PROTOCAL::saveAllToDb();
	return arg;
}

void* timerSaveFaction(void* arg)
{
	return arg;
}

void* timer3600(void* arg)
{
	RoleMgr::getMe().timerClearRoleCache( time(nullptr) );
	return arg;
}

void* timer1800(void* arg)
{
	//RoleMgr::getMe().timerClearRoleCache( time(nullptr) );
	RoleMgr::getMe().addOneDivineTime();
	return arg;
}

void* timer1(void* arg)
{
	TimerMgr::getMe().Update();
	WorldBoss::getMe().updateTime(time(nullptr));
	return arg;
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

void PROTOCAL::init_timers()
{

	static timer aTimer(E_TIMER_300_SECOND, 60 * 1000, timer60, 1);
	g_maptimers[E_TIMER_300_SECOND] = &aTimer;
		aTimer.set_timer();

	SET_TIMER(E_TIMER_60_SECOND, 5*61*1000, timerSave, 1);
	SET_TIMER(E_TIMER_1_SECOND, 1*1000, timer1, 1);

	SET_TIMER(E_TIMER_3600_SECOND, 3602*1000, timerSaveFaction, 1);
	SET_TIMER(E_TIMER_3600_SECOND, 3600*1000, timer3600, 1);
	SET_TIMER(E_TIMER_1800_SECOND, 1800*1000, timer1800, 1);
}