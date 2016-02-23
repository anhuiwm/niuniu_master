#pragma once
#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"



class ROLE;

//using namespace std;

//玩家信息
class RoleSkill
{
public:
	RoleSkill(ROLE* role);
	~RoleSkill();

	int clientGetSkillsIndexs(unsigned char * data, size_t dataLen);
	int clientSkillMoveUp(unsigned char * data, size_t dataLen);
	int clientGetUnlockSkillsList(unsigned char * data, size_t dataLen);
	int clientSaveSkillsIndexs(unsigned char * data, size_t dataLen);
	int clientUpSkillLevel(unsigned char * data, size_t dataLen);

	std::set<WORD> setSkills;//可以使用

	struct stIndexItem
	{
		BYTE byStatus; // 状态 0未解锁 1解锁
		WORD skillid; // 技能id
		stIndexItem()
		{
			byStatus = 0;
			skillid = 0;
		}
	};

	std::vector<stIndexItem> vecIndexSkills; // 技能索引

	// 是否有该索引技能
	bool hasIndexSkill(WORD skillid);
	void clearIndexSkills();

	bool isSkillUnlock(WORD skillid);
	// 技能是否存在
	bool isExistSkill(WORD skillid);
	// 从开启技能列表中删除技能
	void removeSkill(WORD skillid);
	// 忘开启技能列表中添加技能
	void addSkill(WORD skillid);
	// 更新技能索引表中的技能
	void replaceIndexSkill(WORD skillid, WORD newskillid);
	// 获得技能组id
	WORD getSkillGroup(WORD skillid);
	// 是否属于同一技能组
	bool isSameGroup(WORD skillid1, WORD skillid2);
	// 技能组是否已经开启
	bool isSkillGroupUnlock(WORD skillgroup);

	// 自动解锁技能
	void autoUnlockSkills();
	// 自动解锁技能槽
	void autoUnlockSkillIndexs();

	BYTE isCanSetNewSkill();

	BYTE isCanUpSkill();

	void judgeSetSkillTask();
protected:
	ROLE* owner;
};
