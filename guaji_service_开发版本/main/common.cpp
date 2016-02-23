#include "pch.h"
#include "log.h"
#include "base.h"
#include "protocal.h"
#include "db.h"
#include "common.h"
#include "factionCtrl.h"
#include "itemCtrl.h"
#include "jsonpacket.h"
#include <cstdio>
#include "mercenary.h"



PlayerInfo::PlayerInfo( ROLE* role )
{
	memset( this, 0, sizeof(*this) );

	this->_playerId = role->dwRoleID; //��ɫID
	this->_coin = role->dwCoin;
	this->_ingot = role->dwIngot;
	this->_curExp = role->dwExp;
	this->_maxExp = CONFIG::getRoleUpgradeExp(role->wLevel );
	this->_job = role->byJob;
	this->_sex = role->bySex;
	this->_level = (BYTE)role->wLevel;
	this->_vip = (BYTE)role->getVipLevel();
	strncpy( this->_name, role->roleName.c_str(), sizeof(this->_name)-1 );
	this->_curMap = (BYTE)role->wLastFightMapID;
	this->_maxMap = (BYTE)role->wCanFightMaxMapID;
	this->_suite		= role->byNewbieGuideStep;
	this->_lucky		= (BYTE)role->roleAttr[ E_ATTR_LUCK ]; //����ֵ;, 
	this->_luckyDef		= (BYTE)role->roleAttr[ E_LUCK_DEF ];//���˷���, _power; //����(����), 
	this->_titleId		= (BYTE)role->byTitleId; //��ǰʹ�õĳƺ�Id, 
	this->_stamina		= (WORD)role->roleAttr[ E_ATTR_STAMINA	 ]; //����, 
	this->_strength		= (WORD)role->roleAttr[ E_ATTR_STRENGTH	 ]; //����, 
	this->_agility		= (WORD)role->roleAttr[ E_ATTR_AGILITY	 ]; //����, 
	this->_intellect	= (WORD)role->roleAttr[ E_ATTR_INTELLECT ]; //�ǻ�, 
	this->_spirit		= (WORD)role->roleAttr[ E_ATTR_SPIRIT	 ]; //����,  
	this->_curEquipAward= role->byCurLevelAwardStep;
	this->_rechargeState= role->byRechargeState;
	this->_warPower		= role->dwFightValue; //ս��ֵ, 
	this->_maxHP		= role->roleAttr[ E_ATTR_HP_MAX2 ];// ���Ѫ��, 
	this->_maxMP		= role->roleAttr[ E_ATTR_MP_MAX2 ];// ���ħ��, 
	this->_atkMax		= role->roleAttr[ E_ATTR_ATK_MAX2 ];	//����������, 
	this->_atkMin		= role->roleAttr[ E_ATTR_ATK_MIN2 ];	//����������, 
	this->_critVal		= role->roleAttr[ E_ATTR_CRIT ];	//( ����ֵ ), DWORD _solAtkMax;	//����������, 
	this->_critDef		= role->roleAttr[ E_ATTR_CRIT_RES ];//( �����ֿ� ), DWORD _solAtkMin;	//��С��������, 
	this->_critAppend	= role->roleAttr[ E_ATTR_CRIT_INC ]; //( �����˺�����ֵ ),DWORD _magDodge; //ħ������(����), 
	this->_critRelief	= role->roleAttr[ E_ATTR_CRIT_RED ]; //( �����˺�����ֵ ),DWORD _solDodge; //��������(����), 
	this->_defMax		= role->roleAttr[ E_ATTR_DEF_MAX ];	//������, 
	this->_defMin		= role->roleAttr[ E_ATTR_DEF_MIN ];	//��С����, 
	this->_phyDodge		= role->roleAttr[ E_ATTR_DODGE ]; //����, 
	this->_recHP		= role->roleAttr[ E_ATTR_HP_REC2 ]; //�����ظ�, 
	this->_recMP		= role->roleAttr[ E_ATTR_MP_REC2 ]; //ħ���ظ�, 
	this->_phyHit		= role->roleAttr[ E_ATTR_HIT ]; //����������, 
	this->_suckHP		= (WORD)role->roleAttr[ E_ATTR_ATT_HEM ];//������Ѫ, DWORD _curHP;//����(����), 
	this->_suckMP		= (WORD)role->roleAttr[ E_ATTR_ATT_MANA ];//��������, DWORD _curHP;//����(����), 
	this->_equipDrop	= (WORD)role->roleAttr[ E_ATTR_EQU_DROP ]; //װ������, DWORD _curMP;// ����(����), 
	this->_coinDrop		= (WORD)role->roleAttr[ E_ATTR_MONEY_DROP ]; //��ҵ���, DWORD _curMP;// ����(����), 
	this->_magAtkMax	= role->roleAttr[ E_ATTR_SPELLATK_MAX ];	//����ܹ���(����), 
	this->_magAtkMin	= role->roleAttr[ E_ATTR_SPELLATK_MIN ];	//��С���ܹ���(����), 
	this->_magDefMax	= role->roleAttr[ E_ATTR_SPELLDEF_MAX ]; //����ܷ���(����), 
	this->_magDefMin	= role->roleAttr[ E_ATTR_SPELLDEF_MIN ]; //��С���ܷ���(����), 

	//WORD _teammate[3]; //����Ӷ��ID��, ���3��

	//DWORD _atkSpeed;//�����ٶ�(����), 

	this->_repution = role->dwHonour;
	this->_smeltVar = role->dwSmeltValue;
	this->_vipVar = role->dwCharge;
	//memcpy( &(this->_curHP), &(role->roleAttr[E_ATTR_HP]), E_ATTR_PHY_HIT*sizeof(DWORD) );//��Ҫ����������� wm
	
	if ( role->faction )
	{
		this->_factionId = role->faction->id;
		auto member = role->faction->get_member(role->dwRoleID);
		if (member)
			this->_factionJob = member->job;
		this->_factionDistribute = 0; //�ﹱ(����),  
	}

	//size_t nums = min<size_t>(MERCENARY_NUM, role->cMerMgr.m_vecCMercenary.size());
	//for (size_t i = 0; i < nums; i++ )
	//{
	//	this->_teammate[i]= role->cMerMgr.m_vecCMercenary[i].wID;
	//}
	for (auto& it : role->cMerMgr.m_vecCMercenary)
	{
		if (it.getFightStatus() == E_FIGHT_ON)
		{
			this->_teammate[0] = it.wID;
		}
	}
	//SKILL::InitRoleSkills(role, this->_skillId, this->_fightSkill);//�����б�, Ŀǰ��� 8��, 2��Ԥ��

	string suitData;
	//if ( vecTempSuit != role->vecSuit )//�б仯��
	//{
		S_APPEND_WORD( suitData, 0);
		for ( auto& one : role->vecSuit )
		{
			S_APPEND_BYTE( suitData, (BYTE)one );
		}

		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_SUIT_INFO, suitData) );
	//}
}

ItemBriefInfo::ItemBriefInfo(ITEM_CACHE* itemCache)
{
	//WORD byStone[5]; //��ʯ��Ƕ��,��ʯID; 0:δ��Ƕ��ʯ
	memset(this, 0, sizeof(ItemBriefInfo));
	dwId = (DWORD)itemCache; 		//����ΨһID
	wIndex = itemCache->itemCfg->id; 	//��������
	dwCount = itemCache->itemNums;	//���߸���
	byStarLevel = itemCache->star_level;
	quality = itemCache->byQuality;
	byStrengthLv = itemCache->byStrengthLv;
	dwPower = itemCache->dwFightValue;
	byExtraAttrNums = itemCache->byExtraAttrNums;
	byLucky = itemCache->byLuckPoint;
	byLuckyDef = itemCache->byLuckDefPoint;

	for (size_t i = 0; i < MAX_EQUIP_BASE_ATTR_NUMS; i++ )
	{
		BYTE type = itemCache->attrValueBase[i].attr;
		WORD value = WORD( itemCache->attrValueBase[i].value );

		if ( type == E_ATTR_LUCK )
		{
			byLucky += value;
		}
		if ( type == E_LUCK_DEF )
		{
			byLuckyDef += value;
		}
	}

	byHole = itemCache->byHole;

	for (size_t i = 0; i < MAX_EQUIP_EXTRA_ATTR_NUMS; i++ )
	{
		BYTE type = itemCache->attrValueExtra[i].attr;
		WORD value = WORD( itemCache->attrValueExtra[i].value );

		COND_CONTINUE(type < E_ATTR_STAMINA || type > E_ATTR_SPIRIT );
		wAttr[ type - E_ATTR_STAMINA ] = value;
	}

	wStone = itemCache->wStone;
}

MercenaryInfo::MercenaryInfo( CMercenary* mercenary)
{
	memset(this, 0, sizeof( *this ) );

	for ( BYTE type = 0; type < BASE_ATTR_NUM; type++ )
	{
		DWORD dwTempAttrValue = mercenary->merAttr[ E_ATTR_STAMINA + type ];
		ADDUP( dwTempAttrValue, mercenary->attrValueTrain[type] );
		//mercenary->tranformBaseAttr( E_ATTR_STAMINA + type, dwTempAttrValue );
	}

	wIndex		= mercenary->wID	; //Ӷ������
	byLevel		= (BYTE)mercenary->wLevel	;  //Ӷ���ȼ�
	byStar		= mercenary->byStar	;	//��ǰ�Ǽ�
	dwPower		= mercenary->dwFightValue	; //ս��
	dwMaxHP		= mercenary->merAttr[E_ATTR_HP_MAX2]	; //�������
	dwMaxMP		= mercenary->merAttr[E_ATTR_MP_MAX2]	; //���ħ��
	dwAtkMin	= mercenary->merAttr[E_ATTR_ATK_MIN2]	; //��С����
	dwAtkMax	= mercenary->merAttr[E_ATTR_ATK_MAX2]	; //��󹥻�
	dwMagMin	= mercenary->merAttr[E_ATTR_SPELLATK_MIN]	; //��С���ܹ���
	dwMagMax	= mercenary->merAttr[E_ATTR_SPELLATK_MAX]	; //����ܹ���
	dwAtkDefMin = mercenary->merAttr[E_ATTR_DEF_MIN]	; //��С����
	dwAtkDefMax = mercenary->merAttr[E_ATTR_DEF_MAX]	; //������
	dwMagDefMin = mercenary->merAttr[E_ATTR_SPELLDEF_MIN]	; //��С���ܷ���
	dwMagDefMax = mercenary->merAttr[E_ATTR_SPELLDEF_MAX]	; //����ܷ���
	wTamina		= (WORD)mercenary->merAttr[E_ATTR_STAMINA	]	; //����, 
	//wTamina += mercenary->attrValueTrain[0];
	wStrength	= (WORD)mercenary->merAttr[E_ATTR_STRENGTH]	; //����, 
	//wStrength += mercenary->attrValueTrain[1];
	wAgility	= (WORD)mercenary->merAttr[E_ATTR_AGILITY	]	; //����, 
	//wAgility += mercenary->attrValueTrain[2];
	wIntellect	= (WORD)mercenary->merAttr[E_ATTR_INTELLECT]	; //�ǻ�, 
	//wIntellect += mercenary->attrValueTrain[3];
	wIpirit		= (WORD)mercenary->merAttr[E_ATTR_SPIRIT	]	; //����,  
	//wIpirit += mercenary->attrValueTrain[4];
	byStatus	= (WORD)mercenary->byFight; //Ӷ��״̬λ:0δ����, 1:����, 2:δ����
	byResVal	= 0; //�����ֽ�
	//vSkills[4]; //�����б�,ֻ��ʾ����ʹ�õ�.û����дΪ0

	for (size_t i = 0; i < SKILL_MAX_NUM; i++ )
	{
		vSkills[ i ] = mercenary->vIndexSkills[ i ].skillid;
	}
}