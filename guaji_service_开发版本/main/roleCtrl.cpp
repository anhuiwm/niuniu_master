#include "pch.h"
#include "log.h"
#include "base.h"
#include "protocal.h"
#include "item.h"
#include "itemCtrl.h"
#include "db.h"
#include "skill.h"
#include "zlibapi.h"
#include "roleCtrl.h"
#include "factionCtrl.h"
#include "rank.h"
#include "arena.h"
#include "shop.h"
#include "task.h"
#include "activity.h"
#include "broadCast.h"
#include "TimerMgr.h"
#include "service.h"
#include "logs_db.h"



tuple<char*, unsigned long> compressRoleblob(ROLE_BLOB* roleBlob)
{
	//static char buf[ sizeof(ROLE_BLOB) ];
	static char *buf = new char[ ESTIMATE_COMPRESS_BOUND(sizeof(ROLE_BLOB)) ];
	unsigned long dstSize = BASE::dataCompress( (const char* )roleBlob, sizeof(ROLE_BLOB), buf );
	return make_tuple(buf, dstSize);
}

#define MAX_SERIAL_LEN	32

ROLE* RoleMgr::newRole( const string& serial, const string& roleName, int job, int sex, int phone_type, int platform_id )
{
	if ( _mapRoleName2Uuids.find( roleName ) != _mapRoleName2Uuids.end() )
		return NULL;
	if ( roleName.empty() || roleName.size() >= ROLE_NAME_LEN )
		return NULL;

	string uuid = BASE::getUUID2( );

	MYSQL_ESCAPE_N(szRoleName, roleName.data(), min<size_t>(roleName.size(), ROLE_NAME_LEN), ROLE_NAME_LEN );

	if ( serial.empty() )
	{
		if (!EXEC_SQL("insert into " TABLE_ROLE_INFO "(uuid, job, sex, phone_type, platform_id, rolename) values('%s', %d, %d, %d, %d, '%s')", uuid.c_str(), job, !!sex, phone_type, platform_id, szRoleName))
		{
			return NULL;
		}
	}
	else 
	{
		MYSQL_ESCAPE_N(szParam, serial.data(), min<size_t>(serial.size(), MAX_SERIAL_LEN), MAX_SERIAL_LEN );
		if (!EXEC_SQL("insert into " TABLE_ROLE_INFO "(uuid, job, sex, phone_type, platform_id, rolename, serial) values('%s', %d, %d, %d, %d, '%s', '%s')", uuid.c_str(), job, !!sex, phone_type, platform_id, szRoleName, szParam))
		{
			return NULL;
		}
	}

	DWORD roleID = (DWORD)mysql_insert_id( DBCtrl::getSQL() );

	EXEC_SQL("insert into " TABLE_CHARGE_INFO "(roleid, charge) values(%u, %d)", roleID, 0);

	RolePtr role( new ROLE() );
	_mapUuid2Roles[uuid] = role;

	role->uuid = uuid;
	role->serial = serial;
	role->dwRoleID = roleID;
	role->byJob = (BYTE)job;
	role->bySex = !!(BYTE)sex;
	role->roleName = roleName;

	role->wLevel = 1;

	role->bySendFightBossTimes = SEND_BOSS_FIGHT_NUMS;
	role->bySendFightHeroTimes = SEND_BOSS_FIGHT_NUMS;
	role->m_packmgr.giveItems();

	CONFIG::forgeRefresh( role.get() );
	role->calRoleAttr( );
	role->cMarMgr.cMarMys.initTime();
	_mapRoleId2Uuids[roleID] = uuid;
	_mapRoleName2Uuids[roleName] = uuid;

	role->roleSave2DB();

	return role.get();
}

#if 0
//��������:
	E_TASK_LEVEL = 1,					//1��ҵȼ���				
	E_TASK_KILL_MON = 2,				//2ɱ�֣�
	E_TASK_HAS_EQUIP_ENHANCE = 3,		//3����ǿ����
	E_TASK_HAS_EQUIP_RARE = 4,			//4װ��ϡ�жȣ�
	E_TASK_HAS_EQUIP_COMPOSE = 5,		//5װ���ϳɣ�
	E_TASK_TITLE = 6,					//6ӵ�гƺţ�
	E_TASK_VIP_LEVEL = 7,				//7VIP�ȼ���
	E_TASK_BUY_QUICK_FIGHT = 8,			//8����ս�����������
	E_TASK_SPECIAL_TASK = 9,			//9���ָ������
	E_TASK_DO_EQUIP_ENHANCE = 10,		//10����ǿ��������
	E_TASK_DO_EQUIP_COMPOSE = 11,		//11���кϳɲ�����
	E_TASK_FACTION_DONATE = 12,			//12���й�����ײ�����
	E_TASK_EVENT = 13,					//13����¼���
	E_TASK_ARENA = 14,					//14��������ս��
	E_TASK_MINING = 15,					//15�����ڿ�
	E_TASK_FACTION_WAR = 16,			//16�μ��л�ս,
	E_TASK_EQUIP = 17,					//17����һ��װ������������
	E_TASK_USE_MONEYTREE = 18,			//18ʹ��ҡǮ����
	E_TASK_WORLD_BOSS = 19,				//19��������BOSSս��
	E_TASK_FIGHT_BOSS = 20,				//20BOSS��ս
	E_TASK_SMELT = 21,					//21����
	E_TASK_ALL_EQUIP_LEVEL = 22,		//22ȫ��װ���ȼ�	������ϴ�����װ��������ָ���ȼ�����ʽ��װ���ȼ�
	E_TASK_GET_GREEN_TIMES = 23,		//23�����װ����	����ܹ���õ���װ��������ʽ����װ��õĴ���
	E_TASK_FORGE_EQUIP_TIMES = 24,		//24����װ������	���ͨ�������õ�װ����������ʽ������Ĵ���
	E_TASK_UNIJECT_DIAMOND = 25,		//25��Ƕ��ʯ����	�������װ���ܹ���Ƕ�ı�ʯ��������ʽ������װ���ܹ��ı�ʯ������
	E_TASK_EQUIP_RESET_TIMES = 26,		//26ϴ��װ������	���ϴ��װ���Ĵ�������ʽ��ϴ���Ĵ���
	E_TASK_EQUIP_LUCK = 27,				//27�������˵Ĳ�������	��Ҳ����������˵Ĵ�������ʽ�������������˵Ĵ���
	E_TASK_CLOTH_LUCK = 28,				//28�·����˵Ĳ�������	��Ҳ����·����˵Ĵ�������ʽ�������·����˵Ĵ���
	E_TASK_GET_SUIT_TIMES = 29,			//29������װ����	�������װ���������װ������Ŀ����ʽ����װ���Լ�����Ŀ��
	E_TASK_BUY_FIGHT_BOSS_TIMES = 30,	//30����boss��ս����	������ս�ؿ�boss�Ĵ�������ʽ���������
	E_TASK_SHOP_BUY_TIMES = 31,			//31�����̳ǵ��ߴ���	�����̳ǵ��ߵĴ�������ʽ���������
	E_TASK_SET_FIGHT_SKILL_NUM = 32,	//32����ս������	�����е�ս�����ܸ�������ʽ���������õĸ���
	E_TASK_GET_MERCENARY_NUM = 33,		//33���Ӷ��	����Ӷ������������ʽ������Ӷ����������
	E_TASK_FIGHT_BOSS_TIMES = 34,		//34boss��ս	�����ս����ؿ�boss����ʽ����ս����
	E_TASK_FIGHT_ELITE_TIMES = 35,		//35��Ӣ�ؿ�boss��ս	�����ս���⾫Ӣ�ؿ�boss����ʽ����ս����
	E_TASK_RUNE_TIMES = 36,				//36����ռ������	��ҷ���ϵͳռ���Ĵ�������ʽ��ռ������
	E_TASK_MERCENARY_STAR = 37,			//37Ӷ���Ǽ�	���Ӷ���ﵽ������Ǽ�����ʽ��Ӷ���Ǽ�
#endif


void RoleMgr::judgeCompleteTypeOpenActivity(ROLE* role, const BYTE& byType,  ITEM_INFO* pPara )
{
	vector<WORD>* taskIDsCfg= CONFIG::getOpenActivityVecIDsByType(byType);
	if (taskIDsCfg == NULL)
	{
		return;
	}

	vector<WORD>& vecIDs = *taskIDsCfg;
	for (size_t i = 0; i < vecIDs.size(); i++)
	{
		judgeCompleteOpenActivity(role, vecIDs[i], pPara );
	}
}

void RoleMgr::judgeCompleteOpenActivity(ROLE* role, const WORD& wTaskID, ITEM_INFO* pPara )
{
	CONFIG::OPEN_ACTIVITY *missionCfg = CONFIG::getOpenActivityCfg( wTaskID );
	RETURN_VOID( missionCfg == NULL );

	auto it = role->mapCompleteActivityIDs.find( wTaskID );

	RETURN_VOID( it == role->mapCompleteActivityIDs.end() );


	auto& task = it->second;

	RETURN_VOID(wTaskID != task.wTaskID);//û������

	RETURN_VOID( task.byTaskStatus != E_TASK_UNCOMPLETE );//����δ��ɵ�

	switch (missionCfg->type)
	{
	case E_ACTIVITY_LOGIN:
			task.byTaskStatus = E_TASK_COMPLETE_CAN_GET_PRIZE;
			break;
	case E_ACTIVITY_FIRST_CHARGE:
			task.byTaskStatus = E_TASK_COMPLETE_CAN_GET_PRIZE;
			break;
		default:
			return;
	}
}

void RoleMgr::judgeCompleteTypeTask(ROLE* role, const BYTE& byType,  ITEM_INFO* pPara )
{
	//logwm("task rolename = %s , byType=%u",role->roleName.c_str(), byType);

	vector<WORD>* taskIDsCfg= CONFIG::getTaskVecIDsByType(byType);
	if (taskIDsCfg == NULL)
	{
		return;
	}
	
	vector<WORD>& vecIDs = *taskIDsCfg;
	for (size_t i = 0; i < vecIDs.size(); i++)
	{
		judgeCompleteTask(role, vecIDs[i], pPara );
	}
}

void RoleMgr::judgeCompleteTask(ROLE* role, const WORD& wTaskID,  ITEM_INFO* pPara )
{
	CONFIG::MISSION_CFG *missionCfg = CONFIG::getMissionCfg( wTaskID );
	if ( missionCfg == NULL )
	{
		//logwm("task missionCfg == NULL");
		return;
	}

	if (role->wLevel < missionCfg->level_limit)//�ȼ�δ��
	{
		//logwm("task role->wLevel = %u < %u",role->wLevel, missionCfg->level_limit);
		return;
	}
	
	auto it = role->mapTask.find( TASK::getGroupByID(wTaskID) );//û�����������
	if ( it == role->mapTask.end() )
	{
		return;
	}

	auto& task = it->second;

	if (wTaskID != task.wTaskID)//û���������
	{
		return;
	}
	
	if ( task.byTaskStatus != E_TASK_UNCOMPLETE )//����δ��ɵ�
	{
		return;
	}

	switch (missionCfg->type)
	{
		case E_TASK_LEVEL:
			task.byTaskCompleteNums = (BYTE)role->wLevel;
			if (role->wLevel < missionCfg->parameter.itemNums)
			{
				TASK::clientGetTasksList( role, NULL, 0 );
				return;
			}
			break;//1��ҵȼ���

		case E_TASK_KILL_MON:
			RETURN_VOID( pPara ==NULL );
			if (pPara->itemIndex != missionCfg->parameter.itemIndex)//ɱָ����ͼ�Ĺ���
			{
				return;
			}
			task.byTaskCompleteNums += (BYTE)pPara->itemNums;
			if (task.byTaskCompleteNums < missionCfg->parameter.itemNums)
			{
				TASK::clientGetTasksList( role, NULL, 0 );
				return;
			}
			break;//2ɱ�֣�

		case E_TASK_HAS_EQUIP_RARE:
			RETURN_VOID( pPara ==NULL );
			if (pPara->itemIndex != missionCfg->parameter.itemIndex || pPara->byQuality != (WORD)missionCfg->parameter.itemNums)
			{
				return;
			}
			break;//4װ��ϡ�жȣ�

		case E_TASK_HAS_EQUIP_COMPOSE:
			if (pPara->itemIndex != missionCfg->parameter.itemIndex)
			{
				return;
			}
			task.byTaskCompleteNums++;
			if (task.byTaskCompleteNums < missionCfg->parameter.itemNums)
			{
				TASK::clientGetTasksList( role, NULL, 0 );
				return;
			}
			break;//5װ���ϳɣ�

	//һ��ֻ���һ��
		case E_TASK_HAS_EQUIP_ENHANCE:	
		case E_TASK_BUY_QUICK_FIGHT:
		case E_TASK_DO_EQUIP_ENHANCE:
		case E_TASK_DO_EQUIP_COMPOSE:
		case E_TASK_FACTION_DONATE:
		case E_TASK_ARENA:
		case E_TASK_MINING:
		case E_TASK_FACTION_WAR:
		case E_TASK_EQUIP:
		case E_TASK_USE_MONEYTREE:
		case E_TASK_WORLD_BOSS:
		case E_TASK_SMELT:
		case E_TASK_FORGE_EQUIP_TIMES	:       			//24����װ������		���ͨ�������õ�װ����������ʽ������Ĵ���
		case E_TASK_UNIJECT_DIAMOND		:       			//25��Ƕ��ʯ����		�������װ���ܹ���Ƕ�ı�ʯ��������ʽ������װ���ܹ��ı�ʯ������
		case E_TASK_EQUIP_RESET_TIMES	:       			//26ϴ��װ������		���ϴ��װ���Ĵ�������ʽ��ϴ���Ĵ���
		case E_TASK_EQUIP_LUCK			:					//27�������˵Ĳ�������	��Ҳ����������˵Ĵ�������ʽ�������������˵Ĵ���
		case E_TASK_CLOTH_LUCK			:       			//28�·����˵Ĳ�������	��Ҳ����·����˵Ĵ�������ʽ�������·����˵Ĵ���
		case E_TASK_GET_SUIT_TIMES		:       			//29������װ����		�������װ���������װ������Ŀ����ʽ����װ���Լ�����Ŀ��
		case E_TASK_BUY_FIGHT_BOSS_TIMES:					//30����boss��ս����	������ս�ؿ�boss�Ĵ�������ʽ���������
		case E_TASK_GET_MERCENARY_NUM	:       			//33���Ӷ��			����Ӷ������������ʽ������Ӷ����������
		case E_TASK_FIGHT_BOSS_TIMES	:       			//34boss��ս			�����ս����ؿ�boss����ʽ����ս����
		case E_TASK_FIGHT_ELITE_TIMES	:       			//35��Ӣ�ؿ�boss��ս	�����ս���⾫Ӣ�ؿ�boss����ʽ����ս����
		case E_TASK_RUNE_TIMES			:       			//36����ռ������		��ҷ���ϵͳռ���Ĵ�������ʽ��ռ������
			task.byTaskCompleteNums++;
			if (task.byTaskCompleteNums < missionCfg->parameter.itemNums)
			{
				TASK::clientGetTasksList( role, NULL, 0 );
				return;
			}
			break;//8����ս��������

		case E_TASK_FIGHT_BOSS:
			RETURN_VOID( pPara ==NULL );
			//if (pPara->itemNums != missionCfg->parameter.itemNums || pPara->itemIndex != missionCfg->parameter.itemIndex)
			if (pPara->itemIndex < missionCfg->parameter.itemIndex)
			{
				return;
			}
			break;// = 20,				//20��սBOSS

	//�����������
		case E_TASK_SET_FIGHT_SKILL_NUM	:					//32����ս������		�����е�ս�����ܸ�������ʽ���������õĸ���
		case E_TASK_ALL_EQUIP_LEVEL		:   
		case E_TASK_MERCENARY_STAR		: 
		case E_TASK_VIP_LOGIN		:  
			RETURN_VOID( pPara ==NULL );
			if ( pPara->itemNums < missionCfg->parameter.itemNums )
			{
				TASK::clientGetTasksList( role, NULL, 0 );
				return;
			}
			break;

	//һ�ο�����ɶ��
		case E_TASK_SHOP_BUY_TIMES		:       			
			RETURN_VOID( pPara ==NULL );
			task.byTaskCompleteNums += (BYTE)pPara->itemNums;
			if (task.byTaskCompleteNums < missionCfg->parameter.itemNums)
			{
				TASK::clientGetTasksList( role, NULL, 0 );
				return;
			}
			break;

		default:
			return;
	}

	completeTask( role, wTaskID, 0 );//���˴����
}

void RoleMgr::completeTask(ROLE* role, const WORD& wTaskID, BYTE byNums)
{
	WORD group = TASK::getGroupByID(wTaskID);
	if (group != 0)
	{
		auto it = role->mapTask.find( group );
		if ( it != role->mapTask.end() && wTaskID == it->second.wTaskID)//��������ȱ��
		{
			it->second.byTaskStatus = E_TASK_COMPLETE_CAN_GET_PRIZE;
			it->second.byTaskCompleteNums = byNums;
			TASK::clientGetTasksList( role, NULL, 0 );

			CONFIG::MISSION_CFG* taskCfg = CONFIG::getMissionCfg(wTaskID);
			TASK::notifyRoleTaskStatus( role, taskCfg, 2 );
		}
	}
}

#define UNCOMPRESS_ROLE_BLOB(dst, src, src_size, ret) \
		do{\
		unsigned long dst_size = sizeof(ROLE_BLOB);\
		if ( UnCompress(dst, &dst_size, src, src_size) != Z_OK || dst_size != sizeof(ROLE_BLOB)) {\
			logFATAL("%d != %d", dst_size, sizeof(ROLE_BLOB));\
			ret = false;\
		}}while(0)
#define TOSMALL(x) (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))  

string RoleMgr::loadZoneInfoFromDB( ROLE* role )//��Ϊ��ȡ�ļ�
{
	string strZone;
	
	if (role == NULL || role->client == NULL)
	{
		return strZone;
	}
	//DWORD dwIP1 = CStr2BigendianIP("1.206.175.0");
	//dwIP1 =  T(dwIP1);
	DWORD dwIP = CStr2BigendianIP(role->client->ip.c_str());
	dwIP =  TOSMALL(dwIP);
	//SQLSelect sqlx( DBCtrl::getSQL(), "select Country from IPTABLE where %u >= StartIPNum and %u <= EndIPNum limit 1 ",dwIP,dwIP );
	//MYSQL_ROW row = sqlx.getRow();
	//if ( row==NULL )
	//{
	//	strZone = "Internal network";
	//	return strZone;
	//}
	//strZone = row[0];

	auto pInfo = CONFIG::findIP(dwIP);
	if ( pInfo != NULL )
	{
		strZone = pInfo->Country;
	}
	
	return strZone;
}

ROLE* RoleMgr::loadRoleInfoFromDB(const string& uuid, DWORD dwRoleID, const string* pRoleName, const string* ptr_imei)
{
	char where[256]={0};

	if ( !uuid.empty() )
		sprintf( where, " where uuid='%s' ", uuid.c_str() );
	else if ( dwRoleID )
		sprintf( where, " where id=%u ", dwRoleID );
	else if ( pRoleName )
	{
		//sprintf( where, " where rolename='%s' ", pRoleName->data() );
		char *p = where;
		p += sprintf( p, " where rolename='");
		p += mysql_escape_string( p, pRoleName->data(), pRoleName->size() );
		p += sprintf( p, "'" );
	}
	else if (ptr_imei)
	{
		char *p = where;
		p += sprintf(p, " where imei='");
		p += mysql_escape_string(p, ptr_imei->data(), ptr_imei->size());
		p += sprintf(p, "'");
	}
	else
	{
		return NULL;
	}

	SQLSelect sqlx( DBCtrl::getSQL(), "select rolename, level, viplevel, exp, hp, money, gold, charge, mapid, cur_sceneid, fromid,unix_timestamp(lastaccess), unix_timestamp(inc_hp_time), roleblob, uuid, id,job,sex,email,imei from "
		TABLE_ROLE_INFO " %s limit 1", where );

	MYSQL_ROW row = sqlx.getRow();
	if ( row==NULL )
		return NULL;


	dwRoleID = ATOUL(row[15]);

	string dbuuid( row[14] );
	RolePtr role(new ROLE());
	role->dwRoleID = dwRoleID;
	role->uuid = dbuuid;

	if ( row[0] )
		role->roleName  = row[0];
	if ( role->roleName.empty() )
	{
		return NULL;
	}

	role->wLevel        = ATOW(row[1]);
	role->wVipLevel = ATOW(row[2]);
	role->dwExp     = ATOUL(row[3]);
	role->dwHP      = ATOUL(row[4]);
	role->dwCoin   = ATOUL(row[5]);
	role->dwIngot        = ATOUL(row[6]);

	role->dwCharge        = ATOUL(row[7]);
	
	role->dwHonour	= 0;
	
	//role->byCurSceneID  = ATOB(row[9]);
	//role->byCurFormID  = ATOB(row[10]);

	role->tLastAccess  = ATOUL(row[11]);
	role->tIncHPTime  = ATOUL(row[12]);
	role->byJob  = ATOB(row[16]);
	role->bySex  = ATOB(row[17]);
	if (row[18])
	{
		role->email  = row[18];
	}
	if (row[19])
	{
		role->phone_imei = row[19];
	}
	
	role->faction = guild::get_guild_from_roleid(dwRoleID);



	if ( sqlx.lengths[13] )
	{
		ROLE_BLOB roleBlob;

		unsigned long dstSize = sizeof(ROLE_BLOB);
		if ( UnCompress( &roleBlob, &dstSize, row[13], sqlx.lengths[13]) != Z_OK)// || dstSize != sizeof(ROLE_BLOB) )
		{
			logFATAL("%u != %u", dstSize, sizeof(ROLE_BLOB) );
			return NULL;
		}

		if ( !role->blob2Cache( roleBlob ) )
		{
			return NULL;
		}
	}
	else
	{
		//blob == NULL, ������û���ü������,ʹ�ó�ʼ����ROLE::ROLE()
	}

	if ( !role->roleName.empty() )
	{
		_mapUuid2Roles[ dbuuid ] = role;
		_mapRoleName2Uuids[ role->roleName ] = role->uuid;
		_mapRoleId2Uuids[ dwRoleID ] = role->uuid;
		if (!role->phone_imei.empty())
		{
			_map_imei_uuids[role->phone_imei] = role->uuid;
		}
	}


	role->patch();

	role->calRoleAttr( );
	
	//_mapCookie2Role[ row[14] ] = role;
	return role.get();
}

void RoleMgr::timerClearRoleCache( DWORD now )
{
	//������RoleMgr.loadRoleInfoFromDB,	�������ʱ��
	const DWORD OFFLINE_TIME = 2*24*60*60;
	const DWORD MAX_CACHE_ROLES = 10000;

	if ( _mapUuid2Roles.size() < MAX_CACHE_ROLES )
		return;
	
	for ( auto it = _mapUuid2Roles.begin(); it!=_mapUuid2Roles.end(); )
	{
		//�����ߣ���������ʱ�䳬��3��
		if ( service::get_online_session(it->second->dwRoleID )== NULL && now - it->second->tLogoutTime  > OFFLINE_TIME )
		{
			_map_imei_uuids.erase(it->second->phone_imei);
			_mapRoleName2Uuids.erase( it->second->roleName );
			_mapRoleId2Uuids.erase( it->second->dwRoleID );
			_mapUuid2Roles.erase(it++);
			
		}
		else
		{
			++it;
		}
	}
}

#include "mailCtrl.h"
void RoleMgr::timerDailyClean( ROLE* role, time_t now ,bool bForce )
{
	if (bForce)
	{
		role->byArenaDayCount = 0;
		role->byBuyArenaDayCount = 0;
		role->vecDianZhanIDs.clear();

		role->bySendFightBossTimes = SEND_BOSS_FIGHT_NUMS;
		role->byDailyBuyBossTimes = 0;
		role->bySendFightHeroTimes = SEND_BOSS_FIGHT_NUMS;
		role->byDailyBuyHeroTimes = 0;

		role->byBuyQuickFightTimes = 0;
		role->byForgeRreshTimes = 0;
		role->shrine_times = 0;
		//ACTIVITY::setBossFightStatusNO();
		role->cMerMgr.setFreshTimes(FRESH_MERCENARY_NUM);
		role->cMarMgr.cMarJew.resetFreshTime();
		role->cMarMgr.cMarMys.resetFreshTime();
		role->byUseMoneyTreeTimes = 0;
		role->cMarMgr.cMarJew.InitShopItem( role, false, true );
		return;
	}
	
	if ( !BASE::inTheSameDay( (time_t) role->tLastAccess, now ) )
	{
		if ( !BASE::inTheSameMonth( (time_t) role->tLastAccess, now ) )
		{
			role->dwDaySignStatus = 0;
		}
		
		role->byArenaDayCount = 0;
		role->byBuyArenaDayCount = 0;
		role->vecDianZhanIDs.clear();
		role->byBuyQuickFightTimes = 0;
		role->bySendFightBossTimes = SEND_BOSS_FIGHT_NUMS;
		role->byDailyBuyBossTimes = 0;
		role->bySendFightHeroTimes = SEND_BOSS_FIGHT_NUMS;
		role->byDailyBuyHeroTimes = 0;
		role->byForgeRreshTimes = 0;
		role->shrine_times = 0;
		role->cMerMgr.setFreshTimes(FRESH_MERCENARY_NUM);
		role->cMarMgr.cMarJew.resetFreshTime();
		role->cMarMgr.cMarMys.resetFreshTime();
		TASK::dailySetTask( role );
		role->byUseMoneyTreeTimes = 0;
		role->cMarMgr.cMarJew.InitShopItem( role, false, true );
		TASK::clearRepeatOpenActivityDaily( role );

		resetVipByLogin(role);

		if (role->faction)
		{
			auto member = role->faction->get_member(role->dwRoleID);
			if (member)
			{
				member->contribute_count = 0;
			}
		}
		if (role->bodyguard_loot_count < 2)role->bodyguard_loot_count = 2;
		role->bodyguard_buy_loot_count = 0;

		if (role->jiangeState == E_TASK_COMPLETE_CAN_GET_PRIZE || role->jiangeState == E_TASK_HAVEPRIZE_UNJOIN)
		{
			role->jiangeState = E_TASK_HAVEPRIZE_UNJOIN;
		}
		else
		{
			role->jiangeState = E_TASK_UNCOMPLETE;
		}
	}
	role->tLastAccess = time(nullptr);
}

string RoleMgr::findUuidBySerial( const string& serial, const BYTE platform_id )
{
	string dbuuid;

	MYSQL_ESCAPE_N(szParam, serial.data(), min<size_t>(serial.size(), MAX_SERIAL_LEN), MAX_SERIAL_LEN);

	SQLSelect sqlx( DBCtrl::getSQL(), "select uuid from "
		TABLE_ROLE_INFO " where serial = '%s' and platform_id = %u limit 1", szParam, platform_id);

	//SQLSelect sqlx( DBCtrl::getSQL(), "select uuid from "
	//	TABLE_ROLE_INFO " where serial='1754a10f7f3fe13a42c3d43f2fc38e8c' limit 1" );

	MYSQL_ROW row = sqlx.getRow();
	if ( row!=NULL )
	{
		dbuuid = row[0];
	}

	return dbuuid;
}

ROLE* RoleMgr::findRoleByUUID( const string& uuid )
{
	auto it = _mapUuid2Roles.find(uuid);
	if ( it != _mapUuid2Roles.end() )
		return it->second.get();

	return loadRoleInfoFromDB( uuid );
}

ROLE* RoleMgr::getRoleByImei(const string& phone_imei)
{
	ROLE* role = getCacheRoleByImei(phone_imei);
	if (role != NULL)
	{
		return role;
	}

	return loadRoleInfoFromDB("", 0, NULL,&phone_imei);
}


ROLE* RoleMgr::getCacheRoleByImei(const string& phone_imei)
{
	auto it = _map_imei_uuids.find(phone_imei);
	if (it == _map_imei_uuids.end())
	{
		return NULL;
	}

	const string& uuid = it->second;
	auto it2 = _mapUuid2Roles.find(uuid);
	if (it2 == _mapUuid2Roles.end())
	{
		_map_imei_uuids.erase(phone_imei);
		return NULL;
	}

	return it2->second.get();
}

ROLE* RoleMgr::getCacheRoleByName(const string& roleName)
{
	auto it = _mapRoleName2Uuids.find( roleName );
	if (it == _mapRoleName2Uuids.end())
	{
		return NULL;
	}
	
	const string& uuid = it->second;
	auto it2 = _mapUuid2Roles.find(uuid);
	if (it2 == _mapUuid2Roles.end())
	{
		_mapRoleName2Uuids.erase(roleName);
		return NULL;
	}
	
	return it2->second.get();
}


ROLE* RoleMgr::getRoleByName(const string& roleName)
{
	ROLE* role = getCacheRoleByName(roleName);
	if (role != NULL)
	{
		return role;
	}
	
	return loadRoleInfoFromDB("", 0, &roleName );
}

ROLE* RoleMgr::getCacheRoleByID( DWORD dwRoleID)
{
	auto it = _mapRoleId2Uuids.find(dwRoleID);
	if (it == _mapRoleId2Uuids.end())
	{
		return NULL;
	}
	
	const string& uuid = it->second;
	auto it2 = _mapUuid2Roles.find(uuid);
	if (it2 == _mapUuid2Roles.end())
	{
		_mapRoleId2Uuids.erase(dwRoleID);
		return NULL;
	}

	return it2->second.get();
}

ROLE* RoleMgr::getRoleByID( DWORD dwRoleID)
{
	ROLE* role = getCacheRoleByID(dwRoleID);
	if (NULL != role)
	{
		return role;
	}
	
	return loadRoleInfoFromDB( "", dwRoleID );
}

#define RECOMMOND_NUMS_LIMIT  (6)		//�Ƽ�������������

void RoleMgr::getRecommondIDs( ROLE*role, vector<DWORD> &recommondIDs )
{

	PROTOCAL::getOnlineID(role, recommondIDs, (WORD)RECOMMOND_NUMS_LIMIT);
	//���ڴ����


	//�����ݿ�
	//WORD level = role->wLevel;
	int level = (int)role->wLevel;
	while (recommondIDs.size() < RECOMMOND_NUMS_LIMIT)
	{ 
		recommondIDs.clear();
		char where[256]={0};
		sprintf( where, " where level>=%u and rolename is not null order by level asc  limit %d",level, RECOMMOND_NUMS_LIMIT + 31 );
		SQLSelect sqlx( DBCtrl::getSQL(), "select id from " TABLE_ROLE_INFO " %s ", where );
		while( MYSQL_ROW row = sqlx.getRow() )
		{
			if (ATOUL(row[0]) == role->dwRoleID)
			{
				continue;
			}
			vector<DWORD>::iterator itdword2 = find(role->vecFriends.begin(), role->vecFriends.end(), ATOUL(row[0]));
			if (itdword2 != role->vecFriends.end())
			{
				continue;
			}
			recommondIDs.push_back(ATOUL(row[0]));
			if (recommondIDs.size() >=  RECOMMOND_NUMS_LIMIT)
			{
				return;
			}
		}
		level -=5 ;
		if (level <= 0)
		{
			break;
		}
	}	
}

/*
@����60sһ��
*/
void RoleMgr::DailyCleanAll()
{
	time_t now = time(nullptr);
	ROLE *role = NULL;

	for (auto& item : service::online_players)
	{
		if ((role = item.second.role) != NULL)
		{
			timerDailyClean(role, now );
		}	
	}
}


void RoleMgr::onLevelUP( ROLE* role, WORD wOldLevel )
{
	activity_top_upgrade::update(role);
	RANK::insertLevelRanks(role);

	if (role->wLevel >= ARENA_NEED_LEVEL)
	{
		ARENA:: insertArenaRank(role);
	}

	role->cMerMgr.addMercenary( role->wLevel );
	
	RoleMgr::judgeCompleteTypeTask( role, E_TASK_LEVEL );

	//ACTIVITY::judgeNewActivity( role, wOldLevel );//�»����

	CONFIG::upgradeForgeRefresh( role, wOldLevel );

	ItemCtrl::updateNotifyRoleAttr(role, true);

	notify_X_If( role, ROLE_PRO_LEVEL, wLevel, wOldLevel );

	//���ߵ�ʱ����������㲥,playinfo��û�����ͻ���
	BroadcastCtrl::levelUpgrade( role );

	string sql = "insert into log_levelup values(0,"+to_string(service::id)+", '"+role->roleName+"', "+
		to_string(time(0))+","+to_string(role->dwRoleID)+", "+to_string(role->wLevel)+","+to_string(wOldLevel)+")";
	logs_db_thread::singleton().push(sql);

	role->onLevelUp();
}

// ռ����������1
void RoleMgr::addOneDivineTime()
{
	for (auto itr=_mapUuid2Roles.begin(); itr!=_mapUuid2Roles.end(); itr++)
	{
		RolePtr pRole = itr->second;
		if (NULL == pRole)
		{
			continue;
		}
		pRole->role_rune.addOneDivineTime();
	}
}


void RoleMgr::addFiveDivineTime()
{
	for (auto itr=_mapUuid2Roles.begin(); itr!=_mapUuid2Roles.end(); itr++)
	{
		RolePtr pRole = itr->second;
		if (NULL == pRole)
		{
			continue;
		}
		pRole->role_rune.addFiveDivineTime();// ռ����������5
	}
}

// ����ռ������
void RoleMgr::resetDivineTime()
{
	for (auto itr=_mapUuid2Roles.begin(); itr!=_mapUuid2Roles.end(); itr++)
	{
		RolePtr pRole = itr->second;
		if (NULL == pRole)
		{
			continue;
		}
		pRole->role_rune.resetDivineTime();
	}

	ARENA::offerArenaReward();//������ÿ��0�㷢
	guild::daily_reset_guilds();//�л�Ĥ�ݴ���
}

void RoleMgr::resetVipByLogin( ROLE* role )
{
	if (role->wLevel >= 40 && role->login_day_num <= VIP_NEED_LOGIN_DAY)
	{
		role->login_day_num++;

		if (role->login_day_num >= 90  )
		{
			role->setLoginVipLevel(4);
		}
		else if (role->login_day_num >= 30 )
		{
			role->setLoginVipLevel(3);
		}
		else if (role->login_day_num >= 7 )
		{
			role->setLoginVipLevel(2);
		}
		else if ( role->login_day_num >= 1 )
		{
			role->setLoginVipLevel(1);				
		}
	}

}

void RoleMgr::triggerOpenGS7Day()
{
	activity_top_upgrade::send_award();
}

void RoleMgr::triggerEight()
{
	////ɾ������ڼ䣬ÿ������8�㡢����12�㡢����20�㣬ȫ��ÿ����ҷ����ʼ���
	////	�ʼ�����1��300Ԫ����vip3���鿨 * 1
	////	�ʼ�����2��300Ԫ��
	////	�ʼ�����3��300Ԫ��

	//vector<ITEM_INFO> vecItemInfo;
	//vecItemInfo.push_back(ITEM_INFO(20011, 300));
	//vecItemInfo.push_back(ITEM_INFO(810, 1));
	//MailCtrl::send_full_scale_mail(E_MAIL_TYPE_PRIZE, (CONFIG::getMsgSendOne()).c_str(), vecItemInfo);
}

void RoleMgr::triggerAtTwelve()
{
	for (auto itr = _mapUuid2Roles.begin(); itr != _mapUuid2Roles.end(); itr++)
	{
		RolePtr pRole = itr->second;
		if (NULL == pRole)
		{
			continue;
		}
		pRole->cMarMgr.freshJewShop(pRole.get());//�����̳Ǹ���
	}

	//vector<ITEM_INFO> vecItemInfo;
	//vecItemInfo.push_back(ITEM_INFO(20011, 300));
	//MailCtrl::send_full_scale_mail(E_MAIL_TYPE_PRIZE, (CONFIG::getMsgSendTwo()).c_str(), vecItemInfo);
}

void RoleMgr::triggerTwenty()
{
	////ɾ������ڼ䣬ÿ������8�㡢����12�㡢����20�㣬ȫ��ÿ����ҷ����ʼ���
	////�ʼ����ݣ�300Ԫ����vip3���鿨 * 1

	//vector<ITEM_INFO> vecItemInfo;
	//vecItemInfo.push_back(ITEM_INFO(20011, 300));
	//MailCtrl::send_full_scale_mail(E_MAIL_TYPE_PRIZE, (CONFIG::getMsgSendThree()).c_str(), vecItemInfo);
}

void RoleMgr::init()
{


	////ÿ��8�����
	//TimerMgr::getMe().AddinDayStaticTimer(std::bind(&RoleMgr::triggerEight,this), 8*3600);

	////ÿ��20�����
	//TimerMgr::getMe().AddinDayStaticTimer(std::bind(&RoleMgr::triggerTwenty,this), 20*3600);

	//ÿ��12�����
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&RoleMgr::triggerAtTwelve,this), 12*3600);

	// ÿ��12���21��ʱ������5�㣬
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&RoleMgr::addFiveDivineTime,this), 12*3600 );
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&RoleMgr::addFiveDivineTime,this), 21*3600);
	// ÿ��0������ʣ�����Ϊ����ֵ
	TimerMgr::getMe().AddinDayStaticTimer(std::bind(&RoleMgr::resetDivineTime,this), 0);

	// ����7��
	TimerMgr::getMe().AddOnceTimer( time_t(TASK::getDayFirstTime(service::open_time ))+7 * 24 * 3600, std::bind(&RoleMgr::triggerOpenGS7Day, this));

}

void RoleMgr::ExecAllRoles(RoleFunction func)
{
	for (auto itr=_mapUuid2Roles.begin(); itr!=_mapUuid2Roles.end(); itr++)
	{
		RolePtr pRole = itr->second;
		if (NULL == pRole)
		{
			continue;
		}

		func(pRole);
	}
}

size_t RoleMgr::get_cached_role_count()
{
	return _mapUuid2Roles.size();
}


void RoleMgr::save_data(bool bForce )
{
	auto now = time(0);

	
	for (auto it : service::online_players)
	{
		it.second.role->tLogoutTime = now;
	}

	for (auto it : _mapUuid2Roles)
	{
		if ( bForce || (now - it.second->tLastSaveTime > 60) )
		{
			it.second->tLastSaveTime = now;
			it.second->roleSave2DB();
		}
	}
}

void RoleMgr::insert_imei_uuids(ROLE* role)
{
	RETURN_VOID(role->phone_imei.empty());
	_map_imei_uuids[role->phone_imei] = role->uuid;
}