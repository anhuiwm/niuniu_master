#pragma once
#include "config.h"
#include "item.h"


enum
{
	MERCENARY_EQUIP_WEAPON = 0,	//武器
	MERCENARY_EQUIP_CLOTH = 1,	//衣服
	MERCENARY_EQUIP_HELMET = 2,	//帽子
	MERCENARY_EQUIP_RING = 3,	//戒指
};

enum FIGHT_STATUS
{
	E_FIGHT_DOWN = 0,//佣兵下阵
	E_FIGHT_ON  = 1,//佣兵上阵
};

#define notifyMerAttr( role, merID, type, field  )	 \
	do {\
	string strData;\
	S_APPEND_WORD( strData, merID );\
	S_APPEND_BYTE( strData, type );\
	S_APPEND_DWORD( strData, this->field );\
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_PRO_UPDATE, strData) );\
	}while(0)

#define notifyMerChangeAttr( role, merID, type, field, oldValue  )	\
	do {\
	if ( oldValue != this->field )\
	notifyMerAttr( role, merID, type, field );\
	}while(0)

#define notifyMerFightValue(role,merID) 	notifyMerAttr( role, merID, ROLE_PRO_WARPWR, dwFightValue )

#define notifyMerStar(role,merID) 	notifyMerAttr( role, merID, ROLE_PRO_STAR, byStar )

namespace NETMERCENARY
{

	#pragma  pack( push, 1)
		struct TAttrExtraAdd
		{
			BYTE attr; 
			WORD addBefore;
			WORD addAfter;
			TAttrExtraAdd( BYTE attr, float base, float add )
			{
				this->attr = attr;
				this->addBefore = WORD( base * 100 );
				this->addAfter = WORD( add * 100 );
			}
		};
	#pragma pack(pop)

	int clientGetMerInfo( ROLE* role, unsigned char * data, size_t dataLen );//获取佣兵信息
	int clientMerUpDown( ROLE* role, unsigned char * data, size_t dataLen );//佣兵上阵
	int clientRefreshSkills( ROLE* role, unsigned char * data, size_t dataLen );//刷新佣兵技能
	int clientGetSkills( ROLE* role, unsigned char * data, size_t dataLen );//获取佣兵技能列表
	int clientTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen );//佣兵培养
	int clientPreTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen );//
	int clientPreViewMerStar( ROLE* role, unsigned char * data, size_t dataLen );//
	int clientConfirmMerStar( ROLE* role, unsigned char * data, size_t dataLen );//
	int clientPreViewComposeBook( ROLE* role, unsigned char * data, size_t dataLen );//
	int clientConfirmComposeBook( ROLE* role, unsigned char * data, size_t dataLen );//合成书本
	int clientGetMerHaloList( ROLE* role, unsigned char * data, size_t dataLen );//获取光环技能列表
	int clientActiveMerHalo( ROLE* role, unsigned char * data, size_t dataLen );//激活光环
	int clientMerEquipAttach( ROLE* role, unsigned char * data, size_t dataLen );//穿装备
	int clientGetTrainMercenaryBaseAttr( ROLE* role, unsigned char * data, size_t dataLen );
}


class CMercenary
{

public:

	CMercenary( const MERCENARY_DATA* pMerData );

	CMercenary( ROLE* role, const CONFIG::MERDEF_CFG* merDefCfg );

	~CMercenary();

	vector< stIndexItem > vecHaloSkills; // 光环技能

	map<WORD, WORD> mapSuit;//拥有套装ID及其数量

	vector< WORD > vecSuit;//已经得到的套装

	WORD wID;//memset 分隔符

	stIndexItem vIndexSkills[ SKILL_MAX_NUM ]; // 技能索引

	int attrValueTrain[ BASE_ATTR_NUM ];//培养所得五项基本属性

	int tempAttrValuetrain[ BASE_ATTR_NUM ];//暂存  每次所得五项基本属性

	ITEM_CACHE* vBodyEquip[ MER_BODY_GRID_NUM ];

	BYTE byFight;//是否战斗

	BYTE  byJob;

	BYTE  byStar;//星级

	BYTE bySex;

	WORD wLevel;

	DWORD merAttr[E_ATTR_MAX];

	DWORD dwFightValue;   //战斗力
	
	DWORD dwStarExp;	//用于升星的剩余经验

	WORD suitStrengthLevel;//身上装备等级和

	bool operator == ( const WORD& dst )const
	{
		return ( this->wID == dst );
	}

	void calMerFightValue( );//计算佣兵战斗力

	void updateNotifyMerAttr( ROLE* role );//计算并通知佣兵所有属性

	void calMerAttr( );//重新计算佣兵所有属性

	void tranformBaseAttr(const BYTE& type, const DWORD& value);

	void setMerAttr( CONFIG::MERCENARY_CFG* mercenaryCfg );

	void setFightStatus( BYTE byFight );

	BYTE getFightStatus( );

	void setStar( BYTE byStar );

	void unLockSkills();

	void setSoltSkill( BYTE bySolt, WORD wSkill );

	void refreshSkills( BYTE* vLock );

	void setSoltSkillRand(  BYTE bySolt, WORD wSkill  );

	void updateNotifyFightValue( ROLE* role );

	void updateNotifyStar( ROLE* role );

	void addSuitExtraAttr();

};

class CMercenaryMgr
{

public:

	 vector< CMercenary > m_vecCMercenary;

	 CMercenaryMgr(  ROLE* pRole  );

	 ~CMercenaryMgr();

	 void setMercenay();

	 void addMercenary( WORD wRoleLevel );//获得佣兵 人物升级用 更新佣兵等级

	 void addMercenary( const CONFIG::MERDEF_CFG* merDef );//获得佣兵

	 void delMercenary();//no use

	 int equip( ROLE* role, ITEM_CACHE* itemCache, int srcPkgType, int &tgtBodyIndex, WORD wMercenary );

	 void swapEquip(ROLE* role, vector<CMercenary>::iterator itMer, int bodyIndex, list<ITEM_CACHE*>& lstDst, list<ITEM_CACHE*>::iterator pkgItemIt,  ITEM_CACHE* itemCache);

	 int getBodyIndexBySubtype( WORD sub_type );

	 void updateNotifyMgrMerAttr( WORD wMerID );

	 void updateNotifyMgrMerAttr( vector<CMercenary>::iterator itMer );

	 BYTE getFreshTimes( );

	 void setFreshTimes( const BYTE& time  );

	 int clientGetMerInfo( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientMerUpDown( ROLE* role, unsigned char * data, size_t dataLen );	

	 int clientRefreshSkills( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientGetSkills( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientPreTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientGetTrainMercenaryBaseAttr( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientPreViewMerStar( ROLE* role, unsigned char * data, size_t dataLen );

	 void appendToclientPreviewMerStar(  vector< CMercenary >::iterator& it, string& strData, CONFIG::MER_STAR_CFG* merStarCfg, CONFIG::MER_STAR_CFG* merNextStarCfg );

	 int clientConfirmMerStar( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientPreViewComposeBook( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientConfirmComposeBook( ROLE* role, unsigned char * data, size_t dataLen );

	 void insertAddAttr(const BYTE& type, vector< NETMERCENARY::TAttrExtraAdd>& vecstAttrAdd, float beforeRatio, float afterRatio );

	 int clientGetMerHaloList( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientActiveMerHalo( ROLE* role, unsigned char * data, size_t dataLen );

	 int clientMerEquipAttach( ROLE* role, unsigned char * data, size_t dataLen );

	 uint16_t get_current(){ return m_wOperatorMer; }

private:

	ROLE* m_pRole;

	BYTE m_byFreshTime;//刷新次数

	WORD m_wOperatorMer;//正在操作的佣兵id

};

