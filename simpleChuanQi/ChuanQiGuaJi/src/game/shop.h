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

	MYSTERY_SHOP_INFO m_vMysInfo[MYSTERY_SHOP_NUM];//�������˵���Ϣ

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
	list<SHOP_ITEM> m_listShopItem;//�䱦�̳ǿ�����Ʒ  �����ݿ�

	DWORD m_dwFreshTimes;//ˢ�´���  ÿ������

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