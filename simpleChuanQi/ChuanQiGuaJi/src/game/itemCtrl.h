#ifndef __ITEMCTRL_H__
#define __ITEMCTRL_H__


#include "typedef.h"
#include "config.h"

using namespace std;

//#include "role.h"
class ROLE;
class ROLE_BLOB;
struct PlayerInfo;

enum
{
	E_ITEM_NOT_EXIST  = 1,
	E_PKG_FULL  = 2,
	E_LEVEL_LIMIT  = 3,	//等级限制
	E_JOB_LIMIT  = 4,		//职业限制
	E_SEX_LIMIT  = 5,		//性别限制
};

enum class e_reset_type : byte
{
	coin_type,
	stone_type,
	gold_type,
};

#define IS_AUTO_SELL_FLAG_SET(role, flag)	( role->dwAutoSellOption & (1 << ( flag - 1 ) ) )

#define SEND_BOSS_FIGHT_NUMS  (3)


#define MONSTER_DROP_SELL_ALL(opt)		( ( opt & 127 ) == 127 )		//掉落全部卖掉

enum E_AUTO_SELL_OPTION
{
	E_AUTO_SELL_EQUIP_GREY = E_QUALITY_WHITE,		//1,
	E_AUTO_SELL_EQUIP_WHITE = E_QUALITY_BLUE,		//2,
	E_AUTO_SELL_EQUIP_GREEN = E_QUALITY_GOLD,		//3
	E_AUTO_SELL_EQUIP_BLUE = E_QUALITY_DARK_GOLD,		//4
	//E_AUTO_SELL_EQUIP_PUPLE = E_QUALITY_GREEN,		//5
	//E_AUTO_SELL_EQUIP_GOLD = E_QUALITY_ORANGE,		//6

	E_AUTO_SELL_NOT_SELF_EQUIP = 7,			//职业限制,性别限制


	E_AUTO_SELL_COPPER = 16,
	E_AUTO_SELL_IRON = 17,
	E_AUTO_SELL_SILVER = 18,
	E_AUTO_SELL_GOLD = 19,

	E_AUTO_SELL_BLACK_IRON = 20,
	E_AUTO_SELL_DIAMON = 21,

	E_AUTO_SELL_NONE = 31,

};



#define IS_JOB_LIMIT(role, itemCfg)		( itemCfg->job_limit != role->byJob && itemCfg->job_limit != 4 )
#define IS_SEX_LIMIT(role, itemCfg)		( (itemCfg->sex_limit == 1 && role->bySex == E_SEX_MALE) || ( itemCfg->sex_limit == 0 && role->bySex == E_SEX_FEMALE ) )
#define IS_LEVEL_LIMIT(role, itemCfg)	( itemCfg->level_limit > role->wLevel + 15 )


#define ENHANCE_COFF		0.097222222f

#define VALUE_MAX		(2000000000UL)
#define BYTE_MAX				256
#define ITEM_ID_DIAMOND			6		//金刚石
#define ITEM_ID_COIN			20002	//金币
#define ITEM_ID_INGOT			20011	//元宝
#define ITEM_ID_VIPMONEY		20021	//vip币
#define ITEM_ID_HONOUR			20031	//声望
#define ITEM_ID_RESET_STONE		1001	//洗练石
#define VIP_NEED_LOGIN_DAY		  90
#define VIP_NEED_LEVEL		      40
#define SEND_VIP_LEVEL		      4

#define IS_MINE(itemCfg)		( itemCfg->type == TYPE_ITEM && ( itemCfg->sub_type == SUB_TYPE_ITEM_MINE || itemCfg->sub_type == SUB_TYPE_ITEM_BLACK_IRON || itemCfg->sub_type == SUB_TYPE_ITEM_DIAMON) )

#define IS_BLACK_IRON(itemCfg)		( itemCfg->type == TYPE_ITEM && ( itemCfg->sub_type == SUB_TYPE_ITEM_BLACK_IRON) )

#define IS_MINE_COPPER(itemCfg)	( itemCfg->type == TYPE_ITEM && itemCfg->sub_type == SUB_TYPE_ITEM_MINE && itemCfg->rare == 1 )
#define IS_MINE_IRON(itemCfg)		( itemCfg->type == TYPE_ITEM && itemCfg->sub_type == SUB_TYPE_ITEM_MINE && itemCfg->rare == 2 )
#define IS_MINE_SILVER(itemCfg)	( itemCfg->type == TYPE_ITEM && itemCfg->sub_type == SUB_TYPE_ITEM_MINE && itemCfg->rare == 3 )
#define IS_MINE_GOLD(itemCfg)		( itemCfg->type == TYPE_ITEM && itemCfg->sub_type == SUB_TYPE_ITEM_MINE && itemCfg->rare == 4 )

#define IS_DIAMON(itemCfg)	( itemCfg->id == TYPE_ITEM && itemCfg->sub_type == SUB_TYPE_ITEM_DIAMON)

#define IS_COIN(itemCfg)        ( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_ITEM_COIN) )
#define IS_INGOT(itemCfg)       ( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_ITEM_INGOT) )
#define IS_RESET_STONE(itemCfg)        ( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_RESET_STONE) )
//#define IS_VIPCOIN(itemCfg)       ( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_ITEM_VIPCOIN) )
#define IS_HONOUR(itemCfg)       ( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_ITEM_HONOUR) )

#define IS_BAOXIANG(itemCfg)       ( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_ITEM_BOX) )


#define IS_FIXED_BOX(itemCfg)				( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_ITEM_FIXED_BOX) )
#define IS_EXP_DAN(itemCfg)				( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_EXP_DAN) )
#define IS_FAC_CONTRIBUTE_DAN(itemCfg)	( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_FAC_CONTRIBUTE_DAN) )
#define IS_COIN_CARD(itemCfg)				( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_COIN_CARD) )
#define IS_INGOT_CARD(itemCfg)			( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_INGOT_CARD) )
#define IS_BLESS_OIL(itemCfg)				( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_BLESS_OIL) )
#define IS_VIP_CARD(itemCfg)				( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_VIP_CARD) )



#define IS_PROPS(itemCfg)		( itemCfg->type == TYPE_ITEM )
#define IS_EQUIP(itemCfg)		( itemCfg->type == TYPE_EQUIP )
#define IS_DIVINE(itemCfg)      ( itemCfg->type == TYPE_FUWEN )
#define IS_WEAPON(itemCfg)	( IS_EQUIP( itemCfg ) && itemCfg->sub_type == SUB_TYPE_EQUIP_WEAPON  )

#define  IS_EQUIP_GEM(itemCfg) ( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type >= SUB_TYPE_EQUIP_GEM_RED) && (itemCfg->sub_type <= SUB_TYPE_EQUIP_GEM_PURPLE) )

#define  IS_ELITE_MAP( mapid )  ( (mapid) >= MIN_ELITE_MAP_ID )
#define  IS_MAP_END( mapCfg, key )  ( mapCfg.find(key) == mapCfg.end() )

//不包括灵珠的所有装备
#define IS_NORMAL_EQUIP(itemCfg)		( IS_EQUIP( itemCfg ) && itemCfg->sub_type>= SUB_TYPE_EQUIP_WEAPON && itemCfg->sub_type <= SUB_TYPE_EQUIP_GEM  )



#define IS_LINGZHU(itemCfg)	( IS_EQUIP( itemCfg ) && itemCfg->sub_type == SUB_TYPE_EQUIP_LINGZHU  )

//#define IS_INGOT(itemCfg)       ( itemCfg->type == TYPE_ITEM && (itemCfg->sub_type == SUB_TYPE_ITEM_INGOT) )


#define IS_WARRIOR_WEAPON(itemCfg)	( IS_WEAPON( itemCfg ) && itemCfg->job_limit == E_JOB_WARRIOR )
#define IS_MAGIC_WEAPON(itemCfg)	( IS_WEAPON(itemCfg) && itemCfg->job_limit == E_JOB_MAGIC )
#define IS_MONK_WEAPON(itemCfg)	( IS_WEAPON(itemCfg)  && itemCfg->job_limit == E_JOB_MONK )

enum E_SELL_MINE
{
	E_SELL = 1,
	E_NOT_SELL = 2,
	E_NOT_SELL_KEEP_PURE_ABOVE_5 = 3,
	
};


#define GET_ITEM_PURE_RAND(itemCfg)		((BYTE) BASE::randBetween( itemCfg->hp_max, itemCfg->mp_max+1 ))

#define notifyX( role, type, field  )	 \
	do {\
		string strData;\
		S_APPEND_BYTE( strData, type );\
		S_APPEND_DWORD( strData, role->field );\
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UPDATE_ROLE_DATA, strData) );\
	}while(0)

#define notifyTypeValue( role, type, value  )	 \
	do {\
		string strData;\
		S_APPEND_BYTE( strData, type );\
		S_APPEND_DWORD( strData, value );\
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UPDATE_ROLE_DATA, strData) );\
	}while(0)


#define notifyTypeValue_Client( client, type, value  )	 \
	do {\
		string strData;\
		S_APPEND_BYTE( strData, type );\
		S_APPEND_DWORD( strData, value );\
		PROTOCAL::sendClient( client, PROTOCAL::cmdPacket(S_UPDATE_ROLE_DATA, strData) );\
	}while(0)

#define notifyTypeValuebyRoleID(dwRoleID, type, value ) \
	do {\
		session* client = PROTOCAL::getOnlineClient(dwRoleID);\
		if ( client )\
			notifyTypeValue_Client( client, type, value  );\
	}while(0)
	

#define notifyCharge(role) 	notifyX( role, ROLE_PRO_VIPVAR, dwCharge )
#define notifyCoin(role) 	notifyX( role, ROLE_PRO_COIN, dwCoin  )
#define notifyIngot(role) 	notifyX( role, ROLE_PRO_INGOT, dwIngot )
#define notifyExp(role) 	notifyX( role, ROLE_PRO_CUREXP, dwExp  )
#define notifyMaxMap(role) 	notifyX( role, ROLE_PRO_MAX_MAP, wCanFightMaxMapID )
//#define notifyMaxHero(role) 	notifyX( role, ROLE_PRO_MAX_HERO, wCanFightMaxHeroID )
//#define notifyFightValue(role) 	notifyX( role, ROLE_PRO_WARPWR, dwFightValue )
#define notifyLevel(role) 	notifyX( role, ROLE_PRO_LEVEL, wLevel )
#define notifyHonour(role) 	notifyX( role, ROLE_PRO_HONOUR, dwHonour )
#define notifySmeltValue(role) 	notifyX( role, ROLE_PRO_SMELTVAR, dwSmeltValue )
#define notifywVipLevel(role) 	notifyX( role, ROLE_PRO_VIP_LEVEL, getVipLevel() )


#define notify_X_If( role, type, field, oldValue  )	\
	do {\
		if ( oldValue != role->field )\
			notifyX( role, type, field );\
	}while(0)

#define IS_MERGE_ITEM(itemCfg)	( itemCfg->count > 1 )

struct KILL_MON_DROP_STATISTICS 
{
	size_t vColorSell[4];//[2];	
	//颜色;0总掉落，1卖出的

	size_t baoXiangNumsAll;		//总掉落
	size_t baoXiangNumsSell;	//卖出的
	
	//得到的或者包裹满被丢弃的

	ZERO_CONS(KILL_MON_DROP_STATISTICS)

};


//是否有足够的空间
enum E_SLOT_ENOUGH
{
	E_SLOT_OK = 0,			//都满足
	E_SLOT_PROP = 1,		//道具空间不足
	E_SLOT_EQUIP = 2,			//装备包裹空间不足
};

int getBodyIndexBySubtype( ROLE* role, WORD sub_type );

namespace ItemCtrl
{
	int clientGetItemBrief( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGetItemDetail( ROLE* role, unsigned char * data, size_t dataLen );

	//int clientMoveItem( ROLE* role, unsigned char * data, size_t dataLen );
	int clientEquipAttach( ROLE* role, unsigned char * data, size_t dataLen );
	int clientEquipDettach( ROLE* role, unsigned char * data, size_t dataLen );
	int clientSellItems( ROLE* role, unsigned char * data, size_t dataLen );
	int clientLookOthersItem( ROLE* role, unsigned char * data, size_t dataLen );
	int clientLookOther( ROLE* role, unsigned char * data, size_t dataLen );

	DECLARE_CLIENT_FUNC( clientGetFightPredictAward );
	DECLARE_CLIENT_FUNC( clientSetAutoSellOption );
	DECLARE_CLIENT_FUNC( clientGetAutoSellOption );

	DECLARE_CLIENT_FUNC( clientPackageExtend );
	//DECLARE_CLIENT_FUNC( clientItemUse );
	DECLARE_CLIENT_FUNC( clientProcsUse );

	DECLARE_CLIENT_FUNC(clientPreViewStrengthenInfo);
	DECLARE_CLIENT_FUNC(clientConfirmStrengthenInfo);

	DECLARE_CLIENT_FUNC(clientPunchHole);
	DECLARE_CLIENT_FUNC(clientInjectDiamond);
	DECLARE_CLIENT_FUNC(clientUninjectDiamond);
	DECLARE_CLIENT_FUNC(clientUninjectAllDiamonds);
	DECLARE_CLIENT_FUNC(clientEquipReset);

	//DECLARE_CLIENT_FUNC(clientSmeltEquipRemain);
	DECLARE_CLIENT_FUNC(clientSmeltEquip);
	DECLARE_CLIENT_FUNC(clientForgeSuitNum);
	DECLARE_CLIENT_FUNC(clientForgeEquip);
	DECLARE_CLIENT_FUNC(clientExchangeEquip);

	int clientEnterMap( ROLE* role, unsigned char * data, size_t dataLen );
	int clientFightReport( ROLE* role, unsigned char * data, size_t dataLen );
	int clientQuickFightReport( ROLE* role, unsigned char * data, size_t dataLen );
	int clientFightRequest( ROLE* role, unsigned char * data, size_t dataLen );
	int clientBuyFight( ROLE* role, unsigned char * data, size_t dataLen );
	int clientBuyFightCost( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGetFightTimes( ROLE* role, unsigned char * data, size_t dataLen );
	void notifyClientItem( ROLE* role, list<ITEM_CACHE*>& lstItemCache, int pkgType );
	void notifyItem( ROLE* role, ITEM_CACHE* itemCache, int pkgType);
	CONFIG::MAP_CFG* getFightMap( ROLE* role, WORD wMapID, BYTE &byRet );

	//bool isAutoSellItem( ROLE* role, ITEM_CACHE* itemCache );
	E_SELL_MINE getAutoSellMine( ROLE* role, CONFIG::ITEM_CFG * itemCfg );

	//int makeItem( ROLE* role, list<ITEM_CACHE*>& lstItemCache, size_t curLimit, CONFIG::ITEM_CFG* itemCfg, size_t itemNum, list<ITEM_CACHE*>* pLstItemCache=NULL, bool checkAutoSell =false, BYTE byQuality = 0);//, BYTE byChest1_RandNums7=0 );
	int makeMineItem( ROLE* role, CONFIG::ITEM_CFG* itemCfg, size_t itemNum, size_t itemPure=0, list<ITEM_CACHE*>* pLstItemCache=NULL );

	//bool makeItem3( ROLE* role, ITEM_CACHE* itemCache, list<ITEM_CACHE*>* pLstNotifyItem, list<ITEM_CACHE*>* pLstPropsNotify);

	void calRoleAttr( ROLE* role );

	int setMapMonster( ROLE* role, WORD wMapID );

	void setitemBasePro(ITEM_CACHE* itemCache);

	//bool killMonster( ROLE* role, WORD wMonIndex, DWORD dwMonNums, list<ITEM_CACHE*>* pLstItemNotify,  list<ITEM_CACHE*>* pLstPropsNotify, list<ITEM_CACHE*>* pLstAutoSellItem, list<ITEM_CACHE*>* pLstNoSellItem );

	bool killMapMonster( ROLE* role, WORD wMapID, BYTE byDropType, list<ITEM_CACHE*>* pLstItemNotify,  list<ITEM_CACHE*>* pLstPropsNotify, list<ITEM_CACHE*>* pLstAutoSellItem, list<ITEM_CACHE*>* pLstNoSellItem, DWORD* pvAdd = NULL );

	void updateNotifyRoleAttr(ROLE* role, bool withMaxExp=false);

	int makeOneTypeItem( ROLE* role, CONFIG::ITEM_CFG* itemCfg, size_t itemNum, BYTE byQuality = 1 );
	int makeAllTypeItem(ROLE* role, const vector<ITEM_INFO> &vecItemInfo);
	int makeItemByType(ROLE* role, CONFIG::ITEM_CFG* itemCfg, list<ITEM_CACHE*> *pListItemCachePkg, list<ITEM_CACHE*> *pListPropsCachePkg, /*list<ITEM_CACHE*> *pListItemCacheMine,*/ const ITEM_INFO & ii);

	int getEnoughSlot( ROLE* role, const vector<ITEM_INFO> &vecItemInfo );

	DWORD getPropItemIDNums( ROLE* role, const WORD id );

	DWORD rmIDItemNotify( ROLE* role, list<ITEM_CACHE*>& lstItemCache, PKG_TYPE pkgType, const WORD id, const DWORD dwDelNum );

	DWORD rmItemNotify( ROLE* role, list<ITEM_CACHE*>& lstItemCache, PKG_TYPE pkgType, CONFIG::ITEM_CFG* itemCfg, DWORD dwDelNum );

	DWORD subItemNotify( ROLE* role, list<ITEM_CACHE*>& lstItemCache, PKG_TYPE pkgType, ITEM_CACHE* item, DWORD dwDelNum );

	void init();

	//// 熔炼时是否生成新装备
	////bool isGenerateSmeltEquip(ROLE* role, DWORD count);
	//// 获得熔炼预期还需要的装备数量
	//BYTE getNeedSmeltEquip(ROLE* role);
	//// 增加熔炼的装备数量
	//void addSmeltEquipCount(ROLE* role, BYTE count);

	int equip( ROLE* role, ITEM_CACHE* itemCache, int srcPkgType, int &tgtBodyIndex, BYTE byMercenary = 0 );
};

#endif		//__ITEMCTRL_H__
