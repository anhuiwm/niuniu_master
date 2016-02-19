#ifndef __COMMON_H__
#define __COMMON_H__

#include "typedef.h"
#include "config.h"

class CMercenary;

#pragma pack(push, 1)


//创建角色

struct PlayerInfo
{
	DWORD _playerId; //角色ID
	DWORD _coin;//金币, 
	DWORD _ingot;//元宝, 

	DWORD _curExp;// 当前经验, 
	DWORD _maxExp;// 最大经验, 

	BYTE _job; //角色职业
	BYTE _sex; //角色性别
	BYTE _level;// 角色等级, 
	BYTE _vip;//角色VIP等级, 

	char _name[22]; //角色名称

	BYTE _curMap; //当前所处地图场景
	BYTE _maxMap; //最大可通关地图ID, 

	BYTE _suite; //当前新手引导步骤 
	BYTE _lucky; //幸运值;, 
	BYTE _luckyDef;//幸运防御, _power; //体力(保留), 
	BYTE _titleId; //当前使用的称号Id, 

	WORD _stamina; //耐力, 
	WORD _strength; //力量, 
	WORD _agility; //敏捷, 
	WORD _intellect; //智慧, 
	WORD _spirit; //精神,  

	BYTE _curEquipAward; //当前装备奖励领取进度
	BYTE _rechargeState; //首充奖励状体.0:未充值;1:已充值未领取;2:已领取

	DWORD _warPower; //战力值, 
	DWORD _maxHP;// 最大血量, 
	DWORD _maxMP;// 最大魔法, 

	DWORD _atkMax;	//物理攻击上限, 
	DWORD _atkMin;	//物理攻击下限, 

	DWORD _critVal;	//( 暴击值 ), DWORD _solAtkMax;	//最大道术攻击, 
	DWORD _critDef;//( 暴击抵抗 ), DWORD _solAtkMin;	//最小道术攻击, 

	DWORD _critAppend; //( 暴击伤害增加值 ),DWORD _magDodge; //魔法闪避(保留), 
	DWORD _critRelief; //( 暴击伤害减免值 ),DWORD _solDodge; //道术闪避(保留), 

	DWORD _defMax;	//最大物理防御, 
	DWORD _defMin;	//最小物理防御, 

	DWORD _phyDodge; //闪避, 
	DWORD _recHP; //生命回复, 
	DWORD _recMP; //魔法回复, 
	DWORD _phyHit; //物理命中率, 

	WORD _suckHP;//攻击吸血, DWORD _curHP;//保留(二级), 
	WORD _suckMP;//攻击吸蓝, DWORD _curHP;//保留(二级), 
	WORD _equipDrop; //装备掉率, DWORD _curMP;// 保留(二级), 
	WORD _coinDrop; //金币掉率, DWORD _curMP;// 保留(二级), 

	DWORD _magAtkMax;	//最大技能攻击(二级), 
	DWORD _magAtkMin;	//最小技能攻击(二级), 

	DWORD _magDefMax; //最大技能防御(二级), 
	DWORD _magDefMin; //最小技能防御(二级), 

	WORD _skillId[10];//技能列表, 目前最多 8个, 2个预留
	BYTE _fightSkill[4];//战斗技能索引，指向_skillId[10]中1-10位置，0表示没有

	WORD _teammate[3]; //上阵佣兵ID组, 最多3个

	DWORD _factionId; //公会Id:为0没有加入公会,  
	BYTE _factionJob;//公会职位,  
	DWORD _factionDistribute; //帮贡(保留),  

	DWORD _atkSpeed;//攻击速度(保留), 

	DWORD _repution; //声望(保留),  
	DWORD _smeltVar; //熔炼值(保留), 

	DWORD _vipVar; //累计充值, 

	PlayerInfo() { memset(this, 0, sizeof(*this) ); };

	PlayerInfo(ROLE* role);
};

//普通道具

struct ItemBriefInfo
{
	DWORD dwId; //道具唯一ID
	WORD wIndex; //道具索引
	BYTE byEquipLeftRight:1; //1:left 0:right, (预留字段)
	BYTE byExtraAttrNums:2; //额外属性条数, (预留字段)
	BYTE byStatus:5; //道具状态信息, (预留字段)
	BYTE byRes;	//预留字段
	DWORD dwCount; //道具叠加个数
	BYTE quality:4; //道具品质
	BYTE byHole:4; //已经开的孔数量
	BYTE byStrengthLv; //强化等级
	BYTE byLucky;	//装备幸运值
	BYTE byLuckyDef;	//装备幸运值防御
	DWORD dwPower; //装备战力
	WORD wStone[5]; //宝石镶嵌孔,宝石ID; 0:未镶嵌宝石
	WORD wAttr[5]; //五项基本属性,


	//DWORD dwId; //道具唯一ID
	//WORD wIndex; //道具索引

	//BYTE byEquipLeftRight:1; //1:left 0:right, (预留字段)

	//BYTE byExtraAttrNums:2; //额外属性条数, (预留字段)

	//BYTE byStatus:5; //道具状态信息, (预留字段)

	//DWORD dwCount; //道具叠加个数

	//BYTE quality:4; //道具品质
	//BYTE byRes:4; //已经开的孔数量
	//BYTE byStrengthLv; //强化等级 // 符文等级
	//BYTE byLucky;	//装备幸运值
	//DWORD dwPower; //装备战力 //符文经验
	//WORD wStone[MAX_HOLE_NUMS]; //宝石镶嵌孔,宝石ID; 0:未镶嵌宝石

	ZERO_CONS(ItemBriefInfo)

	ItemBriefInfo(ITEM_CACHE* itemCache);
};


struct MercenaryInfo
{
	WORD wIndex; //佣兵索引
	BYTE byLevel;  //佣兵等级
	BYTE byStar;	//当前星级

	DWORD dwPower; //战力

	DWORD dwMaxHP; //最大生命
	DWORD dwMaxMP; //最大魔法

	DWORD dwAtkMin; //最小攻击
	DWORD dwAtkMax; //最大攻击

	DWORD dwMagMin; //最小技能攻击
	DWORD dwMagMax; //最大技能攻击

	DWORD dwAtkDefMin; //最小防御
	DWORD dwAtkDefMax; //最大防御

	DWORD dwMagDefMin; //最小技能防御
	DWORD dwMagDefMax; //最大技能防御

	WORD wTamina; //耐力, 
	WORD wStrength; //力量, 
	WORD wAgility; //敏捷, 
	WORD wIntellect; //智慧, 
	WORD wIpirit; //精神,  
	BYTE byStatus; //佣兵状态位:0未解锁, 1:上阵, 2:未上阵
	BYTE byResVal; //保留字节
	//技能列表
	WORD vSkills[4]; //技能列表,只显示正在使用的.没有填写为0

	MercenaryInfo( CMercenary* mercenary);
};

#pragma pack(pop)


#endif		//__COMMON_H__
