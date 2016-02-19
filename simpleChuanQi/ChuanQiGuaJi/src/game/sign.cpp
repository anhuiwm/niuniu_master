#include "sign.h"
#include "itemCtrl.h"
#include "role.h"

BYTE SIGN::getMonthBytime(time_t now)
{
	struct tm tmNow;
	LOCAL_TIME(now, tmNow);
	return BYTE(tmNow.tm_mon + 1);
}


BYTE SIGN::getMonth()
{
	time_t now  =  PROTOCAL::getCurTime();
	struct tm tmNow;
	LOCAL_TIME(now, tmNow);
	return BYTE(tmNow.tm_mon + 1);
}

BYTE SIGN::getDayByTime(time_t now)
{
	struct tm tmNow;
	LOCAL_TIME(now, tmNow);
	return BYTE(tmNow.tm_mday);
}

BYTE SIGN::getDay()
{
	time_t now  =  PROTOCAL::getCurTime();
	struct tm tmNow;
	LOCAL_TIME(now, tmNow);
	return BYTE(tmNow.tm_mday);
}

/*
@ #define S_GET_SIGN_TIMES   0x0600 //get sign info
*/
int SIGN::getSignInfo( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;
	S_APPEND_BYTE( strData, getMonth() );
	S_APPEND_BYTE( strData, role->bySignTimes );
	//S_APPEND_BYTE( strData, role->byTimeSignStatus );
	S_APPEND_DWORD( strData, role->dwDaySignStatus );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_SIGN_INFO, strData) );
	return 0;
}

/*
@ #define S_CLIENT_SIGN 0x0601 //client sign
*/
int SIGN::clientSign( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;

	BYTE byDay = getDay();

	BYTE byOldDay = byDay;

	REDUCEUNSIGNED(byDay, 1);

	BYTE byMonth = getMonth();

	BYTE byCode = SIGN_SUCCESS;
	
	DWORD dwCode = 0x00000001;

	DWORD dwDayCode = dwCode << byDay;//今天

   // DWORD dwTimeCode = dwCode << (byDay-1) ;//昨天

	if( dwDayCode & role->dwDaySignStatus )//已经领取
	{
		byCode =  SIGN_ALREADY;
		S_APPEND_BYTE( strData, byCode );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_CLIENT_SIGN, strData) );
		return 0;
	}

	role->dwDaySignStatus = role->dwDaySignStatus | dwDayCode;

	S_APPEND_BYTE( strData, byCode );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_CLIENT_SIGN, strData) );

	//if( dwTimeCode & role->dwDaySignStatus )//连续领取
	{
		role->bySignTimes == CONFIG::getMaxContinueSignTimes() ? 1 : role->bySignTimes++;

		CONFIG::SIGN_CFG *signTimeCfg = CONFIG::getSignTimeCfg( role->bySignTimes );
		if ( signTimeCfg != NULL )
		{
			//role->byTimeSignStatus = role->byTimeSignStatus | ( (BYTE)dwCode << signTimeCfg->times );
			CONFIG::ITEM_CFG* itemTimeCfg = CONFIG::getItemCfg( signTimeCfg->item_id );
			if ( itemTimeCfg != NULL )
			{
				ItemCtrl::makeOneTypeItem(role, itemTimeCfg, 1);
			}
		}
	}

	{
		CONFIG::SIGN_CFG *signCfg = getSignCfg4( role->byJob, role->bySex, byMonth, byOldDay );
		RETURN_COND( signCfg == NULL, 0 );

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( signCfg->item_id );
		RETURN_COND( itemCfg==NULL, 0 )

		ItemCtrl::makeOneTypeItem(role, itemCfg, signCfg->item_num);

	}

	return 0;
}