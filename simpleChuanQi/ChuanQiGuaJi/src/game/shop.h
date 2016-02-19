#ifndef SHOP_H
#define SHOP_H
#include "protocal.h"
#include "config.h"

#define  JEWELLERY_SHOP_BASE_NUM   3

#define  JEWELLERY_SHOP_FULL_NUM   6

class CMargetMystery 
{

public:
	CMargetMystery();

	int ClientGetCoodsList( ROLE* role, unsigned char * data, size_t dataLen );

	int ClientBuyCoods( ROLE* role, unsigned char * data, size_t dataLen );

	void resetFreshTime();

//private:

	MYSTERY_SHOP_INFO m_vMysInfo[MYSTERY_SHOP_NUM];//两个商人的信息

//	friend ROLE;
};


class CMargetJewellery
{

public:
	CMargetJewellery( );

	void InitShopItem(ROLE* role, bool bUseFresh = false, bool bPush = false );

	int ClientGetCoodsList( ROLE* role, unsigned char * data, size_t dataLen );

	int ClientBuyCoods( ROLE* role, unsigned char * data, size_t dataLen );

	int clientBuyAllCood( ROLE* role, unsigned char * data, size_t dataLen );

	int clientRequestUpdateCost( ROLE* role, unsigned char * data, size_t dataLen );

	int clientRequestUpdate( ROLE* role, unsigned char * data, size_t dataLen );

	void resetFreshTime();

	void addFreshTime();

	DWORD getFreshTime();

	void setFreshTime( const DWORD time);

//private:
	list<SHOP_ITEM> m_listShopItem;//珍宝商城可买物品  存数据库

	DWORD m_dwFreshTimes;//刷新次数  每天重置

//	friend ROLE;
};


class CMarketMgr
{

public:
	CMarketMgr( );

	CMargetMystery cMarMys;

	CMargetJewellery cMarJew;

	int clientUseMoneyTree( ROLE* role, unsigned char * data, size_t dataLen );

	int clientRequestMoneyTree( ROLE* role, unsigned char * data, size_t dataLen );

	void freshJewShop( ROLE* role );

};

#endif // SHOP_H