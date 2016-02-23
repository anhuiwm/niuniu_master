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

	byte _job; //��ɫְҵ
	byte _sex; //��ɫ�Ա�
	byte _level;// ��ɫ�ȼ�, 
	byte _vip;//��ɫVIP�ȼ�, 

	char _name[22]; //��ɫ����

	byte _curMap; //��ǰ������ͼ����
	byte _maxMap; //����ͨ�ص�ͼID, 

	byte _suite; //��ǰ������������ 
	byte _lucky; //����ֵ;, 
	byte _luckyDef;//���˷���, _power; //����(����), 
	byte _titleId; //��ǰʹ�õĳƺ�Id, 

	WORD _stamina; //����, 
	WORD _strength; //����, 
	WORD _agility; //����, 
	WORD _intellect; //�ǻ�, 
	WORD _spirit; //����,  

	byte _curEquipAward; //��ǰװ��������ȡ����
	byte _rechargeState; //�׳佱��״��.0:δ��ֵ;1:�ѳ�ֵδ��ȡ;2:����ȡ

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
	byte _fightSkill[4];//ս������������ָ��_skillId[10]��1-10λ�ã�0��ʾû��

	WORD _teammate[3]; //����Ӷ��ID��, ���3��

	DWORD _factionId; //����Id:Ϊ0û�м��빫��,  
	byte _factionJob;//����ְλ,  
	DWORD _factionDistribute; //�ﹱ(����),  

	DWORD _atkSpeed;//�����ٶ�(����), 

	DWORD _repution; //����(����),  
	DWORD _smeltVar; //����ֵ(����), 

	DWORD _vipVar; //�ۼƳ�ֵ, 

	PlayerInfo() { memset(this, 0, sizeof(*this) ); };

	PlayerInfo(ROLE* role);
};

//��ͨ����

class ItemBriefInfo
{
public:
	DWORD dwId = 0; //����ΨһID
	WORD wIndex = 0; //��������
	union{
		byte __unused = 0;
		struct{
			byte byEquipLeftRight:1; //1:left 0:right, (Ԥ���ֶ�)
			byte byExtraAttrNums:2; //������������, (Ԥ���ֶ�)
			byte byStarLevel:5; //����״̬��Ϣ, (Ԥ���ֶ�)
		};
	};
	byte byRes = 0;	//Ԥ���ֶ�
	unsigned dwCount = 0; //���ߵ��Ӹ���
	union{
		byte __unused2 = 0;
		struct{
			byte quality : 4; //����Ʒ��
			byte byHole : 4; //�Ѿ����Ŀ�����
		};
	};
	byte byStrengthLv = 0; //ǿ���ȼ�
	byte byLucky = 0;	//װ������ֵ
	byte byLuckyDef = 0;	//װ������ֵ����
	DWORD dwPower = 0; //װ��ս��
	array<WORD, 5> wStone{}; //��ʯ��Ƕ��,��ʯID; 0:δ��Ƕ��ʯ
	array<WORD, 5> wAttr{}; //�����������,


	//DWORD dwId; //����ΨһID
	//WORD wIndex; //��������

	//byte byEquipLeftRight:1; //1:left 0:right, (Ԥ���ֶ�)

	//byte byExtraAttrNums:2; //������������, (Ԥ���ֶ�)

	//byte byStatus:5; //����״̬��Ϣ, (Ԥ���ֶ�)

	//DWORD dwCount; //���ߵ��Ӹ���

	//byte quality:4; //����Ʒ��
	//byte byRes:4; //�Ѿ����Ŀ�����
	//byte byStrengthLv; //ǿ���ȼ� // ���ĵȼ�
	//byte byLucky;	//װ������ֵ
	//DWORD dwPower; //װ��ս�� //���ľ���
	//WORD wStone[MAX_HOLE_NUMS]; //��ʯ��Ƕ��,��ʯID; 0:δ��Ƕ��ʯ


	ItemBriefInfo(ITEM_CACHE* itemCache);
};


struct MercenaryInfo
{
	WORD wIndex; //Ӷ������
	byte byLevel;  //Ӷ���ȼ�
	byte byStar;	//��ǰ�Ǽ�

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
	byte byStatus; //Ӷ��״̬λ:0δ����, 1:����, 2:δ����
	byte byResVal; //�����ֽ�
	//�����б�
	WORD vSkills[4]; //�����б�,ֻ��ʾ����ʹ�õ�.û����дΪ0

	MercenaryInfo( CMercenary* mercenary);
};

#pragma pack(pop)


#endif		//__COMMON_H__
