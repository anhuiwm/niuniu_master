#include "quickfight.h"
#include "itemCtrl.h"
#include "roleCtrl.h"
#include "logs_db.h"
#include "service.h"


namespace QUICKFIGHT
{
	const static DWORD MIN_COOL_TIME = 20; 
	static WORD _Cost[COST_TIMES] = { 30,40,50,60,70,80,90,100,100,100 };
	static WORD _EliteCost[COST_TIMES] = { 50,100,150,200,250,300,350,400,450,500 };
}

//#define		S_BUY_QUICK_FIGHT 			0x0923//ȷ�Ϲ������ս���������
int QUICKFIGHT::clientBuyQuickFight( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byRet = REPORT_SUCCESS;//�ɹ�
	string strData;
	WORD index =  role->byBuyQuickFightTimes;
	DWORD dwCostGold = index <= (COST_TIMES - 1) ? _Cost[index] : _Cost[COST_TIMES - 1];
	if (role->dwIngot < dwCostGold)
	{
		byRet = REPORT_NO_INGOT;//Ǯ����
	}
	else
	{
		CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
		if (vipCfg != NULL && vipCfg->time_buy > role->byBuyQuickFightTimes)
		{
			ADDUP(role->dwIngot, -(int)dwCostGold );
			role->byBuyQuickFightTimes++;
			notifyIngot(role);
			logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID, 0, -(int)dwCostGold, purpose::buy_quick_fight, 0);
		}
		else
		{
			byRet  = REPORT_NOT_VIP ;//vip�ȼ�����
		}	
	}

	S_APPEND_BYTE(strData, byRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BUY_QUICK_FIGHT, strData) );
	return 0;
}

//#define		S_GET_QUICK_FIGHT_COST 		0x0924//��ȡ�������ս��������Ҫ���
int QUICKFIGHT::clientBuyQuickFightCost( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byNextRet = REPORT_SUCCESS;//�ɹ�
	string strData;
	WORD index =  role->byBuyQuickFightTimes;
	DWORD dwCostGold = ( index <= (COST_TIMES - 1) ) ? _Cost[index] : _Cost[COST_TIMES - 1];
	BYTE byLeftBuyTimes;
	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
	if (vipCfg != NULL && vipCfg->time_buy > role->byBuyQuickFightTimes)
	{
		byLeftBuyTimes = vipCfg->time_buy - role->byBuyQuickFightTimes;
	}
	else
	{
		byLeftBuyTimes = 0;
	}

	if (role->dwIngot < dwCostGold)
	{
		byNextRet = REPORT_NO_INGOT;
	}

	if (byLeftBuyTimes == 0)
	{
		byNextRet = REPORT_NOT_VIP;
	}
	
	S_APPEND_BYTE(strData, byLeftBuyTimes);
	S_APPEND_DWORD(strData, dwCostGold);
	S_APPEND_BYTE(strData, byNextRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_QUICK_FIGHT_COST, strData) );
	return 0;
}

//#define		S_GET_QUICK_FIGHT_TIMES		0x0925//��ȡ����ÿ���ս������
int QUICKFIGHT::clientGetQuickFightTimes( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;
	BYTE times =  role->byBuyQuickFightTimes;
	S_APPEND_BYTE(strData, times);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_QUICK_FIGHT_TIMES, strData) );
	return 0;
}

//#define		S_QUICK_FIGHT_REPORT		0x0921//����ս������
int QUICKFIGHT::clientQuickFightReport( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;
	BYTE byRet = REPORT_SUCCESS;//�ɹ�
	BYTE byLeftBuyTimes = 0;

	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
	if (vipCfg != NULL && vipCfg->time_buy > role->byBuyQuickFightTimes)
	{
		byLeftBuyTimes = vipCfg->time_buy - role->byBuyQuickFightTimes;
	}

	QUICK_REPORT stcReport;

	if ( byLeftBuyTimes != 0 )
	{

		WORD levelOrig = role->wLevel;
		DWORD expOrig = role->dwExp;
		DWORD coinOrig = role->dwCoin;

		DWORD dwFightTime = 2 * 60 * 60;

		dwFightTime += DWORD( float(dwFightTime) * float(vipCfg->battle_reward) / 100.0f );

		list<ITEM_CACHE*> lstNotifyItem, lstNotifyProps;
		
		byRet = QUICKFIGHT::offlineEarning( role, dwFightTime, &lstNotifyItem, &lstNotifyProps, stcReport );

		//notify_X_If( role, ROLE_PRO_LEVEL, wLevel, levelOrig );
		notify_X_If( role, ROLE_PRO_CUREXP, dwExp, expOrig );
		notify_X_If( role, ROLE_PRO_COIN, dwCoin, coinOrig );

		ItemCtrl::notifyClientItem(role, lstNotifyItem, PKG_TYPE_EQUIP);
		ItemCtrl::notifyClientItem(role, lstNotifyProps, PKG_TYPE_ITEM);

		WORD index =  role->byBuyQuickFightTimes;
		DWORD dwCostGold = ( index <= (COST_TIMES - 1) ) ? _Cost[index] : _Cost[COST_TIMES - 1];
		ADDUP(role->dwIngot, -(int)dwCostGold );
		notifyIngot(role);
		role->byBuyQuickFightTimes++;
		logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID, 0, -(int)dwCostGold, purpose::buy_quick_fight, 0);

		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_BUY_QUICK_FIGHT);
	}
	else
	{
		byRet = REPORT_NOT_TIMES;
	}

	S_APPEND_BYTE(strData, byRet);
	S_APPEND_NBYTES( strData, &stcReport, sizeof(stcReport) );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_QUICK_FIGHT_REPORT, strData) );
	return 0;
}


BYTE QUICKFIGHT::offlineEarning( ROLE* role, DWORD fightTime, list<ITEM_CACHE*>* pLstNotifyItem,  list<ITEM_CACHE*>* pLstNotifyProps,QUICK_REPORT& stcReport )
{
	CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(role->wLastFightMapID);
	if ( mapCfg == NULL )
		return REPORT_ERROR_MAP;
	
	fightTime = min<DWORD>( fightTime, 24*60*60 );

	//�����1������
	DWORD dwCoolTime = max<DWORD>(MIN_COOL_TIME, mapCfg->cool_down);

	DWORD dropTimes = fightTime / dwCoolTime;

	WORD levelOrig = role->wLevel;
	DWORD expOrig = role->dwExp;
	DWORD coinOrig = role->dwCoin;
	
	//DWORD tick = BASE::getTickCount();

	DWORD dwExpAdd = 0;
	DWORD dwCoinAdd = 0;

	 list<CONFIG::DROP_EARNING*> lstDropEarning;

	for ( DWORD i=0; i<dropTimes; i++)
	{
		CONFIG::DROP_EARNING* de = CONFIG::getRandMonDrop( role->wLastFightMapID );	
		if ( de == NULL )
			continue;
		
		dwExpAdd += de->dwExp;
		dwCoinAdd += de->dwCoin;
		if ( de->ic )
		{
			//����ֱ������,�õ�ͭǮ,û����ҪNOTIFY��
			role->m_packmgr.makeItem3( de->ic, pLstNotifyItem, pLstNotifyProps);
			lstDropEarning.push_back(de);
		}

	}

	WORD wDropSum = 0;
	WORD vvColorSell[ 7 ][2]={0};
	size_t vBaoxiang[2]={0};
	size_t equipSum =0;
	size_t baoxiangSum =0;
	for( auto & it : lstDropEarning )
	{
		ITEM_CACHE *ic = it->ic;
		
		if ( IS_NORMAL_EQUIP(ic->itemCfg) )
		{
			//byQuality:1-7
			if ( !! ic->byDropAndSell )
			{
				vvColorSell[ ic->byQuality-1 ][ 1 ]++;
			}
			vvColorSell[ ic->byQuality-1 ][ 0 ]++;
			//vvColorSell[ ic->byQuality-1 ][ !! ic->byDropAndSell ]++;
			equipSum++;
		}
		else if ( IS_BAOXIANG(ic->itemCfg) )
		{
			vBaoxiang[ !!ic->byDropAndSell ]++;
			baoxiangSum++;
		}

		if (ic)
		{
			wDropSum++;
		}
	}

	//����ֱ������,�õ�ͭǮ
	ADDUP( role->dwCoin, dwCoinAdd );
	logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID, dwCoinAdd, 0, purpose::offline_earning, 0);
	CONFIG::addRoleExp( role, role->dwExp, dwExpAdd, true );

	DWORD dwExpGet = CONFIG::getExpDelta( levelOrig, role->wLevel, expOrig, role->dwExp );
	DWORD dwCoinGet = role->dwCoin - coinOrig;

	stcReport.wFightNum = (WORD)dropTimes;
	stcReport.dwTime = fightTime;
	stcReport.wWinFightNum = (WORD)dropTimes;
	stcReport.wLoseFightNum = 0;
	stcReport.dwGetExp = dwExpGet;
	stcReport.dwGetCoin = dwCoinGet;
	stcReport.wDropAllNum = wDropSum;
	
	memcpy( stcReport.vvColorSellNum,  vvColorSell, sizeof(vvColorSell) );
	
	stcReport.wChestsNum = baoxiangSum;
	stcReport.wAutoSellChestsNum = vBaoxiang[1];

	//logff("XXX: droptime:%u, tick:%u, roleid:%u", dropTimes, BASE::getTickCount() - tick, role->dwRoleID );

	if (role->m_packmgr.lstEquipPkg.size() >= role->m_packmgr.curEquipPkgLimit)
	{		return REPORT_ITEM_FULL;
	}
	
	return REPORT_SUCCESS;
}




//#define		S_OFFLINE_FIGHT_REPORT		0x0920//����ս������
int QUICKFIGHT::clientOfflineFightReport( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byType;
	if ( !BASE::parseBYTE( data, dataLen, byType) )
		return 0;
	string strData;
	DWORD tNow = (DWORD)PROTOCAL::getCurTime();
	BYTE byRet = REPORT_SUCCESS;//�ɹ�
	if (byType == 0)//�ж�����ʱ���Ƿ񳬹�2����
	{
		if ( tNow <= role->tLogoutTime + 120 )
		{
			byRet = REPORT_ERROR;
		}

		S_APPEND_BYTE(strData, byRet);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_OFFLINE_FIGHT_REPORT, strData) );
	}
	else if (byType == 1)//����ս������
	{
		if ( tNow <= role->tLogoutTime )
		{
			return 0;
		}

		WORD levelOrig = role->wLevel;
		DWORD expOrig = role->dwExp;
		DWORD coinOrig = role->dwCoin;
		DWORD ingotOrig = role->dwIngot;

		list<ITEM_CACHE*> lstNotifyItem, lstNotifyProps;

		QUICK_REPORT stcReport;
		//����Ҫnotify item
		byRet = QUICKFIGHT::offlineEarning( role, tNow-role->tLogoutTime, &lstNotifyItem, &lstNotifyProps, stcReport );
		S_APPEND_BYTE(strData, byRet);
		S_APPEND_NBYTES( strData, &stcReport, sizeof(stcReport) );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_QUICK_FIGHT_REPORT, strData) );

		notify_X_If( role, ROLE_PRO_CUREXP, dwExp, expOrig );
		notify_X_If( role, ROLE_PRO_COIN, dwCoin, coinOrig );
		notify_X_If( role, ROLE_PRO_INGOT, dwIngot, ingotOrig );
		ItemCtrl::notifyClientItem(role, lstNotifyItem, PKG_TYPE_EQUIP);
		ItemCtrl::notifyClientItem(role, lstNotifyProps, PKG_TYPE_ITEM);
	}
	return 0;
}



int QUICKFIGHT::clientEliteInfo( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//��Ӣ�ؿ���Ϣ����
//#define	S_REQUEST_ELITE_INFO	0x0057
//C:[]
//S:[[BYTE]]
//	[[curOpendIdx]]  //����������Ԥ��20��?
//curOpendIdx:ÿһ�������п��ŵ�������� (1-9)

	string strData;

	if ( role->mapElite.empty() )
	{
		role->resetEliteMap();
	}
	
	for ( auto& it : role->mapElite )
	{
		CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(it.second.wBase);

		COND_CONTINUE( mapCfg == NULL );

		COND_CONTINUE( mapCfg->level_limit > role->wLevel );//�ȼ�����

		BYTE byIndex = mapCfg->sub_order;

		S_APPEND_BYTE(strData, (BYTE)mapCfg->group_id);

		S_APPEND_BYTE(strData, byIndex);
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_ELITE_INFO, strData) );

	return 0;
}

int QUICKFIGHT::clientSelectElitePass( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//ѡ����ս�ؿ�
//#define	S_REQUEST_SELECTED_ELITE_INFO	0x0058
//C:[]
//S:[BYTE, BYTE, WORD]
//	[remain, curTime, perCost]
//remain:vipʣ�๺�����
//curTime:��ǰӵ�д���
//perCost:����۸�

	string strData;
	BYTE byRemain = 0;
	BYTE byCurTime = 0;
	WORD wPerCost = 0;
	WORD index =  role->byDailyBuyHeroTimes;

	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
	if (vipCfg != NULL && vipCfg->elite_time > index)
	{
		byRemain = vipCfg->elite_time - index;
	}

	byCurTime = role->bySendFightHeroTimes + role->byAllBuyHeroTimes;//wm���������ҪBYTE�������   ÿ��������Ҫ�������͵Ĵ���

	DWORD dwCostGold = index <= (COST_TIMES - 1) ? _EliteCost[index] : _EliteCost[COST_TIMES - 1];

	wPerCost = WORD(dwCostGold);

	S_APPEND_BYTE(strData, byRemain);
	S_APPEND_BYTE(strData, byCurTime);
	S_APPEND_WORD(strData, wPerCost);

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_SELECTED_ELITE_INFO, strData) );
	return 0;
}

int QUICKFIGHT::clientBuyEliteTimes( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//������ս����
//#define	S_BUY_CHALLENGE_TIME	0x0059
//C:[]
//S:[WORD, BYTE, BYTE, WORD]
//	[error, remain, curTime, nextCost]
//error:0:�ɹ�
//		  1:ʣ�๺���������
//		  2:Ԫ������
//remain:vipʣ�๺�����
//curTime:��ǰӵ�д���
//nextCost:�´ι���۸�

	WORD wRet = 0;//�ɹ�
	BYTE byRemain = 0;
	BYTE byCurTime = 0;
	string strData;
	WORD index =  role->byDailyBuyHeroTimes;
	DWORD dwCostGold = index <= (COST_TIMES - 1) ? _EliteCost[index] : _EliteCost[COST_TIMES - 1];
	DWORD dwNextCost = (index+1) <= (COST_TIMES - 1) ? _EliteCost[index+1] : _EliteCost[COST_TIMES - 1];
	if (role->dwIngot < dwCostGold)
	{
		wRet = 2;//Ǯ����
	}
	else
	{
		CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
		if (vipCfg != NULL && vipCfg->elite_time > index)
		{
			ADDUP(role->dwIngot, -(int)dwCostGold );
			ADDUP3(role->byAllBuyHeroTimes, 1, BYTE_MAX);
			ADDUP3(role->byDailyBuyHeroTimes, 1, BYTE_MAX);
			byRemain = vipCfg->elite_time - role->byDailyBuyHeroTimes;
			notifyIngot(role);
			logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID, 0, -(int)dwCostGold, purpose::buy_elite_time, 0);
		}
		else
		{
			wRet  = 1 ;//vip�ȼ�����
		}	
	}
	byCurTime = role->bySendFightHeroTimes + role->byAllBuyHeroTimes;
	S_APPEND_WORD(strData, wRet);
	S_APPEND_BYTE(strData, byRemain);
	S_APPEND_BYTE(strData, byCurTime);
	S_APPEND_WORD(strData, WORD(dwNextCost) );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BUY_CHALLENGE_TIME, strData) );
	return 0;
}

int QUICKFIGHT::clientRequestFightElite( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//������ս�ؿ�
//#define	S_ELITE_CHALLENGE	0x005A
//C:[WORD]
//	[eliteId]
//eliteId:��Ӣ�ؿ�idx
//S:[WORD]
//	[error]
//error:������
//		  0:�ɹ�
//		  1:δ����
//		  2:��������

	WORD wEliteID;

	RETURN_COND( !BASE::parseWORD( data, dataLen, wEliteID), 0 );

	WORD wRet = 1;

	string strData;

	if ( role->bySendFightHeroTimes + role->byAllBuyHeroTimes == 0 )
	{
		wRet = 2;
	}
	else
	{
		CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(wEliteID);
		if ( mapCfg != NULL )
		{
			auto it = role->mapElite.find( mapCfg->group_id );
			if ( it !=role->mapElite.end() && wEliteID <= it->second.wBase )
			{
				wRet = 0;
			}
		}
	}

	S_APPEND_WORD(strData, wRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ELITE_CHALLENGE, strData) );
	return 0;

}


int QUICKFIGHT::clientQuickFightElite( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//����ɨ��
//#define	S_QUICK_CHALLENGE	0x005B
//C:[WORD]
//	[eliteId]
//eliteId:��Ӣ�ؿ�idx
//S:[WORD]
//	[error]
//error:������
//		  0:�ɹ�
//		  1:δ����
//		  2:��������
//		  3:�����һ����ս
//		  4:װ��������
//		  5:���߱�����
	WORD wEliteID;

	RETURN_COND( !BASE::parseWORD( data, dataLen, wEliteID), 0 );

	RETURN_COND( !IS_ELITE_MAP( wEliteID ), 0 );

	WORD wRet = 0;

	string strData;

	S_APPEND_WORD(strData, 0);

	if ( role->bySendFightHeroTimes + role->byAllBuyHeroTimes == 0 )
	{
		wRet = 2;
	}
	else
	{
		CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(wEliteID);
		if ( mapCfg != NULL && mapCfg->level_limit <= role->wLevel )
		{
			auto it = role->mapElite.find( mapCfg->group_id );
			if ( it == role->mapElite.end() || wEliteID > it->second.wBase )
			{
				wRet = 1;
			}
			else if ( wEliteID > it->second.wExtra )
			{
				wRet = 3;
			}
			else
			{
				if ( role->bySendFightHeroTimes > 0 )
				{
					role->bySendFightHeroTimes--;
				}
				else if ( role->byAllBuyHeroTimes > 0 )
				{
					role->byAllBuyHeroTimes--;
				}

				list<ITEM_CACHE*> lstEquipNotify, lstPropsNotify, lstAutoSellItem, lstNoSellItem;

				ItemCtrl::killMapMonster( role, wEliteID, E_DROP_BOSS, &lstEquipNotify, &lstPropsNotify, &lstAutoSellItem, &lstNoSellItem );

				if ( role->m_packmgr.lstEquipPkg.size() >= role->m_packmgr.curEquipPkgLimit )//������ǰ����Ȼ���ͻ���
				{
					wRet = 4;
				}

				if ( role->m_packmgr.lstItemPkg.size() >= role->m_packmgr.curItemPkgLimit )
				{
					wRet = 5;
				}

				for ( auto it : lstAutoSellItem )//�Զ�����
				{
					CC_SAFE_DELETE(it);
				}
				for ( auto it : lstNoSellItem )
				{
					//S_APPEND_DWORD( strData, it );
					S_APPEND_WORD( strData, it->itemCfg->id );
					S_APPEND_WORD( strData, it->byQuality );
					CC_SAFE_DELETE(it);
				}

				//��Ҫ��notity
				ItemCtrl::notifyClientItem(role, lstEquipNotify, PKG_TYPE_EQUIP);
				ItemCtrl::notifyClientItem(role, lstPropsNotify, PKG_TYPE_ITEM);
			}

		}
	}

	*(WORD *)&strData[0] = wRet;
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_QUICK_CHALLENGE, strData) );
	return 0;
}