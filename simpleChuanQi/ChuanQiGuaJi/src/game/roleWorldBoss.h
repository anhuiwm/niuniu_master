#ifndef __ROLE_WORLDBOSS_H__
#define __ROLE_WORLDBOSS_H__

#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"
#include <set>

class ROLE;

//�����Ϣ
class RoleWorldBoss
{
public:
	RoleWorldBoss(ROLE* role);
	~RoleWorldBoss();

	// �������BOSS״̬
	int clientGetWorldBossStatus(unsigned char * data, size_t dataLen);
	// ��������BOSS
	int clientSignUpWorldBoss(unsigned char * data, size_t dataLen);
	// ��������BOSS
	int clientAttackWorldBoss(unsigned char * data, size_t dataLen);
	// ��������BOSS
	int clientEmbraveWorldBoss(unsigned char * data, size_t dataLen);
	// ����˺�����
	int clientGetWorldBossRank(unsigned char * data, size_t dataLen);
	// �������BOSS��ȴʣ��ʱ��
	int clientGetWorldBossCoolTime(unsigned char * data, size_t dataLen);

	void cache2Blob(ROLE_BLOB& roleBlob );
	void blob2Cache( const ROLE_BLOB& roleBlob );

	void resetSignup();

	bool _bSignup; // �Ƿ���

protected:
	ROLE* owner;

	BYTE _byEnhanceTime; //����bossս�������
public:
	unsigned attack_index;

	
};

#endif	//__ROLE_SKILL_H__
