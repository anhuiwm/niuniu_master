#ifndef __ITEM_H__
#define __ITEM_H__


//#include "libevent-net/net_util.h"

#include "base.h"
#include "log.h"
#include "config.h"
#include <list>
#include "session.h"
#include <map>

using namespace std;

//=======================================================================================

//背包扩充
//背包默认开放60格、仓库默认开放60格、矿石背包默认开放60格；
//可通过扩充的方式，扩充至200格背包，200格仓库，200格矿石背包；

#define DEFAULT_PKG_NUMS_EQUIP		 30		//背包默认开放30格、
#define DEFAULT_PKG_NUMS_PROP		 80		//背包默认开放30格、

#define MAX_PKG_NUMS				 100
#define MAX_ITEM_NUMS				 100	    //道具背包
#define DEFAULT_MINE_PKG_NUMS		  30		//矿石默认开放30格、
#define MAX_MINE_PKG_NUMS			 100
#define MAX_FUWEN_PKG_NUMS			 100		// 符文背包上限
#define MAX_EQUIP_ATTR_NUMS			(10)		//装备上的属性最多10个,7个正常属性,3个附加属性,附加属性不受强化等级影响
#define MAX_EQUIP_BASE_ATTR_NUMS	(36)
#define MAX_EQUIP_EXTRA_ATTR_NUMS	 (5)
#define MAX_HOLE_NUMS	(5)
#define SKILL_MAX_NUM				 (4)		//佣兵技能
#define HALO_SKILL_NUM				 (3)		//佣兵光环
#define  MERCENARY_NUM				   3		//佣兵数
#define  FRESH_MERCENARY_NUM		   3		//刷新佣兵技能数
#define  MER_BODY_GRID_NUM			 (4)		//身上有4格
#define  MYSTERY_SHOP_NUM			   2		//神秘商城商人数
#define MAX_SKILL_NUMS				 (10)		//技能8组，还有两个备用
#define MAX_ARENA_RECORD			 (10)		//竞技场记录
#define QUICK_SKILL_INDEXS			  (4)		//技能快捷索引
#define MAX_FRIENDS_NUMS			 (30)		//好友id最多30个
#define MAX_TASK_NUMS				 (40)		//任务最多线数
#define MAX_TASK_OPEN_ACTIVITY_NUMS  (10)		
#define MAX_DIANZHAN_NUMS			  (5)
#define BODY_GRID_NUMS				 (15)		//身上有15格
#define MER_GRID_NUMS				  (4)		//身上有4格
//#define MAX_MERGE_NUMS			 (99)		//最大叠加数量
#define MAX_SHOP_ITEM				  (6)		//商品数量
#define WORLD_BOSS_HELP_NUM			  (6)		//世界boss helper数量
#define MIN_ELITE_MAP_ID		   (1001)		//精英关卡最小值
#define MAX_ELITE_GROUP				 (20)       //精英关卡最多组数
#define MAX_CHARGE_TYPE				 (10)       //充值种类
#define MAX_CODE_AWARD_NUM			 (10)       //验证码领取次数

struct ROLE_CLIENT
{
	ROLE* role;
	session* client;

	ROLE_CLIENT( ROLE* role, session* client )
	{
		this->role = role;
		this->client = client;
	}

};


enum
{
	E_SUCC = 0,
	E_COMMON_ERROR = 999,
};

enum
{

//佣兵
	//MERCENARY_EQUIP_WEAPON = 1,	//武器
	//MERCENARY_EQUIP_CLOTH = 2,	//衣服
	//MERCENARY_EQUIP_HELMET = 3,	//帽子
	//MERCENARY_EQUIP_RING = 8,	//戒指
	
//-------------------------------

//1装备，2道具
	TYPE_EQUIP = 1,
	TYPE_ITEM = 2,
	TYPE_FUWEN = 3,

	//-------------------------------
	SUB_TYPE_EQUIP_WEAPON = 1,
	SUB_TYPE_EQUIP_CLOTH = 2,
	SUB_TYPE_EQUIP_HELMET = 3,
	SUB_TYPE_EQUIP_BELT = 4,
	SUB_TYPE_EQUIP_SHOE = 5,	
	SUB_TYPE_EQUIP_NECKLACE = 6,
	
	SUB_TYPE_EQUIP_BRACELET = 7,
	SUB_TYPE_EQUIP_RING = 8,
	
	SUB_TYPE_EQUIP_MEDAL = 9,
	SUB_TYPE_EQUIP_GEM = 10,
	SUB_TYPE_EQUIP_LINGZHU = 11,

	//-------------------------------

	//SUB_TYPE_ITEM_MATERIAL = 1,
	//SUB_TYPE_ITEM_CURE = 2,
	//SUB_TYPE_ITEM_MULTIPLE_EXP = 3,
	//SUB_TYPE_ITEM_USABLE = 4,
	SUB_TYPE_ITEM_SKILLBOOK = 5,//技能书

	SUB_TYPE_ITEM_MINE = 6,
	
	SUB_TYPE_ITEM_BLACK_IRON = 7,		//7黑铁，8金刚石"
	SUB_TYPE_ITEM_DIAMON = 8,	//8金刚石"

	SUB_TYPE_ITEM_COIN = 9,		//9金币
	SUB_TYPE_ITEM_INGOT = 10,	//10元宝
	SUB_TYPE_ITEM_BOX = 11,		//11随机宝箱

	SUB_TYPE_ITEM_FIXED_BOX = 12,		//12固定宝箱、
	SUB_TYPE_EXP_DAN = 13,				//13经验丹、
	SUB_TYPE_FAC_CONTRIBUTE_DAN = 14,	//14帮会贡献丹、

	SUB_TYPE_COIN_CARD = 15,	//15金币卡、
	SUB_TYPE_INGOT_CARD = 16,	//16元宝卡、
	SUB_TYPE_BLESS_OIL = 17,		//17祝福油、

	ITEM_STYPE_VIP_CARD = 18, //vip体验卡
	ITEM_STYPE_BOSS_CARD = 19, //BOSS挑战卷
	SUB_TYPE_ITEM_HONOUR = 20,		//声望

	SUB_TYPE_EQUIP_GEM_RED =  21, //红宝石
	SUB_TYPE_EQUIP_GEM_YELLOW =  22, //黄宝石 
	SUB_TYPE_EQUIP_GEM_BLUE =  23, //蓝宝石
	SUB_TYPE_EQUIP_GEM_GREEN = 24, //绿宝石
	SUB_TYPE_EQUIP_GEM_PURPLE =  25, //紫宝石
	SUB_TYPE_FUWEN =  26, //符文
	SUB_TYPE_RESET_STONE = 28, //洗练石
	SUB_TYPE_LINGZHU_USE = 99,		//99灵珠使用


//"装备的功能类型：1武器,2衣服，3头盔，4腰带，5鞋子，6项链，7手套，8戒指，
//9勋章，10宝石，11灵珠

//道具的功能类型：1材料，2恢复，3多倍经验，4可使用，5技能书，6矿石，7黑铁，8金刚石"
//9金币，10元宝，11随机宝箱


};


enum
{
	//EQUIP_WEAPON = 1 --武器
	//EQUIP_CLOTHES = 2 --铠甲
	//EQUIP_HAT = 3 --帽子
	//EQUIP_BELT= 4 --腰带
	//EQUIP_SHOES = 5 --鞋子
	//EQUIP_NECKLACE = 6 --项链
	//EQUIP_BRACLET = 7 --手套(手镯)
	//EQUIP_RING = 8 --戒指
	//EQUIP_MEDAL = 9 --勋章
	//EQUIP_STONE = 10 --宝石
	//EQUIP_PEAL = 11 --灵珠

	BODY_INDEX_WEAPON = 0,//穿武器
	BODY_INDEX_CLOTH = 1,//穿衣服
	BODY_INDEX_HELMET = 2,
	BODY_INDEX_BELT = 3,
	BODY_INDEX_SHOE = 4,
	BODY_INDEX_NECKLACE = 5,

	BODY_INDEX_BRACELET_L = 6,
	BODY_INDEX_BRACELET_R = 7,

	BODY_INDEX_RING_L = 8,
	BODY_INDEX_RING_R = 9,
	
	BODY_INDEX_MEDAL = 10,
	BODY_INDEX_GEM = 11,
	BODY_INDEX_LINGZHU = 12,
};//13件装备


enum ATTR_TYPE
{
	//佣兵属性更新通知,服务器推送
#if 0
		ROLE_PRO_WARPWR = 106, //战力值
		ROLE_PRO_STAR = 119, //佣兵星级

		ROLE_PRO_CUR_HP = 1, //血量(没有?保留)
		ROLE_PRO_MAX_HP = 2, //最大血量
		ROLE_PRO_CUR_MP = 3, //当前魔法
		ROLE_PRO_MAX_MP = 4, //最大魔法
		ROLE_PRO_ATK_MAX = 5,//物理攻击上限
		ROLE_PRO_ATK_MIN = 6,//物理攻击下限
		ROLE_PRO_MAG_ATK_MAX = 7,//最大魔法攻击
		ROLE_PRO_MAG_ATK_MIN = 8, //最小魔法攻击

		ROLE_PRO_SOL_ATK_MAX = 9, //最大道术攻击,sol_max(保留)
		ROLE_PRO_SOL_ATK_MIN = 10, //最小道术攻击,sol_min(保留)

		ROLE_PRO_PHY_DEF_MAX = 11, //最大防御, phydef_max
		ROLE_PRO_PHY_DEF_MIN = 12, //最小防御, phydef_min
		ROLE_PRO_MAG_DEF_MAX = 13, //最大魔法防御, magdef_max
		ROLE_PRO_MAG_DEF_MIN = 14, //最小魔法防御, magdef_min
		ROLE_PRO_PHY_DODGE = 15, //物理回避率

		ROLE_PRO_MAG_DODGE = 16, //魔法回避率(保留)
		ROLE_PRO_SOL_DODGE = 17, //道术回避率(保留)

		ROLE_PRO_ATK_SPEED = 18, //攻击速度
		ROLE_PRO_REC_HP = 19, //生命回复
		ROLE_PRO_REC_MP = 20, //魔法回复
		ROLE_PRO_PHY_HIT = 21, //物理命中率
		ROLE_PRO_STAMINA = 22, //耐力改变
		ROLE_PRO_STRENGTH = 23, //力量改变
		ROLE_PRO_AGILITY = 24, //敏捷改变
		ROLE_PRO_INTELLECT = 25, //智慧改变
		ROLE_PRO_SPIRIT = 26, //精神
		ROLE_PRO_CRIT_VAL = 27, //暴击值
		ROLE_PRO_CRIT_DEF = 28, //暴击抵抗
		ROLE_PRO_CRIT_APPEND = 29, //暴击伤害增加值
		ROLE_PRO_CRIT_RELIEF = 30, //暴击伤害减免值
		ROLE_PRO_SUCK_HP = 31, //攻击吸血
		ROLE_PRO_SUCK_MP = 32, //攻击吸蓝
		ROLE_PRO_EQUIP_DROP = 33, //装备掉率
		ROLE_PRO_COIN_DROP = 34, //金币掉率
#endif
	ROLE_PRO_COIN = 101,//金币
	ROLE_PRO_INGOT = 102,//元宝

	ROLE_PRO_CUREXP = 103, //当前经验改变
	ROLE_PRO_MAX_HERO = 104,//最大精英关卡
	ROLE_PRO_POWER = 105,	//体力改变
	ROLE_PRO_WARPWR = 106, //战力值

	ROLE_PRO_LEVEL = 107, //等级变化
	ROLE_PRO_FACTION = 108, //公会发生改变


	ROLE_PRO_TITLE = 109, //获得称号
	ROLE_PRO_MAX_MAP = 110, //通关地图改变
	ROLE_PRO_FACTION_JOB = 111, //公会职位改变


	ROLE_PRO_FACTION_CONTRIBUTE = 112, //帮贡改变
	ROLE_PRO_MAX_EXP = 113, //经验上限
	ROLE_PRO_HONOUR = 114, //声望
	ROLE_PRO_SMELTVAR = 115, //熔炼值更改
	ROLE_PRO_VIPVAR = 116, //累计充值
	ROLE_PRO_VIP_LEVEL = 117, //vip等级更改

ROLE_PRO_STAR = 119, //佣兵星级
	//	ROLE_PRO_POWER = 201,				//--装备类道具的战力属性

	//E_ATTR_HP = 1,//体力值 保留
	E_ATTR_HP_MAX2		 =  2,//最大生命值
	//E_ATTR_MP = 3,//魔法值
	E_ATTR_MP_MAX2	=  4,//魔法上限
	E_ATTR_ATK_MAX2	  =  5,//攻击最大值
	E_ATTR_ATK_MIN2  =  6,//攻击最小值
	E_ATTR_SPELLATK_MAX  =	7,//技能攻击最大值
	E_ATTR_SPELLATK_MIN  =	8,//技能攻击最小值
	E_ATTR_SOL_MAX = 9,//道术攻击上限
	E_ATTR_SOL_MIN = 10,//道术攻击下限
	E_ATTR_DEF_MAX	     =  11,//防御最大值
	E_ATTR_DEF_MIN	     =  12,//防御最小值
	E_ATTR_SPELLDEF_MAX  =  13,//技能防御最大值
	E_ATTR_SPELLDEF_MIN  =  14,//技能防御最小值
	E_ATTR_DODGE		 =	15,//闪避值
	E_ATTR_MAG_DODGE = 16,//魔法闪避
	E_ATTR_SOL_DODGE = 17,//道术闪避
	E_ATTR_SPEED = 18,//攻击速度
	E_ATTR_MP_REC2		 =	19,//每回合魔法恢复值
	E_ATTR_HP_REC2		 =	20,//每回合生命恢复值
	E_ATTR_HIT			 =	21,//命中值
	E_ATTR_STAMINA	     =  22,//耐力//一级属性
	E_ATTR_STRENGTH	     =  23,//力量//一级属性
	E_ATTR_AGILITY	     =  24,//敏捷//一级属性
	E_ATTR_INTELLECT     =  25,//智慧//一级属性
	E_ATTR_SPIRIT		 =  26,//精神//一级属性
	E_ATTR_CRIT		     =	27,//暴击值
	E_ATTR_CRIT_RES	     =	28,//暴击抵抗值
	E_ATTR_CRIT_INC	     =	29,//暴击伤害增加值
	E_ATTR_CRIT_RED	     =	30,//暴击伤害减免值
	E_ATTR_ATT_HEM		 =	31,//攻击吸血
	E_ATTR_ATT_MANA	     =	32,//攻击吸蓝
	E_ATTR_EQU_DROP	     =	33,//装备掉率
	E_ATTR_MONEY_DROP	 =	34,//金币掉率
	E_ATTR_LUCK		     =	35,//幸运
	E_LUCK_DEF			 =  36,//幸运防御
	E_ATTR_MAX,//最大属性限制
	
	DEC_DIZZY_TIMES = 50,// 减少眩晕回合
	HURT,// 固定伤害
	HURT_DOWN,// 固定免伤
	BOSS_HURT,// 对boss伤害提升
	SKILL_HURT,// 技能固定伤害
	SKILL_HURT_DOWN,// 技能固定减伤
	CONSUME_MP_DOWN,// 蓝耗降低
	E_ATTR_LINGZHU = 201,//灵珠
};


enum E_TASK_STATUS
{

	E_TASK_UNCOMPLETE = 0,						//0未完成，
	E_TASK_COMPLETE_CAN_GET_PRIZE = 1,			//1已完成未领奖，
	E_TASK_COMPLETE_ALREADY_GET_PRIZE = 2,		//2已完成已领奖(瞬态，立刻进入下一个主线任务，除非是末尾)
};

#define IS_VALID_ATTR(attr) ( attr >= E_ATTR_HP_MAX2 && attr < E_ATTR_MAX )


#pragma pack(push, 1)

//struct AttrValue
//{
//	BYTE attr;
//	WORD value;
//
//	ZERO_CONS(AttrValue)
//		AttrValue(BYTE attr, WORD value)
//	{
//		this->attr = attr;
//		this->value = value;
//	}
//};

//////////////////////////////////////////////装备扩展属性///////////////////////////////////////////////////////
#define MAX_EQUIP_EXTEND_COUNT 20
enum class e_equip_extend_attr : byte
{
	dec_dizzy_times,// 减少眩晕回合
	hurt,// 固定伤害
	hurt_down,// 固定免伤
	boss_hurt,// 对boss伤害提升
	skill_hurt,// 技能固定伤害
	skill_hurt_down,// 技能固定减伤
	consume_mp_down,// 蓝耗降低
	max_count,
};

class equip_extend
{
public:
	ATTR_TYPE type;
	float value;
};
typedef equip_extend* equip_extend_ptr;


class item
{
public:
	static void load_config();
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ITEM_CACHE;

class CMercenary;

struct ITEM_DATA
{
	WORD itemIndex;

	DWORD itemNums;

	BYTE byStatus:1; 		//道具状态信息
	BYTE byQuality:7;	//合成等级
	BYTE byStrengthLv;	//强化等级

	BYTE byLuckPoint;		//
	BYTE byLuckDefPoint;
	BYTE byHole;
	WORD wStone[ MAX_HOLE_NUMS];

	AttrValue attrValueExtra[ MAX_EQUIP_EXTRA_ATTR_NUMS ];

	DWORD dwGetStrengthStoneNum; // 使用强化石数量
	byte star_level;
	equip_extend equip_extends[3];

	ZERO_CONS(ITEM_DATA)

	void initFromItemCache(const ITEM_CACHE* itemCache);
};

struct stIndexItem
{
	BYTE byStatus; // 状态 0未解锁 1解锁
	WORD skillid; // 技能id
	WORD group;//技能组

	ZERO_CONS(stIndexItem);

	stIndexItem( WORD haloID )
	{
		this->byStatus = 0;
		this->skillid = haloID;
		this->group = 0;
	}

	bool operator == (const WORD& haloId ) const
	{
		return skillid == haloId;
	}
};

struct MERCENARY_DATA
{
	stIndexItem vIndexSkills[ SKILL_MAX_NUM ]; // 技能
	ITEM_DATA  vBodyEquip[ MER_BODY_GRID_NUM ];
	WORD wID;
	BYTE byFight;
	BYTE  byJob;
	BYTE  byStar;
	BYTE bySex;
	WORD wLevel;
	//DWORD merAttr[E_ATTR_MAX];
	int attrValueTrain[ BASE_ATTR_NUM ];
	//DWORD dwFightValue;   //战斗力
	stIndexItem vHaloSkills[HALO_SKILL_NUM]; // 光环技能
	DWORD dwStarExp;
	ZERO_CONS(MERCENARY_DATA);

	void initFromMerCache(const CMercenary* pCmer);
};

struct  MYSTERY_SHOP_INFO
{
	WORD  wFreeTimes;//免费次数
	BYTE  byFirst;//是否第一次够买 1是 0否
	DWORD dwCanTreeTime;//免费买的时间
	ZERO_CONS(MYSTERY_SHOP_INFO);
	MYSTERY_SHOP_INFO(CONFIG::SHOP0_CFG* shop0Cfg)
	{
		if ( shop0Cfg == NULL )
		{
			return;
		}

		wFreeTimes = shop0Cfg->free_time;
		dwCanTreeTime = shop0Cfg->firstCoolFreeTime + time(NULL);
		byFirst = 1;
	}
};

struct stRuneItem
{
	BYTE status; // 符文位置是否开放，0未开放，1开放 
	DWORD exp; // 当前经验
	WORD runeid; // 符文id
	BYTE level; // 符文等级
	stRuneItem()
	{
		status = 0;
		exp = 0;
		runeid = 0;
		level = 1;
	};
};


#define CLIENT_SHOW_TYPE_BASE	(100)

struct MINE
{
	WORD itemIndex;
	DWORD itemNum;//数量	
};

struct ITEM_CACHE
{
	CONFIG::ITEM_CFG * itemCfg;

	DWORD itemNums;//数量

	BYTE byStatus:4; 		//道具状态信息
	BYTE byExtraAttrNums:4;	//保存极品属性条目数，无需保存,

	BYTE byStrengthLvRes:4;	//强化等级到50 保留
	BYTE byQuality:4;		//品质
	BYTE byStrengthLv;	//强化等级 // 符文等级
	//BYTE byNeedUpdateClient:1;	//
	BYTE byDropAndSell:1;	//掉落并且卖出,cache, 不存数据库

	BYTE byLuckPoint:7;	//额外幸运,武器,祝福油附加

	BYTE byLuckDefPoint;//额外幸运防御,武器,祝福油附加

	DWORD dwLingZhuExp;	//灵珠积攒经验

	DWORD dwFightValue; // 装备战力 符文经验

	BYTE byHole; //已经开的孔数量

	BYTE byPkgType;

	WORD wStone[MAX_HOLE_NUMS]; //宝石镶嵌孔,宝石ID; 0:未镶嵌宝石

	AttrValue attrValueBase[ MAX_EQUIP_BASE_ATTR_NUMS ];			//这部分可以不保存,只cache 
	AttrValue attrValueExtra[ MAX_EQUIP_EXTRA_ATTR_NUMS ];			//额外属性 不保存	
	AttrValue attrOldValueExtra[ MAX_EQUIP_EXTRA_ATTR_NUMS ];		//初始化时候得到的属性

	DWORD dwGetStrengthStoneNum;  // 使用强化石数量

	byte star_level;
	equip_extend equip_extends[3];
	equip_extend equip_extends_for_skill;

	ZERO_CONS(ITEM_CACHE);
	void resetItemAttrValue();
	void resetAttrValueBase();//设置基础属性(二级属性)
	void resetEquipExtraPro();//重置额外属性（一级属性）

	void setItemBasePro();	//读取配置表的基础属性
	void randEquipExtraPro();//根据品质获取额外属性
	//void randEquipExtraProChest(BYTE Chest1_RandNums7 );
	void setAttrValueExtra( const int randTimes );

	//void transformExtraAttr();

	// 是否达到强化最大等级
	bool isStrengthenMaxLevel();
	// 强化升级
	void levelUpStrengthen();


	void calEquipFightValue( );
	
	DWORD getExtraNums();

	// 孔位是否已满
	bool isHoleFull();
	// 打孔消耗金刚钻数量
	DWORD PuchHoleStoneCount();
	// 打孔成功
	void PuchOneHole();
	// 孔位是否已开启
	bool isHoleOpen(BYTE pos);
	// 位置是否已镶嵌
	bool isHoleInjected(BYTE pos);
	// 是否已经镶嵌同类型宝石
	bool hasInjectedSameTypeGem(WORD itemid);
	// 镶嵌宝石
	bool injectEquipGem(BYTE pos, WORD gemid);
	// 获得孔位镶嵌的宝石
	WORD getInjectedGem(BYTE pos);
	// 卸载宝石
	void unInjectGem(BYTE pos);
	// 获得所有宝石
	std::vector<WORD> getAllInjectGems();
	// 卸载所有宝石
	void unInjectAllGems();
	// 装备洗练
	void equipReset(  const BYTE reset_type  );
	//// 是否装备洗练增加
	//bool isequipResetAdd(DWORD& value);

	// 获得产出的熔炼值
	DWORD getOutputSmeltValue();
	// 获得强化石数量
	DWORD getOutputStrengthenStoneCount();
	// 获得套装碎片数量
	DWORD getOutputSuitPatchCount();
	// 获得产出的品质值
	DWORD getOutputQualityValue();

	// 获得符文经验
	DWORD getDivineExp();
	
	ITEM_CACHE(const ITEM_CACHE* itemCache);


	ITEM_CACHE(const ITEM_DATA* itemData);
	//ITEM_CACHE(const MINE* itemMine);

	ITEM_CACHE(CONFIG::ITEM_CFG* itemCfg, size_t itemNums, BYTE byQuality = 1);//, BYTE byChest_chestRandNums=0 );//高1位:使用chest,低7位属性条数

};


struct PRACTISE_SKILL
{
	WORD  wID;				//技能id
	DWORD dwStartTime;		//最后启动修炼时间，0表示没有修炼
	DWORD dwReqTime;		//剩余修炼时间，配置表分钟需要乘以60
	ZERO_CONS(PRACTISE_SKILL);
	PRACTISE_SKILL(WORD  ID, DWORD startTime, DWORD reqTime)
	{
		wID = ID;
		dwStartTime = startTime;
		dwReqTime = reqTime;
	}
};

struct ARENA_RECORD
{
	BYTE  byResult;			//结果
	char szName[NAME_MAX_LEN];	
	DWORD dwRank;		    //最终排名
	DWORD dwTime;		    //竞技时间
	BYTE  byJob;
	BYTE  bySex;
	WORD  wLevel;
	DWORD id;
	ZERO_CONS(ARENA_RECORD);
	ARENA_RECORD(BYTE  byResult, const char *szName, DWORD dwRank, DWORD dwTime, BYTE byJob, BYTE bySex, WORD wLevel, DWORD id )
	{
		this->byResult= byResult;	
		strncpy(this->szName, szName, sizeof(this->szName));
		this->dwRank  = dwRank;	
		this->dwTime  = dwTime;	
		this->byJob = byJob;
		this->bySex = bySex;
		this->wLevel = wLevel;
		this->id = id;
	}
	ARENA_RECORD& operator = (const ARENA_RECORD&dst )
	{
		this->byResult= dst.byResult;	
		strncpy(this->szName, dst.szName, sizeof(this->szName));
		this->dwRank  = dst.dwRank;	
		this->dwTime  = dst.dwTime;
		this->byJob = dst.byJob;
		this->bySex = dst.bySex;
		this->wLevel = dst.wLevel;
		this->id = dst.id;
		return  *this;
	}

};
// 偶遇PK结果
enum class meet_pk_result : byte
{
	nothing,
	attack_win,
	attack_lose,
	defence_win,
	defence_lose,
	revenge_win,
	revenge_lose,
};

// 偶遇PK记录
struct meet_pk_record
{
	unsigned id;
	char name[NAME_MAX_LEN];
	byte  job;
	byte  sex;
	unsigned time;
	meet_pk_result  result;
	unsigned coin;
	int reward_pkvalue;
};

//主线任务
struct MAIN_TASK_INFO
{
	WORD wTaskID;

	BYTE byTaskCompleteNums;	//当前主线任务完成次数

	BYTE byTaskStatus;	//0未完成，1已完成未领奖，2已完成已领奖(瞬态，立刻进入下一个主线任务，除非是末尾)

	ZERO_CONS(MAIN_TASK_INFO);
	MAIN_TASK_INFO( const WORD& id )
	{
		this->wTaskID = id;
		this->byTaskCompleteNums = 0;
		this->byTaskStatus = 0;

	}
	bool operator == (const WORD& id)const
	{
		return (this->wTaskID == id);
	}

};



#pragma pack(pop)

struct CLIENT;
struct FACTION;

struct PREDICT_EARNING
{
	DWORD dwExp;
	DWORD dwCoin;
	WORD wItemNums;
	WORD wTimeIntv;
};

#define PREDICT_NUMS	5

struct MAIL
{
	DWORD mailID;
	//DWORD prizeID;
	DWORD recvTime;
	BYTE mailType;
	BYTE isRead;

	vector<ITEM_INFO> vecPrize;
	string strContent;
	
	MAIL( )
	{
		mailID = 0;
		recvTime = 0;
		mailType = 0;
		isRead = 0;
	}

	
};


#define ROOM_MINER_NUMS		(4)		//同房间还有其他4人在挖矿

struct MINER
{
	DWORD dwRoleID;	//0表示此位置空缺,startTime表示空缺开始时间
	DWORD startTime;	//挖矿开始时间,
	//DWORD remainTime;	//挖矿剩余时间,(秒)

	ZERO_CONS(MINER)
};

#pragma pack(push, 1)

class ROLE_BLOB
{
public:
	BYTE res_[128];

	BYTE curEquipPkgLimit;		//当前最大的包裹格子数
	BYTE curFuwenPkgLimit;	//当前最大的仓库格子数
	BYTE curMineLimit;		//当前最大的矿物格子数
	BYTE curItemPkgLimit;		//当前最大的矿物格子数


	ITEM_DATA arrBody[BODY_GRID_NUMS];		
	ITEM_DATA arrPkg[MAX_PKG_NUMS];		
	ITEM_DATA arrProps[MAX_ITEM_NUMS];
	ITEM_DATA arrFuwen[MAX_FUWEN_PKG_NUMS];

	//MINE arrMine[MAX_MINE_PKG_NUMS];		//100*3

	
	WORD wOnUpSkill;						//正在修炼技能ID,没有为0
	WORD wFightSkill[QUICK_SKILL_INDEXS];  //快捷技能索引
	WORD arrSkill[MAX_SKILL_NUMS];//10*10 技能列表

	DWORD arrFriendsID[MAX_FRIENDS_NUMS];   

	ARENA_RECORD arrArenaRecord[MAX_ARENA_RECORD];

	WORD wLastFightMapID;//挂机地图
	WORD wCanFightMaxMapID;//当前可以打的地图ＩＤ
	WORD wWinMaxMapID;

	WORD wLastFightHeroID;		//精英关卡 正在战斗的地图
	WORD wCanFightMaxHeroID;	//精英关卡 当前可以打的地图(编号大于这个的地图都不能打)
	WORD wWinMaxHeroID;			//精英关卡 胜的最大地图

	BYTE byTitleId;

	BYTE bySex;

	DWORD tLogoutTime;		//退出时间
	DWORD arrDianZhanIDs[MAX_DIANZHAN_NUMS];   

	BYTE byPredictIndex;
	PREDICT_EARNING predictEarning[PREDICT_NUMS];

	//DWORD dwComID;//竞技场对手id 需要保存

	DWORD dwAutoSellOption;

	DWORD dwMiningRemainTime;//本人挖矿剩余时间
	BYTE byMiningSlotIndex;//挖矿的坑位

	MINER vMiner[ROOM_MINER_NUMS];//同房间其他挖矿者
	SHOP_ITEM vShopItem[MAX_SHOP_ITEM];//商城可买物品

	BYTE bySendFightBossTimes;												//挑战boss次数每天三次0点重置
	BYTE byDailyBuyBossTimes;												//购买挑战boss次数不重置
	BYTE byAllBuyBossTimes;

	BYTE bySendFightHeroTimes;												//精英关卡
	BYTE byDailyBuyHeroTimes;												//购买挑战hero次数不重置
	BYTE byAllBuyHeroTimes;	

	DWORD dwShopItemTimes;//商城刷新
	BYTE byBuyQuickFightTimes;//购买快速战斗次数

	BYTE byEnhanceTime;//鼓舞次数
	BYTE byFreshTimes;//刷新佣兵技能次数
	BYTE bySignTimes;//continue sign times per month
	DWORD dwDaySignStatus;//day sign status bit
	BYTE byTimeSignStatus;// times sign status bit

	MAIN_TASK_INFO tasksInfo[MAX_TASK_NUMS];

	WORD wTmpVipLevel;
	DWORD dwTmpVipEndTime;//暂时VIP 失效时间

	DWORD dwSmeltValue;//熔炼值

	BYTE byForgeRreshTimes;	//打造界面已经刷新次数,日清

	ITEM_DATA itemDataForge;	//打造界面当前装备
	BYTE byNewMapKillMonNum;

	BYTE byCurLevelAwardStep; //按等级领取奖励步数
	// 已经熔炼的品质值
	DWORD _res;

	MERCENARY_DATA arrMerData[ MERCENARY_NUM ];
	
	BYTE divine_status; //占卜状态 0阴 1阳 2合
	BYTE divine_times; // 占卜次数
	stRuneItem rune_items[8];

	BYTE byUseMoneyTreeTimes;

	MYSTERY_SHOP_INFO m_vMysInfo[MYSTERY_SHOP_NUM];//两个商人的信息

	DOUBLE_WORD m_vEliteGroupInfo[MAX_ELITE_GROUP];//精英关卡组
	BYTE  byNewbieGuideStep;											//新手引导步骤
	DWORD dwFisrtLoginTime;												//初始登录时间

	MAIN_TASK_INFO openActivityInfo[MAX_TASK_OPEN_ACTIVITY_NUMS];
	
	DWORD arrFirstCharge[MAX_CHARGE_TYPE];

	BYTE  byRechargeState;

	DWORD close_user_valid_time;										// 封号有效时间
	BYTE deny_chat;														// 禁言
	BYTE arrCodeAwardTypes[MAX_CODE_AWARD_NUM];							//兑换码领取

	unsigned pkvalue;													// PK值
	unsigned pk_count;													// PK次数
	unsigned pk_buy_count;													// PK次数
	unsigned pk_dead_count;													// PK死亡次数
	unsigned last_pk_count_inc_time;						// 最后pk数量递增时间
	meet_pk_record meet_pk_records[20];
	unsigned world_boss_battle_attack_index;		// 世界老怪战斗攻击序号
	BYTE login_day_num;								//累计登陆天数

	//BYTE res___[ 5776 ];

	ROLE_BLOB()
	{
		memset(this, 0, sizeof(ROLE_BLOB) );
	}
};

//static_assert( sizeof(ROLE_BLOB) == 24*1024, "sizeof(ROLE_BLOB) SHOULD BE 24576" );

				
#pragma pack(pop)

namespace MON
{
	DWORD getMonFightValue( CONFIG::MONSTER_CFG* moncfg );
}


#endif	//__ITEM_H__
