
#include <algorithm>

#include "protocal.h"
#include "item.h"
#include "itemCtrl.h"
#include "roleCtrl.h"
#include "factionCtrl.h"
#include "config.h"
#include "skill.h"
#include "friend.h"
#include "rank.h"
#include "arena.h"
#include "../tinyxml/tinyxml.h"

using namespace BASE;


bool get_element_text(TiXmlNode* node, string& str)
{
	if(node == nullptr)
		return false;
	node = node->FirstChild();
	if(node == nullptr)
		return false;
	str = node->Value();
	return true;
}

bool get_element_int(TiXmlNode* node, int& value)
{
	if(node == nullptr)
		return false;
	node = node->FirstChild();
	if(node == nullptr)
		return false;
	value = atoi(node->Value());
	return true;
}

void item::load_config()
{
	TiXmlDocument doc;
	string config_file = string( getProgDir() )+ STR_FSEP + "config/equip_halidome.xml";
	doc.LoadFile(config_file.c_str());
	if(doc.Error())
	{
		logERROR("%s", doc.ErrorDesc());
		exit(0);
	}

	auto root = doc.RootElement();

	auto sub = root->FirstChild();
	while(sub)
	{
		int itemid;
		auto attr = sub->FirstChild();
		if(!get_element_int(attr, itemid))continue;

		auto temp = CONFIG::getItemCfg(itemid);
		if(temp == nullptr)
		{
			logFATAL("can't find basic config of equip from id:%d", itemid);
			exit(0);
		}

		attr = attr->NextSibling();
		while(attr)
		{
			if(strstr(attr->Value(), "basic_attr") == nullptr)break;
			string value;
			get_element_text(attr, value);
			vector<string> value_seg;
			BASE::strtoken2Vec(value.c_str(), ":", value_seg);
			if(value_seg.size() != 2)
			{
				// error process
				exit(0);
			}
			auto attr_index = atoi(value_seg[0].c_str());
			if(attr_index >= E_ATTR_MAX)
			{
				// error process
				exit(0);
			}
			vector<string> range_str;
			BASE::strtoken2Vec(value_seg[1].c_str(), ";", range_str);
			if(range_str.size() > 11)
			{
				// error process
				exit(0);
			}

			CONFIG::ITEM_CFG::equip_extend_attr_config eattr;
			eattr.type = (byte)attr_index;
			for(size_t i = 0; i < range_str.size(); i++)
			{
				vector<string> range_pair;
				BASE::strtoken2Vec(range_str[i].c_str(), ",", range_pair);
				if(range_pair.size() > 2)
				{
					// error process
					exit(0);
				}
				eattr.range.push_back(make_pair(atoi(range_pair[0].c_str()), atoi(range_pair[1].c_str())));
			}
			temp->equip_extend.extend_attrs.push_back(eattr);

			attr = attr->NextSibling();
		}
		while(attr)
		{
			if(strstr(attr->Value(), "skill_attr") == nullptr)break;
			string value;
			get_element_text(attr, value);
			vector<string> value_seg;
			BASE::strtoken2Vec(value.c_str(), ":", value_seg);
			if(value_seg.size() != 2)
			{
				// error process
				exit(0);
			}
			auto attr_index = atoi(value_seg[0].c_str());
			if(attr_index < 50)
			{
				// error process
				exit(0);
			}
			vector<string> range_str;
			BASE::strtoken2Vec(value_seg[1].c_str(), ";", range_str);
			if(range_str.size() > 11)
			{
				// error process
				exit(0);
			}

			CONFIG::ITEM_CFG::equip_extend_attr_config eattr;
			eattr.type = (byte)attr_index;
			for(size_t i = 0; i < range_str.size(); i++)
			{
				vector<string> range_pair;
				BASE::strtoken2Vec(range_str[i].c_str(), ",", range_pair);
				if(range_pair.size() > 2)
				{
					// error process
					exit(0);
				}
				eattr.range.push_back(make_pair(atoi(range_pair[0].c_str()), atoi(range_pair[1].c_str())));
			}
			temp->equip_extend.skill_attrs.push_back(eattr);

			attr = attr->NextSibling();
		}
		if(attr)
		{
			string value;
			get_element_text(attr, value);
			vector<string> value_seg;
			BASE::strtoken2Vec(value.c_str(), ":", value_seg);
			if(value_seg.size() != 2)
			{
				// error process
				exit(0);
			}
			auto attr_index = atoi(value_seg[0].c_str());
			if(attr_index >= E_ATTR_MAX)
			{
				// error process
				exit(0);
			}

			temp->equip_extend.basic_attr.type = (byte)attr_index;
			vector<string> values;
			BASE::strtoken2Vec(value_seg[1].c_str(), ",", values);
			for(size_t i = 0; i < values.size(); i++)
			{
				temp->equip_extend.basic_attr.values.push_back(atoi(values[i].c_str()));
			}

			attr = attr->NextSibling();
		}
		if(attr)
		{
			string value;
			get_element_text(attr, value);
			vector<string> value_seg;
			BASE::strtoken2Vec(value.c_str(), ":", value_seg);
			if(value_seg.size() != 2)
			{
				// error process
				exit(0);
			}
			auto attr_index = atoi(value_seg[0].c_str());
			if(attr_index >= E_ATTR_MAX)
			{
				// error process
				exit(0);
			}

			temp->equip_extend.forging_up_attr.type = (byte)attr_index;
			vector<string> values;
			BASE::strtoken2Vec(value_seg[1].c_str(), ",", values);
			for(size_t i = 0; i < values.size(); i++)
			{
				temp->equip_extend.forging_up_attr.values.push_back(atoi(values[i].c_str()));
			}
		}

		sub = sub->NextSibling();
	}
}

#define ADDUP_STRENGTH( attr, addAttr, value, addValue ) \
	if ( addAttr == attr  ) \
	{\
		value += addValue;\
		break;\
	}


void ITEM_DATA::initFromItemCache(const ITEM_CACHE* itemCache)
{
	this->itemIndex = itemCache->itemCfg->id;

	this->itemNums = itemCache->itemNums;
	this->byStatus = itemCache->byStatus;
	this->byStrengthLv = itemCache->byStrengthLv;
	this->byQuality = itemCache->byQuality;

	this->byLuckPoint = itemCache->byLuckPoint;
	this->byLuckDefPoint = itemCache->byLuckDefPoint;
	this->byHole = itemCache->byHole;
	memcpy( this->wStone, itemCache->wStone, sizeof(this->wStone) );

	memcpy( this->attrValueExtra, itemCache->attrOldValueExtra, sizeof(this->attrValueExtra) );
	star_level = itemCache->star_level;
	memcpy(equip_extends, itemCache->equip_extends, sizeof(equip_extends));

	this->dwGetStrengthStoneNum = itemCache->dwGetStrengthStoneNum;
}

void MERCENARY_DATA::initFromMerCache( const CMercenary* pCmer )
{
	memcpy(this->vIndexSkills, pCmer->vIndexSkills, sizeof( this->vIndexSkills) ); // 技能
	this->wID = pCmer->wID;
	//memcpy(this->vBodyEquip, pCmer->vBodyEquip, sizeof( this->vBodyEquip) );
	this->byFight = pCmer->byFight;
	this->byJob = pCmer->byJob;
	this->byStar = pCmer->byStar;
	this->bySex = pCmer->bySex;
	this->wLevel = pCmer->wLevel;
	//memcpy(this->merAttr, pCmer->merAttr, sizeof( this->merAttr) );
	memcpy(this->attrValueTrain, pCmer->attrValueTrain, sizeof( this->attrValueTrain) );
	//this->dwFightValue = pCmer->dwFightValue;   //战斗力
	size_t nums = min<size_t>( pCmer->vecHaloSkills.size(), HALO_SKILL_NUM );
	for ( size_t i = 0; i < nums; i++ )
	{
		COND_CONTINUE( pCmer->vecHaloSkills[i].skillid == 0 );
		this->vHaloSkills[i] = pCmer->vecHaloSkills[i];
	}
	this->dwStarExp = pCmer->dwStarExp;

	//身上
	for ( int i=0; i<MER_GRID_NUMS; i++)
	{
		if ( pCmer->vBodyEquip[i] == NULL )
			continue;

		if ( pCmer->vBodyEquip[i]->itemCfg == NULL )
			continue;

		if ( !IS_EQUIP(pCmer->vBodyEquip[i]->itemCfg) )
		{
			continue;
		}

		this->vBodyEquip[i].initFromItemCache( pCmer->vBodyEquip[i] );

	}
}


ITEM_CACHE::ITEM_CACHE(CONFIG::ITEM_CFG* itemCfg, size_t itemNums, BYTE byQuality)//, BYTE byChest_chestRandNums )
{
	memset(this, 0, sizeof(*this) );

	this->itemCfg = itemCfg;

	this->itemNums = itemNums;//数量

	if ( itemCfg->zeroHole == E_NO_HOLE )
	{
		this->byHole = 0;
	}
	else
	{
		RETURN_VOID( itemCfg->vecHoleRandSum.empty() );

		int index = getIndexOfVectorSum(itemCfg->vecHoleRandSum);

		this->byHole = (BYTE)index;
	}
	
	if (IS_DIVINE(itemCfg))
	{
		byStrengthLv = 1;
	}

	{
		//if ( IS_NORMAL_EQUIP(itemCfg) )
		//{
			//策划保证品质不为0
	this->byQuality = byQuality;
	if ( this->byQuality == 0 )
		this->byQuality = 1;

		//	//计算极品属性条数
		//	//if ( IS_EQUIP(itemCfg) )
		//	{
		//		for ( int i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++ )
		//			this->byExtraAttrNums += !!attrValueExtra[ i ].attr;
		//	}
		////}
	}

	this->randEquipExtraPro();

	this->resetItemAttrValue();
}

//ITEM_CACHE::ITEM_CACHE(const MINE* itemMine)
//{
//	memset(this, 0, sizeof(*this) );
//
//	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( itemMine->itemIndex);
//	if ( itemCfg == NULL )
//		return;
//
//	this->itemCfg = itemCfg;
//	this->itemNums = itemMine->itemNum;//数量
//	this->dwLingZhuExp = DWORD( ceil( itemCfg->price * itemCfg->mp_max * 0.001f ) );
//}

ITEM_CACHE::ITEM_CACHE(const ITEM_DATA* itemData)
{
	memset(this, 0, sizeof(*this) );

	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( itemData->itemIndex);
	if ( itemCfg == NULL )
		return;
	
	this->itemCfg = itemCfg;
	this->itemNums = itemData->itemNums;
	this->byStatus = itemData->byStatus;
	this->byStrengthLv = itemData->byStrengthLv;
	this->byQuality = itemData->byQuality;
	this->byLuckPoint = itemData->byLuckPoint;
	this->byLuckDefPoint = itemData->byLuckDefPoint;
	this->dwGetStrengthStoneNum = itemData->dwGetStrengthStoneNum;

	//if ( !IS_EQUIP(itemCfg) )
	//	return;

	this->byHole = itemData->byHole;
	memcpy( this->wStone, itemData->wStone, sizeof( this->wStone ) );
	memcpy( this->attrOldValueExtra, itemData->attrValueExtra, sizeof(this->attrValueExtra) );

	star_level = itemData->star_level;
	memcpy(equip_extends, itemData->equip_extends, sizeof(equip_extends));

	this->resetItemAttrValue();

	////计算极品属性条数
	//{
	//	for ( int i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++ )
	//		this->byExtraAttrNums += !!this->attrValueExtra[ i ].attr;
	//}
}

ITEM_CACHE::ITEM_CACHE(const ITEM_CACHE* itemCache)
{
	memset(this, 0, sizeof(*this) );

	memcpy( this, itemCache, sizeof(ITEM_CACHE) );
}

void ITEM_CACHE::resetEquipExtraPro()
{
	//创建时候随机出来的一级属性
	memcpy( this->attrValueExtra, this->attrOldValueExtra, sizeof(this->attrOldValueExtra) );

	//randEquipExtraPro( );

	//镶嵌的宝石增加的
	for ( int i=0; i<MAX_HOLE_NUMS; i++)
	{
		CONFIG::ITEM_CFG* itemStoneCfg = CONFIG::getItemCfg(wStone[i]);
		if( itemStoneCfg!=NULL && IS_EQUIP_GEM(itemStoneCfg) )
		{		
			BYTE attrType = itemStoneCfg->addAttrType;
			DWORD attrValue = itemStoneCfg->addAttrValue;

			COND_CONTINUE(attrType < E_ATTR_STAMINA || attrType > E_ATTR_SPIRIT );

			attrValueExtra[attrType-E_ATTR_STAMINA].attr = attrType;
			attrValueExtra[attrType-E_ATTR_STAMINA].value += attrValue;
		}
	}

	////计算极品属性条数
	for ( int i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++ )
	{
		this->byExtraAttrNums += !!attrValueExtra[ i ].attr;
	}

	//for(size_t i=0; i < sizeof(equip_extends)/sizeof(equip_extend); i++)
	//{
	//}
};

// 是否达到强化最大等级
bool ITEM_CACHE::isStrengthenMaxLevel()
{
	const BYTE STRENGHEN_MAX_LEVEL = 10; // 强化最高等级
	return this->byStrengthLv >= STRENGHEN_MAX_LEVEL;
}

// 强化升级 只影响基础属性
void ITEM_CACHE::levelUpStrengthen()
{
	if (!isStrengthenMaxLevel())
	{
		this->byStrengthLv++;

		this->resetItemAttrValue();
	}
}

//void ITEM_CACHE::transformExtraAttr()
//{
//	BYTE attr;
//	DWORD value;
//	BYTE addAttr;
//	DWORD addValue;
//	for ( int i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++)//道具额外属性才会出现一级属性（基础属性）
//	{
//		if ( attrValueExtra[i].attr == 0 )
//		{
//			continue;
//		}
//
//		attr = attrValueExtra[i].attr ;
//		value = attrValueExtra[i].value ;
//
//		if ( attr < E_ATTR_STAMINA || attr > E_ATTR_SPIRIT )//保证 一级属性  基础属性
//		{
//			continue;
//		}
//
//		CONFIG::ATTRDEFORM_CFG* attrTransCfg = CONFIG::getAttrDeformCfg( attr );
//		if ( attrTransCfg == NULL )
//		{
//			continue;
//		}
//
//		for ( auto& it : attrTransCfg->vecAttrDeform )
//		{
//			addAttr = it.attr;
//			addValue = DWORD( it.ratio * value ) ;
//			for ( int j=0; j<MAX_EQUIP_BASE_ATTR_NUMS; j++)//转换目标不会是一级属性 不会再额外属性中
//			{
//				BYTE attr = attrValueBase[ j ].attr;
//				if ( !IS_VALID_ATTR( attr  ) )
//					continue;
//				ADDUP_STRENGTH( attr, addAttr, attrValueBase[ j ].value, addValue );
//			}	
//		}
//	}
//}

void ITEM_CACHE::randEquipExtraPro( )
{
	//品质对应随机属性条数
	static int quality2ExtraNums[8] = { 0 ,0, 2, 3,	4, 5, 5, 5 };	//0index不用,品质1-7
	const size_t halidomes[] = {2,4,5,6,7,11,12,13,21,15,27,28,29,30,19,20,35,36,33,34,31,32,};

	int randTimes = min<size_t>( quality2ExtraNums[ this->byQuality ], MAX_EQUIP_EXTRA_ATTR_NUMS );

	setAttrValueExtra( randTimes );

	if(byQuality >= 5 && itemCfg->level_limit >= 25 && itemCfg->type == TYPE_EQUIP)
	{
		int extend_count = BASE::randBetween(1, 3);
		bool has_skill_extend = extend_count < 3 && BASE::randTo(2) == 0;
		size_t i = 0;
		for(;i < extend_count; i++)
		{
			auto index = BASE::randTo(itemCfg->equip_extend.extend_attrs.size());
			auto& attr = itemCfg->equip_extend.extend_attrs[index];
			equip_extends[i].type = (ATTR_TYPE)attr.type;
			equip_extends[i].value = BASE::randBetween(attr.range[0].first, attr.range[0].second);
		}
		if(has_skill_extend)
		{
			equip_extends_for_skill.type = (ATTR_TYPE)(BASE::randBetween(1, (int)e_equip_extend_attr::max_count) + 50);
			auto& range = itemCfg->equip_extend.skill_attrs[equip_extends[i].type].range[0];
			equip_extends_for_skill.value = BASE::randBetween(range.first, range.second);
		}
	}
}

void ITEM_CACHE::setAttrValueExtra( const int randTimes )
{
	BYTE attrType = 0;
	DWORD attrValue = 0;

	CONFIG::ATT_RAND_CFG* attRandCfg = CONFIG::getAttRandCfg( itemCfg->att_random );

	RETURN_VOID( attRandCfg == NULL );

	vector< CONFIG::ATT_RAND_PARA > vecAttrRand( attRandCfg->vAttRandPara );

	size_t nums = min<size_t>( randTimes, vecAttrRand.size() );

	for ( size_t i = 0; i < nums; i++ )
	{
		attrType = vecAttrRand[i].attr;
		attrValue = BASE::randBetween( vecAttrRand[i].valueMin, vecAttrRand[i].valueMax );

		COND_CONTINUE(attrType < E_ATTR_STAMINA || attrType > E_ATTR_SPIRIT );

		attrOldValueExtra[attrType-E_ATTR_STAMINA].attr = attrType;
		attrOldValueExtra[attrType-E_ATTR_STAMINA].value = attrValue;
	}
}

void ITEM_CACHE::setItemBasePro()
{
	//品质对应属性加成
	//static float quality2ProPer[8] = {1.0f,	0.9f, 0.95f, 1.00f, 1.05f, 1.10f, 1.15f, 1.20f };	//0index不用,品质1-7
	

#define SET_ITEM_BASE_PRO( itemCache, field, type  ) \
	do {\
		if ( itemCache->itemCfg->field == 0 ) \
			break;\
		if ( count >= MAX_EQUIP_BASE_ATTR_NUMS )\
			break;\
		itemCache->attrValueBase[ count ].attr = type;\
		itemCache->attrValueBase[ count ].value = (DWORD)(itemCache->itemCfg->field /** quality2ProPer[itemCache->byQuality]*/) ;\
		count++;\
	}while(0)

	int count = 0;
	SET_ITEM_BASE_PRO (this,hp_max	,E_ATTR_HP_MAX2 );			//	E_ATTR_HP_MAX2		//最大生命值
	SET_ITEM_BASE_PRO (this,mp_max	,E_ATTR_MP_MAX2 );			//	E_ATTR_MP_MAX2		//魔法上限
	SET_ITEM_BASE_PRO (this,atk_max	,E_ATTR_ATK_MAX2 );			//	E_ATTR_ATK_MAX2	    //攻击最大值
	SET_ITEM_BASE_PRO (this,atk_min	,E_ATTR_ATK_MIN2 );			//	E_ATTR_ATK_MIN2	    //攻击最小值
	SET_ITEM_BASE_PRO (this,def_max,E_ATTR_DEF_MAX );			//	E_ATTR_DEF_MAX	    //防御最大值
	SET_ITEM_BASE_PRO (this,def_min	,E_ATTR_DEF_MIN );			//	E_ATTR_DEF_MIN	    //防御最小值
	SET_ITEM_BASE_PRO (this,spellatk_max,E_ATTR_SPELLATK_MAX  );//	E_ATTR_SPELLATK_MAX //技能攻击最大值
	SET_ITEM_BASE_PRO (this,spellatk_min,E_ATTR_SPELLATK_MIN  );//	E_ATTR_SPELLATK_MIN //技能攻击最小值
	SET_ITEM_BASE_PRO (this,spelldef_max,E_ATTR_SPELLDEF_MAX  );//	E_ATTR_SPELLDEF_MAX //技能防御最大值
	SET_ITEM_BASE_PRO (this,spelldef_min,E_ATTR_SPELLDEF_MIN  );//	E_ATTR_SPELLDEF_MIN //技能防御最小值
	SET_ITEM_BASE_PRO (this,hit,E_ATTR_HIT );					//	E_ATTR_HIT			//命中值
	SET_ITEM_BASE_PRO (this,dodge,E_ATTR_DODGE );				//	E_ATTR_DODGE		//闪避值
	SET_ITEM_BASE_PRO (this,crit,E_ATTR_CRIT );					//	E_ATTR_CRIT		    //暴击值
	SET_ITEM_BASE_PRO (this,crit_res,E_ATTR_CRIT_RES );			//	E_ATTR_CRIT_RES	    //暴击抵抗值
	SET_ITEM_BASE_PRO (this,crit_inc	,E_ATTR_CRIT_INC );		//	E_ATTR_CRIT_INC	    //暴击伤害增加值
	SET_ITEM_BASE_PRO (this,crit_red	,E_ATTR_CRIT_RED );		//	E_ATTR_CRIT_RED	    //暴击伤害减免值
	SET_ITEM_BASE_PRO (this,mp_rec	,E_ATTR_MP_REC2 );			//	E_ATTR_MP_REC2		//每回合魔法恢复值
	SET_ITEM_BASE_PRO (this,hp_rec	,E_ATTR_HP_REC2 );			//	E_ATTR_HP_REC2		//每回合生命恢复值
	SET_ITEM_BASE_PRO (this,luck	,E_ATTR_LUCK );				//	E_ATTR_LUCK		    //幸运
	SET_ITEM_BASE_PRO (this,att_hem	,E_ATTR_ATT_HEM );			//	E_ATTR_ATT_HEM		//攻击吸血
	SET_ITEM_BASE_PRO (this,att_mana	,E_ATTR_ATT_MANA );		//	E_ATTR_ATT_MANA	    //攻击吸蓝
	SET_ITEM_BASE_PRO (this,equ_drop	,E_ATTR_EQU_DROP );		//	E_ATTR_EQU_DROP	    //装备掉率
	SET_ITEM_BASE_PRO (this,money_drop	,E_ATTR_MONEY_DROP );	//	E_ATTR_MONEY_DROP	//金币掉率
	SET_ITEM_BASE_PRO (this,luck_def,E_LUCK_DEF );				//	E_LUCK_DEF			//幸运防御
}

//// 装备洗练
//bool ITEM_CACHE::isequipResetAdd(DWORD& value)
//{
	//value = 0;
	//if (dwGetStrengthStoneNum < 5 )
	//{
	//	if (BASE::randHit(50,100))
	//	{
	//		value = BASE::randInter(1,10);
	//		return true;
	//	}
	//	else
	//	{
	//		value = BASE::randInter(1,10);
	//		return false;
	//	}
	//}
	//else if (dwGetStrengthStoneNum < 10)
	//{
	//	if (BASE::randHit(60,100))
	//	{
	//		value = BASE::randInter(1,10);
	//		return true;
	//	}
	//	else
	//	{
	//		value = BASE::randInter(1,10);
	//		return false;
	//	}
	//}
	//else
	//{
	//	if (BASE::randHit(70,100))
	//	{
	//		value = BASE::randInter(1,10);
	//		return true;
	//	}
	//	else
	//	{
	//		value = BASE::randInter(1,10);
	//		return false;
	//	}
	//}

	//return true;
//}

// 获得产出的熔炼值
DWORD ITEM_CACHE::getOutputSmeltValue()
{
	static int smeltvalues[8] = { 0 ,5, 10, 15, 30, 60, 0, 0 };
	return smeltvalues[this->byQuality];
}

// 获得强化石数量
DWORD ITEM_CACHE::getOutputStrengthenStoneCount()
{
	DWORD result = 0;
	DWORD dwLevel = (DWORD)this->byStrengthLv;
	if (this->byStrengthLv)
	{
		result += DWORD((float)( this->dwGetStrengthStoneNum ) * 0.7);
	}

	//10级（包括10级）以上装备才有概率获得强化材料
	if (itemCfg->level_limit < 10)
	{
		return result;
	}

	if (this->byQuality == 1)
	{
		if (BASE::randHit(5,100))
		{
			result += 1;
		}
	}
	else if (this->byQuality == 2)
	{
		if (BASE::randHit(10,100))
		{
			result += 1;
		}
	}
	else if (this->byQuality == 3)
	{
		if (BASE::randHit(15,100))
		{
			result += 1;
		}
	}
	else if (this->byQuality == 4)
	{
		if (BASE::randHit(30,100))
		{
			result += 1;
		}
	}
	else if (this->byQuality == 5)
	{
		result += 1;
	}

	return result;
}

// 获得套装碎片数量
DWORD ITEM_CACHE::getOutputSuitPatchCount()
{
	//15级（包括15级）以上装备才有概率获得套装碎片。
	if (itemCfg->level_limit < 15)
	{
		return 0;
	}

	if (this->byQuality == 4)
	{
		if (BASE::randHit(20,100))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (this->byQuality == 5)
	{
		return 1;
	}

	return 0;
}


static DWORD getDivineQualityExp(DWORD quality)
{
	/*
	符文品质对应提供的品质经验如下：
	品质=3   品质经验=20（提供20经验）
	品质=4   品质经验=40（提供40经验）
	品质=5   品质经验=80（提供80经验）
	品质=2   品质经验=0
	品质=1   品质经验=0
	*/
	if (3 == quality)
	{
		return 20;
	}
	else if (4 == quality)
	{
		return 40;
	}
	else if (5 == quality)
	{
		return 80;
	}

	return 0;
}

// 获得符文经验
DWORD ITEM_CACHE::getDivineExp()
{
	if (!IS_DIVINE(itemCfg))
	{
		return 0;
	}

	if (byStrengthLv == 0)
	{
		byStrengthLv = 1;
	}

	DWORD quality_exp = getDivineQualityExp(itemCfg->rare); // 品质经验
	DWORD level_exp  = CONFIG::sumRuneExp(itemCfg->id, (WORD)byStrengthLv);
	DWORD cur_exp = dwFightValue;

	return quality_exp + level_exp + cur_exp;
}

// 获得产出的品质值
DWORD ITEM_CACHE::getOutputQualityValue()
{
	static int qualityvalues[8] = { 0 ,1, 5, 10, 20, 40, 0, 0 };
	return qualityvalues[this->byQuality];
}

static void addBetween(DWORD& src_value, DWORD value, DWORD min_value, DWORD max_value)
{
	src_value += value;
	if (src_value < min_value)
	{
		src_value = min_value;
	}
	else if (src_value > max_value)
	{
		src_value = max_value;
	}
}

static void subBetween(DWORD& src_value, DWORD value, DWORD min_value, DWORD max_value)
{
	if (src_value >= value)
	{
		src_value -= value;
	}
	else
	{
		src_value = 0;
	}

	if (src_value < min_value)
	{
		src_value = min_value;
	}
	else if (src_value > max_value)
	{
		src_value = max_value;
	}
}

// 装备洗练
void ITEM_CACHE::equipReset( const BYTE reset_type )
{
	for ( int i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++ )
	{
		if (attrOldValueExtra[ i ].attr != 0)
		{
			int min_value = 0;
			int max_value = 0;

			//30%概率，在-6~-2之间随机		
			//20%概率，在-1~1之间随机		
			//50%概率，在2~6之间随机		
			if( reset_type == BYTE( e_reset_type::coin_type ) )
			{
				DWORD dwRand = BASE::randTo( 100 );
				if ( dwRand <= 30 )
				{
					min_value = -6;
					max_value = -2;
				}
				else if ( dwRand <= 50 )
				{
					min_value = -1;
					max_value = 1;
				}
				else
				{
					min_value = 2;
					max_value = 6;
				}
			}
			else if ( reset_type == BYTE( e_reset_type::stone_type ) )
			{
				min_value = 0;
				max_value = 8;
			}
			else
			{
				min_value = 5;
				max_value = 12;
			}


			int addValue = BASE::randInter( min_value, max_value );

			ADDUP( attrOldValueExtra[i].value, addValue );

			DWORD dwMinvalue = 1;
			DWORD dwMaxValue = this->itemCfg->mapBaseAttrValue[ attrOldValueExtra[ i ].attr ];
			if (attrOldValueExtra[i].value < dwMinvalue )
			{
				attrOldValueExtra[i].value = dwMinvalue;
			}


			if ( attrOldValueExtra[i].value > dwMaxValue )
			{
				attrOldValueExtra[i].value = dwMaxValue;
			}
		}
	}

	//dwGetStrengthStoneNum++;

	//calEquipFightValue();
}

DWORD ITEM_CACHE::getExtraNums()
{
	if ( !IS_EQUIP( itemCfg ) )
		return 0;

	int nums = 0;
	
	for ( int i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++ )
	{
		if (attrValueExtra[ i ].attr != 0)
		{
			nums += 1;
		}
	}

	return nums;
}

void ITEM_CACHE::resetItemAttrValue()//属性
{
	resetAttrValueBase();
	resetEquipExtraPro();
	calEquipFightValue();
}

void ITEM_CACHE::resetAttrValueBase()//不会出现一级属性（额外属性）
{
	//配置表中基础属性
	setItemBasePro( );

	//强化加成,额外加成
	for ( int j=0; j<MAX_EQUIP_BASE_ATTR_NUMS; j++)
	{
		BYTE attr = attrValueBase[ j ].attr;
		if ( !IS_VALID_ATTR( attr  ) )
			continue;
		ADDUP_STRENGTH( attr, this->itemCfg->attType, attrValueBase[ j ].value, this->itemCfg->attValue * this->byStrengthLv );
	}
}

//注意：
//	此处的一级属性换算战力，仅用于装备评分。
//	人物的一级属性换算为二级属性，已经通过二级属性来计算战力。
//	所以人物的一级属性不计算战力。
static float attItemRatio[ E_ATTR_MAX ]={0.0f,0.0f,0.2f,0.0f,1.0f,2.0f,0.0f,2.0f,0.0f,0.0f,0.0f,5.0f,0.0f,2.0f,0.0f,5.0f,0.0f,0.0f,0.0f,0.3f,0.3f,5.0f,4.012f,4.0f,4.0f,4.2f,2.0024f,2.5f,2.5f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,50.0f,50.0f};

/*
@武器的战斗力 属性值除以系数和再相加
*/
void ITEM_CACHE::calEquipFightValue( )
{
	float fValue = 0.0 ;

	for ( size_t i=0; i<MAX_EQUIP_BASE_ATTR_NUMS; i++)
	{
		fValue += float( attrValueBase[i].value * attItemRatio[ attrValueBase[i].attr ] );
	}

	for ( size_t i=0; i<MAX_EQUIP_EXTRA_ATTR_NUMS; i++)
	{
		fValue += float( attrValueExtra[i].value * attItemRatio[ attrValueExtra[i].attr ] );
	}

	this->dwFightValue = (DWORD)fValue;
}

// 镶嵌宝石
bool ITEM_CACHE::injectEquipGem(BYTE pos, WORD gemid)
{
	if (0==pos || pos>MAX_HOLE_NUMS)
	{
		return false;
	}

	if (!isHoleOpen(pos))
	{
		return false;
	}
	
	wStone[pos-1] = gemid;

	resetEquipExtraPro();

	calEquipFightValue();

	return true;
}

// 获得孔位镶嵌的宝石
WORD ITEM_CACHE::getInjectedGem(BYTE pos)
{
	if (0==pos || pos>MAX_HOLE_NUMS)
	{
		return 0;
	}

	return wStone[pos-1];
}

// 卸载宝石
void ITEM_CACHE::unInjectGem(BYTE pos)
{
	if (0==pos || pos>MAX_HOLE_NUMS)
	{
		return;
	}

	wStone[pos-1] = 0;

	resetEquipExtraPro();

	calEquipFightValue();
}

// 获得所有宝石
std::vector<WORD> ITEM_CACHE::getAllInjectGems()
{
	std::vector<WORD> vecItems;
	for(BYTE i=0; i<MAX_HOLE_NUMS; i++)
	{
		WORD itemid = wStone[i];
		if (itemid==0)
		{
			continue;
		}
		vecItems.push_back(itemid);
	}
	
	return vecItems;
}


// 卸载所有宝石
void ITEM_CACHE::unInjectAllGems()
{
	for (BYTE i=0; i<MAX_HOLE_NUMS; i++)
	{
		wStone[i] = 0;
	}
}

// 孔位是否已满
bool ITEM_CACHE::isHoleFull()
{
	if (byHole >= MAX_HOLE_NUMS)
	{
		return true;
	}
	
	return false;
}

DWORD ITEM_CACHE::PuchHoleStoneCount()
{
	static DWORD STONE_COUNT[MAX_HOLE_NUMS] = {1,2,4,10,30};
	if (byHole >= MAX_HOLE_NUMS)
	{
		return 0;
	}

	return STONE_COUNT[byHole];
}

// 打孔成功
void ITEM_CACHE::PuchOneHole()
{
	if (!isHoleFull())
	{
		byHole++;
	}
}

// 孔位是否已开启
bool ITEM_CACHE::isHoleOpen(BYTE pos)
{
	if (0 == byHole)
	{
		return false;
	}
	
	return pos <= byHole;
}

// 位置是否已镶嵌
bool ITEM_CACHE::isHoleInjected(BYTE pos)
{
	if (0==pos || pos>MAX_HOLE_NUMS)
	{
		return true;
	}
	
	if (wStone[pos-1] != 0)
	{
		return true;
	}

	return false;
}

// 是否已经镶嵌同类型宝石
bool ITEM_CACHE::hasInjectedSameTypeGem(WORD srcitemid)
{
	CONFIG::ITEM_CFG* srcItem = CONFIG::getItemCfg(srcitemid);
	if ( srcItem == NULL )
		return true;

	for (BYTE i=0; i<MAX_HOLE_NUMS; i++)
	{
		WORD destItemid = wStone[i];
		CONFIG::ITEM_CFG* destItem = CONFIG::getItemCfg(destItemid);
		if ( destItem == NULL )
			continue;

		if (destItem->type==srcItem->type && destItem->sub_type == srcItem->sub_type)
		{
			return true;
		}
	}
	
	return false;
}

DWORD MON::getMonFightValue( CONFIG::MONSTER_CFG* moncfg )//monsterCfg
{
//#define MONFIGHTATTR( attr, attrtype)  (float)( (attr)/(attRe[attrtype]) )

	float fValue = 0;/*MONFIGHTATTR(moncfg->hp_max,E_ATTR_HP_MAX) + MONFIGHTATTR(moncfg->mp_max,E_ATTR_MP) + MONFIGHTATTR(moncfg->atk_min,E_ATTR_ATK_MIN) + MONFIGHTATTR(moncfg->atk_max,E_ATTR_ATK_MAX)\
				   + MONFIGHTATTR(moncfg->mag_min,E_ATTR_MAG_MIN) + MONFIGHTATTR(moncfg->magdef_max,E_ATTR_MAG_MAX) + MONFIGHTATTR(moncfg->sol_min,E_ATTR_SOL_MIN) + MONFIGHTATTR(moncfg->sol_max,E_ATTR_SOL_MAX)\
				   + MONFIGHTATTR(moncfg->phydef_max,E_ATTR_PHYDEF_MAX) + MONFIGHTATTR(moncfg->phydef_min,E_ATTR_PHYDEF_MIN) + MONFIGHTATTR(moncfg->magdef_max,E_ATTR_MAGDEF_MAX)+ MONFIGHTATTR(moncfg->magdef_min,E_ATTR_MAGDEF_MIN)\
				   + MONFIGHTATTR(moncfg->phy_dodge,E_ATTR_PHY_DODGE) + MONFIGHTATTR(moncfg->mag_dodge,E_ATTR_MAG_DODGE) + MONFIGHTATTR(moncfg->sol_dodge,E_ATTR_SOL_DODGE) \
				   + MONFIGHTATTR(moncfg->speed,E_ATTR_SPEED) + MONFIGHTATTR(moncfg->hp_rec,E_ATTR_HP_REC) + MONFIGHTATTR(moncfg->mp_rec,E_ATTR_MP_REC) + MONFIGHTATTR(moncfg->phy_hit,E_ATTR_PHY_HIT);*/

	return (DWORD)fValue;
}
