#include "pch.h"
#include "mercenary.h"
#include "role.h"
#include "common.h"
#include "protocal.h"
#include "itemCtrl.h"
#include "roleCtrl.h"
#include "logs_db.h"
#include "service.h"


#define  FRESH_SKILL_GOLD   20
#define  LOCK_SKILL_GOLD    20
#define  MAX_LOCK_SKILL      3
#define  COMPOSE_BOOK_NUM    5

enum SOLT_STATUS
{
	E_SKILL_SOLT_OFF = 0,//δ�������ܲ�     
	E_SKILL_SOLT_ON  = 1,//����δ����
	E_SKILL_SOLT_SET  = 2,//�Ѿ����� 
};

enum SKILL_STATUS
{
	E_SKILL_LOCK = 1,
	E_SKILL_UNLOCK = 0,
};

namespace NETMERCENARY
{
	enum
	{
		E_ITEM_NO_EXIST  = 1,
		E_PKG_FULL  = 2,
		E_LEVEL_LIMIT  = 3,		//�ȼ�����
		E_JOB_LIMIT  = 4,		//ְҵ����
		E_SEX_LIMIT  = 5,		//�Ա�����
		E_NO_MERCENARY = 6,		//�޴�Ӷ��
	};
	static BYTE s_byNeedStar[4] = { 0, 1, 3, 5 };

	struct TBookNum
	{
		WORD wID;
		BYTE byNum;
		TBookNum( WORD wID, BYTE byNum )
		{
			this->wID		= wID			;
			this->byNum		= byNum			; 
		}
		ZERO_CONS(TBookNum);
	};

#pragma  pack( push, 1)

	struct TClientSkill
	{
		WORD wID;
		BYTE pos; 
		BYTE status:4;
		BYTE byNeedStar:4;
		TClientSkill( WORD wID, BYTE pos, BYTE status, BYTE byNeedStar )
		{
			this->wID		= wID			;
			this->pos		= pos			; 
			this->status	= status		;
			this->byNeedStar= byNeedStar	;
			if( this->wID > 0)
			{
				this->status	= E_SKILL_SOLT_SET;
			}
		}
	};

#pragma pack(pop)
}

int NETMERCENARY::clientGetMerInfo( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientGetMerInfo( role, data, dataLen );
}

int NETMERCENARY::clientMerUpDown( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientMerUpDown( role, data, dataLen );
}

int NETMERCENARY::clientRefreshSkills( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientRefreshSkills( role, data, dataLen );
}

int NETMERCENARY::clientGetSkills( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientGetSkills( role, data, dataLen );
}

int NETMERCENARY::clientPreTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientPreTrainMercenary( role, data, dataLen );
}

int NETMERCENARY::clientTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientTrainMercenary( role, data, dataLen );
}

int NETMERCENARY::clientPreViewMerStar( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientPreViewMerStar( role, data, dataLen );
}

int NETMERCENARY::clientConfirmMerStar( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientConfirmMerStar( role, data, dataLen );
}

int NETMERCENARY::clientPreViewComposeBook( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientPreViewComposeBook( role, data, dataLen );
}

int NETMERCENARY::clientConfirmComposeBook( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientConfirmComposeBook( role, data, dataLen );
}

int NETMERCENARY::clientGetMerHaloList( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientGetMerHaloList( role, data, dataLen );
}

int NETMERCENARY::clientActiveMerHalo( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientActiveMerHalo( role, data, dataLen );
}

int NETMERCENARY::clientMerEquipAttach( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientMerEquipAttach( role, data, dataLen );
}

int NETMERCENARY::clientGetTrainMercenaryBaseAttr( ROLE* role, unsigned char * data, size_t dataLen )
{
	return role->cMerMgr.clientGetTrainMercenaryBaseAttr( role, data, dataLen );
}

CMercenaryMgr::CMercenaryMgr( ROLE* pRole ):m_pRole(pRole)
{
	m_byFreshTime = FRESH_MERCENARY_NUM;
	m_wOperatorMer = 0;
}

CMercenaryMgr::~CMercenaryMgr()
{

}

void CMercenaryMgr::setMercenay()
{
	//WORD level = this->m_pRole->wLevel;
}


void CMercenaryMgr::addMercenary( WORD wRoleLevel )//��������ʱ�����
{
	CONFIG::forEachMercenaryDefCfg([&]( const std::pair<WORD, CONFIG::MERDEF_CFG>& x )
	{
		if ( wRoleLevel >= x.second.level )
		{
			const CONFIG::MERDEF_CFG& merdefCfg = x.second;

			CMercenaryMgr::addMercenary( &merdefCfg );
		}

	});

	for (auto& mer : m_vecCMercenary )
	{
		mer.wLevel = wRoleLevel;
		mer.calMerAttr();
	}
}

void CMercenaryMgr::addMercenary( const CONFIG::MERDEF_CFG* merDef )
{

	if ( merDef == NULL )
	{
		return;
	}

	if ( m_vecCMercenary.size() > MERCENARY_NUM )
	{
		return;
	}

	if ( std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), merDef->id ) != m_vecCMercenary.end() )
	{
		return;
	}

	CMercenary cMer( this->m_pRole, merDef );

	m_vecCMercenary.push_back( cMer );

	RoleMgr::getMe().judgeCompleteTypeTask( m_pRole, E_TASK_GET_MERCENARY_NUM );

}

void CMercenaryMgr::delMercenary()
{

}

int CMercenaryMgr::clientPreViewMerStar( ROLE* role, unsigned char * data, size_t dataLen )
{
	//	//Ӷ����������Ԥ��
	//#define	S_SLAVE_STAR_PRO_PREVIEW	0x0257
	//C:[WORD]
	//	[slaveIdx]
	//slaveIdx:���µ�Ӷ�������ݱ�����
	//S:[BYTE, DWORD, DWORD, [BYTE, BYTE, BYTE]]
	//	[starLv, curExp, maxExp, [eType, curAdd, nextAdd]]
	//starLv:��ǰ�Ǽ�
	//curExp:��ǰ���Ǿ���
	//maxExp:������Ǿ���
	//	   [eType, curAdd, nextAdd]
	//eType:
	//	   ROLE_PRO_MAX_HP = 2, //���Ѫ��
	//		   ROLE_PRO_MAX_MP = 4, //���ħ��
	//
	//		   ROLE_PRO_ATK_MAX = 5,//����������
	//		   ROLE_PRO_ATK_MIN = 6,//����������
	//
	//		   ROLE_PRO_MAG_ATK_MAX = 7,//���ħ������
	//
	//		   ROLE_PRO_PHY_DEF_MAX = 11, //������, phydef_max
	//
	//		   ROLE_PRO_MAG_DEF_MAX = 13, //���ħ������, magdef_max
	//
	//		   ROLE_PRO_PHY_DODGE = 15, //����ر���
	//
	//		   ROLE_PRO_PHY_HIT = 21, //����������
	//
	//		   ROLE_PRO_CRIT_VAL = 27, //����ֵ
	//curAdd:
	//	   ���Ե�ǰ�Ǽ����Ӱٷֱ�
	//nextAdd:
	//	   ������һ�Ǽ����Ӱٷֱ�

	WORD wMerID;
	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;

	auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;

	RETURN_COND(it == m_vecCMercenary.end(), 0);

	string strData;

	CONFIG::MER_STAR_CFG* merStarCfg = CONFIG::getMerUpstarCfg( it->byStar );

	RETURN_COND(merStarCfg == NULL, 0);

	CONFIG::MER_STAR_CFG* merNextStarCfg = CONFIG::getMerUpstarCfg( it->byStar + 1 );

	RETURN_COND(merNextStarCfg == NULL, 0);

	appendToclientPreviewMerStar( it, strData, merStarCfg, merNextStarCfg );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_STAR_PRO_PREVIEW, strData) );

	return 0;
}

void CMercenaryMgr::appendToclientPreviewMerStar( vector< CMercenary >::iterator& it, string& strData, CONFIG::MER_STAR_CFG* merStarCfg, CONFIG::MER_STAR_CFG* merNextStarCfg )
{
	if ( merStarCfg == nullptr )
	{
		return;
	}

	vector< NETMERCENARY::TAttrExtraAdd> vecstAttrAdd;

	float after_add_ratio[BASE_ATTR_NUM];
	if (merNextStarCfg != nullptr)
	{
		memcpy(after_add_ratio,merNextStarCfg->baseAttrRatio,sizeof(after_add_ratio));
	}
	else
	{
		memcpy(after_add_ratio, merStarCfg->baseAttrRatio, sizeof(after_add_ratio));
	}
	for (size_t i = 0; i < BASE_ATTR_NUM; i++ )
	{
		insertAddAttr(i + E_ATTR_STAMINA, vecstAttrAdd, merStarCfg->baseAttrRatio[i], after_add_ratio[i]);
	}

	S_APPEND_BYTE( strData, it->byStar );

	S_APPEND_DWORD( strData, it->dwStarExp );

	S_APPEND_DWORD( strData, merStarCfg->exp_need );

	for ( size_t i = 0; i < vecstAttrAdd.size(); i++ )
	{
		S_APPEND_NBYTES(strData, (char*)&vecstAttrAdd[i], sizeof(vecstAttrAdd[i]));
	}

}

int CMercenaryMgr::clientConfirmMerStar( ROLE* role, unsigned char * data, size_t dataLen )
{
//#define	S_SLAVE_UPGRADE_STAR	0x0258
//C:[WORD, WORD, WORD, WORD, WORD]
//	[slaveIdx, bookIdx1, bookIdx2, bookIdx3, bookIdx4, bookIdx5]
//slaveIdx:���µ�Ӷ�������ݱ�����
//bookIdxXXX: ���������ݱ�����
//			0:û���鱾
//S:[BYTE, BYTE, DWORD, DWORD, [BYTE, BYTE, BYTE]]
//		 [error, starLv, curExp, maxExp, [eType, curAdd, nextAdd]]
//error:������
//		  0:�ɹ�
//		  1:�Ѵ�����
//		  2:�����鲻����
//		  3:δ���������
//		  4:Ӷ��������
//
//		  ͬ��
//		  starLv, curExp, maxExp, [eType, curAdd, nextAdd]
//	  �Ƿ�ֻ�еȼ��ı�Ŵ��� ���Բ���?

	WORD wMerID;
	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;

	WORD bookIDs[COMPOSE_BOOK_NUM] = {0};//����޸��� ÿ���������һ��

	WORD whiteID = CONFIG::getMerComposeBookID(0);

	vector<NETMERCENARY::TBookNum> vecBookIDs;

	for (size_t i = 0; i < COMPOSE_BOOK_NUM; i++ )
	{
		if ( !BASE::parseWORD( data, dataLen, bookIDs[i]) )
			return 0;

		COND_CONTINUE( whiteID == bookIDs[i] );

		COND_CONTINUE(bookIDs[i] == 0 );

		CONFIG::MER_COMPOSE_BOOK_CFG* merComBookCfg = CONFIG::getMerComposebookCfg( bookIDs[i] );

		COND_CONTINUE( merComBookCfg == NULL );

		auto bookit = find_if( vecBookIDs.begin(), vecBookIDs.end(), [&](NETMERCENARY::TBookNum& book){return (bookIDs[i] == book.wID);} );

		if ( bookit == vecBookIDs.end() )
		{
			vecBookIDs.push_back( NETMERCENARY::TBookNum( bookIDs[i], 1 ) );
		}
		else
		{
			bookit->byNum++;
		}
	}

	string strData;

	WORD wErr = 0;

	S_APPEND_WORD( strData, 0 );

	do 
	{
		COND_BREAK( vecBookIDs.empty(), wErr, 3);

		auto itMer = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;

		COND_BREAK(itMer == m_vecCMercenary.end(), wErr, 4);

		CONFIG::MER_STAR_CFG* merStarCfg = CONFIG::getMerUpstarCfg( itMer->byStar );

		COND_BREAK(merStarCfg == NULL, wErr, 1);

		CONFIG::MER_STAR_CFG* merNextStarCfg = CONFIG::getMerUpstarCfg( itMer->byStar + 1 );//�Ѿ�����

		COND_BREAK(merNextStarCfg == NULL, wErr, 1);
		
		DWORD  dwMaxExp = CONFIG::getMerUpstarMaxExpCfg();

		DWORD  dwNowExp = CONFIG::getMerUpstarNowExpCfg(itMer->byStar) + itMer->dwStarExp;

		DWORD wMaxAddExp = dwMaxExp>dwNowExp ? dwMaxExp-dwNowExp : 0;//��ǰӶ����������Ҫ����

		DWORD canGiveExp = 0;

		vector<ITEM_CACHE*> vecProps;

		vector<BYTE> vecNums;

		for ( auto& bookNeed : vecBookIDs )
		{
			CONFIG::MER_COMPOSE_BOOK_CFG* merComBookCfg = CONFIG::getMerComposebookCfg( bookNeed.wID );

			COND_CONTINUE( merComBookCfg == NULL );

			BYTE haveNum = 0;

			BYTE needNum = bookNeed.byNum;

			for (auto it = role->m_packmgr.lstItemPkg.begin(); it != role->m_packmgr.lstItemPkg.end() ; it++)
			{
				if ( (*it)->itemCfg->id != bookNeed.wID )
				{
					continue;
				}

				size_t oneTypeNum = (*it)->itemNums;

				size_t getOneTypeNum = 0;

				while( oneTypeNum > 0 )
				{
					oneTypeNum--;

					getOneTypeNum++;

					haveNum++;

					canGiveExp += merComBookCfg->offer_exp;

					if ( haveNum >= needNum )
					{
						break;
					}
					if ( canGiveExp >= wMaxAddExp )
					{
						haveNum = needNum;
						break;
					}
				}

				vecProps.push_back( *it );

				vecNums.push_back( getOneTypeNum );
			}

			COND_BREAK(haveNum < needNum, wErr, 2);
		}

		COND_BREAK(canGiveExp == 0, wErr, 1);

		//DWORD getAllExp = canGiveExp;

		while ( canGiveExp > 0 )
		{
			if ( canGiveExp + itMer->dwStarExp < merStarCfg->exp_need )
			{
				itMer->dwStarExp += canGiveExp;
				canGiveExp = 0;
				break;
			}
			else
			{
				if (itMer->dwStarExp >= merStarCfg->exp_need)
				{
					itMer->dwStarExp -= merStarCfg->exp_need;
				}
				else
				{
					canGiveExp -= ( merStarCfg->exp_need - itMer->dwStarExp );
				}

				itMer->byStar++;

				itMer->dwStarExp = 0;

				merStarCfg = CONFIG::getMerUpstarCfg( itMer->byStar );
				if ( merStarCfg == NULL || merStarCfg->exp_need == 0 )
				{
					break;
				}

			}
		}

		updateNotifyMgrMerAttr( itMer );

		for ( size_t i = 0; i < vecProps.size(); i++)
		{
			ItemCtrl::rmItemNotify( role, role->m_packmgr.lstItemPkg, PKG_TYPE_ITEM, vecProps[i]->itemCfg, vecNums[i] );
		}

		merStarCfg = CONFIG::getMerUpstarCfg(itMer->byStar);
		merNextStarCfg = CONFIG::getMerUpstarCfg(itMer->byStar + 1);

		appendToclientPreviewMerStar( itMer, strData, merStarCfg, merNextStarCfg );

		for ( size_t i = 0; i < SKILL_MAX_NUM; i++ )
		{
			BREAK_COND( itMer->byStar < NETMERCENARY::s_byNeedStar[i] );
			itMer->vIndexSkills[i].byStatus = E_SKILL_SOLT_ON;
		}

		itMer->updateNotifyStar( role );

	} while (0);

	*((WORD*)&strData[0]) = wErr;

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_UPGRADE_STAR, strData) );

	if (wErr == 0)
	{
		BYTE byMaxStar = 0;

		for ( auto& oneMer : m_vecCMercenary )
		{
			byMaxStar = byMaxStar > oneMer.byStar ? byMaxStar : oneMer.byStar;
		}

		ITEM_INFO para(0, byMaxStar);
		RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_MERCENARY_STAR, &para);
	}

	return 0;
}

int CMercenaryMgr::clientPreViewComposeBook( ROLE* role, unsigned char * data, size_t dataLen )
{
	//	//����ϳ��鱾��Ϣ
	//#define	S_REQUEST_COMBO_BOOKS_INFO	0x0259
	//C:[]
	//S:[[WORD, BYTE, BYTE, BYTE, DWORD]]
	//	[bookIdx, needVip, needNum, moneyType, price]
	//bookIdx:�鱾���ݱ�index
	//needVip:����VIP�ȼ�
	//needNum:�ϳ���һ���鱾������Ŀ
	//moneyType:���Ļ�������
	//price:�۸�

	string strData;

	CONFIG::forEachComposeBookCfg([&strData]( const std::pair<WORD, CONFIG::MER_COMPOSE_BOOK_CFG>& comCfg )
	{
		S_APPEND_WORD( strData, comCfg.second.material_id );

		S_APPEND_BYTE( strData, (BYTE)comCfg.second.vip_level_limit );

		S_APPEND_BYTE( strData, (BYTE)comCfg.second.number );

		S_APPEND_BYTE( strData, comCfg.second.money_type );

		S_APPEND_DWORD( strData, comCfg.second.money_cost );

	}
	);

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_COMBO_BOOKS_INFO, strData) );

	return 0;
}


int CMercenaryMgr::clientGetMerHaloList( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//����⻷��Ϣ
//#define	S_REQUEST_HALO_LIST	0x025B
//C:[  WORD ]
//	[ slaveIdx ]
//S:[ WORD ]n��
//	  [haloIdx]

	WORD wMerID;

	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;

	string strData;

	auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;

	RETURN_COND( it == m_vecCMercenary.end(), 0);

	for ( auto& haloskillit : it->vecHaloSkills )
	{
		if ( haloskillit.byStatus == E_SKILL_SOLT_ON )
		{
			S_APPEND_WORD( strData, haloskillit.skillid );
		}
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_HALO_LIST, strData) );

	return 0;

}

int CMercenaryMgr::clientActiveMerHalo( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//����ĳ���⻷����
//#define	S_ACTIVE_HALO	0x025C
//C:[  WORD,      WORD ]
//	[ slaveIdx,haloIdx ]
//
//S:[WORD]
//	[error]
//error:0:�ɹ�
//		  1:Ӷ��������
//		  2:�Ǽ�����
//		  3:��Ҳ���
//		  4:Ԫ������
//		  5:��Ӷ���޴˹⻷����
	WORD wMerID;
	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;

	WORD wHaloID;
	if ( !BASE::parseWORD( data, dataLen, wHaloID) )
		return 0;

	string strData;

	WORD wErr = 0;

	do 
	{
		auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;

		COND_BREAK( it == m_vecCMercenary.end(), wErr, 1 );

		CONFIG::MERHALO_CFG* merHaloCfg = CONFIG::getMerHaloCfg(wHaloID);

		COND_BREAK( merHaloCfg == NULL, wErr, 5);

		auto haloit = std::find( it->vecHaloSkills.begin(), it->vecHaloSkills.end(), wHaloID );

		COND_BREAK(  haloit == it->vecHaloSkills.end(), wErr, 5);

		COND_BREAK( merHaloCfg->byMoneyType == E_GOLD_MONEY && role->dwIngot < merHaloCfg->dwMoneyCost, wErr, 4 );

		COND_BREAK( merHaloCfg->byMoneyType == E_COIN_MONEY && role->dwCoin < merHaloCfg->dwMoneyCost, wErr, 3 );

		haloit->byStatus = E_SKILL_SOLT_ON;

		if ( merHaloCfg->byMoneyType == E_GOLD_MONEY)//1Ԫ��
		{
			ADDUP(role->dwIngot, -(int)merHaloCfg->dwMoneyCost);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)merHaloCfg->dwMoneyCost, purpose::active_halo, 0);
			notifyIngot(role);
		}
		else if( merHaloCfg->byMoneyType == E_COIN_MONEY)//2ͭǮ
		{
			ADDUP(role->dwCoin, -(int)merHaloCfg->dwMoneyCost);
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, -(int)merHaloCfg->dwMoneyCost, 0, purpose::active_halo, 0);
			notifyCoin(role);
		}

	} while (0);


	S_APPEND_WORD( strData, wErr );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ACTIVE_HALO, strData) );

	return 0;

}


int CMercenaryMgr::clientConfirmComposeBook( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//�ϳ��鱾
//#define	S_COMBO_BOOK	0x025A
//C:[WORD]
//	[bookIdx]
//bookIdx:�鱾���ݱ�index
//S:[WORD]
//	[error]
//error:0:�ɹ�
//		  1:vip�ȼ�����
//		  2:�鱾��������
//		  3:��Ҳ���
//		  4:Ԫ������
//		  5:���鱾���ܺϳ�

	WORD wBookID;
	if ( !BASE::parseWORD( data, dataLen, wBookID) )
		return 0;

	string strData;
	WORD wErr = 0;

	do 
	{
		CONFIG::MER_COMPOSE_BOOK_CFG* merComCfg = CONFIG::getMerComposebookCfg( wBookID );

		COND_BREAK( merComCfg==NULL || merComCfg->target_id == 0, wErr, 5 );

		COND_BREAK( merComCfg->money_type == E_GOLD_MONEY && role->dwIngot < merComCfg->money_cost, wErr, 3 );

		COND_BREAK( merComCfg->money_type == E_COIN_MONEY && role->dwCoin < merComCfg->money_cost, wErr, 4 );

		BYTE haveNum = 0;

		BYTE needNum = (BYTE)merComCfg->number;

		vector<ITEM_CACHE*> vecProps;

		vector<BYTE> vecNums;

		wErr = 2;

		for (auto it = role->m_packmgr.lstItemPkg.begin(); it != role->m_packmgr.lstItemPkg.end() ; it++)
		{
			if ( (*it)->itemCfg->id != wBookID )
			{
				continue;
			}

			size_t oneTypeNum = (*it)->itemNums;

			size_t getOneTypeNum = 0;

			while( oneTypeNum > 0 )
			{
				oneTypeNum--;

				getOneTypeNum++;

				haveNum++;

				if ( haveNum >= needNum )
				{
					break;
				}
			}

			vecProps.push_back( *it );

			vecNums.push_back( getOneTypeNum );

			if ( haveNum >= needNum )
			{
				for ( size_t i = 0; i < vecProps.size(); i++)
				{
					ItemCtrl::rmItemNotify( role,  role->m_packmgr.lstItemPkg, PKG_TYPE_ITEM, vecProps[i]->itemCfg, vecNums[i] );
				}

				CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( merComCfg->target_id );

				ItemCtrl::makeOneTypeItem( role, itemCfg, 1, (BYTE)itemCfg->rare );

				if (merComCfg->money_type == E_GOLD_MONEY)//1Ԫ��
				{
					ADDUP(role->dwIngot, -(int)merComCfg->money_cost);
					logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)merComCfg->money_cost, purpose::synthesis_book, 0);
					notifyIngot(role);
				}
				else if(merComCfg->money_type == E_COIN_MONEY)//2ͭǮ
				{
					ADDUP(role->dwCoin, -(int)merComCfg->money_cost);
					logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, -(int)merComCfg->money_cost, 0, purpose::synthesis_book, 0);
					notifyCoin(role);
				}

				COND_BREAK( 1, wErr, 0 );
			}

		}

	} while (0);

	S_APPEND_WORD( strData, wErr );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_COMBO_BOOK, strData) );

	return 0;
}

int CMercenaryMgr::clientTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen )
{
//	//��������
//#define	S_SLAVE_TRAIN_SAVE	0x0256
//C:[]

//S:[WORD]
//error:0�ɹ�
//		  1:��Ҫ��Ԥ��  Ԥ��Ӷ��δ�ҵ�

	string strData;
	WORD wErr = 0 ;
	do 
	{
		auto itMer = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), m_wOperatorMer ) ;

		COND_BREAK(itMer == m_vecCMercenary.end(), wErr, 1);

		for (size_t i = 0; i < BASE_ATTR_NUM; i++ )
		{
			itMer->attrValueTrain[ i ] += itMer->tempAttrValuetrain[ i ];
			itMer->tempAttrValuetrain[ i ] = 0;

			//lognew("confirm pre::  %d-- %d",  itMer->attrValueTrain[ i ], itMer->tempAttrValuetrain[ i ] );
		}

		m_wOperatorMer = 0;
		
		updateNotifyMgrMerAttr( itMer );		

	} while (0);


	S_APPEND_WORD( strData, wErr );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_TRAIN_SAVE, strData) );

	return 0;
}

int CMercenaryMgr::clientPreTrainMercenary( ROLE* role, unsigned char * data, size_t dataLen )
{
	//	//Ӷ������Ԥ��
	//#define S_SLAVE_TRAIN_PREVIEW	0x0255
	//C:[WORD, WORD]
	//	[slaveIdx, trainType]
	//slaveIdx:Ӷ�����ݱ�����
	//trainType:��������
	//S:[WORD, [BYTE, DWORD]]
	//	[error, [type, value]]
	//error:������,
	//		  0:�ɹ�
	//		  1:ͭǮ����
	//		  2:Ԫ������
	//		  3:vip����
	//		  [
	//type:��������
	//value:Ԥ��ֵ
	//		  ]

	WORD wMerID;
	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;

	BYTE byTrainType;
	if ( !BASE::parseBYTE( data, dataLen, byTrainType) )
		return 0;

	string strData;
	WORD wErr = 0 ;
	auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;
	if ( it == m_vecCMercenary.end() )
	{
		wErr = 4;
		S_APPEND_WORD( strData, wErr );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_TRAIN_PREVIEW, strData) );
		return 0;
	}

	CONFIG::MERCENARY_TRAIN_CFG* trainCfg = CONFIG::getMercenaryTrainCfg( byTrainType );
	if ( trainCfg == NULL )
	{
		wErr = 5;
		S_APPEND_WORD( strData, wErr );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_TRAIN_PREVIEW, strData) );
		return 0;
	}

	if ( role->getVipLevel() < trainCfg->viplevel_limit)
	{
		wErr = 3;
		S_APPEND_WORD( strData, wErr );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_TRAIN_PREVIEW, strData) );
		return 0;
	}

	if ( trainCfg->money_type == E_GOLD_MONEY )//1Ԫ��
	{
		if ( role->dwIngot < trainCfg->money_cost )
		{
			wErr = 2;
			S_APPEND_WORD( strData, wErr );
			PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_TRAIN_PREVIEW, strData) );
			return 0;
		}
		else
		{
			ADDUP(role->dwIngot, -(int)trainCfg->money_cost );
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)trainCfg->money_cost, purpose::train_mercenary, 0);
			notifyIngot(role);
		}
	}
	else
	{
		if ( role->dwCoin < trainCfg->money_cost )//2ͭǮ
		{
			wErr = 1;
			S_APPEND_WORD( strData, wErr );
			PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_TRAIN_PREVIEW, strData) );
			return 0;
		}
		else
		{
			ADDUP(role->dwCoin, -(int)trainCfg->money_cost );
			logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, -(int)trainCfg->money_cost, 0, purpose::train_mercenary, 0);
			notifyCoin(role);
		}
	}

	S_APPEND_WORD( strData, wErr );

	WORD wMaxTrainAttrValue = (WORD)CONFIG::getMercenaryTrainMaxCfg( it->wLevel );//train max attr value

	CONFIG::MERCENARY_CFG* mercenaryCfg = getMercenaryCfg2( it->byJob, it->wLevel );//���ñ�
	DWORD dwBaseAttr[BASE_ATTR_NUM] = {0};
	dwBaseAttr[ 0 ] = mercenaryCfg->stamina	   ;
	dwBaseAttr[ 1 ] = mercenaryCfg->strength	   ;
	dwBaseAttr[ 2 ] = mercenaryCfg->agility	   ;
	dwBaseAttr[ 3 ] = mercenaryCfg->intellect	;
	dwBaseAttr[ 4 ] = mercenaryCfg->spirit		;

	for ( BYTE type = 0; type < BASE_ATTR_NUM; type++ )
	{
		ADDUP( dwBaseAttr[ type ], it->attrValueTrain[type] );//��������
	}

	for ( size_t i = 0; i < BASE_ATTR_NUM; i++ )
	{
		RETURN_COND( trainCfg->vecRandSum .empty() , 0 );

		size_t index = BASE::getIndexOfVectorSum( trainCfg->vecRandSum );

		CONFIG::AddAttrValue& addAttr = trainCfg->vecAddAttrValue[index];

		it->tempAttrValuetrain[ i ] =  BASE::randInter( addAttr.min,addAttr.max );

		//AttrValue tempvalue( E_ATTR_STAMINA + i,  it->merAttr[ E_ATTR_STAMINA + i ]);
		//it->attrValueTrain[ i ];

		if ( wMaxTrainAttrValue != 0 && ( dwBaseAttr[ i ] + it->tempAttrValuetrain[ i ] ) > wMaxTrainAttrValue )
		{
			it->tempAttrValuetrain[ i ] = wMaxTrainAttrValue - dwBaseAttr[ i ];
		}

		AttrValue tempvalue( E_ATTR_STAMINA + i,  dwBaseAttr[ i ]);

		ADDUP( tempvalue.value, it->tempAttrValuetrain[ i ] );

		S_APPEND_NBYTES( strData, (char*)&tempvalue, sizeof( tempvalue ) );
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_TRAIN_PREVIEW, strData) );

	m_wOperatorMer = it->wID;

	return 0;
}


int CMercenaryMgr::clientGetTrainMercenaryBaseAttr( ROLE* role, unsigned char * data, size_t dataLen )
{
	WORD wMerID;
	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;

	string strData;

	auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;
	if ( it == m_vecCMercenary.end() )
	{
		return 0;
	}

	CONFIG::MERCENARY_CFG* mercenaryCfg = getMercenaryCfg2( it->byJob, it->wLevel );//���ñ�
	DWORD dwBaseAttr[BASE_ATTR_NUM] = {0};
	dwBaseAttr[ 0 ] = mercenaryCfg->stamina	   ;
	dwBaseAttr[ 1 ] = mercenaryCfg->strength	   ;
	dwBaseAttr[ 2 ] = mercenaryCfg->agility	   ;
	dwBaseAttr[ 3 ] = mercenaryCfg->intellect	;
	dwBaseAttr[ 4 ] = mercenaryCfg->spirit		;

	for ( BYTE type = 0; type < BASE_ATTR_NUM; type++ )
	{
		ADDUP( dwBaseAttr[ type ], it->attrValueTrain[type] );//��������
	}

	for ( size_t i = 0; i < BASE_ATTR_NUM; i++ )
	{
		AttrValue tempvalue( E_ATTR_STAMINA + i,  dwBaseAttr[ i ]);

		S_APPEND_NBYTES( strData, (char*)&tempvalue, sizeof( tempvalue ) );
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_BASR_ATTR, strData) );

	return 0;
}




int CMercenaryMgr::clientGetMerInfo( ROLE* role, unsigned char * data, size_t dataLen )
{
	//	//����Ӷ����Ϣ
	//#define S_REQUES_MERCENARY_INFO	0x0250
	//C:[WORD]
	//	[mercenaryIdx]
	//mercenaryIdx:Ӷ������
	//S:[WORD, MercenaryInfo, BYTE, ItemBriefInfo]
	//	[error, mercenInfo, equipNum, equipBriefInfo]
	//error:
	//	0:�ɹ�
	//		1:δ����
	//		2:������
	//mercenInfo:Ӷ����Ϣ
	//equipNum:װ������
	//equipBriefInfo:װ����Ϣ
	//װ��λ�ö���:1-4,
	//	   1:������2:�·���3:ñ�ӡ�4:��ָ

	WORD wMerID;
	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;

	string strData;
	WORD wErr = 0 ;
	auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;
	if ( it == m_vecCMercenary.end() )
	{
		wErr = 2;
		S_APPEND_WORD( strData, wErr );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUES_MERCENARY_INFO, strData) );
		return 0;
	}

	MercenaryInfo tMerInfo( &(*it) );

	S_APPEND_WORD( strData, wErr );
	S_APPEND_NBYTES(strData, &tMerInfo, sizeof(tMerInfo));

	vector<ITEM_CACHE*> vecItemBody;
	for ( int i=0; i < MER_GRID_NUMS; i++)
	{

		ITEM_CACHE* itemCache = it->vBodyEquip[i];

		COND_CONTINUE( itemCache == NULL  );

		COND_CONTINUE( itemCache->itemCfg == NULL  );

		vecItemBody.push_back(itemCache);

	}

	S_APPEND_BYTE( strData, vecItemBody.size() );

	for ( auto itemCachePtr : vecItemBody )
	{
		ItemBriefInfo brefInfo(itemCachePtr);

		S_APPEND_STRUCT( strData, brefInfo);
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUES_MERCENARY_INFO, strData) );

	//vector<WORD> vecTempSuit( role->vecSuit );
	string suitData;
	//if ( vecTempSuit != role->vecSuit )//�б仯��
	//{
		S_APPEND_WORD( suitData, wMerID);
		for ( auto& one : role->vecSuit )
		{
			S_APPEND_BYTE( suitData, (BYTE)one );
		}

		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_SUIT_INFO, suitData) );
	//}

	return 0;

}


int CMercenaryMgr::clientMerUpDown( ROLE* role, unsigned char * data, size_t dataLen )
{
	//	//Ӷ��������
	//#define	S_SLAVE_UP_DOWN	0x0251
	//C:[WORD, BYTE]
	//	[mercenaryIdx, byState]
	//mercenaryIdx:���ݱ�����
	//byState:Ӷ�����������
	//		0:����
	//		1:����
	//S:[WORD]
	//			 [error]
	//error:0:�ɹ�
	//		  1:�Ѿ�������
	//		  2:�Ѿ�����
	//		  3:Ӷ��������

	WORD wMerID;
	BYTE byStatus;
	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;
	if ( !BASE::parseBYTE( data, dataLen, byStatus) )
		return 0;

	string strData;
	WORD wErr = 0 ;
	auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;
	if ( it == m_vecCMercenary.end() )
	{
		wErr = 3;
	}
	else
	{
		if( byStatus == E_FIGHT_DOWN )//����
		{
			if( it->getFightStatus() == E_FIGHT_DOWN )
			{
				wErr = 2;
			}
			else
			{
				it->setFightStatus( E_FIGHT_DOWN );
			}
		}
		else if( byStatus == E_FIGHT_ON )//����
		{
			if( it->getFightStatus() == E_FIGHT_ON )
			{
				wErr = 1;
			}
			else
			{
				for ( auto& cMr : m_vecCMercenary )
				{
					cMr.setFightStatus( E_FIGHT_DOWN );//only one up
				}

				it->setFightStatus( E_FIGHT_ON );
			}

		}
	}

	S_APPEND_WORD( strData, wErr );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_UP_DOWN, strData) );
	return 0;
}


int CMercenaryMgr::clientGetSkills( ROLE* role, unsigned char * data, size_t dataLen )
{
	////����Ӷ�������б�
	//#define	S_SLAVE_SKILL_LIST	0x0252
	//C:[WORD]
	//[mercenaryIdx]
	//mercenaryIdx:���ݱ�����
	//S:[WORD, BYTE, WORD, WORD, [WORD, BYTE, BYTE:4, BYTE:4]]
	//[error, free, nextIngot, lockIngot, [skillId, pos, status, needStar]]
	//error:
	//0:�ɹ�
	//	1:Ӷ��������
	//free:ʣ����Ѵ���
	//nextIngot:�´�ˢ������Ԫ��
	//lockIngot:û����һ�����ܷ���
	//		  [
	//skillId:�������ݱ�����
	//pos:���ܲ�λ
	//status:���ܲ�״̬
	//	   0:δ����
	//	   1:����δ����
	//	   2:����������
	//needStar:���������Ǽ�
	//		  ]

	WORD wMerID;

	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;

	string strData;
	WORD wErr = 0 ;
	auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;
	if ( it == m_vecCMercenary.end() )
	{
		wErr = 1;
		S_APPEND_WORD( strData, wErr );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_SKILL_LIST, strData) );
		return 0;
	}


	
	S_APPEND_WORD( strData, wErr );
	S_APPEND_BYTE( strData, m_byFreshTime );
	S_APPEND_WORD( strData, FRESH_SKILL_GOLD );
	S_APPEND_WORD( strData, LOCK_SKILL_GOLD );

	for ( size_t i = 0; i < SKILL_MAX_NUM; i++ )
	{
		NETMERCENARY::TClientSkill stCliSkill(  it->vIndexSkills[i].skillid, i,  it->vIndexSkills[i].byStatus, NETMERCENARY::s_byNeedStar[i] );
		S_APPEND_NBYTES( strData, &stCliSkill, sizeof(stCliSkill) );
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_SKILL_LIST, strData) );

	return 0;
}




int CMercenaryMgr::clientRefreshSkills( ROLE* role, unsigned char * data, size_t dataLen )
{
	//	//Ӷ������ˢ��
	//#define	S_SLAVE_REFRESH_SKILLS	0x0253
	//C:[WORD, BYTE]
	//	[mercenaryIdx, lockPos]
	//mercenaryIdx:���ݱ�����
	//lockPos:�������ܲ�λ,λ������
	//		7 - - - | - - - 0
	//		0 0 0 0| x x x x
	//x:0��־δ������λ���ܲ�; 1����
	//
	//S:[WORD, BYTE, WORD, WORD, [WORD, BYTE, BYTE:4, BYTE:4]]
	//			 [error, free, nextIngot, lockIngot, [skillId, pos, status, needStar]]
	//error:
	//			 0:�ɹ�
	//				 1:Ӷ��������
	//				 2:Ԫ������
	//				 3:������λ����.���3����λ
	//free:ʣ����Ѵ���
	//nextIngot:�´�ˢ������Ԫ��
	//lockIngot:û����һ�����ܷ���
	//		  [
	//skillId:�������ݱ�����
	//pos:���ܲ�λ
	//status:���ܲ�״̬
	//	   0:δ����
	//	   1:����δ����
	//	   2:����������
	//needStar:���������Ǽ�
	//		  ]

	WORD wMerID;
	BYTE byLock;
	if ( !BASE::parseWORD( data, dataLen, wMerID) )
		return 0;
	if ( !BASE::parseBYTE( data, dataLen, byLock) )
		return 0;

	string strData;
	WORD wErr = 0 ;
	auto it = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID ) ;
	if ( it == m_vecCMercenary.end() )
	{
		wErr = 1;
		S_APPEND_WORD( strData, wErr );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_REFRESH_SKILLS, strData) );
		return 0;
	}

	BYTE  nMask = 2;

	BYTE getLock[SKILL_MAX_NUM] = { 0 };
	BYTE lockCount = 0;
	for ( BYTE i = 0; i < SKILL_MAX_NUM; i++ )
	{
		BYTE lockMask = (BYTE)pow( nMask, i );
		if ( lockMask & byLock )
		{
			getLock[ i ] = 1;
			lockCount++;
		}
	}

	if ( lockCount > MAX_LOCK_SKILL )
	{
		wErr = 3;
		S_APPEND_WORD( strData, wErr );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_REFRESH_SKILLS, strData) );
		return 0;
	}

	DWORD needIngot = m_byFreshTime > 0 ? 0 : (FRESH_SKILL_GOLD + LOCK_SKILL_GOLD * lockCount);

	if ( needIngot > role->dwIngot )
	{
		wErr = 2;
		S_APPEND_WORD( strData, wErr );
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_REFRESH_SKILLS, strData) );
		return 0;
	}

	it->refreshSkills( getLock );

	ADDUP(role->dwIngot, -(int)needIngot );
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)needIngot, purpose::refresh_skill, 0);
	notifyIngot(role);
	REDUCEUNSIGNED( m_byFreshTime,1 );

	S_APPEND_WORD( strData, wErr );
	S_APPEND_BYTE( strData, m_byFreshTime );
	S_APPEND_WORD( strData, FRESH_SKILL_GOLD );
	S_APPEND_WORD( strData, LOCK_SKILL_GOLD );

	for ( size_t i = 0; i < SKILL_MAX_NUM; i++ )
	{
		NETMERCENARY::TClientSkill stCliSkill(  it->vIndexSkills[i].skillid, i,  it->vIndexSkills[i].byStatus, NETMERCENARY::s_byNeedStar[i] );
		S_APPEND_NBYTES( strData, &stCliSkill, sizeof(stCliSkill) );
	}

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_REFRESH_SKILLS, strData) );

	return 0;
}

void CMercenaryMgr::updateNotifyMgrMerAttr( vector<CMercenary>::iterator itMer )
{
	itMer->updateNotifyMerAttr( m_pRole );
}

void CMercenaryMgr::updateNotifyMgrMerAttr( WORD wMerID )
{
	auto itMer = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMerID );

	if ( itMer == m_vecCMercenary.end() )
	{
		return ;
	}

	updateNotifyMgrMerAttr(  itMer );

}


int CMercenaryMgr::clientMerEquipAttach( ROLE* role, unsigned char * data, size_t dataLen )
{
	//	//Ӷ������װ��
	//#define	S_SLAVE_EQUIP_ATTACH	0x025D
	//C:[WORD, BYTE, BYTE, DWORD]
	//	[slaveIdx, pkgType, targetGrid, itemId]
	//slaveIdx:Ӷ�������ݱ�����
	//pkgType:��������
	//targetGrid:Ŀ�����.=0:�������Զ�ѡ��, != 0,�ͻ���ѡ��Ŀ�����װ��.
	//itemId:װ��ΨһID
	//
	//S:[BYTE, BYTE]
	//	[error, tagetGrid]
	//error:0:�ɹ�
	//		  [1] = "װ��������",
	//		  [2] = "�ȼ�����",
	//		  [3] = "�ȼ�����",
	//		  [4] = "ְҵ��ƥ��",
	//		  [5] = "�Ա�����",
	//tagetGrid:װ����Ŀ�����λ


	BYTE pkgType;
	DWORD itemId;
	BYTE clientDstGrid;
	WORD merId;

	if ( !BASE::parseWORD( data, dataLen, merId) )
		return 0;

	if ( !BASE::parseBYTE( data, dataLen, pkgType) )
		return 0;

	if ( !BASE::parseBYTE( data, dataLen, clientDstGrid) )
		return 0;

	if ( !BASE::parseDWORD( data, dataLen, itemId) )
		return 0;

	if ( clientDstGrid ==0 || clientDstGrid > MER_GRID_NUMS )
		return 0;


	ITEM_CACHE* itemCache = (ITEM_CACHE*)itemId ;

	int tgtBodyIndex = /*clientDstGrid ==0 ? -1 : */clientDstGrid-1;

	int ret = CMercenaryMgr::equip( role,  itemCache, pkgType, tgtBodyIndex, merId );

	string strData;
	S_APPEND_BYTE( strData, ret );
	S_APPEND_BYTE( strData, (BYTE)clientDstGrid );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_EQUIP_ATTACH, strData) );

	if ( ret == E_SUCC )
	{
		updateNotifyMgrMerAttr(merId);
	}

	return 0;
}

//Ӷ������װ��,����������ͻ���ˢ�°���,װ��
int CMercenaryMgr::equip( ROLE* role, ITEM_CACHE* itemCache, int srcPkgType, int &tgtBodyIndex, WORD wMercenary )
{
	auto itMer = std::find( m_vecCMercenary.begin(), m_vecCMercenary.end(), wMercenary );

	if ( itMer == m_vecCMercenary.end() )
	{
		return NETMERCENARY::E_NO_MERCENARY;
	}

	if ( srcPkgType != PKG_TYPE_EQUIP  )
		return NETMERCENARY::E_ITEM_NO_EXIST;

	list<ITEM_CACHE*>& lstDst = role->m_packmgr.lstEquipPkg;

	auto pkgItemIt = find( lstDst.begin(), lstDst.end(), itemCache );
	if ( pkgItemIt == lstDst.end() )
		return E_ITEM_NOT_EXIST;

	if ( itemCache->itemCfg->type != TYPE_EQUIP )
		return NETMERCENARY::E_ITEM_NO_EXIST;

	//if ( tgtBodyIndex == -1 )
	//{
		tgtBodyIndex = getBodyIndexBySubtype( itemCache->itemCfg->sub_type );
		if ( tgtBodyIndex == -1 )
			return NETMERCENARY::E_ITEM_NO_EXIST;
	//}

	if ( IS_LEVEL_LIMIT(role, itemCache->itemCfg) )//Ӷ��������ȼ�һ��
		return NETMERCENARY::E_LEVEL_LIMIT;

	if ( IS_JOB_LIMIT(itMer, itemCache->itemCfg) )
		return NETMERCENARY::E_JOB_LIMIT;

	if ( IS_SEX_LIMIT(itMer, itemCache->itemCfg ) )
		return NETMERCENARY::E_SEX_LIMIT;

	swapEquip( role, itMer, tgtBodyIndex, lstDst, pkgItemIt,  itemCache );

	return 0;

}

void CMercenaryMgr::swapEquip(ROLE* role, vector<CMercenary>::iterator itMer, int bodyIndex, list<ITEM_CACHE*>& lstDst, list<ITEM_CACHE*>::iterator pkgItemIt,  ITEM_CACHE* itemCache)
{
	if ( itemCache == NULL )
		return;

	if ( itemCache->itemCfg == NULL )
		return;

	if ( !IS_EQUIP(itemCache->itemCfg) )
	{
		return;
	}

	if ( itMer->vBodyEquip[bodyIndex] != NULL )//���ر���
	{
		*pkgItemIt = itMer->vBodyEquip[bodyIndex];
	}
	else
	{
		lstDst.erase( pkgItemIt );
	}

	itMer->vBodyEquip[bodyIndex] = itemCache;

}

void CMercenaryMgr::insertAddAttr(const BYTE& type, vector< NETMERCENARY::TAttrExtraAdd>& vecstAttrAdd, float beforeRatio, float afterRatio )
{
	if ( type < E_ATTR_STAMINA || type > E_ATTR_SPIRIT )//��֤ һ������  ��������
	{
		return ;
	}

	vecstAttrAdd.push_back( NETMERCENARY::TAttrExtraAdd( type, beforeRatio, afterRatio ) );

	CONFIG::ATTRDEFORM_CFG* attrTransCfg = CONFIG::getAttrDeformCfg(type);
	if ( attrTransCfg == NULL )
	{
		return;
	}

	for ( auto& it : attrTransCfg->vecAttrDeform )
	{
		auto addit = find_if( vecstAttrAdd.begin(), vecstAttrAdd.end(), [it](NETMERCENARY::TAttrExtraAdd& add){ return (add.attr == it.attr);} );
		if ( addit == vecstAttrAdd.end() )
		{
			vecstAttrAdd.push_back( NETMERCENARY::TAttrExtraAdd( it.attr, beforeRatio, afterRatio ) );
		}
		//else
		//{
		//	addit->addBefore += WORD(it.ratio * beforeRatio * 100);
		//	addit->addAfter += WORD(it.ratio * afterRatio * 100);
		//}
	}
}

int CMercenaryMgr::getBodyIndexBySubtype( WORD sub_type )
{
	if ( sub_type > BODY_GRID_NUMS )
		return -1;

	switch ( sub_type )
	{
		case	SUB_TYPE_EQUIP_WEAPON:	// = 1,
				return MERCENARY_EQUIP_WEAPON;

		case	SUB_TYPE_EQUIP_CLOTH :	//= 2,
				return MERCENARY_EQUIP_CLOTH;

		case	SUB_TYPE_EQUIP_HELMET:	// = 3,
				return MERCENARY_EQUIP_HELMET;

		case	SUB_TYPE_EQUIP_RING	 :	// = 8,
				return MERCENARY_EQUIP_RING;
	default:
		break;
	}

	return -1;	

}

BYTE CMercenaryMgr::getFreshTimes( )
{
	return this->m_byFreshTime;
}

void CMercenaryMgr::setFreshTimes( const BYTE& time  )
{
	 this->m_byFreshTime = time;
}

CMercenary::CMercenary( const MERCENARY_DATA* pMerData )
{
	//size_t offset = offsetof( CMercenary, wID ); //caution
	//memset( ((char*)this) + offset,  0, sizeof(*this)-offset );

	memset(vIndexSkills, 0, sizeof(vIndexSkills) ); // ��������
	memset(attrValueTrain, 0, sizeof(attrValueTrain) );//�������������������
	memset(tempAttrValuetrain, 0,sizeof(tempAttrValuetrain) );//�ݴ�  ÿ�����������������
	memset(vBodyEquip, 0,sizeof(vBodyEquip) );
	byFight= 0  ;//�Ƿ�ս��
	byJob  = 0  ;
	byStar = 0  ;//�Ǽ�
	bySex = 0   ;
	wLevel = 0  ;
	memset(merAttr, 0,sizeof(merAttr));
	dwFightValue      = 0;   //ս����
	dwStarExp         = 0;	//�������ǵ�ʣ�ྭ��
	suitStrengthLevel = 0;//����װ���ȼ���


	memcpy(this->vIndexSkills, pMerData->vIndexSkills, sizeof( this->vIndexSkills) ); // ����
	this->wID = pMerData->wID;
	this->byFight = pMerData->byFight;
	this->byJob = pMerData->byJob;
	this->byStar = pMerData->byStar;
	this->bySex = pMerData->bySex;
	this->wLevel = pMerData->wLevel;
	memcpy(this->attrValueTrain, pMerData->attrValueTrain, sizeof( this->attrValueTrain) );

	for ( size_t i = 0; i < HALO_SKILL_NUM; i++ )
	{
		COND_CONTINUE( pMerData->vHaloSkills[i].skillid == 0 );
		this->vecHaloSkills.push_back( pMerData->vHaloSkills[i]);
	}

	//����	
	for ( size_t i=0; i < MER_GRID_NUMS; i++)
	{
		if ( pMerData->vBodyEquip[i].itemIndex == 0 )
			continue;

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( pMerData->vBodyEquip[i].itemIndex );
		if ( itemCfg == NULL )
			continue;

		this->vBodyEquip[i] = new ITEM_CACHE( pMerData->vBodyEquip + i );

	}

	this->dwStarExp = pMerData->dwStarExp;
	this->calMerAttr();
}


CMercenary::CMercenary(  ROLE* role,const CONFIG::MERDEF_CFG* merDefCfg  )
{
	
	//size_t offset = offsetof( CMercenary, wID ); //caution
	//memset( ((char*)this) + offset,  0, sizeof(*this)-offset );

	//WORD wID;//memset �ָ���

	memset(vIndexSkills, 0, sizeof(vIndexSkills) ); // ��������
	memset(attrValueTrain, 0, sizeof(attrValueTrain) );//�������������������
	memset(tempAttrValuetrain, 0,sizeof(tempAttrValuetrain) );//�ݴ�  ÿ�����������������
	memset(vBodyEquip, 0,sizeof(vBodyEquip) );
	byFight= 0  ;//�Ƿ�ս��
	byJob  = 0  ;
	byStar = 0  ;//�Ǽ�
	bySex = 0   ;
	wLevel = 0  ;
	memset(merAttr, 0,sizeof(merAttr));
	dwFightValue      = 0;   //ս����
	dwStarExp         = 0;	//�������ǵ�ʣ�ྭ��
	suitStrengthLevel = 0;//����װ���ȼ���

	if ( merDefCfg == NULL )
	{
		return;
	}

	this->wID = merDefCfg->id;

	this->byJob = merDefCfg->job_id;

	this->bySex = merDefCfg->sex;

	//this->wLevel = role->wLevel;

	setStar( this->byStar );

	setSoltSkill( 0, merDefCfg->skill_id );

	CONFIG::MERCENARY_CFG* mercenaryCfg = getMercenaryCfg2( this->byJob, role->wLevel );

	setMerAttr( mercenaryCfg );

	for (auto& halo : merDefCfg->vecHalo )
	{
		this->vecHaloSkills.push_back( stIndexItem(halo) );
	}

}


CMercenary::~CMercenary()
{

}

void CMercenary::setMerAttr( CONFIG::MERCENARY_CFG* attrCfg )
{
	if( attrCfg == NULL )
	{
		return;
	}
	//merAttr[ E_ATTR_HP ] =			 attrCfg->hp_max;
	//merAttr[ E_ATTR_MP ] =			 attrCfg->mp_max;
	merAttr[ E_ATTR_STAMINA	    ] = attrCfg->stamina	   ;
	merAttr[ E_ATTR_STRENGTH	    ] = attrCfg->strength	   ;
	merAttr[ E_ATTR_AGILITY	    ] = attrCfg->agility	   ;
	merAttr[ E_ATTR_INTELLECT    ] = attrCfg->intellect	;
	merAttr[ E_ATTR_SPIRIT		] = attrCfg->spirit		;
	merAttr[ E_ATTR_HP_MAX2		] = attrCfg->hp_max		;
	merAttr[ E_ATTR_ATK_MAX2	    ] = attrCfg->atk_max	   ;
	merAttr[ E_ATTR_ATK_MIN2	    ] = attrCfg->atk_min	   ;
	merAttr[ E_ATTR_DEF_MAX	    ] = attrCfg->def_max	   ;
	merAttr[ E_ATTR_DEF_MIN	    ] = attrCfg->def_min	   ;
	merAttr[ E_ATTR_SPELLATK_MAX ] = attrCfg->spellatk_max;
	merAttr[ E_ATTR_SPELLATK_MIN ] = attrCfg->spellatk_min;
	merAttr[ E_ATTR_SPELLDEF_MAX ] = attrCfg->spelldef_max;
	merAttr[ E_ATTR_SPELLDEF_MIN ] = attrCfg->spelldef_min;
	merAttr[ E_ATTR_MP_MAX2		] = attrCfg->mp_max		;
	merAttr[ E_ATTR_HIT			] = attrCfg->hit		   ;
	merAttr[ E_ATTR_DODGE		] = attrCfg->dodge		;
	merAttr[ E_ATTR_CRIT		    ] = attrCfg->crit		   ;
	merAttr[ E_ATTR_CRIT_RES	    ] = attrCfg->crit_res	   ;
	merAttr[ E_ATTR_CRIT_INC	    ] = attrCfg->crit_inc	   ;
	merAttr[ E_ATTR_CRIT_RED	    ] = attrCfg->crit_red	   ;
	merAttr[ E_ATTR_MP_REC2		] = attrCfg->mp_rec		;
	merAttr[ E_ATTR_HP_REC2		] = attrCfg->hp_rec		;
	merAttr[ E_ATTR_LUCK		    ] = attrCfg->luck		   ;
	merAttr[ E_ATTR_ATT_HEM		] = attrCfg->att_hem	   ;
	merAttr[ E_ATTR_ATT_MANA	    ] = attrCfg->att_mana	   ;
	merAttr[ E_LUCK_DEF			] = attrCfg->luck_def	;
}

void CMercenary::updateNotifyFightValue( ROLE* role )
{
	notifyMerFightValue( role, this->wID);
}

void CMercenary::updateNotifyStar( ROLE* role )
{
	notifyMerStar( role, this->wID);
}


void CMercenary::updateNotifyMerAttr( ROLE* role )
{

	DWORD merTempAttr[E_ATTR_MAX];

	memcpy( merTempAttr, this->merAttr, sizeof(this->merAttr) );

	vector<WORD> vecTempSuit( this->vecSuit );

	this->calMerAttr( );

	updateNotifyFightValue( role );

	string updateData;
	S_APPEND_WORD( updateData, this->wID );
	for ( int i=E_ATTR_HP_MAX2; i< E_ATTR_MAX; i++)
	{
		if ( merTempAttr[i] != this->merAttr[i] )
		{
			S_APPEND_BYTE( updateData, i );
			S_APPEND_DWORD( updateData, this->merAttr[i] );
		}
	}
	if ( !updateData.empty() )
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SLAVE_PRO_UPDATE, updateData) );

	string suitData;
	if ( vecTempSuit != this->vecSuit )//
	{
		S_APPEND_WORD( suitData, this->wID );
		for ( auto& one : this->vecSuit )
		{
			S_APPEND_BYTE( suitData, one );
		}

		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_SUIT_INFO, suitData) );
	}
}

void CMercenary::addSuitExtraAttr()
{
	for ( auto& suit : this->mapSuit )
	{
		CONFIG::forEachSuitCfg(  suit.first, [&]( const CONFIG::SUIT_CFG& x)
		{
			if ( suit.second >= x.suit_number )
			{
				for ( auto sthit = x.mapStrengthEffAttr.rbegin(); sthit != x.mapStrengthEffAttr.rend(); ++sthit )
				{
					if ( this->suitStrengthLevel >= sthit->first )
					{
						for ( auto& addattr : sthit->second )
						{
							this->merAttr[ addattr.attr ] += DWORD( addattr.value );
						}

						break;//�ҵ����ȼ�����
					}
				}
				this->vecSuit.push_back( suit.first );
			}
		} 
		);
	}
}

void CMercenary::calMerAttr( )
{
	CONFIG::MERCENARY_CFG* mercenaryCfg = getMercenaryCfg2( this->byJob, this->wLevel );

	setMerAttr( mercenaryCfg );//ְҵ�Ա���������

	for ( BYTE type = 0; type < BASE_ATTR_NUM; type++ )
	{
		ADDUP( this->merAttr[ E_ATTR_STAMINA + type ], this->attrValueTrain[type] );//��������
	}

	CONFIG::MER_STAR_CFG* merStarCfg = CONFIG::getMerUpstarCfg( byStar );

	for ( BYTE type = 0; type < BASE_ATTR_NUM; type++ )
	{
		float ratio = 1.0f;

		if ( merStarCfg != NULL )
		{
			ratio = 1.0f + merStarCfg->baseAttrRatio[type];//�Ǽ�������Ӱ��
		}

		DWORD dwTempAttrValue = DWORD(this->merAttr[ E_ATTR_STAMINA + type ] * ratio);

		ADDUP( dwTempAttrValue, this->attrValueTrain[type] );

		tranformBaseAttr( E_ATTR_STAMINA + type, dwTempAttrValue );//����ת��
	}

	//����
	for (int i = 0; i < MER_BODY_GRID_NUM; i++)
	{
		ITEM_CACHE* itemCache = this->vBodyEquip[i];
		if (itemCache == NULL)
			continue;

		//��������
		for (int j = 0; j < MAX_EQUIP_BASE_ATTR_NUMS; j++)
		{
			BYTE attr = itemCache->attrValueBase[j].attr;

			if (!IS_VALID_ATTR(attr))
				continue;

			this->merAttr[attr] += itemCache->attrValueBase[j].value;

		}

		// ��������
		auto& allup_item = itemCache->itemCfg->equip_extend.all_up_attr.values;
		auto upvalue = 1.f;
		if (itemCache->star_level > 0 && itemCache->star_level < allup_item.size())
			upvalue = 1.f + allup_item[itemCache->star_level - 1];
		for (auto& item : itemCache->equip_extends)
		{
			this->merAttr[item.type] += (uint)(item.value * upvalue);
		}
		if (itemCache->star_level > 0 && itemCache->star_level < itemCache->itemCfg->equip_extend.value_attr.values.size())
		{
			this->merAttr[itemCache->itemCfg->equip_extend.value_attr.type] +=
				(uint)itemCache->itemCfg->equip_extend.value_attr.values[itemCache->star_level];
		}


		//��������,����ǿ���ȼ�Ӱ��
		for (int j = 0; j < MAX_EQUIP_EXTRA_ATTR_NUMS; j++)
		{
			BYTE attr = itemCache->attrValueExtra[j].attr;

			if (!IS_VALID_ATTR(attr))
				continue;

			this->merAttr[attr] += itemCache->attrValueExtra[j].value;
		}



		WORD suitID = itemCache->itemCfg->suit;
		if (suitID != 0)
		{
			this->mapSuit[itemCache->itemCfg->suit] += 1;
		}
		this->suitStrengthLevel += (WORD)itemCache->byStrengthLv;
	}

	this->addSuitExtraAttr();

	this->calMerFightValue();
}

static float attRatio[ E_ATTR_MAX ]={0.0f,0.0f,0.2f,0.0f,1.0f,2.0f,0.0f,2.0f,0.0f,0.0f,0.0f,5.0f,0.0f,2.0f,0.0f,5.0f,0.0f,0.0f,0.0f,0.3f,0.3f,5.0f,0.0f,0.0f,0.0f,0.0f,0.0f,2.5f,2.5f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};

#define FIGHTATTR( attrtype )  (float)( merAttr[attrtype] * attRatio[attrtype] )

void  CMercenary::calMerFightValue( )
{

	float fValue = 0;
	fValue = FIGHTATTR(E_ATTR_HP_MAX2) + FIGHTATTR(E_ATTR_MP_MAX2) + FIGHTATTR(E_ATTR_ATK_MAX2) + FIGHTATTR(E_ATTR_ATK_MIN2) + FIGHTATTR(E_ATTR_SPELLATK_MAX) + FIGHTATTR(E_ATTR_SPELLATK_MIN)\
		+ FIGHTATTR(E_ATTR_SOL_MIN) + FIGHTATTR(E_ATTR_DODGE) + FIGHTATTR(E_ATTR_MP_REC2) + FIGHTATTR(E_ATTR_HP_REC2) + FIGHTATTR(E_ATTR_HIT)\
		+ FIGHTATTR(E_ATTR_CRIT) + FIGHTATTR(E_ATTR_CRIT_RES) + FIGHTATTR(E_ATTR_CRIT_INC) + FIGHTATTR(E_ATTR_CRIT_RED) ;
	this->dwFightValue = (DWORD)fValue;
}

void CMercenary::tranformBaseAttr( const BYTE& type, const DWORD& value )
{
	if ( type < E_ATTR_STAMINA || type > E_ATTR_SPIRIT )//��֤ һ������  ��������
	{
		return ;
	}

	if ( value == 0 )
	{
		return;
	}

	CONFIG::ATTRDEFORM_CFG* attrTransCfg = CONFIG::getAttrDeformCfg(type);
	if ( attrTransCfg == NULL )
	{
		return;
	}

	for ( auto& it : attrTransCfg->vecAttrDeform )
	{
		this->merAttr[ it.attr ] += DWORD( it.ratio * value ) ;
	}
}

void CMercenary::setFightStatus( BYTE byFight )
{
	this->byFight = byFight;
}

void CMercenary::setStar( BYTE byStar )
{
	this->byStar = byStar;
	unLockSkills();
}

void CMercenary::unLockSkills()
{
	for ( BYTE i = 0; i < SKILL_MAX_NUM; i++ )
	{
		if ( this->byStar >= NETMERCENARY::s_byNeedStar[i] )
		{
			if ( vIndexSkills[i].byStatus == E_SKILL_SOLT_OFF )
			{
				vIndexSkills[i].byStatus = E_SKILL_SOLT_ON;
			}
		}
	}
}



void CMercenary::setSoltSkill( BYTE bySolt, WORD wSkill )
{
	CONFIG::SKILL_CFG2* skillCfg = CONFIG::getSkillCfg(wSkill);
	if ( skillCfg == NULL )
	{
		return;
	}

	if ( bySolt >= SKILL_MAX_NUM )
	{
		return ;
	}

	if ( vIndexSkills[ bySolt ].byStatus == E_SKILL_SOLT_OFF )
	{
		return;
	}
	
	vIndexSkills[ bySolt ].skillid = wSkill;
	vIndexSkills[ bySolt ].group = skillCfg->group;
}

BYTE CMercenary::getFightStatus( )
{
	return this->byFight;
}

void CMercenary::refreshSkills(  BYTE* vLock  )
{
	CONFIG::SKILL_STORAGE* skillStorageCfg = CONFIG::getSkillStorage( this->byJob );
	if ( skillStorageCfg == NULL )
	{
		return;
	}

	for ( size_t i = 0; i < SKILL_MAX_NUM; i++ )
	{
		if ( vLock[i] == E_SKILL_LOCK )
		{
			continue;
		}

		vIndexSkills[i].skillid = 0;
		vIndexSkills[i].group = 0;	
	}

	for ( size_t i = 0; i < SKILL_MAX_NUM; i++ )
	{
		COND_CONTINUE( vLock[i] == E_SKILL_LOCK  );

		COND_CONTINUE( vIndexSkills[i].byStatus == E_SKILL_SOLT_OFF  );

		deque<WORD> dequeTempIDs( skillStorageCfg->vecGroupID.begin(), skillStorageCfg->vecGroupID.end() );

		for ( size_t j = 0; j < SKILL_MAX_NUM; j++ )
		{
			if ( vIndexSkills[j].group != 0 )
			{
				auto it = find(dequeTempIDs.begin(), dequeTempIDs.end(), vIndexSkills[j].group);
				if (it != dequeTempIDs.end())
				{
					dequeTempIDs.erase(it);
				}
			}
		}

		size_t randGroup = rand() % dequeTempIDs.size();

		WORD group = dequeTempIDs[ randGroup ];

		vector<CONFIG::SKILL_CFG2*>& vecSkillCfg= skillStorageCfg->mapGroupSkills[group];

		int nMaxByLevel = this->wLevel <= 3 ? 1 : ( this->wLevel - 3 ) / 2;

		//int nMax = nMaxByLevel > 0 ? nMaxByLevel : 1;

		int nMax = std::min<size_t>( nMaxByLevel, vecSkillCfg.size() );

		//size_t randSkill = BASE::randTo(nMax);

		size_t randSkill = rand() % nMax;

		CONFIG::SKILL_CFG2* skillCfg = vecSkillCfg[randSkill];

		if ( skillCfg != NULL )
		{
			vIndexSkills[i].skillid = skillCfg->id;
			vIndexSkills[i].group = skillCfg->group;	
		}

	}
}
