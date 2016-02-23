#include "pch.h"
#include "protocal.h"
#include "roleskill.h"
#include "role.h"
#include "itemCtrl.h"
#include "roleCtrl.h"



RoleSkill::RoleSkill(ROLE* role)
	:owner(role)
{
	vecIndexSkills.resize(4);
}

RoleSkill::~RoleSkill()
{

}

//������������
BYTE RoleSkill::isCanUpSkill()
{

	for (DWORD i=0; i<4; i++)
	{
		const stIndexItem& skill  = vecIndexSkills[i];

		COND_CONTINUE( skill.byStatus == 0 || skill.skillid == 0 );

		const WORD wSkillID = skill.skillid;

		CONFIG::SKILL_CFG2* pSkillCfg = CONFIG::getSkillCfg( wSkillID );

		COND_CONTINUE( nullptr == pSkillCfg );

		COND_CONTINUE( !owner->m_packmgr.hasEnoughItem(pSkillCfg->skill_book.itemid,pSkillCfg->skill_book.count) );

		const WORD wNewSkillid = wSkillID + 1;

		COND_CONTINUE( nullptr ==  CONFIG::getSkillCfg( wNewSkillid ) );

		return 1;
	}

	return 0;
}

//�������ü���
BYTE RoleSkill::isCanSetNewSkill()
{
	for (DWORD i=0; i<4; i++)
	{
		stIndexItem& item = vecIndexSkills[i];

		RETURN_COND( item.byStatus == 1 && item.skillid == 0, 1 );
	}

	return 0;
}

// �ͻ��˻�ü��������б�
int RoleSkill::clientGetSkillsIndexs(unsigned char * data, size_t dataLen)
{
	string strData;

	for (DWORD i=0; i<4; i++)
	{
		stIndexItem item = vecIndexSkills[i];
		if(item.byStatus == 0)
		{
			// δ����
			BYTE status = 0;
			S_APPEND_BYTE(strData, status);
		}
		else if(item.byStatus == 1)
		{
			// ����
			if(item.skillid == 0)
			{
				BYTE status = 1;
				S_APPEND_BYTE(strData, status);
			}
			else
			{
				BYTE status = 2;
				S_APPEND_BYTE(strData, status);
				S_APPEND_WORD( strData, item.skillid );
			}
		}
	}

	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_GET_SKILLS_INDEXS, strData) );

	return 0;
}

// ��������
int RoleSkill::clientSkillMoveUp(unsigned char * data, size_t dataLen)
{
	WORD skillid = 0;
	if ( !BASE::parseWORD( data, dataLen, skillid) )
		return -1;

	if(0 == skillid)
		return -1;

	if (!hasIndexSkill(skillid))
	{
		string strData;
		BYTE byError = 2; // ���ܲ�����
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_SKILL_MOVE_UP, strData) );
		return 0;
	}

	for(DWORD i=0; i<4; i++)
	{
		stIndexItem item = vecIndexSkills[i];
		if(skillid == item.skillid)
		{
			if (i != 0)
			{
				WORD tmp = vecIndexSkills[i].skillid;
				vecIndexSkills[i].skillid = vecIndexSkills[i-1].skillid;
				vecIndexSkills[i-1].skillid = tmp;
				break;
			}
		}
	}

	string strData;
	BYTE byError = 0; // ���Ƴɹ�
	S_APPEND_BYTE(strData, byError);
	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_SKILL_MOVE_UP, strData) );

	return 0;
}

// �Ƿ��и���������
bool RoleSkill::hasIndexSkill(WORD skillid)
{
	if(0 == skillid)
		return false;

	for(DWORD i=0; i<4; i++)
	{
		stIndexItem item = vecIndexSkills[i];
		if(skillid == item.skillid)
			return true;
	}

	return false;
}

void  RoleSkill::clearIndexSkills()
{
	for(DWORD i=0; i<4; i++)
	{
		vecIndexSkills[i].skillid = 0;
	}
}

int RoleSkill::clientGetUnlockSkillsList(unsigned char * data, size_t dataLen)
{
	string strData;
	for (auto itr=setSkills.begin(); itr!=setSkills.end(); itr++)
	{
		WORD skillid = *itr;
		S_APPEND_WORD( strData, skillid );
	}
	
	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_GET_UNLOCK_SKILLS_LIST, strData) );

	return 0;
}

int RoleSkill::clientSaveSkillsIndexs(unsigned char * data, size_t dataLen)
{
	std::vector<WORD> vecSkills;

	WORD skillid = 0;
	while (BASE::parseWORD( data, dataLen, skillid))
	{
		if (skillid !=0)
		{
			CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(skillid);
			if (NULL == pSkill)
			{
				// ���ܲ�����
				string strData;
				BYTE byError = 2; 
				S_APPEND_BYTE(strData, byError);
				PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_SAVE_SKILLS_INDEXS, strData) );
				return 0;
			}
		}

		vecSkills.push_back(skillid);
		skillid = 0;
	} 

	if(vecSkills.size() != 4)
		return 0;

	for (DWORD i=0; i<4; i++)
	{
		WORD skillid = vecSkills[i];
		if (skillid != 0)
		{
			if (vecIndexSkills[i].byStatus == 0)
			{
				// δ���������ܲ���������
				string strData;
				BYTE byError = 3; 
				S_APPEND_BYTE(strData, byError);
				PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_SAVE_SKILLS_INDEXS, strData) );
				return 0;
			}
		}
	}

	clearIndexSkills();

	for (DWORD i=0; i<4; i++)
	{
		vecIndexSkills[i].skillid = vecSkills[i];
	}

	// ���漼�������ɹ�
	string strData;
	BYTE byError = 0; 
	S_APPEND_BYTE(strData, byError);
	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_SAVE_SKILLS_INDEXS, strData) );

	if ( byError == 0 )
	{
		judgeSetSkillTask();
	}
	return 0;
}

void RoleSkill::judgeSetSkillTask()
{
	BYTE bySetSkillNum = 0;

	for (DWORD i=0; i<4; i++)
	{
		if (vecIndexSkills[i].byStatus != 0 && vecIndexSkills[i].skillid != 0 )
		{
			bySetSkillNum += 1;
		}
	}

	ITEM_INFO para(0, bySetSkillNum );
	RoleMgr::getMe().judgeCompleteTypeTask(owner, E_TASK_SET_FIGHT_SKILL_NUM, &para);
}
bool RoleSkill::isSkillUnlock(WORD skillid)
{
	return setSkills.find(skillid) != setSkills.end();
}

// �����Ƿ����
bool RoleSkill::isExistSkill(WORD skillid)
{
	CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(skillid);
	if (NULL == pSkill)
		return false;

	return true;
}

// �ӿ��������б���ɾ������
void RoleSkill::removeSkill(WORD skillid)
{
	setSkills.erase(skillid);
}

// �����������б�����Ӽ���
void RoleSkill::addSkill(WORD skillid)
{
	setSkills.insert(skillid);
}

// ���¼����������еļ���
void RoleSkill::replaceIndexSkill(WORD skillid, WORD newskillid)
{
	for (DWORD i=0; i<4; i++)
	{
		if (vecIndexSkills[i].skillid == skillid)
		{
			vecIndexSkills[i].skillid = newskillid;
		}
	}
}

int RoleSkill::clientUpSkillLevel(unsigned char * data, size_t dataLen)
{
	WORD skillid = 0;
	if ( !BASE::parseWORD( data, dataLen, skillid) )
		return 0;

	CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(skillid);
	if (NULL == pSkill)
	{
		// ���ܲ�����
		string strData;
		BYTE byError = 2; 
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_UP_SKILL_LEVEL, strData) );
		return 0;
	}

	// ����δ����
	if (!isSkillUnlock(skillid))
	{
		string strData;
		BYTE byError = 3; 
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_UP_SKILL_LEVEL, strData) );
		return 0;
	}

	// û���㹻�ļ�����
	if (!owner->m_packmgr.hasEnoughItem(pSkill->skill_book.itemid,pSkill->skill_book.count))
	{
		string strData;
		BYTE byError = 3; 
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_UP_SKILL_LEVEL, strData) );
		return 0;
	}

	WORD newskillid = skillid + 1;

	CONFIG::SKILL_CFG2* pNewSkill = CONFIG::getSkillCfg(newskillid);
	if (NULL == pNewSkill)
	{
		// �¼��ܲ����ڣ�˵���Ѿ���������ߵȼ�
		string strData;
		BYTE byError = 1;  // ʧ��
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_UP_SKILL_LEVEL, strData) );
		return 0;
	}

	// δ�ﵽ��������ȼ�
	if(owner->wLevel < pNewSkill->unlock_level)
	{
		string strData;
		BYTE byError = 5;  // ʧ��
		S_APPEND_BYTE(strData, byError);
		PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_UP_SKILL_LEVEL, strData) );
		return 0;
	}

	owner->m_packmgr.consumeItem(pSkill->skill_book.itemid,pSkill->skill_book.count);

	removeSkill(skillid);
	addSkill(newskillid);
	replaceIndexSkill(skillid,newskillid);

	string strData;
	BYTE byError = 0;  // ���������ɹ�
	S_APPEND_BYTE(strData, byError);
	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_UP_SKILL_LEVEL, strData) );

	return 0;
}

USE_EXTERN_CONFIG;

void RoleSkill::autoUnlockSkills()
{
	for (auto &it : mapSkillCfg)
	{
		auto skill = it.second;
		if ( (skill.study == 0) && (owner->wLevel >= skill.unlock_level)  &&  (owner->byJob == skill.job) )//ְҵ�͵ȼ�����
		{
			if (skill.level == 1 /*|| skill.level == 0*/)
			{
				if (!isSkillGroupUnlock(getSkillGroup(skill.id)))
				{
					addSkill(skill.id);
				}
			}
		}
	}

	// ���¿ͻ��˿��������б�
	string strData;
	for (auto itr=setSkills.begin(); itr!=setSkills.end(); itr++)
	{
		WORD skillid = *itr;
		S_APPEND_WORD( strData, skillid );
	}
	PROTOCAL::sendClient( owner->client, PROTOCAL::cmdPacket(S_GET_UNLOCK_SKILLS_LIST, strData) );

	autoUnlockSkillIndexs();
}

// �Զ��������ܲ�
void RoleSkill::autoUnlockSkillIndexs()
{
	for (DWORD i=0; i<4; i++)
	{
		vecIndexSkills[i].byStatus = 0;
		if ( (i==0) && (owner->wLevel>=3))
		{
			vecIndexSkills[i].byStatus = 1;
		}
		else if ( (i==1) && (owner->wLevel>=15))
		{
			vecIndexSkills[i].byStatus = 1;
		}
		else if ( (i==2) && (owner->wLevel>=25))
		{
			vecIndexSkills[i].byStatus = 1;
		}
		else if ( (i==3) && (owner->wLevel>=35))
		{
			vecIndexSkills[i].byStatus = 1;
		}
	}
}

WORD RoleSkill::getSkillGroup(WORD skillid)
{
	CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(skillid);
	if ( pSkill == NULL)
	{
		return 0;
	}
	return pSkill->group;
}

// �Ƿ�����ͬһ������
bool RoleSkill::isSameGroup(WORD skillid1, WORD skillid2)
{
	return getSkillGroup(skillid1) == getSkillGroup(skillid2);
}

// �������Ƿ��Ѿ�����
bool RoleSkill::isSkillGroupUnlock(WORD skillgroup)
{
	for (auto itr=setSkills.begin(); itr!=setSkills.end(); itr++)
	{
		WORD skillid = *itr;
		if (skillgroup == getSkillGroup(skillid))
		{
			return true;
		}
	}

	return false;
}


