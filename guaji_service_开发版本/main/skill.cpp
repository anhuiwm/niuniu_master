#include "pch.h"
#include "skill.h"
#include "itemCtrl.h"
#include "role.h"


int SKILL::clientGetSkillsIndexs(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientGetSkillsIndexs(data,dataLen);
}

int SKILL::clientSkillMoveUp(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientSkillMoveUp(data,dataLen);
}

int SKILL::clientGetUnlockSkillsList(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientGetUnlockSkillsList(data,dataLen);
}

int SKILL::clientSaveSkillsIndexs(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientSaveSkillsIndexs(data,dataLen);
}

int SKILL::clientUpSkillLevel(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientUpSkillLevel(data,dataLen);
}