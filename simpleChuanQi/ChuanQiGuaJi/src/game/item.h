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

//��������
//����Ĭ�Ͽ���60�񡢲ֿ�Ĭ�Ͽ���60�񡢿�ʯ����Ĭ�Ͽ���60��
//��ͨ������ķ�ʽ��������200�񱳰���200��ֿ⣬200���ʯ������

#define DEFAULT_PKG_NUMS_EQUIP		 30		//����Ĭ�Ͽ���30��
#define DEFAULT_PKG_NUMS_PROP		 80		//����Ĭ�Ͽ���30��

#define MAX_PKG_NUMS				 100
#define MAX_ITEM_NUMS				 100	    //���߱���
#define DEFAULT_MINE_PKG_NUMS		  30		//��ʯĬ�Ͽ���30��
#define MAX_MINE_PKG_NUMS			 100
#define MAX_FUWEN_PKG_NUMS			 100		// ���ı�������
#define MAX_EQUIP_ATTR_NUMS			(10)		//װ���ϵ��������10��,7����������,3����������,�������Բ���ǿ���ȼ�Ӱ��
#define MAX_EQUIP_BASE_ATTR_NUMS	(36)
#define MAX_EQUIP_EXTRA_ATTR_NUMS	 (5)
#define MAX_HOLE_NUMS	(5)
#define SKILL_MAX_NUM				 (4)		//Ӷ������
#define HALO_SKILL_NUM				 (3)		//Ӷ���⻷
#define  MERCENARY_NUM				   3		//Ӷ����
#define  FRESH_MERCENARY_NUM		   3		//ˢ��Ӷ��������
#define  MER_BODY_GRID_NUM			 (4)		//������4��
#define  MYSTERY_SHOP_NUM			   2		//�����̳�������
#define MAX_SKILL_NUMS				 (10)		//����8�飬������������
#define MAX_ARENA_RECORD			 (10)		//��������¼
#define QUICK_SKILL_INDEXS			  (4)		//���ܿ������
#define MAX_FRIENDS_NUMS			 (30)		//����id���30��
#define MAX_TASK_NUMS				 (40)		//�����������
#define MAX_TASK_OPEN_ACTIVITY_NUMS  (10)		
#define MAX_DIANZHAN_NUMS			  (5)
#define BODY_GRID_NUMS				 (15)		//������15��
#define MER_GRID_NUMS				  (4)		//������4��
//#define MAX_MERGE_NUMS			 (99)		//����������
#define MAX_SHOP_ITEM				  (6)		//��Ʒ����
#define WORLD_BOSS_HELP_NUM			  (6)		//����boss helper����
#define MIN_ELITE_MAP_ID		   (1001)		//��Ӣ�ؿ���Сֵ
#define MAX_ELITE_GROUP				 (20)       //��Ӣ�ؿ��������
#define MAX_CHARGE_TYPE				 (10)       //��ֵ����
#define MAX_CODE_AWARD_NUM			 (10)       //��֤����ȡ����

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

//Ӷ��
	//MERCENARY_EQUIP_WEAPON = 1,	//����
	//MERCENARY_EQUIP_CLOTH = 2,	//�·�
	//MERCENARY_EQUIP_HELMET = 3,	//ñ��
	//MERCENARY_EQUIP_RING = 8,	//��ָ
	
//-------------------------------

//1װ����2����
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
	SUB_TYPE_ITEM_SKILLBOOK = 5,//������

	SUB_TYPE_ITEM_MINE = 6,
	
	SUB_TYPE_ITEM_BLACK_IRON = 7,		//7������8���ʯ"
	SUB_TYPE_ITEM_DIAMON = 8,	//8���ʯ"

	SUB_TYPE_ITEM_COIN = 9,		//9���
	SUB_TYPE_ITEM_INGOT = 10,	//10Ԫ��
	SUB_TYPE_ITEM_BOX = 11,		//11�������

	SUB_TYPE_ITEM_FIXED_BOX = 12,		//12�̶����䡢
	SUB_TYPE_EXP_DAN = 13,				//13���鵤��
	SUB_TYPE_FAC_CONTRIBUTE_DAN = 14,	//14��ṱ�׵���

	SUB_TYPE_COIN_CARD = 15,	//15��ҿ���
	SUB_TYPE_INGOT_CARD = 16,	//16Ԫ������
	SUB_TYPE_BLESS_OIL = 17,		//17ף���͡�

	ITEM_STYPE_VIP_CARD = 18, //vip���鿨
	ITEM_STYPE_BOSS_CARD = 19, //BOSS��ս��
	SUB_TYPE_ITEM_HONOUR = 20,		//����

	SUB_TYPE_EQUIP_GEM_RED =  21, //�챦ʯ
	SUB_TYPE_EQUIP_GEM_YELLOW =  22, //�Ʊ�ʯ 
	SUB_TYPE_EQUIP_GEM_BLUE =  23, //����ʯ
	SUB_TYPE_EQUIP_GEM_GREEN = 24, //�̱�ʯ
	SUB_TYPE_EQUIP_GEM_PURPLE =  25, //�ϱ�ʯ
	SUB_TYPE_FUWEN =  26, //����
	SUB_TYPE_RESET_STONE = 28, //ϴ��ʯ
	SUB_TYPE_LINGZHU_USE = 99,		//99����ʹ��


//"װ���Ĺ������ͣ�1����,2�·���3ͷ����4������5Ь�ӣ�6������7���ף�8��ָ��
//9ѫ�£�10��ʯ��11����

//���ߵĹ������ͣ�1���ϣ�2�ָ���3�౶���飬4��ʹ�ã�5�����飬6��ʯ��7������8���ʯ"
//9��ң�10Ԫ����11�������


};


enum
{
	//EQUIP_WEAPON = 1 --����
	//EQUIP_CLOTHES = 2 --����
	//EQUIP_HAT = 3 --ñ��
	//EQUIP_BELT= 4 --����
	//EQUIP_SHOES = 5 --Ь��
	//EQUIP_NECKLACE = 6 --����
	//EQUIP_BRACLET = 7 --����(����)
	//EQUIP_RING = 8 --��ָ
	//EQUIP_MEDAL = 9 --ѫ��
	//EQUIP_STONE = 10 --��ʯ
	//EQUIP_PEAL = 11 --����

	BODY_INDEX_WEAPON = 0,//������
	BODY_INDEX_CLOTH = 1,//���·�
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
};//13��װ��


enum ATTR_TYPE
{
	//Ӷ�����Ը���֪ͨ,����������
#if 0
		ROLE_PRO_WARPWR = 106, //ս��ֵ
		ROLE_PRO_STAR = 119, //Ӷ���Ǽ�

		ROLE_PRO_CUR_HP = 1, //Ѫ��(û��?����)
		ROLE_PRO_MAX_HP = 2, //���Ѫ��
		ROLE_PRO_CUR_MP = 3, //��ǰħ��
		ROLE_PRO_MAX_MP = 4, //���ħ��
		ROLE_PRO_ATK_MAX = 5,//����������
		ROLE_PRO_ATK_MIN = 6,//����������
		ROLE_PRO_MAG_ATK_MAX = 7,//���ħ������
		ROLE_PRO_MAG_ATK_MIN = 8, //��Сħ������

		ROLE_PRO_SOL_ATK_MAX = 9, //����������,sol_max(����)
		ROLE_PRO_SOL_ATK_MIN = 10, //��С��������,sol_min(����)

		ROLE_PRO_PHY_DEF_MAX = 11, //������, phydef_max
		ROLE_PRO_PHY_DEF_MIN = 12, //��С����, phydef_min
		ROLE_PRO_MAG_DEF_MAX = 13, //���ħ������, magdef_max
		ROLE_PRO_MAG_DEF_MIN = 14, //��Сħ������, magdef_min
		ROLE_PRO_PHY_DODGE = 15, //����ر���

		ROLE_PRO_MAG_DODGE = 16, //ħ���ر���(����)
		ROLE_PRO_SOL_DODGE = 17, //�����ر���(����)

		ROLE_PRO_ATK_SPEED = 18, //�����ٶ�
		ROLE_PRO_REC_HP = 19, //�����ظ�
		ROLE_PRO_REC_MP = 20, //ħ���ظ�
		ROLE_PRO_PHY_HIT = 21, //����������
		ROLE_PRO_STAMINA = 22, //�����ı�
		ROLE_PRO_STRENGTH = 23, //�����ı�
		ROLE_PRO_AGILITY = 24, //���ݸı�
		ROLE_PRO_INTELLECT = 25, //�ǻ۸ı�
		ROLE_PRO_SPIRIT = 26, //����
		ROLE_PRO_CRIT_VAL = 27, //����ֵ
		ROLE_PRO_CRIT_DEF = 28, //�����ֿ�
		ROLE_PRO_CRIT_APPEND = 29, //�����˺�����ֵ
		ROLE_PRO_CRIT_RELIEF = 30, //�����˺�����ֵ
		ROLE_PRO_SUCK_HP = 31, //������Ѫ
		ROLE_PRO_SUCK_MP = 32, //��������
		ROLE_PRO_EQUIP_DROP = 33, //װ������
		ROLE_PRO_COIN_DROP = 34, //��ҵ���
#endif
	ROLE_PRO_COIN = 101,//���
	ROLE_PRO_INGOT = 102,//Ԫ��

	ROLE_PRO_CUREXP = 103, //��ǰ����ı�
	ROLE_PRO_MAX_HERO = 104,//���Ӣ�ؿ�
	ROLE_PRO_POWER = 105,	//�����ı�
	ROLE_PRO_WARPWR = 106, //ս��ֵ

	ROLE_PRO_LEVEL = 107, //�ȼ��仯
	ROLE_PRO_FACTION = 108, //���ᷢ���ı�


	ROLE_PRO_TITLE = 109, //��óƺ�
	ROLE_PRO_MAX_MAP = 110, //ͨ�ص�ͼ�ı�
	ROLE_PRO_FACTION_JOB = 111, //����ְλ�ı�


	ROLE_PRO_FACTION_CONTRIBUTE = 112, //�ﹱ�ı�
	ROLE_PRO_MAX_EXP = 113, //��������
	ROLE_PRO_HONOUR = 114, //����
	ROLE_PRO_SMELTVAR = 115, //����ֵ����
	ROLE_PRO_VIPVAR = 116, //�ۼƳ�ֵ
	ROLE_PRO_VIP_LEVEL = 117, //vip�ȼ�����

ROLE_PRO_STAR = 119, //Ӷ���Ǽ�
	//	ROLE_PRO_POWER = 201,				//--װ������ߵ�ս������

	//E_ATTR_HP = 1,//����ֵ ����
	E_ATTR_HP_MAX2		 =  2,//�������ֵ
	//E_ATTR_MP = 3,//ħ��ֵ
	E_ATTR_MP_MAX2	=  4,//ħ������
	E_ATTR_ATK_MAX2	  =  5,//�������ֵ
	E_ATTR_ATK_MIN2  =  6,//������Сֵ
	E_ATTR_SPELLATK_MAX  =	7,//���ܹ������ֵ
	E_ATTR_SPELLATK_MIN  =	8,//���ܹ�����Сֵ
	E_ATTR_SOL_MAX = 9,//������������
	E_ATTR_SOL_MIN = 10,//������������
	E_ATTR_DEF_MAX	     =  11,//�������ֵ
	E_ATTR_DEF_MIN	     =  12,//������Сֵ
	E_ATTR_SPELLDEF_MAX  =  13,//���ܷ������ֵ
	E_ATTR_SPELLDEF_MIN  =  14,//���ܷ�����Сֵ
	E_ATTR_DODGE		 =	15,//����ֵ
	E_ATTR_MAG_DODGE = 16,//ħ������
	E_ATTR_SOL_DODGE = 17,//��������
	E_ATTR_SPEED = 18,//�����ٶ�
	E_ATTR_MP_REC2		 =	19,//ÿ�غ�ħ���ָ�ֵ
	E_ATTR_HP_REC2		 =	20,//ÿ�غ������ָ�ֵ
	E_ATTR_HIT			 =	21,//����ֵ
	E_ATTR_STAMINA	     =  22,//����//һ������
	E_ATTR_STRENGTH	     =  23,//����//һ������
	E_ATTR_AGILITY	     =  24,//����//һ������
	E_ATTR_INTELLECT     =  25,//�ǻ�//һ������
	E_ATTR_SPIRIT		 =  26,//����//һ������
	E_ATTR_CRIT		     =	27,//����ֵ
	E_ATTR_CRIT_RES	     =	28,//�����ֿ�ֵ
	E_ATTR_CRIT_INC	     =	29,//�����˺�����ֵ
	E_ATTR_CRIT_RED	     =	30,//�����˺�����ֵ
	E_ATTR_ATT_HEM		 =	31,//������Ѫ
	E_ATTR_ATT_MANA	     =	32,//��������
	E_ATTR_EQU_DROP	     =	33,//װ������
	E_ATTR_MONEY_DROP	 =	34,//��ҵ���
	E_ATTR_LUCK		     =	35,//����
	E_LUCK_DEF			 =  36,//���˷���
	E_ATTR_MAX,//�����������
	
	DEC_DIZZY_TIMES = 50,// ����ѣ�λغ�
	HURT,// �̶��˺�
	HURT_DOWN,// �̶�����
	BOSS_HURT,// ��boss�˺�����
	SKILL_HURT,// ���̶ܹ��˺�
	SKILL_HURT_DOWN,// ���̶ܹ�����
	CONSUME_MP_DOWN,// ���Ľ���
	E_ATTR_LINGZHU = 201,//����
};


enum E_TASK_STATUS
{

	E_TASK_UNCOMPLETE = 0,						//0δ��ɣ�
	E_TASK_COMPLETE_CAN_GET_PRIZE = 1,			//1�����δ�콱��
	E_TASK_COMPLETE_ALREADY_GET_PRIZE = 2,		//2��������콱(˲̬�����̽�����һ���������񣬳�����ĩβ)
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

//////////////////////////////////////////////װ����չ����///////////////////////////////////////////////////////
#define MAX_EQUIP_EXTEND_COUNT 20
enum class e_equip_extend_attr : byte
{
	dec_dizzy_times,// ����ѣ�λغ�
	hurt,// �̶��˺�
	hurt_down,// �̶�����
	boss_hurt,// ��boss�˺�����
	skill_hurt,// ���̶ܹ��˺�
	skill_hurt_down,// ���̶ܹ�����
	consume_mp_down,// ���Ľ���
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

	BYTE byStatus:1; 		//����״̬��Ϣ
	BYTE byQuality:7;	//�ϳɵȼ�
	BYTE byStrengthLv;	//ǿ���ȼ�

	BYTE byLuckPoint;		//
	BYTE byLuckDefPoint;
	BYTE byHole;
	WORD wStone[ MAX_HOLE_NUMS];

	AttrValue attrValueExtra[ MAX_EQUIP_EXTRA_ATTR_NUMS ];

	DWORD dwGetStrengthStoneNum; // ʹ��ǿ��ʯ����
	byte star_level;
	equip_extend equip_extends[3];

	ZERO_CONS(ITEM_DATA)

	void initFromItemCache(const ITEM_CACHE* itemCache);
};

struct stIndexItem
{
	BYTE byStatus; // ״̬ 0δ���� 1����
	WORD skillid; // ����id
	WORD group;//������

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
	stIndexItem vIndexSkills[ SKILL_MAX_NUM ]; // ����
	ITEM_DATA  vBodyEquip[ MER_BODY_GRID_NUM ];
	WORD wID;
	BYTE byFight;
	BYTE  byJob;
	BYTE  byStar;
	BYTE bySex;
	WORD wLevel;
	//DWORD merAttr[E_ATTR_MAX];
	int attrValueTrain[ BASE_ATTR_NUM ];
	//DWORD dwFightValue;   //ս����
	stIndexItem vHaloSkills[HALO_SKILL_NUM]; // �⻷����
	DWORD dwStarExp;
	ZERO_CONS(MERCENARY_DATA);

	void initFromMerCache(const CMercenary* pCmer);
};

struct  MYSTERY_SHOP_INFO
{
	WORD  wFreeTimes;//��Ѵ���
	BYTE  byFirst;//�Ƿ��һ�ι��� 1�� 0��
	DWORD dwCanTreeTime;//������ʱ��
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
	BYTE status; // ����λ���Ƿ񿪷ţ�0δ���ţ�1���� 
	DWORD exp; // ��ǰ����
	WORD runeid; // ����id
	BYTE level; // ���ĵȼ�
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
	DWORD itemNum;//����	
};

struct ITEM_CACHE
{
	CONFIG::ITEM_CFG * itemCfg;

	DWORD itemNums;//����

	BYTE byStatus:4; 		//����״̬��Ϣ
	BYTE byExtraAttrNums:4;	//���漫Ʒ������Ŀ�������豣��,

	BYTE byStrengthLvRes:4;	//ǿ���ȼ���50 ����
	BYTE byQuality:4;		//Ʒ��
	BYTE byStrengthLv;	//ǿ���ȼ� // ���ĵȼ�
	//BYTE byNeedUpdateClient:1;	//
	BYTE byDropAndSell:1;	//���䲢������,cache, �������ݿ�

	BYTE byLuckPoint:7;	//��������,����,ף���͸���

	BYTE byLuckDefPoint;//�������˷���,����,ף���͸���

	DWORD dwLingZhuExp;	//������ܾ���

	DWORD dwFightValue; // װ��ս�� ���ľ���

	BYTE byHole; //�Ѿ����Ŀ�����

	BYTE byPkgType;

	WORD wStone[MAX_HOLE_NUMS]; //��ʯ��Ƕ��,��ʯID; 0:δ��Ƕ��ʯ

	AttrValue attrValueBase[ MAX_EQUIP_BASE_ATTR_NUMS ];			//�ⲿ�ֿ��Բ�����,ֻcache 
	AttrValue attrValueExtra[ MAX_EQUIP_EXTRA_ATTR_NUMS ];			//�������� ������	
	AttrValue attrOldValueExtra[ MAX_EQUIP_EXTRA_ATTR_NUMS ];		//��ʼ��ʱ��õ�������

	DWORD dwGetStrengthStoneNum;  // ʹ��ǿ��ʯ����

	byte star_level;
	equip_extend equip_extends[3];
	equip_extend equip_extends_for_skill;

	ZERO_CONS(ITEM_CACHE);
	void resetItemAttrValue();
	void resetAttrValueBase();//���û�������(��������)
	void resetEquipExtraPro();//���ö������ԣ�һ�����ԣ�

	void setItemBasePro();	//��ȡ���ñ�Ļ�������
	void randEquipExtraPro();//����Ʒ�ʻ�ȡ��������
	//void randEquipExtraProChest(BYTE Chest1_RandNums7 );
	void setAttrValueExtra( const int randTimes );

	//void transformExtraAttr();

	// �Ƿ�ﵽǿ�����ȼ�
	bool isStrengthenMaxLevel();
	// ǿ������
	void levelUpStrengthen();


	void calEquipFightValue( );
	
	DWORD getExtraNums();

	// ��λ�Ƿ�����
	bool isHoleFull();
	// ������Ľ��������
	DWORD PuchHoleStoneCount();
	// ��׳ɹ�
	void PuchOneHole();
	// ��λ�Ƿ��ѿ���
	bool isHoleOpen(BYTE pos);
	// λ���Ƿ�����Ƕ
	bool isHoleInjected(BYTE pos);
	// �Ƿ��Ѿ���Ƕͬ���ͱ�ʯ
	bool hasInjectedSameTypeGem(WORD itemid);
	// ��Ƕ��ʯ
	bool injectEquipGem(BYTE pos, WORD gemid);
	// ��ÿ�λ��Ƕ�ı�ʯ
	WORD getInjectedGem(BYTE pos);
	// ж�ر�ʯ
	void unInjectGem(BYTE pos);
	// ������б�ʯ
	std::vector<WORD> getAllInjectGems();
	// ж�����б�ʯ
	void unInjectAllGems();
	// װ��ϴ��
	void equipReset(  const BYTE reset_type  );
	//// �Ƿ�װ��ϴ������
	//bool isequipResetAdd(DWORD& value);

	// ��ò���������ֵ
	DWORD getOutputSmeltValue();
	// ���ǿ��ʯ����
	DWORD getOutputStrengthenStoneCount();
	// �����װ��Ƭ����
	DWORD getOutputSuitPatchCount();
	// ��ò�����Ʒ��ֵ
	DWORD getOutputQualityValue();

	// ��÷��ľ���
	DWORD getDivineExp();
	
	ITEM_CACHE(const ITEM_CACHE* itemCache);


	ITEM_CACHE(const ITEM_DATA* itemData);
	//ITEM_CACHE(const MINE* itemMine);

	ITEM_CACHE(CONFIG::ITEM_CFG* itemCfg, size_t itemNums, BYTE byQuality = 1);//, BYTE byChest_chestRandNums=0 );//��1λ:ʹ��chest,��7λ��������

};


struct PRACTISE_SKILL
{
	WORD  wID;				//����id
	DWORD dwStartTime;		//�����������ʱ�䣬0��ʾû������
	DWORD dwReqTime;		//ʣ������ʱ�䣬���ñ������Ҫ����60
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
	BYTE  byResult;			//���
	char szName[NAME_MAX_LEN];	
	DWORD dwRank;		    //��������
	DWORD dwTime;		    //����ʱ��
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
// ż��PK���
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

// ż��PK��¼
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

//��������
struct MAIN_TASK_INFO
{
	WORD wTaskID;

	BYTE byTaskCompleteNums;	//��ǰ����������ɴ���

	BYTE byTaskStatus;	//0δ��ɣ�1�����δ�콱��2��������콱(˲̬�����̽�����һ���������񣬳�����ĩβ)

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


#define ROOM_MINER_NUMS		(4)		//ͬ���仹������4�����ڿ�

struct MINER
{
	DWORD dwRoleID;	//0��ʾ��λ�ÿ�ȱ,startTime��ʾ��ȱ��ʼʱ��
	DWORD startTime;	//�ڿ�ʼʱ��,
	//DWORD remainTime;	//�ڿ�ʣ��ʱ��,(��)

	ZERO_CONS(MINER)
};

#pragma pack(push, 1)

class ROLE_BLOB
{
public:
	BYTE res_[128];

	BYTE curEquipPkgLimit;		//��ǰ���İ���������
	BYTE curFuwenPkgLimit;	//��ǰ���Ĳֿ������
	BYTE curMineLimit;		//��ǰ���Ŀ��������
	BYTE curItemPkgLimit;		//��ǰ���Ŀ��������


	ITEM_DATA arrBody[BODY_GRID_NUMS];		
	ITEM_DATA arrPkg[MAX_PKG_NUMS];		
	ITEM_DATA arrProps[MAX_ITEM_NUMS];
	ITEM_DATA arrFuwen[MAX_FUWEN_PKG_NUMS];

	//MINE arrMine[MAX_MINE_PKG_NUMS];		//100*3

	
	WORD wOnUpSkill;						//������������ID,û��Ϊ0
	WORD wFightSkill[QUICK_SKILL_INDEXS];  //��ݼ�������
	WORD arrSkill[MAX_SKILL_NUMS];//10*10 �����б�

	DWORD arrFriendsID[MAX_FRIENDS_NUMS];   

	ARENA_RECORD arrArenaRecord[MAX_ARENA_RECORD];

	WORD wLastFightMapID;//�һ���ͼ
	WORD wCanFightMaxMapID;//��ǰ���Դ�ĵ�ͼ�ɣ�
	WORD wWinMaxMapID;

	WORD wLastFightHeroID;		//��Ӣ�ؿ� ����ս���ĵ�ͼ
	WORD wCanFightMaxHeroID;	//��Ӣ�ؿ� ��ǰ���Դ�ĵ�ͼ(��Ŵ�������ĵ�ͼ�����ܴ�)
	WORD wWinMaxHeroID;			//��Ӣ�ؿ� ʤ������ͼ

	BYTE byTitleId;

	BYTE bySex;

	DWORD tLogoutTime;		//�˳�ʱ��
	DWORD arrDianZhanIDs[MAX_DIANZHAN_NUMS];   

	BYTE byPredictIndex;
	PREDICT_EARNING predictEarning[PREDICT_NUMS];

	//DWORD dwComID;//����������id ��Ҫ����

	DWORD dwAutoSellOption;

	DWORD dwMiningRemainTime;//�����ڿ�ʣ��ʱ��
	BYTE byMiningSlotIndex;//�ڿ�Ŀ�λ

	MINER vMiner[ROOM_MINER_NUMS];//ͬ���������ڿ���
	SHOP_ITEM vShopItem[MAX_SHOP_ITEM];//�̳ǿ�����Ʒ

	BYTE bySendFightBossTimes;												//��սboss����ÿ������0������
	BYTE byDailyBuyBossTimes;												//������սboss����������
	BYTE byAllBuyBossTimes;

	BYTE bySendFightHeroTimes;												//��Ӣ�ؿ�
	BYTE byDailyBuyHeroTimes;												//������սhero����������
	BYTE byAllBuyHeroTimes;	

	DWORD dwShopItemTimes;//�̳�ˢ��
	BYTE byBuyQuickFightTimes;//�������ս������

	BYTE byEnhanceTime;//�������
	BYTE byFreshTimes;//ˢ��Ӷ�����ܴ���
	BYTE bySignTimes;//continue sign times per month
	DWORD dwDaySignStatus;//day sign status bit
	BYTE byTimeSignStatus;// times sign status bit

	MAIN_TASK_INFO tasksInfo[MAX_TASK_NUMS];

	WORD wTmpVipLevel;
	DWORD dwTmpVipEndTime;//��ʱVIP ʧЧʱ��

	DWORD dwSmeltValue;//����ֵ

	BYTE byForgeRreshTimes;	//��������Ѿ�ˢ�´���,����

	ITEM_DATA itemDataForge;	//������浱ǰװ��
	BYTE byNewMapKillMonNum;

	BYTE byCurLevelAwardStep; //���ȼ���ȡ��������
	// �Ѿ�������Ʒ��ֵ
	DWORD _res;

	MERCENARY_DATA arrMerData[ MERCENARY_NUM ];
	
	BYTE divine_status; //ռ��״̬ 0�� 1�� 2��
	BYTE divine_times; // ռ������
	stRuneItem rune_items[8];

	BYTE byUseMoneyTreeTimes;

	MYSTERY_SHOP_INFO m_vMysInfo[MYSTERY_SHOP_NUM];//�������˵���Ϣ

	DOUBLE_WORD m_vEliteGroupInfo[MAX_ELITE_GROUP];//��Ӣ�ؿ���
	BYTE  byNewbieGuideStep;											//������������
	DWORD dwFisrtLoginTime;												//��ʼ��¼ʱ��

	MAIN_TASK_INFO openActivityInfo[MAX_TASK_OPEN_ACTIVITY_NUMS];
	
	DWORD arrFirstCharge[MAX_CHARGE_TYPE];

	BYTE  byRechargeState;

	DWORD close_user_valid_time;										// �����Чʱ��
	BYTE deny_chat;														// ����
	BYTE arrCodeAwardTypes[MAX_CODE_AWARD_NUM];							//�һ�����ȡ

	unsigned pkvalue;													// PKֵ
	unsigned pk_count;													// PK����
	unsigned pk_buy_count;													// PK����
	unsigned pk_dead_count;													// PK��������
	unsigned last_pk_count_inc_time;						// ���pk��������ʱ��
	meet_pk_record meet_pk_records[20];
	unsigned world_boss_battle_attack_index;		// �����Ϲ�ս���������
	BYTE login_day_num;								//�ۼƵ�½����

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
