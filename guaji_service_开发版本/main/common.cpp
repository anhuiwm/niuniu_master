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

	this->_playerId = role->dwRoleID; //角色ID
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
	this->_lucky		= (BYTE)role->roleAttr[ E_ATTR_LUCK ]; //幸运值;, 
	this->_luckyDef		= (BYTE)role->roleAttr[ E_LUCK_DEF ];//幸运防御, _power; //体力(保留), 
	this->_titleId		= (BYTE)role->byTitleId; //当前使用的称号Id, 
	this->_stamina		= (WORD)role->roleAttr[ E_ATTR_STAMINA	 ]; //耐力, 
	this->_strength		= (WORD)role->roleAttr[ E_ATTR_STRENGTH	 ]; //力量, 
	this->_agility		= (WORD)role->roleAttr[ E_ATTR_AGILITY	 ]; //敏捷, 
	this->_intellect	= (WORD)role->roleAttr[ E_ATTR_INTELLECT ]; //智慧, 
	this->_spirit		= (WORD)role->roleAttr[ E_ATTR_SPIRIT	 ]; //精神,  
	this->_curEquipAward= role->byCurLevelAwardStep;
	this->_rechargeState= role->byRechargeState;
	this->_warPower		= role->dwFightValue; //战力值, 
	this->_maxHP		= role->roleAttr[ E_ATTR_HP_MAX2 ];// 最大血量, 
	this->_maxMP		= role->roleAttr[ E_ATTR_MP_MAX2 ];// 最大魔法, 
	this->_atkMax		= role->roleAttr[ E_ATTR_ATK_MAX2 ];	//物理攻击上限, 
	this->_atkMin		= role->roleAttr[ E_ATTR_ATK_MIN2 ];	//物理攻击下限, 
	this->_critVal		= role->roleAttr[ E_ATTR_CRIT ];	//( 暴击值 ), DWORD _solAtkMax;	//最大道术攻击, 
	this->_critDef		= role->roleAttr[ E_ATTR_CRIT_RES ];//( 暴击抵抗 ), DWORD _solAtkMin;	//最小道术攻击, 
	this->_critAppend	= role->roleAttr[ E_ATTR_CRIT_INC ]; //( 暴击伤害增加值 ),DWORD _magDodge; //魔法闪避(保留), 
	this->_critRelief	= role->roleAttr[ E_ATTR_CRIT_RED ]; //( 暴击伤害减免值 ),DWORD _solDodge; //道术闪避(保留), 
	this->_defMax		= role->roleAttr[ E_ATTR_DEF_MAX ];	//最大防御, 
	this->_defMin		= role->roleAttr[ E_ATTR_DEF_MIN ];	//最小防御, 
	this->_phyDodge		= role->roleAttr[ E_ATTR_DODGE ]; //闪避, 
	this->_recHP		= role->roleAttr[ E_ATTR_HP_REC2 ]; //生命回复, 
	this->_recMP		= role->roleAttr[ E_ATTR_MP_REC2 ]; //魔法回复, 
	this->_phyHit		= role->roleAttr[ E_ATTR_HIT ]; //物理命中率, 
	this->_suckHP		= (WORD)role->roleAttr[ E_ATTR_ATT_HEM ];//攻击吸血, DWORD _curHP;//保留(二级), 
	this->_suckMP		= (WORD)role->roleAttr[ E_ATTR_ATT_MANA ];//攻击吸蓝, DWORD _curHP;//保留(二级), 
	this->_equipDrop	= (WORD)role->roleAttr[ E_ATTR_EQU_DROP ]; //装备掉率, DWORD _curMP;// 保留(二级), 
	this->_coinDrop		= (WORD)role->roleAttr[ E_ATTR_MONEY_DROP ]; //金币掉率, DWORD _curMP;// 保留(二级), 
	this->_magAtkMax	= role->roleAttr[ E_ATTR_SPELLATK_MAX ];	//最大技能攻击(二级), 
	this->_magAtkMin	= role->roleAttr[ E_ATTR_SPELLATK_MIN ];	//最小技能攻击(二级), 
	this->_magDefMax	= role->roleAttr[ E_ATTR_SPELLDEF_MAX ]; //最大技能防御(二级), 
	this->_magDefMin	= role->roleAttr[ E_ATTR_SPELLDEF_MIN ]; //最小技能防御(二级), 

	//WORD _teammate[3]; //上阵佣兵ID组, 最多3个

	//DWORD _atkSpeed;//攻击速度(保留), 

	this->_repution = role->dwHonour;
	this->_smeltVar = role->dwSmeltValue;
	this->_vipVar = role->dwCharge;
	//memcpy( &(this->_curHP), &(role->roleAttr[E_ATTR_HP]), E_ATTR_PHY_HIT*sizeof(DWORD) );//还要添加其他属性 wm
	
	if ( role->faction )
	{
		this->_factionId = role->faction->id;
		auto member = role->faction->get_member(role->dwRoleID);
		if (member)
			this->_factionJob = member->job;
		this->_factionDistribute = 0; //帮贡(保留),  
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
	//SKILL::InitRoleSkills(role, this->_skillId, this->_fightSkill);//技能列表, 目前最多 8个, 2个预留

	string suitData;
	//if ( vecTempSuit != role->vecSuit )//有变化的
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
	//WORD byStone[5]; //宝石镶嵌孔,宝石ID; 0:未镶嵌宝石
	memset(this, 0, sizeof(ItemBriefInfo));
	dwId = (DWORD)itemCache; 		//道具唯一ID
	wIndex = itemCache->itemCfg->id; 	//道具索引
	dwCount = itemCache->itemNums;	//道具个数
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

	wIndex		= mercenary->wID	; //佣兵索引
	byLevel		= (BYTE)mercenary->wLevel	;  //佣兵等级
	byStar		= mercenary->byStar	;	//当前星级
	dwPower		= mercenary->dwFightValue	; //战力
	dwMaxHP		= mercenary->merAttr[E_ATTR_HP_MAX2]	; //最大生命
	dwMaxMP		= mercenary->merAttr[E_ATTR_MP_MAX2]	; //最大魔法
	dwAtkMin	= mercenary->merAttr[E_ATTR_ATK_MIN2]	; //最小攻击
	dwAtkMax	= mercenary->merAttr[E_ATTR_ATK_MAX2]	; //最大攻击
	dwMagMin	= mercenary->merAttr[E_ATTR_SPELLATK_MIN]	; //最小技能攻击
	dwMagMax	= mercenary->merAttr[E_ATTR_SPELLATK_MAX]	; //最大技能攻击
	dwAtkDefMin = mercenary->merAttr[E_ATTR_DEF_MIN]	; //最小防御
	dwAtkDefMax = mercenary->merAttr[E_ATTR_DEF_MAX]	; //最大防御
	dwMagDefMin = mercenary->merAttr[E_ATTR_SPELLDEF_MIN]	; //最小技能防御
	dwMagDefMax = mercenary->merAttr[E_ATTR_SPELLDEF_MAX]	; //最大技能防御
	wTamina		= (WORD)mercenary->merAttr[E_ATTR_STAMINA	]	; //耐力, 
	//wTamina += mercenary->attrValueTrain[0];
	wStrength	= (WORD)mercenary->merAttr[E_ATTR_STRENGTH]	; //力量, 
	//wStrength += mercenary->attrValueTrain[1];
	wAgility	= (WORD)mercenary->merAttr[E_ATTR_AGILITY	]	; //敏捷, 
	//wAgility += mercenary->attrValueTrain[2];
	wIntellect	= (WORD)mercenary->merAttr[E_ATTR_INTELLECT]	; //智慧, 
	//wIntellect += mercenary->attrValueTrain[3];
	wIpirit		= (WORD)mercenary->merAttr[E_ATTR_SPIRIT	]	; //精神,  
	//wIpirit += mercenary->attrValueTrain[4];
	byStatus	= (WORD)mercenary->byFight; //佣兵状态位:0未解锁, 1:上阵, 2:未上阵
	byResVal	= 0; //保留字节
	//vSkills[4]; //技能列表,只显示正在使用的.没有填写为0

	for (size_t i = 0; i < SKILL_MAX_NUM; i++ )
	{
		vSkills[ i ] = mercenary->vIndexSkills[ i ].skillid;
	}
}