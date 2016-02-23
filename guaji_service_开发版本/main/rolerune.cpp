#include "pch.h"
#include "protocal.h"
#include "rolerune.h"
#include "role.h"
#include "itemCtrl.h"
#include "roleCtrl.h"



RoleRune::RoleRune(ROLE* role)
	:owner(role)
{
	m_divine_status = 0;
	m_divine_times = 0;
	_vecRuneItems.resize(MAX_RUNE_NUM);
}

RoleRune::~RoleRune()
{

}

static DWORD getDivineCoins(DWORD times)
{
	//1-5����Ҫ���5000
	//6-10����Ҫ���10000
	//11-15����Ҫ���50000
	//16-20����Ҫ���100000
	//21-25����Ҫ���200000
	//26-30����Ҫ���300000
	//31��������Ҫ���400000

	if (times <= 5)
	{
		return 5000;
	}

	if (times <= 10)
	{
		return 10000;
	}

	if (times <= 15)
	{
		return 50000;
	}

	if (times <= 20)
	{
		return 100000;
	}

	if (times <= 25)
	{
		return 200000;
	}

	if (times <= 30)
	{
		return 300000;
	}

	return 400000;
}


// ��÷���״̬
int RoleRune::clientGetRuneStatus(unsigned char * data, size_t dataLen)
{
	sendRuneStats();
	return 0;
}

void RoleRune::sendRuneStats()
{
	string strData;

	S_APPEND_BYTE(strData, m_divine_status);
	S_APPEND_BYTE(strData, m_divine_times);
	S_APPEND_BYTE(strData, getDivineTimesLimit());
	S_APPEND_DWORD(strData, getDivineCoins(m_divine_times));
	for (DWORD i=0; i<MAX_RUNE_NUM; i++)
	{
		const stRuneItem& item = _vecRuneItems[i];
		S_APPEND_BYTE(strData, item.status);
		S_APPEND_DWORD(strData, item.exp);
		S_APPEND_WORD(strData, item.runeid);
		S_APPEND_BYTE(strData, item.level);
	}

	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_GET_FUWEN_STATUS, strData) );
}

static BYTE randDivineStatus(BYTE status)
{
	//״ֵ̬=����30%������������70%����������
	//״ֵ̬=����40%������������30%������������30%����Ϊ��
	//״ֵ̬=�ϣ�90%������������10%����������
	//0�� 1�� 2��
	if (0 == status)
	{
		if (BASE::randHit(30,100))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (1 == status)
	{
		int randResult = BASE::randBetween(0,100);
		if (randResult < 40)
		{
			return 0;
		}
		else if (randResult < 70)
		{
			return 1;
		}
		else 
		{
			return 2;
		}
	}
	else if (2 == status)
	{
		if (BASE::randHit(90,100))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	return 0;
}

static WORD randomOneDivineOrPatch(BYTE status)
{
	//��״ֵ̬=����80%���ʻ��Ʒ��=3�ķ��ģ�15%���Ʒ��=4�ķ��ģ�5%���ʻ�÷�����Ƭ
    //��״ֵ̬=����60%���ʻ��Ʒ��=3�ķ��ģ�30%���Ʒ��=4�ķ��ģ�10%���ʻ�÷�����Ƭ
	//��״ֵ̬=�ϣ�5%���ʻ��Ʒ��=3�ķ��ģ�60%���Ʒ��=4�ķ��ģ�30%���ʻ�÷�����Ƭ
	if (0 == status)
	{
		int nRand = BASE::randBetween(0,100);
		if (nRand < 80)
		{
			return CONFIG::randOneRune(3);
		}
		else if (nRand < 95)
		{
			return CONFIG::randOneRune(4);
		}
	}
	else if (1 == status)
	{
		int nRand = BASE::randBetween(0,100);
		if (nRand < 60)
		{
			return CONFIG::randOneRune(3);
		}
		else if (nRand < 90)
		{
			return CONFIG::randOneRune(4);
		}
	}
	else if (2 == status)
	{
		int nRand = BASE::randBetween(0,100);
		if (nRand < 5)
		{
			return CONFIG::randOneRune(3);
		}
		else if (nRand < 65)
		{
			return CONFIG::randOneRune(4);
		}
	}

	return 0;
}

// ����һ��ռ��
int RoleRune::clientReqDivine(unsigned char * data, size_t dataLen)
{
	if (!hasRemainDivineTimes())
	{
		// ռ����������
		string strData;
		BYTE byError = 4; 
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_REQ_DIVINE, strData) );
		return 0;
	}

	DWORD need_coin = getDivineCoins(m_divine_times+1);
	if (!owner->hasEnoughCoin(need_coin))
	{
		// ��Ҳ���
		string strData;
		BYTE byError = 3; 
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_REQ_DIVINE, strData) );
		return 0;
	}

	m_divine_status = randDivineStatus(m_divine_status); // �����µ�ռ��״ֵ̬
	WORD itemid = randomOneDivineOrPatch(m_divine_status);
	m_divine_times++; // ռ����������

	if (itemid == 0)
	{
		// ���ɷ�����Ƭ
		const WORD DIVINE_PATCH_ID = 404;
		std::list<ITEM_CACHE*> lstItemPkg;
		if ( E_SUCC != owner->m_packmgr.makeItem( CONFIG::getItemCfg(DIVINE_PATCH_ID), 1, &lstItemPkg) )
		{
			string strData;
			BYTE byError = 2;  // ��������
			S_APPEND_BYTE(strData, byError);
			PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_REQ_DIVINE, strData) );
			return 0;
		}
		ItemCtrl::notifyClientItem(owner, lstItemPkg, PKG_TYPE_ITEM);

		string strData;
		BYTE byError = 0; 
		DWORD uuid = 0;
		DWORD count = 1;
		S_APPEND_BYTE(strData, byError);
		S_APPEND_BYTE(strData, m_divine_status);
		S_APPEND_DWORD(strData,uuid);
		S_APPEND_DWORD(strData,count);
		S_APPEND_DWORD(strData, getDivineCoins(m_divine_times));
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_REQ_DIVINE, strData) );
		return 0;
	}
	
	// ���ɷ���
	std::list<ITEM_CACHE*> lstItemsPkg;
	if ( E_SUCC != owner->m_packmgr.makeItem( CONFIG::getItemCfg(itemid), 1, &lstItemsPkg) )
	{
		string strData;
		BYTE byError = 2;  // ��������
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_REQ_DIVINE, strData) );
		return 0;
	}
	for ( auto pIt : lstItemsPkg )
	{
		pIt->dwFightValue = 0;
	}
	ItemCtrl::notifyClientItem(owner, lstItemsPkg, PKG_TYPE_FUWEN);
	DWORD index = (DWORD)(*lstItemsPkg.begin());
	DWORD count = 0;
	string strData;
	BYTE byError = 0; 
	S_APPEND_BYTE(strData, byError);
	S_APPEND_BYTE(strData, m_divine_status);
	S_APPEND_DWORD(strData,index);
	S_APPEND_DWORD(strData,count);
	S_APPEND_DWORD(strData, getDivineCoins(m_divine_times));
	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_REQ_DIVINE, strData) );

	if (byError == 0)
	{
		ADDUP( owner->dwCoin, -(int)need_coin );
		notifyCoin( owner );
		RoleMgr::getMe().judgeCompleteTypeTask( owner, E_TASK_RUNE_TIMES );
	}
	return 0;
}

// ���Ĵ���
int RoleRune::clientPutOnRune(unsigned char * data, size_t dataLen)
{
	BYTE pos = 0;
	DWORD itemuuid = 0;

	if (!BASE::parseBYTE(data,dataLen, pos))
		return -1;

	if ( !BASE::parseDWORD( data, dataLen, itemuuid) )
		return -1;

	if (pos >= MAX_RUNE_NUM)
		return - 1;

	ITEM_CACHE* pItem =(ITEM_CACHE* )itemuuid;
	PKG_TYPE pk_type = owner->m_packmgr.findItem(pItem);

	if (pItem==NULL || pItem->itemCfg==NULL || PKG_TYPE_NONE == pk_type || !IS_DIVINE( pItem->itemCfg))
	{
		return 0; // ���Ĳ�����
	}

	if (!isDivineGridOpen(pos))
	{
		// ���ĸ���δ����
		string strData;
		BYTE byError = 3; 
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_PUT_ON_FUWEN, strData) );
		return 0;
	}

	if (isDivineRepeated(pos,pItem->itemCfg->sub_type))
	{
		// ���������ظ�
		string strData;
		BYTE byError = 2; 
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_PUT_ON_FUWEN, strData) );
		return 0;
	}

	stRuneItem old_item = _vecRuneItems[pos]; // ��¼һ���ϵķ���
	// ���µķ��ķŵ�������
	stRuneItem& new_item = _vecRuneItems[pos];
	new_item.runeid = pItem->itemCfg->id;
	new_item.level = pItem->byStrengthLv > 1 ? pItem->byStrengthLv : 1;
	new_item.exp = pItem->dwFightValue;
	// ���·��Ĵӱ�����ɾ��
	ItemCtrl::subItemNotify(owner,owner->m_packmgr.lstFuwenPkg,PKG_TYPE_FUWEN,pItem,1);

	// �����Ϸ���
	if (old_item.runeid != 0)
	{
		std::list<ITEM_CACHE*> lstItemPkg;
		if ( E_SUCC != owner->m_packmgr.makeItem( CONFIG::getItemCfg(old_item.runeid), 1, &lstItemPkg) )
		{
			BYTE error = 1; 
			string strData;
			S_APPEND_BYTE(strData, error);
			PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_PUT_ON_FUWEN, strData) );

			ItemCtrl::updateNotifyRoleAttr( owner );
			return 0;
		}

		if (!lstItemPkg.empty())
		{
			ITEM_CACHE* pNew = *lstItemPkg.begin();
			pNew->byStrengthLv = old_item.level;
			pNew->dwFightValue = old_item.exp;
		}

		ItemCtrl::notifyClientItem(owner, lstItemPkg, PKG_TYPE_FUWEN);
	}

	// �������ĳɹ�
	BYTE error = 0; 
	string strData;
	S_APPEND_BYTE(strData, error);
	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_PUT_ON_FUWEN, strData) );

	ItemCtrl::updateNotifyRoleAttr( owner );
	return 0;
}

static DWORD getTotalDivineExp( const std::set<ITEM_CACHE*>& setEquips)
{
	DWORD result = 0;
	for (auto itr=setEquips.begin(); itr!=setEquips.end(); itr++)
	{
		ITEM_CACHE* item = *itr;
		if (NULL == item)
		{
			continue;
		}

		result += item->getDivineExp();
	}

	return result;
}

// ��������
int RoleRune::clientRuneLevelUp(unsigned char * data, size_t dataLen)
{
	BYTE pos = 0;
	if (!BASE::parseBYTE(data,dataLen, pos))
		return -1;

	std::set< ITEM_CACHE* > setEquips;
	while(1) 
	{
		ITEM_CACHE* item;
		if ( !BASE::parseDWORD( data, dataLen, (*(DWORD*)&item)) )
			break;

		PKG_TYPE weaponPos = owner->m_packmgr.findItem(item);

		if (item==NULL || item->itemCfg==NULL || PKG_TYPE_NONE == weaponPos || !IS_DIVINE( item->itemCfg))
		{
			return 0; 
		}

		setEquips.insert(item);
	}

	if ( setEquips.empty() )
		return 0;

	if (pos >= MAX_RUNE_NUM)
		return - 1;

	if (!isDivineGridOpen(pos))
	{
		return 0;
	}

	stRuneItem& rune_item = _vecRuneItems[pos];
	if (rune_item.runeid == 0)
	{
		return 0;
	}

	if (!CONFIG::isExistRuneCfg(rune_item.runeid, rune_item.level+1))
	{
		return 0;
	}

	DWORD output_exp = getTotalDivineExp(setEquips);
	{
		// ɾ��װ��
		for (auto itr=setEquips.begin(); itr!=setEquips.end(); itr++)
		{
			ITEM_CACHE* item = *itr;
			if (NULL != item)
			{
				ItemCtrl::subItemNotify(owner,owner->m_packmgr.lstFuwenPkg,PKG_TYPE_FUWEN,item,1);
			}
		}
	}

	divieLevelUp(rune_item,output_exp);

	// �������ĳɹ�
	BYTE error = 0; 
	string strData;
	S_APPEND_BYTE(strData, error);
	S_APPEND_BYTE(strData, rune_item.level);
	S_APPEND_DWORD(strData, rune_item.exp);
	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_FUWEN_LEVELUP, strData) );

	//sendRuneStats();
	ItemCtrl::updateNotifyRoleAttr( owner );

	return 0;
}

// ��������
void RoleRune::divieLevelUp(stRuneItem& item, DWORD exp)
{
	item.exp += exp;
	while (true)
	{
		CONFIG::RUNE_CFG_ITEM* itemCfg = CONFIG::getRuneCfg(item.runeid, item.level);
		if (itemCfg == NULL)
			break;

		if (item.exp < itemCfg->need_exp)
			break;

		if (!CONFIG::isExistRuneCfg(item.runeid, item.level+1))
		{
			item.exp = 0;
			break;
		}

		item.level++;
		item.exp -= itemCfg->need_exp;
	}
}

void RoleRune::cache2Blob(ROLE_BLOB& roleBlob )
{
	roleBlob.divine_times = m_divine_times;
	roleBlob.divine_status = m_divine_status;
	for (DWORD i=0; i<MAX_RUNE_NUM; i++)
	{
		roleBlob.rune_items[i] = _vecRuneItems[i];
	}
}

void RoleRune::blob2Cache( const ROLE_BLOB& roleBlob )
{
	m_divine_times = roleBlob.divine_times;
	m_divine_status = roleBlob.divine_status;
	for (DWORD i=0; i<MAX_RUNE_NUM; i++)
	{
		_vecRuneItems[i] = roleBlob.rune_items[i];
	}
}

// ռ����������1
void RoleRune::addOneDivineTime()
{
	if (m_divine_times > 1)
	{
		m_divine_times--;
	}

	sendRuneStats();
}

// ռ����������5
void RoleRune::addFiveDivineTime()
{
	if (m_divine_times > 5)
	{
		m_divine_times -= 5;
	}
	else
	{
		m_divine_times = 0;
	}

	sendRuneStats();
}

// ����ռ������
void RoleRune::resetDivineTime()
{
	m_divine_times = 0;

	sendRuneStats();
}

// ���ռ����������
BYTE RoleRune::getDivineTimesLimit()
{
	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(owner->getVipLevel());
	if (NULL == vipCfg)
	{
		return 10;
	}

	return vipCfg->divine_time;
}

// ����ʣ��ռ������
bool RoleRune::hasRemainDivineTimes()
{
	return m_divine_times < getDivineTimesLimit();
}

//  �����Ƿ���
bool RoleRune::isDivineGridOpen(BYTE pos)
{
	if (pos > MAX_RUNE_NUM)
	{
		return false;
	}

	if (_vecRuneItems[pos].status == 1)
	{
		return true;
	}

	return false;
}

// �жϷ��������Ƿ��ظ�
bool RoleRune::isDivineRepeated(BYTE pos, WORD sub_type)
{
	std::set<WORD> setTypes;
	for (DWORD i=0; i<MAX_RUNE_NUM; i++)
	{
		if (i == pos)
		{
			continue;
		}

		const stRuneItem& item = _vecRuneItems[i];
		if (item.status == 0)
		{
			continue;
		}

		if (item.runeid == 0)
		{
			continue;
		}

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(item.runeid);
		if ( itemCfg == NULL )
			return true;

		setTypes.insert(itemCfg->sub_type);
	}

	return setTypes.find(sub_type) != setTypes.end();
}

// �Զ��������Ĳ�
void RoleRune::autoUnlockDivineGrids()
{
	for (DWORD i=0; i<MAX_RUNE_NUM; i++)
	{
		_vecRuneItems[i].status = 0;
	}

	//��1��2��	�ȼ�40����������ϵͳʱ����
	//��3��	�ȼ�45������
	//��4��	�ȼ�50������
	//��5��	VIP�ȼ�1����
	//��6��	vIP�ȼ�2����
	//��7��	VIP�ȼ�4����
	//��8��	VIP�ȼ�6����
	if (owner->wLevel >= 40)
	{
		_vecRuneItems[0].status = 1;
		_vecRuneItems[1].status = 1;
	}
	if (owner->wLevel >= 45)
	{
		_vecRuneItems[2].status = 1;
	}

	if (owner->wLevel >= 50)
	{
		_vecRuneItems[3].status = 1;
	}

	if (owner->getVipLevel() >= 1)
	{
		_vecRuneItems[4].status = 1;
	}

	if (owner->getVipLevel() >= 2)
	{
		_vecRuneItems[5].status = 1;
	}

	if (owner->getVipLevel() >= 4)
	{
		_vecRuneItems[6].status = 1;
	}

	if (owner->getVipLevel() >= 6)
	{
		_vecRuneItems[7].status = 1;
	}
}

const std::vector<stRuneItem>& RoleRune::getRuneItems()
{
	return _vecRuneItems;
}
