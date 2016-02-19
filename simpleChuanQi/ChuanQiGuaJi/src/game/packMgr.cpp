
#include <algorithm>

#include "protocal.h"
#include "packMgr.h"
#include "role.h"
#include "itemCtrl.h"
#include "roleCtrl.h"
#include "logs_db.h"
#include "service.h"

PackMgr::PackMgr(ROLE* role)
	:owner(role)
{
	curEquipPkgLimit = DEFAULT_PKG_NUMS_EQUIP;
	curItemPkgLimit  = DEFAULT_PKG_NUMS_PROP;
	curMineLimit	 = DEFAULT_MINE_PKG_NUMS;
	curFuwenPkgLimit = MAX_FUWEN_PKG_NUMS;
}

PackMgr::~PackMgr()
{
	for( auto &it : lstEquipPkg )
	{
		//delete it;
		CC_SAFE_DELETE(it);
	}

	for( auto &it : lstMinePkg )
	{
		//delete it;
		CC_SAFE_DELETE(it);
	}

	for( auto &it : lstItemPkg )
	{
		//delete it;
		CC_SAFE_DELETE(it);
	}

	for( auto &it : lstFuwenPkg )
	{
		//delete it;
		CC_SAFE_DELETE(it);
	}
}

static void itemCache2Data( ROLE* role, const list<ITEM_CACHE*>& lstItemCache, ITEM_DATA* arrItemData)
{
	size_t i =0;
	for ( auto it = lstItemCache.begin(); it!= lstItemCache.end(); ++it )
	{
		ITEM_CACHE*  itemCache = (*it);

		//assert( i< MAX_PKG_NUMS );

		//MEMCPY_OFFSET_ITEMDATA_ITEMNUMS( &arrItemData[i], itemCache );

		arrItemData[i].initFromItemCache( itemCache );

		i++;

		if ( i >= MAX_PKG_NUMS )
			break;

	}

	if ( lstItemCache.size() > MAX_PKG_NUMS )
	{
		logff( "xxxxxxxxxxxxxxx     size:%u, roleid:%u", lstItemCache.size(), role->dwRoleID );
	}
}

void PackMgr::cache2Blob(ROLE_BLOB& roleBlob )
{
	roleBlob.curEquipPkgLimit = curEquipPkgLimit;
	roleBlob.curItemPkgLimit = curItemPkgLimit;
	roleBlob.curMineLimit = curMineLimit;
	roleBlob.curFuwenPkgLimit = curFuwenPkgLimit;

	//����,�ֿ�
	itemCache2Data( owner, this->lstEquipPkg, roleBlob.arrPkg );
	itemCache2Data( owner, this->lstItemPkg, roleBlob.arrProps );
	itemCache2Data( owner, this->lstFuwenPkg, roleBlob.arrFuwen );

	//��ʯ����
	//{
	//	size_t i =0;
	//	for ( auto it = this->lstMinePkg.begin(); it!= this->lstMinePkg.end(); ++it )
	//	{
	//		ITEM_CACHE* itemCache = (*it);

	//		///assert( i< MAX_MINE_PKG_NUMS );

	//		roleBlob.arrMine[i].itemIndex = itemCache->itemCfg->id;
	//		roleBlob.arrMine[i].itemNum = itemCache->itemNums;

	//		i++;
	//	}
	//}
}


static void itemData2Cache(list<ITEM_CACHE*>& lstItemCache, const ITEM_DATA* arrItemData, size_t nums)
{
	for ( size_t i=0; i<nums; i++)
	{
		if ( arrItemData[i].itemIndex == 0 )
			break;

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( arrItemData[i].itemIndex );
		if ( itemCfg == NULL )
			continue;

		ITEM_CACHE* itemCache = new ITEM_CACHE( arrItemData+i );

		lstItemCache.push_back( itemCache );

	}
}

void PackMgr::blob2Cache( const ROLE_BLOB& roleBlob )
{
	this->curEquipPkgLimit = roleBlob.curEquipPkgLimit;
	this->curItemPkgLimit = roleBlob.curItemPkgLimit;
	this->curMineLimit = roleBlob.curMineLimit;

	//װ������,����
	itemData2Cache( this->lstEquipPkg, roleBlob.arrPkg, this->curEquipPkgLimit );	
	itemData2Cache( this->lstItemPkg, roleBlob.arrProps, this->curItemPkgLimit );
	itemData2Cache( this->lstFuwenPkg, roleBlob.arrFuwen, this->curFuwenPkgLimit);

	////��ʯ����
	//for ( size_t i=0; i<this->curMineLimit; i++)
	//{
	//	if ( roleBlob.arrMine[i].itemIndex == 0 )
	//		break;

	//	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( roleBlob.arrMine[i].itemIndex );
	//	if ( itemCfg == NULL )
	//		continue;

	//	//ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, roleBlob.arrMine[i].itemPure );
	//	ITEM_CACHE* itemCache = new ITEM_CACHE( roleBlob.arrMine+i );
	//	this->lstMinePkg.push_back( itemCache );		
	//}
}


void PackMgr::giveItems()
{
	ROLE* role = owner;

	//����id��װ��Ʒ�ʣ���Ϊ1
	//	���ϣ�
	//	//ְҵ սʿ
	//	//�Ա� ��
	//	//11701

	//	//ְҵ սʿ
	//	//�Ա� Ů
	//	//11701

	//	//ְҵ ��ʦ
	//	//�Ա� ��
	//	//11701

	//	//ְҵ ��ʦ
	//	//�Ա� Ů
	//	//11701

	//	//ְҵ ��ʿ
	//	//�Ա� ��
	//	//11701

	//	//ְҵ ��ʿ
	//	//�Ա� Ů
	//	//11701

	WORD wWeapon = 11701;
	//WORD wCloth = 0;
	//if ( role->byJob == E_JOB_WARRIOR )
	//{
	//	wWeapon = 11001;
	//	wCloth = role->bySex == E_SEX_MALE ? 11101 : 11201;
	//}
	//else if ( role->byJob == E_JOB_MAGIC )
	//{
	//	wWeapon = 12001;
	//	wCloth = role->bySex == E_SEX_MALE ? 12101 : 12201;
	//}
	//else if ( role->byJob == E_JOB_MONK )
	//{
	//	wWeapon = 13001;
	//	wCloth = role->bySex == E_SEX_MALE ? 13101 : 13201;
	//}

	{
		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(wWeapon);
		if ( itemCfg && IS_EQUIP(itemCfg) )
		{
			int bodyIndex = getBodyIndexBySubtype( role, itemCfg->sub_type );

			ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, 1, 1 );
			role->vBodyEquip[bodyIndex] = itemCache;
		}

	}

	//{
	//	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(wCloth);
	//	if ( itemCfg && IS_EQUIP(itemCfg) )
	//	{
	//		ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, 1 );
	//		role->vBodyEquip[BODY_INDEX_CLOTH] = itemCache;			
	//	}

	//}

	//	������
	//	//=============
	//	//սʿ��
	//	//11001
	//	//11101

	//	//սʿŮ
	//	//11001
	//	//11101

	//	//��ʦ��
	//	//12001
	//	//11101

	//	//��ʦŮ
	//	//12001
	//	//11101

	//	//��ʿ��
	//	//13001
	//	//11101

	//	//��ʿŮ
	//	//13001
	//	//11101

	WORD wItem1 = 0;
	WORD wItem2 = 0;
	if ( role->byJob == E_JOB_WARRIOR )
	{
		wItem1 = 11001;
		wItem2 = 11101;//role->bySex == E_SEX_MALE ? 11102 : 11202;
	}
	else if ( role->byJob == E_JOB_MAGIC )
	{
		wItem1 = 12001;
		wItem2 = 11101;//role->bySex == E_SEX_MALE ? 12102 : 12202;
	}
	else if ( role->byJob == E_JOB_MONK )
	{
		wItem1 = 13001;
		wItem2 = 11101;//role->bySex == E_SEX_MALE ? 13102 : 13202;
	}

	{
		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(wItem1);
		if ( itemCfg )
		{
			ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, 1, 1 );
			this->lstEquipPkg.push_back(itemCache);
		}

	}

	{
		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(wItem2);
		if ( itemCfg )
		{
			ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, 1, 1 );
			this->lstEquipPkg.push_back(itemCache);
		}

	}
}

void PackMgr::patch()
{
	ROLE* role = owner;

	if ( this->curEquipPkgLimit == 0 )
		this->curEquipPkgLimit = DEFAULT_PKG_NUMS_EQUIP;
	if ( this->curItemPkgLimit == 0 )
		this->curItemPkgLimit = DEFAULT_PKG_NUMS_PROP;

	if ( this->curMineLimit == 0 )
		this->curMineLimit = DEFAULT_MINE_PKG_NUMS;

	if ( role->wLevel ==0 )
		role->wLevel =1;

	if ( role->wCanFightMaxMapID ==0 )
		role->wCanFightMaxMapID =1;

	if ( role->wLastFightMapID ==0 )
		role->wLastFightMapID =1;

	//if ( role->mainTaskInfo.wTaskID == 0 )
	//{
	//	role->mainTaskInfo.wTaskID = 1;	//��һ����������ʼ:�ﵽ10��
	//	
	//}

	//if ( role->itemForge == NULL )
	//	CONFIG::forgeRefresh( role );
}

//���ӵ��Ѿ����ڵ���Ʒ��,��༸��
static size_t canMergeNum(const list<ITEM_CACHE*>& lstItemCache, CONFIG::ITEM_CFG* itemCfg)
{

	size_t nums = 0;

	for ( auto it = lstItemCache.begin(); it!=lstItemCache.end(); ++it )
	{
		ITEM_CACHE* itemCache = *it;
		if ( itemCache->itemCfg != itemCfg )
			continue;

		size_t freeNums = itemCfg->count - itemCache->itemNums;

		nums += freeNums;
		//if ( makeNum <= freeNums)
		//	return true;

		//makeNum -= freeNums;
	}

	return nums;
}

static bool hasFreeSlot4Item( const list<ITEM_CACHE*>& lstItemCache, size_t curLimit, CONFIG::ITEM_CFG* itemCfg, size_t makeNum )
{
	size_t curNums = lstItemCache.size();

	//���ɶѵ���Ʒ
	if ( !IS_MERGE_ITEM(itemCfg) )
	{
		if ( curNums + makeNum > curLimit )
			return false;

		return true;
	}

	//�ɶѵ���Ʒ,������������Ϊ���
	//���ٷ���  �������Ѿ��еļ�������Ƿ���
	if ( curNums + ( ( makeNum + itemCfg->count -1 ) / itemCfg->count ) <= curLimit )
		return true;

	size_t mergeNum = canMergeNum( lstItemCache, itemCfg );//�Ѿ��и���Ʒ  ��������λ��
	if ( makeNum <= mergeNum )
		return true;

	makeNum -= mergeNum;
	return ( curNums + ( ( makeNum + itemCfg->count -1 ) / itemCfg->count ) <= curLimit );
}

static int makeIteminner(ROLE* role, list<ITEM_CACHE*>& lstItemCache,size_t curLimit, CONFIG::ITEM_CFG* itemCfg, size_t itemNum, list<ITEM_CACHE*>* pLstItemCache, bool checkAutoSell, BYTE byQuality)//, BYTE byChest1_RandNums7 )
{
	if ( !hasFreeSlot4Item( lstItemCache, curLimit, itemCfg, itemNum ))
		return -1;


	if ( !IS_MERGE_ITEM(itemCfg) )//���ɺϲ���
	{
		for ( size_t i=0; i<itemNum; i++)
		{
			ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, 1, byQuality);

			//�Զ�������ֻ��װ��
			if ( checkAutoSell && role->m_packmgr.isAutoSellItem( itemCache) )
			{
				ADDUP( role->dwCoin, itemCfg->price );
				logs_db_thread::singleton().consume(service::id, role->roleName, role->dwRoleID, itemCfg->price, 0, purpose::auto_sell, 0);

				CC_SAFE_DELETE(itemCache);
			
				continue;
			}

			if ( pLstItemCache )
			{
				pLstItemCache->push_back(itemCache);
			}

			lstItemCache.push_back( itemCache );

			ITEM_INFO para(itemCfg->sub_type,0,(BYTE)byQuality);
			RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_HAS_EQUIP_RARE, &para);
		}

		return 0;
	}

	for ( auto it = lstItemCache.begin(); it!=lstItemCache.end() && itemNum ; ++it )
	{
		ITEM_CACHE* itemCache = *it;
		if ( itemCache->itemCfg != itemCfg )
			continue;

		size_t freeNums = itemCfg->count - itemCache->itemNums;
		if ( freeNums == 0 )
			continue;

		size_t addNum = min<size_t>( itemNum, freeNums);
		itemCache->itemNums += addNum;

		if ( pLstItemCache)
		{
			pLstItemCache->push_back(itemCache);
		}

		itemNum -= addNum;
		//itemCache->dwLingZhuExp = DWORD( ceil( itemCfg->price * itemCfg->mp_max * 0.001f ) );
	}

	while ( itemNum )
	{
		size_t addNum = min<size_t>( itemNum, itemCfg->count );

		ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, addNum, true );

		if ( pLstItemCache)//����delete listnotify ���ð������Ұָ��
		{
			pLstItemCache->push_back(itemCache);
		}

		lstItemCache.push_back( itemCache );
		itemNum -= addNum;
	}

	return 0;
}

int PackMgr::makeItem(CONFIG::ITEM_CFG* itemCfg, size_t itemNum, list<ITEM_CACHE*>* pLstItemCache, bool checkAutoSell, BYTE byQuality)//, BYTE byChest1_RandNums7 )
{
	//ROLE* role, list<ITEM_CACHE*>& lstItemCache, size_t curLimit, 
	//assert(itemCfg);

	ROLE* role = owner;
	//list<ITEM_CACHE*>& lstItemCache = lstItemPkg;
	//size_t curLimit = curItemPkgLimit;

	RETURN_COND( itemCfg == NULL, 0);

	if (itemCfg->type == TYPE_ITEM)//����Ʒ��item������
	{
		byQuality = (BYTE)itemCfg->rare;
		return makeIteminner(owner,lstItemPkg,curItemPkgLimit, itemCfg, itemNum, pLstItemCache, checkAutoSell, byQuality);
	}
	else if (itemCfg->type == TYPE_EQUIP)
	{
		return makeIteminner(owner,lstEquipPkg,curEquipPkgLimit, itemCfg, itemNum, pLstItemCache, checkAutoSell, byQuality);
	}
	else if (itemCfg->type == TYPE_FUWEN)
	{
		byQuality = (BYTE)itemCfg->rare;
		return makeIteminner(owner,lstFuwenPkg,curFuwenPkgLimit, itemCfg, itemNum, pLstItemCache, checkAutoSell, byQuality);
	}

	return 0;
}

bool PackMgr::isAutoSellItem( ITEM_CACHE* itemCache )
{
	ROLE* role = owner;

	CONFIG::ITEM_CFG* itemCfg = itemCache->itemCfg;

	if ( !IS_NORMAL_EQUIP( itemCfg ) )
		return false;

	//װ��
	E_AUTO_SELL_OPTION opt = (E_AUTO_SELL_OPTION) itemCache->byQuality;
	if ( IS_AUTO_SELL_FLAG_SET(role, opt ) )
		return true;

	if ( IS_AUTO_SELL_FLAG_SET(role, E_AUTO_SELL_NOT_SELF_EQUIP ) && 
		( IS_JOB_LIMIT(role, itemCache->itemCfg) || IS_SEX_LIMIT(role, itemCache->itemCfg) ) )
		return true;

	return false;

}

bool PackMgr::makeItem3( ITEM_CACHE* itemCache, list<ITEM_CACHE*>* pLstNotifyItem, list<ITEM_CACHE*>* pLstPropsNotify)
{
	ROLE* role = owner;

	DWORD itemNum = itemCache->itemNums;
	CONFIG::ITEM_CFG * itemCfg = itemCache->itemCfg;
	BYTE byQuality = itemCache->byQuality;

	if (itemCfg->type == TYPE_ITEM)//����Ʒ��item������
	{
		byQuality = (BYTE)itemCfg->rare;

		BYTE ret =  makeIteminner(owner,lstItemPkg,curItemPkgLimit, itemCfg, itemNum, pLstPropsNotify, true, byQuality);
		if (ret == -1)
		{
			return false;
		}
	}
	else if (itemCfg->type == TYPE_EQUIP)//װ��
	{
		BYTE ret = makeIteminner(owner,lstEquipPkg,curEquipPkgLimit, itemCfg, itemNum, pLstNotifyItem, true, byQuality);

		if (ret == -1)
		{
			return false;
		}
	}

	return true;

	//ROLE* role = owner;

	//DWORD itemNums = itemCache->itemNums;
	//CONFIG::ITEM_CFG * itemCfg = itemCache->itemCfg;


	////��Ϊ�������˾��Զ�����
	//if ( isAutoSellItem(itemCache) || !hasFreeSlot4Item( role->m_packmgr.lstEquipPkg, role->m_packmgr.curEquipPkgLimit, itemCfg, itemNums ) )
	//{
	//	//�����п�ʯ
	//	ADDUP( role->dwCoin, itemNums * itemCfg->price );

	//	itemCache->byDropAndSell = 1;
	//	return false;
	//}

	//if ( !IS_MERGE_ITEM(itemCfg) )
	//{
	//	while ( itemNums )
	//	{
	//		ITEM_CACHE* ic = new ITEM_CACHE( itemCache );

	//		if ( IS_EQUIP(itemCfg) )
	//		{
	//			role->m_packmgr.lstEquipPkg.push_back( ic );
	//			if (  pLstNotifyItem )
	//			{
	//				pLstNotifyItem->push_back(ic);
	//			}
	//		}
	//		else 
	//		{
	//			role->m_packmgr.lstItemPkg.push_back( ic );
	//			if ( pLstPropsNotify )
	//			{
	//				pLstPropsNotify->push_back(ic);
	//			}
	//		}
	//		itemNums--;
	//	}

	//	return true;
	//}


	//if ( IS_EQUIP(itemCfg) )
	//{
	//	for ( auto it = role->m_packmgr.lstEquipPkg.begin(); it!=role->m_packmgr.lstEquipPkg.end(); ++it )
	//	{
	//		ITEM_CACHE* ic = *it;
	//		if ( ic->itemCfg != itemCfg )
	//			continue;

	//		size_t freeNums = itemCfg->count - ic->itemNums;
	//		if ( freeNums == 0 )
	//			continue;

	//		size_t addNum = min<size_t>( itemNums, freeNums);//itemNums ��Ȼ���� addNum

	//		ic->itemNums += addNum;

	//		if (  pLstNotifyItem )
	//		{
	//			pLstNotifyItem->push_back(ic);
	//		}

	//		itemNums -= addNum;

	//		BREAK_COND( itemNums == 0);
	//	}
	//}
	//else if ( IS_PROPS(itemCfg) )
	//{
	//	for ( auto it = role->m_packmgr.lstItemPkg.begin(); it!=role->m_packmgr.lstItemPkg.end(); ++it )
	//	{
	//		ITEM_CACHE* ic = *it;
	//		if ( ic->itemCfg != itemCfg )
	//			continue;

	//		size_t freeNums = itemCfg->count - ic->itemNums;
	//		if ( freeNums == 0 )
	//			continue;

	//		size_t addNum = min<size_t>( itemNums, freeNums);

	//		ic->itemNums += addNum;

	//		if ( pLstPropsNotify )
	//		{
	//			pLstPropsNotify->push_back(ic);
	//		}

	//		itemNums -= addNum;

	//		BREAK_COND( itemNums == 0);
	//	}

	//}


	//while ( itemNums )
	//{
	//	size_t addNum = min<size_t>( itemNums, itemCfg->count );

	//	ITEM_CACHE* ic = new ITEM_CACHE( itemCfg, addNum );

	//	if ( IS_EQUIP(itemCfg) )
	//	{
	//		role->m_packmgr.lstEquipPkg.push_back( ic );
	//		if (  pLstNotifyItem )
	//		{
	//			pLstNotifyItem->push_back(ic);
	//		}

	//		itemNums -= addNum;
	//	}
	//	else if ( IS_PROPS(itemCfg) )
	//	{
	//		role->m_packmgr.lstItemPkg.push_back( ic );
	//		if ( pLstPropsNotify )
	//		{
	//			pLstPropsNotify->push_back(ic);
	//		}

	//		itemNums -= addNum;
	//	}
	//}

	//return true;
}


// ��ȡ��������
DWORD PackMgr::getItemCount( WORD itemid)
{
	DWORD total_count = 0;

	for (auto itr=lstItemPkg.begin(); itr!=lstItemPkg.end(); itr++)
	{
		ITEM_CACHE* item = *itr;
		if (NULL ==  item)
		{
			continue;
		}

		if (item->itemCfg->id == itemid)
		{
			total_count += item->itemNums;
		}
	}

	return total_count;
}

// ���������Ƿ�
bool PackMgr::hasEnoughItem(WORD itemid,DWORD count)
{
	return getItemCount(itemid) >= count;
}

// �۳�ָ����������
bool PackMgr::consumeItem(WORD itemid, DWORD count)
{
	return clientSkillBookUse(itemid,(BYTE)count) == 0;
}


//#���������ʹ��
BYTE PackMgr::clientSkillBookUse(const WORD& itemID, const BYTE& itemNum )
{

	BYTE needNum = itemNum;

	if ( ItemCtrl::getPropItemIDNums( owner, itemID ) < needNum )
	{
		return 1;//��������
	}

	if ( needNum > 0)
	{
		needNum = (BYTE)ItemCtrl::rmIDItemNotify( owner, owner->m_packmgr.lstItemPkg, PKG_TYPE_ITEM, itemID, needNum );
	}

	return 0;//�ɹ�
	//vector<ITEM_CACHE*> vecProps;

	//vector<BYTE> vecNums;

	//for (auto it = lstItemPkg.begin(); it != lstItemPkg.end() ; it++)
	//{
	//	if ( (*it)->itemCfg->id != itemID )
	//	{
	//		continue;
	//	}

	//	size_t oneTypeNum = (*it)->itemNums;

	//	size_t getOneTypeNum = 0;

	//	while( oneTypeNum > 0 )
	//	{
	//		oneTypeNum--;

	//		getOneTypeNum++;

	//		haveNum++;

	//		if ( haveNum >= needNum )
	//		{
	//			break;
	//		}
	//	}

	//	vecProps.push_back( *it );

	//	vecNums.push_back( getOneTypeNum );

	//	if ( haveNum >= needNum )
	//	{
	//		byErrCode = 0;//�������� �ɹ�

	//		for ( size_t i = 0; i < vecProps.size(); i++)
	//		{
	//			ItemCtrl::rmItemNotify( owner, lstItemPkg, PKG_TYPE_ITEM, vecProps[i]->itemCfg, vecNums[i] );
	//		}

	//		return byErrCode;
	//	}
	//}

	//return 0;
}

//ɾ��ĳһ����Ʒ
static void rmItem( list<ITEM_CACHE*>& lstItemCache, ITEM_CACHE* itemCache)
{
	for ( auto it = lstItemCache.begin(); it!=lstItemCache.end(); ++it )
	{
		if ( itemCache == *it )
		{
			CC_SAFE_DELETE(itemCache);
			lstItemCache.erase( it );
			return;
		}
	}
}

static int getItemInBodyIndex( ROLE* role, ITEM_CACHE* item)
{
	for ( int i=0; i<BODY_GRID_NUMS; i++)
	{
		ITEM_CACHE* itemCache = role->vBodyEquip[i];
		if ( itemCache == item )
		{
			return i;
		}
	}

	return -1;
}

static int getItemInMerBodyIndex( ROLE* role, ITEM_CACHE* item)
{
	for ( auto& it : role->cMerMgr.m_vecCMercenary )
	{
		for ( int i=0; i<MER_GRID_NUMS; i++)
		{
			ITEM_CACHE* itemCache = it.vBodyEquip[i];
			if ( itemCache == item )
			{
				return i;
			}
		}
	}

	return -1;
}

PKG_TYPE PackMgr::findItem(ITEM_CACHE* item, int* pBodyIndex)
{
	if ( find( lstEquipPkg.begin(), lstEquipPkg.end(), item ) != lstEquipPkg.end() )
		return PKG_TYPE_EQUIP;
	if ( find( lstFuwenPkg.begin(), lstFuwenPkg.end(), item ) != lstFuwenPkg.end() )
		return PKG_TYPE_FUWEN;
	//if ( find( lstMinePkg.begin(), lstMinePkg.end(), item ) != lstMinePkg.end() )
	//	return PKG_TYPE_MINE;
	if ( find( lstItemPkg.begin(), lstItemPkg.end(), item ) != lstItemPkg.end() )
		return PKG_TYPE_ITEM;

	int bodyIndex = getItemInBodyIndex( owner, item );
	if ( bodyIndex != -1 )
	{
		if (pBodyIndex)
			*pBodyIndex = bodyIndex;

		return PKG_TYPE_BODY;
	}

	if( -1 != getItemInMerBodyIndex( owner, item ) )
	{
		return PKG_TYPE_SLAVE;
	}

	return PKG_TYPE_NONE;
}


bool PackMgr::rmItemInPkg( ITEM_CACHE* item)
{
	rmItem( lstEquipPkg, item);
	rmItem( lstMinePkg, item);
	rmItem( lstItemPkg, item);
	rmItem( lstFuwenPkg, item);

	return true;
}