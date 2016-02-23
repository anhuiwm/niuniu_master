#include "pch.h"
#include "log.h"
#include "base.h"
#include "protocal.h"
#include "db.h"
#include "zlibapi.h"
#include "roleCtrl.h"
#include "factionCtrl.h"
#include "itemCtrl.h"
#include "skill.h"
#include "broadCast.h"
#include "arena.h"
#include "jsonpacket.h"
#include "common.h"
#include "mailCtrl.h"
#include "logs_db.h"
#include "service.h"

using namespace rapidjson;


//#include "../game/logDefine.hpp"
//#include "logIntf.h"


#include "rank.h"

//#define  COST_TIMES   10


namespace ItemCtrl
{
	//class SELL_ITEM_CACHE : public ITEM_CACHE
	//{
	//public:
	//	bool bAutoSellType;
	//	DWORD dwPrice;
	//};

	static WORD _Cost[COST_TIMES] = { 50,50,50,50,50,50,50,50,50,50 };
	//static BYTE quality2EnhanceNums[8] = {0,20,25,30,35,40,45,50 };	//0index不用,品质1-7 对饮最大强化等级

	//3.强化消耗的铜钱
	//	稀有度7：1经验＝68铜钱
	//	稀有度6：1经验＝61铜钱
	//	稀有度5：1经验＝54铜钱
	//	稀有度4：1经验＝48铜钱
	//	稀有度3：1经验＝43铜钱
	//	稀有度2：1经验＝38铜钱
	//	稀有度1：1经验＝34铜钱
	//static WORD quality2Coin[8] = {0,34,38,43,48,54,61,68 };	//0index不用,品质1-7

	//子类型	部位	系数
	//	1	武器	1.2
	//	2	衣服	1.1
	//	3	头盔	0.8
	//	4	腰带	1.1
	//	5	鞋子	1.1
	//	6	项链	1
	//	7	手套	0.9
	//	8	戒指	1.2
	//	9	护腿	0.8
	//	10	手镯	0.8
	static float strength_ratio[ 11 ] = { 1.0f,1.2f,1.1f,0.8f,1.1f,1.1f,1.0f,0.9f,1.2f,0.8f,0.8f };//身体各个部位对应强化需要的强化石系数,第一个修正用

	//(品质)稀有度	属性比例
	//	1		   90	
	//	2		   95	
	//	3		   100	
	//	4		   105	
	//	5		   110	
	//	6		   115	
	//	7		   120	

	enum E_FIGHT_BOSS
	{
		FIGHT_BOSS_SUCCESS = 0,//成功
		FIGHT_BOSS_NO_INGOT = 1,//钱不够
		FIGHT_BOSS_NOT_VIP = 2,//VIP不够
		FIGHT_BOSS_NOT_TIMES = 3,//次数不够
	};

	enum E_STRENGTH_TYPE
	{
		STRENGTH_TYPE_ONE = 0,
		STRENGTH_TYPE_TWO = 1,
		STRENGTH_TYPE_THREE = 2,
	};

	//static WORD _strengthTimes[3] = { 1, 10, 50 };

	typedef int (*FUNC_ITME_USE)( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType, BYTE bySubOpera);
	typedef map<WORD, FUNC_ITME_USE> MAP_ITEM_USE;
	static MAP_ITEM_USE m_mapItemUse;

	static DWORD getPropItemNums( ROLE* role, CONFIG::ITEM_CFG* itemCfg );


	const static std::map<int, int> _map_exchange_level =
	{
		std::map<WORD, WORD>::value_type(15, 17),
		std::map<WORD, WORD>::value_type(25, 27),
		std::map<WORD, WORD>::value_type(35, 37),
		std::map<WORD, WORD>::value_type(45, 47),
		std::map<WORD, WORD>::value_type(55, 57),
		std::map<WORD, WORD>::value_type(65, 67),
		std::map<WORD, WORD>::value_type(75, 77),
		std::map<WORD, WORD>::value_type(80, 82)
	};

}





#define MAX_COMPOSE_VALUE(times)		(times*3)

#define ENHANCE_COIN			100000
#define COMPOSE_COIN			100000

#define MAX_ENHANCE_LEVEL	10
#define MAX_COMPOSE_LEVEL	10


//弃用
DWORD getComposeMaxLevel( ITEM_CACHE* itemCache )
{
	static DWORD composeLevel[ ] = { 0,  2,3,4,5,6,  8,  10 };

	if ( itemCache->itemCfg->rare >=7 )
		return composeLevel[7];

	return composeLevel[ itemCache->itemCfg->rare ];
	
}

//itemPure == 255 时，确保纯度最低5  out
int ItemCtrl::makeMineItem( ROLE* role, CONFIG::ITEM_CFG* itemCfg, size_t itemNum, size_t itemPure, list<ITEM_CACHE*>* pLstItemCache )
{
	assert(itemCfg);

	if ( role->m_packmgr.lstMinePkg.size() + itemNum  > role->m_packmgr.curMineLimit )
		return -1;

	for ( size_t i = 0; i<itemNum; i++)
	{

		//矿石只需要这个检查
		//if ( checkAutoSell && ( role->dwAutoSellOption & (1 << E_AUTO_SELL_NOT_EQUIP ) ) )
//		if ( checkAutoSell && isAutoSellMine( role, itemCfg ) )
//		{
//			ADDUP( role->dwCoin, itemCfg->price );
//			continue;
//		}

		ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, itemNum );
		role->m_packmgr.lstMinePkg.push_back( itemCache );

		if ( pLstItemCache)
			pLstItemCache->push_back(itemCache);
		
	}

	return 0;
}

void getNeedSlot( const vector<ITEM_INFO> &vecItemInfo, size_t& needSlot , size_t& needMineSlot )
{
	needSlot =0;
	needMineSlot =0;

	for ( auto it : vecItemInfo )
	{
		if ( it.itemIndex == ITEM_ID_COIN )
		{
			//dwAddCoin += it.itemNums;
			continue;
		}
		if ( it.itemIndex == ITEM_ID_INGOT )
		{
			//dwAddIngot += it.itemNums;
			continue;
		}
		if ( it.itemIndex == ITEM_ID_VIPMONEY )
		{
			//dwAddIngot += it.itemNums;
			continue;
		}
		if ( it.itemIndex == ITEM_ID_HONOUR )
		{
			//dwAddIngot += it.itemNums;
			continue;
		}
		
		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( it.itemIndex );
		if ( itemCfg==NULL )
			continue;

		if ( IS_PROPS( itemCfg ) )
		{
			if ( IS_MERGE_ITEM( itemCfg ) )
				needMineSlot += ( it.itemNums + itemCfg->count-1) /itemCfg->count;
			else
				needMineSlot += it.itemNums;
		}
		else
		{
			if ( IS_MERGE_ITEM( itemCfg ) )
				needSlot += ( it.itemNums + itemCfg->count-1) /itemCfg->count;
			else
				needSlot += it.itemNums;
		}
	}

}
static void updateMerAttrByEquip( ROLE* role, const ITEM_CACHE* pWeapon )
{
	for ( auto& mer : role->cMerMgr.m_vecCMercenary )
	{
		for ( int i=0; i<MER_GRID_NUMS; i++)
		{
			ITEM_CACHE* itemCache = mer.vBodyEquip[i];
			if ( itemCache == pWeapon )
			{
				mer.updateNotifyMerAttr(role);
			}
		}
	}
}


int ItemCtrl::getEnoughSlot( ROLE* role, const vector<ITEM_INFO> &vecItemInfo )
{
	if ( vecItemInfo.empty() )
		return E_SLOT_OK;

	//不考虑合并
	size_t needEquipSlot = 0;
	size_t needPropsSlot = 0;

	getNeedSlot( vecItemInfo, needEquipSlot , needPropsSlot );

	int prop = !!(role->m_packmgr.lstItemPkg.size() + needPropsSlot  > role->m_packmgr.curItemPkgLimit);//道具包裹
	int equip = !!(role->m_packmgr.lstEquipPkg.size() + needEquipSlot  > role->m_packmgr.curEquipPkgLimit);//装备包裹

	return ( prop << E_SLOT_PROP ) | ( equip << E_SLOT_EQUIP );
}

E_SELL_MINE ItemCtrl::getAutoSellMine( ROLE* role, CONFIG::ITEM_CFG * itemCfg)
{

	if ( IS_AUTO_SELL_FLAG_SET(role, E_AUTO_SELL_COPPER ) && IS_MINE_COPPER(itemCfg) )
		return E_SELL;
	if ( IS_AUTO_SELL_FLAG_SET(role, E_AUTO_SELL_IRON ) && IS_MINE_IRON(itemCfg) )
		return E_SELL;
	if ( IS_AUTO_SELL_FLAG_SET(role, E_AUTO_SELL_SILVER ) && IS_MINE_SILVER(itemCfg) )
		return E_SELL;
	if ( IS_AUTO_SELL_FLAG_SET(role, E_AUTO_SELL_GOLD ) && IS_MINE_GOLD(itemCfg) )
		return E_SELL;

	if ( IS_AUTO_SELL_FLAG_SET(role, E_AUTO_SELL_BLACK_IRON ) && IS_BLACK_IRON(itemCfg) )
	{
		size_t itemPure = GET_ITEM_PURE_RAND(itemCfg);
		return itemPure <5 ?  E_SELL : E_NOT_SELL_KEEP_PURE_ABOVE_5;
	}
	else	if ( IS_AUTO_SELL_FLAG_SET(role, E_AUTO_SELL_DIAMON ) && IS_DIAMON(itemCfg) )
	{
		size_t itemPure = GET_ITEM_PURE_RAND(itemCfg);
		return itemPure <5 ?  E_SELL : E_NOT_SELL_KEEP_PURE_ABOVE_5;
	}

	return E_NOT_SELL;
		
}

//打开向服务器请求当前的设置
int ItemCtrl::clientGetAutoSellOption( ROLE* role, unsigned char * data, size_t dataLen )
{
//	C:[]
//	S:[DWORD]低7位 0表示关,1表示开 
//	

	string strData;
	S_APPEND_DWORD( strData, role->dwAutoSellOption );
			
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_DROPSET, strData) );
	return 0;

	
}

//请求保存设置
int ItemCtrl::clientSetAutoSellOption( ROLE* role, unsigned char * data, size_t dataLen )
{
//	C:[DWORD]
//		
//	S:[BYTE]
//		error 0是成功

	DWORD dwAutoSellOption;
	if ( !BASE::parseDWORD( data, dataLen, dwAutoSellOption)) 
		return 0;

	role->dwAutoSellOption = dwAutoSellOption;

	string strData;
	S_APPEND_BYTE( strData, 0 );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_DROPSET, strData) );

	return 0;
	
}

DWORD ItemCtrl::getPropItemIDNums( ROLE* role, const WORD id )
{
	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(id);

	RETURN_COND(role==NULL || itemCfg == NULL , 0);

	return getPropItemNums( role, itemCfg );
}

DWORD ItemCtrl::getPropItemNums( ROLE* role, CONFIG::ITEM_CFG* itemCfg )
{
	RETURN_COND(role==NULL || itemCfg == NULL , 0);

	DWORD count = 0;

	for ( auto &it : role->m_packmgr.lstItemPkg )
	{
		count += ( (itemCfg->id == it->itemCfg->id)? it->itemNums : 0 );
	}

	return count;
}

int getMinorFightValueIndex(ROLE* role, int a, int b)
{

	if ( role->vBodyEquip[a] == NULL )
		return a;
	
	if ( role->vBodyEquip[b] == NULL )
		return b;

	return ( role->vBodyEquip[a]->dwFightValue <= role->vBodyEquip[b]->dwFightValue ) ? a : b;

}

int getBodyIndexBySubtype( ROLE* role, WORD sub_type )
{
	//if ( sub_type > BODY_GRID_NUMS )
	//	return -1;
	//
	if ( sub_type >= SUB_TYPE_EQUIP_WEAPON && sub_type <= SUB_TYPE_EQUIP_GEM )
	{
		return sub_type-1;
	}

	
	//if ( sub_type == SUB_TYPE_EQUIP_BRACELET  )
	//{
	//	return getMinorFightValueIndex( role, BODY_INDEX_BRACELET_L, BODY_INDEX_BRACELET_R);
	//}
	//else if ( sub_type == SUB_TYPE_EQUIP_RING )
	//{
	//	return getMinorFightValueIndex( role, BODY_INDEX_RING_L, BODY_INDEX_RING_R);
	//}
	//else if ( sub_type >= SUB_TYPE_EQUIP_WEAPON && sub_type <= SUB_TYPE_EQUIP_NECKLACE )
	//{
	// 	return sub_type-1;
	//}
	//else if ( sub_type >= SUB_TYPE_EQUIP_MEDAL && sub_type <= SUB_TYPE_EQUIP_LINGZHU )
	//{
	// 	return sub_type+1;
	//}

	return -1;	

}

void swapEquip(ROLE* role, int bodyIndex, list<ITEM_CACHE*>& lstDst, list<ITEM_CACHE*>::iterator pkgItemIt,  ITEM_CACHE* itemCache)
{

	if ( itemCache == NULL )
		return;

	if ( itemCache->itemCfg == NULL )
		return;

	if ( !IS_EQUIP(itemCache->itemCfg) )
	{
		return;
	}


	if ( role->vBodyEquip[bodyIndex] != NULL )
	{
		*pkgItemIt = role->vBodyEquip[bodyIndex];
	}
	else
	{
		lstDst.erase( pkgItemIt );
	}
	
	role->vBodyEquip[bodyIndex] = itemCache;

}

//穿脱装备,服务器不向客户端刷新包裹,装备 //佣兵传装备在另外的地方做
int ItemCtrl::equip( ROLE* role, ITEM_CACHE* itemCache, int srcPkgType, int &tgtBodyIndex, BYTE byMercenary )
{

//等级不足

	if ( srcPkgType != PKG_TYPE_EQUIP /*&& srcPkgType != PKG_TYPE_STORAGE*/ )
		return E_ITEM_NOT_EXIST;

	list<ITEM_CACHE*>& lstDst = role->m_packmgr.lstEquipPkg;
	
	auto pkgItemIt = find( lstDst.begin(), lstDst.end(), itemCache );
	if ( pkgItemIt == lstDst.end() )
		return E_ITEM_NOT_EXIST;

	if ( itemCache->itemCfg->type != TYPE_EQUIP )
		return E_ITEM_NOT_EXIST;

	//if ( tgtBodyIndex == -1 )
	//{
		tgtBodyIndex = getBodyIndexBySubtype( role, itemCache->itemCfg->sub_type );
		if ( tgtBodyIndex == -1 )
			return E_ITEM_NOT_EXIST;
	//}
	
	if ( IS_LEVEL_LIMIT(role, itemCache->itemCfg) )
		return E_LEVEL_LIMIT;

	if ( IS_JOB_LIMIT(role, itemCache->itemCfg) )
		return E_JOB_LIMIT;

	if ( IS_SEX_LIMIT(role, itemCache->itemCfg ) )
		return E_SEX_LIMIT;

	swapEquip( role, tgtBodyIndex, lstDst, pkgItemIt,  itemCache );

	return 0;

}

//穿脱装备,服务器不向客户端刷新包裹,装备
int unEquip( ROLE* role, int bodyIndex, int dstPkgType=PKG_TYPE_EQUIP )
{

	//if ( bodyIndex >= BODY_GRID_NUMS )
	//	return E_ITEM_NOT_EXIST;

	//if ( role->vBodyEquip[bodyIndex] == NULL )
	//	return E_ITEM_NOT_EXIST;

	//list<ITEM_CACHE*>& lstDst = role->m_packmgr.lstEquipPkg;
	//BYTE curLimit = role->m_packmgr.curEquipPkgLimit;
	//
	//if ( lstDst.size() >= curLimit )
	//	return E_PKG_FULL;

	//lstDst.push_back( role->vBodyEquip[bodyIndex] );
	//
	//role->vBodyEquip[bodyIndex] = NULL;

	return E_SUCC;
}


/*
#define GET_PKG_LIMIT(role, pkgtype)	( pkgtype==PKG_TYPE_EQUIP ? role->m_packmgr.curEquipPkgLimit \
										: pkgtype==PKG_TYPE_STORAGE ? role->m_packmgr.curStorageLimit \
										: pkgtype==PKG_TYPE_MINE ? role->m_packmgr.curMineLimit : 0 )
*/

DWORD getExtraValueByAttr( ITEM_CACHE* item, BYTE attr)
{

		for ( int i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++)
		{
			if ( item->attrValueExtra[ i ].attr == attr )
				return item->attrValueExtra[ i ].value;

		}
		return 0;
}

bool equipCompose( ROLE* role, ITEM_CACHE* itemMain, ITEM_CACHE* item1, ITEM_CACHE* item2, ITEM_CACHE* itemDiamon )
{

	assert( itemMain->byQuality < getComposeMaxLevel(itemMain) );
	assert( item1 );
	assert( item2 );
	
	//
	if ( itemDiamon )
	{

		for ( int i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++)
		{
			BYTE tgtAttr = itemMain->attrValueExtra[ i ].attr;
			DWORD &tgtValue = itemMain->attrValueExtra[ i ].value;

			DWORD effectiveValue = max<size_t>(getExtraValueByAttr( item1, tgtAttr), getExtraValueByAttr( item2, tgtAttr) );
			DWORD addValue = min<DWORD>( itemDiamon->itemNums, effectiveValue );

			tgtValue = min<size_t>( tgtValue+addValue, MAX_COMPOSE_VALUE( itemMain->byQuality+1) );
		}

	}

	itemMain->byQuality += 1;
	ITEM_INFO para(itemMain->itemCfg->sub_type,0,(WORD)itemMain->byQuality);
	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_HAS_EQUIP_COMPOSE, &para);
	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_DO_EQUIP_COMPOSE);
	return true;

}

void ItemCtrl::notifyItem( ROLE* role, ITEM_CACHE* itemCache, int pkgType)
{
	string strData;
	S_APPEND_BYTE( strData, pkgType );
	S_APPEND_BYTE( strData, 1 );
	ItemBriefInfo brefInfo(itemCache);
	S_APPEND_STRUCT( strData, brefInfo);

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_NOTIFY_NEW_ITEM, strData) );
}

void ItemCtrl::notifyClientItem( ROLE* role, list<ITEM_CACHE*>& lstItemCache, int pkgType)
{
//	C[]
//	S:[BYTE, BYTE, [ItemBriefInfo]]
//		[packType, num, [itemInfo]]
//		packType:包裹类型
//		num:道具数量
//		itemInfo:道具数据

	if ( lstItemCache.empty() )
		return;

	BYTE byCount=0;

	string strData;
	S_APPEND_BYTE( strData, pkgType );

	S_APPEND_BYTE( strData, 0 );//数量

	for ( auto it = lstItemCache.begin(); it!=lstItemCache.end(); ++it )
	{
		ITEM_CACHE* itemCache = *it;

		ItemBriefInfo brefInfo(itemCache);

		S_APPEND_STRUCT( strData, brefInfo);
		byCount++;

		//lognew("putong: pkg:%d--itemtype:%d--%d--%s",pkgType,itemCache->itemCfg->type,itemCache->itemCfg->id,itemCache->itemCfg->name.c_str());

		//if (pkgType == PKG_TYPE_EQUIP)
		//{
		//	if( IS_PROPS(itemCache->itemCfg) )
		//	{
		//		lognew("error: %d--%s",itemCache->itemCfg->id,itemCache->itemCfg->name.c_str());
		//	}
		//}

	}
	
	*((BYTE *)&strData[1]) = byCount;

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_NOTIFY_NEW_ITEM, strData) );



}



string& mkItemBriefInfo( const list<ITEM_CACHE*> &lstItemCache, string & strData )
{
	S_APPEND_BYTE( strData, lstItemCache.size() );
	for ( auto it = lstItemCache.begin(); it!=lstItemCache.end(); ++it )
	{
		ITEM_CACHE* itemCache = *it;

		ItemBriefInfo brefInfo(itemCache);


		S_APPEND_STRUCT( strData, brefInfo);
	}

	return strData;
}

#define GET_LEFT_RIGHT(bodyIndex)		( ( bodyIndex == BODY_INDEX_BRACELET_L || bodyIndex == BODY_INDEX_RING_L ) ? 1 : 0 )

int ItemCtrl::clientGetItemBrief( ROLE* role, unsigned char * data, size_t dataLen )
{
//道具摘要信息
//	C:[BYTE]
//		[packType]
//		packType:包裹类型, 1:装备包裹; 2:普通包裹; 3:仓库包裹

//	S:[BYTE, DWORD, BYTE, [ItemBriefInfo]]
//		[grids, nextCost, num, [itemInfo]]
//		grids:已经开启的格子数
//		nextCost:下次开启新格子消耗元宝
//		num:道具数量
//		itemInfo:道具数据


	BYTE pkgType=0;
	if ( !BASE::parseBYTE( data, dataLen, pkgType) )
		return 0;
	
	string strData;

	if ( pkgType == PKG_TYPE_BODY )
	{
		S_APPEND_BYTE( strData, BODY_GRID_NUMS );
		S_APPEND_DWORD( strData, 0 );

		S_APPEND_BYTE( strData, 0 );
		BYTE byCount = 0;
	
		for ( int i=0; i<BODY_GRID_NUMS; i++)
		{

			ITEM_CACHE* itemCache = role->vBodyEquip[i];
			if ( itemCache == NULL )
				continue;

			if ( itemCache->itemCfg == NULL )
			{
				 role->vBodyEquip[i] = NULL;
				continue;
			}

			if (!IS_EQUIP(itemCache->itemCfg))
			{
				role->vBodyEquip[i] = NULL;
				continue;
			}

			ItemBriefInfo brefInfo(itemCache);

			brefInfo.byEquipLeftRight = 1;//GET_LEFT_RIGHT(i);

			byCount++;
			S_APPEND_STRUCT( strData, brefInfo);
		}

		*((BYTE *)&strData[5]) = byCount;
	}
	else if ( pkgType == PKG_TYPE_EQUIP )
	{
		S_APPEND_BYTE( strData, role->m_packmgr.curEquipPkgLimit );
		S_APPEND_DWORD( strData, CONFIG::getPkgExtendCost( role, PKG_TYPE_EQUIP) );
		mkItemBriefInfo( role->m_packmgr.lstEquipPkg, strData );
	}
	else if ( pkgType == PKG_TYPE_FUWEN )
	{
		S_APPEND_BYTE( strData, role->m_packmgr.curFuwenPkgLimit );
		S_APPEND_DWORD( strData, CONFIG::getPkgExtendCost( role, PKG_TYPE_FUWEN) );
		mkItemBriefInfo( role->m_packmgr.lstFuwenPkg, strData );
	}
	else if ( pkgType == PKG_TYPE_ITEM )
	{
		S_APPEND_BYTE( strData, role->m_packmgr.curItemPkgLimit );
		S_APPEND_DWORD( strData, CONFIG::getPkgExtendCost( role, PKG_TYPE_ITEM) );
		mkItemBriefInfo( role->m_packmgr.lstItemPkg, strData );
	}
	//else if ( pkgType == PKG_TYPE_MINE )
	//{

	//	S_APPEND_BYTE( strData, role->m_packmgr.curMineLimit );
	//	S_APPEND_DWORD( strData, CONFIG::getPkgExtendCost( role, PKG_TYPE_MINE) );		
	//	mkItemBriefInfo( role->m_packmgr.lstMinePkg, strData );
	//}
	else
	{
		return 0;
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ITEMS_BRIEF, strData) );

	return 0;
}


string& mkItemPro( string& strData, ITEM_CACHE* itemCache )
{
	//基础属性
	size_t attr_count = 0;
	for ( int j=0; j<MAX_EQUIP_BASE_ATTR_NUMS; j++)
	{
		if ( !IS_VALID_ATTR( itemCache->attrValueBase[ j ].attr  ) )
			continue;
		if(itemCache->attrValueBase[ j ].value != 0)attr_count++;
	}

	S_APPEND_BYTE( strData, (byte)attr_count );
	for ( int j=0; j<MAX_EQUIP_BASE_ATTR_NUMS; j++)
	{
		BYTE attr = itemCache->attrValueBase[ j ].attr;

		if ( !IS_VALID_ATTR( attr  ) )
			continue;

		if(itemCache->attrValueBase[ j ].value != 0)
		{
			S_APPEND_BYTE( strData, attr );
			S_APPEND_DWORD( strData, itemCache->attrValueBase[ j ].value );
		}
	}

	S_APPEND_BYTE( strData, (byte)itemCache->equip_extends.size() );
	for(size_t i = 0; i < itemCache->equip_extends.size(); i++)
	{
		S_APPEND_BYTE( strData, itemCache->equip_extends[i].type );
		S_APPEND_DWORD( strData, *(unsigned*)(void*)&itemCache->equip_extends[i].value );
	}
	////附加属性,不受强化等级影响
	//for ( int j=0; j<MAX_EQUIP_EXTRA_ATTR_NUMS; j++)
	//{
	//	BYTE attr = itemCache->attrValueExtra[ j ].attr;

	//	if ( !IS_VALID_ATTR( attr  ) )
	//		continue;

	//	S_APPEND_BYTE( strData, attr + CLIENT_SHOW_TYPE_BASE );
	//	S_APPEND_DWORD( strData, itemCache->attrValueExtra[ j ].value );
	//}

	//if ( IS_LINGZHU( itemCache->itemCfg ) )
	//{
	//	S_APPEND_BYTE( strData, E_ATTR_LINGZHU );
	//	S_APPEND_DWORD( strData, itemCache->dwLingZhuExp );
	//}

	
//	S_APPEND_BYTE( strData, ROLE_PRO_POWER );
//	S_APPEND_WORD( strData, itemCache->wFightValue );

	return strData;

}

int ItemCtrl::clientGetItemDetail( ROLE* role, unsigned char * data, size_t dataLen )
{

//	C:[DWORD]
//		[itemId]
//		itemId:道具唯一ID
//	S:[WORD, DWORD, [BYTE, WORD]]
//		[error, itemId, [attrType, attrVal]]
//		error:错误码
//			0:成功
//			1:道具不存在
//		itemId:道具ID
//		attrType:属性类型
//		attrVal:属性值



	ITEM_CACHE* item;
//	BYTE pkgType;
	
	if ( !BASE::parseDWORD( data, dataLen, (*(DWORD*)&item)) )
		return 0;

//	if ( !BASE::parseBYTE( data, dataLen, pkgType ) )
//		return -1;

	PKG_TYPE pkgType = role->m_packmgr.findItem(item );
	if(PKG_TYPE_NONE == pkgType)
		return 0;

	string strData;
	S_APPEND_WORD( strData, pkgType == PKG_TYPE_NONE ? 1 : 0 );	
	S_APPEND_DWORD( strData, item );
	if ( pkgType != PKG_TYPE_NONE  && IS_EQUIP( item->itemCfg ) )
		mkItemPro( strData, item );
		
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_ITEM_DETAIL, strData) );
	return 0;
}

//int ItemCtrl::clientMoveItem( ROLE* role, unsigned char * data, size_t dataLen )
//{
//	return 0;
//
////道具包裹间移动
////#define S_MOVE_ITEM_TO	0x0023
////	C:[DWORD, BYTE, BYTE]
////		[itemId, srcPkgType, dstPkgType]
////			itemId:道具ID
////			srcPkgType:原包裹类型
////			dstPkgType:目的包裹类型
////	S:[WORD]
////		[error]
////		error:0:成功
////			1:道具不存在
////			2:目标包裹已满
////			3:不能移动到目的包裹,道具类型不匹配
//
//	DWORD itemId;
//	BYTE srcPkgType, dstPkgType;
//
//	if ( !BASE::parseDWORD( data, dataLen, itemId) )
//		return 0;
//	if ( !BASE::parseBYTE( data, dataLen, srcPkgType) )
//		return 0;
//	if ( !BASE::parseBYTE( data, dataLen, dstPkgType) )
//		return 0;
//
//	int moveRet = 0;
//	ITEM_CACHE* itemCache = (ITEM_CACHE*)itemId ;
//	do
//	{
//
//		COND_BREAK(srcPkgType == dstPkgType, moveRet, 3);
//
//		if ( srcPkgType == PKG_TYPE_EQUIP )
//		{
//			COND_BREAK(dstPkgType != PKG_TYPE_STORAGE, moveRet, 3);
//			//moveRet = itemMove( role, role->m_packmgr.lstEquipPkg, srcPkgType, role->m_packmgr.lstStorage, PKG_TYPE_STORAGE, itemCache );
//		}
//		else if ( srcPkgType == PKG_TYPE_MINE )
//		{
//			COND_BREAK(dstPkgType != PKG_TYPE_STORAGE, moveRet, 3);
//			//moveRet = itemMove( role, role->m_packmgr.lstMinePkg, srcPkgType, role->m_packmgr.lstStorage, PKG_TYPE_STORAGE, itemCache );
//		}
//		else if ( srcPkgType == PKG_TYPE_STORAGE )
//		{
//
//			if ( dstPkgType == PKG_TYPE_EQUIP )
//			{
//				//moveRet = itemMove( role, role->m_packmgr.lstStorage, PKG_TYPE_STORAGE, role->m_packmgr.lstEquipPkg, dstPkgType, itemCache );
//			}
//			else if ( dstPkgType == PKG_TYPE_MINE )
//			{
//				//moveRet = itemMove( role, role->m_packmgr.lstStorage, PKG_TYPE_STORAGE, role->m_packmgr.lstMinePkg, dstPkgType, itemCache );
//			}
//			else
//			{
//				moveRet = 3;
//			}
//
//		}
//		else
//		{
//			moveRet = 3;
//		}
//
//
//	}while(0);
//
//	string strData;
//	S_APPEND_WORD( strData, moveRet );
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_MOVE_ITEM_TO, strData) );
//	return 0;
//}


string& getRoleFightPro(ROLE*role, string& updateData)
{
	for ( int i=E_ATTR_HP_MAX2; i< E_ATTR_MAX; i++)
	{
		S_APPEND_BYTE( updateData, i );
		S_APPEND_DWORD( updateData, role->roleAttr[i] );	
	}
	return updateData;
}

void ItemCtrl::updateNotifyRoleAttr(ROLE* role, bool withMaxExp)
{
	DWORD roleAttr[E_ATTR_MAX];
	memcpy( roleAttr, role->roleAttr, sizeof(role->roleAttr) );

	vector<WORD> vecTempSuit( role->vecSuit );

	role->calRoleAttr( );

	string updateData;

	S_APPEND_BYTE( updateData, ROLE_PRO_WARPWR );
	S_APPEND_DWORD( updateData, role->dwFightValue );
	
	for ( int i=E_ATTR_HP_MAX2; i< E_ATTR_MAX; i++)
	{
		if ( roleAttr[i] != role->roleAttr[i] )
		{
			S_APPEND_BYTE( updateData, i );
			S_APPEND_DWORD( updateData, role->roleAttr[i] );
		}
	}

	if ( withMaxExp )
	{
		S_APPEND_BYTE( updateData, ROLE_PRO_MAX_EXP );
		S_APPEND_DWORD( updateData, CONFIG::getRoleUpgradeExp(role->wLevel ) );
	}

	if ( !updateData.empty() )
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UPDATE_ROLE_DATA, updateData) );

	string suitData;
	if ( vecTempSuit != role->vecSuit )//有变化的
	{
		S_APPEND_WORD( suitData, 0);
		for ( auto& one : role->vecSuit )
		{
			S_APPEND_BYTE( suitData, (BYTE)one );
		}

		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_SUIT_INFO, suitData) );
	}
}


#if 0

#define notifyCoin( role )	 \
	do {\
		string strData;\
		S_APPEND_BYTE( strData, ROLE_PRO_COIN );\
		S_APPEND_DWORD( strData, role->dwCoin );\
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UPDATE_ROLE_DATA, strData) );\
	}

#define notifyExp( role )		\
	do {\
		string strData;\
		S_APPEND_BYTE( strData, ROLE_PRO_CUREXP );\
		S_APPEND_DWORD( strData, role->dwExp );\
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UPDATE_ROLE_DATA, strData) );\
	}

#endif

enum
{
						//			0:成功
	E_UNOPEN = 	1,		//1:地图未开放
	E_MAP_LEVEL_LIMIT = 2,	//2:角色等级未达到
	E_NOT_EXIST = 3,		//3:地图不存在
};


//CONFIG::MAP_CFG* ItemCtrl::getFightMap( ROLE* role, WORD wMapID, BYTE &byRet )
//{
//	if ( wMapID > role->wCanFightMaxMapID )
//	{
//		byRet = E_UNOPEN;
//		return NULL;
//	}
//
//	CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(wMapID);
//	if ( mapCfg == NULL )
//	{
//		byRet = E_NOT_EXIST;
//		return NULL;
//	}
//	
//	if ( mapCfg->level_limit > role->wLevel )
//	{
//		//等级放在外面判断
//		byRet = E_MAP_LEVEL_LIMIT;
//		//return NULL;
//		return mapCfg;
//	}
//
//	byRet = 0;
//	return mapCfg;
//}

int ItemCtrl::setMapMonster( ROLE* role, WORD wMapID)
{
	//enum eFIGHT
	//{
	//	ENTER_MAP_SUCCESS = 0,     //成功
	//	ENTER_MAP_NO_EXISTS = 1,   //地图不存在
	//	ENTER_MAP_NOT_OPEN = 2,    //地图未开放
	//	ENTER_MAP_LOW_LEVEL = 3,   //角色等级未达到
	//	FIGHT_MON_NO_COOL = 4,     //冷却时间未到
	//	FIGHT_MON_NO_EXISTS = 5,   //本地图怪物不存在
	//	DROP_ITEM_FULL = 6,   	   //背包已满
	//	DROP_MINE_FULL = 7,        //矿石背包已满
	//};

	if ( wMapID > role->wCanFightMaxMapID )
	{
		return 2;
	}

	CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(wMapID);
	if ( mapCfg == NULL )
	{
		return 1;
	}

//#if 0
//	if ( mapCfg->level_limit > role->wLevel )
//	{
//		return 3;
//	}
//#endif
	//DWORD tNow = (DWORD)PROTOCAL::getCurTime();
	//if ( tNow < role->dwCanEnterFightMapTime && role->dwCanEnterFightMapTime != 0 )
	//{
	//	return 4;
	//}

	////下一次可以进入的时间
	//role->dwCanEnterFightMapTime = tNow + mapCfg->cool_down;
	//
	//role->lstMonsterIndex.clear();

	////if ( !fightBoss )
	//{
	//	
	//	//vector<DWORD> vecMonsterDropID;
	//	int nums = BASE::randBetween( mapCfg->dwMin, mapCfg->dwMax+1 );

	//	for ( int i=0; i<nums; i++ )
	//	{
	//		RETURN_COND( mapCfg->vecMonsterRandSum.empty() , 0 );

	//		int index = BASE::getIndexOfVectorSum( mapCfg->vecMonsterRandSum );

	//		WORD monsIndex = mapCfg->vecMonsterIndex[index];

	//		role->lstMonsterIndex.push_back( monsIndex );
	//	}

	//}

	auto old_map_id = role->wLastFightMapID;
	role->wLastFightMapID = wMapID;
	service::player_enter_map(role, old_map_id);
	
	return 0;

}


// 	预估经验获得计算
//3)	预估经验=（上5场总共获得经验/5）*（3600秒/（上5场总共花费的秒数/5））
// 	预估经验获得计算
//4)	预估金币=（上5场总共获得金币/5）*（3600秒/（上5场总共花费的秒数/5））
// 	预估装备获得计算
//5)	预估装备=（上5场总共获得装备数量/5）*（3600秒/（上5场总共花费的秒数/5））




#define NEXT_PREDICT_INDEX(role) \
	do {\
		role->byPredictIndex++;\
		if ( role->byPredictIndex >= PREDICT_NUMS )\
			role->byPredictIndex =0;\
	}while(0)


int ItemCtrl::clientGetFightPredictAward( ROLE* role, unsigned char * data, size_t dataLen )
{

//	C:[]
//	S:[DWORD, DWORD, DWORD]
//		[expAward, coinAward, equipAward]

	DWORD dwExpSum = 0;
	DWORD dwCoinSum = 0;
	WORD wItemNumsSum = 0;
	WORD wTimeIntvSum = 0;

	for ( int i=0; i<PREDICT_NUMS; i++)
	{
		if ( role->predictEarning[ i ].wTimeIntv == 0 )
			continue;

		dwExpSum += role->predictEarning[ i ].dwExp;
		dwCoinSum += role->predictEarning[ i ].dwCoin;
		wItemNumsSum += role->predictEarning[ i ].wItemNums;
		wTimeIntvSum += role->predictEarning[ i ].wTimeIntv;
	}

	DWORD expAward=0, coinAward=0, equipAward=0;
	//每秒获得的经验
	if ( wTimeIntvSum && role->byNewMapKillMonNum > 0)
	{

		float coff = 1.0f + 0.1f * ( role->byNewMapKillMonNum-1 );
		
		expAward = DWORD( (3600.0f / wTimeIntvSum) * dwExpSum * 0.8f * coff );
		coinAward = DWORD( (3600.0f / wTimeIntvSum) * dwCoinSum * 0.8f * coff );
		equipAward = DWORD( (3600.0f / wTimeIntvSum) * wItemNumsSum * 0.8f * coff );
		//float coff = 0;

		//if ( role->wLastFightMapID <= 99 && role->vMapInitFightValue[ role->wLastFightMapID ] )
		//{
		//	if (  role->dwFightValue > role->vMapInitFightValue[ role->wLastFightMapID ] )
		//	{
		//		DWORD delta = role->dwFightValue - role->vMapInitFightValue[ role->wLastFightMapID ];
		//		
		//		//每涨1000点,多0.1 %, 最多10%
		//		coff = ( float) delta / 1000 * 0.001f;
		//		if ( coff > 0.1f )
		//			coff = 0.1f;
		//	
		//	}
		//}

		//expAward = (DWORD) ( (float ) dwExpSum*(1+coff) / wTimeIntvSum * 3600 );
		//coinAward = (DWORD) ( (float ) dwCoinSum*(1+coff) / wTimeIntvSum* 3600 );
		//
		//CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(role->wLastFightMapID);
		//if ( mapCfg  )
		//{
		//	//先按刷10只怪算		
		//	//equipAward = (DWORD) ( (float ) wItemNumsSum / wTimeIntvSum* 3600 );
		//	equipAward = (DWORD) ( (float)3600 *10 / mapCfg->cool_down * 0.05f );
		//}
	}

	string strData;
	S_APPEND_DWORD( strData, expAward );
	S_APPEND_DWORD( strData, coinAward );
	S_APPEND_DWORD( strData, equipAward );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_AWARDS, strData) );

	return 0;

}

bool ItemCtrl::killMapMonster(ROLE* role, WORD wMapID, BYTE byDropType, list<ITEM_CACHE*>* pLstItemNotify, list<ITEM_CACHE*>* pLstPropsNotify, list<ITEM_CACHE*>* pLstAutoSellItem, list<ITEM_CACHE*>* pLstNoSellItem, uint* pvAdd)
{

	if ( byDropType >= E_DROP_NONE )
	{
		return false;
	}
	else if ( byDropType == E_DROP_FIRST_HERO )
	{
		byDropType = E_DROP_FIRST_BOSS;
	}
	else if ( byDropType == E_DROP_HERO)
	{
		byDropType = E_DROP_BOSS;
	}
	
	CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg( wMapID );
	if ( mapCfg == NULL)
	{
		return false;
	}

	bool bNoAutoSell;
	WORD levelOrig = role->wLevel;
	DWORD expOrig = role->dwExp;
	DWORD coinOrig = role->dwCoin;

	//关卡获胜经验=关卡配置经验×（1-等级差系数）
	//等级差系数：当角色的等级和关卡等级相差小于等于10，则为0；相差大于10，则每多1级系数10%，最多到100%

	//等级差系数：当角色的等级和关卡等级相差小于等于20，则为0；相差大于20，则每多1级系数增加5%，最多到100%

	int nLevel = abs( (int)role->wLevel - (int)mapCfg->level_limit );

	float succExp = nLevel <= 20 ? 0 : ( nLevel >= 40 ? 1 : ( ( nLevel - 20 ) * 0.05f ) );

	DWORD dwExp = DWORD( (float)mapCfg->exp * ( 1 - succExp ) );

	DWORD dwCoin = 0;

	//WORD itemIndex = 0;

	CONFIG::DROP_MONEY& dropMoney = mapCfg->stDropMoney[ byDropType ];

	// 开服七天双倍
	auto end_time = service::open_time + 3600 * 24 * 7;
	if (uint(time(nullptr)) < end_time)
	{
		dropMoney.moneyrate *= 2.0f;
	}
	if ( RAND_HIT(dropMoney.moneyrate) )
	{
		dwCoin = BASE::randBetween(dropMoney.minMoney, dropMoney.maxMoney + 1);
	}

	dwCoin += DWORD(dwCoin*(guild::get_shrine_addcoin_coff(role)));

	// 开服七天双倍
	if (uint(time(nullptr)) < end_time)dwCoin *= 2;

	if (pvAdd)
	{
		pvAdd[0] = dwCoin;
		pvAdd[2] = dwExp;
	}

	CONFIG::addRoleExp( role, role->dwExp, dwExp, true );

	ADDUP( role->dwCoin, dwCoin );
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, dwCoin, 0, purpose::kill_monster, 0);

	vector<ITEM_INFO> tryV;
	if ( !CONFIG::getRandItem( mapCfg->dropID[ byDropType ], tryV ) )
		goto out;

	for ( auto &ii : tryV )
	{
		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( ii.itemIndex );
		if ( itemCfg==NULL )
			continue;

		//不会有矿石
		ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, ii.itemNums, (BYTE)ii.byQuality );//传出之后delete
		bNoAutoSell = role->m_packmgr.makeItem3( itemCache, pLstItemNotify, pLstPropsNotify);

		if ( bNoAutoSell )
		{
			if (pLstNoSellItem)
			{
				pLstNoSellItem->push_back(itemCache);
			}
			else//不传出的时候   传出的话在外析构
			{
				CC_SAFE_DELETE(itemCache);
			}
		}
		else
		{
			if (pLstAutoSellItem)
			{
				pLstAutoSellItem->push_back(itemCache);
			}
			else
			{
				CC_SAFE_DELETE(itemCache);
			}
		}
	}

out:
	//离线的不需要
	if ( pLstItemNotify || pLstPropsNotify )
	{
		time_t tNow = time(nullptr);
		if ( tNow > role->lastKillMonAddExpTime && tNow - role->lastKillMonAddExpTime < 100 )
		{
			role->predictEarning[ role->byPredictIndex ].dwExp = mapCfg->exp;
			role->predictEarning[ role->byPredictIndex ].dwCoin = (WORD)dwCoin;
			role->predictEarning[ role->byPredictIndex ].wItemNums = tryV.size();
			role->predictEarning[ role->byPredictIndex ].wTimeIntv = (WORD)(tNow - role->lastKillMonAddExpTime);
			NEXT_PREDICT_INDEX( role );
		}

		role->lastKillMonAddExpTime = tNow;

		notify_X_If( role, ROLE_PRO_LEVEL, wLevel, levelOrig );
		notify_X_If( role, ROLE_PRO_CUREXP, dwExp, expOrig );
		notify_X_If( role, ROLE_PRO_COIN, dwCoin, coinOrig );

		if ( role->byNewMapKillMonNum < 100 )
		{
			role->byNewMapKillMonNum++;
		}

		ItemCtrl::clientGetFightPredictAward( role, NULL, 0 );
	}

	return true;
}

//bool ItemCtrl::killMonster( ROLE* role, WORD wMonIndex, DWORD dwMonNums, list<ITEM_CACHE*>* pLstItemNotify, list<ITEM_CACHE*>* pLstPropsNotify, list<ITEM_CACHE*>* pLstAutoSellItem, list<ITEM_CACHE*>* pLstNoSellItem  )
//{
//
//	CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(wMonIndex);
//	if ( monCfg == NULL)
//		return false;
//	bool bNoAutoSell;
//	WORD levelOrig = role->wLevel;
//	DWORD expOrig = role->dwExp;
//	DWORD coinOrig = role->dwCoin;
//
//	DWORD dwExp = dwMonNums * monCfg->exp;
//	DWORD dwCoin = dwMonNums * monCfg->gold;
//	
//	CONFIG::addRoleExp( role, role->dwExp, dwExp, true );
//
//	ADDUP( role->dwCoin, dwCoin );
//
//	//if ( role->wLevel > levelOrig  )
//	//{
//	//	//RoleMgr::getMe().onLevelUP( role, levelOrig );
//
//	//	if ( pLstItemNotify )
//	//	{
//	//		updateNotifyRoleAttr(role, true);
//
//	//		//离线的时候不能在这里广播,playinfo还没传给客户端
//	//		BroadcastCtrl::levelUpgrade( role );
//	//	}
//
//	//}
//
//	vector<ITEM_INFO> tryV;
//	if ( !CONFIG::getRandItem( monCfg->drop_id, tryV ) )
//		goto out;
//
//	for ( auto &ii : tryV )
//	{
//
//		if ( dwMonNums > 1 )
//			ii.itemNums *= dwMonNums;
//
//		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( ii.itemIndex );
//		if ( itemCfg==NULL )
//			continue;
//
//		//不会有矿石
//		ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, ii.itemNums, (BYTE)ii.byQuality );
//		bNoAutoSell = role->m_packmgr.makeItem3( itemCache, pLstItemNotify, pLstPropsNotify);
//		
//		if ( bNoAutoSell )
//		{
//			if (pLstNoSellItem)
//			{
//				pLstNoSellItem->push_back(itemCache);
//			}
//			else
//			{
//				delete itemCache;
//			}
//		}
//		else
//		{
//			if (pLstAutoSellItem)
//			{
//				pLstAutoSellItem->push_back(itemCache);
//			}
//			else
//			{
//				delete itemCache;
//			}
//			
//		}
//		
//		//if (pLstItemDrop)
//		//{
//		//	pLstItemDrop->push_back(itemCache);
//		//}
//		//else
//		//{
//		//	delete itemCache;
//		//}
//	}
//
//out:
//	
//	//离线的不需要
//	if ( pLstItemNotify || pLstPropsNotify )
//	{
//		time_t tNow = PROTOCAL::getCurTime();
//		if ( tNow > role->lastKillMonAddExpTime && tNow - role->lastKillMonAddExpTime < 100 )
//		{
//			role->predictEarning[ role->byPredictIndex ].dwExp = monCfg->exp;
//			role->predictEarning[ role->byPredictIndex ].dwCoin = (WORD)monCfg->gold;
//			role->predictEarning[ role->byPredictIndex ].wItemNums = tryV.size();
//			role->predictEarning[ role->byPredictIndex ].wTimeIntv = (WORD)(tNow - role->lastKillMonAddExpTime);
//			NEXT_PREDICT_INDEX( role );
//		}
//
//		role->lastKillMonAddExpTime = tNow;
//
//		notify_X_If( role, ROLE_PRO_LEVEL, wLevel, levelOrig );
//		notify_X_If( role, ROLE_PRO_CUREXP, dwExp, expOrig );
//		notify_X_If( role, ROLE_PRO_COIN, dwCoin, coinOrig );
//
//		if ( role->byNewMapKillMonNum < 100 )
//		{
//			role->byNewMapKillMonNum++;
//		}
//		
//		ItemCtrl::clientGetFightPredictAward( role, NULL, 0 );
//	}
//
//	return true;
//
//}


int ItemCtrl::clientEquipAttach( ROLE* role, unsigned char * data, size_t dataLen )
{

//穿装备
//	C:[BYTE, BYTE, DWORD]
//		[pkgType, targetGrid, itemId]
//		pkgType:包裹类型
//		targetGrid:目标格子.=0:服务器自动选择, != 0,客户端选择目标格子装备.
//	S:[BYTE, BYTE]
//		[error, tagetGrid]
//		error:0:成功
//			[1] = "装备不存在",
//			[2] = "等级不足",
//			[3] = "等级不足",
//			[4] = "职业不匹配",
//			[5] = "性别限制",
//		tagetGrid:装备到目标格子位



	BYTE pkgType;
	DWORD itemId;
	BYTE clientDstGrid;
	
	if ( !BASE::parseBYTE( data, dataLen, pkgType) )
		return 0;
	
	if ( !BASE::parseBYTE( data, dataLen, clientDstGrid) )
		return 0;
	
	if ( !BASE::parseDWORD( data, dataLen, itemId) )
		return 0;

	if ( clientDstGrid ==0 || clientDstGrid >= BODY_GRID_NUMS )
		return 0;

	int tgtBodyIndex = /*clientDstGrid ==0 ? -1 : */clientDstGrid-1;//服务器类型
	

	ITEM_CACHE* itemCache = (ITEM_CACHE*)itemId ;

	int ret = ItemCtrl::equip( role,  itemCache, pkgType, tgtBodyIndex );

	string strData;
	S_APPEND_BYTE( strData, ret );
	S_APPEND_BYTE( strData, clientDstGrid );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EQUIP_ATTACH, strData) );

	if ( ret == E_SUCC )
	{
		updateNotifyRoleAttr(role);

		if (tgtBodyIndex == BODY_INDEX_WEAPON/* && itemCache->itemCfg->sub_type == SUB_TYPE_WEAPON*/)
		{
			RANK::insertWeaponListRank( role, itemCache );
		}

		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_EQUIP);

		BYTE byMinEquipLevel = role->getMinEquipLevel();
		ITEM_INFO para(0, (DWORD)byMinEquipLevel);
		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_ALL_EQUIP_LEVEL, &para);

		DWORD dwSuitNum = role->vecSuit.size();
		ITEM_INFO para1(0, dwSuitNum);
		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_GET_SUIT_TIMES, &para1);

	}

	return 0;
}

int ItemCtrl::clientEquipDettach( ROLE* role, unsigned char * data, size_t dataLen )
{

//脱装备
//#define S_EQUIP_DETACH	0x0025
//	C:[BYTE, DWORD]
//		[pkgType, itemId]
//		pkgType:包裹类型
//	S:[WORD]
//		[error]
//		error:0:成功
//			1:装备不存在
//			2:包裹满


	//BYTE pkgType;
	//DWORD itemId;

	//if ( !BASE::parseBYTE( data, dataLen, pkgType) )
	//	return -1;
	//
	//if ( !BASE::parseDWORD( data, dataLen, itemId) )
	//	return -1;

	//ITEM_CACHE* itemCache = (ITEM_CACHE*)itemId ;


	//int bodyIndex = -1;
	//
	//for ( int i=0; i<BODY_GRID_NUMS; i++)
	//{
	//	if ( role->vBodyEquip[i] == itemCache )
	//	{
	//		bodyIndex = i;
	//		break;
	//	}
	//}

	//int ret = E_ITEM_NOT_EXIST;
	//if ( bodyIndex!= -1 )
	//	ret = unEquip( role, bodyIndex, pkgType );

	//string strData;
	//S_APPEND_WORD( strData, ret );
	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EQUIP_DETACH, strData) );

	//if ( ret == E_SUCC )
	//{
	//	updateNotifyRoleAttr(role);
	//	RANK::eraseWeaponListRank(role->dwRoleID, itemCache->itemCfg->id);
	//}
	//
	return 0;

}


#define PUSH_BACK_IF_NOT_EXIST(list, e)		\
	do {\
		if ( find( list.begin(), list.end(), e) == list.end() ) list.push_back(e);\
	}while(0)



int ItemCtrl::clientSellItems( ROLE* role, unsigned char * data, size_t dataLen )
{

//道具出售
//#define S_SELL_ITEMS	0x0026
//	C:[BYTE, BYTE, [DWORD]]
//		[pkgType, itemnums, [itemId]]
//		pkgType:包裹类型
//		itemnums:出售数量
//		itemId:道具ID
//	S:[WORD]
//		[error]
//		error:错误码, 0:成功
//			1:失败

	BYTE pkgType, sellNums;

	if ( !BASE::parseBYTE( data, dataLen, pkgType) )
		return 0;
	
	if ( !BASE::parseBYTE( data, dataLen, sellNums) )
		return 0;

	list<ITEM_CACHE*>* P = pkgType == PKG_TYPE_EQUIP ? &role->m_packmgr.lstEquipPkg
						: pkgType == PKG_TYPE_ITEM ? &role->m_packmgr.lstItemPkg
						: pkgType == PKG_TYPE_FUWEN ? &role->m_packmgr.lstFuwenPkg
						: NULL;
						/*: pkgType == PKG_TYPE_MINE ? &role->m_packmgr.lstMinePkg*/
						

	WORD wErrCode = 1;
		
	if ( P != NULL )
	{
		list<ITEM_CACHE*>& lstDst=*P;

		list<ITEM_CACHE*> lstSell;
		for ( BYTE i=0; i<sellNums; i++)
		{
			DWORD itemId;
			if ( !BASE::parseDWORD( data, dataLen, itemId) )
				return -1;

			ITEM_CACHE* itemCache = (ITEM_CACHE* )itemId;
			auto it = find( lstDst.begin(), lstDst.end(), itemCache );
			if ( it == lstDst.end() )
				goto out;

			PUSH_BACK_IF_NOT_EXIST( lstSell, itemCache );
			//lstSell.push_back( itemCache );
		}

		for ( auto it = lstSell.begin(); it!=lstSell.end(); ++it )
		{
			//服务器不向客户端刷新包裹,装备
			ITEM_CACHE* itemCache = *it;
			auto price = itemCache->itemCfg->price * itemCache->itemNums;
			ADDUP( role->dwCoin,  price);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, price, 0, purpose::sell_item, itemCache->itemCfg->id);

			role->m_packmgr.rmItemInPkg( itemCache);
		}

		notifyCoin( role );

		wErrCode = 0;
	}

out:
	{
		string strData;
		S_APPEND_WORD( strData, wErrCode );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SELL_ITEMS, strData) );
	}

	return 0;
}

DWORD ItemCtrl::rmIDItemNotify( ROLE* role, list<ITEM_CACHE*>& lstItemCache, PKG_TYPE pkgType, const WORD id, const DWORD dwDelNum )
{
	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(id);

	RETURN_COND( role==NULL || itemCfg==NULL, 0);

	return ItemCtrl::rmItemNotify( role, role->m_packmgr.lstItemPkg, PKG_TYPE_ITEM, itemCfg, dwDelNum );
}
//返回剩余数量
DWORD ItemCtrl::rmItemNotify( ROLE* role, list<ITEM_CACHE*>& lstItemCache, PKG_TYPE pkgType, CONFIG::ITEM_CFG* itemCfg, DWORD dwDelNum )
{
	//已经确保有此index,数量足够
	RETURN_COND( dwDelNum == 0, 0 )

	list<ITEM_CACHE*> lstNotifyPkg;

	for ( auto it = lstItemCache.begin(); it!=lstItemCache.end();  )
	{
		if ( itemCfg->id != (*it)->itemCfg->id )
		{
			++it;
			continue;
		}

		if ( (*it)->itemNums > dwDelNum )//等于不能加
		{
			(*it)->itemNums -= dwDelNum;

			dwDelNum = 0;

			lstNotifyPkg.push_back( (*it) );
		}
		else
		{
			dwDelNum -= (*it)->itemNums;

			(*it)->itemNums = 0;

			lstNotifyPkg.push_back( (*it) );

			it = lstItemCache.erase( it );
		}
		
		BREAK_COND( dwDelNum == 0 );
	}

	ItemCtrl::notifyClientItem(role, lstNotifyPkg, pkgType);

	for ( auto& ele : lstNotifyPkg )
	{
		if ( ele!=NULL && ele->itemNums == 0 )
		{
			CC_SAFE_DELETE(ele);
		}
	}

	return dwDelNum;
}

DWORD ItemCtrl::subItemNotify( ROLE* role, list<ITEM_CACHE*>& lstItemCache, PKG_TYPE pkgType, ITEM_CACHE* item, DWORD dwDelNum )
{
	PKG_TYPE weaponPos = role->m_packmgr.findItem(item);

	if (weaponPos != pkgType)
	{
		return 0;
	}
	if (role==NULL || item==NULL)
	{
		return 0;
	}

	if ( dwDelNum == 0 )
		return 0;

	list<ITEM_CACHE*> lstNotifyPkg;

	if ( (DWORD)item->itemNums > dwDelNum )
	{
		item->itemNums -= (BYTE)dwDelNum;
		dwDelNum = 0;
	}
	else
	{
		dwDelNum -= item->itemNums;
		item->itemNums = 0;
		lstItemCache.remove( item );
	}

	lstNotifyPkg.push_back( item );

	if ( dwDelNum != 0 )
	{	
		for ( auto it = lstItemCache.begin(); it!=lstItemCache.end();  )
		{
			if ( item->itemCfg != (*it)->itemCfg )
			{
				++it;
				continue;
			}

			auto itemCache = *it;

			if ( (DWORD)itemCache->itemNums > dwDelNum )
			{
				itemCache->itemNums -= dwDelNum;
				dwDelNum = 0;
			}
			else
			{
				dwDelNum -= itemCache->itemNums;
				itemCache->itemNums = 0;
				lstItemCache.erase( it++ );
			}
			//}

			lstNotifyPkg.push_back( itemCache );

			if ( dwDelNum == 0 )
				break;
		}
	}
	
	ItemCtrl::notifyClientItem(role, lstNotifyPkg, pkgType);

	for ( auto &it : lstNotifyPkg )
	{
		if ( it->itemNums == 0 )
		{	
			CC_SAFE_DELETE(it);
		}
	}

	return dwDelNum;
}

int  ItemCtrl::clientPreViewStrengthenInfo( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	DWORD itemId = 0;
	if ( !BASE::parseDWORD( data, dataLen, itemId) )
		return 0;
	
	ITEM_CACHE* itemWeapon =(ITEM_CACHE* )itemId;

	PKG_TYPE weaponPos = role->m_packmgr.findItem( itemWeapon);

	if ( itemWeapon == NULL || PKG_TYPE_NONE == weaponPos || itemWeapon->itemCfg == NULL || !IS_EQUIP( itemWeapon->itemCfg) )
	{
		return 0;
	}

	WORD wStrengthLevel = itemWeapon->byStrengthLv;

	WORD wItemLevel = itemWeapon->itemCfg->level_limit;

	WORD wSubType = itemWeapon->itemCfg->sub_type;

	const DWORD dwNeedCoin = ( wStrengthLevel + 5 ) * 10000; // 需要金币数

	const float fNeedRatio = wSubType > 10 ? strength_ratio[0] : strength_ratio[wSubType];

	DWORD dwStoneNums = DWORD(ceil(fNeedRatio *  (pow((wStrengthLevel + 1), 1.7) + pow((wItemLevel + 1) / 3, (1.3 + 0.1*wStrengthLevel)))));

	string strData;
	S_APPEND_WORD(strData, (WORD)itemWeapon->itemCfg->attType);
	S_APPEND_WORD(strData, (WORD)itemWeapon->itemCfg->attValue);
	S_APPEND_DWORD(strData, dwStoneNums);
	S_APPEND_DWORD(strData, dwNeedCoin);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_STRENGTHEN_PREVIEW, strData) );

	return 0;
}

int  ItemCtrl::clientConfirmStrengthenInfo( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	DWORD itemId = 0;
	if ( !BASE::parseDWORD( data, dataLen, itemId) )
		return 0;

	ITEM_CACHE* itemWeapon =(ITEM_CACHE* )itemId;

	PKG_TYPE weaponPos = role->m_packmgr.findItem( itemWeapon );

	if (itemWeapon==NULL || PKG_TYPE_NONE == weaponPos || itemWeapon->itemCfg == NULL || !IS_EQUIP( itemWeapon->itemCfg))
	{
		WORD dwError = 4; // 装备不存在
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_CONFIRM_STRENGTHEN, strData) );
		return 0;
	}

	const WORD STONE_ID = 402; // 强化石

	WORD wStrengthLevel = itemWeapon->byStrengthLv;

	WORD wItemLevel = itemWeapon->itemCfg->level_limit;

	WORD wSubType = itemWeapon->itemCfg->sub_type;

	const DWORD dwNeedCoin = ( wStrengthLevel + 5 ) * 10000; // 需要金币数

	const float fNeedRatio  = wSubType > 10 ? strength_ratio[0] : strength_ratio[wSubType];

	DWORD dwStoneNums = DWORD( ceil ( fNeedRatio *  ( pow((wStrengthLevel + 1), 1.7) + pow( (wItemLevel + 1) / 3, (1.3 + 0.1*wStrengthLevel ) ) ) ) );

	if (!role->m_packmgr.hasEnoughItem(STONE_ID, dwStoneNums))
	{
		WORD dwError = 2; //强化石不足
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_CONFIRM_STRENGTHEN, strData) );
		return 0;
	}

	if (!role->hasEnoughCoin(dwNeedCoin))
	{
		WORD dwError = 1; //金币不足
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_CONFIRM_STRENGTHEN, strData) );
		return 0;
	}

	if (itemWeapon->isStrengthenMaxLevel())
	{
		// 已达强化最大等级
		WORD dwError = 3; 
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_CONFIRM_STRENGTHEN, strData) );
		return 0;
	}

	itemWeapon->levelUpStrengthen(); // 强化升级
	
	//如果在身上
	if ( PKG_TYPE_BODY == weaponPos )
	{
		updateNotifyRoleAttr( role );
		int bodyIndex = getBodyIndexBySubtype( role, itemWeapon->itemCfg->sub_type );
		if (bodyIndex == BODY_INDEX_WEAPON)
		{
			RANK::insertWeaponListRank(role, itemWeapon);
		}	
	}
	else if ( PKG_TYPE_SLAVE == weaponPos )
	{
		updateMerAttrByEquip( role, itemWeapon );
	}

	notifyItem( role, itemWeapon, weaponPos);

	ITEM_INFO para(0, 0, itemWeapon->byStrengthLv);
	RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_HAS_EQUIP_ENHANCE, &para );
	RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_DO_EQUIP_ENHANCE );

	// 扣除金币
	role->subCoin(dwNeedCoin);
	// 扣除强化石
	role->m_packmgr.consumeItem(STONE_ID, dwStoneNums);

	// 强化成功
	{
		WORD dwError = 0; 
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_CONFIRM_STRENGTHEN, strData) );

		ADDUP(itemWeapon->dwGetStrengthStoneNum, (int)dwStoneNums);
	}


	return 0;
}

int  ItemCtrl::clientPunchHole( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	DWORD itemId = 0;
	if ( !BASE::parseDWORD( data, dataLen, itemId) )
		return 0;

	ITEM_CACHE* itemWeapon =(ITEM_CACHE* )itemId;
	PKG_TYPE weaponPos = role->m_packmgr.findItem( itemWeapon);

	if ( itemWeapon == NULL || PKG_TYPE_NONE == weaponPos || itemWeapon->itemCfg == NULL || !IS_EQUIP( itemWeapon->itemCfg) )
	{
		WORD dwError = 1; // 装备不存在
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_MAKE_HOLE, strData) );
		return 0;
	}

	if (itemWeapon->isHoleFull())
	{
		WORD dwError = 3; // 孔位已满
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_MAKE_HOLE, strData) );
		return 0;
	}

	const WORD DIAMOND_STONE_ID = 401; // 金刚钻
	if (!role->m_packmgr.hasEnoughItem(DIAMOND_STONE_ID,itemWeapon->PuchHoleStoneCount()))
	{
		WORD dwError = 2; // 金刚钻不足
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_MAKE_HOLE, strData) );
		return 0;
	}

	// 扣除金刚钻
	role->m_packmgr.consumeItem(DIAMOND_STONE_ID, itemWeapon->PuchHoleStoneCount());

	// 打一个孔
	itemWeapon->PuchOneHole();

	notifyItem( role, itemWeapon, weaponPos);

	if ( PKG_TYPE_BODY == weaponPos )
	{
		updateNotifyRoleAttr( role );
		int bodyIndex = getBodyIndexBySubtype( role, itemWeapon->itemCfg->sub_type );
		if (bodyIndex == BODY_INDEX_WEAPON)
		{
			RANK::insertWeaponListRank(role, itemWeapon);
		}	
	}
	else if (PKG_TYPE_SLAVE == weaponPos)
	{
		updateMerAttrByEquip(role, itemWeapon);
	}

	WORD dwError = 0; // 打孔成功
	string strData;
	S_APPEND_WORD(strData, dwError);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_MAKE_HOLE, strData) );
	return 0;
}

int  ItemCtrl::clientInjectDiamond( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	DWORD equipId = 0;
	DWORD gemId = 0;
	BYTE pos  = 0;
	if ( !BASE::parseDWORD( data, dataLen, equipId) )
		return 0;
	if ( !BASE::parseDWORD( data, dataLen, gemId) )
		return 0;
	if ( !BASE::parseBYTE( data, dataLen, pos) )
		return 0;

	ITEM_CACHE* pWeapon =(ITEM_CACHE* )equipId;
	PKG_TYPE weaponPos = role->m_packmgr.findItem( pWeapon);

	if ( PKG_TYPE_NONE == weaponPos || pWeapon==NULL || pWeapon->itemCfg==NULL || !IS_EQUIP( pWeapon->itemCfg))
	{
		WORD dwError = 4; // 装备不存在
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_INJECT_DIAMOND_AT, strData) );
		return 0;
	}

	ITEM_CACHE* pGem =(ITEM_CACHE* )gemId;
	PKG_TYPE gmPos = role->m_packmgr.findItem( pGem);

	if ( PKG_TYPE_NONE == gmPos || NULL==pGem || pGem->itemCfg==NULL || !IS_EQUIP_GEM(pGem->itemCfg))
	{
		WORD dwError = 3; // 宝石不存在
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_INJECT_DIAMOND_AT, strData) );
		return 0;
	}

	if (!pWeapon->isHoleOpen(pos))
	{
		WORD dwError = 1; // 孔位未开启
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_INJECT_DIAMOND_AT, strData) );
		return 0;
	}

	// 该位置已经镶嵌，怀疑外挂
	if (pWeapon->isHoleInjected(pos))
	{
		return 0;
	}

	if (pWeapon->hasInjectedSameTypeGem(pGem->itemCfg->id))
	{
		WORD dwError = 2; // 已经镶嵌同类型宝石
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_INJECT_DIAMOND_AT, strData) );
		return 0;
	}

	pWeapon->injectEquipGem(pos,pGem->itemCfg->id);

	// 扣除宝石
	//ItemCtrl::consumeItem(role, pGem->itemCfg->id, 1);
	subItemNotify(role,role->m_packmgr.lstItemPkg,PKG_TYPE_ITEM,pGem,1);

	pWeapon->resetItemAttrValue();

	notifyItem( role, pWeapon, weaponPos);

	if ( PKG_TYPE_BODY == weaponPos )
	{
		updateNotifyRoleAttr( role );

		int bodyIndex = getBodyIndexBySubtype( role, pWeapon->itemCfg->sub_type );
		if (bodyIndex == BODY_INDEX_WEAPON)
		{
			RANK::insertWeaponListRank(role, pWeapon);
		}	
	}
	else if ( PKG_TYPE_SLAVE == weaponPos )
	{
		updateMerAttrByEquip( role, pWeapon );
	}

	WORD dwError = 0; // 镶嵌宝石成功
	string strData;
	S_APPEND_WORD(strData, dwError);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_INJECT_DIAMOND_AT, strData) );

	RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_UNIJECT_DIAMOND );

	return 0;
}

int  ItemCtrl::clientUninjectDiamond( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	DWORD equipId = 0;
	BYTE pos  = 0;
	if ( !BASE::parseDWORD( data, dataLen, equipId) )
		return 0;
	if ( !BASE::parseBYTE( data, dataLen, pos) )
		return 0;

	ITEM_CACHE* pWeapon =(ITEM_CACHE* )equipId;
	PKG_TYPE weaponPos = role->m_packmgr.findItem(pWeapon);

	if (pWeapon==NULL || pWeapon->itemCfg==NULL || PKG_TYPE_NONE == weaponPos || !IS_EQUIP( pWeapon->itemCfg))
	{
		return 0; // 装备不存在
	}

	if (!pWeapon->isHoleOpen(pos))
	{
		return 0; // 孔位未开启
	}

	if (!pWeapon->isHoleInjected(pos))
	{
		WORD dwError = 2; // 孔位未镶嵌宝石
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UNINJECT_DIAMOND_AT, strData) );
		return 0;
	}

	WORD gemid = pWeapon->getInjectedGem(pos);
	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(gemid);
	if ( itemCfg == NULL )
		return true;

	std::list<ITEM_CACHE*> lstItemPkg;
	if ( E_SUCC != role->m_packmgr.makeItem(itemCfg, 1, &lstItemPkg) )
	{
		WORD dwError = 1; //包裹已满
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UNINJECT_DIAMOND_AT, strData) );
		return 0;
	}
	ItemCtrl::notifyClientItem(role, lstItemPkg, PKG_TYPE_ITEM);

	pWeapon->unInjectGem(pos);
	
	pWeapon->resetItemAttrValue();

	notifyItem( role, pWeapon, weaponPos);
	
	if ( PKG_TYPE_BODY == weaponPos )
	{
		updateNotifyRoleAttr( role );
		int bodyIndex = getBodyIndexBySubtype( role, pWeapon->itemCfg->sub_type );
		if (bodyIndex == BODY_INDEX_WEAPON)
		{
			RANK::insertWeaponListRank(role, pWeapon);
		}	
	}
	else if ( PKG_TYPE_SLAVE == weaponPos )
	{
		updateMerAttrByEquip( role, pWeapon );
	}

	WORD dwError = 0; 
	string strData;
	S_APPEND_WORD(strData, dwError);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UNINJECT_DIAMOND_AT, strData) );
	return 0;
}

int ItemCtrl::clientUninjectAllDiamonds( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	DWORD equipId = 0;
	if ( !BASE::parseDWORD( data, dataLen, equipId) )
		return 0;

	ITEM_CACHE* pWeapon =(ITEM_CACHE* )equipId;
	PKG_TYPE weaponPos = role->m_packmgr.findItem(pWeapon);

	if (pWeapon==NULL || pWeapon->itemCfg==NULL || PKG_TYPE_NONE == weaponPos || !IS_EQUIP( pWeapon->itemCfg))
	{
		return 0; // 装备不存在
	}

	std::vector<WORD> vecItems = pWeapon->getAllInjectGems();
	vector<ITEM_INFO> vecItemInfos;
	for (DWORD i=0; i<vecItems.size(); i++)
	{
		WORD itemid = vecItems[i];
		ITEM_INFO info;
		info.itemIndex = itemid;
		info.itemNums = 1;
		vecItemInfos.push_back(info);
	}

	if (!MailCtrl::makeMailItems(role, vecItemInfos))
	{
		WORD dwError = 1;  // 包裹已满
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UNINJECT_ALL_DIAMOND, strData) );
	}
	
	pWeapon->unInjectAllGems();

	pWeapon->resetItemAttrValue();

	notifyItem( role, pWeapon, weaponPos);

	if ( PKG_TYPE_BODY == weaponPos )
	{
		updateNotifyRoleAttr( role );
		int bodyIndex = getBodyIndexBySubtype( role, pWeapon->itemCfg->sub_type );
		if (bodyIndex == BODY_INDEX_WEAPON)
		{
			RANK::insertWeaponListRank(role, pWeapon);
		}	
	}
	else if ( PKG_TYPE_SLAVE == weaponPos )
	{
		updateMerAttrByEquip( role, pWeapon );
	}

	WORD dwError = 0; 
	string strData;
	S_APPEND_WORD(strData, dwError);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_UNINJECT_ALL_DIAMOND, strData) );
	return 0;
}

// 装备洗练
int ItemCtrl::clientEquipReset( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	DWORD equipId = 0;
	BYTE  reset_type = 0;

	if ( !BASE::parseDWORD( data, dataLen, equipId) )
		return 0;
	if ( !BASE::parseBYTE( data, dataLen, reset_type) )
		return 0;

	RETURN_COND( reset_type > BYTE(e_reset_type::gold_type), 0 );//三种洗练方式

	ITEM_CACHE* pWeapon =(ITEM_CACHE* )equipId;
	PKG_TYPE weaponPos = role->m_packmgr.findItem( pWeapon);

	if (pWeapon==NULL || pWeapon->itemCfg==NULL || PKG_TYPE_NONE == weaponPos || !IS_EQUIP( pWeapon->itemCfg))
	{
		return 0; // 装备不存在
	}

	if (pWeapon->getExtraNums() < 2)
	{
		WORD dwError = 2;  //装备品质过低
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EQUIP_RESET, strData) );
		return 0;
	}

	if (role->wLevel < 5)
	{
		WORD dwError = 1;  //功能未开放
		string strData;
		S_APPEND_WORD(strData, dwError);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EQUIP_RESET, strData) );
		return 0;
	}

	DWORD need_num = 0;

	if ( reset_type == BYTE(e_reset_type::coin_type) )
	{
		need_num = pWeapon->itemCfg->level_limit * 1500;

		if (!role->hasEnoughCoin(need_num))
		{
			WORD dwError = 3; //金币不足
			string strData;
			S_APPEND_WORD(strData, dwError);
			PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_EQUIP_RESET, strData ) );
			return 0;
		}

		role->subCoin(need_num);
	}
	else if ( reset_type == BYTE(e_reset_type::stone_type) )
	{
		need_num = 10;

		CONFIG::ITEM_CFG* needItemCfg = CONFIG::getItemCfg( ITEM_ID_RESET_STONE );

		if (  ItemCtrl::getPropItemNums( role, needItemCfg ) < need_num)
		{
			WORD dwError = 4; //元宝不足
			string strData;
			S_APPEND_WORD(strData, dwError);
			PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_EQUIP_RESET, strData ) );
			return 0;
		}

		ItemCtrl::rmItemNotify( role, role->m_packmgr.lstItemPkg, PKG_TYPE_ITEM, needItemCfg, need_num );
	}
	else
	{
		need_num = 100;

		if (  !role->hasEnoughGold( need_num ) )
		{
			WORD dwError = 5; //元宝不足
			string strData;
			S_APPEND_WORD(strData, dwError);
			PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket( S_EQUIP_RESET, strData ) );
			return 0;
		}

		role->subGold( need_num );
	}

	pWeapon->equipReset( reset_type );

	pWeapon->resetItemAttrValue();

	notifyItem( role, pWeapon, weaponPos);

	if ( PKG_TYPE_BODY == weaponPos )
	{
		updateNotifyRoleAttr( role );
		int bodyIndex = getBodyIndexBySubtype( role, pWeapon->itemCfg->sub_type );
		if (bodyIndex == BODY_INDEX_WEAPON)
		{
			RANK::insertWeaponListRank(role, pWeapon);
		}	
	}
	else if ( PKG_TYPE_SLAVE == weaponPos )
	{
		updateMerAttrByEquip( role, pWeapon );
	}

	WORD dwError = 0; 
	string strData;
	S_APPEND_WORD(strData, dwError);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EQUIP_RESET, strData) );

	RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_EQUIP_RESET_TIMES );

	return 0;
}

//// 熔炼预期
//int ItemCtrl::clientSmeltEquipRemain( ROLE* role, unsigned char * data, size_t dataLen )
//{
//	if (NULL == role)
//		return 0;
//
//	string strData;
//	S_APPEND_WORD(strData, getNeedSmeltEquip(role));
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SMELT_EQUIP_REMAIN, strData) );
//	return 0;
//}

// 获得产出的总熔炼值
static DWORD getOutputSmeltValues(const vector< ITEM_CACHE* >& vecEquips)
{
	DWORD result = 0;
	for (DWORD i=0; i<vecEquips.size(); i++)
	{
		ITEM_CACHE* item = vecEquips[i];
		if (NULL != item)
		{
			result += item->getOutputSmeltValue();
		}
	}
	return result;
}

// 获得产出强化石数量
static DWORD getOutputStrengthenStones(const vector< ITEM_CACHE* >& vecEquips)
{
	DWORD result = 0;
	for (DWORD i=0; i<vecEquips.size(); i++)
	{
		ITEM_CACHE* item = vecEquips[i];
		if (NULL != item)
		{
			result += item->getOutputStrengthenStoneCount();
		}
	}
	return result;
}

// 获得产出的套装碎片
static DWORD getOutputSuitPatch(const vector< ITEM_CACHE* >& vecEquips)
{
	DWORD result = 0;
	for (DWORD i=0; i<vecEquips.size(); i++)
	{
		ITEM_CACHE* item = vecEquips[i];
		if (NULL != item)
		{
			result += item->getOutputSuitPatchCount();
		}
	}
	return result;
}

// 获得产出的品质值
static DWORD getOutputQualityValue(const vector< ITEM_CACHE* >& vecEquips)
{
	DWORD result = 0;
	for (DWORD i=0; i<vecEquips.size(); i++)
	{
		ITEM_CACHE* item = vecEquips[i];
		if (NULL != item)
		{
			result += item->getOutputQualityValue();
		}
	}
	return result;
}


// 获得随机装备等级段
static WORD getRandEquipLevel(ROLE* role)
{
	/*
		//玩家等级 = 1-5级，100%从表__equip_index中获得等级字段_level值=7的；
		//玩家等级 = 6-10级，100%从表__equip_index中获得等级字段_level值=12的；
		//玩家等级 = 11-15级，20%取level=12；80%取level=17；
		//玩家等级 = 16-20级，30%取level=17；70%取level=22；
		//玩家等级 = 21-25级，40%取level=22；60%取level=27；
		//玩家等级 = 26-30级，50%取level=27；50%取level=32；
		//玩家等级 = 31-35级，50%取level=32；50%取level=37；
		//玩家等级 = 36-40级，50%取level=37；50%取level=42；
		//玩家等级 = 41-45级，50%取level=42；50%取level=47；
		//玩家等级 = 46-50级，50%取level=47；50%取level=52；
		//玩家等级 = 51-55级，50%取level=52；50%取level=57；
		//玩家等级 = 56-60级，50%取level=57；50%取level=62；
		//玩家等级 = 61-65级，50%取level=62；50%取level=67；
		//玩家等级 = 66-70级，60%取level=67；40%取level=72；
		玩家等级 = 71-75级，70%取level=72；30%取level=77；
		玩家等级 = 76++级，80%取level=77；20%取level=82；
		*/
	if (NULL == role)
	{
		return 0;
	}

	if (role->wLevel <= 5)
	{
		return 7;
	}
	else if (role->wLevel <= 10)
	{
		return 12;
	}
	else if (role->wLevel <= 15)
	{
		return BASE::randHit(20,100) ? 12 : 17;
	}
	else if (role->wLevel <= 20)
	{
		return BASE::randHit(30,100) ? 17 : 22;
	}
	else if (role->wLevel <= 25)
	{
		return BASE::randHit(40,100) ? 22 : 27;
	}
	else if (role->wLevel <= 30)
	{
		return BASE::randHit(50,100) ? 27 : 32;
	}
	else if (role->wLevel <= 35)
	{
		return BASE::randHit(50,100) ? 32 : 37;
	}
	else if (role->wLevel <= 40)
	{
		return BASE::randHit(50,100) ? 37 : 42;
	}
	else if (role->wLevel <= 45)
	{
		return BASE::randHit(50,100) ? 42 : 47;
	}
	else if (role->wLevel <= 50)
	{
		return BASE::randHit(50,100) ? 47 : 52;
	}
	else if (role->wLevel <= 55)
	{
		return BASE::randHit(50,100) ? 52 : 57;
	}
	else if (role->wLevel <= 60)
	{
		return BASE::randHit(50,100) ? 57 : 62;
	}
	else if (role->wLevel <= 65)
	{
		return BASE::randHit(50,100) ? 62 : 67;
	}
	else if (role->wLevel <= 70)
	{
		return BASE::randHit(60,100) ? 67 : 72;
	}
	else if (role->wLevel <= 75)
	{
		return BASE::randHit(70,100) ? 72 : 77;
	}
	else
	{
		return BASE::randHit(80,100) ? 77 : 82;
	}

	return 0;
}

static WORD randomOneEquip(WORD job, WORD level)
{
	CONFIG::FORGE_REFRESH_CFG* item =  CONFIG::getForgeCfg( job, level);
	if (NULL == item)
	{
		return 0;
	}

	int index = BASE::randInter(0,9);
	return item->vEquip_1_10[index];
}

static bool canSmeltOutputEquip( ITEM_CACHE* item )
{
	RETURN_COND( item == NULL, false );

	BYTE byQua = item->byQuality;

	if ( byQua == E_QUALITY_WHITE )
	{
		if ( BASE::randHit( 25, 100 ) )
		{
			return true;
		}
	}
	if ( byQua == E_QUALITY_BLUE )
	{
		if ( BASE::randHit( 20, 100 ) )
		{
			return true;
		}
	}
	else if( byQua == E_QUALITY_GOLD )
	{
		if ( BASE::randHit( 15, 100 ) )
		{
			return true;
		}
	}
	else if ( byQua == E_QUALITY_DARK_GOLD )
	{
		if ( BASE::randHit( 10, 100 ) )
		{
			return true;
		}
	}
	else
	{
		if ( BASE::randHit( 10, 100 ) )
		{
			return true;
		}
	}

	return false;
}

static BYTE getOutputEquipQuality( ITEM_CACHE* item )
{
	RETURN_COND( item == NULL, 1 );

	BYTE byQua = item->byQuality;

	if ( byQua <= E_QUALITY_BLUE )
	{
		return ( byQua + 1 );
	}
	else if( byQua == E_QUALITY_GOLD )
	{
		if( BASE::randHit( 40, 100 ) )
		{
			return byQua;
		}
		else
		{
			return ( byQua + 1 );
		}
	}
	else if ( byQua == E_QUALITY_DARK_GOLD )
	{
		if ( BASE::randHit( 75, 100 ) )
		{
			return byQua;
		}
		else
		{
			return ( byQua + 1 );
		}
	}
	else
	{
		return E_QUALITY_GREEN;
	}
}
static BYTE getOutputEquipQuality(DWORD qualityvalue)
{
	if (qualityvalue < 60)
	{
		if (BASE::randHit(80,100))
		{
			return E_QUALITY_BLUE;
		}
		else
		{
			return	E_QUALITY_GOLD;
		}
	}
	else if (qualityvalue < 100)
	{
		if (BASE::randHit(80,100))
		{
			return E_QUALITY_GOLD;
		}
		else
		{
			return E_QUALITY_DARK_GOLD;
		}
	}
	else if (qualityvalue < 250)
	{
		if (BASE::randHit(98,100))
		{
			return E_QUALITY_DARK_GOLD;
		}
		else
		{
			return E_QUALITY_GREEN;
		}
	}
	else
	{
		return E_QUALITY_GREEN;
	}

	return E_QUALITY_WHITE;
}

// 熔炼
int ItemCtrl::clientSmeltEquip( ROLE* role, unsigned char * data, size_t dataLen )
{
	RETURN_COND( NULL == role , 0 );
 
	vector< ITEM_CACHE* > vecEquips;

	int maxNums = 6;//最多六个

	while( maxNums >0 ) 
	{
		ITEM_CACHE* item;
		if ( !BASE::parseDWORD( data, dataLen, (*(DWORD*)&item)) )
			break;

		PKG_TYPE weaponPos = role->m_packmgr.findItem(item);

		if (item==NULL || item->itemCfg==NULL || PKG_TYPE_NONE == weaponPos || !IS_EQUIP( item->itemCfg))
		{
			return 0; // 装备不存在
		}

		if ( PKG_TYPE_BODY == weaponPos || PKG_TYPE_ITEM == weaponPos)
		{
			continue; // 非装备
		}

		vecEquips.push_back(item);

		maxNums--;
	}

	RETURN_COND( vecEquips.empty(), 0 );

	//带强化的装备，返还强化所消耗的70%的强化石
	{
		const WORD STONE_ID = 402; // 强化石

		DWORD stone_count = getOutputStrengthenStones(vecEquips);

		ItemCtrl::makeOneTypeItem( role, CONFIG::getItemCfg(STONE_ID), stone_count );
	}

	// 获得产出的套装碎片数量
	{
		const WORD SUIT_PATCH_ID = 403;
		DWORD suit_patch_count = getOutputSuitPatch(vecEquips);
		std::list<ITEM_CACHE*> lstItemPkg;
		ItemCtrl::makeOneTypeItem( role, CONFIG::getItemCfg(SUIT_PATCH_ID), suit_patch_count );
	}

	//获得镶嵌的宝石
	{
		vector<ITEM_INFO> vecItemInfos;

		for (DWORD i=0; i<vecEquips.size(); i++)
		{
			ITEM_CACHE* item = vecEquips[i];

			std::vector<WORD> vecItems = item->getAllInjectGems();

			for (DWORD i=0; i<vecItems.size(); i++)
			{
				WORD itemid = vecItems[i];
				ITEM_INFO info;
				info.itemIndex = itemid;
				info.itemNums = 1;
				vecItemInfos.push_back(info);
			}
		}

		MailCtrl::makeMailItems(role, vecItemInfos);
	}


	//获得生成的熔炼值
	{
		DWORD smelt_values = getOutputSmeltValues(vecEquips);
		role->addSmeltValue(smelt_values);
	}

	// 删除装备   // 生成新装备
	{
		vector<ITEM_INFO> vecItemInfos;

		for ( size_t i=0; i<vecEquips.size(); i++ )
		{
			ITEM_CACHE* item = vecEquips[i];

			COND_CONTINUE( !canSmeltOutputEquip(item) );//每一件装备，均有概率，生成1件新装备

			WORD equipid = item->itemCfg->id;//randomOneEquip(role->byJob, getRandEquipLevel(role));

			BYTE quality = getOutputEquipQuality( item );

			vecItemInfos.push_back( ITEM_INFO( equipid, 1, quality ) );
		}


		for (DWORD i=0; i<vecEquips.size(); i++)
		{
			ITEM_CACHE* item = vecEquips[i];
			if (NULL != item)
			{
				subItemNotify(role,role->m_packmgr.lstEquipPkg,PKG_TYPE_EQUIP,item,1);
			}
		}


		MailCtrl::makeMailItems(role, vecItemInfos);

		BYTE error = 0;
		string strData;
		S_APPEND_BYTE(strData, error);
		for (auto& one : vecItemInfos)
		{
			S_APPEND_WORD( strData, (WORD)one.itemIndex );
			S_APPEND_WORD( strData, (WORD)one.byQuality );
		}
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SMELT_EQUIP_NEW, strData) );

		RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_SMELT );
	}
	return 0;
}

// 获得随机装备等级段
static WORD getForgeEquipLevel(ROLE* role)
{
	// TODO
	if (NULL == role)
	{
		return 0;
	}

	WORD arrLevels[17] = {1,7,12,17,22,27,32,37,42,47,52,57,62,67,72,77,82};
	for (DWORD i=1; i<17; i++)
	{
		if (role->wLevel <= arrLevels[i])
		{
			return arrLevels[i];
		}
	}


	return 82;
}

// 获得兑换装备等级段
static WORD getExchangeEquipLevel(ROLE* role)
{
	// TODO
	if (NULL == role)
		return 0;

	auto it = ItemCtrl::_map_exchange_level.lower_bound(role->wLevel);
	if ( it == ItemCtrl::_map_exchange_level.end() )
		return 0;

	return it->second;
}

static BYTE getForgeEquipQuality()
{
	if (BASE::randHit(1,100))
	{
		return E_QUALITY_GREEN;
	}

	if (BASE::randHit(10,99))
	{
		return E_QUALITY_DARK_GOLD;
	}
	
	return E_QUALITY_GOLD;
}

static WORD randomOneEquip(WORD job, WORD level, DWORD& smelt_value)
{
	CONFIG::FORGE_REFRESH_CFG* item =  CONFIG::getForgeCfg( job, level);
	if (NULL == item)
	{
		return 0;
	}

	int index = BASE::randInter(0,9);

	smelt_value = item->dwSmeltValue;

	return item->vEquip_1_10[index];
}

// 打造装备
int ItemCtrl::clientForgeEquip( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	DWORD smelt_value = 0;

	WORD equipid = randomOneEquip(role->byJob, getForgeEquipLevel(role), smelt_value);

	BYTE quality = getForgeEquipQuality();

	if (!role->hasEnoughSmeltValue(smelt_value))
	{
		WORD error = 2; //荣炼值不足
		string strData;
		S_APPEND_WORD(strData, error);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FORGE_ITEM, strData) );
		return 0;
	}

	// 扣除熔炼值
	role->subSmeltValue(smelt_value);

	std::list<ITEM_CACHE*> lstItemsPkg;
	if ( E_SUCC != role->m_packmgr.makeItem( CONFIG::getItemCfg(equipid), 1, &lstItemsPkg, false, quality) )
	{
		WORD error = 1; //包裹已满
		string strData;
		S_APPEND_WORD(strData, error);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FORGE_ITEM, strData) );
		return 0;
	}
	ItemCtrl::notifyClientItem(role, lstItemsPkg, PKG_TYPE_EQUIP);

	WORD error = 0;
	DWORD index = (DWORD)(*lstItemsPkg.begin());
	string strData;
	S_APPEND_WORD(strData, error);
	S_APPEND_DWORD(strData, index);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FORGE_ITEM, strData) );

	RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_FORGE_EQUIP_TIMES );

	return 0;
}

static WORD getExchangeSuitLeftTimes(ROLE* role)
{
	if (NULL == role)
	{
		return 0;
	}

	if (role->byForgeRreshTimes <= role->getExchangeSuitLimit())
	{
		return role->getExchangeSuitLimit() - role->byForgeRreshTimes;
	}
	
	return 0;
}

int ItemCtrl::clientForgeSuitNum( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
	{
		return 0;
	}

	WORD left = getExchangeSuitLeftTimes(role);
	string strData;
	S_APPEND_WORD(strData, left);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_FORGE_SUIT_NUM, strData) );
	return 0;
}

// 获得随机套装等级段
static WORD getForgeSuitLevel(ROLE* role)
{
	// TODO
	if (NULL == role)
	{
		return 0;
	}

	WORD arrLevels[17] = {1,7,12,17,22,27,32,37,42,47,52,57,62,67,72,77,82};
	for (DWORD i=1; i<17; i++)
	{
		if (role->wLevel <= arrLevels[i])
		{
			return arrLevels[i];
		}
	}


	return 82;
}

static BYTE getForgeSuitQuality()
{
	DWORD dwRand = BASE::randTo( 100 );

	if ( dwRand <= 1 )
	{
		return E_QUALITY_GREEN;
	}
	else if ( dwRand <= 11 )
	{
		return E_QUALITY_DARK_GOLD;
	}
	else
	{
		return E_QUALITY_GOLD;
	}

	//if (BASE::randHit(1,100))
	//{
	//	return E_QUALITY_GREEN;
	//}

	//if (BASE::randHit(10,99))
	//{
	//	return E_QUALITY_DARK_GOLD;
	//}

	//return E_QUALITY_GOLD;
}

static WORD randomSuitEquip(WORD job, WORD level, DWORD& smelt_value, DWORD& patch)
{
	CONFIG::FORGE_REFRESH_CFG* item =  CONFIG::getForgeCfg( job, level);
	if (NULL == item)
	{
		return 0;
	}

	int index = BASE::randInter(0,9);

	smelt_value = item->dwSuitSmeltValue;
	patch = item->dwSuitPatchCount;

	return item->vEquip_1_10[index];
}

// 兑换装备
int ItemCtrl::clientExchangeEquip( ROLE* role, unsigned char * data, size_t dataLen )
{
	if (NULL == role)
		return 0;

	WORD equipid = 0;
	if ( !BASE::parseWORD( data, dataLen, equipid) )
		return 0;

	DWORD smelt_value = 0;
	DWORD patch_count = 0;
	randomSuitEquip(role->byJob, getExchangeEquipLevel(role), smelt_value, patch_count);

	//BYTE quality = getForgeSuitQuality()
	BYTE quality = E_QUALITY_GREEN;

	if (!role->hasEnoughSmeltValue(smelt_value))
	{
		WORD error = 3; //荣炼值不足
		string strData;
		S_APPEND_WORD(strData, error);
		S_APPEND_WORD(strData, getExchangeSuitLeftTimes(role));
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EXCHANGE_EQUIP_NEW, strData) );
		return 0;
	}

	if (getExchangeSuitLeftTimes(role) == 0)
	{
		WORD error = 4; //打造次数不足
		string strData;
		S_APPEND_WORD(strData, error);
		S_APPEND_WORD(strData, getExchangeSuitLeftTimes(role));
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EXCHANGE_EQUIP_NEW, strData) );
		return 0;
	}

	const WORD SUIT_PATCH_ID = 403;
	if (!role->m_packmgr.hasEnoughItem(SUIT_PATCH_ID, patch_count))
	{
		WORD dwError = 2; //套装碎片不足
		string strData;
		S_APPEND_WORD(strData, dwError);
		S_APPEND_WORD(strData, getExchangeSuitLeftTimes(role));
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EXCHANGE_EQUIP_NEW, strData) );
		return 0;
	}

	// 扣除熔炼值
	role->subSmeltValue(smelt_value);
	// 扣除套装碎片
	role->m_packmgr.consumeItem(SUIT_PATCH_ID,patch_count);
	// 兑换套装次数增加
	role->byForgeRreshTimes++;

	std::list<ITEM_CACHE*> lstItemsPkg;
	if ( E_SUCC != role->m_packmgr.makeItem( CONFIG::getItemCfg(equipid), 1, &lstItemsPkg, false, quality) )
	{
		WORD error = 1; //包裹已满
		string strData;
		S_APPEND_WORD(strData, error);
		S_APPEND_WORD(strData, getExchangeSuitLeftTimes(role));
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EXCHANGE_EQUIP_NEW, strData) );
		return 0;
	}
	ItemCtrl::notifyClientItem(role, lstItemsPkg, PKG_TYPE_EQUIP);

	WORD dwError = 0; //成功
	string strData;
	S_APPEND_WORD(strData, dwError);
	S_APPEND_WORD(strData, getExchangeSuitLeftTimes(role));
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EXCHANGE_EQUIP_NEW, strData) );
	return 0;
}

int ItemCtrl::clientLookOthersItem( ROLE* role, unsigned char * data, size_t dataLen )
{
//	C:[DWORD, DWORD]
//		[playerId, itemId]
//	S:[ItemBriefInfo, [BYTE, WORD]]
//		[briefInfo, [type, value]]
//		ItemBriefInfo:道具基本信息
//		type:属性类型
//		value:属性值

	DWORD dwOtherRoleID;
	ITEM_CACHE* item;
	if ( !BASE::parseDWORD( data, dataLen, dwOtherRoleID) )
		return 0;
	if ( !BASE::parseDWORD( data, dataLen, (*(DWORD*)&item)) )
		return 0;

	ROLE* role2 = RoleMgr::getMe().getRoleByID( dwOtherRoleID );
	if ( role2==NULL)
		return 0;

	int bodyIndex = -1;
	if ( PKG_TYPE_NONE == role2->m_packmgr.findItem( item, &bodyIndex) )
		return 0;

//装备戒指分左右

	string strData;
	ItemBriefInfo brefInfo(item);

	if ( bodyIndex != -1 )
		brefInfo.byEquipLeftRight = GET_LEFT_RIGHT(bodyIndex);

	S_APPEND_STRUCT( strData, brefInfo);

	if ( IS_EQUIP( item->itemCfg ) )
		mkItemPro( strData, item );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_LOOK_OTHER_ITEM, strData) );

	return 0;
}


int ItemCtrl::clientLookOther( ROLE* role, unsigned char * data, size_t dataLen )
{
//	C:[DWORD]
//		[playerId]
//		playerId:玩家ID
//	S:[  DWORD,   BSTR, BYTE, BYTE, BYTE, [DWORD, WORD, BYTE]]
//		[playerId, name, job, sex,  lv, [equipId, equipIdx, equipPos]]
//		playerId:玩家ID
//		name:玩家名称
//		job:玩家职业
//		sex:玩家性别
//		lv:玩家等级
//		装备列表:[equipId, equipIdx, equipPos]
//			equipId:装备唯一ID
//			equipIdx:装备index
//			equipPos:装备位置



//C:[DWORD]
//	[playerId]
//playerId:玩家ID
//S:[   DWORD,   DWORD,  DWORD, BSTR, BYTE, BYTE, BYTE, BYTE,   [DWORD, WORD, BYTE]]
//	[playerId, assist, power, name, job,  sex,  lv, isFriend, [equipId, equipIdx, equipPos]]
//playerId:玩家ID
//assist:点赞数//
//power:战斗力//
//name:玩家名称
//job:玩家职业
//sex:玩家性别
//lv:玩家等级
//isFriend:是否为好友. 1:是, 其他:否  //
//装备列表:[equipId, equipIdx, equipPos]
//equipId:装备唯一ID
//equipIdx:装备index
//equipPos:装备位置


	//map<WORD,PRACTISE_SKILL>::const_iterator it = role->mapSkills.end();//存放技能< 组 + 技能信息表>


	DWORD dwOtherRoleID;
	if ( !BASE::parseDWORD( data, dataLen, dwOtherRoleID) )
		return 0;

	ROLE* role2 = RoleMgr::getMe().getRoleByID( dwOtherRoleID );
	if ( role2==NULL)
		return 0;

	string strData;
	S_APPEND_DWORD( strData, role2->dwRoleID );
	S_APPEND_DWORD( strData, role2->dwFightValue );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_HP_MAX2] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_MP_MAX2] );

	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_STAMINA	] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_STRENGTH	] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_AGILITY	] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_INTELLECT] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_SPIRIT	] );

	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_ATK_MAX2] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_ATK_MIN2] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_DEF_MIN	] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_DEF_MAX] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_CRIT	] );

	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_CRIT_RES] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_CRIT_INC] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_CRIT_RED] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_HIT] );
	S_APPEND_DWORD( strData, role2->roleAttr[E_ATTR_DODGE] );

	S_APPEND_BSTR( strData, role2->roleName );
	S_APPEND_BYTE( strData, role2->byJob );
	S_APPEND_BYTE( strData, role2->bySex );
	S_APPEND_BYTE( strData, role2->wLevel );

	if (find(role->vecFriends.begin(), role->vecFriends.end(), dwOtherRoleID) != role->vecFriends.end())
	{
		S_APPEND_BYTE( strData, 1 );
	}
	else
	{
		S_APPEND_BYTE( strData, 0 );
	}
	
	BYTE byEquipCOunt = 0;
	for ( int i=0; i<BODY_GRID_NUMS; i++)
	{
		COND_CONTINUE( role2->vBodyEquip[i] == NULL );
		byEquipCOunt++;
	}

	S_APPEND_BYTE( strData, byEquipCOunt );//客户端从1到10

	for ( int i=0; i<BODY_GRID_NUMS; i++)
	{
		ITEM_CACHE* itemCache = role2->vBodyEquip[i];

		COND_CONTINUE( itemCache == NULL )

		S_APPEND_DWORD( strData, itemCache );

		S_APPEND_BYTE( strData, i+1 );//客户端从1到10

		ItemBriefInfo brefInfo(itemCache);

		S_APPEND_STRUCT( strData, brefInfo);

	}
	
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_LOOK_OTHER, strData) );

	return 0;
}


int ItemCtrl::clientEnterMap( ROLE* role, unsigned char * data, size_t dataLen )
{

	//if ( dataLen >= 255 )
	//	return -1;

	//Document document;
	//if (document.Parse<0>( string( (char*)data, dataLen).c_str() ).HasParseError())
	//	return -1;

	//int nMapIndex = JSON::getJsonValueInt( document, "map" );
	//if (nMapIndex <= 0)
	//{
	//	logwm("map = %d",nMapIndex);
	//	return 0;
	//}


	//
	//int nTag = JSON::getJsonValueInt( document, "tag" );
	//if (nTag <= 0)
	//{
	//	logwm("0nTag = %d",nTag);
	//	//return 0;
	//}

	//WORD wMapIndex = (WORD )nMapIndex;

	//char szMsg[256]={0};
	//char* p = szMsg;

	//string strData;
	//
	//BYTE byRet = (BYTE )setMapMonster( role, wMapIndex );	
	//if (  byRet != 0 || role->lstMonsterIndex.empty() )
	//{
	//	sprintf(szMsg, "{\"tag\":%d,\"error\":%u }",nTag, byRet );
	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ENTER_MAP, szMsg) );
	//	//logplan("22xxx:%s",szMsg);
	//	return 0;
	//}


	//////0不用,设置初始战力
	////if ( wMapIndex <= 99 && role->vMapInitFightValue[ wMapIndex ] == 0 )
	////{
	////	role->vMapInitFightValue[ wMapIndex ] = role->dwFightValue;
	////}
	//memset(role->predictEarning, 0, sizeof( role->predictEarning ));
	//role->byNewMapKillMonNum = 0;

	//p += sprintf( p, "{\"tag\":%d,\"error\":%u,\"mon\":[ ", nTag, byRet );
	//for ( auto it : role->lstMonsterIndex )
	//{
	//	p += sprintf( p, "%u,", it );
	//}
	//*(--p) = 0;
	//p += sprintf(p, "]}");
	////logplan("map  %u",nMapIndex);
	////logplan("xxx:%s",szMsg);
	//logFATAL("rolename=%s %s", role->roleName.c_str(), szMsg);
	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ENTER_MAP, szMsg) );
	return 0;
}


//#define S_FIGHT_REQUEST	0x0052//BOSS挑战请求
int ItemCtrl::clientFightRequest( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byRet = FIGHT_BOSS_SUCCESS;//成功
	string strData;
	
	if ( role->bySendFightBossTimes + role->byAllBuyBossTimes == 0 )
	{
		byRet = FIGHT_BOSS_NOT_TIMES;
	}
	
	S_APPEND_BYTE(strData, byRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REQUEST, strData) );
	return 0;
}

//#define S_BUY_FIGHT 	0x0053//购买BOSS挑战次数
int ItemCtrl::clientBuyFight( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byRet = FIGHT_BOSS_SUCCESS;//成功
	string strData;
	WORD index =  role->byDailyBuyBossTimes;
	DWORD dwCostGold = index <= (COST_TIMES - 1) ? _Cost[index] : _Cost[COST_TIMES - 1];
	if (role->dwIngot < dwCostGold)
	{
		byRet = FIGHT_BOSS_NO_INGOT;//钱不够
	}
	else
	{
		CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
		if (vipCfg != NULL && vipCfg->boss_challenge > index)
		{
			ADDUP(role->dwIngot, -(int)dwCostGold);
			ADDUP(role->byAllBuyBossTimes, 1);
			ADDUP(role->byDailyBuyBossTimes, 1);
			notifyIngot(role);
		}
		else
		{
			byRet  = FIGHT_BOSS_NOT_VIP ;//vip等级不足
		}	
	}

	S_APPEND_BYTE(strData, byRet);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BUY_FIGHT, strData) );

	if ( byRet == FIGHT_BOSS_SUCCESS )
	{
		RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_BUY_FIGHT_BOSS_TIMES );
	}
	return 0;
}

//#define S_BUY_FIGHT_COST 	0x0054//购买BOSS挑战需要金币
int ItemCtrl::clientBuyFightCost( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;
	WORD index =  role->byDailyBuyBossTimes;
	DWORD dwCostGold = index <= (COST_TIMES - 1) ? _Cost[index] : _Cost[COST_TIMES - 1];
	BYTE byLeftBuyTimes;
	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
	if (vipCfg != NULL && vipCfg->boss_challenge > role->byDailyBuyBossTimes)
	{
		byLeftBuyTimes = vipCfg->boss_challenge - role->byDailyBuyBossTimes;
	}
	else
	{
		byLeftBuyTimes = 0;
	}	
	S_APPEND_BYTE(strData, byLeftBuyTimes);
	S_APPEND_DWORD(strData, dwCostGold);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BUY_FIGHT_COST, strData) );
	return 0;
}

//#define S_GET_FIGHT_TIMES	0x0055//请求次数
int ItemCtrl::clientGetFightTimes( ROLE* role, unsigned char * data, size_t dataLen )
{
	string strData;
	BYTE times =  0;

	times = role->bySendFightBossTimes + role->byAllBuyBossTimes;

	S_APPEND_BYTE(strData, times);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_FIGHT_TIMES, strData) );
	return 0;
}

//一键结束#define S_QUICK_FIGHT_BOSS	0x0056//一键结束战斗报告
int ItemCtrl::clientQuickFightReport( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//	C:{mon=id1, tag=tagid, mon=monid, map=mapid}
//	//		mon:怪物ID
//	//	S:{error=0, drop=[{itemId, num}]}
//	//		error:错误码
//	//			0:成功
//	//			1:背包已满
//	//			2:矿石背包已满
//	//			3:背包和矿石背包都满
//	//			4:怪物不存在
//	//			5:冷却时间
//	//			6:等级不足(与地图等级相比，打别的地图BOSS)
//	//7:此地图怪物未战胜过
//	//8:战斗失败
//	//9:挑战次数不足
//
//	//		drop:物品掉落列表
//	//			itemId:道具ID
//	//			num:物品数量
//logwm("quickfightreport rolename=%s",role->roleName.c_str());
//	if (role->byFight == 0)
//	{
//		logwm("1mon role=%u byFight=%u",role->dwRoleID,role->byFight);
//		return 0;
//	}
//	if ( dataLen >= 255 )
//	{
//		logwm("dataLen >= 255 role=%u",role->dwRoleID);
//		return 0;
//	}
//
//	if ( role->byUseFightBossTimes >= role->byBuyFightBossTimes + role->bySendFightBossTimes)
//	{
//		logwm("role->byUseFightBossTimes=%d,role->byBuyFightBossTimes=%d,role->bySendFightBossTimes=%d",role->byUseFightBossTimes,role->byBuyFightBossTimes,role->bySendFightBossTimes);
//		return 0;
//	}
//	
//	Document document;
//	if (document.Parse<0>( string( (char*)data, dataLen).c_str() ).HasParseError())
//	{
//		logwm("3mon role=%u byFight=%u",role->dwRoleID,role->byFight);
//		return 0;
//	}
//
//	int nMonIndex = JSON::getJsonValueInt(document, "mon");
//	if ( nMonIndex <= 0 )
//	{
//		logwm("4mon role=%u byFight=%u",role->dwRoleID,role->byFight);
//		return 0;
//	}
//
//	int nTag = JSON::getJsonValueInt( document, "tag" );
//
//	WORD wMonIndex = (WORD )nMonIndex;
//	char xxx[256]={0};
//	string strData;
//
//	int nMapIndex = JSON::getJsonValueInt(document, "map");
//	if ( nMapIndex <= 0  || (WORD)nMapIndex > role->wWinMaxMapID )
//	{
//		sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 7 );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//		return 0;
//	}
//
//	//int nResult = 1;//胜利
//	//if ( nResult != 1 )//不赢
//	//{
//	//	logwm("5mon role=%u nResult=%d",role->dwRoleID,nResult);
//	//	sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 8 );
//	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//	//	return 0;
//	//}
//	
//
//	CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(wMonIndex);
//	if ( monCfg == NULL )
//	{
//		logwm( "monCfg == NULL=%d",wMonIndex );
//		sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 4 );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//		return 0;
//	}
//
//	if (  wMonIndex == role->wBossID )//本地图boss
//	{
//		role->wBossID = 0;
//		role->byUseFightBossTimes++;
//
//		ITEM_INFO para(role->wLastFightMapID, wMonIndex);
//		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_FIGHT_BOSS, &para);
//	}
//	else//其他地图boss
//	{
//		WORD wMapIndex = (WORD )nMapIndex;
//		BYTE byRet = 0;
//		CONFIG::MAP_CFG* mapCfg = getFightMap( role, wMapIndex, byRet );
//
//		if ( mapCfg==NULL || mapCfg->boss_id !=wMonIndex )
//		{
//			sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 4 );
//			PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//			return 0;
//		}
//		
//		role->byUseFightBossTimes++;
//
//		ITEM_INFO para(role->wLastFightMapID, wMonIndex);
//		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_FIGHT_BOSS, &para);
//	}
//
//	//list<ITEM_CACHE*> lstItemNotify, lstAutoSellItem, lstNoSellItem;
//	//killMonster( role, wMonIndex, 1, &lstItemNotify, &lstAutoSellItem, &lstNoSellItem );
//	list<ITEM_CACHE*> lstEquipNotify, lstPropsNotify, lstAutoSellItem, lstNoSellItem;
//	killMonster( role, wMonIndex, 1, &lstEquipNotify, &lstPropsNotify, &lstAutoSellItem, &lstNoSellItem );
//
//	ITEM_INFO para(role->wLastFightMapID, 1);
//	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_KILL_MON, &para);
//
//	ItemCtrl::notifyClientItem(role, lstEquipNotify, PKG_TYPE_EQUIP);
//	ItemCtrl::notifyClientItem(role, lstPropsNotify, PKG_TYPE_ITEM);
//
//	BYTE byRet = !!(role->m_packmgr.lstEquipPkg.size() >= role->m_packmgr.curEquipPkgLimit);
//
//	char* p = xxx;
//	p += sprintf( p, "{\"tag\":%d,\"error\":%u,\"drop\":[ ", nTag, byRet );
//	//1自动卖出 2未卖
//	for ( auto it : lstAutoSellItem )
//	{
//		p += sprintf( p, "[%u,%u,%u,%u,%u],", it->itemCfg->id,  it->itemNums, it->byQuality,1, (it->itemNums * it->itemCfg->price) );
//		delete it;
//	}
//	for ( auto it : lstNoSellItem )
//	{
//		p += sprintf( p, "[%u,%u,%u,%u,%u],", it->itemCfg->id,  it->itemNums, it->byQuality,2, (it->itemNums * it->itemCfg->price) );
//		delete it;
//	}
//	*(--p) = 0;
//
//	p += sprintf(p, "]}");
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_QUICK_FIGHT_BOSS, xxx) );
//	logwm("roleid=%u %s",role->dwRoleID, xxx);

	return 0;
}

int ItemCtrl::clientFightReport( ROLE* role, unsigned char * data, size_t dataLen )
{
//	if (role->byFight == 0)
//	{
//		logwm("1mon role=%u byFight=%u",role->dwRoleID,role->byFight);
//		return 0;
//	}
//return 0;
//
////	C:{mon=id1}
////		mon:怪物ID
////	S:{error=0, drop=[{itemId, num}]}
////		error:错误码
////			0:成功
////			1:背包已满
////			2:矿石背包已满
////			3:背包和矿石背包都满
////			4:怪物不存在
////			5:冷却时间
////			6:等级不足(与地图等级相比，打别的地图BOSS)
//
////		drop:物品掉落列表
////			itemId:道具ID
////			num:物品数量
//
//	if ( dataLen >= 255 )
//	{
//		logwm("2mon role=%u",role->dwRoleID);
//		return 0;
//	}
//
//	Document document;
//	if (document.Parse<0>( string( (char*)data, dataLen).c_str() ).HasParseError())
//	{
//		logwm("3mon role=%u byFight=%u",role->dwRoleID,role->byFight);
//		return 0;
//	}
//
//	int nMonIndex = JSON::getJsonValueInt(document, "mon");
//	if ( nMonIndex <= 0 )
//	{
//		logwm("4mon role=%u byFight=%u",role->dwRoleID,role->byFight);
//		return 0;
//	}
//
//	int nTag = JSON::getJsonValueInt( document, "tag" );
//	if (nTag <= 0)
//	{
//		logwm("1nTag = %d",nTag);
//		//return 0;
//	}
//
//	WORD wMonIndex = (WORD )nMonIndex;
//	char xxx[256]={0};
//	string strData;
//
//	int nResult = JSON::getJsonValueInt(document, "result");
//	if ( nResult != 1 )//不赢
//	{
//		logwm("5mon role=%u nResult=%d",role->dwRoleID,nResult);
//		sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 4 );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//		return 0;
//	}
//
//	CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(wMonIndex);
//	if ( monCfg == NULL || role->dwFightValue * 4 < monCfg->dwFightValue )
//	{
//		logwm( "monCfg == NULL=%d",wMonIndex );
//		if (monCfg != NULL )
//		{
//			logwm( "roleid=%u role=%d  mon=%d",role->dwRoleID, role->dwFightValue, monCfg->dwFightValue );
//		}
//		
//		sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 4 );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//		return 0;
//	}
//
//	auto it = find( role->lstMonsterIndex.begin(), role->lstMonsterIndex.end(), wMonIndex );
//	if ( it != role->lstMonsterIndex.end() )//推图
//	{
//		role->lstMonsterIndex.erase( it );
//	}
//	else//守关boss
//	{
//		if ( role->byUseFightBossTimes >= role->byBuyFightBossTimes + role->bySendFightBossTimes)
//		{
//			return 0;
//		}
//
//		if (  wMonIndex == role->wBossID )
//		{
//			//if ( role->wCanFightMaxMapID < role->wLastFightMapID )
//			if ( role->wCanFightMaxMapID <= role->wLastFightMapID )
//			{
//				//role->wCanFightMaxMapID =  role->wLastFightMapID;
//				role->wCanFightMaxMapID ++;
//				notifyMaxMap(role);
//			}
//
//			if ( role->wWinMaxMapID <= role->wLastFightMapID )
//			{
//				role->wWinMaxMapID = role->wLastFightMapID;
//			}
//
//			role->wBossID = 0;
//
//			//role->byUseFightBossTimes++;
//
//			//ITEM_INFO para(role->wLastFightMapID, wMonIndex);
//			//RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_FIGHT_BOSS, &para);
//
//		}
//		else
//		{
//			//打别的地图的BOSS
//			int nMapIndex = JSON::getJsonValueInt(document, "map");
//			if ( nMapIndex <= 0 )
//			{
//				sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 4 );
//				PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//				return 0;
//			}
//
//			WORD wMapIndex = (WORD )nMapIndex;
//
//			if ( wMapIndex > role->wCanFightMaxMapID )
//			{
//				sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 4 );
//				PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//				return 0;
//			}
//
//			BYTE byRet = 0;
//			CONFIG::MAP_CFG* mapCfg = getFightMap( role, wMapIndex, byRet );
//
//			if ( mapCfg==NULL || mapCfg->boss_id !=wMonIndex )
//			{
//				sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 4 );
//				PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//				return 0;
//			}
//
//			//DWORD tNow = NNOW;
//			//if ( tNow < role->dwCanFightOtherMapBossTime && role->dwCanFightOtherMapBossTime != 0 )
//			//{
//			//	sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 5 );
//			//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//			//	return 0;
//			//}
//
//			//等级不足仍然开新地图
//			if ( role->wCanFightMaxMapID <= wMapIndex )
//			{
//				role->wCanFightMaxMapID ++;
//				notifyMaxMap(role);
//
//				BroadcastCtrl::mapOpen( role, role->wCanFightMaxMapID );
//			}
//		
//			if ( role->wWinMaxMapID <= role->wLastFightMapID )
//			{
//				role->wWinMaxMapID = role->wLastFightMapID;
//			}
//			//if ( mapCfg->level_limit > role->wLevel )
//			//{
//			//	sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, 6 );
//			//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//			//	return 0;
//			//}
//
//			//挑战 别的地图BOSS,没有进入地图的步骤，从这里记CD
//			//role->dwCanFightOtherMapBossTime = tNow+mapCfg->cool_down;
//		}
//		role->byUseFightBossTimes++;
//
//		ITEM_INFO para(role->wLastFightMapID, wMonIndex);
//		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_FIGHT_BOSS, &para);
//	}
//
//	
//
//	//list<ITEM_CACHE*> lstItemNotify, lstItemDrop, lstSetSellItem, lstFullSellItem;
//	list<ITEM_CACHE*> lstEquipNotify, lstPropsNotify, lstAutoSellItem, lstNoSellItem;
//	killMonster( role, wMonIndex, 1, &lstEquipNotify, &lstPropsNotify, &lstAutoSellItem, &lstNoSellItem );
//	
//	ITEM_INFO para(role->wLastFightMapID, 1);
//	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_KILL_MON, &para);
//
//	ItemCtrl::notifyClientItem(role, lstEquipNotify, PKG_TYPE_EQUIP);
//	ItemCtrl::notifyClientItem(role, lstPropsNotify, PKG_TYPE_ITEM);
//
//	BYTE byRet = !!(role->m_packmgr.lstEquipPkg.size() >= role->m_packmgr.curEquipPkgLimit);
//
//	char* p = xxx;
//	p += sprintf( p, "{\"tag\":%d,\"error\":%u,\"drop\":[ ", nTag, byRet );
//
//	//wm 0设置自动卖出 1包满自动卖出 2未卖
//	//for ( auto it : lstItemDrop )
//	//{
//	//	p += sprintf( p, "[%u,%u,%u,%u,%u],", it->itemCfg->id,  it->itemNums, it->byQuality,2,100);
//	//	delete it;
//	//}
//
//	//1自动卖出 2未卖
//	for ( auto it : lstAutoSellItem )
//	{
//		p += sprintf( p, "[%u,%u,%u,%u,%u],", it->itemCfg->id,  it->itemNums, it->byQuality,1, (it->itemNums * it->itemCfg->price) );
//		delete it;
//	}
//	for ( auto it : lstNoSellItem )
//	{
//		p += sprintf( p, "[%u,%u,%u,%u,%u],", it->itemCfg->id,  it->itemNums, it->byQuality,2, (it->itemNums * it->itemCfg->price) );
//		delete it;
//	}
//	*(--p) = 0;
//
//	p += sprintf(p, "]}");
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FIGHT_REPORT, xxx) );
//
//	logwm("roleid=%u rolename=%s  %s",role->dwRoleID,role->roleName.c_str(), xxx);
////if ( bPrintXXX == 1 )
////{
////	logFATAL("%s", xxx);
////}
//
	return 0;
}

/*
@role获取itemNum件itemCfg物品，并通知客户端
*/
int ItemCtrl::makeOneTypeItem(ROLE* role, CONFIG::ITEM_CFG* itemCfg, size_t itemNum, BYTE byQuality, bool checkAutoSell )
{
	RETURN_COND( role==NULL || itemCfg==NULL, -1 );

	list<ITEM_CACHE*> lstItemCachePkg, lstPropsCachePkg;

	if (makeItemByType(role, itemCfg, &lstItemCachePkg, &lstPropsCachePkg, ITEM_INFO(itemCfg->id, itemNum, byQuality), checkAutoSell) == -1)//满
	{
		return -1;
	}

	ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_EQUIP);
	ItemCtrl::notifyClientItem(role, lstPropsCachePkg, PKG_TYPE_ITEM);
	return 0;
}

/*
@role获取vecItemInfog物品，并通知客户端
*/
int ItemCtrl::makeAllTypeItem(ROLE* role, const vector<ITEM_INFO> &vecItemInfo)
{
	list<ITEM_CACHE*> lstItemCachePkg, lstPropsCachePkg;
	for ( auto &it : vecItemInfo )
	{
		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( it.itemIndex );
		if ( itemCfg == NULL )
			continue;

		makeItemByType(role, itemCfg, &lstItemCachePkg, &lstPropsCachePkg, it );
	
	}
	
	ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_EQUIP);
	ItemCtrl::notifyClientItem(role, lstPropsCachePkg, PKG_TYPE_ITEM);

	return 0;
}

/*
@role获取itemNum件itemCfg物品，钱币通知客户端
*/
int ItemCtrl::makeItemByType(ROLE* role, CONFIG::ITEM_CFG* itemCfg, list<ITEM_CACHE*> *pListItemCachePkg, list<ITEM_CACHE*> *pListPropsCachePkg, const ITEM_INFO & ii, const bool checkAutoSell)
{
	if ( IS_COIN(itemCfg) )//金币
	{
		ADDUP(role->dwCoin, ii.itemNums );
		notifyTypeValuebyRoleID(role->dwRoleID, ROLE_PRO_COIN, role->dwCoin);
		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, ii.itemNums, 0, purpose::reward, 0);
	}
	else if ( IS_INGOT(itemCfg) )//元宝
	{
		ADDUP(role->dwIngot, ii.itemNums);
		notifyTypeValuebyRoleID(role->dwRoleID, ROLE_PRO_INGOT, role->dwIngot);
		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, ii.itemNums, purpose::reward, 0);
	}
	else if ( IS_HONOUR( itemCfg ) )//声望
	{
		ADDUP(role->dwHonour, ii.itemNums);
		notifyTypeValuebyRoleID(role->dwRoleID, ROLE_PRO_HONOUR, role->dwHonour);
	}
	else if ( IS_EQUIP( itemCfg ) )
	{
		return role->m_packmgr.makeItem(itemCfg, ii.itemNums, pListItemCachePkg, checkAutoSell, ii.byQuality );	
	}
	else
	{
		return role->m_packmgr.makeItem( itemCfg, ii.itemNums, pListPropsCachePkg, checkAutoSell, ii.byQuality );
	}
	return 0;
}

//包裹扩展
int ItemCtrl::clientPackageExtend( ROLE* role, unsigned char * data, size_t dataLen )
{

//	C:[BYTE]
//		[packType]
//		packType:包裹类型, 2:普通包裹; 3:仓库包裹;4

//	S:[WORD, WORD]
//		[error, nextCost]
//		error:错误码
//			0:成功
//			1:元宝不足
//			2:已经最大容量
//		nextCost:下次开启新格子消耗元宝


	BYTE pkgType;
	if ( !BASE::parseBYTE( data, dataLen, pkgType) )
		return 0;

	WORD wErrCode = 0;
	do {

		CONFIG::PKG_EXTEND_CFG* pkgExtendCfg = CONFIG::getPkgExtendCfg( role, (PKG_TYPE)pkgType );

		COND_BREAK( pkgExtendCfg == NULL, wErrCode, 2 );

		if ( pkgExtendCfg->needCoin )
		{
			COND_BREAK( role->dwCoin < pkgExtendCfg->needCoin, wErrCode, 2 );
			ADDUP(role->dwCoin, -(int)pkgExtendCfg->needCoin);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, -(int)pkgExtendCfg->needCoin, 0, purpose::package_extend, 0);
			notifyCoin( role );

		}
		else 	if ( pkgExtendCfg->needIngot )//只会扣1个
		{
			COND_BREAK( role->dwIngot < pkgExtendCfg->needIngot, wErrCode, 2 );
			ADDUP(role->dwIngot, -(int)pkgExtendCfg->needIngot);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)pkgExtendCfg->needIngot, purpose::package_extend, 0);
			notifyIngot( role );
		}

		if ( pkgType == PKG_TYPE_EQUIP )
		{
			role->m_packmgr.curEquipPkgLimit+=5;
		}
		//else if ( pkgType == PKG_TYPE_FUWEN )
		//{
		//	//role->m_packmgr.curStorageLimit+=5;
		//}
		//else if ( pkgType == PKG_TYPE_MINE )
		//{
		//	role->m_packmgr.curMineLimit+=5;
		//}
	
	}while(0);


	string strData;
	S_APPEND_WORD( strData, wErrCode );
	S_APPEND_DWORD( strData, CONFIG::getPkgExtendCost( role, (PKG_TYPE)pkgType ) );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_PACKAGE_EXTEND, strData) );
	
	return 0;
}


//void rmMineItemNotify( ROLE* role, CONFIG::ITEM_CFG* itemCfg, DWORD dwDelNum )
//{
//	//已经确保有此index,数量足够
//
//	list<ITEM_CACHE*> lstNotifyPkg;
//
//	for ( auto it = role->m_packmgr.lstMinePkg.begin(); it!=role->m_packmgr.lstMinePkg.end();  )
//	{
//		if ( itemCfg == (*it)->itemCfg ) 
//		{
//			dwDelNum -= 1;
//			lstNotifyPkg.push_back( *it );
//
//			role->m_packmgr.lstMinePkg.erase( it++ );
//		}
//		else		
//		{
//			++it;
//		}
//
//		if ( dwDelNum == 0 )
//			break;
//		
//	}
//
//	ItemCtrl::notifyClientItem(role, lstNotifyPkg, PKG_TYPE_MINE);
//
//	for ( auto &it : lstNotifyPkg )
//		delete it;
//
//}




int useRandBox( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{

	WORD needItemIndex = (WORD)item->itemCfg->atk_max;
	DWORD needNum = item->itemCfg->atk_min;

	CONFIG::ITEM_CFG* keyItemCfg = NULL;
	//填0表示不需要钥匙
	if ( needItemIndex != 0 )
	{
		keyItemCfg = CONFIG::getItemCfg( needItemIndex );
		if ( keyItemCfg == NULL )
			return 1;

		//数量不足
		if ( ItemCtrl::getPropItemNums( role, keyItemCfg ) < needNum )
			return 1;
	}


	DWORD dropID = role->byJob == E_JOB_WARRIOR ? ( role->bySex == E_SEX_MALE ? item->itemCfg->stamina : item->itemCfg->strength )
					: role->byJob == E_JOB_MAGIC ? ( role->bySex == E_SEX_MALE ? item->itemCfg->agility : item->itemCfg->intellect )
											: ( role->bySex == E_SEX_MALE ? item->itemCfg->spirit : item->itemCfg->hp_max );

	//DWORD dropTimes = item->itemCfg->sol_max;

	
	vector<ITEM_INFO> tryV;
	//for ( DWORD i=0; i<dropTimes; i++)
	//{
	CONFIG::getRandItem( dropID, tryV, true );
	//}

	//包裹放不下
	if ( E_SLOT_OK != ItemCtrl::getEnoughSlot( role, tryV ) )
		return 2;

	if ( keyItemCfg !=NULL && needNum > 0)
	{
		needNum = ItemCtrl::rmItemNotify( role, role->m_packmgr.lstItemPkg, PKG_TYPE_ITEM, keyItemCfg, needNum );
	}

	ItemCtrl::makeAllTypeItem(role, tryV);
	
	return E_SUCC;

}

//礼包同宝箱，只是没有配置开启所需的钥匙
int useFixedBox( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{
	DWORD dropID = role->byJob == E_JOB_WARRIOR ? ( role->bySex == E_SEX_MALE ? item->itemCfg->stamina : item->itemCfg->strength )
		: role->byJob == E_JOB_MAGIC ? ( role->bySex == E_SEX_MALE ? item->itemCfg->agility : item->itemCfg->intellect )
		: ( role->bySex == E_SEX_MALE ? item->itemCfg->spirit : item->itemCfg->hp_max );
	
	vector<ITEM_INFO> tryV;
	CONFIG::getRandItem( dropID, tryV, true );

	//包裹放不下
	if ( E_SLOT_OK != ItemCtrl::getEnoughSlot( role, tryV ) )
		return 2;

	ItemCtrl::makeAllTypeItem(role, tryV);
	
	return E_SUCC;

}

int useExpDan( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{

	DWORD dwExpAdd = item->itemCfg->stamina;

	CONFIG::addRoleExp( role, role->dwExp, dwExpAdd );

	return E_SUCC;

}


//int useFacContributeDan( ROLE* role, ITEM_CACHE* item)
//{
//
//	DWORD dwAddContribute = item->itemCfg->hp_max;
//
//	if ( !FactionCtrl::addRoleFacContribute( role, dwAddContribute ) )
//	{
//		return 1;
//	}
//
//	return E_SUCC;
//
//}


int useCoinCard( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{

	DWORD dwAddCoin = item->itemCfg->stamina;

	ADDUP(role->dwCoin, dwAddCoin );
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, dwAddCoin, 0, purpose::coin_card, 0);
	notifyCoin( role );

	return E_SUCC;

}
int useIngotCard( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{

	DWORD dwAddIngot = item->itemCfg->stamina;

	ADDUP(role->dwIngot, dwAddIngot );
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, dwAddIngot, purpose::ingot_card, 0);
	notifyIngot( role );

	return E_SUCC;
}


int useBlessOil( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{
//	--祝福油相关
//opType:3符文操作相关
//subType:1:对武器使用祝福油
//		2:对衣服使用祝福油
//error:错误码
//	  1:材料不足
//	  2:包裹已满不可合成/分解
//	  3:没有穿戴对应装备

	//攻击幸运值	防御幸运	成功率
	//	小于30	小于30	80%概率升，20%不变
	//	30~59	31~59	60%概率升，30%不变，10%概率降
	//	60~69	60~69	40%概率升，40%不变，20%概率降
	//	70~-79	70~-79	30%概率升，50%概率不变，25%概率降
	//	80~90	80~90	20%概率升，61%概率不变，19%概率降

	RETURN_COND( bySubOpera!=1 && bySubOpera!=2, 4 );

	ITEM_CACHE* ic = NULL;
	ic = bySubOpera == 1 ? role->vBodyEquip[BODY_INDEX_WEAPON] :  role->vBodyEquip[BODY_INDEX_CLOTH];
	if ( ic == NULL )
		return 3;
	WORD wLuck = bySubOpera == 1 ? ic->byLuckPoint : ic->byLuckDefPoint;
	int nAddLuck = 0;
	int nAddCoin = 1000;

	DWORD dwRand = BASE::randTo( 100 );

	if (wLuck < 30 )
	{
		if ( dwRand <= 80 )
		{
			nAddLuck = 1;
			nAddCoin = 0;
		}
	}
	else if (wLuck <= 59)
	{
		if ( dwRand <= 60 )
		{
			nAddLuck = 1;
			nAddCoin = 0;
		}
		else if ( dwRand <= 70 )
		{
			nAddLuck = -1;
		}
	}
	else if (wLuck <= 69)
	{
		if ( dwRand <= 40 )
		{
			nAddLuck = 1;
			nAddCoin = 0;
		}
		else if ( dwRand <= 60 )
		{
			nAddLuck = -1;
		}
	}
	else if (wLuck <= 79)
	{
		if ( dwRand <= 30 )
		{
			nAddLuck = 1;
			nAddCoin = 0;
		}
		else if ( dwRand <= 55 )
		{
			nAddLuck = -1;
		}
	}
	else if (wLuck < 90)
	{
		if ( dwRand <= 20 )
		{
			nAddLuck = 1;
			nAddCoin = 0;
		}
		else if ( dwRand <= 39 )
		{
			nAddLuck = -1;
		}
	}

	if (nAddLuck != 0)
	{
		if ( bySubOpera == 1 )
		{
			ADDUP(ic->byLuckPoint, (int8)nAddLuck );
			RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_EQUIP_LUCK );
		}
		else
		{
			ADDUP(ic->byLuckDefPoint, nAddLuck );
			RoleMgr::getMe().judgeCompleteTypeTask( role, E_TASK_CLOTH_LUCK );
		}
		ItemCtrl::notifyItem( role, ic, PKG_TYPE_BODY);
		ItemCtrl::updateNotifyRoleAttr(role);

	}

	if (nAddCoin > 0 )
	{
		ADDUP(role->dwCoin, nAddCoin );
		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, nAddCoin, 0, purpose::bless_oil, 0);
		notifyCoin(role);
	}
	

	return E_SUCC;

}

int useBossCard( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{
	ADDUP(role->byAllBuyBossTimes, 1);

	return E_SUCC;
}

int useVipCard( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{
	WORD wTmpVipLevel = (WORD)item->itemCfg->stamina;
	DWORD cd = item->itemCfg->strength;

	//同一 等级，只要有剩余时间，都不给更新
	if ( role->getVipLevel() >= wTmpVipLevel )
		return 1;
	
	role->dwTmpVipEndTime = time(nullptr) + cd;	
	role->wTmpVipLevel = wTmpVipLevel;
	notifywVipLevel(role);
	//if ( byRet == 0 )
	//{
		ITEM_INFO para1(0, role->getVipLevel());
		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_VIP_LOGIN, &para1);
	//}

	return E_SUCC;
}

//使用宝石
int useGem( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{
//	--宝石相关
//opType:1宝石操作相关
//subType:1:宝石合成
//		  2:宝石分解			
//error:错误码
//	  1:材料不足
//	  2:包裹已满不可合成/分解
//	  3:已达最高等级

	RETURN_COND( bySubOpera != 1 /*&& bySubOpera!=2*/, 4 );

	CONFIG::GEM_CFG* gemCfg = CONFIG::getGemCfg(item->itemCfg->id);

	RETURN_COND( gemCfg==NULL, 3 );

	//if ( bySubOpera == 1 )//升级
	//{

		WORD needItemIndex = gemCfg->upgrade_id;

		WORD targetItemIndex = gemCfg->target_id;

		DWORD needNum = gemCfg->upgrade_num;

		CONFIG::ITEM_CFG* needItemCfg = CONFIG::getItemCfg( needItemIndex );

		RETURN_COND( needItemCfg == NULL, 1 );

		CONFIG::ITEM_CFG* targetItemCfg = CONFIG::getItemCfg( targetItemIndex );

		RETURN_COND( targetItemCfg == NULL, 1 );

		RETURN_COND( ItemCtrl::getPropItemNums( role, needItemCfg ) < needNum, 1 );

		RETURN_COND( ItemCtrl::makeOneTypeItem( role, targetItemCfg, 1 ) == -1, 2)

		if ( needItemCfg !=NULL && needNum > 0)
		{
			needNum = ItemCtrl::rmItemNotify( role, role->m_packmgr.lstItemPkg, PKG_TYPE_ITEM, needItemCfg, needNum );
		}

		item = NULL;//删除自身 引用传出  防止外面再次删除

	//}
	//else//分解
	//{
	//	if ( RAND_HIT(gemCfg->decompose_item_ratio) )
	//	{
	//		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( gemCfg->decompose_item_id );

	//		RETURN_COND( itemCfg == NULL, 1 );

	//		RETURN_COND( ItemCtrl::makeOneTypeItem( role, itemCfg, gemCfg->decompose_item_num, (BYTE)itemCfg->rare ) == -1, 2)

	//		role->vecTempGetItems.push_back(itemCfg->id);
	//	}
	//	//if ( RAND_HIT(gemCfg->decompose_coin_ratio) )
	//	else
	//	{
	//		ADDUP(role->dwCoin, (int)gemCfg->decompose_coin_num);
	//		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, (int)gemCfg->decompose_coin_num, 0, purpose::use_gem, 0);
	//		notifyCoin(role);
	//	}

	//}

	return E_SUCC;
}

//符文合成 兑换
int useFuwen( ROLE* role, ITEM_CACHE* &item, BYTE byOperaType = 0, BYTE bySubOpera = 0 )
{
//	--符文相关
//opType:2符文操作相关
//subType:1:符文碎片合成
//
//error:错误码
//	  1:材料不足
//	  2:包裹已满不可合成/分解

	RETURN_COND( bySubOpera!=1, 4 );

	do 
	{
		BYTE haveNum = 0;

		BYTE needNum = 10;

		vector<ITEM_CACHE*> vecProps;

		vector<BYTE> vecNums;

		//nErr = 1;

		for (auto it = role->m_packmgr.lstItemPkg.begin(); it != role->m_packmgr.lstItemPkg.end() ; it++)
		{
			if ( (*it)->itemCfg->id != 404 )
			{
				continue;
			}

			size_t oneTypeNum = (*it)->itemNums;

			size_t getOneTypeNum = 0;

			while( oneTypeNum > 0 )
			{
				oneTypeNum--;

				getOneTypeNum++;

				haveNum++;

				if ( haveNum >= needNum )
				{
					break;
				}
			}

			vecProps.push_back( *it );

			vecNums.push_back( getOneTypeNum );

			if ( haveNum >= needNum )
			{
				WORD target_id = 0;

				DWORD dwRand = BASE::randTo( 100 );
				if ( dwRand <= 30 )
				{
					target_id = CONFIG::randOneRune(3);
				}
				else if ( dwRand <= 80 )
				{
					target_id = CONFIG::randOneRune(4);
				}
				else
				{
					target_id = CONFIG::randOneRune(5);
				}

				CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( target_id );

				//RETURN_COND( ItemCtrl::makeOneTypeItem( role, itemCfg, 1, (BYTE)itemCfg->rare ) == -1, 2)

				// 生成符文
				std::list<ITEM_CACHE*> lstItemsPkg;
				if ( E_SUCC != role->m_packmgr.makeItem( itemCfg, 1, &lstItemsPkg) )
				{
					return 0;
				}
				ItemCtrl::notifyClientItem(role, lstItemsPkg, PKG_TYPE_FUWEN);

				for ( size_t i = 0; i < vecProps.size(); i++)
				{
					ItemCtrl::rmItemNotify( role,  role->m_packmgr.lstItemPkg, PKG_TYPE_ITEM, vecProps[i]->itemCfg, vecNums[i] );
				}

				role->vecTempGetItems.push_back(itemCfg->id);

				//COND_BREAK( 1, nErr, 0 );
				break;
			}

		}

	} while (0);

	item = NULL;//删除自身 引用传出  防止外面再次删除

	return E_SUCC;
}

//int useLingZhu( ROLE* role, ITEM_CACHE* item)
//{
//	auto icLingZhu = role->vBodyEquip[ BODY_INDEX_LINGZHU ];
//
//	if ( icLingZhu->dwLingZhuExp < icLingZhu->itemCfg->hp_max )
//	{
//		return 1;
//	}
//
//	//可以再加别的灵珠
//	CONFIG::addRoleExp( role, role->dwExp, icLingZhu->itemCfg->hp_max );
//	return E_SUCC;
//
//}

//int useSkillBook( ROLE* role, ITEM_CACHE* item)
//{
//	return SKILL::unlockSkillByItem(role, item->itemCfg);
//}

void ItemCtrl::init()
{

	m_mapItemUse.clear();

	m_mapItemUse[ SUB_TYPE_ITEM_BOX ] = useRandBox;
	m_mapItemUse[ SUB_TYPE_ITEM_FIXED_BOX ] = useFixedBox;

	m_mapItemUse[ SUB_TYPE_EXP_DAN ] = useExpDan;
	//m_mapItemUse[ SUB_TYPE_FAC_CONTRIBUTE_DAN ] = useFacContributeDan;
	m_mapItemUse[ SUB_TYPE_COIN_CARD ] = useCoinCard;
	m_mapItemUse[ SUB_TYPE_INGOT_CARD ] = useIngotCard;
	m_mapItemUse[ SUB_TYPE_BLESS_OIL ] = useBlessOil;
	m_mapItemUse[ ITEM_STYPE_VIP_CARD ] = useVipCard;
	m_mapItemUse[ ITEM_STYPE_BOSS_CARD ] = useBossCard;
	//m_mapItemUse[ SUB_TYPE_LINGZHU_USE ] = useLingZhu;
	//m_mapItemUse[ SUB_TYPE_ITEM_SKILLBOOK ] = useSkillBook;

	m_mapItemUse[ SUB_TYPE_EQUIP_GEM_RED    ] = useGem;
	m_mapItemUse[ SUB_TYPE_EQUIP_GEM_YELLOW ] = useGem;
	m_mapItemUse[ SUB_TYPE_EQUIP_GEM_BLUE   ] = useGem;
	m_mapItemUse[ SUB_TYPE_EQUIP_GEM_GREEN  ] = useGem;
	m_mapItemUse[ SUB_TYPE_EQUIP_GEM_PURPLE ] = useGem;
	m_mapItemUse[ SUB_TYPE_FUWEN ] = useFuwen;

	//SUB_TYPE_EQUIP_GEM_RED    =  21, //红宝石
	//SUB_TYPE_EQUIP_GEM_YELLOW =  22, //黄宝石 
	//SUB_TYPE_EQUIP_GEM_BLUE   =  23, //蓝宝石
	//SUB_TYPE_EQUIP_GEM_GREEN  = 24, //绿宝石
	//SUB_TYPE_EQUIP_GEM_PURPLE =  25, //紫宝石
}

//// 熔炼时是否生成新装备
//bool ItemCtrl::isGenerateSmeltEquip(ROLE* role, DWORD count)
//{
//	if (NULL == role)
//	{
//		return false;
//	}
//
//	if (role->bySmeltEquipCount >= 10)
//	{
//		return true;
//	}
//
//	return false;
//}

//// 获得熔炼预期还需要的装备数量
//BYTE ItemCtrl::getNeedSmeltEquip(ROLE* role)
//{
//	if (NULL == role)
//	{
//		return 0;
//	}
//
//	BYTE byNeed = 10;
//	
//	REDUCEUNSIGNED(byNeed, role->bySmeltEquipCount);
//
//	return byNeed;
//}

//// 增加熔炼的装备数量
//void ItemCtrl::addSmeltEquipCount(ROLE* role, BYTE count)
//{
//	if (NULL == role)
//	{
//		return;
//	}
//
//	role->bySmeltEquipCount += count;
//	role->bySmeltEquipCount %= 10;
//}

int ItemCtrl::clientProcsUse( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//道具使用
//#define	S_MINI_ITEM_USE	0x004A
//C:[DWORD, BYTE, BYTE]
//	[itemId, opType, subType]
//
//itemId:道具唯一ID
//
//opType:道具操作类型
//subType:子类型操作
//
//S:[WORD, [WORD]]
//	[error, [retIteIdx]]
//error:错误码
//		  0:成功
//retIteIdx:操作获得新道具静态表Id索引
//
//		  --宝石相关
//opType:1宝石操作相关
//subType:1:宝石合成
//		2:宝石分解			
//error:错误码
//	  1:材料不足
//	  2:包裹已满不可合成/分解
//	  3:已达最高等级
//
//	  --符文相关
//opType:2符文操作相关
//subType:1:符文碎片合成
//
//error:错误码
//	  1:材料不足
//	  2:包裹已满不可合成/分解
//
//	  --祝福油相关
//opType:3符文操作相关
//subType:1:对武器使用祝福油
//		2:对衣服使用祝福油
//error:错误码
//	  1:材料不足
//	  2:包裹已满不可合成/分解
//	  3:没有穿戴对应装备
//
//	  --可使用道具操作
//opType:4可使用道具操作相关
//subType:0
//error:错误码
//	  1:材料不足
//	  2:包裹已满不可合成/分解


	ITEM_CACHE* item;
	BYTE byOperaType;
	BYTE bySubOpera;
	RETURN_COND( !BASE::parseDWORD( data, dataLen, (*(DWORD*)&item)), 0 );
	RETURN_COND( !BASE::parseBYTE( data, dataLen, byOperaType), 0 );
	RETURN_COND( !BASE::parseBYTE( data, dataLen, bySubOpera), 0 );

	WORD wErrCode = 0;
	role->vecTempGetItems.clear();

	do
	{
		PKG_TYPE pkgType = role->m_packmgr.findItem( item );

		COND_BREAK( pkgType == PKG_TYPE_NONE, wErrCode, 2 );

		WORD subType = item->itemCfg->sub_type;

		COND_BREAK( item->itemCfg->type != TYPE_ITEM, wErrCode, 3 );

		COND_BREAK( item->itemNums == 0, wErrCode, 4 );

		//COND_BREAK ( IS_JOB_LIMIT(role, item->itemCfg), wErrCode, 6 );
		//COND_BREAK ( IS_LEVEL_LIMIT(role, item->itemCfg), wErrCode, 7 );
		//COND_BREAK ( IS_SEX_LIMIT(role, item->itemCfg), wErrCode, 8 );

		auto it = m_mapItemUse.find( subType );
		COND_BREAK( it == m_mapItemUse.end(), wErrCode, 5 );

		int ret = it->second( role, item, byOperaType, bySubOpera );
		if ( ret == E_SUCC && item != NULL )
		{
			//点那个删哪个,所以不用rmItemNotify( ROLE* role, WORD wIndex, DWORD dwDelNum )
			item->itemNums -= 1;
			notifyItem( role, item, pkgType);

			if ( item->itemNums == 0 )
			{
				if ( pkgType == PKG_TYPE_BODY /*&& IS_LINGZHU(item->itemCfg)*/ )
				{
					//灵珠不会加属性
					CC_SAFE_DELETE(item);
					role->vBodyEquip[ BODY_INDEX_LINGZHU ] = NULL;
				}
				else
				{
					role->m_packmgr.rmItemInPkg( item);
				}
			}
		}

		wErrCode = (WORD) ret;

	}while(0);


	string strData;
	S_APPEND_WORD( strData, wErrCode );
	for (auto id : role->vecTempGetItems )
	{
		S_APPEND_WORD( strData, id );
	}
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ITEM_USE, strData) );

	return 0;

}

////#道具使用  弃用
//int ItemCtrl::clientItemUse( ROLE* role, unsigned char * data, size_t dataLen )
//{
////	C:[DWORD]
////		[itemId]
////		itemId:道具唯一ID
////	S:[WORD]
////		[error]
////			error:错误码
////				0:成功
////				1:等级不足
////				2:不可使用
//
//
//	ITEM_CACHE* item;
//	if ( !BASE::parseDWORD( data, dataLen, (*(DWORD*)&item)) )
//		return 0;
//
//	WORD wErrCode = 0;
//
//	do
//	{
//		PKG_TYPE pkgType = role->m_packmgr.findItem( item );
//
//		COND_BREAK( pkgType == PKG_TYPE_NONE, wErrCode, 2 );
//
//		WORD subType = ( IS_LINGZHU(item->itemCfg) ) ? SUB_TYPE_LINGZHU_USE : item->itemCfg->sub_type;
//
//		COND_BREAK( item->itemCfg->type != TYPE_ITEM && subType != SUB_TYPE_LINGZHU_USE , wErrCode, 3 );
//		
//		COND_BREAK( item->itemNums == 0, wErrCode, 4 );
//
//		COND_BREAK ( IS_JOB_LIMIT(role, item->itemCfg), wErrCode, 6 );
//		COND_BREAK ( IS_LEVEL_LIMIT(role, item->itemCfg), wErrCode, 7 );
//		COND_BREAK ( IS_SEX_LIMIT(role, item->itemCfg), wErrCode, 8 );
//		
//		auto it = m_mapItemUse.find( subType );
//		COND_BREAK( it == m_mapItemUse.end(), wErrCode, 5 );
//
//		int ret = it->second( role, item );
//		if ( ret == E_SUCC )
//		{
//			//点那个删哪个,所以不用rmItemNotify( ROLE* role, WORD wIndex, DWORD dwDelNum )
//			item->itemNums -= 1;
//			notifyItem( role, item, pkgType);
//
//			if ( item->itemNums == 0 )
//			{
//				if ( pkgType == PKG_TYPE_BODY && IS_LINGZHU(item->itemCfg) )
//				{
//					//灵珠不会加属性
//					delete item;
//					role->vBodyEquip[ BODY_INDEX_LINGZHU ] = NULL;
//				}
//				else
//				{
//					role->m_packmgr.rmItemInPkg( item);
//				}
//			}
//		}
//
//		wErrCode = (WORD) ret;
//		
//	}while(0);
//
//	
//	string strData;
//	S_APPEND_WORD( strData, wErrCode );
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ITEM_USE, strData) );
//
//	return 0;
//
//}




