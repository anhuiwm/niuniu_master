#ifndef __COMMON_H__
#define __COMMON_H__

#include "typedef.h"
#include "config.h"

class CMercenary;

#pragma pack(push, 1)


//������ɫ

struct PlayerInfo
{
	DWORD _playerId; //��ɫID
	DWORD _coin;//���, 
	DWORD _ingot;//Ԫ��, 

	DWORD _curExp;// ��ǰ����, 
	DWORD _maxExp;// �����, 

	BYTE _job; //��ɫְҵ
	BYTE _sex; //��ɫ�Ա�
	BYTE _level;// ��ɫ�ȼ�, 
	BYTE _vip;//��ɫVIP�ȼ�, 

	char _name[22]; //��ɫ����

	BYTE _curMap; //��ǰ������ͼ����
	BYTE _maxMap; //����ͨ�ص�ͼID, 

	BYTE _suite; //��ǰ������������ 
	BYTE _lucky; //����ֵ;, 
	BYTE _luckyDef;//���˷���, _power; //����(����), 
	BYTE _titleId; //��ǰʹ�õĳƺ�Id, 

	WORD _stamina; //����, 
	WORD _strength; //����, 
	WORD _agility; //����, 
	WORD _intellect; //�ǻ�, 
	WORD _spirit; //����,  

	BYTE _curEquipAward; //��ǰװ��������ȡ����
	BYTE _rechargeState; //�׳佱��״��.0:δ��ֵ;1:�ѳ�ֵδ��ȡ;2:����ȡ

	DWORD _warPower; //ս��ֵ, 
	DWORD _maxHP;// ���Ѫ��, 
	DWORD _maxMP;// ���ħ��, 

	DWORD _atkMax;	//����������, 
	DWORD _atkMin;	//����������, 

	DWORD _critVal;	//( ����ֵ ), DWORD _solAtkMax;	//����������, 
	DWORD _critDef;//( �����ֿ� ), DWORD _solAtkMin;	//��С��������, 

	DWORD _critAppend; //( �����˺�����ֵ ),DWORD _magDodge; //ħ������(����), 
	DWORD _critRelief; //( �����˺�����ֵ ),DWORD _solDodge; //��������(����), 

	DWORD _defMax;	//����������, 
	DWORD _defMin;	//��С�������, 

	DWORD _phyDodge; //����, 
	DWORD _recHP; //�����ظ�, 
	DWORD _recMP; //ħ���ظ�, 
	DWORD _phyHit; //����������, 

	WORD _suckHP;//������Ѫ, DWORD _curHP;//����(����), 
	WORD _suckMP;//��������, DWORD _curHP;//����(����), 
	WORD _equipDrop; //װ������, DWORD _curMP;// ����(����), 
	WORD _coinDrop; //��ҵ���, DWORD _curMP;// ����(����), 

	DWORD _magAtkMax;	//����ܹ���(����), 
	DWORD _magAtkMin;	//��С���ܹ���(����), 

	DWORD _magDefMax; //����ܷ���(����), 
	DWORD _magDefMin; //��С���ܷ���(����), 

	WORD _skillId[10];//�����б�, Ŀǰ��� 8��, 2��Ԥ��
	BYTE _fightSkill[4];//ս������������ָ��_skillId[10]��1-10λ�ã�0��ʾû��

	WORD _teammate[3]; //����Ӷ��ID��, ���3��

	DWORD _factionId; //����Id:Ϊ0û�м��빫��,  
	BYTE _factionJob;//����ְλ,  
	DWORD _factionDistribute; //�ﹱ(����),  

	DWORD _atkSpeed;//�����ٶ�(����), 

	DWORD _repution; //����(����),  
	DWORD _smeltVar; //����ֵ(����), 

	DWORD _vipVar; //�ۼƳ�ֵ, 

	PlayerInfo() { memset(this, 0, sizeof(*this) ); };

	PlayerInfo(ROLE* role);
};

//��ͨ����

struct ItemBriefInfo
{
	DWORD dwId; //����ΨһID
	WORD wIndex; //��������
	BYTE byEquipLeftRight:1; //1:left 0:right, (Ԥ���ֶ�)
	BYTE byExtraAttrNums:2; //������������, (Ԥ���ֶ�)
	BYTE byStatus:5; //����״̬��Ϣ, (Ԥ���ֶ�)
	BYTE byRes;	//Ԥ���ֶ�
	DWORD dwCount; //���ߵ��Ӹ���
	BYTE quality:4; //����Ʒ��
	BYTE byHole:4; //�Ѿ����Ŀ�����
	BYTE byStrengthLv; //ǿ���ȼ�
	BYTE byLucky;	//װ������ֵ
	BYTE byLuckyDef;	//װ������ֵ����
	DWORD dwPower; //װ��ս��
	WORD wStone[5]; //��ʯ��Ƕ��,��ʯID; 0:δ��Ƕ��ʯ
	WORD wAttr[5]; //�����������,


	//DWORD dwId; //����ΨһID
	//WORD wIndex; //��������

	//BYTE byEquipLeftRight:1; //1:left 0:right, (Ԥ���ֶ�)

	//BYTE byExtraAttrNums:2; //������������, (Ԥ���ֶ�)

	//BYTE byStatus:5; //����״̬��Ϣ, (Ԥ���ֶ�)

	//DWORD dwCount; //���ߵ��Ӹ���

	//BYTE quality:4; //����Ʒ��
	//BYTE byRes:4; //�Ѿ����Ŀ�����
	//BYTE byStrengthLv; //ǿ���ȼ� // ���ĵȼ�
	//BYTE byLucky;	//װ������ֵ
	//DWORD dwPower; //װ��ս�� //���ľ���
	//WORD wStone[MAX_HOLE_NUMS]; //��ʯ��Ƕ��,��ʯID; 0:δ��Ƕ��ʯ

	ZERO_CONS(ItemBriefInfo)

	ItemBriefInfo(ITEM_CACHE* itemCache);
};


struct MercenaryInfo
{
	WORD wIndex; //Ӷ������
	BYTE byLevel;  //Ӷ���ȼ�
	BYTE byStar;	//��ǰ�Ǽ�

	DWORD dwPower; //ս��

	DWORD dwMaxHP; //�������
	DWORD dwMaxMP; //���ħ��

	DWORD dwAtkMin; //��С����
	DWORD dwAtkMax; //��󹥻�

	DWORD dwMagMin; //��С���ܹ���
	DWORD dwMagMax; //����ܹ���

	DWORD dwAtkDefMin; //��С����
	DWORD dwAtkDefMax; //������

	DWORD dwMagDefMin; //��С���ܷ���
	DWORD dwMagDefMax; //����ܷ���

	WORD wTamina; //����, 
	WORD wStrength; //����, 
	WORD wAgility; //����, 
	WORD wIntellect; //�ǻ�, 
	WORD wIpirit; //����,  
	BYTE byStatus; //Ӷ��״̬λ:0δ����, 1:����, 2:δ����
	BYTE byResVal; //�����ֽ�
	//�����б�
	WORD vSkills[4]; //�����б�,ֻ��ʾ����ʹ�õ�.û����дΪ0

	MercenaryInfo( CMercenary* mercenary);
};

#pragma pack(pop)


#endif		//__COMMON_H__
