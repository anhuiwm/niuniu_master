#ifndef SIGN_H
#define SIGN_H

#include "protocal.h"
#include "config.h"

namespace SIGN
{
	enum eSIGN
	{
		SIGN_SUCCESS = 0,     		//成功
		SIGN_ITEM_NO_EXISTS = 1,    //物品不存在
		SIGN_ITEM_ERROR = 2,        //领取物品错误
		SIGN_ALREADY = 3,           //今日已签到已经领取
		SIGN_TIMES_NOT_ENOUGH = 4,  //次数未到
	};

	enum eSIGNSTATU
	{
		NO_SIGN_TODAY = 0,     		//今日未签到
		SIGN_TODAY = 1,			//今日已签到
	};

	int getSignInfo( ROLE* role, unsigned char * data, size_t dataLen );

	int clientSign( ROLE* role, unsigned char * data, size_t dataLen );

	BYTE getMonth();

	BYTE getDay();

	BYTE getDayByTime(time_t now);

	BYTE getMonthBytime(time_t now);
}


#endif //SIGN_H
