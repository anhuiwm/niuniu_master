#ifndef __CONFIG_H__
#define __CONFIG_H__


#include "base.h"
#include "typedef.h"
#include "logIntf.h"
#include "../logdefine/logDefine.hpp"
#include <string>
#include <map>
#include <tuple>
#include <numeric>
#include <list>
#include <iostream>
using namespace std;
class ROLE;

#define getConfigDir()		( string( getProgDir() )+ STR_FSEP + "config" + STR_FSEP )

#define DECLARE_CLIENT_FUNC(func)		int func( ROLE* role, unsigned char * data, size_t dataLen )
#define  LEVELINTERVAL  7
#define  COST_TIMES   10
#define  BASE_ATTR_NUM 5
#define MAX_VIP_LEVEL	10


enum PKG_TYPE
{
	PKG_TYPE_NONE = 0,
	PKG_TYPE_BODY = 1,//身上
	PKG_TYPE_EQUIP = 2,//装备
	//PKG_TYPE_STORAGE = 3,
	//PKG_TYPE_MINE = 4,
	PKG_TYPE_ITEM = 5,// --普通道具包
	PKG_TYPE_FUWEN = 6, // 符文背包
	PKG_TYPE_SLAVE = 7, //佣兵装备包裹
};

enum MONEY_TYPE
{
	E_GOLD_MONEY = 1,//元宝 充值币
	E_COIN_MONEY = 2,//金币 游戏币
};

enum E_JOB
{
	E_JOB_WARRIOR = 1,//战士
	E_JOB_MAGIC = 2,//法师
	E_JOB_MONK = 3,//道士
};

enum 
{
	E_SEX_MALE = 0,		//男
	E_SEX_FEMALE = 1,	//女
};

enum
{
	E_NO_HOLE = 0,		//无孔
	E_HAVE_HOLE = 1,	//有孔
};

enum
{
	E_ATTR_ADD = 1,	       //加
	E_ATTR_REDUCE = 2,		//减
};

enum
{
	E_MYSTERY_SHOP_COMMON = 1,	    //神秘商城普通
	E_MYSTERY_SHOP_HIGN = 2,		//神秘商城高级

	E_MYSTERY_SHOP_ONE = 1,	    //1
	E_MYSTERY_SHOP_TEN = 2,		//10
};

enum SKILL_TYPE
{
	//技能的类型：1攻击类技能,2治疗类技能,3附加效果类技能,4普通攻击,5攻击吸蓝
	E_SKILL_TYPE_ATTACK = 1,
	E_SKILL_TYPE_TREAT = 2,
	E_SKILL_TYPE_EFFECT = 3,
	E_SKILL_TYPE_GENERAL = 4,
	E_SKILL_TYPE_LESSMP = 5,

	//技能的类型：1攻击、2召唤、3被动、4治疗 //wm  giveup 
	//E_SKILL_TYPE_ATTACK = 1,
	E_SKILL_TYPE_SUMMON = 2,
	E_SKILL_TYPE_PASSIVE = 3,
	//E_SKILL_TYPE_TREAT = 4,


	//技能额外效果类型
	E_SKILL_EFFECT_POFANG  = 1 ,//	破防
	E_SKILL_EFFECT_MABI  = 2,	//麻痹
	E_SKILL_EFFECT_CHENGMO  = 3,	//沉默
	E_SKILL_EFFECT_KUANGBAO  = 4,	//狂暴
	E_SKILL_EFFECT_QIANNENG  = 5,	//无力
	E_SKILL_EFFECT_ZHUOSAO  = 6,	//灼烧
	E_SKILL_EFFECT_XUANYUN  = 7,	//诱惑
	E_SKILL_EFFECT_XURUO  = 8,	//虚弱
	E_SKILL_EFFECT_MOFADUN  = 9,	//魔法盾
	E_SKILL_EFFECT_SAOLAN  = 10,//	烧蓝
	E_SKILL_EFFECT_ZHONGDU  = 11,//	中毒
	E_SKILL_EFFECT_SHUFU  = 12,//	束缚
	E_SKILL_EFFECT_MONATTACKTWO  = 13,//	神兽2次攻击
	E_SKILL_EFFECT_ZHENQI  = 14,//	真气
	E_SKILL_EFFECT_CHUNFENG  = 15,//	春风
	E_SKILL_EFFECT_MAX  = 16,
};

enum E_QUALITY
{
	E_QUALITY_WHITE = 1,   // 白色
	E_QUALITY_BLUE = 2,  // 蓝色
	E_QUALITY_GOLD = 3, // 金色
	E_QUALITY_DARK_GOLD = 4, // 暗金色 
	E_QUALITY_GREEN = 5, //绿色
	E_QUALITY_ORANGE = 6,//橙色
	E_QUALITY_RED = 7, // 红色
};

//包裹扩展
enum E_PKG_EXTEND
{

//1背包，2仓库，3矿石背包			
	E_PKG_EXTEND_PACK = 1,
	E_PKG_EXTEND_STORAGE = 2,
	E_PKG_EXTEND_MINE = 3,
	E_PKG_EXTEND_UTILITY = 2,
};

enum eDROPTYPE
{
	E_DROP_MON = 0,//普通怪物掉落
	E_DROP_FIRST_BOSS = 1,
	E_DROP_BOSS = 2,
	E_DROP_FIRST_HERO = 3,
	E_DROP_HERO = 4,
	E_DROP_NONE = 5,
	//E_DROP_HERO = 4,
};

#define MAX_RARE_LEVEL		(7)

//#define USE_EXTERN_CONFIG	\
//	extern map<DWORD, CONFIG::ATTR_UPGRADE_CFG>& mapAttrUpgradeCfg;\
//	extern map<WORD, CONFIG::MONSTER_CFG>& mapMonsterCfg;\
//	extern map<WORD, CONFIG::SKILL_CFG2>& mapSkillCfg;\
//	extern map<WORD, CONFIG::ROLE_EXP_CFG>& mapExpCfg;\
//	extern map<WORD, CONFIG::ACTIVITY_INFO_CFG> &mapActivityInfoCfg;\
//	extern map<WORD, CONFIG::MISSION_CFG> &mapMissionCfg;

#define USE_EXTERN_CONFIG	\
	extern map<DWORD, CONFIG::ATTR_UPGRADE_CFG>& mapAttrUpgradeCfg;\
	extern map<WORD, CONFIG::MONSTER_CFG>& mapMonsterCfg;\
	extern map<WORD, CONFIG::SKILL_CFG2>& mapSkillCfg;\
	extern map<WORD, CONFIG::ROLE_EXP_CFG>& mapExpCfg;\
	extern map<WORD, CONFIG::ACTIVITY_INFO_CFG>& mapActivityInfoCfg;\
	extern map<WORD, CONFIG::MISSION_CFG>& mapMissionCfg;\
	extern map<WORD, CONFIG::OPEN_ACTIVITY>& mapOpenActivityCfg;


#define getComposeCfgBy3(times, sub_type, job_limit)			\
	CONFIG::getComposeCfg( ( ((WORD )(times)) << 16) | ( ((BYTE)(sub_type)) << 8) | (BYTE)(job_limit) )

#define getComposeNowExpBy3(times, sub_type, job_limit)			\
	CONFIG::getComposeNowExpCfg( ( ((WORD )(times)) << 16) | ( ((BYTE)(sub_type)) << 8) | (BYTE)(job_limit) )

#define getComposeMaxExpBy2(sub_type, job_limit)			\
	CONFIG::getComposeMaxExpCfg( ( ((BYTE)(sub_type)) << 8) | (BYTE)(job_limit) )

#define getGuildBuildCfg2(type, level)			\
	CONFIG::getGuildBuildCfg( ( ((BYTE)(type)) << 8) | (BYTE)(level) )

#define getShop1Cfg2( job, level)			\
	CONFIG::getShop1Cfg( (BYTE)(job) << 16 | (WORD)(level) )
#define getVipShop1Cfg2(job, level)			\
	CONFIG::getVipShop1Cfg( (BYTE)(job) << 16 | (WORD)(level) )

#define getSignCfg4( job, sex, month, day)			\
	CONFIG::getSignCfg( (BYTE)(job) << 24 | (BYTE)(sex) << 16 | (BYTE)(month) << 8 | (BYTE)(day) )

#define getSkillEffCfg2( type, level )			\
	CONFIG::getSkillEffCfg( (BYTE)(type) << 8 | (BYTE)(level) )

#define getMercenaryCfg2( job, level )			\
	CONFIG::getMercenaryCfg( (BYTE)(job) << 16 | (WORD)(level) )


#define ONCE_ROLL_BASE		(10000)		//单roll基数

#define MALLOC_MESSAGE_CREAT(LOG_DATA, LOG_TYPE) \
	LOG_DATA* pLog = (LOG_DATA*)malloc( sizeof(LOG_DATA) );\
	memset( pLog, 0, sizeof(LOG_DATA) );\
	pLog->wLen = sizeof(LOG_DATA);\
	pLog->wCmdType = LOG_TYPE;\
	pLog->eventID = GET_EVENT_ID(); 

#define MALLOC_MESSAGE_SEND() \
	LogIntf::LOG_MESSAGE message(pLog);\
	LOGINTF->send(message);

#define  NAME_MAX_LEN   22

enum TASK_TYPE
{
	//任务类型:
	E_TASK_LEVEL = 1,					//1玩家等级，				
	E_TASK_KILL_MON = 2,				//2杀怪，
	E_TASK_HAS_EQUIP_ENHANCE = 3,		//3武器强化，
	E_TASK_HAS_EQUIP_RARE = 4,			//4装备稀有度，
	E_TASK_HAS_EQUIP_COMPOSE = 5,		//5装备合成，
	E_TASK_TITLE = 6,					//6拥有称号，
	E_TASK_VIP_LEVEL = 7,				//7VIP等级，
	E_TASK_BUY_QUICK_FIGHT = 8,			//8快速战斗购买次数，
	E_TASK_SPECIAL_TASK = 9,			//9完成指定任务，
	E_TASK_DO_EQUIP_ENHANCE = 10,		//10进行强化操作，
	E_TASK_DO_EQUIP_COMPOSE = 11,		//11进行合成操作，
	E_TASK_FACTION_DONATE = 12,			//12进行公会捐献操作，
	E_TASK_EVENT = 13,					//13完成事件，
	E_TASK_ARENA = 14,					//14竞技场挑战，
	E_TASK_MINING = 15,					//15进行挖矿，
	E_TASK_FACTION_WAR = 16,			//16参加行会战,
	E_TASK_EQUIP = 17,					//17进行一次装备穿戴操作，
	E_TASK_USE_MONEYTREE = 18,			//18使用摇钱树，
	E_TASK_WORLD_BOSS = 19,				//19报名世界BOSS战斗
	E_TASK_FIGHT_BOSS = 20,				//20BOSS挑战
	E_TASK_SMELT = 21,					//21熔炼
	E_TASK_ALL_EQUIP_LEVEL = 22,		//22全身装备等级	玩家身上穿戴的装备均到达指定等级。格式：装备等级
	E_TASK_GET_GREEN_TIMES = 23,		//23获得绿装次数	玩家总共获得的绿装次数。格式：绿装获得的次数
	E_TASK_FORGE_EQUIP_TIMES = 24,		//24打造装备次数	玩家通过打造获得的装备次数。格式：打造的次数
	E_TASK_UNIJECT_DIAMOND = 25,		//25镶嵌宝石格数	玩家身上装备总共镶嵌的宝石格数。格式：身上装备总共的宝石数量。
	E_TASK_EQUIP_RESET_TIMES = 26,		//26洗炼装备次数	玩家洗炼装备的次数。格式：洗炼的次数
	E_TASK_EQUIP_LUCK = 27,				//27武器幸运的操作次数	玩家操作武器幸运的次数。格式：操作武器幸运的次数
	E_TASK_CLOTH_LUCK = 28,				//28衣服幸运的操作次数	玩家操作衣服幸运的次数。格式：操作衣服幸运的次数
	E_TASK_GET_SUIT_TIMES = 29,			//29激活套装属性	玩家身上装备激活的套装属性条目。格式：套装属性激活条目。
	E_TASK_BUY_FIGHT_BOSS_TIMES = 30,	//30购买boss挑战次数	购买挑战关卡boss的次数。格式：购买次数
	E_TASK_SHOP_BUY_TIMES = 31,			//31购买商城道具次数	购买商城道具的次数。格式：购买次数
	E_TASK_SET_FIGHT_SKILL_NUM = 32,	//32配置战斗技能	配置中的战斗技能个数。格式：技能配置的个数
	E_TASK_GET_MERCENARY_NUM = 33,		//33获得佣兵	激活佣兵的数量。格式：激活佣兵的数量。
	E_TASK_FIGHT_BOSS_TIMES = 34,		//34boss挑战	玩家挑战任意关卡boss。格式：挑战次数
	E_TASK_FIGHT_ELITE_TIMES = 35,		//35精英关卡boss挑战	玩家挑战任意精英关卡boss。格式：挑战次数
	E_TASK_RUNE_TIMES = 36,				//36符文占卜次数	玩家符文系统占卜的次数。格式：占卜次数
	E_TASK_MERCENARY_STAR = 37,			//37佣兵星级	玩家佣兵达到的最高星级。格式：佣兵星级
	E_TASK_VIP_LOGIN   = 38,            //38vip等级为X的玩家每日登录奖励	玩家VIP等级达到的级别。格式：VIP等级数值

};

enum OPEN_ACTIVITY_TYPE
{
	//运营活动类型 1登陆 2首次充值 3优惠购买 4消费返现
	E_ACTIVITY_LOGIN = 1,							
	E_ACTIVITY_FIRST_CHARGE = 2,				
	E_ACTIVITY_DISCOUNT_BUY = 3,		
	E_ACTIVITY_CASH_BACK = 4,			
};

struct SKILL_EFFECT
{
	//增减类型,属性编号,值,百分比;
	BYTE addType;     
	BYTE attr; 
	WORD value;	
	float coff;
	//ZERO_CONS(SKILL_EFFECT);

	SKILL_EFFECT( const SKILL_EFFECT& dst )
	{
		this->addType = dst.addType;
		this->attr = dst.attr;
		this->value = dst.value;
		this->coff = dst.coff;
	}

	SKILL_EFFECT& operator = ( const SKILL_EFFECT& dst )
	{
		if ( this == &dst )
		{
			return *this;
		}

		this->addType = dst.addType;
		this->attr = dst.attr;
		this->value = dst.value;
		this->coff = dst.coff;

		return *this;
	}

	SKILL_EFFECT( BYTE addType, BYTE attr, WORD value, float coff )
	{
		this->addType = addType;     
		this->attr = attr; 
		this->value = value;	
		this->coff = coff / 100.0f;
	}
};

struct ITEM_INFO
{
	WORD itemIndex;
	DWORD itemNums;

	BYTE byQuality;

	BYTE byUseChest:1;
	BYTE byChestRandNums:7;


	ITEM_INFO(WORD itemIndex, DWORD itemNums, BYTE byQuality=0)//=0, BYTE byChest=0)
	{
		this->itemIndex = itemIndex;
		this->itemNums = itemNums;

		this->byQuality = byQuality;
		
		this->byUseChest = 0;
		this->byChestRandNums = 0;

//		this->byUseChest = byChest>> 7;				//高1位
//		this->byChestRandNums = byChest & 127;		//低7位

	}

	ZERO_CONS(ITEM_INFO)

};

#pragma pack(push, 1)
struct AttrValue
{
	BYTE attr;
	DWORD value;

	ZERO_CONS(AttrValue)
	AttrValue(BYTE attr, DWORD value)
	{
		this->attr = attr;
		this->value = value;
	}
};

struct SHOP_ITEM
{
	WORD wID;
	WORD wItemID;
	WORD wItemNums;
	DWORD dwItemPrice;
	BYTE byMoneyType;
	BYTE byDiscount;
	WORD wQuality;//现在为品质
	ZERO_CONS(SHOP_ITEM);

	SHOP_ITEM(WORD wID,WORD wItemID,DWORD dwItemPrice, WORD wItemNums,BYTE byMoneyType, BYTE byDiscount,WORD wQuality)
	{
		this->wID  = wID;
		this->wItemID  = wItemID;
		this->wItemNums  = wItemNums;
		this->dwItemPrice  = dwItemPrice;
		this->byMoneyType  = byMoneyType;
		this->byDiscount  = byDiscount;	
		this->wQuality  = wQuality;	
	}

	bool operator == (const WORD id)const
	{
		return (this->wID == id);
	}

	SHOP_ITEM& operator = (const SHOP_ITEM& dst)
	{
		this->wID  = dst.wID;
		this->wItemID  = dst.wItemID;
		this->wItemNums  = dst.wItemNums;
		this->dwItemPrice  = dst.dwItemPrice;
		this->byMoneyType  = dst.byMoneyType;
		this->byDiscount  = dst.byDiscount;
		return *this;
	}
};

struct DOUBLE_WORD 
{
	WORD wBase;//开放的
	WORD wExtra;//赢过的

	ZERO_CONS(DOUBLE_WORD);

	DOUBLE_WORD( WORD wBase, WORD wExtra )
	{
		this->wBase = wBase;
		this->wExtra = wExtra;
	}

	DOUBLE_WORD( WORD wBase )
	{
		this->wBase = wBase;
		wExtra = 0;
	}
};

#pragma pack(pop)


struct AttrDeform
{
	BYTE attr;
	float ratio;

	ZERO_CONS(AttrDeform);
	AttrDeform(BYTE attr, float ratio)
	{
		this->attr = attr;
		this->ratio = ratio;
	}
};


//struct MAP_CFG;

//struct MONSTER_DROP_PRE_MAKE
//{

//	WORD _wMapID;

//	MAP_CFG* mapCfg;

//};


class ROLE;
struct ITEM_CACHE;
namespace CONFIG
{

	enum
	{
		E_DROP_ROLL_SINGLE = 1, 	//
		E_DROP_ROLL_MULTI = 2, 		//

	};



	struct DROP_PARA
	{

		//DWORD groupType;		//掉落方式	group_type	int	
							//1每一组根据组概率各自掉落一次、(单roll)
							//2所有组的组概率作为权重值，从所有组中随机一个物品			掉落方式为1时，组概率为0-10000之间的整数，表示0%-100%

		WORD groupID;
		WORD itemIndex;
		vector<WORD> vecItem;
		WORD dropMin;
		WORD dropMax;

		vector<BYTE>  vecQuality;
		vector<DWORD>  vecQuaRand;

		WORD  qualityMin;
		WORD  qualityMax;
		
	};

	struct GROUP_INFO
	{
		vector<DWORD> vRandSum;
		vector<DROP_PARA> vDropPara;//key goroupid
	};

	struct DROP_INFO_CFG
	{
		DWORD dropID;
		WORD groupType;	
							//掉落方式	group_type	int	
							//1每一组根据组概率各自掉落一次、(单roll)
							//2所有组的组概率作为权重值，从所有组中随机一个物品	

		
		vector<DWORD> vRandSum;
		map< WORD, 	GROUP_INFO > mapDropGroup;//key goroupid
	};


//职业编号索引	等级	耐力	力量	敏捷	智慧	精神	最大生命值	攻击最大值	攻击最小值	防御最大值	防御最小值	技能攻击最大值	技能攻击最小值	技能防御最大值	技能防御最小值	最大魔法值	命中值	闪避值	暴击值	暴击抵抗值	暴击伤害增加值	暴击伤害减免值	每回合魔法恢复值	每回合生命恢复值	幸运	攻击吸血	攻击吸蓝	装备掉率	金币掉率  幸运防御

	struct ATTR_UPGRADE_CFG
	{
		WORD job_id        ;//职业编号索引
		WORD level		   ;//等级
		DWORD stamina	   ;//耐力
		DWORD strength	   ;//力量
		DWORD agility	   ;//敏捷
		DWORD intellect	   ;//智慧
		DWORD spirit		   ;//精神
		DWORD hp_max		   ;//最大生命值
		DWORD atk_max	   ;//攻击最大值
		DWORD atk_min	   ;//攻击最小值
		DWORD def_max	   ;//防御最大值
		DWORD def_min	   ;//防御最小值
		DWORD spellatk_max  ;//技能攻击最大值
		DWORD spellatk_min  ;//技能攻击最小值
		DWORD spelldef_max  ;//技能防御最大值
		DWORD spelldef_min  ;//技能防御最小值
		DWORD mp_max		   ;//最大魔法值
		DWORD hit		   ;//命中值
		DWORD dodge		   ;//闪避值
		DWORD crit		   ;//暴击值
		DWORD crit_res	   ;//暴击抵抗值
		DWORD crit_inc	   ;//暴击伤害增加值
		DWORD crit_red	   ;//暴击伤害减免值
		DWORD mp_rec		   ;//每回合魔法恢复值
		DWORD hp_rec		   ;//每回合生命恢复值
		DWORD luck		   ;//幸运
		DWORD att_hem	   ;//攻击吸血
		DWORD att_mana	   ;//攻击吸蓝
		DWORD equ_drop	   ;//装备掉率
		DWORD money_drop	   ;//金币掉率
		DWORD luck_def;//幸运防御
	};

//	编号	名称	美术资源	等级	技能	经验	金币	掉落索引	战斗力	耐力	力量	敏捷	智慧	精神	生命值上限	魔法值上限	攻击最大值	攻击最小值	防御最大值	防御最小值	技能攻击最大值	技能攻击最小值	技能防御最大值	技能防御最小值	命中值	闪避值	暴击值	暴击抵抗值	暴击伤害增加值	暴击伤害减免值	生命恢复	魔法恢复	幸运	攻击吸血	攻击吸蓝
//	id	name	asset	level	skill_id	exp	gold	drop_id	battle_point	stamina	strength	agility	intellect	spirit	hp_max	mp_max	atk_max	atk_min	def_max	def_min	spellatk_max	spellatk_min	spelldef_max	spelldef_min	hit	dodge	crit	crit_res	crit_inc	crit_red	hp_rec	mp_rec	luck	att_hem	att_mana

	struct MONSTER_CFG
	{
		WORD id;
		string name;
		WORD asset;
		WORD level;

		WORD skill_id;

		DWORD hp_max;
		DWORD mp_max;
		DWORD atk_max;
		DWORD atk_min;
		DWORD mag_max;
		DWORD mag_min;
		DWORD sol_max;
		DWORD sol_min;
		DWORD phydef_max;
		DWORD phydef_min;
		DWORD magdef_max;
		DWORD magdef_min;
		DWORD phy_hit;
		DWORD phy_dodge;
		DWORD mag_dodge;
		DWORD sol_dodge;
		DWORD speed;
		DWORD hp_rec;
		DWORD mp_rec;
		DWORD luck_def;
		DWORD stamina       ;
		DWORD strength		;
		DWORD agility		;
		DWORD intellect		;
		DWORD spirit		;
		//DWORD hp_max		;
		//DWORD mp_max		;
		//DWORD atk_max		;
		//DWORD atk_min		;
		DWORD def_max		;
		DWORD def_min		;
		DWORD spellatk_max	;
		DWORD spellatk_min	;
		DWORD spelldef_max	;
		DWORD spelldef_min	;
		DWORD hit			;
		DWORD dodge			;
		DWORD crit			;
		DWORD crit_res		;
		DWORD crit_inc		;
		DWORD crit_red		;
		//DWORD hp_rec		;
		//DWORD mp_rec		;
		DWORD luck			;
		DWORD att_hem		;
		DWORD att_mana		;
		DWORD exp;
		DWORD gold;

		WORD drop_id;
		DWORD dwFightValue;
	};



	struct SKILL_CFG2
	{
//id	name	job	group	level	unlock_level	study	description	type	parameter	mp_cost	cooldown	priority	levelup_exp	target	eff_rate	eff_type	round	parameter_add

		WORD id;  // 技能id
		string name; // 技能名称
		BYTE job; // 职业限制
		WORD group; // 技能组
		WORD level; // 技能等级
		WORD unlock_level; // 解锁需要角色等级
		BYTE study; // 学士方式，是否需要技能书 0不需要 1需要
		string description; //  技能描述
		BYTE type; //  技能类型
		vector< AttrValue > vecAttr;
		struct SKILL_PARA
		{
			BYTE skillType;     // 治疗/伤害类型
			BYTE skillTarget;   // 治疗/伤害目标
			WORD skillAtt;		// 值,
			float skillCoff;		// 系数
			WORD skillMpAtt;	// 蓝值,
			float skillMpCoff;	// 蓝系数
		}skillPara;

		WORD mp_cost; // 魔法消耗
		DWORD cooldown; // 释放技能间隔
		WORD  priority; // 技能释放优先级
		WORD levelup_exp;//经验增加时间（单位：分钟)
		BYTE target; // 技能特效目标 1自身，2目标
		float eff_rate; // 额外效果概率
		BYTE eff_type; // 额外效果类型
		BYTE round; // 效果持续回合

		float extra_coff;
		//vector< SKILL_EFFECT > vecEffectAttr;

		struct   SKILL_BOOK_ITEM
		{
			WORD itemid; // 技能书id
			WORD count; // 消耗数量
			SKILL_BOOK_ITEM()
			{
				itemid = 0;
				count = 0;
			}
		}skill_book;

	};

	struct SKILL_EFF_CFG
	{
		//编号	效果类型编号	效果等级	效果名称	效果参数
		//id	type	          level	name	parameter_add

		WORD id;
		string name;
		BYTE type;
		BYTE level;
		vector< SKILL_EFFECT > vecEffectAttr;
	};

//编号	名称	美术资源	类型	子类型	说明	堆叠数量	职业限制	性别限制	等级限制	稀有度	出售基础价格	属性随机	随机次数	属性随机出现概率	战斗力	耐力	力量	敏捷	智慧	精神	最大生命值	最大魔法值	攻击最大值	攻击最小值	防御最大值	防御最小值	技能攻击最大值	技能攻击最小值	技能防御最大值	技能防御最小值	命中值	闪避值	暴击值	暴击抵抗值	暴击伤害增加值	暴击伤害减免值	每回合魔法恢复值	每回合生命恢复值	幸运	攻击吸血	攻击吸蓝	装备掉率	金币掉率	幸运	强化	镶嵌孔概率	套装

	struct ITEM_CFG 
	{
		WORD         id;			
		string       name;		
		WORD         asset;		
		DWORD        type;		
		WORD         sub_type;	
		//string       description;//改至最后一列	
		WORD         count;		
		WORD         job_limit;	
		WORD		 sex_limit;
		WORD         level_limit;	
		DWORD        rare;		
		DWORD        price;
		DWORD        att_random;
		DWORD         hp_max;		
		DWORD         mp_max;		
		DWORD         atk_max;		
		DWORD         atk_min;		
		DWORD         mag_max;  //		
		DWORD         mag_min;	//	
		DWORD         sol_max;	//	
		DWORD         sol_min;	//	
		DWORD         phydef_max;//	
		//DWORD         phydef_min;//	
		DWORD         magdef_max;//	
		DWORD         magdef_min;//
		//DWORD         phy_hit;//		
		//DWORD         phy_dodge;//	
		//DWORD         mag_dodge;//	
		//DWORD         sol_dodge;//	
		//DWORD         speed;//		
		DWORD         hp_rec;		
		DWORD         mp_rec;	

		DWORD def_max	    ;
		DWORD def_min		;
		DWORD spellatk_max;
		DWORD spellatk_min;
		DWORD spelldef_max;
		DWORD spelldef_min;
		DWORD hit			;
		DWORD dodge		;
		DWORD crit		;
		DWORD crit_res	;
		DWORD crit_inc	;
		DWORD crit_red	;
		DWORD att_hem		;
		DWORD att_mana	;

		

		struct  ATT_TIMES
		{
			WORD min_times;
			WORD max_times;
			ATT_TIMES()
			{
				min_times = 0;
				max_times = 0;
			}
		}att_times;

		DWORD        att_rate;	//100表示100%，单件装备的每一次随机都需要进行概率检测
		WORD		 luck;
		BYTE	   addAttrType;
		DWORD      addAttrValue;
		//耐力	力量	敏捷	智慧	精神
		DWORD	stamina;
		DWORD	strength;
		DWORD	agility;
		DWORD	intellect;
		DWORD	spirit;
		DWORD   equ_drop;	
		DWORD   money_drop;

		//幸运	强化	镶嵌孔概率
		//luck_def	strengthen	hole_odds

		DWORD   luck_def;

		string  strengthen;

		BYTE attType;  // 属性类型
		DWORD attValue; // 属性值
		DWORD strengthen_stones; // 强化石数量

		BYTE zeroHole;
		vector<BYTE>  vecHole;
		vector<DWORD>  vecHoleRandSum;
		WORD suit;
		map< BYTE, DWORD >mapBaseAttrValue;//宝石添加的属性 以及 洗练最大属性用

		// equit extend
		class equip_extend_attr_config
		{
		public:
			byte type;
			vector<pair<float, float>> range;
		};
		typedef equip_extend_attr_config* equip_extend_attr_config_ptr;

		class equip_extend_star
		{
		public:
			byte type;
			vector<float> values;
		};
		typedef equip_extend_star* equip_extend_star_ptr;


		class equip_extend_config
		{
		public:
			vector<equip_extend_attr_config> extend_attrs;
			vector<equip_extend_attr_config> skill_attrs;
			equip_extend_star basic_attr;
			equip_extend_star forging_up_attr;// 锻造
		}equip_extend;

		ITEM_CFG()
		{
			id = 0;			
			asset = 0;		
			type = 0;		
			sub_type = 0;	
			count = 0;		
			job_limit = 0;	
			sex_limit = 0;
			level_limit = 0;	
			rare = 0;		
			price = 0;
			hp_max = 0;		
			mp_max = 0;		
			atk_max = 0;		
			atk_min = 0;		
			mag_max = 0; 		
			mag_min = 0;	
			sol_max = 0;	
			sol_min = 0;	
			phydef_max = 0;
			//phydef_min = 0;
			magdef_max = 0;
			magdef_min = 0;
			//phy_hit = 0;	
			//phy_dodge = 0;
			//mag_dodge = 0;
			//sol_dodge = 0;
			//speed = 0;
			hp_rec = 0;		
			mp_rec = 0;	
			def_max = 0;
			def_min = 0;
			spellatk_max = 0;
			spellatk_min = 0;
			spelldef_max = 0;
			spelldef_min = 0;
			hit = 0;
			dodge = 0;
			crit = 0;
			crit_res = 0;
			crit_inc = 0;
			crit_red = 0;
			att_hem = 0;
			att_mana = 0;
			att_random = 0;

			att_rate = 0;	//100表示100%，单件装备的每一次随机都需要进行概率检测
			luck = 0;
			//battle_point = 0;
			addAttrType = 0;
			addAttrValue = 0;

			//耐力	力量	敏捷	智慧	精神
			stamina = 0;
			strength = 0;
			agility = 0;
			intellect = 0;
			spirit = 0;
			equ_drop = 0;	
			money_drop = 0;

			//幸运	强化	镶嵌孔概率
			//luck_def	strengthen	hole_odds

			luck_def = 0;

			attType = 0;  // 属性类型
			attValue = 0; // 属性值
			strengthen_stones = 0; // 强化石数量

			zeroHole = 0;
			vecHole.clear();
			vecHoleRandSum.clear();
			suit = 0;
			mapBaseAttrValue.clear();
		}
	};

	struct PLAYER_CFG 
	{
		WORD	     id;			
		string	     name;		
		WORD	     level;		
		DWORD	     exp;		
		WORD	     asset;		
		DWORD	     money;		
		DWORD	     gold;		
		DWORD	     power;		
		DWORD	     power_limit;	
		DWORD	     lead;		
		DWORD	     lead_limit;	
		DWORD	     vip_level;	
	};

	//离线掉落获得
	struct DROP_EARNING
	{
		DWORD dwExp;
		DWORD dwCoin;
		ITEM_CACHE *ic;

		DROP_EARNING(DWORD dwExp, DWORD dwCoin, ITEM_CACHE *ic)
		{
			this->dwExp = dwExp;
			this->dwCoin = dwCoin;
			this->ic = ic;
		}
		
	};

	CONFIG::DROP_EARNING* getRandMonDrop( WORD wMapID );


	struct DROP_MONEY 
	{
		float moneyrate;
		DWORD minMoney;
		DWORD maxMoney;
		ZERO_CONS( DROP_MONEY );
		DROP_MONEY(float moneyrate, DWORD minMoney, DWORD maxMoney)
		{
			this->moneyrate = moneyrate;
			this->minMoney  = minMoney;
			this->maxMoney  = maxMoney;
		}
		DROP_MONEY& operator = ( const DROP_MONEY& drop )
		{
			this->moneyrate = drop.moneyrate;
			this->minMoney  = drop.minMoney;
			this->maxMoney  = drop.maxMoney;
			return *this;
		}

	};
	struct MAP_CFG 
	{
		//id	name	asset	level_limit	monster	number	event	cool_down	boss_id	gold	exp	drop_id	boss_drop1	boss_gold1	boss_drop2	boss_gold2	group_id
		WORD         id;			
		string       name;		
		WORD         asset;		
		WORD	     level_limit;

		vector<WORD> vecMonsterIndex;
		vector<DWORD> vecMonsterRandSum;
		
		vector<float> vecMonsterRatio;	//每种怪占比重,用于离线计算0.3....
		
		DWORD dwMin;
		DWORD dwMax;
		
		WORD	     event;		
		DWORD	     cool_down;		
		WORD	     boss_id;

		DROP_MONEY stDropMoney[ 3 ];//three type drop money
		WORD dropID[ 3 ];//three type drop id
		DWORD exp;
		WORD  group_id;
		BYTE  sub_order;
		size_t _curIndex;//可以不初始化
		vector<DROP_EARNING> _vecDropEarning;

		DROP_EARNING* getDropItemCache();
		void refreshMonsterDropLibrary( size_t dropTimes );

	};

	struct COMPOSE_CFG 
	{
//times	sub_type	job_limit	hp_max	mp_max	atk_max	atk_min	mag_max	mag_min	sol_max	sol_min	phydef_max	phydef_min	magdef_max	magdef_min
		WORD        times		;
		BYTE        sub_type	;
		BYTE        job_limit	;
		
		DWORD        hp_max		;
		DWORD        mp_max		;
		DWORD        atk_max		;
		DWORD        atk_min		;
		DWORD        mag_max		;
		DWORD        mag_min		;
		DWORD        sol_max		;
		DWORD        sol_min		;
		DWORD        phydef_max	;
		DWORD        phydef_min	;
		DWORD        magdef_max	;
		DWORD        magdef_min	;
		DWORD        exp;
	};

	struct ROLE_EXP_CFG
	{
		WORD level;
		DWORD upgrade_exp;	//升到下一级需要的经验

		DWORD sum_exp;		//前一等级累计经验
		
	};
	
	struct ATT_RAND_PARA
	{
		BYTE attr;
		DWORD valueMin;
		DWORD valueMax;
	};

	//struct ATT_RAND_GROUP
	//{
	//	WORD groupID;//mapRandGroup 的key
	//	WORD groupRate;//
	//	vector<DWORD> vRandSum;
	//	vector<ATT_RAND_PARA> vAttRandPara;
	//};

	struct ATT_RAND_CFG
	{
		WORD groupID;
		vector<ATT_RAND_PARA> vAttRandPara;

		//map<WORD, ATT_RAND_GROUP > mapRandGroup;		//key: group id
		//vector<DWORD> vRandSumGroup;						//使用这个确定用哪一组
	};

	struct GUILD_BUILD_CFG
	{
		//type	level	upgrade_exp	parameter
		BYTE type;
		BYTE level;
		DWORD upgrade_exp;
		//string parameter;
		union PARA
		{
			struct MIN_MAX
			{
				WORD min_value;
				WORD max_value;
			} min_max;
	
			WORD one_value;
		} union_para;
	};

	struct ARENA_BOUNS_CFG
	{	
		DWORD   rank;
		WORD    goldID;
		DWORD	goldNum;
		WORD	itemID;
		WORD    itemNum;
	};

	struct ARENA_REWARD_CFG
	{	
		DWORD rank;
		DWORD win_gold;
		DWORD win_money;
		DWORD lose_gold;
		DWORD lose_money;
	};

	struct SUIT_CFG
	{
//索引id	套装id	件数要求	1阶强化要求	1阶套装属性	2阶强化要求	2阶套装属性	3阶强化要求	3阶套装属性	4阶强化要求	4阶套装属性	5阶强化要求	5阶套装属性	套装前缀	套装后缀	套装说明

		WORD   ID				;
		WORD   suit_id			;
		WORD   suit_number		;
		
		map< WORD, vector< SKILL_EFFECT> > mapStrengthEffAttr;
	};

	struct SIGN_CFG
	{	
		BYTE    job;
		BYTE    sex;
		BYTE    month;
		BYTE    times;
		//BYTE	vip_bonus;
		WORD	item_id;
		DWORD    item_num;
	};

	struct VIP_ADDITION_CFG
	{	
		//等级	描述	升级经验	快速战斗购买次数	摇钱树次数	竞技场购买次数	BOSS挑战次数	快速战斗收益	最大占卜次数	商城额外物品栏	购买精英副本次数	符文槽额外开启	升级条件2	每日登录送元宝
		WORD    level		  ;
		string  description	  ;
		DWORD    upgrade_exp	  ;
		BYTE    time_buy	  ;
		BYTE	money_buy	  ;
		BYTE	match_buy	  ;
		BYTE    boss_challenge;
		BYTE	battle_reward ;
		BYTE	divine_time	  ;
		BYTE	shop_basket	  ;
		BYTE	elite_time	  ;
		BYTE	rune_basket	  ;
		string  require		  ;
		DWORD	free_gold	  ;


		//WORD    level;
		//WORD    upgrade_exp;
		//BYTE    time_buy;
		//BYTE    money_buy;
		//BYTE	skill_speed;
		//string	vip_name;
		//BYTE    match_buy;
		//BYTE	gift_buy;	
		//BYTE	rob_times;	
		//BYTE	dig_times;	
		//BYTE	bonus_contribution;
		//BYTE	boss_challenge;
		//BYTE    rune_limit;
	};

	struct MONEY_TREE0_CFG
	{	
	//	WORD    time;
		WORD    mul;
		DWORD   cost;
	};

	struct ACTIVITY_INFO_CFG
	{	
		WORD    id;
		BYTE    type;
		WORD    open_level;
		string	vip_name;
		DWORD	advance_time;
		DWORD	begin_time;
		DWORD	end_time;
		WORD	monster_id;
	};

	struct MINE_CFG
	{	
		WORD    id;
		WORD    level_min;
		WORD    level_max;
		DWORD	time;
		DWORD	count;
		//DWORD	mineral;
		WORD	speed;
		WORD	event;
	
		vector<WORD> vecMineIndex;
		vector<DWORD> vecMineRandSum;

		vector<float> vecMineIndexRatio;	//离线挖矿一次暴多个的时候用
		float minRatio;
		

	};

	//struct SHOP_CFG
	//{	
	//	WORD    id;
	//	WORD    group;	
	//	WORD    group_id;	
	//	DWORD	group_rate;	
	//	DWORD	item_rate;	
	//	WORD	number;
	//	WORD    compose_times;	
	//	WORD    item_lv;
	//	BYTE    currency_type;	
	//	vector<WORD> vecItemID;
	//	vector<WORD> vecItemLevel;
	//	vector<WORD> vecItemPrice;
	//};

	struct SHOP0_CFG
	{	
		BYTE type	 ;
		BYTE free_time;
		BYTE sell_one_money_type;	
		BYTE sell_ten_money_type;
		DWORD sell_one_money_Num;	
		DWORD sell_ten_money_Num;
		BYTE sell_ten_money_discount;
		BYTE giftQua;
		WORD giftiD	 ;
		WORD giftNum ;
		DWORD coolFreeTime;
		DWORD firstCoolFreeTime;
		WORD  vip_limit;
	};

	struct SHOP1_CFG
	{	
		WORD  id;
		BYTE  job_limit;	
		//BYTE  sex_limit;	
		WORD  min_level;	
		WORD  max_level;	
		vector<WORD> vecDrop;
	};

	struct SHOP2_CFG
	{
		WORD    id;
		WORD    group;	
		WORD    group_id;	
		DWORD	group_rate;	
		DWORD	item_rate;	
		WORD	number;
		WORD    quality;
		BYTE    currency_type;	
		vector<WORD> vecItem;	
		DWORD   price;
	};

	struct SHOP_GRUOP
	{
		vector<SHOP2_CFG*> vecShop2Cfg;
		vector<DWORD> vecItemRandSum;
	};

	struct SHOP_STORAGE//每个掉落shop1的掉落id对应此结构
	{
		vector<WORD> vecGroupID;//组ID
		vector<DWORD> vecGroupRandSum;//组概率
		map<WORD, SHOP_GRUOP> mapGroupShop;//每个掉落库对应的掉落组结构
	};

	struct SHOP_DISCOUNT
	{
		WORD  time;
		vector<BYTE> vecDiscount;
		vector<DWORD> vecDisRandSum;
	};

	struct OPEN_REWARD
	{
		vector< ITEM_INFO > vecItems;
		DWORD gold;
		DWORD coin;
	};

	struct OPEN_ACTIVITY
	{
		WORD id;
		BYTE bySwitch;
		string name;
		BYTE type;
		BYTE repeat;
		BYTE date_type;
		DWORD begin_time;
		DWORD end_time;
		BYTE reward_date_type;
		DWORD reward_begin_time;
		DWORD reward_end_time;
		BYTE send_reward_type;
		OPEN_REWARD	stReward;
	};

	struct SKILL_STORAGE
	{
		vector<WORD> vecGroupID;
		map<WORD, vector<SKILL_CFG2*> > mapGroupSkills;
	};

	struct MISSION_CFG
	{
		WORD id;
		WORD level_limit;
		BYTE type;
		WORD group;	
		ITEM_INFO parameter;
		WORD front_id;
		WORD next_id;
		DWORD coin;//配置表名字gold
		DWORD ingot;
		WORD itemID;
		size_t itemNum;
		DWORD exp;
		BYTE repeat;
	};

	//struct WORLD_BOSS_REWARD_CFG
	//{
	//	WORD id;
	//	WORD min_boss;
	//	WORD max_boss;
	//	vector< vector<ITEM_INFO> > vvecReward;
	//};

	
	struct PKG_EXTEND_CFG
	{	
		DWORD    needCoin;
		DWORD    needIngot;
		ZERO_CONS(PKG_EXTEND_CFG);
	};

	struct RUNE_CFG_ITEM
	{
		WORD  itemid;
		WORD  level;
		DWORD need_exp; // 升级所需经验
		DWORD stamina; //耐力	
		DWORD strength; //力量										
		DWORD agility; //敏捷	
		DWORD intellect; //智慧
		DWORD spirit;	//精神	
		DWORD hp_max;	//最大生命值	
		DWORD mp_max;	//最大魔法值	
		DWORD atk_max; //攻击最大值	
		DWORD def_max;	//防御最大值
		DWORD spellatk_max;	 //技能攻击最大值
		DWORD spelldef_max; //技能防御最大值	
		DWORD hit; //命中值	
		DWORD dodge; //闪避值	
		DWORD crit; //暴击值	
		DWORD crit_res; //暴击抵抗值	
		DWORD crit_inc;	//暴击伤害增加值
		DWORD crit_red; //暴击伤害减免值
		ZERO_CONS(RUNE_CFG_ITEM);
	};

	
	struct FORGE_REFRESH_CFG
	{
		//WORD wLevel;
		WORD vEquip_1_10[ 10 ];
		DWORD dwSmeltValue;
		DWORD dwSuitSmeltValue;
		DWORD dwSuitPatchCount;
		ZERO_CONS(FORGE_REFRESH_CFG);
	};


	struct IP_INFO
	{
		DWORD StartIPNum;
		//DWORD EndIPNum;
		string Country;
		string Local;
	};

//职业编号索引	等级	耐力	力量	敏捷	智慧	精神	最大生命值	最大魔法值	攻击最大值	攻击最小值	防御最大值	防御最小值	技能攻击最大值	技能攻击最小值	技能防御最大值	技能防御最小值	命中值	闪避值	暴击值	暴击抵抗值	暴击伤害增加值	暴击伤害减免值	每回合魔法恢复值	每回合生命恢复值	幸运	攻击吸血	攻击吸蓝	装备掉率	金币掉率	幸运防御
//job_id	level	stamina	strength	agility	intellect	spirit	hp_max	mp_max	atk_max	atk_min	def_max	def_min	spellatk_max	spellatk_min	spelldef_max	spelldef_min	hit	dodge	crit	crit_res	crit_inc	crit_red	mp_rec	hp_rec	luck	att_hem	att_mana	equ_drop	money_drop	luck_def
	struct MERCENARY_CFG
	{
		BYTE job_id		;
		WORD level		;
		DWORD stamina		;
		DWORD strength	;
		DWORD agility		;
		DWORD intellect	;
		DWORD spirit		;
		DWORD hp_max		;
		DWORD mp_max;
		DWORD atk_max		;
		DWORD atk_min		;
		DWORD def_max		;
		DWORD def_min		;
		DWORD spellatk_max;
		DWORD spellatk_min;
		DWORD spelldef_max;
		DWORD spelldef_min;
		DWORD hit			;
		DWORD dodge		;
		DWORD crit		;
		DWORD crit_res	;
		DWORD crit_inc	;
		DWORD crit_red	;
		DWORD mp_rec		;
		DWORD hp_rec		;
		DWORD luck		;
		DWORD att_hem		;
		DWORD att_mana	;
		DWORD equ_drop	;
		DWORD money_drop	;
		DWORD luck_def	;
	};

	//struct TMerHalo 
	//{
	//	BYTE  byStar;
	//	BYTE  byMoneyType;
	//	DWORD dwMoneyCost;

	//	vector< SKILL_EFFECT > vecEffectAttr;

	//	TMerHalo( string& strAttr, string& strCond )
	//	{
	//		vector<string> vecEffectType;
	//		assert( BASE::strtoken2Vec( strAttr.c_str(), ";", vecEffectType, NULL, NULL) > 0 );
	//		for ( auto& it : vecEffectType )
	//		{
	//			vector<string> vecTypePara;
	//			assert( BASE::strtoken2Vec( it.c_str(), ",", vecTypePara, NULL, NULL) == 3 );
	//			vecEffectAttr.push_back( SKILL_EFFECT( E_ATTR_ADD, ATOB_(vecTypePara[0]),  ATOW_(vecTypePara[1]),  ATOW_(vecTypePara[2]) ) ) ;
	//		}

	//		vector<string> vecCond;
	//		assert( BASE::strtoken2Vec( strCond.c_str(), ";", vecCond, NULL, NULL) == 3 );
	//		byStar = ATOB_( vecCond[ 0 ] );
	//		byMoneyType = ATOB_(vecCond[ 1 ]);
	//		dwMoneyCost = ATOUL_(vecCond[ 2 ]);
	//	}
	//};

	//编号	附带光环1	光环名称	光环1技能说明	开启条件
	//id	halo	halo_name	halo_info	halo_open
	struct MERHALO_CFG
	{
		WORD  id;
		BYTE  byStar;
		BYTE  byMoneyType;
		DWORD dwMoneyCost;
		vector< SKILL_EFFECT > vecEffectAttr;
	};

	//编号	职业编号索引	开放等级	名称	初始技能	性别	形象	头像	附带光环1
	//id	job_id	level	name	skill_id	sex	avatar_img	head_img	halo
	struct MERDEF_CFG
	{
		WORD id;
		BYTE job_id;
		BYTE sex;
		WORD level;
		WORD skill_id;
		vector< WORD > vecHalo;
	};

	//星级	耐力	力量	敏捷	智力	精神	单次升级经验
	//star_num	stamina	strength	agility	intellect	spirit	exp_need
	struct MER_STAR_CFG
	{
		BYTE star_num;
		float baseAttrRatio[BASE_ATTR_NUM];
		//float stamina;
		//float strength;
		//float agility;
		//float intellect;
		//float spirit;
		DWORD exp_need;
		ZERO_CONS(MER_STAR_CFG);
	};

	//训练书页	需要数量	消耗钱币类型	消耗钱币数量	合成目标类型	vip限制									
	//material_id	number	money_type	money_cost	target_id	vip_level_limit		
	struct MER_COMPOSE_BOOK_CFG
	{
		WORD material_id;
		WORD number;
		BYTE money_type;
		DWORD money_cost;
		WORD target_id;
		WORD vip_level_limit;
		DWORD offer_exp;
		ZERO_CONS(MER_COMPOSE_BOOK_CFG);
	};									

	//宝石ID	宝石名称	升级条件	生成目标	分解获得金钱	分解获得碎片	分解tips描述
	//gem_id	gem_name	upgrade	taget	decompose_money	decompose_item	decompose_tips	
	struct GEM_CFG
	{
		WORD gem_id;
		//string	gem_name;
		WORD upgrade_id;//升级精华id
		WORD upgrade_num;//升级精华数量
		WORD target_id;	
		float decompose_coin_ratio;
		DWORD decompose_coin_num;
		float decompose_item_ratio;
		WORD decompose_item_id;
		WORD decompose_item_num;
		//	decompose_tips	
		ZERO_CONS(GEM_CFG);
	};	

	struct ATTRDEFORM_CFG
	{
		BYTE type;
		vector<AttrDeform> vecAttrDeform;
	};

	struct AddAttrValue
	{
		int min;
		int max;
		ZERO_CONS(AddAttrValue);
		AddAttrValue( int min, int max )
		{
			this->min = min;
			this->max = max;
		}
	};
	//培养类型	名称	vip等级限制	消耗钱币类型	消耗钱币数量	属性变更参数
	//id	name	viplevel_limit	money_type	money_cost	attr_param
	struct MERCENARY_TRAIN_CFG
	{
		BYTE type;
		BYTE viplevel_limit;
		BYTE money_type;
		DWORD money_cost;
		vector<AddAttrValue> vecAddAttrValue;
		vector<DWORD> vecRandSum;		
	};

	struct TARGET_AWARD_CFG
	{
		WORD id;
		WORD level;
		vector<ITEM_INFO> vecItem;
	};

	//索引	职业	物品id	物品类型	物品数量	物品品质	物品强化等级	物品附加属性1	物品附加属性2	物品附加属性3	物品附加属性4	物品附加属性5	物品镶嵌孔
	//id	job  	item	type	    number	    rare	    strengthen	     random_1	    random_2	    random_3	    random_4	    random_5	     hole
	struct  RECHARGE_AWARD_CFG
	{
		WORD id;
		BYTE job;
		WORD item;
		BYTE type;
		WORD number;
		BYTE rare;
		BYTE strengthen;
		DWORD random[5];
		WORD  hole;
	};

	ATTR_UPGRADE_CFG* getAttrCfg( WORD job, WORD level);

	bool initCfg(void);
	bool reloadCfg(const string& strCfgName);
	void reloadLogCfg();
	void resetLogserver(const char* ip, int port);
	int readCfgGmName(void);
	int readCfgShieldWords(void);
	CONFIG::MISSION_CFG* getMissionCfg(WORD index);
	CONFIG::OPEN_ACTIVITY* getOpenActivityCfg(WORD index);
	CONFIG::ITEM_CFG* getItemCfg(WORD index);
	CONFIG::SKILL_CFG2* getSkillCfg( WORD id );
	CONFIG::SKILL_EFF_CFG* getSkillEffCfg( WORD id );
	CONFIG::ROLE_EXP_CFG* getExpCfg( WORD id );
	CONFIG::ATT_RAND_CFG* getAttRandCfg(DWORD id);
	CONFIG::COMPOSE_CFG* getComposeCfg(DWORD id);
	CONFIG::GUILD_BUILD_CFG* getGuildBuildCfg(WORD typeLevel);
	CONFIG::DROP_INFO_CFG* getDropInfoCfg(DWORD id);
	CONFIG::DROP_INFO_CFG* getDropInfoChestCfg(DWORD id);
	CONFIG::MERCENARY_TRAIN_CFG* getMercenaryTrainCfg( BYTE id);
	CONFIG::MERCENARY_CFG* getMercenaryCfg( DWORD id);
	CONFIG::MERDEF_CFG* getMerDefCfg(WORD id);
	CONFIG::MERHALO_CFG* getMerHaloCfg(WORD id);
	DWORD getMerUpstarMaxExpCfg();
	CONFIG::MER_STAR_CFG* getMerUpstarCfg(BYTE star);
	CONFIG::ATTRDEFORM_CFG* getAttrDeformCfg(BYTE id);
	CONFIG::MAP_CFG* getMapCfg(WORD id);
	CONFIG::MONSTER_CFG* getMonsterCfg(WORD id);
	CONFIG::ARENA_BOUNS_CFG* getArenaBounsCfg(DWORD id);
	const CONFIG::ARENA_REWARD_CFG* getArenaRewardCfg( DWORD dwRank );
	DWORD* getMoneyTreeCfg(WORD level);
	DWORD* getComposeMaxExpCfg(WORD id);
	DWORD* getComposeNowExpCfg(DWORD id);
	CONFIG::SIGN_CFG* getSignCfg(DWORD id);
	CONFIG::SIGN_CFG* getSignTimeCfg(BYTE times);
	CONFIG::VIP_ADDITION_CFG* getVipAddCfg(WORD level);
	CONFIG::ACTIVITY_INFO_CFG* getActivityInfoCfg(WORD id);
	CONFIG::SHOP1_CFG* getShop1Cfg( const DWORD id);
	CONFIG::SHOP2_CFG* getShop2Cfg(WORD id);
	CONFIG::SHOP1_CFG* getVipShop1Cfg( const DWORD id);
	CONFIG::SHOP2_CFG* getVipShop2Cfg(WORD id);
	CONFIG::SKILL_STORAGE* getSkillStorage( BYTE job );
	CONFIG::MER_COMPOSE_BOOK_CFG* getMerComposebookCfg( WORD id );
	CONFIG::GEM_CFG* getGemCfg( WORD id );
	CONFIG::MONEY_TREE0_CFG* getMoneyTree0Cfg(WORD time);
	CONFIG::TARGET_AWARD_CFG* getTargetAwardCfg(WORD id);
	
	DWORD getMerUpstarNowExpCfg(BYTE star);
	DWORD getMercenaryTrainMaxCfg(WORD level);
	float* getLuckCfg(DWORD dwLuck);
	vector<WORD>* getTaskVecIDsByType( const BYTE& type);
	vector<WORD>* getOpenActivityVecIDsByType( const BYTE type);
	bool getRandItem(DWORD dropID, vector<ITEM_INFO>& tryV, bool bUseChest=false );
	DWORD addExp(WORD& wCurLevel, DWORD& dwCurExp, DWORD dwAddExp, WORD maxLevel, std::function<DWORD ( WORD )> funcGetUpgradeExp );
	DWORD getVipUpgradeExp(WORD wLevel);
	DWORD getRoleUpgradeExp(WORD wLevel);
	DWORD addRoleExp(ROLE* role, DWORD& dwCurExp, DWORD dwAddExp, bool bKillMon = false );
	void upgradeForgeRefresh( ROLE* role, WORD wOldLevel );
	bool forgeRefresh( ROLE* role );
	bool exchangeRefresh( ROLE* role );
	FORGE_REFRESH_CFG* getForgeCfg( WORD wJob, WORD wLevel );
	// 根据品质随机一个符文
	WORD randOneRune(DWORD quality);
	RUNE_CFG_ITEM* getRuneCfg(WORD itemid, WORD level);
	DWORD sumRuneExp(WORD itemid, WORD level);
	bool isExistRuneCfg(WORD itemid, WORD level);

	void parseItemInfoStr( const string& str, vector<ITEM_INFO>& vecItemInfo );
	
	void parseJobEquipInfoStr( const string& str, vector<ITEM_INFO>& vecItemInfo, size_t size = 1 );
	
	void parseEquipInfoStr( const string& str, vector<ITEM_INFO>& vecItemInfo );

	int itemInfo2Str( char *buf, const vector<ITEM_INFO>& vecItemInfo );

	DWORD getCoinDonateEach();
	DWORD getEquipDonatePoint(DWORD quality, DWORD subType );

	const string& getMsgJingjichang();

	const string& getMsgBossHurtRank();

	const string& getMsgBossLastHurt();

	const string& getMsgSendOne();

	const string& getMsgSendTwo();

	const string& getMsgSendThree();

	CONFIG::ITEM_CFG* getDigDropMineIndex(WORD wLevel);
	CONFIG::MINE_CFG* getMineCfg(WORD wLevel );
	CONFIG::ITEM_CFG* getDigDropItemCfg( CONFIG::MINE_CFG* mineCfg );

	DWORD getDigDropMineSpeed(WORD wLevel );
	void initShopItems(const WORD wLevel,const BYTE job, list<SHOP_ITEM>& shopItems, const DWORD updateTime );


	DWORD getExpDelta(WORD level1, WORD level2, DWORD exp1, DWORD exp2);

	CONFIG::PKG_EXTEND_CFG* getPkgExtendCfg(ROLE* role, PKG_TYPE pkgType );
	DWORD getPkgExtendCost(ROLE* role, PKG_TYPE pkgType );

	size_t getSmeltValue(BYTE quality, WORD subType);

	const CONFIG::IP_INFO* findIP( DWORD ip );

	const CONFIG::MERCENARY_CFG* findOpenMercenary( WORD wLevel );

	WORD getMerComposeBookID( WORD type );

	void forEachComposeBookCfg( std::function<void ( const std::pair<WORD, MER_COMPOSE_BOOK_CFG> &x )> func );

	void forEachMercenaryDefCfg( std::function< void ( const std::pair<WORD, MERDEF_CFG> &x )> func );

	void forEachSuitCfg( WORD id, std::function< void ( const SUIT_CFG&x )> func );

	void forEachRechargeAwardCfg( std::function< void ( const RECHARGE_AWARD_CFG& rechargeEle)> rechargeFunc );

	void  resetEliteMap( std::function< void ( const map<WORD, DOUBLE_WORD>&x )> func);

	BYTE getMaxContinueSignTimes();

	CONFIG::SHOP0_CFG* getVipShop0Cfg(BYTE type);

	CONFIG::SHOP2_CFG* getShopItem( const WORD drop,  map<WORD, SHOP_STORAGE>& mapShopStorageCfg);

	BYTE getRandDiscount( const WORD updateTime );

	void initVipShopItems( ITEM_INFO& itemInfo, const WORD drop );

	bool isGmName( const string& strName );

	bool isSieldWord( const string& strName );

	bool isInnerSieldWord( const string& strName );

}//namespace CONFIG

#endif//__CONFIG_H__
