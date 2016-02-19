#include "task.h"
#include "itemCtrl.h"
#include "roleCtrl.h"
#include "mailCtrl.h"
#include "logs_db.h"
#include "service.h"


//#define  NOT_MAIN_TASK_ID    20
//#define  MAX_ROLE_LEVEL      99
namespace TASK
{
#pragma  pack(push, 1)
	struct TOpenActivityType
	{
		BYTE type;
		BYTE status;
		DWORD time;
		TOpenActivityType( 	BYTE type, BYTE status, DWORD time )
		{
			this->type = type;
			this->status = status;
			this->time = time;
		}
		ZERO_CONS(TOpenActivityType);
	};
#pragma  pack(pop)

	static const DWORD dwDaySecond = 24 * 3600;

	void notifyTaskStatus( ROLE* role, const BYTE& status );

	WORD getLevelByID( const WORD& wTaskID);

	DWORD getDayFromLogin( ROLE* role );

	void clearTimeoutOpenActivity( ROLE* role );

	void getOpenActivity( ROLE* role );
}

DWORD TASK::getDayFromLogin( ROLE* role )
{
	time_t longFirstLoginTime = role->getFirstLoginTime();
	DWORD dwNow = PROTOCAL::getCurTime();

	if (longFirstLoginTime == 0 || dwNow < (DWORD)longFirstLoginTime)
	{
		role->setFirstLoginTime(dwNow);
		longFirstLoginTime =dwNow;
	}
	struct tm * timeinfo;
	timeinfo = localtime ( &longFirstLoginTime );
	timeinfo->tm_hour=0;
	timeinfo->tm_min=0;
	timeinfo->tm_sec=0;
	DWORD dwFirstLoginTime = mktime(timeinfo);

	DWORD dwHaveLoginTime = 0;

	if ( dwNow > dwFirstLoginTime )
	{
		dwHaveLoginTime = dwNow - dwFirstLoginTime;
	}


	DWORD loginDay = dwHaveLoginTime / dwDaySecond + 1;//第几天登陆

	return loginDay;
}

void TASK::clearTimeoutOpenActivity( ROLE* role )
{
	DWORD loginDay = getDayFromLogin(role);//第几天登陆

	DWORD dwCondTime = 0;

	//超时的清空
	for ( auto it = role->mapCompleteActivityIDs.begin(); it != role->mapCompleteActivityIDs.end(); )
	{
		CONFIG::OPEN_ACTIVITY* openActivity =  CONFIG::getOpenActivityCfg( it->first );
		if ( openActivity == NULL )
		{
			role->mapCompleteActivityIDs.erase( it++ );
			continue;
		}

		if ( openActivity->type == 1 )
		{
			dwCondTime = loginDay;
		}
		else if ( openActivity->type == 3)
		{
		}
		else if ( openActivity->type == 2)
		{
		}

		if( it->second.byTaskStatus != E_TASK_COMPLETE_ALREADY_GET_PRIZE  )
		{//时间0无限制
			if( ( (dwCondTime < openActivity->begin_time)&&(openActivity->begin_time != 0) ) || ( (dwCondTime > openActivity->end_time)&&(openActivity->end_time != 0) ) )
			{
				role->mapCompleteActivityIDs.erase( it++ );
				continue;
			}
		}
		else
		{
			if( ( (dwCondTime < openActivity->reward_begin_time)&&(openActivity->reward_begin_time != 0) ) || ( (dwCondTime > openActivity->reward_end_time)&&(openActivity->reward_end_time != 0) ) )
			{
				role->mapCompleteActivityIDs.erase( it++ );
				continue;
			}

		}

		it++;

	}
}

void TASK::clearRepeatOpenActivityDaily( ROLE* role )
{
	//重复活动的删掉
	for ( auto it = role->mapCompleteActivityIDs.begin(); it != role->mapCompleteActivityIDs.end(); )
	{
		CONFIG::OPEN_ACTIVITY* openActivity =  CONFIG::getOpenActivityCfg( it->first );
		if ( openActivity == NULL )
		{
			role->mapCompleteActivityIDs.erase( it++ );
		}
		else if( openActivity->repeat == 1 )
		{
			role->mapCompleteActivityIDs.erase( it++ );
		}
		else
		{
			it++;
		}
	}
}

void TASK::getOpenActivity( ROLE* role )
{
	DWORD loginDay = getDayFromLogin(role);//第几天登陆

	for (auto &it : mapOpenActivityCfg)
	{
		auto& activity = it.second;

		COND_CONTINUE( role->mapCompleteActivityIDs.find( activity.id ) != role->mapCompleteActivityIDs.end() );

		if ( activity.type == 1 || activity.type == 2  || activity.type == 3 )
		{
			COND_CONTINUE( (loginDay<activity.begin_time&&activity.begin_time!=0) || (loginDay>activity.end_time&&activity.end_time!=0)  );//不在时间内

			role->mapCompleteActivityIDs.insert( make_pair(activity.id, MAIN_TASK_INFO(activity.id)));
		}
	}
}

void TASK::autoUnlockOpenActivitiyIDs( ROLE* role )
{
	clearTimeoutOpenActivity( role );

	getOpenActivity( role );
}



int TASK::clientGetOpenActivitiyIDs( ROLE* role, unsigned char * data, size_t dataLen )
{
//#define	    S_GET_OPEN_ACTIVITY		0x0930		//get open activity list
//C:[ ]
//S:[BYTE, BYTE, DWORD]n个
//	  [actType, actState, para]
//actType:活动类型
//			1:登录奖励
//actState:活动状态:
//		//0无，
//		//1有效，
//para:剩余时间
	TASK::autoUnlockOpenActivitiyIDs( role );

	RoleMgr::getMe().judgeCompleteTypeOpenActivity(role, E_ACTIVITY_LOGIN);

	DWORD loginDay = getDayFromLogin(role);//第几天登陆

	string strData;

	map<BYTE,TOpenActivityType> mapTopenType;

	for ( auto it = role->mapCompleteActivityIDs.begin(); it != role->mapCompleteActivityIDs.end(); it++ )
	{
		CONFIG::OPEN_ACTIVITY* openActivity =  CONFIG::getOpenActivityCfg( it->first );

		COND_CONTINUE( openActivity == NULL );

		DWORD time = openActivity->end_time;
		
		REDUCEUNSIGNED(time, (int)loginDay);

		if ( it->second.byTaskStatus != E_TASK_COMPLETE_ALREADY_GET_PRIZE )
		{
			TOpenActivityType openAct(openActivity->type, 1, time);
			mapTopenType[openActivity->type] = openAct;
		}
		if ( it->second.byTaskStatus == E_TASK_COMPLETE_ALREADY_GET_PRIZE )
		{
			TOpenActivityType openAct(openActivity->type, 0, time);
			mapTopenType[openActivity->type] = openAct;
		}
	}

	for (auto& it : mapTopenType)
	{
		S_APPEND_NBYTES( strData, (char*)&it, sizeof(TOpenActivityType) );
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_OPEN_REWARD_LIST, strData) );

	return 0;
}

int TASK::clientGetOpenActivitiyInfo( ROLE* role, unsigned char * data, size_t dataLen )
{
//#define	    S_GET_OPEN_ACTIVITY_INFO		0x0932		//get open activity list
//C:[BYTE]
//	[actType]
//actType:活动类型, 同上
//S:[[WORD, BYTE]]
//	[[actIdx, actState]]
//	[
//actIdx:活动index, 同上
//actState:活动状态:
//		//0未完成，
//		//1已完成未领奖，
//		//2已完成已领奖	
//	]

	BYTE byType;

	RETURN_COND( !BASE::parseBYTE( data, dataLen, byType ), 0 );

	string strData;

	for ( auto it = role->mapCompleteActivityIDs.begin(); it != role->mapCompleteActivityIDs.end(); it++ )
	{
		CONFIG::OPEN_ACTIVITY* openActivity =  CONFIG::getOpenActivityCfg( it->first );

		COND_CONTINUE( openActivity == NULL );

		COND_CONTINUE( openActivity->type != byType );

		if( openActivity->type == 1 )//wm待用
		{
			S_APPEND_WORD( strData, it->second.wTaskID );
			S_APPEND_BYTE( strData, it->second.byTaskStatus );
		}
		else if ( openActivity->type == 3 )//wm待用
		{

		}
		else if ( openActivity->type == 2 )
		{

		}
	}


	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_OPEN_ACTIVITY_INFO, strData) );
	return 0;

}

BYTE TASK::isCanActivityReward( const ROLE* role )
{
	for ( auto it = role->mapCompleteActivityIDs.begin(); it != role->mapCompleteActivityIDs.end(); it++ )
	{
		RETURN_COND(  it->second.byTaskStatus == E_TASK_COMPLETE_CAN_GET_PRIZE, 1 );
	}

	return 0;
}
int TASK::clientGetOpenActivitiyIDsReward( ROLE* role, unsigned char * data, size_t dataLen )
{
//#define	    S_GET_OPEN_REWARD_GIFTS		0x0931		//get open activity reward
//C:[WORD]
//	[actIdx]
//
//S:[BYTE]
//	返回码
//		TASK_SUCCESS = 0,//成功
//		ITEM_FULL = 1,//背包满
//		TASK_ERROR_ID = 2,//不存在的物品
//		NOT_ENOUGH_LEVEL = 3,//等级不足
//		NOT_COMPELETE = 4,//未完成
//		ALREADY_GET = 5,//已经领取
	WORD wTaskID;

	RETURN_COND( !BASE::parseWORD( data, dataLen, wTaskID ), 0 );

	string strData;

	BYTE byRet = TASK_SUCCESS;

	DWORD loginDay = getDayFromLogin(role);//第几天登陆

	do 
	{
		CONFIG::OPEN_ACTIVITY* openActivity =  CONFIG::getOpenActivityCfg( wTaskID );

		COND_BREAK(openActivity==NULL, byRet, TASK_ERROR_ID );

		auto it = role->mapCompleteActivityIDs.find( wTaskID );

		COND_BREAK(  it == role->mapCompleteActivityIDs.end(), byRet, TASK_ERROR_ID  );

		COND_BREAK(  it->second.byTaskStatus == E_TASK_UNCOMPLETE, byRet, NOT_COMPELETE  );

		COND_BREAK(  it->second.byTaskStatus == E_TASK_COMPLETE_ALREADY_GET_PRIZE, byRet, ALREADY_GET  );

		const CONFIG::OPEN_REWARD& stReward = openActivity->stReward; 

		COND_BREAK(!MailCtrl::makeMailItems( role, stReward.vecItems ), byRet, ITEM_FULL );

		ADDUP( role->dwIngot, (int)stReward.gold );
		notifyIngot(role);

		ADDUP( role->dwCoin, (int)stReward.coin );
		notifyCoin(role);
		logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID, -(int)stReward.coin, -(int)stReward.gold, purpose::opening_activity_reward, 0);

		it->second.byTaskStatus = E_TASK_COMPLETE_ALREADY_GET_PRIZE;

	} while (0);
	
	S_APPEND_BYTE( strData, byRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_OPEN_REWARD_GIFTS, strData) );
	return 0;

}


WORD TASK::getGroupByID( const WORD& wTaskID)
{
	CONFIG::MISSION_CFG* taskCfg = CONFIG::getMissionCfg( wTaskID );
	if (taskCfg != NULL)
	{
		return taskCfg->group;
	}
	return 0;
}

WORD TASK::getLevelByID( const WORD& wTaskID)
{
	CONFIG::MISSION_CFG* taskCfg = CONFIG::getMissionCfg( wTaskID );
	if (taskCfg != NULL)
	{
		return taskCfg->level_limit;
	}
	return 0;
}

/*
@每日任务重置
*/
void TASK::dailySetTask( ROLE* role )
{
	for ( auto it = role->mapTask.begin(); it != role->mapTask.end();  )//等级没到先放进去了,配置表里没有的删掉
	{
		CONFIG::MISSION_CFG* taskCfg = CONFIG::getMissionCfg( it->second.wTaskID );
		if (taskCfg == NULL)
		{
			role->mapTask.erase( it++ );
		}
		else
		{
			if ( taskCfg->repeat == 1)
			{
				it->second.byTaskStatus = E_TASK_UNCOMPLETE;
				it->second.byTaskCompleteNums = 0;

				notifyRoleTaskStatus( role, taskCfg, 1 );
			}
			
			++it;
		}
	}
}


/*
@任务解锁： 建号 领取(先解锁后改)
*/
void TASK::autoUnlockTask( ROLE* role, WORD wCompeleteID)
{
	for (auto &it : mapMissionCfg)//等级没到先放进去,新加的会添加给用户
	{
		auto& task = it.second;

		if ( ( role->mapTask.find( task.group ) == role->mapTask.end() ) && ( wCompeleteID == task.front_id ) )//default level is 0
		{
			role->mapTask.insert( make_pair(task.group, MAIN_TASK_INFO(task.id)));

			notifyRoleTaskStatus( role, &task, 1 );
		}
	}
}



/*
@S_GET_TASK_LIST=0x0910  --获取任务列表
*/
int TASK::clientGetTasksList( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;

	for ( auto& it : role->mapTask )
	{
		auto& task = it.second;
		//logwm("taskid = %u, group=%u",task.wTaskID,it.first);
		WORD level = getLevelByID( task.wTaskID );
		if (  level == 0 || level > role->wLevel)
		{
			continue;
		}
		if ( task.byTaskStatus == E_TASK_UNCOMPLETE || task.byTaskStatus == E_TASK_COMPLETE_CAN_GET_PRIZE )
		{
			//logwm("roleid = %u true taskid = %u  %u  %u",role->dwRoleID, task.wTaskID, task.byTaskCompleteNums, task.byTaskStatus);
			S_APPEND_NBYTES( strData, &task, sizeof(task) );
		}
	}
	
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_TASK_LIST, strData) );
	return 0;
}

BYTE TASK::isCanTaskReward( const ROLE* role )
{
	for ( auto& task : role->mapTask )
	{
		RETURN_COND(  task.second.byTaskStatus == E_TASK_COMPLETE_CAN_GET_PRIZE, 1 );
	}

	return 0;
}

/*
@S_GET_TASK_GIFTS=0x0911 --获取任务奖励
*/
int TASK::clientGetTaskReward( ROLE* role, unsigned char * data, size_t dataLen )
{
	WORD wTaskID;
	if ( !BASE::parseWORD( data, dataLen, wTaskID ) )
		return 0;

	BYTE byRet = TASK_SUCCESS;

	string strData;

	do 
	{
		CONFIG::MISSION_CFG* taskCfg = CONFIG::getMissionCfg(wTaskID);

		COND_BREAK( nullptr == taskCfg, byRet, TASK_ERROR_ID );

		auto it = role->mapTask.find( taskCfg->group );

		COND_BREAK( it == role->mapTask.end(), byRet, TASK_ERROR_ID );

		auto& task = it->second;

		COND_BREAK( task.wTaskID != wTaskID, byRet, TASK_ERROR_ID );

		COND_BREAK( role->wLevel < taskCfg->level_limit, byRet, NOT_ENOUGH_LEVEL );

		COND_BREAK( task.byTaskStatus == E_TASK_UNCOMPLETE, byRet, NOT_ENOUGH_LEVEL );

		COND_BREAK( task.byTaskStatus == E_TASK_COMPLETE_ALREADY_GET_PRIZE, byRet, ALREADY_GET );

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( taskCfg->itemID );
		if ( itemCfg!=NULL )//可能没有物品奖励
		{
			COND_BREAK(ItemCtrl::makeOneTypeItem(role, itemCfg, taskCfg->itemNum) == -1, byRet, ITEM_FULL);	
		}

		autoUnlockTask( role, wTaskID);

		CONFIG::MISSION_CFG* nextTaskCfg = CONFIG::getMissionCfg( wTaskID+1 );

		if (nextTaskCfg != NULL && nextTaskCfg->group == taskCfg->group)
		{
			task = MAIN_TASK_INFO( wTaskID+1 );
		}
		else
		{
			task.byTaskStatus = E_TASK_COMPLETE_ALREADY_GET_PRIZE;
		}

		if ( taskCfg->ingot > 0 )//1元宝
		{
			ADDUP(role->dwIngot, (int)taskCfg->ingot);
			notifyIngot(role);
		}
		if( taskCfg->coin > 0 )//2铜钱
		{
			ADDUP(role->dwCoin, (int)taskCfg->coin);
			notifyCoin(role);
		}

		if(taskCfg->exp > 0)//3经验
		{
			WORD wOldLevel = role->wLevel;
			CONFIG::addRoleExp( role, role->dwExp, taskCfg->exp );
			notifyExp(role);
		}

		if ( taskCfg->type == E_TASK_LEVEL )
		{
			RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_LEVEL);//升级触发点
		}

		if ( taskCfg->type == E_TASK_VIP_LOGIN )
		{
			ITEM_INFO para1(0, role->getVipLevel());
			RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_VIP_LOGIN, &para1);
		}

		if ( taskCfg->type == E_TASK_SET_FIGHT_SKILL_NUM )
		{
			role->role_skill.judgeSetSkillTask();
		}

		if( taskCfg->type == E_TASK_FIGHT_BOSS )
		{
			ADDUP3( role->byAllBuyBossTimes, 1, BYTE_MAX );
		}

		logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID,
			(int)taskCfg->coin, (int)taskCfg->ingot, purpose::task_reward, 0);

	} while (0);
	S_APPEND_BYTE( strData, byRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_TASK_GIFTS, strData) );
	return 0;

#if 0
	if (taskCfg != NULL)
	{
		auto it = role->mapTask.find( taskCfg->group );
		if ( it != role->mapTask.end())
		{
			auto& task = it->second;

			if(task.wTaskID == wTaskID )
			{
				if (role->wLevel >= taskCfg->level_limit)
				{
					if (task.byTaskStatus == E_TASK_UNCOMPLETE)
					{
						byRet = NOT_COMPELETE;
					}
					else if (task.byTaskStatus == E_TASK_COMPLETE_ALREADY_GET_PRIZE)
					{
						byRet = ALREADY_GET;
					}
					else
					{
						byRet = TASK_SUCCESS;
						if (taskCfg->itemID != 0)
						{
							CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( taskCfg->itemID );
							if ( itemCfg!=NULL )
							{
								if (ItemCtrl::makeOneTypeItem(role, itemCfg, taskCfg->itemNum) == -1)
								{
									byRet = ITEM_FULL;
								}		
							}
						}

						if (byRet != ITEM_FULL)
						{
							autoUnlockTask( role, wTaskID);

							nextTaskCfg = CONFIG::getMissionCfg( wTaskID+1 );

							if (nextTaskCfg != NULL && nextTaskCfg->group == taskCfg->group)
							{
								task = MAIN_TASK_INFO( wTaskID+1 );
							}
							else
							{
								task.byTaskStatus = E_TASK_COMPLETE_ALREADY_GET_PRIZE;

								//if (taskCfg->repeat == 1)
								//{
								//	task.byTaskStatus = E_TASK_COMPLETE_ALREADY_GET_PRIZE;
								//}
								//else
								//{
								//	role->mapTask.erase(it);
								//}
							}

							if ( taskCfg->ingot > 0 )//1元宝
							{
								ADDUP(role->dwIngot, (int)taskCfg->ingot);
								notifyIngot(role);
							}
							if(taskCfg->coin > 0)//2铜钱
							{
								ADDUP(role->dwCoin, (int)taskCfg->coin);
								notifyCoin(role);
							}
							logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID,
								(int)taskCfg->coin, (int)taskCfg->ingot, purpose::task_reward, 0);
							if(taskCfg->exp > 0)//3经验
							{
								WORD wOldLevel = role->wLevel;
								CONFIG::addRoleExp( role, role->dwExp, taskCfg->exp );
								notifyExp(role);
							}

							if ( taskCfg->type == E_TASK_LEVEL )
							{
								RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_LEVEL);//升级触发点
							}

							if ( taskCfg->type == E_TASK_VIP_LOGIN )
							{
								ITEM_INFO para1(0, role->getVipLevel());
								RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_VIP_LOGIN, &para1);
							}

							if ( taskCfg->type == E_TASK_SET_FIGHT_SKILL_NUM )
							{
								role->role_skill.judgeSetSkillTask();
							}

							if( taskCfg->type == E_TASK_FIGHT_BOSS )
							{
								ADDUP3( role->byAllBuyBossTimes, 1, BYTE_MAX );
							}
						}
					}
				}
				else
				{
					byRet = NOT_ENOUGH_LEVEL;
				}
			}
		}
	}
	
	S_APPEND_BYTE( strData, byRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_TASK_GIFTS, strData) );
	return 0;
#endif
}




void TASK::notifyRoleTaskStatus( ROLE* role, CONFIG::MISSION_CFG* taskCfg, const BYTE& status )
{
	if (role==NULL || taskCfg==NULL || role->wLevel < taskCfg->level_limit)
	{
		return;
	}

	notifyTaskStatus( role, status );
}

void TASK::notifyTaskStatus( ROLE* role, const BYTE& status )
{
	string strData;
	S_APPEND_BYTE( strData, 4 );//任务模块
	S_APPEND_BYTE( strData, status );//该活动状态通知
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FUNCTION_NOTIFY, strData) );
}

void TASK::loginNotifyRoleTaskStatus( ROLE* role )
{
	if ( role==NULL )
	{
		return;
	}

	for ( auto& it : role->mapTask )
	{
		if ( it.second.byTaskStatus == E_TASK_COMPLETE_CAN_GET_PRIZE )
		{
			notifyTaskStatus( role, E_TASK_COMPLETE_CAN_GET_PRIZE );
			break;
		}
		if ( it.second.byTaskStatus == E_TASK_COMPLETE_ALREADY_GET_PRIZE )
		{
			notifyTaskStatus( role, E_TASK_COMPLETE_ALREADY_GET_PRIZE );
			break;
		}
	}
}