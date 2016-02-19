
#include <algorithm>

#include "protocal.h"
#include "roleWorldBoss.h"
#include "role.h"
#include "itemCtrl.h"
#include "WorldBoss.h"

RoleWorldBoss::RoleWorldBoss(ROLE* role)
	:owner(role)
{
	_byEnhanceTime = 0;
	_bSignup = false;
	attack_index = 0;
}

RoleWorldBoss::~RoleWorldBoss()
{

}

// 获得世界BOSS状态
int RoleWorldBoss::clientGetWorldBossStatus(unsigned char * data, size_t dataLen)
{
	return WorldBoss::getMe().clientGetWorldBossStatus(owner);
}

// 报名世界BOSS
int RoleWorldBoss::clientSignUpWorldBoss(unsigned char * data, size_t dataLen)
{
	_bSignup = true;

	return WorldBoss::getMe().clientSignUpWorldBoss(owner);
}

// 攻击世界BOSS
int RoleWorldBoss::clientAttackWorldBoss(unsigned char * data, size_t dataLen)
{
	return WorldBoss::getMe().clientAttackWorldBoss(owner);
}

// 鼓舞世界BOSS
int RoleWorldBoss::clientEmbraveWorldBoss(unsigned char * data, size_t dataLen)
{
	return WorldBoss::getMe().clientEmbraveWorldBoss(owner);
}

// 获得伤害排行
int RoleWorldBoss::clientGetWorldBossRank(unsigned char * data, size_t dataLen)
{
	return WorldBoss::getMe().clientGetWorldBossRank(owner);
}

// 获得世界BOSS冷却剩余时间
int RoleWorldBoss::clientGetWorldBossCoolTime(unsigned char * data, size_t dataLen)
{
	return WorldBoss::getMe().clientGetWorldBossCoolTime(owner);
}

void RoleWorldBoss::cache2Blob(ROLE_BLOB& roleBlob )
{
	roleBlob.byEnhanceTime = _byEnhanceTime;
}

void RoleWorldBoss::blob2Cache( const ROLE_BLOB& roleBlob )
{
	_byEnhanceTime = roleBlob.byEnhanceTime;
}

void RoleWorldBoss::resetSignup()
{
	_bSignup = false;
}