#pragma once
#include "config.h"
#include "item.h"


enum
{
	MERCENARY_EQUIP_WEAPON = 0,	//����
	MERCENARY_EQUIP_CLOTH = 1,	//�·�
	MERCENARY_EQUIP_HELMET = 2,	//ñ��
	MERCENARY_EQUIP_RING = 3,	//��ָ
};

enum FIGHT_STATUS
{
	E_FIGHT_DOWN = 0,//Ӷ������
	E_FIGHT_ON  = 1,//Ӷ������
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

	int clientGetMerInfo( ROLE* role, unsigned char * data, size_t dataLen );//��ȡӶ����Ϣ
	int clientMerUpDown( ROLE* role, unsigned char * data, size_t dataLen );//Ӷ������
	int clientRefreshSkills( ROLE* role, unsigned char * data, size_t dataLen );//ˢ��Ӷ������
	int clientGetSkills( ROLE* role, unsigned char * data, size_t dataLen );//��ȡӶ�������б�
	int clientTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen );//Ӷ������
	int clientPreTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen );//
	int clientPreViewMerStar( ROLE* role, unsigned char * data, size_t dataLen );//
	int clientConfirmMerStar( ROLE* role, unsigned char * data, size_t dataLen );//
	int clientPreViewComposeBook( ROLE* role, unsigned char * data, size_t dataLen );//
	int clientConfirmComposeBook( ROLE* role, unsigned char * data, size_t dataLen );//�ϳ��鱾
	int clientGetMerHaloList( ROLE* role, unsigned char * data, size_t dataLen );//��ȡ�⻷�����б�
	int clientActiveMerHalo( ROLE* role, unsigned char * data, size_t dataLen );//����⻷
	int clientMerEquipAttach( ROLE* role, unsigned char * data, size_t dataLen );//��װ��
	int clientGetTrainMercenaryBaseAttr( ROLE* role, unsigned char * data, size_t dataLen );
}


class CMercenary
{

public:

	CMercenary( const MERCENARY_DATA* pMerData );

	CMercenary( ROLE* role, const CONFIG::MERDEF_CFG* merDefCfg );

	~CMercenary();

	vector< stIndexItem > vecHaloSkills; // �⻷����

	map<WORD, WORD> mapSuit;//ӵ����װID��������

	vector< WORD > vecSuit;//�Ѿ��õ�����װ

	WORD wID;//memset �ָ���

	stIndexItem vIndexSkills[ SKILL_MAX_NUM ]; // ��������

	int attrValueTrain[ BASE_ATTR_NUM ];//�������������������

	int tempAttrValuetrain[ BASE_ATTR_NUM ];//�ݴ�  ÿ�����������������

	ITEM_CACHE* vBodyEquip[ MER_BODY_GRID_NUM ];

	BYTE byFight;//�Ƿ�ս��

	BYTE  byJob;

	BYTE  byStar;//�Ǽ�

	BYTE bySex;

	WORD wLevel;

	DWORD merAttr[E_ATTR_MAX];

	DWORD dwFightValue;   //ս����
	
	DWORD dwStarExp;	//�������ǵ�ʣ�ྭ��

	WORD suitStrengthLevel;//����װ���ȼ���

	bool operator == ( const WORD& dst )const
	{
		return ( this->wID == dst );
	}

	void calMerFightValue( );//����Ӷ��ս����

	void updateNotifyMerAttr( ROLE* role );//���㲢֪ͨӶ����������

	void calMerAttr( );//���¼���Ӷ����������

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

	 void addMercenary( WORD wRoleLevel );//���Ӷ�� ���������� ����Ӷ���ȼ�

	 void addMercenary( const CONFIG::MERDEF_CFG* merDef );//���Ӷ��

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

	BYTE m_byFreshTime;//ˢ�´���

	WORD m_wOperatorMer;//���ڲ�����Ӷ��id

};

