#pragma once
#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"



class ROLE;

//using namespace std;

//�����Ϣ
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

	std::set<WORD> setSkills;//����ʹ��

	struct stIndexItem
	{
		BYTE byStatus; // ״̬ 0δ���� 1����
		WORD skillid; // ����id
		stIndexItem()
		{
			byStatus = 0;
			skillid = 0;
		}
	};

	std::vector<stIndexItem> vecIndexSkills; // ��������

	// �Ƿ��и���������
	bool hasIndexSkill(WORD skillid);
	void clearIndexSkills();

	bool isSkillUnlock(WORD skillid);
	// �����Ƿ����
	bool isExistSkill(WORD skillid);
	// �ӿ��������б���ɾ������
	void removeSkill(WORD skillid);
	// �����������б�����Ӽ���
	void addSkill(WORD skillid);
	// ���¼����������еļ���
	void replaceIndexSkill(WORD skillid, WORD newskillid);
	// ��ü�����id
	WORD getSkillGroup(WORD skillid);
	// �Ƿ�����ͬһ������
	bool isSameGroup(WORD skillid1, WORD skillid2);
	// �������Ƿ��Ѿ�����
	bool isSkillGroupUnlock(WORD skillgroup);

	// �Զ���������
	void autoUnlockSkills();
	// �Զ��������ܲ�
	void autoUnlockSkillIndexs();

	BYTE isCanSetNewSkill();

	BYTE isCanUpSkill();

	void judgeSetSkillTask();
protected:
	ROLE* owner;
};
