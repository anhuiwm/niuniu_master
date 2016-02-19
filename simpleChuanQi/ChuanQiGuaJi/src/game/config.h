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
	PKG_TYPE_BODY = 1,//����
	PKG_TYPE_EQUIP = 2,//װ��
	//PKG_TYPE_STORAGE = 3,
	//PKG_TYPE_MINE = 4,
	PKG_TYPE_ITEM = 5,// --��ͨ���߰�
	PKG_TYPE_FUWEN = 6, // ���ı���
	PKG_TYPE_SLAVE = 7, //Ӷ��װ������
};

enum MONEY_TYPE
{
	E_GOLD_MONEY = 1,//Ԫ�� ��ֵ��
	E_COIN_MONEY = 2,//��� ��Ϸ��
};

enum E_JOB
{
	E_JOB_WARRIOR = 1,//սʿ
	E_JOB_MAGIC = 2,//��ʦ
	E_JOB_MONK = 3,//��ʿ
};

enum 
{
	E_SEX_MALE = 0,		//��
	E_SEX_FEMALE = 1,	//Ů
};

enum
{
	E_NO_HOLE = 0,		//�޿�
	E_HAVE_HOLE = 1,	//�п�
};

enum
{
	E_ATTR_ADD = 1,	       //��
	E_ATTR_REDUCE = 2,		//��
};

enum
{
	E_MYSTERY_SHOP_COMMON = 1,	    //�����̳���ͨ
	E_MYSTERY_SHOP_HIGN = 2,		//�����̳Ǹ߼�

	E_MYSTERY_SHOP_ONE = 1,	    //1
	E_MYSTERY_SHOP_TEN = 2,		//10
};

enum SKILL_TYPE
{
	//���ܵ����ͣ�1�����༼��,2�����༼��,3����Ч���༼��,4��ͨ����,5��������
	E_SKILL_TYPE_ATTACK = 1,
	E_SKILL_TYPE_TREAT = 2,
	E_SKILL_TYPE_EFFECT = 3,
	E_SKILL_TYPE_GENERAL = 4,
	E_SKILL_TYPE_LESSMP = 5,

	//���ܵ����ͣ�1������2�ٻ���3������4���� //wm  giveup 
	//E_SKILL_TYPE_ATTACK = 1,
	E_SKILL_TYPE_SUMMON = 2,
	E_SKILL_TYPE_PASSIVE = 3,
	//E_SKILL_TYPE_TREAT = 4,


	//���ܶ���Ч������
	E_SKILL_EFFECT_POFANG  = 1 ,//	�Ʒ�
	E_SKILL_EFFECT_MABI  = 2,	//���
	E_SKILL_EFFECT_CHENGMO  = 3,	//��Ĭ
	E_SKILL_EFFECT_KUANGBAO  = 4,	//��
	E_SKILL_EFFECT_QIANNENG  = 5,	//����
	E_SKILL_EFFECT_ZHUOSAO  = 6,	//����
	E_SKILL_EFFECT_XUANYUN  = 7,	//�ջ�
	E_SKILL_EFFECT_XURUO  = 8,	//����
	E_SKILL_EFFECT_MOFADUN  = 9,	//ħ����
	E_SKILL_EFFECT_SAOLAN  = 10,//	����
	E_SKILL_EFFECT_ZHONGDU  = 11,//	�ж�
	E_SKILL_EFFECT_SHUFU  = 12,//	����
	E_SKILL_EFFECT_MONATTACKTWO  = 13,//	����2�ι���
	E_SKILL_EFFECT_ZHENQI  = 14,//	����
	E_SKILL_EFFECT_CHUNFENG  = 15,//	����
	E_SKILL_EFFECT_MAX  = 16,
};

enum E_QUALITY
{
	E_QUALITY_WHITE = 1,   // ��ɫ
	E_QUALITY_BLUE = 2,  // ��ɫ
	E_QUALITY_GOLD = 3, // ��ɫ
	E_QUALITY_DARK_GOLD = 4, // ����ɫ 
	E_QUALITY_GREEN = 5, //��ɫ
	E_QUALITY_ORANGE = 6,//��ɫ
	E_QUALITY_RED = 7, // ��ɫ
};

//������չ
enum E_PKG_EXTEND
{

//1������2�ֿ⣬3��ʯ����			
	E_PKG_EXTEND_PACK = 1,
	E_PKG_EXTEND_STORAGE = 2,
	E_PKG_EXTEND_MINE = 3,
	E_PKG_EXTEND_UTILITY = 2,
};

enum eDROPTYPE
{
	E_DROP_MON = 0,//��ͨ�������
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


#define ONCE_ROLL_BASE		(10000)		//��roll����

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
	//��������:
	E_TASK_LEVEL = 1,					//1��ҵȼ���				
	E_TASK_KILL_MON = 2,				//2ɱ�֣�
	E_TASK_HAS_EQUIP_ENHANCE = 3,		//3����ǿ����
	E_TASK_HAS_EQUIP_RARE = 4,			//4װ��ϡ�жȣ�
	E_TASK_HAS_EQUIP_COMPOSE = 5,		//5װ���ϳɣ�
	E_TASK_TITLE = 6,					//6ӵ�гƺţ�
	E_TASK_VIP_LEVEL = 7,				//7VIP�ȼ���
	E_TASK_BUY_QUICK_FIGHT = 8,			//8����ս�����������
	E_TASK_SPECIAL_TASK = 9,			//9���ָ������
	E_TASK_DO_EQUIP_ENHANCE = 10,		//10����ǿ��������
	E_TASK_DO_EQUIP_COMPOSE = 11,		//11���кϳɲ�����
	E_TASK_FACTION_DONATE = 12,			//12���й�����ײ�����
	E_TASK_EVENT = 13,					//13����¼���
	E_TASK_ARENA = 14,					//14��������ս��
	E_TASK_MINING = 15,					//15�����ڿ�
	E_TASK_FACTION_WAR = 16,			//16�μ��л�ս,
	E_TASK_EQUIP = 17,					//17����һ��װ������������
	E_TASK_USE_MONEYTREE = 18,			//18ʹ��ҡǮ����
	E_TASK_WORLD_BOSS = 19,				//19��������BOSSս��
	E_TASK_FIGHT_BOSS = 20,				//20BOSS��ս
	E_TASK_SMELT = 21,					//21����
	E_TASK_ALL_EQUIP_LEVEL = 22,		//22ȫ��װ���ȼ�	������ϴ�����װ��������ָ���ȼ�����ʽ��װ���ȼ�
	E_TASK_GET_GREEN_TIMES = 23,		//23�����װ����	����ܹ���õ���װ��������ʽ����װ��õĴ���
	E_TASK_FORGE_EQUIP_TIMES = 24,		//24����װ������	���ͨ�������õ�װ����������ʽ������Ĵ���
	E_TASK_UNIJECT_DIAMOND = 25,		//25��Ƕ��ʯ����	�������װ���ܹ���Ƕ�ı�ʯ��������ʽ������װ���ܹ��ı�ʯ������
	E_TASK_EQUIP_RESET_TIMES = 26,		//26ϴ��װ������	���ϴ��װ���Ĵ�������ʽ��ϴ���Ĵ���
	E_TASK_EQUIP_LUCK = 27,				//27�������˵Ĳ�������	��Ҳ����������˵Ĵ�������ʽ�������������˵Ĵ���
	E_TASK_CLOTH_LUCK = 28,				//28�·����˵Ĳ�������	��Ҳ����·����˵Ĵ�������ʽ�������·����˵Ĵ���
	E_TASK_GET_SUIT_TIMES = 29,			//29������װ����	�������װ���������װ������Ŀ����ʽ����װ���Լ�����Ŀ��
	E_TASK_BUY_FIGHT_BOSS_TIMES = 30,	//30����boss��ս����	������ս�ؿ�boss�Ĵ�������ʽ���������
	E_TASK_SHOP_BUY_TIMES = 31,			//31�����̳ǵ��ߴ���	�����̳ǵ��ߵĴ�������ʽ���������
	E_TASK_SET_FIGHT_SKILL_NUM = 32,	//32����ս������	�����е�ս�����ܸ�������ʽ���������õĸ���
	E_TASK_GET_MERCENARY_NUM = 33,		//33���Ӷ��	����Ӷ������������ʽ������Ӷ����������
	E_TASK_FIGHT_BOSS_TIMES = 34,		//34boss��ս	�����ս����ؿ�boss����ʽ����ս����
	E_TASK_FIGHT_ELITE_TIMES = 35,		//35��Ӣ�ؿ�boss��ս	�����ս���⾫Ӣ�ؿ�boss����ʽ����ս����
	E_TASK_RUNE_TIMES = 36,				//36����ռ������	��ҷ���ϵͳռ���Ĵ�������ʽ��ռ������
	E_TASK_MERCENARY_STAR = 37,			//37Ӷ���Ǽ�	���Ӷ���ﵽ������Ǽ�����ʽ��Ӷ���Ǽ�
	E_TASK_VIP_LOGIN   = 38,            //38vip�ȼ�ΪX�����ÿ�յ�¼����	���VIP�ȼ��ﵽ�ļ��𡣸�ʽ��VIP�ȼ���ֵ

};

enum OPEN_ACTIVITY_TYPE
{
	//��Ӫ����� 1��½ 2�״γ�ֵ 3�Żݹ��� 4���ѷ���
	E_ACTIVITY_LOGIN = 1,							
	E_ACTIVITY_FIRST_CHARGE = 2,				
	E_ACTIVITY_DISCOUNT_BUY = 3,		
	E_ACTIVITY_CASH_BACK = 4,			
};

struct SKILL_EFFECT
{
	//��������,���Ա��,ֵ,�ٷֱ�;
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

//		this->byUseChest = byChest>> 7;				//��1λ
//		this->byChestRandNums = byChest & 127;		//��7λ

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
	WORD wQuality;//����ΪƷ��
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
	WORD wBase;//���ŵ�
	WORD wExtra;//Ӯ����

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

		//DWORD groupType;		//���䷽ʽ	group_type	int	
							//1ÿһ���������ʸ��Ե���һ�Ρ�(��roll)
							//2��������������ΪȨ��ֵ���������������һ����Ʒ			���䷽ʽΪ1ʱ�������Ϊ0-10000֮�����������ʾ0%-100%

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
							//���䷽ʽ	group_type	int	
							//1ÿһ���������ʸ��Ե���һ�Ρ�(��roll)
							//2��������������ΪȨ��ֵ���������������һ����Ʒ	

		
		vector<DWORD> vRandSum;
		map< WORD, 	GROUP_INFO > mapDropGroup;//key goroupid
	};


//ְҵ�������	�ȼ�	����	����	����	�ǻ�	����	�������ֵ	�������ֵ	������Сֵ	�������ֵ	������Сֵ	���ܹ������ֵ	���ܹ�����Сֵ	���ܷ������ֵ	���ܷ�����Сֵ	���ħ��ֵ	����ֵ	����ֵ	����ֵ	�����ֿ�ֵ	�����˺�����ֵ	�����˺�����ֵ	ÿ�غ�ħ���ָ�ֵ	ÿ�غ������ָ�ֵ	����	������Ѫ	��������	װ������	��ҵ���  ���˷���

	struct ATTR_UPGRADE_CFG
	{
		WORD job_id        ;//ְҵ�������
		WORD level		   ;//�ȼ�
		DWORD stamina	   ;//����
		DWORD strength	   ;//����
		DWORD agility	   ;//����
		DWORD intellect	   ;//�ǻ�
		DWORD spirit		   ;//����
		DWORD hp_max		   ;//�������ֵ
		DWORD atk_max	   ;//�������ֵ
		DWORD atk_min	   ;//������Сֵ
		DWORD def_max	   ;//�������ֵ
		DWORD def_min	   ;//������Сֵ
		DWORD spellatk_max  ;//���ܹ������ֵ
		DWORD spellatk_min  ;//���ܹ�����Сֵ
		DWORD spelldef_max  ;//���ܷ������ֵ
		DWORD spelldef_min  ;//���ܷ�����Сֵ
		DWORD mp_max		   ;//���ħ��ֵ
		DWORD hit		   ;//����ֵ
		DWORD dodge		   ;//����ֵ
		DWORD crit		   ;//����ֵ
		DWORD crit_res	   ;//�����ֿ�ֵ
		DWORD crit_inc	   ;//�����˺�����ֵ
		DWORD crit_red	   ;//�����˺�����ֵ
		DWORD mp_rec		   ;//ÿ�غ�ħ���ָ�ֵ
		DWORD hp_rec		   ;//ÿ�غ������ָ�ֵ
		DWORD luck		   ;//����
		DWORD att_hem	   ;//������Ѫ
		DWORD att_mana	   ;//��������
		DWORD equ_drop	   ;//װ������
		DWORD money_drop	   ;//��ҵ���
		DWORD luck_def;//���˷���
	};

//	���	����	������Դ	�ȼ�	����	����	���	��������	ս����	����	����	����	�ǻ�	����	����ֵ����	ħ��ֵ����	�������ֵ	������Сֵ	�������ֵ	������Сֵ	���ܹ������ֵ	���ܹ�����Сֵ	���ܷ������ֵ	���ܷ�����Сֵ	����ֵ	����ֵ	����ֵ	�����ֿ�ֵ	�����˺�����ֵ	�����˺�����ֵ	�����ָ�	ħ���ָ�	����	������Ѫ	��������
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

		WORD id;  // ����id
		string name; // ��������
		BYTE job; // ְҵ����
		WORD group; // ������
		WORD level; // ���ܵȼ�
		WORD unlock_level; // ������Ҫ��ɫ�ȼ�
		BYTE study; // ѧʿ��ʽ���Ƿ���Ҫ������ 0����Ҫ 1��Ҫ
		string description; //  ��������
		BYTE type; //  ��������
		vector< AttrValue > vecAttr;
		struct SKILL_PARA
		{
			BYTE skillType;     // ����/�˺�����
			BYTE skillTarget;   // ����/�˺�Ŀ��
			WORD skillAtt;		// ֵ,
			float skillCoff;		// ϵ��
			WORD skillMpAtt;	// ��ֵ,
			float skillMpCoff;	// ��ϵ��
		}skillPara;

		WORD mp_cost; // ħ������
		DWORD cooldown; // �ͷż��ܼ��
		WORD  priority; // �����ͷ����ȼ�
		WORD levelup_exp;//��������ʱ�䣨��λ������)
		BYTE target; // ������ЧĿ�� 1����2Ŀ��
		float eff_rate; // ����Ч������
		BYTE eff_type; // ����Ч������
		BYTE round; // Ч�������غ�

		float extra_coff;
		//vector< SKILL_EFFECT > vecEffectAttr;

		struct   SKILL_BOOK_ITEM
		{
			WORD itemid; // ������id
			WORD count; // ��������
			SKILL_BOOK_ITEM()
			{
				itemid = 0;
				count = 0;
			}
		}skill_book;

	};

	struct SKILL_EFF_CFG
	{
		//���	Ч�����ͱ��	Ч���ȼ�	Ч������	Ч������
		//id	type	          level	name	parameter_add

		WORD id;
		string name;
		BYTE type;
		BYTE level;
		vector< SKILL_EFFECT > vecEffectAttr;
	};

//���	����	������Դ	����	������	˵��	�ѵ�����	ְҵ����	�Ա�����	�ȼ�����	ϡ�ж�	���ۻ����۸�	�������	�������	����������ָ���	ս����	����	����	����	�ǻ�	����	�������ֵ	���ħ��ֵ	�������ֵ	������Сֵ	�������ֵ	������Сֵ	���ܹ������ֵ	���ܹ�����Сֵ	���ܷ������ֵ	���ܷ�����Сֵ	����ֵ	����ֵ	����ֵ	�����ֿ�ֵ	�����˺�����ֵ	�����˺�����ֵ	ÿ�غ�ħ���ָ�ֵ	ÿ�غ������ָ�ֵ	����	������Ѫ	��������	װ������	��ҵ���	����	ǿ��	��Ƕ�׸���	��װ

	struct ITEM_CFG 
	{
		WORD         id;			
		string       name;		
		WORD         asset;		
		DWORD        type;		
		WORD         sub_type;	
		//string       description;//�������һ��	
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

		DWORD        att_rate;	//100��ʾ100%������װ����ÿһ���������Ҫ���и��ʼ��
		WORD		 luck;
		BYTE	   addAttrType;
		DWORD      addAttrValue;
		//����	����	����	�ǻ�	����
		DWORD	stamina;
		DWORD	strength;
		DWORD	agility;
		DWORD	intellect;
		DWORD	spirit;
		DWORD   equ_drop;	
		DWORD   money_drop;

		//����	ǿ��	��Ƕ�׸���
		//luck_def	strengthen	hole_odds

		DWORD   luck_def;

		string  strengthen;

		BYTE attType;  // ��������
		DWORD attValue; // ����ֵ
		DWORD strengthen_stones; // ǿ��ʯ����

		BYTE zeroHole;
		vector<BYTE>  vecHole;
		vector<DWORD>  vecHoleRandSum;
		WORD suit;
		map< BYTE, DWORD >mapBaseAttrValue;//��ʯ��ӵ����� �Լ� ϴ�����������

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
			equip_extend_star forging_up_attr;// ����
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

			att_rate = 0;	//100��ʾ100%������װ����ÿһ���������Ҫ���и��ʼ��
			luck = 0;
			//battle_point = 0;
			addAttrType = 0;
			addAttrValue = 0;

			//����	����	����	�ǻ�	����
			stamina = 0;
			strength = 0;
			agility = 0;
			intellect = 0;
			spirit = 0;
			equ_drop = 0;	
			money_drop = 0;

			//����	ǿ��	��Ƕ�׸���
			//luck_def	strengthen	hole_odds

			luck_def = 0;

			attType = 0;  // ��������
			attValue = 0; // ����ֵ
			strengthen_stones = 0; // ǿ��ʯ����

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

	//���ߵ�����
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
		
		vector<float> vecMonsterRatio;	//ÿ�ֹ�ռ����,�������߼���0.3....
		
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
		size_t _curIndex;//���Բ���ʼ��
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
		DWORD upgrade_exp;	//������һ����Ҫ�ľ���

		DWORD sum_exp;		//ǰһ�ȼ��ۼƾ���
		
	};
	
	struct ATT_RAND_PARA
	{
		BYTE attr;
		DWORD valueMin;
		DWORD valueMax;
	};

	//struct ATT_RAND_GROUP
	//{
	//	WORD groupID;//mapRandGroup ��key
	//	WORD groupRate;//
	//	vector<DWORD> vRandSum;
	//	vector<ATT_RAND_PARA> vAttRandPara;
	//};

	struct ATT_RAND_CFG
	{
		WORD groupID;
		vector<ATT_RAND_PARA> vAttRandPara;

		//map<WORD, ATT_RAND_GROUP > mapRandGroup;		//key: group id
		//vector<DWORD> vRandSumGroup;						//ʹ�����ȷ������һ��
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
//����id	��װid	����Ҫ��	1��ǿ��Ҫ��	1����װ����	2��ǿ��Ҫ��	2����װ����	3��ǿ��Ҫ��	3����װ����	4��ǿ��Ҫ��	4����װ����	5��ǿ��Ҫ��	5����װ����	��װǰ׺	��װ��׺	��װ˵��

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
		//�ȼ�	����	��������	����ս���������	ҡǮ������	�������������	BOSS��ս����	����ս������	���ռ������	�̳Ƕ�����Ʒ��	����Ӣ��������	���Ĳ۶��⿪��	��������2	ÿ�յ�¼��Ԫ��
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

		vector<float> vecMineIndexRatio;	//�����ڿ�һ�α������ʱ����
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

	struct SHOP_STORAGE//ÿ������shop1�ĵ���id��Ӧ�˽ṹ
	{
		vector<WORD> vecGroupID;//��ID
		vector<DWORD> vecGroupRandSum;//�����
		map<WORD, SHOP_GRUOP> mapGroupShop;//ÿ��������Ӧ�ĵ�����ṹ
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
		DWORD coin;//���ñ�����gold
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
		DWORD need_exp; // �������辭��
		DWORD stamina; //����	
		DWORD strength; //����										
		DWORD agility; //����	
		DWORD intellect; //�ǻ�
		DWORD spirit;	//����	
		DWORD hp_max;	//�������ֵ	
		DWORD mp_max;	//���ħ��ֵ	
		DWORD atk_max; //�������ֵ	
		DWORD def_max;	//�������ֵ
		DWORD spellatk_max;	 //���ܹ������ֵ
		DWORD spelldef_max; //���ܷ������ֵ	
		DWORD hit; //����ֵ	
		DWORD dodge; //����ֵ	
		DWORD crit; //����ֵ	
		DWORD crit_res; //�����ֿ�ֵ	
		DWORD crit_inc;	//�����˺�����ֵ
		DWORD crit_red; //�����˺�����ֵ
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

//ְҵ�������	�ȼ�	����	����	����	�ǻ�	����	�������ֵ	���ħ��ֵ	�������ֵ	������Сֵ	�������ֵ	������Сֵ	���ܹ������ֵ	���ܹ�����Сֵ	���ܷ������ֵ	���ܷ�����Сֵ	����ֵ	����ֵ	����ֵ	�����ֿ�ֵ	�����˺�����ֵ	�����˺�����ֵ	ÿ�غ�ħ���ָ�ֵ	ÿ�غ������ָ�ֵ	����	������Ѫ	��������	װ������	��ҵ���	���˷���
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

	//���	�����⻷1	�⻷����	�⻷1����˵��	��������
	//id	halo	halo_name	halo_info	halo_open
	struct MERHALO_CFG
	{
		WORD  id;
		BYTE  byStar;
		BYTE  byMoneyType;
		DWORD dwMoneyCost;
		vector< SKILL_EFFECT > vecEffectAttr;
	};

	//���	ְҵ�������	���ŵȼ�	����	��ʼ����	�Ա�	����	ͷ��	�����⻷1
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

	//�Ǽ�	����	����	����	����	����	������������
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

	//ѵ����ҳ	��Ҫ����	����Ǯ������	����Ǯ������	�ϳ�Ŀ������	vip����									
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

	//��ʯID	��ʯ����	��������	����Ŀ��	�ֽ��ý�Ǯ	�ֽ�����Ƭ	�ֽ�tips����
	//gem_id	gem_name	upgrade	taget	decompose_money	decompose_item	decompose_tips	
	struct GEM_CFG
	{
		WORD gem_id;
		//string	gem_name;
		WORD upgrade_id;//��������id
		WORD upgrade_num;//������������
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
	//��������	����	vip�ȼ�����	����Ǯ������	����Ǯ������	���Ա������
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

	//����	ְҵ	��Ʒid	��Ʒ����	��Ʒ����	��ƷƷ��	��Ʒǿ���ȼ�	��Ʒ��������1	��Ʒ��������2	��Ʒ��������3	��Ʒ��������4	��Ʒ��������5	��Ʒ��Ƕ��
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
	// ����Ʒ�����һ������
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
