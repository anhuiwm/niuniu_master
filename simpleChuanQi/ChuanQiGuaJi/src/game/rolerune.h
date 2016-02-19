#ifndef __ROLE_RUNE_H__
#define __ROLE_RUNE_H__

#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"
#include <set>

class ROLE;

//�����Ϣ
class RoleRune
{
public:
	RoleRune(ROLE* role);
	~RoleRune();

	static const DWORD MAX_RUNE_NUM = 8; // ��������

	// ��÷���״̬
	int clientGetRuneStatus(unsigned char * data, size_t dataLen);

	void sendRuneStats();

	// ����һ��ռ��
	int clientReqDivine(unsigned char * data, size_t dataLen);
	// ���Ĵ���
	int clientPutOnRune(unsigned char * data, size_t dataLen);
	// ��������
	int clientRuneLevelUp(unsigned char * data, size_t dataLen);

	void cache2Blob(ROLE_BLOB& roleBlob );
	void blob2Cache( const ROLE_BLOB& roleBlob );
	/*
	struct stRuneItem
	{
		BYTE status; // ����λ���Ƿ񿪷ţ�0δ���ţ�1���� 
		DWORD exp; // ��ǰ����
		WORD runeid; // ����id
		BYTE level; // ���ĵȼ�
		stRuneItem()
		{
			status = 0;
			exp = 0;
			runeid = 0;
			level = 0;
		};
	};
	*/
	// ���ռ����������
	BYTE getDivineTimesLimit();

	// ����ʣ��ռ������
	bool hasRemainDivineTimes();

	// �Զ��������Ĳ�
	void autoUnlockDivineGrids();

	//  �����Ƿ���
	bool isDivineGridOpen(BYTE pos);

	// �жϷ��������Ƿ��ظ�
	bool isDivineRepeated(BYTE pos, WORD sub_type);

	// ��������
	void divieLevelUp(stRuneItem& item, DWORD exp);

	// ռ����������1
	void addOneDivineTime();
	// ռ����������5
	void addFiveDivineTime();
	// ����ռ������
	void resetDivineTime();

	const std::vector<stRuneItem>& getRuneItems();

	BYTE m_divine_times; // ռ������

protected:
	ROLE* owner;

	BYTE m_divine_status; //ռ��״̬ 0�� 1�� 2��

	std::vector<stRuneItem> _vecRuneItems;
};

#endif	//__ROLE_SKILL_H__
