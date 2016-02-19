#ifndef __ROLE_RUNE_H__
#define __ROLE_RUNE_H__

#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"
#include <set>

class ROLE;

//玩家信息
class RoleRune
{
public:
	RoleRune(ROLE* role);
	~RoleRune();

	static const DWORD MAX_RUNE_NUM = 8; // 符文数量

	// 获得符文状态
	int clientGetRuneStatus(unsigned char * data, size_t dataLen);

	void sendRuneStats();

	// 进行一次占卜
	int clientReqDivine(unsigned char * data, size_t dataLen);
	// 符文穿戴
	int clientPutOnRune(unsigned char * data, size_t dataLen);
	// 符文升级
	int clientRuneLevelUp(unsigned char * data, size_t dataLen);

	void cache2Blob(ROLE_BLOB& roleBlob );
	void blob2Cache( const ROLE_BLOB& roleBlob );
	/*
	struct stRuneItem
	{
		BYTE status; // 符文位置是否开放，0未开放，1开放 
		DWORD exp; // 当前经验
		WORD runeid; // 符文id
		BYTE level; // 符文等级
		stRuneItem()
		{
			status = 0;
			exp = 0;
			runeid = 0;
			level = 0;
		};
	};
	*/
	// 获得占卜次数上限
	BYTE getDivineTimesLimit();

	// 还有剩余占卜次数
	bool hasRemainDivineTimes();

	// 自动解锁符文槽
	void autoUnlockDivineGrids();

	//  格子是否开启
	bool isDivineGridOpen(BYTE pos);

	// 判断符文类型是否重复
	bool isDivineRepeated(BYTE pos, WORD sub_type);

	// 符文升级
	void divieLevelUp(stRuneItem& item, DWORD exp);

	// 占卜次数增加1
	void addOneDivineTime();
	// 占卜次数增加5
	void addFiveDivineTime();
	// 重置占卜次数
	void resetDivineTime();

	const std::vector<stRuneItem>& getRuneItems();

	BYTE m_divine_times; // 占卜次数

protected:
	ROLE* owner;

	BYTE m_divine_status; //占卜状态 0阴 1阳 2合

	std::vector<stRuneItem> _vecRuneItems;
};

#endif	//__ROLE_SKILL_H__
