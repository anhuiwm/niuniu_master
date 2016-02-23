#include "pch.h"
#include "shop.h"
#include "itemCtrl.h"
#include "mailCtrl.h"
#include "math.h"
#include "roleCtrl.h"
#include "logs_db.h"
#include "service.h"



#define	 TREE_COST_TIMES 20
#define	 GETPRICE(price, discount)   (DWORD)ceil(((price) * (discount) / 10.0f))

static WORD _Cost[COST_TIMES] = { 200,200,200,200,200,200,200,200,200,200 };//{ 50,50,100,100,150,150,200,300,400,500 };

//static WORD _TreeCost[TREE_COST_TIMES] = {10, 20, 20, 20, 40, 40, 40, 60, 60, 60, 80, 80, 80, 100, 100, 100, 150, 150, 150, 200};

enum E_SHOP
{
	E_SHOP_SUCCESS = 0,//成功
	E_SHOP_NO_INGOT = 1,//元宝不够
	E_SHOP_NO_COIN = 2,//金币不够
	E_ITEM_FULL = 3,//背包满
	E_SHOP_ERROR_ID = 4,//不存在的物品
	E_MONEY_TREE_NO_VIP_LEVEL = 5,//vip次数不足
};

CMarketMgr::CMarketMgr( )
{

}

CMargetMystery::CMargetMystery()
{
	//resetFreshTime();
}

void CMargetMystery::initTime()
{
	CONFIG::SHOP0_CFG* shop0Cfg = CONFIG::getVipShop0Cfg(E_MYSTERY_SHOP_COMMON);
	CONFIG::SHOP0_CFG* shop1Cfg = CONFIG::getVipShop0Cfg(E_MYSTERY_SHOP_HIGN);

	if (shop1Cfg == NULL || shop0Cfg == NULL)
	{
		return;
	}

	m_vMysInfo[E_MYSTERY_SHOP_COMMON - 1] = MYSTERY_SHOP_INFO(shop0Cfg);
	m_vMysInfo[E_MYSTERY_SHOP_HIGN - 1] = MYSTERY_SHOP_INFO(shop1Cfg);
}

void CMargetMystery::resetFreshTime()
{
	CONFIG::SHOP0_CFG* shop0Cfg = CONFIG::getVipShop0Cfg( E_MYSTERY_SHOP_COMMON );
	CONFIG::SHOP0_CFG* shop1Cfg = CONFIG::getVipShop0Cfg( E_MYSTERY_SHOP_HIGN );

	if ( shop1Cfg == NULL || shop0Cfg == NULL )
	{
		return;
	}

	m_vMysInfo[E_MYSTERY_SHOP_COMMON - 1].wFreeTimes = shop0Cfg->free_time;
	m_vMysInfo[E_MYSTERY_SHOP_HIGN - 1].wFreeTimes = shop1Cfg->free_time;
}

int CMargetMystery::ClientGetCoodsList( ROLE* role, unsigned char * data, size_t dataLen )
{
	//#define	    S_REQUEST_MISTERY_SHOP	0x090A
	//C:[]
	//S:[BYTE, DWORD]2个
	//  [次数,  时间]2个

	string strData;

	DWORD now = time(nullptr);

	for ( size_t i = 0; i < MYSTERY_SHOP_NUM; i++ ) 
	{
		BYTE times = (BYTE)m_vMysInfo[i].wFreeTimes;

		DWORD time = m_vMysInfo[i].dwCanTreeTime <= now ? 0 : (m_vMysInfo[i].dwCanTreeTime - now);
		
		S_APPEND_BYTE( strData, times );
		S_APPEND_DWORD( strData, time );
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_MISTERY_SHOP, strData) );

	return 0;
}


int CMargetMystery::ClientBuyCoods( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//#define	    S_REQUEST_MISTERY_SHOP	0x090A
//C:[]
//S:[BYTE, DWORD]2个
//	  [次数,  时间]2个
//
//	  //#define	    S_BUY_MISTERY_SHOP	0x090B
//C:BYTE,BYTE
//类型:1(普通),2(高级)
//数量:1(1个),2(10个)
//
//S:BYTE, [  WORD,   WORD,   BYTE   ]n个 
//  还回, [ itemid, number, quality ]n个

	BYTE byBuyType;

	BYTE byNum;

	string strData;

	BYTE byRet = 0;

	BYTE byMoneyType=0;

	DWORD dwMoneyCost=0;

	WORD  dropID=0;

	vector<ITEM_INFO> vecItemInfo( 1, ITEM_INFO() );

	vector<CONFIG::SHOP2_CFG> vecShop2ItemCfg;

	RETURN_COND( !BASE::parseBYTE( data, dataLen, byBuyType ) , 0 );

	RETURN_COND( !BASE::parseBYTE( data, dataLen, byNum ) , 0 );

	do 
	{
		CONFIG::SHOP0_CFG *shop0Cfg = CONFIG::getVipShop0Cfg( byBuyType );

		COND_BREAK( shop0Cfg == NULL, byRet, 4);

		CONFIG::SHOP1_CFG* shop1Cfg = getVipShop1Cfg2( role->byJob, role->wLevel );

		COND_BREAK( shop1Cfg == NULL, byRet, 4);

		COND_BREAK( byNum != E_MYSTERY_SHOP_ONE && byNum != E_MYSTERY_SHOP_TEN, byRet, 4);

		DWORD now = time(nullptr);

		BYTE byOldBuyType = byBuyType;

		REDUCEUNSIGNED( byBuyType, 1 );

		bool bFreeBuy = false;

		if ( byNum == E_MYSTERY_SHOP_ONE && m_vMysInfo[byBuyType].wFreeTimes > 0 && m_vMysInfo[byBuyType].dwCanTreeTime < now )//one free
		{
			bFreeBuy = true;
		}
		else
		{
			if ( byNum == E_MYSTERY_SHOP_ONE )//buy one
			{
				byMoneyType = shop0Cfg->sell_one_money_type;
				dwMoneyCost = shop0Cfg->sell_one_money_Num;
			}
			else//buy ten
			{
				byMoneyType = shop0Cfg->sell_ten_money_type;
				dwMoneyCost = shop0Cfg->sell_ten_money_Num;

				vecItemInfo[0].itemIndex = shop0Cfg->giftiD;
				vecItemInfo[0].itemNums = shop0Cfg->giftNum;
				vecItemInfo[0].byQuality = shop0Cfg->giftQua;
			}

			COND_BREAK( byMoneyType == E_GOLD_MONEY && role->dwIngot < dwMoneyCost, byRet, 1 );

			COND_BREAK( byMoneyType == E_COIN_MONEY && role->dwCoin < dwMoneyCost, byRet, 2 );

		}

		byNum = byNum == E_MYSTERY_SHOP_ONE ? 1 : 10;

		int dropIndex = 0;

		dropIndex += byOldBuyType == E_MYSTERY_SHOP_COMMON ? 0 : 2;//之前的byBuyType自减1了

		dropIndex += m_vMysInfo[byBuyType].byFirst == 1 ? 0 : 1;

		dropID = shop1Cfg->vecDrop[ dropIndex ];

		for ( size_t i = 0; i < byNum; i++ )
		{
			ITEM_INFO itemInfo;

			CONFIG::initVipShopItems( itemInfo, dropID );

			if ( CONFIG::getItemCfg(itemInfo.itemIndex) )
			{
				vecItemInfo.push_back( itemInfo );
			}

			if ( m_vMysInfo[byBuyType].byFirst == 1 )
			{
				dropIndex += 1;
				dropID = shop1Cfg->vecDrop[ dropIndex ];
				m_vMysInfo[byBuyType].byFirst = 0;
			}
		}

		COND_BREAK( !MailCtrl::makeMailItems( role, vecItemInfo ), byRet, 3);

		if ( byMoneyType == E_GOLD_MONEY)//1元宝
		{
			ADDUP(role->dwIngot, -(int)dwMoneyCost);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)dwMoneyCost, purpose::buy_item, 0);
			notifyIngot(role);
		}
		else if( byMoneyType == E_COIN_MONEY)//2铜钱
		{
			ADDUP(role->dwCoin, -(int)dwMoneyCost);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, -(int)dwMoneyCost, 0, purpose::buy_item, 0);
			notifyCoin(role);
		}

		if ( bFreeBuy )
		{
			REDUCEUNSIGNED(m_vMysInfo[byBuyType].wFreeTimes, 1);
			m_vMysInfo[byBuyType].dwCanTreeTime = now + shop0Cfg->coolFreeTime;
		}
		ITEM_INFO para(0, byNum);
		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_SHOP_BUY_TIMES, &para);


	} while (0);


	S_APPEND_BYTE( strData, byRet );

	for ( auto& item : vecItemInfo )//含礼物 礼物没有的都是0
	{
		S_APPEND_WORD(strData, item.itemIndex);
		S_APPEND_WORD(strData, (WORD)item.itemNums);
		S_APPEND_BYTE(strData, item.byQuality);
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BUY_MISTERY_SHOP, strData) );

	return 0;
}

CMargetJewellery::CMargetJewellery( )
{
	resetFreshTime();
}


void CMargetJewellery::resetFreshTime()
{
	m_dwFreshTimes = 0;
}

void CMargetJewellery::addFreshTime()
{
	ADDUP( m_dwFreshTimes, 1 );
}

DWORD CMargetJewellery::getFreshTime()
{
	return m_dwFreshTimes;
}

void CMargetJewellery::setFreshTime( const DWORD time)
{
	m_dwFreshTimes = time;
}

void CMargetJewellery::InitShopItem(ROLE* role, bool bUseFresh, bool bPush )
{
	CONFIG::initShopItems( role->wLevel, role->byJob, m_listShopItem, m_dwFreshTimes );

	if ( bUseFresh )
	{
		addFreshTime();
	}
	
	if ( bPush )
	{
		ClientGetCoodsList( role, NULL, 0 );
	}
}

int CMargetJewellery::ClientGetCoodsList( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//珍宝商城
//#define	    S_GET_GOODS_LIST		   0x0901		//get goods list
//C:[]
//S:[ WORD ,  BYTE,   22,   WORD,  BYTE ,   WORD, WORD  ]n
//	[ shopid,discount,name,itemid,moneytype,price,number]n	

	//if ( m_listShopItem.empty() )
	//{
	//	InitShopItem( role );
	//}

	string strData;

	int fullNum = JEWELLERY_SHOP_FULL_NUM;

	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->wVipLevel);

	int vipExtraNum = ( vipCfg == NULL ) ? 0 : ( vipCfg->shop_basket );

	int canSellFullNum = JEWELLERY_SHOP_BASE_NUM + vipExtraNum;

	int buyNum = ( fullNum <= (int)m_listShopItem.size() ) ? 0 : ( fullNum - (int)m_listShopItem.size() );

	int showNum = ( canSellFullNum > buyNum ) ? ( canSellFullNum - buyNum ) : 0;

	if ( showNum == 0 )
	{
		InitShopItem( role );

		buyNum = ( fullNum <= (int)m_listShopItem.size() ) ? 0 : ( fullNum - (int)m_listShopItem.size() );

		showNum = ( canSellFullNum > buyNum ) ? ( canSellFullNum - buyNum ) : 0;
	}

	for (auto& it : m_listShopItem ) 
	{
		CONFIG::ITEM_CFG *itemCfg = CONFIG::getItemCfg(it.wItemID);
		COND_CONTINUE( itemCfg == NULL );
		S_APPEND_WORD( strData, it.wID );
		S_APPEND_BYTE( strData, it.byDiscount );
		S_APPEND_NBYTES( strData, itemCfg->name.c_str(), 22 );
		S_APPEND_WORD( strData, it.wItemID );
		S_APPEND_BYTE( strData, it.byMoneyType );
		S_APPEND_DWORD( strData, GETPRICE(it.dwItemPrice, it.byDiscount));
		S_APPEND_WORD( strData, it.wItemNums );
		S_APPEND_BYTE( strData, (BYTE)it.wQuality );

		if( --showNum <= 0)
			break; //控制客户端显示数量
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_GOODS_LIST, strData) );

	return 0;
}


int CMargetJewellery::ClientBuyCoods( ROLE* role, unsigned char * data, size_t dataLen )
{
//#define	    S_REQUEST_BUY			0x0902		//request buy 
//C:[	WORD]
//	[shopid]
//S:[byte]
//	[还回码012345]


	union_id  clinet_union_id;

	RETURN_COND(!BASE::parseWORD(data, dataLen, clinet_union_id.shopID), 0);

	RETURN_COND(!BASE::parseBYTE(data, dataLen, clinet_union_id.byDiscount), 0);

	BYTE byRet = E_SHOP_SUCCESS;
	string strData;
	do 
	{
		auto it = std::find(m_listShopItem.begin(), m_listShopItem.end(), clinet_union_id);

		COND_BREAK(it == m_listShopItem.end(), byRet, E_SHOP_ERROR_ID);

		SHOP_ITEM& sp = *it; 

		DWORD dwMoney = GETPRICE(sp.dwItemPrice, sp.byDiscount);

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( sp.wItemID );

		COND_BREAK(itemCfg == NULL,  byRet, E_SHOP_ERROR_ID);

		COND_BREAK(sp.byMoneyType == E_GOLD_MONEY && role->dwIngot < dwMoney, byRet,E_SHOP_NO_INGOT);

		COND_BREAK(sp.byMoneyType == E_COIN_MONEY && role->dwCoin < dwMoney, byRet,E_SHOP_NO_COIN);		

		COND_BREAK(ItemCtrl::makeOneTypeItem(role, itemCfg, sp.wItemNums,(BYTE)sp.wQuality) == -1, byRet,E_ITEM_FULL);

		if (sp.byMoneyType == E_GOLD_MONEY)//1元宝
		{
			ADDUP(role->dwIngot, -(int)dwMoney);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)dwMoney, purpose::buy_item, 0);
			notifyIngot(role);
		}
		else if(sp.byMoneyType == E_COIN_MONEY)//2铜钱
		{
			ADDUP(role->dwCoin, -(int)dwMoney);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, -(int)dwMoney, 0, purpose::buy_item, 0);
			notifyCoin(role);
		}
		m_listShopItem.erase(it);

		ITEM_INFO para(0, 1);
		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_SHOP_BUY_TIMES, &para);

	} while (0);
	
	S_APPEND_BYTE( strData, byRet );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_BUY, strData) );

	return 0;
}


//#define	    S_REQUEST_BUY_ALL			0x0903		//request buy all
//C:[]
//
//S:[byte]
//[还回码01235]
int CMargetJewellery::clientBuyAllCood( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byRet = E_SHOP_SUCCESS;
	string strData;
	DWORD dwGold = 0;
	DWORD dwCoin = 0;
	//DWORD dwVipCoin = 0;
	vector<ITEM_INFO> vecItemInfo;

	do 
	{
		for (auto& sp : m_listShopItem )
		{
			if (sp.byMoneyType == E_GOLD_MONEY )//1元宝
			{
				dwGold += GETPRICE(sp.dwItemPrice, sp.byDiscount);
			}
			else if (sp.byMoneyType == E_COIN_MONEY )//2铜钱
			{
				//dwCoin += sp.dwItemPrice*sp.byDiscount/10;
				dwCoin += GETPRICE(sp.dwItemPrice, sp.byDiscount);
			}

			vecItemInfo.push_back( ITEM_INFO(sp.wItemID, sp.wItemNums,(BYTE)sp.wQuality) );
		}

		COND_BREAK( role->dwIngot < dwGold, byRet, E_SHOP_NO_INGOT );
	
		COND_BREAK( role->dwCoin < dwCoin, byRet, E_SHOP_NO_COIN );

		COND_BREAK(!MailCtrl::makeMailItems( role, vecItemInfo ), byRet, E_ITEM_FULL );

		if (dwGold != 0)
		{
			ADDUP( role->dwIngot, -(int)dwGold );
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)dwGold, purpose::buy_item, 0);
			notifyIngot(role);
		}

		if (dwCoin != 0)
		{
			ADDUP( role->dwCoin, -(int)dwCoin );
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, -(int)dwCoin, 0, purpose::buy_item, 0);
			notifyCoin(role);
		}

		m_listShopItem.clear();

		ITEM_INFO para( 0, m_listShopItem.size() );
		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_SHOP_BUY_TIMES, &para);

	} while (0);

	S_APPEND_BYTE( strData, byRet );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_BUY_ALL, strData) );

	return 0;

}

/*
@#define	    S_REQUEST_UPDATE_COST		0x0904		//request update fee
*/
int CMargetJewellery::clientRequestUpdateCost( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;

	DWORD index;

	DWORD updateTimes;

	index =  getFreshTime();

	updateTimes = getFreshTime();

	DWORD dwCostGold = updateTimes <= (COST_TIMES - 1) ? _Cost[index] : _Cost[COST_TIMES - 1];

	S_APPEND_DWORD(strData, dwCostGold);

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_UPDATE_COST, strData) );

	return 0;
}

/*
@#define	    S_REQUEST_UPDATE			0x0905		//request update
*/
int CMargetJewellery::clientRequestUpdate( ROLE* role, unsigned char * data, size_t dataLen )
{
	DWORD index;

	DWORD updateTimes;
	
	string strData;

	BYTE byRet = E_SHOP_SUCCESS;

	index =  getFreshTime();

	updateTimes = getFreshTime();

	DWORD dwCostGold = updateTimes <= (COST_TIMES - 1) ? _Cost[index] : _Cost[COST_TIMES - 1];

	if (role->dwIngot < dwCostGold)
	{
		byRet = E_SHOP_NO_INGOT;
	}
	else
	{
		InitShopItem(role, true);

		ADDUP(role->dwIngot, -(int)dwCostGold);
		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)dwCostGold, purpose::request_update, 0);

		notifyIngot(role);
	}

	S_APPEND_BYTE(strData, byRet);

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_UPDATE, strData) );
	return 0;
}


void CMarketMgr::freshJewShop( ROLE* role )
{
	role->cMarMgr.cMarJew.InitShopItem( role, false, true );
}



///*
//@ #define	    S_GET_GOODS_LIST		    0x0901		//get goods list
//*/
//int SHOP::clientGetCoodsList( ROLE* role, unsigned char * data, size_t dataLen )
//{
//	BYTE byShopType;
//	if ( !BASE::parseBYTE( data, dataLen, byShopType) )
//		return 0;
//	list<SHOP_ITEM>* pListShopItem;
//
//	if(byShopType == VIP_SHOP)
//	{
//		if ( role->listVipShopItem.empty() )
//		{
//			SHOP::clientCreateShopItem( role ,true);
//		}
//		pListShopItem = &role->listVipShopItem;
//	}
//	else
//	{
//		if ( role->listShopItem.empty() )
//		{
//			SHOP::clientCreateShopItem( role ,false);
//		}
//		pListShopItem = &role->listShopItem;
//	}
//
//	string strData;
//	S_APPEND_BYTE(strData, byShopType);
//	for (auto& it : (*pListShopItem) ) 
//	{
//		CONFIG::ITEM_CFG *itemCfg = CONFIG::getItemCfg(it.wItemID);
//		if (itemCfg == NULL)
//		{
//			logwm("NULL::shopid = %u",it.wID);
//			continue;
//		}
//		S_APPEND_WORD( strData, it.wID );
//		S_APPEND_BYTE( strData, it.byDiscount );
//		S_APPEND_NBYTES( strData, itemCfg->name.c_str(), 22 );
//		S_APPEND_WORD( strData, it.wItemID );
//		S_APPEND_BYTE( strData, it.byMoneyType );
//		S_APPEND_DWORD( strData, GETPRICE(it.dwItemPrice, it.byDiscount));
//		S_APPEND_WORD( strData, it.wItemNums );
//		S_APPEND_WORD( strData, it.wQuality );
//		logwm("shopid = %u  type=%u  discount=%u  Price=%u  compose = %u",it.wID, it.byMoneyType,it.byDiscount,it.dwItemPrice,it.wQuality  );
//	}
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_GOODS_LIST, strData) );
//
//	return 0;
//}

///*
//@ #define	    S_REQUEST_BUY			    0x0902		//request buy 
//*/
//int SHOP::clientBuyCood( ROLE* role, unsigned char * data, size_t dataLen )
//{
//	BYTE byShopType;
//	if ( !BASE::parseBYTE( data, dataLen, byShopType) )
//		return -1;
//
//	WORD shopID;
//	if ( !BASE::parseWORD( data, dataLen, shopID) )
//		return -1;
//	BYTE byRet = SHOP_SUCCESS;
//	string strData;
//
//	list<SHOP_ITEM>* pListShopItem;
//	if(byShopType == VIP_SHOP)
//	{
//		pListShopItem = &role->listVipShopItem;
//	}
//	else
//	{
//		pListShopItem = &role->listShopItem;
//	}
//
//	auto it = std::find( (*pListShopItem).begin(),(*pListShopItem).end(), shopID);
//	if ( byShopType == VIP_SHOP && role->wVipLevel == 0)
//	{
//		byRet = NO_VIP_LEVEL;
//	}
//	else if ( it == (*pListShopItem).end() )
//	{
//		byRet = SHOP_ERROR_ID;
//	}
//	else
//	{
//		SHOP_ITEM& sp = *it; 
//
//		DWORD dwMoney = GETPRICE(sp.dwItemPrice, sp.byDiscount);
//
//		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( sp.wItemID );
//		if ( itemCfg==NULL )
//		{
//			byRet = SHOP_ERROR_ID;
//		}
//		else if (sp.byMoneyType == E_GOLD_MONEY && role->dwIngot < dwMoney)//1元宝
//		{
//			byRet = SHOP_NO_INGOT;
//		}
//		else if (sp.byMoneyType == E_COIN_MONEY && role->dwCoin < dwMoney)//2铜钱
//		{
//			byRet = SHOP_NO_COIN;
//		}
//		else if (sp.byMoneyType == E_VIP_MONEY && role->dwVipCoin < dwMoney)//3vip币
//		{
//			byRet = SHOP_NO_VIP_MONEY;
//		}
//		else
//		{
//			if (ItemCtrl::makeOneTypeItem(role, itemCfg, sp.wItemNums,(BYTE)sp.wQuality) == -1)
//			{
//				byRet = ITEM_FULL;
//			}
//			else
//			{
//				if (sp.byMoneyType == E_GOLD_MONEY)//1元宝
//				{
//					ADDUP(role->dwIngot, -(int)dwMoney);
//					notifyIngot(role);
//				}
//				else if(sp.byMoneyType == E_COIN_MONEY)//2铜钱
//				{
//					ADDUP(role->dwCoin, -(int)dwMoney);
//					notifyCoin(role);
//				}
//				else if(sp.byMoneyType == E_VIP_MONEY)//vip币
//				{
//					ADDUP(role->dwVipCoin, -(int)dwMoney);
//					notifyVipCoin(role);
//					//string str;
//					//S_APPEND_DWORD( str, role->dwVipCoin );
//					//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_VIPCOIN, str) );
//				}
//				(*pListShopItem).erase(it);
//			}
//			
//		}
//	}
//	S_APPEND_BYTE(strData, byShopType);
//	S_APPEND_BYTE( strData, byRet );
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_BUY, strData) );
//
//	return 0;
//}

///*
//@ #define	    S_REQUEST_BUY_ALL			0x0903		//request buy all
//*/
//int SHOP::clientBuyAllCood( ROLE* role, unsigned char * data, size_t dataLen )
//{
//	BYTE byShopType;
//	if ( !BASE::parseBYTE( data, dataLen, byShopType) )
//		return -1;
//	BYTE byRet = SHOP_SUCCESS;
//	string strData;
//	DWORD dwGold = 0;
//	DWORD dwCoin = 0;
//	DWORD dwVipCoin = 0;
//	vector<ITEM_INFO> vecItemInfo;
//
//
//	list<SHOP_ITEM>* pListShopItem;
//	if(byShopType == VIP_SHOP)
//	{
//
//		pListShopItem = &role->listVipShopItem;
//	}
//	else 
//		pListShopItem = &role->listShopItem;
//	}
//
//	for (auto& sp : (*pListShopItem) )
//	{
//		if (sp.byMoneyType == E_GOLD_MONEY )//1元宝
//		{
//			dwGold += GETPRICE(sp.dwItemPrice, sp.byDiscount);
//		}
//		else if (sp.byMoneyType == E_COIN_MONEY )//2铜钱
//		{
//			//dwCoin += sp.dwItemPrice*sp.byDiscount/10;
//			dwCoin += GETPRICE(sp.dwItemPrice, sp.byDiscount);
//		}
//		else if (sp.byMoneyType == E_VIP_MONEY )//3
//		{
//			//dwVipCoin += sp.dwItemPrice*sp.byDiscount/10;
//			dwVipCoin += GETPRICE(sp.dwItemPrice, sp.byDiscount);
//		}
//		vecItemInfo.push_back( ITEM_INFO(sp.wItemID, sp.wItemNums,(BYTE)sp.wQuality) );
//	}
//
//	if (role->dwIngot < dwGold )
//	{
//		byRet = SHOP_NO_INGOT;
//	}
//	else if ( role->dwCoin < dwCoin)
//	{
//		byRet = SHOP_NO_COIN;
//	}
//	else if ( role->dwVipCoin < dwVipCoin)
//	{
//		byRet = SHOP_NO_VIP_MONEY;
//	}
//	else
//	{
//		if ( MailCtrl::makeMailItems( role, vecItemInfo ) )
//		{
//			if (dwGold != 0)
//			{
//				ADDUP( role->dwIngot, -(int)dwGold );
//				notifyIngot(role);
//			}
//			
//			if (dwCoin != 0)
//			{
//				ADDUP( role->dwCoin, -(int)dwCoin );
//				notifyCoin(role);
//			}
//			if (dwVipCoin != 0)
//			{
//				ADDUP( role->dwVipCoin, -(int)dwVipCoin );
//				notifyVipCoin(role);
//				//string str;
//				//S_APPEND_DWORD( str, role->dwVipCoin );
//				//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_VIPCOIN, str) );
//			}
//
//			(*pListShopItem).clear();
//		}
//		else
//		{
//			byRet = ITEM_FULL;
//		}
//	}
//	S_APPEND_BYTE(strData, byShopType);
//	S_APPEND_BYTE( strData, byRet );
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_BUY_ALL, strData) );
//	logwm("AllCoodbyret = %u",byRet);
//	return 0;
//}


///*
//@#define	    S_REQUEST_UPDATE_COST		0x0904		//request update fee
//*/
//int SHOP::clientRequestUpdateCost( ROLE* role, unsigned char * data, size_t dataLen )
//{
//	BYTE byShopType;
//	if ( !BASE::parseBYTE( data, dataLen, byShopType) )
//		return -1;
//
//	string strData;
//	WORD index;
//	WORD updateTimes;
//
//	if(byShopType == VIP_SHOP)
//	{
//		index =  role->wVipShopItemTimes;
//		updateTimes = role->wVipShopItemTimes;
//	}
//	else/* if (byShopType == GENERAL_SHOP)*/
//	{
//		index =  role->wShopItemTimes;
//		updateTimes = role->wShopItemTimes;
//	}
//
//	DWORD dwCostGold = ( updateTimes <= (COST_TIMES - 1) ) ? _Cost[index] : _Cost[COST_TIMES - 1];
//
//	S_APPEND_BYTE(strData, byShopType);
//	S_APPEND_WORD(strData, dwCostGold);
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_UPDATE_COST, strData) );
//	logwm("dwCostGold = %u",dwCostGold);
//	return 0;
//}



///*
//@#define	    S_REQUEST_UPDATE			0x0905		//request update
//*/
//int SHOP::clientRequestUpdate( ROLE* role, unsigned char * data, size_t dataLen )
//{
//	BYTE byShopType;
//	if ( !BASE::parseBYTE( data, dataLen, byShopType) )
//		return -1;
//
//	WORD index;
//	WORD updateTimes;
//	BYTE byRet = E_SHOP_SUCCESS;
//	string strData;
//	if(byShopType == VIP_SHOP)
//	{
//		index =  role->wVipShopItemTimes;
//		updateTimes = role->wVipShopItemTimes;
//	}
//	else/* if (byShopType == GENERAL_SHOP)*/
//	{
//		index =  role->wShopItemTimes;
//		updateTimes = role->wShopItemTimes;
//	}
//
//	DWORD dwCostGold = updateTimes <= (COST_TIMES - 1) ? _Cost[index] : _Cost[COST_TIMES - 1];
//	if (role->dwIngot < dwCostGold)
//	{
//		byRet = E_SHOP_NO_INGOT;
//	}
//	else
//	{
//		if(byShopType == VIP_SHOP)
//		{
//			role->wVipShopItemTimes++;
//			SHOP::clientCreateShopItem( role,true );
//		}
//		else/* if (byShopType == GENERAL_SHOP)*/
//		{
//			role->wShopItemTimes++;
//			SHOP::clientCreateShopItem( role,false );
//		}
//		ADDUP(role->dwIngot, -(int)dwCostGold);
//		notifyIngot(role);
//	}
//	S_APPEND_BYTE(strData, byShopType);
//	S_APPEND_BYTE(strData, byRet);
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_UPDATE, strData) );
//	return 0;
//}

//int SHOP::clientCreateShopItem( ROLE* role, /*list<SHOP_ITEM>& listShopItem,*/ bool bVip)
//{
	//vector<WORD> vecShopIDs;
	//list<SHOP_ITEM>* pListShopItem;
	//if(bVip)
	//{
	//	pListShopItem = &role->listVipShopItem;
	//}
	//else
	//{
	//	pListShopItem = &role->listShopItem;
	//}
	//CONFIG::initShopItems( role->wLevel, role->byJob, (*pListShopItem), 1 );
	//return 0;
//}

//#define	    S_USE_MONEYTREE				0x0907		//
int CMarketMgr::clientUseMoneyTree( ROLE* role, unsigned char * data, size_t dataLen )
{
	const float ratio = 0.15f;

	BYTE byRet = E_SHOP_SUCCESS;//成功
	string strData;
	BYTE nextIndex = role->byUseMoneyTreeTimes + 1;
	DWORD dwCoin = 0;
	DWORD dwCostGold = 0;
	WORD  mulCion  = 1;
	BYTE  crit = 0;//15%暴击获得两倍
	CONFIG::MONEY_TREE0_CFG* treeCostCfg = CONFIG::getMoneyTree0Cfg( nextIndex );
	if ( treeCostCfg != NULL )
	{
		dwCostGold = treeCostCfg->cost;
		mulCion = treeCostCfg->mul;
	}

	DWORD* pDwCoin = CONFIG::getMoneyTreeCfg( role->wLevel );
	if ( pDwCoin != NULL )
	{
		dwCoin = *pDwCoin * mulCion;
	}

	if (role->dwIngot < dwCostGold)
	{
		byRet = E_SHOP_NO_INGOT;//钱不够
	}
	else
	{
		CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
		if (vipCfg != NULL && vipCfg->money_buy > role->byUseMoneyTreeTimes)
		{
			if( RAND_HIT(ratio) )
			{
				crit = 1;
				dwCoin *= 2;
			}
			ADDUP(role->dwIngot, -(int)dwCostGold );
			ADDUP(role->dwCoin, (int)dwCoin );
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, (int)dwCoin, -(int)dwCostGold, purpose::ingot_to_coin, 0);
			role->byUseMoneyTreeTimes++;
			notifyIngot(role);
			notifyCoin(role);
			RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_USE_MONEYTREE);
		}
		else
		{
			byRet  = E_MONEY_TREE_NO_VIP_LEVEL ;//vip等级不足
		}	
	}

	S_APPEND_BYTE(strData, byRet);
	S_APPEND_BYTE(strData, crit);
	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_USE_MONEYTREE, strData) );

	return 0;
}

//#define	    S_REQUEST_MONEYTREE			0x0908		//
int CMarketMgr::clientRequestMoneyTree( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;
	DWORD dwCoin = 0;
	BYTE byTimes = 0;
	BYTE nextIndex = role->byUseMoneyTreeTimes + 1;
	DWORD dwCostGold = 0;
	WORD  mulCion  = 1;
	
	CONFIG::MONEY_TREE0_CFG* treeCostCfg = CONFIG::getMoneyTree0Cfg( nextIndex );
	if ( treeCostCfg != NULL )
	{
		dwCostGold = treeCostCfg->cost;
		mulCion = treeCostCfg->mul;
	}

	DWORD* pDwCoin = CONFIG::getMoneyTreeCfg( role->wLevel );
	if ( pDwCoin != NULL )
	{
		dwCoin = *pDwCoin * mulCion;
	}
	
	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg( role->getVipLevel());
	if ( vipCfg != NULL && vipCfg->money_buy > role->byUseMoneyTreeTimes)
	{
		byTimes = vipCfg->money_buy - role->byUseMoneyTreeTimes;
	}

	S_APPEND_DWORD( strData, dwCoin );
	S_APPEND_BYTE(strData, byTimes);
	S_APPEND_DWORD( strData, dwCostGold );

	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_REQUEST_MONEYTREE, strData) );

	return 0;
}
