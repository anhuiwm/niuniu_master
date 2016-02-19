#ifndef __PACK_MGR_H__
#define __PACK_MGR_H__

#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"
#include <set>

class ROLE;

//using namespace std;

//玩家信息
class PackMgr
{
public:
	PackMgr(ROLE* role);
	~PackMgr();

	list<ITEM_CACHE*> lstEquipPkg;//装备包裹  以后改名字
	list<ITEM_CACHE*> lstMinePkg;
	list<ITEM_CACHE*> lstItemPkg;//道具
	list<ITEM_CACHE*> lstFuwenPkg; //  符文

	BYTE curEquipPkgLimit;		//当前最大的装备包裹格子数
	BYTE curMineLimit;			//当前最大的矿物格子数
	BYTE curItemPkgLimit;		//当前最大的道具背包格子数
	BYTE curFuwenPkgLimit;

	void cache2Blob(ROLE_BLOB& roleBlob );
	void blob2Cache( const ROLE_BLOB& roleBlob );

	void patch();
	void giveItems();

	int makeItem( CONFIG::ITEM_CFG* itemCfg, size_t itemNum, list<ITEM_CACHE*>* pLstItemCache=NULL, bool checkAutoSell =false, BYTE byQuality = 1);//, BYTE byChest1_RandNums7=0 );
	bool makeItem3( ITEM_CACHE* itemCache, list<ITEM_CACHE*>* pLstNotifyItem, list<ITEM_CACHE*>* pLstPropsNotify);

	bool isAutoSellItem( ITEM_CACHE* itemCache );

	//#技能书道具使用
	BYTE clientSkillBookUse( const WORD& itemID, const BYTE& itemNum );
	// 获取道具数量
	DWORD getItemCount( WORD itemid);
	// 扣除指定道具数量
	bool consumeItem( WORD itemid, DWORD count);
	// 道具数量是否够
	bool hasEnoughItem( WORD itemid,DWORD count);
	bool rmItemInPkg( ITEM_CACHE* item);

	PKG_TYPE findItem(ITEM_CACHE* item, int* pBodyIndex=NULL );
protected:
	ROLE* owner;
};

#endif	//__ROLE_SKILL_H__
