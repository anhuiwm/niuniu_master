#ifndef QUICKFIGHT_H
#define QUICKFIGHT_H

#include "protocal.h"
#include "config.h"

namespace QUICKFIGHT
{
	enum E_QUICKFIGHT
	{
		REPORT_SUCCESS = 0,//成功
		REPORT_NO_INGOT = 1,//钱不够
		REPORT_NOT_VIP = 2,//VIP不够
		REPORT_NOT_TIMES = 3,//次数不够
		REPORT_ERROR_MAP = 4,//地图错误
		REPORT_ITEM_FULL = 5,//背包已经满
		REPORT_ERROR = 6,//
	};

#pragma pack(push, 1)
	struct  QUICK_REPORT
	{
		WORD wFightNum;
		DWORD dwTime;
		WORD wWinFightNum;
		WORD wLoseFightNum;
		DWORD dwGetExp;
		DWORD dwGetCoin;
		WORD wDropAllNum;
		WORD vvColorSellNum[7][2];
		WORD wChestsNum;
		WORD wAutoSellChestsNum;
		ZERO_CONS(QUICK_REPORT);
	};
#pragma pack(pop)


	int clientQuickFightReport( ROLE* role, unsigned char * data, size_t dataLen );
	int clientBuyQuickFight( ROLE* role, unsigned char * data, size_t dataLen );
	int clientBuyQuickFightCost( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGetQuickFightTimes( ROLE* role, unsigned char * data, size_t dataLen );
	//BYTE offlineEarning( ROLE* role, time_t tNow, string& strData );
	BYTE offlineEarning( ROLE* role, DWORD fightTime, list<ITEM_CACHE*>* pLstNotifyItem, list<ITEM_CACHE*>* pLstNotifyProps, QUICK_REPORT& stcReport );
	int clientOfflineFightReport( ROLE* role, unsigned char * data, size_t dataLen );

	int clientEliteInfo( ROLE* role, unsigned char * data, size_t dataLen );
	int clientSelectElitePass( ROLE* role, unsigned char * data, size_t dataLen );
	int clientBuyEliteTimes( ROLE* role, unsigned char * data, size_t dataLen );
	int clientRequestFightElite( ROLE* role, unsigned char * data, size_t dataLen );
	int clientQuickFightElite( ROLE* role, unsigned char * data, size_t dataLen );

	int clientQuickFightBoss(ROLE* role, unsigned char * data, size_t dataLen);
}

#endif//QUICKFIGHT_H
