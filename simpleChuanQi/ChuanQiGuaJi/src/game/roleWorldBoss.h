#ifndef __ROLE_WORLDBOSS_H__
#define __ROLE_WORLDBOSS_H__

#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"
#include <set>

class ROLE;

//玩家信息
class RoleWorldBoss
{
public:
	RoleWorldBoss(ROLE* role);
	~RoleWorldBoss();

	// 获得世界BOSS状态
	int clientGetWorldBossStatus(unsigned char * data, size_t dataLen);
	// 报名世界BOSS
	int clientSignUpWorldBoss(unsigned char * data, size_t dataLen);
	// 攻击世界BOSS
	int clientAttackWorldBoss(unsigned char * data, size_t dataLen);
	// 鼓舞世界BOSS
	int clientEmbraveWorldBoss(unsigned char * data, size_t dataLen);
	// 获得伤害排行
	int clientGetWorldBossRank(unsigned char * data, size_t dataLen);
	// 获得世界BOSS冷却剩余时间
	int clientGetWorldBossCoolTime(unsigned char * data, size_t dataLen);

	void cache2Blob(ROLE_BLOB& roleBlob );
	void blob2Cache( const ROLE_BLOB& roleBlob );

	void resetSignup();

	bool _bSignup; // 是否报名

protected:
	ROLE* owner;

	BYTE _byEnhanceTime; //世界boss战鼓舞次数
public:
	unsigned attack_index;

	
};

#endif	//__ROLE_SKILL_H__
