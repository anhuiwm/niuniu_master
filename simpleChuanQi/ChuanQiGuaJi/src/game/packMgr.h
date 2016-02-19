#ifndef __PACK_MGR_H__
#define __PACK_MGR_H__

#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"
#include <set>

class ROLE;

//using namespace std;

//�����Ϣ
class PackMgr
{
public:
	PackMgr(ROLE* role);
	~PackMgr();

	list<ITEM_CACHE*> lstEquipPkg;//װ������  �Ժ������
	list<ITEM_CACHE*> lstMinePkg;
	list<ITEM_CACHE*> lstItemPkg;//����
	list<ITEM_CACHE*> lstFuwenPkg; //  ����

	BYTE curEquipPkgLimit;		//��ǰ����װ������������
	BYTE curMineLimit;			//��ǰ���Ŀ��������
	BYTE curItemPkgLimit;		//��ǰ���ĵ��߱���������
	BYTE curFuwenPkgLimit;

	void cache2Blob(ROLE_BLOB& roleBlob );
	void blob2Cache( const ROLE_BLOB& roleBlob );

	void patch();
	void giveItems();

	int makeItem( CONFIG::ITEM_CFG* itemCfg, size_t itemNum, list<ITEM_CACHE*>* pLstItemCache=NULL, bool checkAutoSell =false, BYTE byQuality = 1);//, BYTE byChest1_RandNums7=0 );
	bool makeItem3( ITEM_CACHE* itemCache, list<ITEM_CACHE*>* pLstNotifyItem, list<ITEM_CACHE*>* pLstPropsNotify);

	bool isAutoSellItem( ITEM_CACHE* itemCache );

	//#���������ʹ��
	BYTE clientSkillBookUse( const WORD& itemID, const BYTE& itemNum );
	// ��ȡ��������
	DWORD getItemCount( WORD itemid);
	// �۳�ָ����������
	bool consumeItem( WORD itemid, DWORD count);
	// ���������Ƿ�
	bool hasEnoughItem( WORD itemid,DWORD count);
	bool rmItemInPkg( ITEM_CACHE* item);

	PKG_TYPE findItem(ITEM_CACHE* item, int* pBodyIndex=NULL );
protected:
	ROLE* owner;
};

#endif	//__ROLE_SKILL_H__
