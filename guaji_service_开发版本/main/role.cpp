#include "pch.h"
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
#include "db.h"
#include "task.h"
#include "itemCtrl.h"
#include "logs_db.h"
#include "service.h"
#include "WorldBoss.h"
#include "sign.h"
#include "broadCast.h"
#include "common.h"
#include "activity.h"
#include "mailCtrl.h"
#include "quickfight.h"
#include "team.h"
#define  MAX_NEWBIE_GUIDE_STEP  50

using namespace BASE;


 bool isEmailAddress( const string& client_email )
{
	return true;

	RETURN_COND( client_email.empty(), false);

	const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");

	return std::regex_match(client_email, pattern);
}

void ROLE::resetVip()
{
	WORD wLevel = 0;

	DWORD dwCurExp = 0;

	CONFIG::addExp( wLevel, dwCurExp, this->dwCharge, MAX_VIP_LEVEL, CONFIG::getVipUpgradeExp );

	setVipLevel( wLevel );

	ITEM_INFO para(0, this->getVipLevel());
	RoleMgr::getMe().judgeCompleteTypeTask(this, E_TASK_VIP_LOGIN, &para);

}

void ROLE::setVipLevel( const WORD wLevel )
{
	if ( this->wVipLevel != wLevel )
	{
		this->wVipLevel = wLevel;
		notifywVipLevel(this);
	}
}

void ROLE::setLoginVipLevel(const WORD wLevel)
{
	if (this->wLoginVip != wLevel)
	{
		this->wLoginVip = wLevel;
		notifywVipLevel(this);
	}
}
void ROLE::addFiveDivineTime()
{

}

string ROLE::get_phone_imei()
{
	return this->phone_imei;
}

int ROLE::setNewbieStep( ROLE* role, unsigned char * data, size_t dataLen)
{
	BYTE step;

	RETURN_COND(!BASE::parseBYTE(data, dataLen, step), 0);

	RETURN_COND( step > MAX_NEWBIE_GUIDE_STEP, 0);

	role->byNewbieGuideStep = step;

	string strData;
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_NEWBIE_GUIDE, strData) );
	return 0 ;
}

////װ��������ȡ
//#define S_EQUIP_AWARD_FETCH	0x0010
//C:[]
//S:[BYTE, BYTE]
//[error, nextAward]
//error:������
//		  0:�ɹ�
//		  1:�ȼ�δ�ﵽ
//		  2:װ��������
//		  3:���߱�����
//		  4:����������
int ROLE::getLevelAward( ROLE* role, unsigned char * data, size_t dataLen)
{
	BYTE& byAwardStep = role->byCurLevelAwardStep;

	BYTE byJob = role->byJob;

	BYTE byRet = 0;

	RETURN_COND( byJob == 0 || byJob > E_JOB_MONK, 0 );

	do 
	{
		CONFIG::TARGET_AWARD_CFG *pTargetAwardCfg = CONFIG::getTargetAwardCfg( WORD( byAwardStep ) );

		COND_BREAK( pTargetAwardCfg==NULL, byRet, 4);

		COND_BREAK( pTargetAwardCfg->level > role->wLevel, byRet, 1);

		ITEM_INFO& oneItem = pTargetAwardCfg->vecItem[ byJob - 1 ];

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( oneItem.itemIndex );

		COND_BREAK( itemCfg==NULL, byRet, 4);

		if ( ItemCtrl::makeOneTypeItem( role, itemCfg, oneItem.itemNums, oneItem.byQuality ) == -1 )
		{
			COND_BREAK( IS_EQUIP(itemCfg), byRet, 2 );

			COND_BREAK( 1 ,byRet, 3 );
		}

		ADDUP(byAwardStep, 1);

	} while (0);

	string strData;
	S_APPEND_BYTE(strData, byRet);
	S_APPEND_BYTE(strData, byAwardStep);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EQUIP_AWARD_FETCH, strData) );
	return 0 ;
}

////��ȡ�׳佱��
//#define	S_FETCH_RECHARGE_AWARD	0x004B
//C:[]
//S:[WORD]
//[error]
//error:0:�ɹ�
//		  1:��ȡ����δ�ﵽ,�״γ�ֵ�����ȡ
//		  2:װ��������
//		  3:���߱�����
//		  4:�Ѿ���ȡ
int ROLE::getRechargeAward( ROLE* role, unsigned char * data, size_t dataLen)
{
	WORD  wErr = 0;

	do 
	{
		COND_BREAK( role->byRechargeState == E_TASK_UNCOMPLETE, wErr, 1 );

		COND_BREAK( role->byRechargeState == E_TASK_COMPLETE_ALREADY_GET_PRIZE, wErr, 4 );

		vector< ITEM_INFO > vecItemInfo;

		CONFIG::forEachRechargeAwardCfg( [&]( const CONFIG::RECHARGE_AWARD_CFG& recharge_ele )
		{

			CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( recharge_ele.item );

			RETURN_VOID( itemCfg == NULL );

			RETURN_VOID( IS_JOB_LIMIT(role, itemCfg) );

			vecItemInfo.push_back( ITEM_INFO(recharge_ele.item, recharge_ele.number, recharge_ele.rare ) );
		});
	
		int result = ItemCtrl::getEnoughSlot( role, vecItemInfo );

		COND_BREAK( result& (1 << E_SLOT_EQUIP), wErr, 2  );

		COND_BREAK( result& (1 << E_SLOT_PROP), wErr, 3  );

		CONFIG::forEachRechargeAwardCfg( [&]( const CONFIG::RECHARGE_AWARD_CFG& recharge_ele )
		{
			list<ITEM_CACHE*> lstItemCachePkg, lstPropsCachePkg;

			CONFIG::ITEM_CFG* itemInfoCfg = CONFIG::getItemCfg( recharge_ele.item );

			RETURN_VOID( itemInfoCfg == NULL );

			RETURN_VOID( IS_JOB_LIMIT(role, itemInfoCfg) );

			ItemCtrl::makeItemByType(role, itemInfoCfg, &lstItemCachePkg, &lstPropsCachePkg, ITEM_INFO(recharge_ele.item, recharge_ele.number, recharge_ele.rare ) );

			for (auto& item_cache : lstItemCachePkg )
			{
				item_cache->byStrengthLv = recharge_ele.strengthen;
				item_cache->byHole = (BYTE)recharge_ele.hole;
				//memcpy(item_cache->attrOldValueExtra, recharge_ele.random, sizeof(item_cache->attrOldValueExtra) );
				for ( size_t i = 0; i < MAX_EQUIP_EXTRA_ATTR_NUMS; i++ )
				{
					item_cache->attrOldValueExtra[i].value = recharge_ele.random[i];
				}

				item_cache->resetItemAttrValue();
			}

			ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_EQUIP);
			ItemCtrl::notifyClientItem(role, lstPropsCachePkg, PKG_TYPE_ITEM);
		});

		role->byRechargeState = E_TASK_COMPLETE_ALREADY_GET_PRIZE;

	} while (0);

	string strData;
	S_APPEND_WORD(strData, wErr);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FETCH_RECHARGE_AWARD, strData) );
	return 0 ;
}

//#define	S_TODAY_GUIDE					0x0060//��������
//C:[ ]
//S: [BYTE]8��
int ROLE::getTodayGuide( ROLE* role, unsigned char * data, size_t datalen )
{
	string strData;
	BYTE byMoneyTreeTimes = 0;
	BYTE byBuyQuickFightTimes = 0;
	BYTE byWorldBoss = role_worldboss._bSignup ? 1 : 0;

	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg( role->getVipLevel());
	if ( vipCfg != NULL && vipCfg->money_buy > role->byUseMoneyTreeTimes)
	{
		byMoneyTreeTimes = vipCfg->money_buy - role->byUseMoneyTreeTimes;
	}

	if (vipCfg != NULL && vipCfg->time_buy > role->byBuyQuickFightTimes)
	{
		byBuyQuickFightTimes = vipCfg->time_buy - role->byBuyQuickFightTimes;
	}

	MYSTERY_SHOP_INFO& mysCommonInfo = this->cMarMgr.cMarMys.m_vMysInfo[E_MYSTERY_SHOP_COMMON - 1];
	MYSTERY_SHOP_INFO& mysHignInfo = this->cMarMgr.cMarMys.m_vMysInfo[E_MYSTERY_SHOP_HIGN - 1];

	S_APPEND_BYTE(strData, byBuyQuickFightTimes);
	S_APPEND_BYTE(strData, role->byAllBuyBossTimes + role->bySendFightBossTimes);
	S_APPEND_BYTE(strData, MAX_ARENA_USE_NUMS + role->byBuyArenaDayCount - role->byArenaDayCount);
	S_APPEND_BYTE(strData, mysCommonInfo.wFreeTimes );
	S_APPEND_BYTE(strData, byMoneyTreeTimes);
	S_APPEND_BYTE(strData, byWorldBoss);
	S_APPEND_BYTE(strData, role->byAllBuyHeroTimes + role->bySendFightHeroTimes);
	S_APPEND_BYTE(strData, role->role_rune.m_divine_times );
	S_APPEND_BYTE(strData, mysHignInfo.wFreeTimes );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_TODAY_GUIDE, strData) );
	return 0;
}

BYTE ROLE::isCanSetNewSkill()
{
	return role_skill.isCanSetNewSkill();
}

BYTE ROLE::isCanUpSkill()
{
	return role_skill.isCanUpSkill();
}

BYTE ROLE::isCanActivityReward() 
{
	return TASK::isCanActivityReward( this );
}

BYTE ROLE::isCanTaskReward()		
{
	return TASK::isCanTaskReward( this );
}

BYTE ROLE::isNotSign()			
{
	BYTE byDay = SIGN::getDay();

	REDUCEUNSIGNED(byDay, 1);

	const DWORD dwCode = 0x00000001;

	const DWORD dwDayCode = dwCode << byDay;//����

	if( dwDayCode & this->dwDaySignStatus )//�Ѿ���ȡ
	{
		return 0;
	}

	return 1;
}

BYTE ROLE::isMailReward()			
{
	return ( this->mapMail.empty() ? 0 : 1);
}

BYTE ROLE::isNotJoinWorldBoss()	
{
	return ( role_worldboss._bSignup ? 0 : 1 );
}

BYTE ROLE::isMysteryCommonFree()	
{
	uint now = time(nullptr);

	MYSTERY_SHOP_INFO& mysInfo = this->cMarMgr.cMarMys.m_vMysInfo[E_MYSTERY_SHOP_COMMON - 1];

	RETURN_COND( mysInfo.wFreeTimes > 0 && mysInfo.dwCanTreeTime < now ,1 )//one free

	return 0;
}

BYTE ROLE::isMysteryHignFree()	
{
	uint now = time(nullptr);

	MYSTERY_SHOP_INFO& mysInfo = this->cMarMgr.cMarMys.m_vMysInfo[E_MYSTERY_SHOP_HIGN - 1];

	RETURN_COND( mysInfo.wFreeTimes > 0 && mysInfo.dwCanTreeTime < now ,1 )//one free

	return 0;
}	

BYTE ROLE::haveUseMoneyTreeTimes()
{
	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(this->getVipLevel());

	RETURN_COND( vipCfg != NULL && vipCfg->money_buy > this->byUseMoneyTreeTimes , 1 );

	return 0;
}	
BYTE ROLE::haveDivideTimes()		
{
	return ( this->role_rune.m_divine_times > 0 ? 1 : 0 );
}

BYTE ROLE::haveFightArenaTimes()	
{
	return (MAX_ARENA_USE_NUMS + this->byBuyArenaDayCount > this->byArenaDayCount);
}

BYTE ROLE::haveFightBossTimes()	
{
	return ( ( this->byAllBuyBossTimes + this->bySendFightBossTimes ) > 0 ? 1 : 0 );
}	

BYTE ROLE::haveFightEelteTimes()	
{
	return ( ( this->byAllBuyHeroTimes + this->bySendFightHeroTimes ) > 0 ? 1 : 0 );
}

BYTE ROLE::haveQuickFightTimes()	
{
	return ( this->byBuyQuickFightTimes > 0 ? 1 : 0 );
}

BYTE ROLE::canForgeEquip()		
{
	const DWORD dwMaxNeedSmelt = 200 ;
	return ( this->dwSmeltValue >= dwMaxNeedSmelt ? 1 : 0 );
}	

BYTE ROLE::isFullEquipPkg()		
{
	return ( this->m_packmgr.lstEquipPkg.size() >= this->m_packmgr.curEquipPkgLimit ? 1 : 0 );
}	

BYTE ROLE::haveItemBox()			
{
	for ( const auto it : this->m_packmgr.lstItemPkg )
	{
		if ( IS_BAOXIANG(it->itemCfg) || IS_FIXED_BOX(it->itemCfg) )
		{
			return 1;
		}
	}
	return 0;
}

BYTE ROLE::isFriendEmpty()		
{
	return ( this->vecFriends.empty() ? 1 : 0 );
}	
//#define		S_TIME_GUIDE				0x0063		//�ͻ���20min���������
//C:[ ]
//S: [BYTE]20��
int ROLE::getTimeGuide( ROLE* role, unsigned char * data, size_t datalen )
{
//101	2	12	80	һ���µļ��ܲۿ����ˣ��Ͻ�����һ�����ܰɡ�
//102	2	12	80	���¼��ܿ��������ˣ��������ܸ�ǿ����
//103	2	12	80	�������������Ŷ��
//104	2	12	80	��������������Ŷ��
//105	2	12	80	���ˣ����컹û��ǩ��Ŷ��
//106	2	12	80	���ˣ����ˣ����н����ʼ�û����Ŷ��
//107	2	12	80	����boss�ܹ������ˣ��Ͻ������ɡ�
//108	2	12	80	�̳ǵ���ͨ�����������Ŷ���Ͻ��齱ȥ�ɡ�
//109	2	12	80	�̳ǵĸ߼������������Ŷ���Ͻ��齱ȥ�ɡ�
//110	2	12	80	Ҫ��ȱǮ�ˣ���������ҡǮ��Ŷ��
//111	2	12	80	һ�����֣��������С��������ռ������
//112	2	12	80	ÿ�첻��ս�������֣��������Ǿ���ȱ��ʲô��
//113	2	12	80	����BOSS��ÿ���ճ���
//114	2	12	80	Ӷ��һֱϣ������һ��ȥ��Ӣ������
//115	2	12	80	�������죬����ս�������١�
//116	2	12	80	����װ�������Ŵ���˵����������װ�ˡ�
//117	2	12	80	�����������ˣ��Ͻ�����һ�°ɡ�
//118	2	12	80	������ڱ����ﲻ�ã������˷���Դ��
//119	2	12	80	���ˣ�����û�к����أ��Ͻ���Щ���Ѱɡ�

	string strData;

	BYTE   bySetSkill			 = isCanSetNewSkill()		;
	BYTE   byUpSkill			 = isCanUpSkill()			;
	BYTE   byActivityReward		 = isCanActivityReward()	;
	BYTE   byTaskReward			 = isCanTaskReward()		;
	BYTE   byNotSign			 = isNotSign()				;
	BYTE   byMailReward			 = isMailReward()			;
	BYTE   byNotWorldBoss		 = isNotJoinWorldBoss()		;
	BYTE   byMysteryCommonFree	 = isMysteryCommonFree()	;
	BYTE   byMysteryHignFree	 = isMysteryHignFree()		;
	BYTE   byCanUseMoneyTree	 = haveUseMoneyTreeTimes()	;
	BYTE   byCanDivide			 = haveDivideTimes()		;
	BYTE   byCanFightArena		 = haveFightArenaTimes()	;
	BYTE   byCanFightBoss		 = haveFightBossTimes()		;
	BYTE   byCanFightEelte		 = haveFightEelteTimes()	;
	BYTE   byCanQuickFight		 = haveQuickFightTimes()	;
	BYTE   byCanForge			 = canForgeEquip()			;
	BYTE   byShouldSmelt		 = isFullEquipPkg()			;
	BYTE   byHaveItemBox		 = haveItemBox()			;
	BYTE   byNotHaveFriend		 = isFriendEmpty()			;

	
	S_APPEND_BYTE( strData, bySetSkill			);
	S_APPEND_BYTE( strData, byUpSkill			);
	S_APPEND_BYTE( strData, byActivityReward	);
	S_APPEND_BYTE( strData, byTaskReward		);
	S_APPEND_BYTE( strData, byNotSign			);
	S_APPEND_BYTE( strData, byMailReward		);
	S_APPEND_BYTE( strData, byNotWorldBoss		);
	S_APPEND_BYTE( strData, byMysteryCommonFree	);
	S_APPEND_BYTE( strData, byMysteryHignFree	);
	S_APPEND_BYTE( strData, byCanUseMoneyTree	);
	S_APPEND_BYTE( strData, byCanDivide			);
	S_APPEND_BYTE( strData, byCanFightArena		);
	S_APPEND_BYTE( strData, byCanFightBoss		);
	S_APPEND_BYTE( strData, byCanFightEelte		);
	S_APPEND_BYTE( strData, byCanQuickFight		);
	S_APPEND_BYTE( strData, byCanForge			);
	S_APPEND_BYTE( strData, byShouldSmelt		);
	S_APPEND_BYTE( strData, byHaveItemBox		);
	S_APPEND_BYTE( strData, byNotHaveFriend		);

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_TIME_GUIDE, strData) );
	return 0;
}



////�޸İ�����
//S_MODIFY_BIND_EMAIL = 0x0935
//C:[BSTR]
//[bindEmail]
//bindEmail:�󶨵������ַ
//S:[WORD]
//error
//	0:�ɹ�
//	1:û�г�ֵ,���ܰ�
//	2:�Ƿ��������ַ
int ROLE::setEmailAddress( ROLE* role, unsigned char * data, size_t datalen )
{
	string client_email;
	string strData;
	WORD   error = 0;

	if ( !BASE::parseBSTR( data, datalen, client_email) )
		return 0;
	do 
	{
		COND_BREAK( role->vecFirstCharge.empty(), error, 1 );

		COND_BREAK( !isEmailAddress(client_email), error, 2 );

		COND_BREAK( !EXEC_SQL( "update " TABLE_ROLE_INFO  " set email='%s' where id = %u ", client_email.c_str(),role->dwRoleID ), error, 2 );

		role->email = client_email;
	
	} while (0);
	
	S_APPEND_WORD( strData, error );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_MODIFY_BIND_EMAIL, strData) );
	return 0 ;
}

////�����ֵ��������Ϣ
//S_REQUEST_BIND_EMAIL = 0x0934
//	C[]
//S:[DWORD, BSTR]
//[chargeTotal, bindEmail]
//chargeTotal:�ۼƳ�ֵԪ������
//bindEmail:�󶨵������ַ
int ROLE::getEmailAddress( ROLE* role, unsigned char * data, size_t datalen )
{
	string strData;
	DWORD& dwAllCharge = role->dwCharge;
	string& strEmail = role->email ;
	S_APPEND_DWORD( strData, dwAllCharge );
	S_APPEND_BSTR( strData, strEmail );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_BIND_EMAIL, strData) );
	return 0 ;
}

////�һ���(CDKey)�һ�����
//S_EXCHANGE_AWARDS_BY_KEY = 0x0936
//C:[BSTR]
//[cdkey]
//cdkey:�һ���
//S:[WORD]
//error
//	0:�ɹ�
//	1:�öһ�����Ч
//	2:װ����������
//	3:���߱�������
//	4:����ȡ����������������ظ���ȡ
//	5:�ü������ѱ�ʹ��
//	6:��ȡ���߲�����

int ROLE::getCodeAward( ROLE* role, unsigned char * data, size_t datalen )
{
	string strCode;
	string strData;
	WORD   wError = 0;
	RETURN_COND( !BASE::parseBSTR( data, datalen, strCode ), 0 );

	do 
	{
		SQLSelect sqlx( DBCtrl::getSQL(), "select item_id,type,is_use from " CODE_AWARD " where code = '%s' limit 1;", strCode.c_str() );
		MYSQL_ROW row = sqlx.getRow();

		COND_BREAK( nullptr == row, wError, 1 );

		WORD wItemID = ATOW( row[0] );

		BYTE byType = ATOB( row[1] );

		BYTE byUse = ATOB( row[2] );

		COND_BREAK( 1 == byUse, wError, 5 );

		CONFIG::ITEM_CFG* pItemCfg = CONFIG::getItemCfg(wItemID);

		COND_BREAK( nullptr == pItemCfg, wError, 6 );

		COND_BREAK( std::find( role->vecCodeAwardType.begin(), role->vecCodeAwardType.end(), byType ) != role->vecCodeAwardType.end(), wError, 4 );

		if (ItemCtrl::makeOneTypeItem(role, pItemCfg, 1) == -1 )
		{
			COND_BREAK( IS_EQUIP( pItemCfg ),wError,2 );
			COND_BREAK(1, wError, 3);
		}

		role->vecCodeAwardType.push_back(byType);

		execSQL(DBCtrl::getSQL(), "update " CODE_AWARD" set is_use = 1 where code = '%s'", strCode.c_str() );

	} while (0);

	S_APPEND_WORD( strData, wError );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EXCHANGE_AWARDS_BY_KEY, strData) );
	return 0 ;
}


void ROLE::setFirstLoginTime( DWORD dwTime )
{
	this->dwFisrtLoginTime = dwTime;
}

DWORD ROLE::getFirstLoginTime()
{
	return this->dwFisrtLoginTime;
}


BYTE ROLE::getMinEquipLevel()
{
	BYTE level = 0;
	//����
	for ( int i=0; i<BODY_GRID_NUMS; i++)
	{
		ITEM_CACHE* itemCache = this->vBodyEquip[i];

		COND_CONTINUE( itemCache == NULL || itemCache->itemCfg == NULL);

		if ( i == 0 )
		{
			level = (BYTE)itemCache->itemCfg->level_limit;
		}
		else
		{
			level = min<BYTE>( level, (BYTE)itemCache->itemCfg->level_limit );
		}
	}

	return level;
}

void ROLE::resetEliteMap()
{
	CONFIG::resetEliteMap( [&](const map<WORD, DOUBLE_WORD>& x)
	{
		this->mapElite = x;
	}
		);
}


BYTE ROLE::roleGetCharge( const int nChargeNum, const int buyRealNum, bool bGm )
{
	logonline("::::this->dwRoleID=%d, nChargeNum=%d, buyRealNum=%d", this->dwRoleID, nChargeNum, buyRealNum);

	int switchChargeNum = CONFIG::getChargeCfg(buyRealNum);

	if ( switchChargeNum == 0 )
	{
		if ( bGm )
		{
			switchChargeNum = buyRealNum;
		}
		else
		{
			return 1;
		}
	}

	RETURN_COND( nChargeNum <= 0, 1 );

	ADDUP( this->dwIngot, nChargeNum );

	activity_taobao::charge(dwRoleID, (unsigned)buyRealNum);
	
	ADDUP( this->dwCharge, buyRealNum );

	WORD oldLevel = this->wVipLevel;

	this->resetVip();

	notifyIngot( this );

	notifyCharge( this );

	logonline("vip=%d  get=%d",this->wVipLevel,getVipLevel());
	if ( oldLevel != this->wVipLevel )
	{
		notifywVipLevel( this );
	}

	logs_db_thread::singleton().consume(roleName, dwRoleID, 0, buyRealNum, purpose::charge, 0);

	//20150701 ���ã���php���±�������
	//if ( switchChargeNum != 0 )
	//{
	//	EXEC_SQL("update " TABLE_CHARGE_INFO  " set charge=charge+%d where roleid = %u ", switchChargeNum, this->dwRoleID);
	//}
	
	return 0;
}

/*
@�˵�ս���� 
@����ֵ����ϵ��������ӣ�������attRe���崦
*/
void ROLE::calRoleAttr( )
{
	memset( this->roleAttr, 0, sizeof(this->roleAttr) );
	this->mapSuit.clear();
	this->vecSuit.clear();
	CONFIG::ATTR_UPGRADE_CFG* attrCfg = CONFIG::getAttrCfg( this->byJob, this->wLevel );
	if ( attrCfg )
	{
		//this->roleAttr[ E_ATTR_HP_MAX2 ] += attrCfg->hp_max;
		//this->roleAttr[ E_ATTR_MP_MAX2 ] += attrCfg->mp_max;
		this->roleAttr[ E_ATTR_STAMINA	    ] += attrCfg->stamina	   ;
		this->roleAttr[ E_ATTR_STRENGTH	    ] += attrCfg->strength	   ;
		this->roleAttr[ E_ATTR_AGILITY	    ] += attrCfg->agility	   ;
		this->roleAttr[ E_ATTR_INTELLECT    ] += attrCfg->intellect	;
		this->roleAttr[ E_ATTR_SPIRIT		] += attrCfg->spirit		;
		this->roleAttr[ E_ATTR_HP_MAX2		] += attrCfg->hp_max		;
		this->roleAttr[ E_ATTR_ATK_MAX2	    ] += attrCfg->atk_max	   ;
		this->roleAttr[ E_ATTR_ATK_MIN2	    ] += attrCfg->atk_min	   ;
		this->roleAttr[ E_ATTR_DEF_MAX	    ] += attrCfg->def_max	   ;
		this->roleAttr[ E_ATTR_DEF_MIN	    ] += attrCfg->def_min	   ;
		this->roleAttr[ E_ATTR_SPELLATK_MAX ] += attrCfg->spellatk_max;
		this->roleAttr[ E_ATTR_SPELLATK_MIN ] += attrCfg->spellatk_min;
		this->roleAttr[ E_ATTR_SPELLDEF_MAX ] += attrCfg->spelldef_max;
		this->roleAttr[ E_ATTR_SPELLDEF_MIN ] += attrCfg->spelldef_min;
		this->roleAttr[ E_ATTR_MP_MAX2		] += attrCfg->mp_max		;
		this->roleAttr[ E_ATTR_HIT			] += attrCfg->hit		   ;
		this->roleAttr[ E_ATTR_DODGE		] += attrCfg->dodge		;
		this->roleAttr[ E_ATTR_CRIT		    ] += attrCfg->crit		   ;
		this->roleAttr[ E_ATTR_CRIT_RES	    ] += attrCfg->crit_res	   ;
		this->roleAttr[ E_ATTR_CRIT_INC	    ] += attrCfg->crit_inc	   ;
		this->roleAttr[ E_ATTR_CRIT_RED	    ] += attrCfg->crit_red	   ;
		this->roleAttr[ E_ATTR_MP_REC2		] += attrCfg->mp_rec		;
		this->roleAttr[ E_ATTR_HP_REC2		] += attrCfg->hp_rec		;
		this->roleAttr[ E_ATTR_LUCK		    ] += attrCfg->luck		   ;
		this->roleAttr[ E_ATTR_ATT_HEM		] += attrCfg->att_hem	   ;
		this->roleAttr[ E_ATTR_ATT_MANA	    ] += attrCfg->att_mana	   ;
		this->roleAttr[ E_ATTR_EQU_DROP	    ] += attrCfg->equ_drop	   ;
		this->roleAttr[ E_ATTR_MONEY_DROP	] += attrCfg->money_drop	;
		this->roleAttr[ E_LUCK_DEF			] += attrCfg->luck_def	;
	}

	//����
	for ( int i=0; i<BODY_GRID_NUMS; i++)
	{
		ITEM_CACHE* itemCache = this->vBodyEquip[i];
		if ( itemCache == NULL )
			continue;

		if ( itemCache->itemCfg==NULL )
		{
			 this->vBodyEquip[i] = NULL;
			 continue;
		}

		//��������
		for ( int j=0; j < MAX_EQUIP_BASE_ATTR_NUMS; j++)
		{
			BYTE attr = itemCache->attrValueBase[ j ].attr;

			if ( !IS_VALID_ATTR( attr  ) )
				continue;

			this->roleAttr[ attr ] += itemCache->attrValueBase[ j ].value;
		}

		// ��������
		auto& allup_item = itemCache->itemCfg->equip_extend.all_up_attr.values;
		auto upvalue = 1.f;
		if (itemCache->star_level > 0u && (itemCache->star_level - 1u) < allup_item.size())
			upvalue = 1.f + allup_item[itemCache->star_level - 1];
		for(auto& item : itemCache->equip_extends)
		{
			roleAttr[item.type] += (uint)(item.value * upvalue);
		}
		auto& value_attr = itemCache->itemCfg->equip_extend.value_attr;
		if (itemCache->star_level > 0u && (itemCache->star_level - 1u) < value_attr.values.size())
		{
			roleAttr[value_attr.type] += (uint)value_attr.values[itemCache->star_level - 1];
		}

		//��������,����ǿ���ȼ�Ӱ��
		for ( int j=0; j < MAX_EQUIP_EXTRA_ATTR_NUMS; j++)
		{
			BYTE attr = itemCache->attrValueExtra[ j ].attr;

			if ( !IS_VALID_ATTR( attr  ) )
				continue;

			this->roleAttr[ attr ] += itemCache->attrValueExtra[ j ].value;
		}

		WORD suitID = itemCache->itemCfg->suit;
		if ( suitID != 0  && itemCache->byQuality >=5 )
		{
			this->mapSuit[itemCache->itemCfg->suit] += 1;
		}
		this->suitStrengthLevel += (WORD)itemCache->byStrengthLv;

		this->roleAttr[ E_ATTR_LUCK			] += itemCache->byLuckPoint	;
		this->roleAttr[ E_LUCK_DEF			] += itemCache->byLuckDefPoint	;
	}

	for ( BYTE type = E_ATTR_STAMINA; type <= E_ATTR_SPIRIT; type++ )
	{
		this->tranformBaseAttr( type, this->roleAttr[ type ] );
	}

	this->addSuitExtraAttr();
	
	const std::vector<stRuneItem>& vecRuneItems = this->role_rune.getRuneItems();
	for (auto itr=vecRuneItems.begin(); itr!=vecRuneItems.end(); itr++)
	{
		const stRuneItem& rune_item = *itr;
		if (rune_item.status == 0)
		{
			continue;
		}

		if (rune_item.runeid == 0)
		{
			continue;
		}

		CONFIG::RUNE_CFG_ITEM* pCfg = CONFIG::getRuneCfg(rune_item.runeid, rune_item.level);
		if (NULL == pCfg)
		{
			continue;
		}

		this->roleAttr[ E_ATTR_STAMINA	    ] += pCfg->stamina	   ;
		this->roleAttr[ E_ATTR_STRENGTH	    ] += pCfg->strength	   ;
		this->roleAttr[ E_ATTR_AGILITY	    ] += pCfg->agility	   ;
		this->roleAttr[ E_ATTR_INTELLECT    ] += pCfg->intellect	;
		this->roleAttr[ E_ATTR_SPIRIT		] += pCfg->spirit		;
		this->roleAttr[ E_ATTR_HP_MAX2		] += pCfg->hp_max		;
		this->roleAttr[ E_ATTR_ATK_MAX2	    ] += pCfg->atk_max	   ;
		this->roleAttr[ E_ATTR_DEF_MAX	    ] += pCfg->def_max	   ;
		this->roleAttr[ E_ATTR_SPELLATK_MAX ] += pCfg->spellatk_max;
		this->roleAttr[ E_ATTR_SPELLDEF_MAX ] += pCfg->spelldef_max;
		this->roleAttr[ E_ATTR_MP_MAX2		] += pCfg->mp_max		;
		this->roleAttr[ E_ATTR_HIT			] += pCfg->hit		   ;
		this->roleAttr[ E_ATTR_DODGE		] += pCfg->dodge		;
		this->roleAttr[ E_ATTR_CRIT		    ] += pCfg->crit		   ;
		this->roleAttr[ E_ATTR_CRIT_RES	    ] += pCfg->crit_res	   ;
		this->roleAttr[ E_ATTR_CRIT_INC	    ] += pCfg->crit_inc	   ;
		this->roleAttr[ E_ATTR_CRIT_RED	    ] += pCfg->crit_red	   ;
	}

	this->calRoleFightValue();
}

void ROLE::patch()
{
	this->byCurLevelAwardStep = this->byCurLevelAwardStep == 0 ? 1 : this->byCurLevelAwardStep;
}

void ROLE::send_battle_report(byte type, byte result, uint16 delay_time, uint coin, uint ingot, uint exp, vector<DROP_SEQ>& drop_items)
{
	output_stream out(S_FIGHT_END_REPORT);
	out.write_byte((byte)type);
	out.write_byte((byte)result);
	out.write_ushort(delay_time);
	out.write_uint(coin);
	out.write_uint(ingot);
	out.write_uint(exp);
	out.write_byte(drop_items.size());
	for (auto it : drop_items)
	{
		out.write_data(&it, sizeof(DROP_SEQ));
	}

#ifdef WIN32
	cout << dwRoleID << " battle over. result " << (int)battle_result << endl;
#endif
	send(out);
}

void ROLE::addSuitExtraAttr()
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
							this->roleAttr[ addattr.attr ] += DWORD( addattr.value );
						}

						break;//�ҵ����ȼ�����
					}
				}
				this->vecSuit.push_back( x.ID );
			}
		} 
		);
	}
}

void ROLE::tranformBaseAttr( const BYTE& type, const DWORD& value )
{
	if ( type < E_ATTR_STAMINA || type > E_ATTR_SPIRIT )//��֤ һ������  ��������
	{
		return ;
	}
	
	CONFIG::ATTRDEFORM_CFG* attrTransCfg = CONFIG::getAttrDeformCfg(type);
	if ( attrTransCfg == NULL )
	{
		return;
	}
	
	for ( auto& it : attrTransCfg->vecAttrDeform )
	{
		this->roleAttr[ it.attr ] += DWORD( it.ratio * value ) ;
	}
}

static tuple<char*, unsigned long> compressRoleblob(ROLE_BLOB* roleBlob)
{
	//static char buf[ sizeof(ROLE_BLOB) ];
	static char *buf = new char[ ESTIMATE_COMPRESS_BOUND(sizeof(ROLE_BLOB)) ];

	unsigned long dstSize = BASE::dataCompress( (const char* )roleBlob, sizeof(ROLE_BLOB), buf );

	return make_tuple(buf, dstSize);
}

void ROLE::roleSave2DB()
{
	ROLE_BLOB roleBlob;

	cache2Blob( roleBlob );

	MYSQL_ESCAPE_CPP( role_imei, this->phone_imei );

	char*compressed;
	unsigned long comLen;
	tie( compressed, comLen ) = compressRoleblob(&roleBlob);

	MYSQL_ESCAPE_ALLOCA_N(roleblob, compressed, comLen );

	EXEC_SQL( "update " TABLE_ROLE_INFO  " set level=%u, viplevel=%u, exp=%u, hp=%u, money=%u, gold=%u, charge=%u, mapid=%u,cur_sceneid=%u, "
		"fromid=%u, lastaccess=from_unixtime(%u), inc_hp_time=from_unixtime(%u), roleblob='%s', imei='%s'  where id=%u ", 
		this->wLevel, this->wVipLevel, this->dwExp, this->dwHP, this->dwCoin, this->dwIngot, this->dwCharge, this->wLastFightMapID, 0,
		0, this->tLastAccess, this->tIncHPTime, roleblob, role_imei, this->dwRoleID);
}

#define ASSIGN_CACHE_2_BLOB( field )	\
	roleBlob.field = this->field

#define ASSIGN_BLOB_2_CACHE( field )	\
	this->field = roleBlob.field


void ROLE::cache2Blob(ROLE_BLOB& roleBlob )
{
	ASSIGN_CACHE_2_BLOB(wLastFightMapID);
	ASSIGN_CACHE_2_BLOB(wCanFightMaxMapID);
	ASSIGN_CACHE_2_BLOB(wWinMaxMapID);
	ASSIGN_CACHE_2_BLOB(byTitleId);
	ASSIGN_CACHE_2_BLOB(tLogoutTime);
	ASSIGN_CACHE_2_BLOB(dwAutoSellOption);
	ASSIGN_CACHE_2_BLOB(dwMiningRemainTime);
	ASSIGN_CACHE_2_BLOB(byMiningSlotIndex);
	ASSIGN_CACHE_2_BLOB(byPredictIndex);
	memcpy( roleBlob.predictEarning, this->predictEarning, sizeof(this->predictEarning) );
	ASSIGN_CACHE_2_BLOB(wTmpVipLevel);
	ASSIGN_CACHE_2_BLOB(dwTmpVipEndTime);
	ASSIGN_CACHE_2_BLOB(dwSmeltValue);
	ASSIGN_CACHE_2_BLOB(byForgeRreshTimes);
	ASSIGN_CACHE_2_BLOB(byUseMoneyTreeTimes);
	ASSIGN_CACHE_2_BLOB(last_guild_signin);
	ASSIGN_CACHE_2_BLOB(meet_pk_max_combo_count);
	ASSIGN_CACHE_2_BLOB(meet_pk_combo_count);
	ASSIGN_CACHE_2_BLOB(exit_guild_time);
	ASSIGN_CACHE_2_BLOB(bodyguard_loot_count);
	ASSIGN_CACHE_2_BLOB(bodyguard_buy_loot_count);
	
	roleBlob.byNewMapKillMonNum = this->byNewMapKillMonNum;

	//// �Ѿ�������װ������
	//roleBlob.bySmeltEquipCount = this->bySmeltEquipCount; 
	//// �Ѿ�������Ʒ��ֵ
	//roleBlob.dwQualityValue = this->dwQualityValue;

	m_packmgr.cache2Blob(roleBlob);
	role_rune.cache2Blob(roleBlob);

	if ( this->itemForge )	//������浱ǰװ��
	{
		roleBlob.itemDataForge.initFromItemCache( this->itemForge );
	}

	//����
	for ( int i=0; i<BODY_GRID_NUMS; i++)
	{
		if ( this->vBodyEquip[i] == NULL )
			continue;

		if ( this->vBodyEquip[i]->itemCfg == NULL )
		{
			this->vBodyEquip[i] = NULL;
			continue;
		}

		if (!IS_EQUIP(this->vBodyEquip[i]->itemCfg))
		{
			continue;
		}
		roleBlob.arrBody[i].initFromItemCache( this->vBodyEquip[i] );

	}

	
	//����
	ASSIGN_CACHE_2_BLOB(wOnUpSkill);

	{
		for (DWORD i=0; i<this->role_skill.vecIndexSkills.size(); i++)
		{
			roleBlob.wFightSkill[i] = this->role_skill.vecIndexSkills[i].skillid;
		}
	}

	{
		size_t index = 0;
		for (auto it : this->role_skill.setSkills )
		{
			if (index >= MAX_SKILL_NUMS)
			{
				break;
			}
			roleBlob.arrSkill[index] = it;
			index++;
		}
	}

	//�ڿ�
	memcpy( roleBlob.vMiner, this->vMiner, sizeof(roleBlob.vMiner) );

	//����
	{
		
		for (size_t i=0; i<this->vecFriends.size(); i++)
		{
			BREAK_COND( i == MAX_FRIENDS_NUMS );
			roleBlob.arrFriendsID[i] = this->vecFriends[i];
		}
	}


	//��������¼
	{
		size_t index = 0;
		for (auto record : this->listRenaRecord)
		{
			if (index >= MAX_ARENA_RECORD)
			{
				break;
			}
			roleBlob.arrArenaRecord[index] = record;
			++index;
		}
	}

	//����
	{
		for (size_t i=0; i < this->vecDianZhanIDs.size(); i++)
		{
			BREAK_COND( i == MAX_DIANZHAN_NUMS );
			roleBlob.arrDianZhanIDs[i] = this->vecDianZhanIDs[i];
		}
	}

	//�����̳�
	{
		memcpy( roleBlob.m_vMysInfo, this->cMarMgr.cMarMys.m_vMysInfo, sizeof(roleBlob.m_vMysInfo) );
	}


	//�䱦�̳�
	{
		size_t index = 0;
		for ( auto &it : this->cMarMgr.cMarJew.m_listShopItem )
		{
			BREAK_COND( index == MAX_SHOP_ITEM );
			roleBlob.vShopItem[index] = it;
			++index;
		}
	}

	//��Ϸ����
	{
		size_t index = 0;
		for ( auto &it : this->mapTask )
		{
			BREAK_COND( index == MAX_TASK_NUMS );
			roleBlob.tasksInfo[index] = it.second;
			++index;
		}

	}	

	//ƽ̨����
	{
		size_t index = 0;
		for (auto &it : this->map_platform_task)
		{
			BREAK_COND(index == MAX_PLATFORM_TASK_NUMS);
			roleBlob.platform_tasks[index] = it.second;
			++index;
		}
	}

	ASSIGN_CACHE_2_BLOB(bySendFightBossTimes);
	ASSIGN_CACHE_2_BLOB(byDailyBuyBossTimes);
	ASSIGN_CACHE_2_BLOB(byAllBuyBossTimes);

	ASSIGN_CACHE_2_BLOB(bySendFightHeroTimes);
	ASSIGN_CACHE_2_BLOB(byDailyBuyHeroTimes);
	ASSIGN_CACHE_2_BLOB(byAllBuyHeroTimes);
	ASSIGN_CACHE_2_BLOB(byNewbieGuideStep);
	ASSIGN_CACHE_2_BLOB(dwFisrtLoginTime);
	ASSIGN_CACHE_2_BLOB(byCurLevelAwardStep);
	
	roleBlob.dwShopItemTimes = this->cMarMgr.cMarJew.getFreshTime();

	ASSIGN_CACHE_2_BLOB(byBuyQuickFightTimes);
	roleBlob.byFreshTimes = this->cMerMgr.getFreshTimes();
	ASSIGN_CACHE_2_BLOB(bySignTimes);
	ASSIGN_CACHE_2_BLOB(dwDaySignStatus);
	ASSIGN_CACHE_2_BLOB(byRechargeState);
	ASSIGN_CACHE_2_BLOB(close_user_valid_time);
	roleBlob.deny_chat = ( this->deny_chat == false ? 0 : 1 );															// ����
	ASSIGN_CACHE_2_BLOB(pkvalue);
	ASSIGN_CACHE_2_BLOB(pk_count);
	ASSIGN_CACHE_2_BLOB(pk_buy_count);
	ASSIGN_CACHE_2_BLOB(pk_dead_count);
	ASSIGN_CACHE_2_BLOB(last_pk_count_inc_time);
	ASSIGN_CACHE_2_BLOB(login_day_num);
	ASSIGN_CACHE_2_BLOB(meet_pk_protect_count);
	ASSIGN_CACHE_2_BLOB(shrine_times);
	
	// ż��
	size_t i = 0;
	for (auto& item : meet_pk_records)
	{
		roleBlob.meet_pk_records[i] = item;
		i++;
		if (i >= sizeof(roleBlob.meet_pk_records) / sizeof(meet_pk_record))
			break;
	}

	//Ӷ��
	{
		size_t index = 0;
		for (auto& it : this->cMerMgr.m_vecCMercenary )
		{
			if (index >= MERCENARY_NUM)
			{
				break;
			}
			roleBlob.arrMerData[index].initFromMerCache( &it );
			++index;
		}
	}

	//��Ӣ�ؿ�
	{
		size_t index = 0;
		for ( auto &it : this->mapElite )
		{
			if (index >= MAX_TASK_NUMS)
			{
				break;
			}
			roleBlob.m_vEliteGroupInfo[index] = it.second;

			++index;
		}

	}

	//�����
	{
		size_t index = 0;
		for ( auto &it : this->mapCompleteActivityIDs )
		{
			if (index >= MAX_TASK_NUMS)
			{
				break;
			}
			roleBlob.openActivityInfo[index] = it.second;
			++index;
		}
	}

	//�״γ�ֵ����
	{
		for (size_t i=0; i<this->vecFirstCharge.size(); i++)
		{
			BREAK_COND( i == MAX_CHARGE_TYPE );

			roleBlob.arrFirstCharge[i] = this->vecFirstCharge[i];
		}
	}

	//�һ�����ȡ����
	{
		for ( size_t i = 0; i < this->vecCodeAwardType.size(); i++ )
		{
			BREAK_COND( i == MAX_CODE_AWARD_NUM );

			roleBlob.arrCodeAwardTypes[i] = this->vecCodeAwardType[i];
		}
	}
	
	roleBlob.world_boss_battle_attack_index = role_worldboss.attack_index;

	//���ս������
	{
		size_t index = 0;
		for (auto it : vecTeamFightProcess)
		{
			if (index >= 100)
			{
				break;
			}
			BlobTeamProcess& fight = roleBlob.arrTeamFigthProcess[index];
			fight.time = it.time;

			size_t num = std::min<size_t>(3, it.vecFightEle.size());
			for (size_t i = 0; i < num; i++ )
			{
				memcpy(&fight.vFightResult[i], &it.vecFightEle[i], sizeof(fight.vFightResult[i]));
			}
			++index;
		}
	}

	//team fight record
	{
		size_t index = 0;
		for (auto record : this->listTeamFightRecord)
		{
			if (index >= 7)
			{
				break;
			}
			roleBlob.arrTeamFightREcord[index] = record;
			++index;
		}
	}

	//team IDs
	{
		size_t index = 0;
		for (auto id : this->vecTeamRoleIDs)
		{
			if (index >= 3)
			{
				break;
			}
			roleBlob.arrTeamIDs[index] = id;
			++index;
		}
	}

	vector<DWORD> vecTeamRoleIDs;//�����ж�Աid

}

bool ROLE::blob2Cache( const ROLE_BLOB& roleBlob )
{
	ROLE* role = this;

	ASSIGN_BLOB_2_CACHE(wLastFightMapID);
	service::player_enter_map(role, 0);
	ASSIGN_BLOB_2_CACHE(wCanFightMaxMapID);
	ASSIGN_BLOB_2_CACHE(wWinMaxMapID);
	
	ASSIGN_BLOB_2_CACHE(byTitleId);
	ASSIGN_BLOB_2_CACHE(tLogoutTime);

	ASSIGN_BLOB_2_CACHE(dwAutoSellOption);
	ASSIGN_BLOB_2_CACHE(dwMiningRemainTime);
	ASSIGN_BLOB_2_CACHE(byMiningSlotIndex);


	ASSIGN_BLOB_2_CACHE(byPredictIndex);
	memcpy( role->predictEarning, roleBlob.predictEarning, sizeof(roleBlob.predictEarning) );

	ASSIGN_BLOB_2_CACHE(wTmpVipLevel);
	ASSIGN_BLOB_2_CACHE(dwTmpVipEndTime);

	ASSIGN_BLOB_2_CACHE(dwSmeltValue);
	ASSIGN_BLOB_2_CACHE(byForgeRreshTimes);
	ASSIGN_BLOB_2_CACHE(byUseMoneyTreeTimes);
	ASSIGN_BLOB_2_CACHE(last_guild_signin);
	ASSIGN_BLOB_2_CACHE(meet_pk_max_combo_count);
	ASSIGN_BLOB_2_CACHE(meet_pk_combo_count);
	ASSIGN_BLOB_2_CACHE(exit_guild_time);
	ASSIGN_BLOB_2_CACHE(bodyguard_loot_count);
	ASSIGN_BLOB_2_CACHE(bodyguard_buy_loot_count);

	role->byNewMapKillMonNum = roleBlob.byNewMapKillMonNum;

	//��������װ��
	if ( roleBlob.itemDataForge.itemIndex != 0 )
	{
		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( roleBlob.itemDataForge.itemIndex );
		if ( itemCfg )
		{
			role->itemForge = new ITEM_CACHE( &roleBlob.itemDataForge );		
		}		
	}

	m_packmgr.blob2Cache(roleBlob);
	role_rune.blob2Cache(roleBlob);

	//����	
	for ( size_t i=0; i<BODY_GRID_NUMS; i++)
	{
		if ( roleBlob.arrBody[i].itemIndex == 0 )
			continue;

		CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( roleBlob.arrBody[i].itemIndex );
		if ( itemCfg == NULL )
			continue;

		int tgtBodyIndex = getBodyIndexBySubtype( role, itemCfg->sub_type );
		if ( tgtBodyIndex == -1 )
			continue;

		if (!IS_EQUIP(itemCfg))
		{
			continue;
		}
		role->vBodyEquip[tgtBodyIndex] = new ITEM_CACHE(roleBlob.arrBody+i );

	}

	//����
	ASSIGN_BLOB_2_CACHE(wOnUpSkill);
	memcpy(role->byFightSkill, roleBlob.wFightSkill, sizeof(role->byFightSkill));

	{
		for (DWORD i=0; i<4; i++)
		{
			role->role_skill.vecIndexSkills[i].skillid = roleBlob.wFightSkill[i];
		}
	}

	for (size_t i=0; i<MAX_SKILL_NUMS; i++)
	{
		if (roleBlob.arrSkill[i] == 0 || roleBlob.arrSkill[i] > SPECIAL_SKILL_MIN_ID )
		{
			break;
		}

		role->role_skill.setSkills.insert(roleBlob.arrSkill[i]);
	}

	//�ڿ�
	memcpy( role->vMiner, roleBlob.vMiner, sizeof(roleBlob.vMiner) );


	//����
	for (size_t i=0; i<MAX_FRIENDS_NUMS; i++)
	{
		if (roleBlob.arrFriendsID[i] == 0)
		{
			break;
		}
		role->vecFriends.push_back(roleBlob.arrFriendsID[i]);
	}

	//��������¼
	{
		for (size_t i=0; i<MAX_ARENA_RECORD; i++)
		{
			if (roleBlob.arrArenaRecord[i].byResult == 0)
			{
				break;
			}
			role->listRenaRecord.push_back(roleBlob.arrArenaRecord[i]);
		}
	}

	//����
	{
		for (size_t i=0; i < MAX_DIANZHAN_NUMS; i++)
		{
			if (roleBlob.arrDianZhanIDs[i] == 0)
			{
				break;
			}
			role->vecDianZhanIDs.push_back(roleBlob.arrDianZhanIDs[i]);
		}
	}

	//role->dwComID = roleBlob.dwComID;

	ASSIGN_BLOB_2_CACHE(bySendFightBossTimes);
	ASSIGN_BLOB_2_CACHE(byDailyBuyBossTimes);
	ASSIGN_BLOB_2_CACHE(byAllBuyBossTimes);

	ASSIGN_BLOB_2_CACHE(bySendFightHeroTimes);
	ASSIGN_BLOB_2_CACHE(byDailyBuyHeroTimes);
	ASSIGN_BLOB_2_CACHE(byAllBuyHeroTimes);
	ASSIGN_BLOB_2_CACHE(byNewbieGuideStep);
	ASSIGN_BLOB_2_CACHE(dwFisrtLoginTime);
	ASSIGN_BLOB_2_CACHE(byCurLevelAwardStep);

	role->cMarMgr.cMarJew.setFreshTime( roleBlob.dwShopItemTimes );
	ASSIGN_BLOB_2_CACHE(byBuyQuickFightTimes);
	role->cMerMgr.setFreshTimes(roleBlob.byFreshTimes);

	ASSIGN_BLOB_2_CACHE(bySignTimes);
	ASSIGN_BLOB_2_CACHE(dwDaySignStatus);
	ASSIGN_BLOB_2_CACHE(byRechargeState);
	ASSIGN_BLOB_2_CACHE(close_user_valid_time);
	role->deny_chat = ( roleBlob.deny_chat == 0 ? false : true );
	ASSIGN_BLOB_2_CACHE(pkvalue);
	ASSIGN_BLOB_2_CACHE(pk_count);
	ASSIGN_BLOB_2_CACHE(pk_buy_count);
	ASSIGN_BLOB_2_CACHE(pk_dead_count);
	ASSIGN_BLOB_2_CACHE(last_pk_count_inc_time);
	ASSIGN_BLOB_2_CACHE(login_day_num);
	ASSIGN_BLOB_2_CACHE(meet_pk_protect_count);
	ASSIGN_BLOB_2_CACHE(shrine_times);

	// ż��
	for(auto& item : roleBlob.meet_pk_records)
	{
		if(item.id > 0)
			meet_pk_records.push_back(item);
	}

	//�����̳�
	{
		memcpy( this->cMarMgr.cMarMys.m_vMysInfo, roleBlob.m_vMysInfo, sizeof(roleBlob.m_vMysInfo) );
	}
	//�̳�
	{
		for (size_t i=0; i < MAX_SHOP_ITEM; i++)
		{
			if (roleBlob.vShopItem[i].wID == 0)
			{
				break;
			}
			role->cMarMgr.cMarJew.m_listShopItem.push_back( roleBlob.vShopItem[i] );
		}
	}

	//����
	{
		for (size_t i=0; i < MAX_TASK_NUMS; i++)
		{
			if (roleBlob.tasksInfo[i].wTaskID == 0)
			{
				break;
			}
			role->mapTask.insert( make_pair( TASK::getGroupByID(roleBlob.tasksInfo[i].wTaskID), roleBlob.tasksInfo[i]));
			//logplan("loadtask:%u %u %u ",roleBlob.tasksInfo[i].wTaskID,roleBlob.tasksInfo[i].byTaskCompleteNums,roleBlob.tasksInfo[i].byTaskStatus);
		}
	}	

	//����
	{
		for (size_t i = 0; i < MAX_PLATFORM_TASK_NUMS; i++)
		{
			if (roleBlob.platform_tasks[i].wTaskID == 0)
			{
				break;
			}
			role->map_platform_task.insert(make_pair(roleBlob.platform_tasks[i].wTaskID, roleBlob.platform_tasks[i]));
		}
	}

	m_packmgr.patch();

	//Ӷ��
	{
		for (size_t i=0; i < MERCENARY_NUM; i++)
		{
			if (roleBlob.arrMerData[i].wID != 0)
			{
				role->cMerMgr.m_vecCMercenary.push_back( CMercenary(&roleBlob.arrMerData[i]) );
			}
		}
	}

	//��Ӣ�ؿ�
	{
		for (size_t i=0; i < MAX_ELITE_GROUP; i++)
		{
			WORD wEliteID = roleBlob.m_vEliteGroupInfo[i].wBase;
			if (wEliteID == 0)
			{
				break;
			}
			auto it = CONFIG::getMapCfg(wEliteID);

			COND_CONTINUE( it==NULL );

			role->mapElite.insert( make_pair( it->group_id, roleBlob.m_vEliteGroupInfo[i]));
		}
	}	

	//�����
	{
		for (size_t i=0; i < MAX_TASK_OPEN_ACTIVITY_NUMS; i++)
		{
			if (roleBlob.openActivityInfo[i].wTaskID == 0)
			{
				break;
			}
			role->mapCompleteActivityIDs.insert( make_pair( roleBlob.openActivityInfo[i].wTaskID, roleBlob.openActivityInfo[i]));
		}
	}	

	//�״γ�ֵ����
	{
		for (size_t i=0; i < MAX_CHARGE_TYPE; i++)
		{
			BREAK_COND( roleBlob.arrFirstCharge[i] == 0 );

			role->vecFirstCharge.push_back(roleBlob.arrFirstCharge[i]);
		}
	}

	//�һ�����ȡ
	{
		for (size_t i=0; i < MAX_CODE_AWARD_NUM; i++)
		{
			BREAK_COND( roleBlob.arrCodeAwardTypes[i] == 0 );

			role->vecCodeAwardType.push_back(roleBlob.arrCodeAwardTypes[i]);
		}
	}

	role_worldboss.attack_index = roleBlob.world_boss_battle_attack_index;

	//���ս������
	{
		for (size_t i = 0; i < 100; i++)
		{
			BREAK_COND(roleBlob.arrTeamFigthProcess[i].time == 0);
			DWORD fightTime = roleBlob.arrTeamFigthProcess[i].time;
			vector<TeamFightResult> vFight;
			for (size_t j = 0; j < 3; j++)
			{
				BREAK_COND(roleBlob.arrTeamFigthProcess[i].vFightResult[j].role == 0);
				vFight.push_back(roleBlob.arrTeamFigthProcess[i].vFightResult[j]);
			}

			role->vecTeamFightProcess.push_back(jiange_team::TeamFightProcess(fightTime, vFight));
		}
	}

	//team fight record
	{
		for (size_t i = 0; i < 7; i++)
		{
			if (roleBlob.arrTeamFightREcord[i].dwTime == 0)
			{
				break;
			}
			role->listTeamFightRecord.push_back(roleBlob.arrTeamFightREcord[i]);
		}
	}

	//team IDs
	{
		for (size_t i = 0; i < 3; i++)
		{
			if (roleBlob.arrTeamIDs[i] == 0)
			{
				break;
			}
			role->vecTeamRoleIDs.push_back(roleBlob.arrTeamIDs[i]);
		}
	}

	return true;
}

// ����
void ROLE::online()
{
	send_firstcharge_list();
	role_skill.autoUnlockSkills();
	role_rune.autoUnlockDivineGrids();
	//dwCanFightMapTime = 0;
	//dwComID = ARENA_NO_FIGHT;
	//meet_pk_target_roleid = ARENA_NO_FIGHT;
	//byArenaResult = 0;
	first_battle = true; // �����ж��Ƿ������ߺ��һս
	send_offline_report();
	login_broad();
	battle_info.reset();
	TASK::loginNotifyRoleTaskStatus(this);
}

// ����
void ROLE::offline()
{

}

// ����ʱ
void ROLE::onLevelUp()
{
	role_skill.autoUnlockSkills();
	role_rune.autoUnlockDivineGrids();

	if (faction)
	{
		auto member = faction->get_member(dwRoleID);
		if (member)
			member->level = wLevel;
	}
}

// �Ƿ����㹻�Ľ��
bool ROLE::hasEnoughCoin(DWORD need_coin)
{
	return this->dwCoin >= need_coin;
}

// ����ָ�������Ľ��
bool ROLE::subCoin(DWORD need_coin, bool notify)
{
	if (hasEnoughCoin(need_coin))
	{
		this->dwCoin -= need_coin;
		notifyCoin( this );
		return true;
	}
	
	return false;
}

// �Ƿ����㹻��Ԫ��
bool ROLE::hasEnoughGold(DWORD need_gold)
{
	return this->dwIngot >= need_gold;
}

// ����ָ��������Ԫ��
bool ROLE::subGold(DWORD need_gold, bool notify)
{
	if (hasEnoughGold(need_gold))
	{
		this->dwIngot -= need_gold;
		notifyIngot(this);
		return true;
	}

	return false;
}

// ��������ֵ
void ROLE::addSmeltValue(DWORD value)
{
	this->dwSmeltValue += value;

	notifyX( this, ROLE_PRO_SMELTVAR, dwSmeltValue );
}

// �Ƿ����㹻������ֵ
bool ROLE::hasEnoughSmeltValue(DWORD value)
{
	return this->dwSmeltValue >= value;
}

// �۳�����ֵ
bool ROLE::subSmeltValue(DWORD value)
{
	if (hasEnoughSmeltValue(value))
	{
		this->dwSmeltValue -= value;
		notifyX( this, ROLE_PRO_SMELTVAR, dwSmeltValue );
		return true;
	}

	return false;
}

////  ����Ʒ��ֵ
//void ROLE::addQualityValue(DWORD value)
//{
//	this->dwQualityValue += value;
//}

//// ����Ʒ��ֵ
//void ROLE::resetQualityValue()
//{
//	this->dwQualityValue = 0;
//}

//// ����������װ������
//void ROLE::addSmeltEquipCount(BYTE count)
//{
//	this->bySmeltEquipCount += count;
//}

//// ����������װ������
//void ROLE::resetSmeltEquipCount()
//{
//	this->bySmeltEquipCount = 0;
//}

WORD ROLE::getVipLevel()
{
	WORD max_level = std::max<WORD>(wVipLevel,wLoginVip);
	if (max_level >= wTmpVipLevel)
		return max_level;

	if ( (uint)time(nullptr) >= dwTmpVipEndTime )
		return max_level;

	return wTmpVipLevel;
}

WORD ROLE::getExchangeSuitLimit()
{
	//��vip ÿ��1�Ρ�
	//vip1-vip3 ÿ��2�Ρ�
	//vip4-vip5 ÿ��3�Ρ�
	//vip6-vip8 ,ÿ��4�Ρ�
	//vip8���ϣ�ÿ��6�Ρ�
	if (getVipLevel() == 0)
	{
		return 1;
	}
	else if (getVipLevel()>=1 && getVipLevel()<=3)
	{
		return 2;
	}
	else if (getVipLevel()>=4 && getVipLevel()<=5)
	{
		return 3;
	}
	else if (getVipLevel()>=6 && getVipLevel()<=8)
	{
		return 4;
	}
	else
	{
		return 5;
	}

	return 0;
}

static float attRatio[ E_ATTR_MAX ]={0.0f,0.0f,0.2f,0.0f,1.0f,2.0f,0.0f,2.0f,0.0f,0.0f,0.0f,5.0f,0.0f,2.0f,0.0f,5.0f,0.0f,0.0f,0.0f,0.3f,0.3f,5.0f,0.0f,0.0f,0.0f,0.0f,0.0f,2.5f,2.5f,1.0f,1.0f,0.0f,0.0f,0.0f,0.0f,50.0f,50.0f};

//ս����=0.2*�������ֵ+1ħ�����ֵ+2*��������+5*��������+2*���ܹ�������+2*���ܷ�������+5*����ֵ+5*����ֵ+2.5*����ֵ+2.5*����ֵ+1*�����˺�ֵ+1*���˼���ֵ+0.3*ħ���ָ�+0.3*�����ָ�

#define FIGHTATTR( attrtype )  (float)( roleAttr[attrtype] * attRatio[attrtype] )

void ROLE::calRoleFightValue( )
{
	float fValue = 0;
	fValue = FIGHTATTR(E_ATTR_HP_MAX2) + FIGHTATTR(E_ATTR_MP_MAX2) + FIGHTATTR(E_ATTR_ATK_MAX2) + FIGHTATTR(E_ATTR_ATK_MIN2) + FIGHTATTR(E_ATTR_SPELLATK_MAX) + FIGHTATTR(E_ATTR_SPELLATK_MIN)\
		+ FIGHTATTR(E_ATTR_SOL_MIN) + FIGHTATTR(E_ATTR_DODGE) + FIGHTATTR(E_ATTR_MP_REC2) + FIGHTATTR(E_ATTR_HP_REC2) + FIGHTATTR(E_ATTR_HIT)\
		+ FIGHTATTR(E_ATTR_CRIT) + FIGHTATTR(E_ATTR_CRIT_RES) + FIGHTATTR(E_ATTR_CRIT_INC) + FIGHTATTR(E_ATTR_CRIT_RED) + FIGHTATTR(E_ATTR_LUCK) + FIGHTATTR(E_LUCK_DEF)  ;

	this->dwFightValue = (DWORD)fValue;
	RANK::insertFightvalueRanks( this );
}

ROLE::ROLE()
	:role_skill(this)
	,m_packmgr(this)
	,role_rune(this)
	,role_worldboss(this)
	,cMerMgr(this)

{
	size_t offset = offsetof( class ROLE, vBodyEquip );
	//memset( ((char*)this) + offset,  0, sizeof(*this)-offset );

	memset(vBodyEquip, 0, sizeof(vBodyEquip));									//memset �ָ���
	memset(vecBossFightHelpers, 0, sizeof(vecBossFightHelpers));						//����bossս���б� ����
	faction = NULL;														//�����浽blob
	suitStrengthLevel= 0;													//����װ���ȼ���
	wLastFightMapID  = 0;													//�һ���ͼ
	wCanFightMaxMapID= 0;													//��ǰ���Դ�ĵ�ͼ(��Ŵ�������ĵ�ͼ�����ܴ�)
	wWinMaxMapID     = 0;														//ʤ������ͼ
	tLastSaveTime    = 0;													//������
	tLogoutTime      = 0;														//�˳�ʱ��
	byTitleId        = 0;
	client=NULL;
	wLevel      =0;															//�ȼ�
	dwFightValue=0;														//ս����
	byJob       =0;
	bySex       =0;
	wVipLevel   =0;															//VIP�ȼ�
	dwExp       =0;															//��ǰ����
	dwHP        =0;																//����
	dwCoin      =0;															//����(��Ϸ��)
	dwIngot     =0;															//Ԫ��(��ֵ��)
	dwCharge    =0;															//�ۼƳ�ֵ
	tLastAccess =0;														//�������ʱ��
	tIncHPTime  =0;														//����������ʱ��
	dwHonour    =0; 															//��ҫ
	byMailLoaded=0;
	byDirty     =0;
	dwRoleID    =0;
	memset(byFightSkill, 0, sizeof(byFightSkill));									//ս������������ָ��_skillId[10]��1-10λ�ã�0��ʾû��
	wOnUpSkill =0;														//������������id,0��ʾ��������
	memset( roleAttr, 0,sizeof(roleAttr) );												//0����,1-21
	byArenaDayCount   =0;  													//ÿ���Ѿ��μӾ�������������
	byBuyArenaDayCount=0;												//ÿ���Ѿ����򾺼�����������
	dwDianZhan        =0;														//�������ʱ��
	byPredictIndex    =0;
	memset(predictEarning, 0,sizeof(predictEarning) );
	lastKillMonAddExpTime = 0;											//���������
	prizeMailNums =0;
	msgMailNums   =0;
	systemMailNums=0;
	//dwComID       =0;															//����������id ��Ҫ����
	bySignTimes   =0;														//sign times per month
	dwDaySignStatus=0;													//day sign status bit
	byEnhanceTime =0;														//�bossս�������
	bySendFightBossTimes = 0;												//��սboss����ÿ������0������
	byDailyBuyBossTimes  = 0;												//������սboss����������
	byAllBuyBossTimes	   = 0;
	bySendFightHeroTimes = 0;												//��Ӣ�ؿ�
	byDailyBuyHeroTimes  = 0;												//������սhero����������
	byAllBuyHeroTimes    = 0;													
	byUseMoneyTreeTimes  = 0;												//moneytree����0������
	byBuyQuickFightTimes = 0;												//�������ս������
	dwAutoSellOption     = 0;
	dwMiningRemainTime   = 0;												//�����ڿ�ʣ��ʱ��
	byMiningSlotIndex    = 0;													//�����ڿ�Ŀ�λ�����λ�ú�vMiner��Ŀ�λ��ͬ��������Ŀ�λ
	byRes____            = 0;															//�л��󳡣���ʱˢ�����ģ��ڿ�Ŀ�λ��
	memset(vMiner, 0, sizeof(vMiner) );
	memset(vMinerTmpRefresh, 0,sizeof(vMinerTmpRefresh) );								//����л�ʱˢ�³�������ʱ�б�
	wCurMaxTaskID			=0;
	dwCanEnterFightMapTime=0;											//���Խ����ͼˢ��ʱ�䣬������
	dwCanFightMapTime		=0;												//���Խ����ͼ���ʱ�䣬������
	dwFinalWorldBossTime	=0;												//����������bossʱ��
	wTmpVipLevel			=0;
	dwTmpVipEndTime		=0;													//��ʱVIP ʧЧʱ��
	dwSmeltValue			=0;														//����ֵ
	byForgeRreshTimes		=0;													//��������Ѿ�ˢ�´���,����
	itemForge=NULL;													//������浱ǰװ��
	byNewMapKillMonNum = 0;												//�����µ�ͼͳ��ɱ�����������100
	//bySmeltEquipCount  = 0;													// �Ѿ�������װ������
	//dwQualityValue     = 0;													// �Ѿ�������Ʒ��ֵ
	byNewbieGuideStep  = 0;												//������������
	dwFisrtLoginTime   = 0;													//��ʼ��¼ʱ��
	close_user_valid_time = 0;										// �����Чʱ��
	deny_chat = false;															// ����
	pkvalue = 0;													// PKֵ
	pk_count = 0;													// PK����
	pk_buy_count = 0;													// PK����
	pk_dead_count = 0;													// PK��������
	//meet_pk_target_roleid = 0;															//����������id ��Ҫ����
	last_pk_count_inc_time = 0;						// ���pk��������ʱ��
	meet_pk_combo_count = 0;						// ż��PK������
	login_day_num = 0;
	meet_pk_protect_count = 3;
	last_pk_protect_inc_time = 0;

	//����ע���
	wLevel = 1;

	wCanFightMaxMapID = 1;
	wLastFightMapID = 1;

	auto now = time(nullptr);
	tLastSaveTime = now + 60;
	tLogoutTime = now+10;

	byCurLevelAwardStep = 1;
	byRechargeState = 0;
}



ROLE::~ROLE()
{
	CC_SAFE_DELETE(itemForge);

	//if ( itemForge != NULL)
	//{
	//	delete itemForge;
	//}
}




int ROLE::meet_fight_report(const uint target_role_id, const byte result)
{
	uint rewarded_coin = 0;
	vector<DROP_SEQ> drop_items;
	if (result != E_MATCH_STATUS_LOSS && result != E_MATCH_STATUS_WIN)
	{
		send_battle_report(E_FIGHT_MEET_PLAYER, result, 0, rewarded_coin, 0, 0, drop_items);
		return 0;
	}

	ROLE* target = RoleMgr::getMe().getRoleByID( target_role_id );
	if (target == nullptr)
	{
		send_battle_report(E_FIGHT_MEET_PLAYER, result, 0, rewarded_coin, 0, 0, drop_items);
		return 0;
	}

	int reward_pkvalue;
	int reward_pkvalue_t;

	auto mine_record = get_meet_pk_result(target_role_id, false);
	if(pk_count == 5)last_pk_count_inc_time = time(0);
	if(pk_count > 0)pk_count--;
	
	bool terminate_combo = false;
	meet_pk_protect_count++;
	if ( result == E_MATCH_STATUS_LOSS )//����
	{
		meet_pk_combo_count = 0;
		if (target->meet_pk_combo_count >= 5)
		{
			output_stream out(S_SYSTEM_BROADCAST);
			out.write_byte(e_broadcast_type::meet_pk_failed);// ͵Ϯʧ�ܹ㲥
			out.write_string(target->roleName);
			service::broadcast(out);
		}
		if(target->wLevel >= wLevel)
			reward_pkvalue = 1;
		else
			reward_pkvalue = (4 + (wLevel - target->wLevel) * 4) / 4;

		pk_dead_count++;

		// ����з��ؼ�¼��ı��¼Ϊ����ʧ��
		if(mine_record)
		{
			mine_record->result = meet_pk_result::revenge_lose;
		}
		else
		{
			meet_pk_record record;
			record.coin = 0;
			record.id = target_role_id;
			record.job = target->byJob;
			record.sex = target->bySex;
			strcpy(record.name.data(), target->roleName.c_str());
			record.time = time(0);
			record.reward_pkvalue = 0;
			record.result = meet_pk_result::attack_lose;
			meet_pk_records.push_back(record);
		}

		meet_pk_record record;
		record.coin = 0;
		record.id = dwRoleID;
		record.job = byJob;
		record.sex = bySex;
		strcpy(record.name.data(), roleName.c_str());
		record.time = time(0);
		record.reward_pkvalue = 0;
		record.result = meet_pk_result::defence_win;
		target->meet_pk_records.push_back(record);
	}
	else
	{
		if(wLevel >= target->wLevel)
			reward_pkvalue_t = 1;
		else
			reward_pkvalue_t = (4 + (target->wLevel - wLevel) * 4) / 4;

		if(target->wLevel >= wLevel)
			reward_pkvalue = 10 + (target->wLevel - wLevel) * 2;
		else
		{
			reward_pkvalue = 10 - (wLevel - target->wLevel);
			if(reward_pkvalue < 1)reward_pkvalue = 1;
		}

		// ��ɱ���ս�
		meet_pk_combo_count++;
		if (meet_pk_combo_count >= 5)
		{
			service::update_meet_combo_rank(this);
		}
		if (meet_pk_combo_count > meet_pk_max_combo_count)
		{
			meet_pk_max_combo_count = meet_pk_combo_count;
		}
		// ���ṱ��
		if (faction)
		{
			faction->contribute += 1;
			faction->exp += 1;
			auto member = faction->get_member(dwRoleID);
			if (member)
			{
				member->contribute += 1;
				faction->contribute_up(member);
			}
		}
		// ����
		auto target_dec_coin = unsigned(BASE::randBetween(0.01f, 0.05f) * target->dwCoin);
		unsigned reward_coin = target_dec_coin;
		if(target->meet_pk_combo_count >= 5)
		{
			reward_coin += target->meet_pk_combo_count * 10000;
			terminate_combo = true;
			output_stream out(S_SYSTEM_BROADCAST);
			out.write_byte(e_broadcast_type::meet_break_combo);// �ս���ɱ�㲥
			out.write_string(roleName);
			out.write_ushort(wLastFightMapID);
			out.write_string(target->roleName);
			out.write_ushort(target->meet_pk_combo_count);
			out.write_uint(reward_coin);
			service::broadcast(out);

			if (target->client == nullptr)
			{
				char szMsg[512];
				sprintf(szMsg, (CONFIG::get_meet_breakcombo()).c_str(), target->roleName.c_str(), target->meet_pk_combo_count);
				MailCtrl::sendMail(target->dwRoleID, E_MAIL_TYPE_PRIZE, szMsg, vector<ITEM_INFO>());
			}
		}
		target->meet_pk_combo_count = 0;
		if(target->meet_pk_protect_count > 0)target->meet_pk_protect_count--;
		if(target->pkvalue >= (unsigned)reward_pkvalue_t)
			target->pkvalue -= reward_pkvalue_t;
		else
			target->pkvalue = 0;

		if(target->dwCoin - target_dec_coin < 0)
			target->dwCoin = 0;
		else
			target->dwCoin -= target_dec_coin;
		logs_db_thread::singleton().consume(target->roleName, target->dwRoleID, -(int)target_dec_coin, 0, purpose::meet_battle_report, 0);
		if (reward_coin < 10000)reward_coin = 10000;
		if(reward_coin > 100000)
		{
			//drop_items.push_back(DROP_SEQ( 402,  1, 0, 2, 0));
			reward_item(drop_items, 402);

			if(BASE::randTo(100) < 30)
			{
				output_stream out(S_SYSTEM_BROADCAST);
				out.write_byte(e_broadcast_type::meet_pk_successful);
				out.write_string(roleName);
				out.write_ushort(wLastFightMapID);
				out.write_uint(reward_coin);
				service::broadcast(out);
			}
		}

		pkvalue += reward_pkvalue;
		ADDUP(dwCoin, reward_coin);
		rewarded_coin = reward_coin;
		logs_db_thread::singleton().consume(roleName, dwRoleID, reward_coin, 0, purpose::meet_battle_report, 0);

		service::update_meet_pk_rank(this);

		if(target->wLevel + 10 >= wLevel)
		{
			auto probability = 0.3f - (float)(wLevel - target->wLevel) * 0.02f;
			if(probability>0.4f)probability = 0.4f;//����߲�����0.4��
			auto rv = BASE::randTo(10000)/10000.f;
			if(rv < probability)
			{
				unsigned reward_item_id = 0;
				// 70%����Ϊ��ͭ���䣨��Ʒid=510����25%����Ϊ�������䣨��Ʒid=511����5%����Ϊ�ƽ��䣨��Ʒid=512��
				auto rv = BASE::randTo(10000)/10000.f;
				if(rv < 0.7f)
					reward_item_id = 510;
				else if(rv < 0.95f)
					reward_item_id = 511;
				else
					reward_item_id = 512;

				//drop_items.push_back(DROP_SEQ( reward_item_id,  1, 0, 2, 0));
				reward_item( drop_items, reward_item_id);
			}
			else
			{
				probability = 0.15f - (float)(wLevel - target->wLevel) * 0.01f;
				if(probability>0.2f)probability = 0.2f;// 0.15-������ȼ�-�Է��ȼ���*0.01����߲�����0.2��
				auto rv = BASE::randTo(10000)/10000.f;
				if(rv < probability)
				{
					unsigned reward_item_id = 0;
					// 70%����Ϊ��ͭԿ�ף���Ʒid=500����25%����Ϊ����Կ�ף���Ʒid=501����5%����Ϊ�ƽ�Կ�ף���Ʒid=502��
					auto rv = BASE::randTo(10000)/10000.f;
					if(rv < 0.7f)
						reward_item_id = 500;
					else if(rv < 0.95f)
						reward_item_id = 501;
					else
						reward_item_id = 502;

					//drop_items.push_back(DROP_SEQ(reward_item_id, 1, 0, 2, 0));
					reward_item(drop_items, reward_item_id);
				}
			}
		}
		
		// ����з��ؼ�¼��ı��¼Ϊ����ʧ��
		if(mine_record)
		{
			mine_record->result = meet_pk_result::revenge_win;
			mine_record->reward_pkvalue = reward_pkvalue;
			mine_record->coin = reward_coin;
		}
		else
		{
			meet_pk_record record;
			record.coin = reward_coin;
			record.id = target_role_id;
			record.job = target->byJob;
			record.sex = target->bySex;
			strcpy(record.name.data(), target->roleName.c_str());
			record.time = time(0);
			record.reward_pkvalue = reward_pkvalue;
			record.result = meet_pk_result::attack_win;
			meet_pk_records.push_back(record);
		}

		meet_pk_record record;
		record.coin = target_dec_coin;
		record.id = dwRoleID;
		record.job = byJob;
		record.sex = bySex;
		strcpy(record.name.data(), roleName.c_str());
		record.time = time(0);
		record.reward_pkvalue = -reward_pkvalue_t;
		record.result = meet_pk_result::defence_lose;
		target->meet_pk_records.push_back(record);

		// 
		if(target->client)
		{
			output_stream out(S_MEET_BE_KILLED);
			out.write_string(roleName);
			out.write_uint((unsigned)reward_coin);
			target->send(out);
		}
		if(meet_pk_combo_count >= 5 && !terminate_combo)
		{
			output_stream out(S_SYSTEM_BROADCAST);
			out.write_byte(e_broadcast_type::meet_pk_combo);
			out.write_ushort(meet_pk_combo_count);
			out.write_string(roleName);
			out.write_ushort(wLastFightMapID);
			service::broadcast(out);
		}
	}

	send_battle_report(E_FIGHT_MEET_PLAYER, result, 0, rewarded_coin, 0, 0, drop_items);
	return 0;
}


int ROLE::bodyguard_fight_report(const uint target_role_id, const byte result)
{
	ROLE* target = RoleMgr::getMe().getRoleByID(target_role_id);
	if (target == nullptr)
		return 0;
	
	return bodyguard::been_loot(target, this, result);
}



int ROLE::arena_fight_report(const uint target_role_id, const byte result)
{
	if (result != E_MATCH_STATUS_LOSS && result != E_MATCH_STATUS_WIN)
	{
		vector<DROP_SEQ> temp;
		send_battle_report(E_FIGHT_ARENA_PLAYER, result, 0, 0, 0, 0, temp);
		return 0;
	}

	ROLE* target = RoleMgr::getMe().getRoleByID(target_role_id);
	if (target == nullptr)
	{
		vector<DROP_SEQ> temp;
		send_battle_report(E_FIGHT_ARENA_PLAYER, result, 0, 0, 0, 0, temp);
		return 0;
	}

	uint dwRank = ARENA::getRankByRoleID(target_role_id);
	if (dwRank == 0)
	{
		vector<DROP_SEQ> temp;
		send_battle_report(E_FIGHT_ARENA_PLAYER, result, 0, 0, 0, 0, temp);
		return 0;
	}

	const CONFIG::ARENA_REWARD_CFG* cfg = CONFIG::getArenaRewardCfg(dwRank);
	if (cfg == nullptr)
	{
		vector<DROP_SEQ> temp;
		send_battle_report(E_FIGHT_ARENA_PLAYER, result, 0, 0, 0, 0, temp);
		return 0;
	}

	int nAddCoin = 0;
	int nAddGold = 0;
	bool bChange = false;

	byArenaDayCount++;//��������

	if (result == E_MATCH_STATUS_LOSS)//���˸�Ǯ
	{
		nAddCoin = cfg->lose_money;
		nAddGold = cfg->lose_gold;
	}
	else
	{
		ARENA::swapArenaRank(this, target, result);

		nAddCoin = cfg->win_money;
		nAddGold = cfg->win_gold;
		bChange = true;
	}
	ADDUP(dwCoin, nAddCoin);
	ADDUP(dwIngot, nAddGold);
	logs_db_thread::singleton().consume(roleName, dwRoleID, nAddCoin, nAddGold,
		purpose::arena_battle_report, 0);
	notifyIngot(this);
	notifyCoin(this);

	ARENA::toClienCompetitors(this, bChange);

	RoleMgr::getMe().judgeCompleteTypeTask(this, E_TASK_ARENA);

	vector<DROP_SEQ> temp;
	send_battle_report(E_FIGHT_ARENA_PLAYER, result, 0, nAddCoin, nAddGold, 0, temp);
	return 0;
}


int ROLE::monster_fight_report(byte result)
{
	if (result == E_MATCH_STATUS_LOSS)
	{
		uint16 battle_delay_time = 0; // ս���Ƴ�ʱ��
		auto now = (uint)time(0);
		if (now < dwCanFightMapTime)
			battle_delay_time = uint16(dwCanFightMapTime - now);
		vector<DROP_SEQ> temp;
		send_battle_report(E_FIGHT_MON, result, battle_delay_time, 0, 0, 0, temp);
		return 1;
	}

	byte ret = 0;

	ITEM_INFO para(wLastFightMapID, monster_battle_info.lstDropMonster.size());
	RoleMgr::getMe().judgeCompleteTypeTask(this, E_TASK_KILL_MON, &para);

	uint reward[3] = { 0 };
	list<ITEM_CACHE*> lstEquipNotify, lstPropsNotify, lstAutoSellItem, lstNoSellItem;
	ItemCtrl::killMapMonster(this, wLastFightMapID, monster_battle_info.byDropType, &lstEquipNotify, &lstPropsNotify, &lstAutoSellItem, &lstNoSellItem, reward);


	if (m_packmgr.lstEquipPkg.size() >= m_packmgr.curEquipPkgLimit)
	{
		ret = 6;// DROP_EQUIP_FULL;
	}

	if (m_packmgr.lstItemPkg.size() >= m_packmgr.curItemPkgLimit)
	{
		ret = 6;// DROP_PROPS_FULL;
	}

	vector<DROP_SEQ> drop_items;
	//1�Զ����� 2δ��
	for (auto& it : lstAutoSellItem)
	{
		if (it != NULL)
		{
			drop_items.push_back(DROP_SEQ(it->itemCfg->id, uint16(it->itemNums), it->byQuality, 1, uint16(it->itemNums * it->itemCfg->price)));
			CC_SAFE_DELETE(it);
		}

	}
	for (auto& it : lstNoSellItem)
	{
		if (it != NULL)
		{
			drop_items.push_back(DROP_SEQ(it->itemCfg->id, uint16(it->itemNums), it->byQuality, 2, uint16(it->itemNums * it->itemCfg->price)));
			CC_SAFE_DELETE(it);
		}
	}


	ItemCtrl::notifyClientItem(this, lstEquipNotify, PKG_TYPE_EQUIP);
	ItemCtrl::notifyClientItem(this, lstPropsNotify, PKG_TYPE_ITEM);

	uint16 battle_delay_time = 0; // ս���Ƴ�ʱ��
	auto now = (uint)time(0);
	if (now < dwCanFightMapTime)
		battle_delay_time = uint16(dwCanFightMapTime - now);

	send_battle_report(E_FIGHT_MON, result, battle_delay_time, reward[0], reward[1], reward[2], drop_items);
	return ret;
}


int ROLE::elite_fight_report(byte result)
{
	if (result == E_MATCH_STATUS_LOSS)
	{
		vector<DROP_SEQ> temp;
		send_battle_report(E_FIGHT_HERO, result, 0, 0, 0, 0, temp);
		return 1;
	}
	CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(battle_info.mapid);
	if (mapCfg == nullptr)
	{
		vector<DROP_SEQ> temp;
		send_battle_report(E_FIGHT_HERO, result, 0, 0, 0, 0, temp);
		return 1;// ENTER_MAP_NO_EXISTS;
	}

	byte ret = 0;

	if (IS_ELITE_MAP(battle_info.mapid))
	{
		auto it = mapElite.find(mapCfg->group_id);//����
		if (it != mapElite.end())
		{
			uint16& wCanFightMaxHeroID = it->second.wBase;

			uint16& wWinMaxHeroID = it->second.wExtra;

			CONFIG::MAP_CFG* nextMapCfg = CONFIG::getMapCfg(wCanFightMaxHeroID + 1);

			if (wCanFightMaxHeroID <= battle_info.mapid)
			{
				if (nextMapCfg != NULL && nextMapCfg->group_id == mapCfg->group_id)//����Ӣ�ؿ� 
				{
					wCanFightMaxHeroID++;
				}
			}

			if (wWinMaxHeroID <= battle_info.mapid)
			{
				wWinMaxHeroID = battle_info.mapid;
			}
		}

		if (bySendFightHeroTimes > 0)
		{
			bySendFightHeroTimes--;
		}
		else if (byAllBuyHeroTimes > 0)
		{
			byAllBuyHeroTimes--;
		}

		service::elitemap_done(this, battle_info.mapid, mapCfg->boss_id);

		RoleMgr::getMe().judgeCompleteTypeTask(this, E_TASK_FIGHT_ELITE_TIMES);
	}

	uint reward[3] = { 0 };
	list<ITEM_CACHE*> lstEquipNotify, lstPropsNotify, lstAutoSellItem, lstNoSellItem;
	ItemCtrl::killMapMonster(this, wLastFightMapID, E_DROP_HERO, &lstEquipNotify, &lstPropsNotify, &lstAutoSellItem, &lstNoSellItem, reward);


	if (m_packmgr.lstEquipPkg.size() >= m_packmgr.curEquipPkgLimit)
	{
		ret = 6;// DROP_EQUIP_FULL;
	}

	if (m_packmgr.lstItemPkg.size() >= m_packmgr.curItemPkgLimit)
	{
		ret = 6;// DROP_PROPS_FULL;
	}

	vector<DROP_SEQ> drop_items;
	//1�Զ����� 2δ��
	for (auto& it : lstAutoSellItem)
	{
		if (it != NULL)
		{
			drop_items.push_back(DROP_SEQ(it->itemCfg->id, uint16(it->itemNums), it->byQuality, 1, uint16(it->itemNums * it->itemCfg->price)));
			CC_SAFE_DELETE(it);
		}

	}
	for (auto& it : lstNoSellItem)
	{
		if (it != NULL)
		{
			drop_items.push_back(DROP_SEQ(it->itemCfg->id, uint16(it->itemNums), it->byQuality, 2, uint16(it->itemNums * it->itemCfg->price)));
			CC_SAFE_DELETE(it);
		}
	}

	ItemCtrl::notifyClientItem(this, lstEquipNotify, PKG_TYPE_EQUIP);
	ItemCtrl::notifyClientItem(this, lstPropsNotify, PKG_TYPE_ITEM);

	send_battle_report(E_FIGHT_HERO, result, 0, reward[0], reward[1], reward[2], drop_items);
	return ret;
}


int ROLE::boss_fight_report(byte result)
{
	if (result == E_MATCH_STATUS_LOSS)
	{
		vector<DROP_SEQ> temp;
		send_battle_report(E_FIGHT_HERO, result, 0, 0, 0, 0, temp);
		return 1;
	}
	byte ret = 0;

	if (wCanFightMaxMapID == battle_info.mapid)//����ͼ
	{
		if (CONFIG::getMapCfg(wCanFightMaxMapID + 1) != NULL)//����ͼ 
		{
			wCanFightMaxMapID++;
			notifyMaxMap(this);
			FightCtrl::clientEnterMap(this, wCanFightMaxMapID);
			BroadcastCtrl::mapOpen(this, wCanFightMaxMapID);
		}
	}

	if (wWinMaxMapID <= battle_info.mapid)
	{
		wWinMaxMapID = battle_info.mapid;
	}

	if ((battle_info.type != E_DROP_FIRST_BOSS) && (wLevel > 7))//�߻�Ҫ��<8����Ҫ������
	{
		if (bySendFightBossTimes > 0)
		{
			bySendFightBossTimes--;
		}
		else if (byAllBuyBossTimes > 0)
		{
			byAllBuyBossTimes--;
		}
	}

	RoleMgr::getMe().judgeCompleteTypeTask(this, E_TASK_FIGHT_BOSS_TIMES);

	//CONFIG::MAP_CFG* mapCfg = CONFIG::getMapCfg(battle_info.mapid);
	//if (mapCfg)
	{
		ITEM_INFO para(this->wWinMaxMapID, 0);
		RoleMgr::getMe().judgeCompleteTypeTask(this, E_TASK_FIGHT_BOSS, &para);
	}

	uint reward[3] = { 0 };
	list<ITEM_CACHE*> lstEquipNotify, lstPropsNotify, lstAutoSellItem, lstNoSellItem;
	ItemCtrl::killMapMonster(this, battle_info.mapid, E_DROP_BOSS, &lstEquipNotify, &lstPropsNotify, &lstAutoSellItem, &lstNoSellItem, reward);

	if (m_packmgr.lstEquipPkg.size() >= m_packmgr.curEquipPkgLimit)
	{
		ret = 6;// DROP_EQUIP_FULL;
	}

	if (m_packmgr.lstItemPkg.size() >= m_packmgr.curItemPkgLimit)
	{
		ret = 6;// DROP_PROPS_FULL;
	}

	vector<DROP_SEQ> drop_items;
	//1�Զ����� 2δ��
	for (auto& it : lstAutoSellItem)
	{
		if (it != NULL)
		{
			drop_items.push_back(DROP_SEQ(it->itemCfg->id, uint16(it->itemNums), it->byQuality, 1, uint16(it->itemNums * it->itemCfg->price)));
			CC_SAFE_DELETE(it);
		}

	}
	for (auto& it : lstNoSellItem)
	{
		if (it != NULL)
		{
			drop_items.push_back(DROP_SEQ(it->itemCfg->id, uint16(it->itemNums), it->byQuality, 2, uint16(it->itemNums * it->itemCfg->price)));
			CC_SAFE_DELETE(it);
		}
	}

	ItemCtrl::notifyClientItem(this, lstEquipNotify, PKG_TYPE_EQUIP);
	ItemCtrl::notifyClientItem(this, lstPropsNotify, PKG_TYPE_ITEM);

	send_battle_report(E_FIGHT_BOSS, result, 0, reward[0], reward[1], reward[2], drop_items);
	return ret;
}

// ��ȡż��PK��¼��attack Ϊ�Ƿ����󹥻����͵�
meet_pk_record* ROLE::get_meet_pk_result(unsigned roleid, bool attack)
{
	auto iter = std::find_if(meet_pk_records.begin(), 
		meet_pk_records.end(), [&](meet_pk_record& v){
			return v.id == roleid &&
				attack != (v.result == meet_pk_result::defence_lose ||
				v.result == meet_pk_result::defence_win);});
	if(iter!=meet_pk_records.end())
		return &*iter;
	else
		return nullptr;
}

void ROLE::update_meet_pk_count()
{
	const int inc_pk_count_time = 20 * 60;
	const int inc_pk_protect_count_time = 60 * 60;
	if(pk_count < 5)
	{
		auto cur = time(0);
		auto times = (cur - last_pk_count_inc_time) / inc_pk_count_time;
		if(times > 0)
		{
			pk_count += times;
			last_pk_count_inc_time += times * inc_pk_count_time;
			if(pk_count > 5)pk_count = 5;
		}
	}
	if(meet_pk_protect_count < 3)
	{
		auto cur = time(0);
		auto times = (cur - last_pk_protect_inc_time) / inc_pk_protect_count_time;
		if(times > 0)
		{
			meet_pk_protect_count += times;
			last_pk_protect_inc_time += times * inc_pk_protect_count_time;
			if(meet_pk_protect_count > 3)meet_pk_protect_count = 3;
		}
	}
}

void ROLE::equip_get_forging_info(input_stream& stream)
{
	enum result : uint16_t
	{
		successful,
		equip_not_exist,// װ��������
	};

	unsigned item;
	byte type;

	if (!stream.read_uint(item))return;
	if (!stream.read_byte(type))return;

	PKG_TYPE pkgType = m_packmgr.findItem((ITEM_CACHE*)item);
	if (PKG_TYPE_NONE == pkgType)
	{
		return;
	}

	output_stream out(S_PRECIOUS_TRAIN_INFO);
	auto ic = (ITEM_CACHE*)item;
	switch (type)
	{
	case 1:
	{
		//int need400 = 1;
		//unsigned consume_coin = 30000;
		//if (0 > 0){ need400 *= 2; consume_coin *= 2; }
		//if (0 > 0){ need400 *= 2; consume_coin *= 2; }
		out.write_uint(1);
		out.write_uint(30000);
	}
		break;
	case 2:
	{
		unsigned up_need = (unsigned)pow((double)(ic->star_level + 1), 1.7);
		out.write_uint(ic->exp);
		out.write_uint(up_need);
		out.write_uint(10000);
		break;
	}
	case 3:
	{
		auto consume_coin = (unsigned)(pow(ic->itemCfg->level_limit / 5.0, 2.0) * 20000.0);// 20000*(װ��X�ȼ�/5)^2
		unsigned need400 = unsigned(ic->itemCfg->level_limit / 2) + 1;//int��װ��X�ȼ� / 2�� + 1
		out.write_uint(need400);
		out.write_uint(consume_coin);
		break;
	}
	}
	send(out);
}

void ROLE::equip_forging(input_stream& stream)
{
	enum result : uint16_t
	{
		successful,
		insufficient_fragments,// ��Ƭ����
		insufficient_coin,// ��Ҳ���
		equip_not_exist,// װ��������
	};

	unsigned itemid;
	byte locked_item1 = 0, locked_item2 = 0;

	if (!stream.read_uint(itemid))return;
	stream.read_byte(locked_item1);
	stream.read_byte(locked_item2);

	output_stream out(S_PRECIOUS_FORGE);
	PKG_TYPE pkgType = m_packmgr.findItem((ITEM_CACHE*)itemid);
	if (PKG_TYPE_NONE == pkgType)
	{
		out.write_ushort(result::equip_not_exist);
		send(out);
		return;
	}

	unsigned need400 = 1;
	unsigned consume_coin = 30000;
	if (locked_item1 > 0){need400 *= 2;consume_coin *=2;}
	if (locked_item2 > 0){need400 *= 2;consume_coin *=2;}
	
	if (m_packmgr.getItemCount(400) < need400)
	{
		out.write_ushort(result::insufficient_fragments);
		send(out);
		return;
	}
	if (dwCoin < consume_coin)
	{
		out.write_ushort(result::insufficient_coin);
		send(out);
		return;
	}

	if (!m_packmgr.consumeItem(400, need400))
	{
		out.write_ushort(result::insufficient_fragments);
		send(out);
		return;
	}
	auto ic = (ITEM_CACHE*)itemid;
	ADDUP(dwCoin, -(int)consume_coin);
	logs_db_thread::singleton().consume(roleName, dwRoleID, -(int)consume_coin, 0, purpose::equip_forging, ic->itemCfg->id);

	ic->gen_halidome_attr(locked_item1, locked_item2);

	ItemCtrl::notifyItem(this, ic, pkgType);
	out.write_ushort(result::successful);
	for (auto& item : ic->equip_extends)
	{
		out.write_byte(item.type);
		out.write_float(item.value);
	}
	send(out);

	if (pkgType == PKG_TYPE_BODY)
	{
		ItemCtrl::updateNotifyRoleAttr(this);
	}
	else if (pkgType == PKG_TYPE_SLAVE)
	{
		this->cMerMgr.updateNotifyMgrMerAttr(cMerMgr.get_current());
	}
}

void ROLE::equip_star_up(input_stream& stream)
{
	enum result : uint16_t
	{
		successful,
		level_full,// ������
		insufficient_coin, // ��Ҳ���
		equip_not_exist,// װ��������
		material_not_exist,// ���ϲ�����
		not_self,// �������Լ�
	};

	unsigned itemid;
	list<ITEM_CACHE*> consume_items;

	if (!stream.read_uint(itemid))return;

	output_stream out(S_PRECIOUS_UP_STAR);

	PKG_TYPE type = m_packmgr.findItem((ITEM_CACHE*)itemid);
	if (PKG_TYPE_EQUIP != type && PKG_TYPE_BODY != type && PKG_TYPE_SLAVE != type)
	{
		out.write_ushort(result::equip_not_exist);
		send(out);
		return;
	}
	auto consume_coin = 0u;
	for (int i = 0; i < 5; i++)// �����ҵ�ָ���ĵ��ߣ��������Ľ��
	{
		uint temp;
		if (!stream.read_uint(temp))break;
		if (temp == 0)continue;
		PKG_TYPE pkgType = m_packmgr.findItem((ITEM_CACHE*)temp);
		if (PKG_TYPE_EQUIP != pkgType)
		{
			out.write_ushort(result::material_not_exist);
			send(out);
			return;
		}
		if (temp == itemid)
		{
			out.write_ushort(result::not_self);
			send(out);
			return;
		}
		auto item = (ITEM_CACHE*)temp;
		consume_coin += unsigned((item->itemCfg->level_limit) * 10000);
		consume_items.push_back(item);
	}
	auto ic = (ITEM_CACHE*)itemid;
	if (dwCoin < consume_coin)
	{
		out.write_ushort(result::insufficient_coin);
		send(out);
		return;
	}
	if (ic->star_level >= 10)
	{
		out.write_ushort(result::level_full);
		send(out);
		return;
	}
	ADDUP(dwCoin, -(int)consume_coin);
	logs_db_thread::singleton().consume(roleName, dwRoleID, -(int)consume_coin, 0, purpose::equip_star_up, ic->itemCfg->id);
	notifyCoin(this);

	unsigned addexp = 0;
	vector<ITEM_INFO> vecItemInfos;
	for (auto& item : consume_items)
	{
		//��ǿ����װ��������ǿ�������ĵ�70%��ǿ��ʯ
		auto stone_count = item->getOutputStrengthenStoneCount();
		ItemCtrl::makeOneTypeItem(this, CONFIG::getItemCfg(402), stone_count);

		//�����Ƕ�ı�ʯ
		auto gems = item->getAllInjectGems();
		for (auto gemid : gems)
		{
			ITEM_INFO info;
			info.itemIndex = gemid;
			info.itemNums = 1;
			vecItemInfos.push_back(info);
		}

		if (item->star_level)
			addexp += (unsigned)pow((double)(item->star_level + 1), 1.7) + 1;
		else
			addexp += 1;
		ItemCtrl::subItemNotify(this, m_packmgr.lstEquipPkg, PKG_TYPE_EQUIP, item, 1);
	}
	if (!vecItemInfos.empty())MailCtrl::makeMailItems(this, vecItemInfos);// ����������ʯ

	unsigned up_need = (unsigned)pow((double)(ic->star_level + 1), 1.7);
	ic->exp += addexp;
	while (ic->exp >= up_need)
	{
		ic->exp = ic->exp - up_need;
		ic->star_level++;
		if (ic->star_level >= 10)
			break;
		up_need = (unsigned)pow((double)(ic->star_level + 1), 1.7);
	}

	ItemCtrl::notifyItem(this, ic, type);
	out.write_byte(result::successful);
	out.write_byte(ic->star_level);
	out.write_uint(ic->exp);
	out.write_uint(up_need);
	send(out);

	if (type == PKG_TYPE_BODY)
	{
		ItemCtrl::updateNotifyRoleAttr(this);
	}
	else if (type == PKG_TYPE_SLAVE)
	{
		this->cMerMgr.updateNotifyMgrMerAttr(cMerMgr.get_current());
	}
}

void ROLE::equip_inherit(input_stream& stream)
{
	enum result : uint16_t
	{
		successful,// �ɹ�
		insufficient_coin, // ��Ҳ���
		insufficient_fragments,// ��Ƭ����
		source_equip_not_exist,// װ��������
		target_equip_not_exist,// Ŀ�겻����
		not_halidome,// �����������ɴ���
		not_self,// ���ɴ��и��Լ�
		not_united_type,// ���Ͳ�ͳһ
	};

	unsigned sitem, titem;

	if (!stream.read_uint(sitem))return;
	if (!stream.read_uint(titem))return;

	output_stream out(S_PRECIOUS_TRANSLATE);

	if (sitem == titem)
	{
		out.write_ushort(result::not_self);
		send(out);
		return;
	}
	PKG_TYPE stype = m_packmgr.findItem((ITEM_CACHE*)sitem);
	if (PKG_TYPE_EQUIP != stype && PKG_TYPE_BODY != stype && PKG_TYPE_SLAVE != stype)
	{
		out.write_ushort(result::source_equip_not_exist);
		send(out);
		return;
	}
	auto sic = (ITEM_CACHE*)sitem;
	if (!sic->is_halidome())
	{
		out.write_ushort(result::not_halidome);
		send(out);
		return;
	}
	auto ttype = m_packmgr.findItem((ITEM_CACHE*)titem);
	if (PKG_TYPE_EQUIP != ttype && PKG_TYPE_BODY != ttype && PKG_TYPE_SLAVE != ttype)
	{
		out.write_ushort(result::target_equip_not_exist);
		send(out);
		return;
	}
	auto tic = (ITEM_CACHE*)titem;
	if (!tic->is_halidome())
	{
		out.write_ushort(result::not_halidome);
		send(out);
		return;
	}

	auto consume_coin = (unsigned)pow(sic->itemCfg->level_limit / 5.0, 2.0) * 20000.0;// 20000*(װ��X�ȼ�/5)^2
	if (dwCoin < consume_coin)
	{
		out.write_ushort(result::insufficient_coin);
		send(out);
		return;
	}
	unsigned need400 = unsigned(sic->itemCfg->level_limit / 2) + 1;//int��װ��X�ȼ� / 2�� + 1
	if (!m_packmgr.consumeItem(400, need400))
	{
		out.write_ushort(result::insufficient_fragments);
		send(out);
		return;
	}
	if (sic->itemCfg->type != tic->itemCfg->type)
	{
		out.write_ushort(result::not_united_type);
		send(out);
		return;
	}

	ADDUP(dwCoin, -(int)consume_coin);
	logs_db_thread::singleton().consume(roleName, dwRoleID, -(int)consume_coin, 0, purpose::equip_star_up, sic->itemCfg->id);
	tic->equip_extends = sic->equip_extends;
	tic->star_level = sic->star_level;
	sic->equip_extends.clear();
	sic->star_level = 0;
	sic->gen_halidome_attr();

	// �ͻ�����Ҫ�ȷ��͵��߸ı�֪ͨ
	ItemCtrl::notifyItem(this, sic, stype);
	ItemCtrl::notifyItem(this, tic, ttype);

	out.write_ushort(result::successful);
	for (auto& item : sic->equip_extends)
	{
		out.write_byte(item.type);
		out.write_uint(*(unsigned*)(void*)&item.value);
	}
	send(out);

	if (stype == PKG_TYPE_BODY || ttype == PKG_TYPE_BODY)
	{
		ItemCtrl::updateNotifyRoleAttr(this);
	}
	if (stype == PKG_TYPE_SLAVE || ttype == PKG_TYPE_SLAVE)
	{
		this->cMerMgr.updateNotifyMgrMerAttr(cMerMgr.get_current());
	}
}


void ROLE::guild_get_contribute_info(input_stream& stream)
{
	enum result : byte
	{
		successful,// �ɹ�
		not_guild,// �޹���
	};

	output_stream out(S_GUILD_GET_CONTRIBUTE);

	if (faction == nullptr)
	{
		out.write_byte(result::not_guild);
		send(out);
		return;
	}

	auto fm = faction->get_member(dwRoleID);
	if (fm == nullptr)
	{
		out.write_byte(result::not_guild);
		send(out);
		return;
	}

	auto cfg = guild::get_contribute_reward(fm->contribute_count);
	if (cfg == nullptr)
	{
		out.write_byte(result::not_guild);
		send(out);
		return;
	}

	out.write_byte(result::successful);
	out.write_uint(cfg->coin);
	out.write_uint(cfg->gold);
	out.write_uint(cfg->guild_exp);
	out.write_uint(cfg->person_contribute);
	send(out);
}

int ROLE::get_wordship_info(unsigned char* data_ptr, size_t data_len)
{
	//input_stream stream(data_ptr, data_len);
	RETURN_COND(this->faction == nullptr, 0);
	this->faction->get_wordship_info(this);
	return 0;
}
int ROLE::guild_wordship(unsigned char* data_ptr, size_t data_len)
{
	//input_stream stream(data_ptr, data_len);
	RETURN_COND(this->faction == nullptr, 0);
	this->faction->shrine_wordship(this);
	return 0;
}

void ROLE::guild_contribute(input_stream& stream)
{
	byte type;
	if (!stream.read_byte(type))return;

	if (faction == nullptr)
	{
		return;
	}

	faction->do_contribute(this, type);
}



//�����л�����  ����������ľ��ǳ�������
void ROLE::guild_join_guild(input_stream& stream)
{
	enum result : byte
	{
		successful,// �ɹ�
		not_guild,// ���᲻����
		not_enough_fight_value,// ս������
		has_guild,// �Ѿ����빫��
		full,// ��������
		time_limit,// ʱ������
	};
	output_stream out(S_GUILD_add);

	if (faction)
	{
		out.write_byte(result::has_guild);
		send(out);
		return;
	}

	unsigned guildid;
	if (!stream.read_uint(guildid))return;

	auto guild = guild::get_guild(guildid);
	if (guild == nullptr)
	{
		out.write_byte(result::not_guild);
		send(out);
		return;
	}

	if (guild->full())
	{
		out.write_byte(result::full);
		send(out);
		return;
	}

	if (dwFightValue < guild->join_condition)
	{
		out.write_byte(result::not_enough_fight_value);
		out.write_uint(guild->join_condition);
		send(out);
		return;
	}

	// 6Сʱ�ڽ�ֹ���빫��
	auto interval = time(nullptr) - exit_guild_time;
	if (exit_guild_time != 0 && interval < 3600 * 6)
	{
		out.write_byte(result::time_limit);
		out.write_uint((3600 * 6 - interval) / 60);
		send(out);
		return;
	}
	exit_guild_time = 0;

	guild->add_member(this);
	faction = guild;

	out.write_byte(result::successful);
	out.write_uint(guildid);
	send(out);

	output_stream notify(S_UPDATE_ROLE_DATA);
	notify.write_byte(ROLE_PRO_FACTION);
	notify.write_uint(guild->id);
	notify.write_byte(ROLE_PRO_FACTION_JOB);
	notify.write_uint(E_MEMBER);
	send(notify);
}


void ROLE::guild_set_join_condition(input_stream& stream)
{
	enum result : byte
	{
		successful,// �ɹ�
		not_guild_leader,// ���ǻ᳤
		not_guild,// ���᲻����
	};
	output_stream out(S_GUILD_SET_CONDITION);

	if (faction == nullptr)
	{
		out.write_byte(result::not_guild);
		send(out);
		return;
	}

	unsigned condition;
	if (!stream.read_uint(condition))return;

	if (dwRoleID != faction->leader_roleid)
	{
		out.write_byte(result::not_guild_leader);
		send(out);
		return;
	}

	faction->join_condition = condition;
	out.write_byte(result::successful);
	send(out);
}


//������������
void ROLE::guild_find(input_stream& stream)
{
	string name_or_id;
	if (!stream.read_string(name_or_id))
		return;

	auto guild1 = guild::get_guild(name_or_id);
	auto guild2 = guild::get_guild(ATOUL_(name_or_id));

	output_stream out(S_GET_GUILD_find);
	if (guild1)
	{
		out.write_uint(guild1->id);
		out.write_string(guild1->name);
		out.write_byte((byte)(guild1->level + 1));
		out.write_ushort(guild1->get_member_count());
		out.write_ushort((uint16_t)guild1->get_max_member());
		out.write_string(guild1->leader_name);
	}

	if (guild2)
	{
		out.write_uint(guild2->id);
		out.write_string(guild2->name);
		out.write_byte((byte)(guild2->level + 1));
		out.write_ushort(guild2->get_member_count());
		out.write_ushort((uint16_t)guild2->get_max_member());
		out.write_string(guild2->leader_name);
	}

	send(out);
}


void ROLE::guild_signin(input_stream& stream)
{
	enum result : byte
	{
		successful,// �ɹ�
		not_guild,// ���᲻����
		has_signin,// �Ѿ�ǩ��
	};
	output_stream out(S_GUILD_SIGNIN);

	if (faction == nullptr)
	{
		out.write_byte(result::not_guild);
		send(out);
		return;
	}

	auto member = faction->get_member(dwRoleID);
	if (member == nullptr)
	{
		out.write_byte(result::not_guild);
		send(out);
		return;
	}

	auto cfg = guild::get_level_info(faction->level);
	if (cfg == nullptr)
	{
		logFATAL("have not level config of guild(level:%u)", faction->level);
		out.write_byte(result::not_guild);
		send(out);
		return;
	}

	auto now = time(nullptr);
	if (last_guild_signin > now - now % (24 * 3600))
	{
		out.write_byte(result::has_signin);
		send(out);
		return;
	}

	last_guild_signin = now;

	out.write_byte(result::successful);
	if (cfg->reward_gold > 0)
	{
		ADDUP(dwIngot, cfg->reward_gold);
		logs_db_thread::singleton().consume(roleName, dwRoleID, 0, cfg->reward_gold, purpose::guild_signin, 0);
		notifyIngot(this);
	}
	if (cfg->reward_coin > 0)
	{
		ADDUP(dwIngot, cfg->reward_coin);
		logs_db_thread::singleton().consume(roleName, dwRoleID, cfg->reward_coin, 0, purpose::guild_signin, 0);
		notifyCoin(this);
	}

	send(out);
}


void ROLE::send(output_stream& stream)
{
	if (client)
		client->send(stream.get_buffer(), stream.length());
}

void ROLE::reward_item(vector<DROP_SEQ>& drop_items, unsigned item_id, unsigned count )
{
	auto itemcfg = CONFIG::getItemCfg(item_id);
	if (itemcfg == nullptr)
		return;
	if (-1 == ItemCtrl::makeOneTypeItem(this, itemcfg, count, 1, true) )//1�Զ����� 2δ��  Ʒ�����Ʊ����ǵ���
	{
		drop_items.push_back(DROP_SEQ(itemcfg->id, (WORD)count, (BYTE)itemcfg->rare, 1, uint16(count * itemcfg->price)));
	}
	else
	{
		drop_items.push_back(DROP_SEQ(itemcfg->id, (WORD)count, (BYTE)itemcfg->rare, 2, uint16(count * itemcfg->price)));
	}
	

	//drop_items.push_back(DROP_SEQ(reward_item_id, 1, 0, 2, 0));
}

bool ROLE::reward_item(unsigned item_id, unsigned count)
{
	auto itemcfg = CONFIG::getItemCfg(item_id);
	if (itemcfg == nullptr)
		return false;
	ItemCtrl::makeOneTypeItem(this, itemcfg, count);
	//auto item = ITEM_CACHE(itemcfg, count);
	//output_stream stream(S_NOTIFY_NEW_ITEM);
	//stream.write_byte((byte)itemcfg->type);
	//stream.write_byte(1);
	//ItemBriefInfo brefInfo(&item);
	//stream.write_data((char*)&brefInfo, sizeof(ItemBriefInfo));
	//send(stream);
	return true;
}

void ROLE::battle_over(eFIGHTTYPE battle_type)
{
	if (battle_type == E_FIGHT_NONE)
		return;

	if (battle_type == E_FIGHT_ARENA_PLAYER)
	{
		arena_fight_report(battle_info.target_roleid, battle_info.result);
	}
	else if (battle_type == E_FIGHT_MEET_PLAYER)
	{
		meet_fight_report(battle_info.target_roleid, battle_info.result);
	}
	else if (battle_type == E_FIGHT_BODYGUARD)
	{
		bodyguard_fight_report(battle_info.target_roleid, battle_info.result);
	}
	else if (battle_type == E_FIGHT_MON)
	{
		monster_fight_report(battle_result);
		monster_battle_info.resetDrop();
		//FightCtrl::clientReportMap(this, dropSeqs, stFightDrop.wDropMap, stFightDrop.byDropType);
	}
	else if (battle_type == E_FIGHT_HERO)
	{
		elite_fight_report(battle_info.result);
	}
	else if (battle_type == E_FIGHT_BOSS)
	{
		boss_fight_report(battle_info.result);
	}

	if (battle_type != E_FIGHT_MON)// ��ͨս����Ӱ�������
	{
		battle_info.reset();
	}
}

void ROLE::battle_tip(input_stream& stream)
{
	//cout << last_battle_info.type << " type" << endl;

	output_stream out(S_FIGHT_TIP);
	out.write_byte(0);
	out.write_ushort(0);
	//auto now = (uint)time(nullptr);
	//byte type = 0;
	//if (now <= tLogoutTime + 120)
	//{
	//	type = cur_battle_info.type;
	//}
	//out.write_byte(type);
	//if (type == 0)
	//{
	//	auto now = (uint)time(nullptr);
	//	out.write_ushort((uint16)((dwCanFightMapTime != 0u && dwCanFightMapTime > now) ? dwCanFightMapTime - now : 0));
	//}
	send(out);

}


void make_chat(ROLE* role, BYTE channel, const string& strContent, output_stream& out)
{
	out.write_ushort(0);
	out.write_uint(role->dwRoleID);
	out.write_byte(channel);
	out.write_byte(role->byJob);
	out.write_byte(role->bySex);
	out.write_byte((byte)role->getVipLevel());
	out.write_byte(role->byTitleId);
	out.write_uint(role->faction ? role->faction->id : 0);
	out.write_string(role->roleName);
	if (role->faction == NULL)
		out.write_string("");
	else
		out.write_string(role->faction->name);
	out.write_string(role->roleZone);
	out.write_string(strContent);
}


int gmMakeItem(ROLE*role, WORD itemIndex, size_t itemNum, BYTE byQua = 1)
{
	//	WORD itemIndex = ATOW_( vecString[1]  );
	//	WORD itemNum = 1;

	CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(itemIndex);
	if (itemCfg == NULL)
		return 0;

	list<ITEM_CACHE*> lstItemCachePkg, lstItemCacheMine;

	//if ( IS_MINE(itemCfg ) )
	//{
	//	if ( E_SUCC == role->m_packmgr.makeItem( itemCfg, itemNum, &lstItemCacheMine) )
	//	{
	//		ItemCtrl::notifyClientItem(role, lstItemCacheMine, PKG_TYPE_MINE);
	//		return 0;		
	//	}
	//	return -1;
	//}

	if (IS_PROPS(itemCfg))
	{
		if (E_SUCC == role->m_packmgr.makeItem(itemCfg, itemNum, &lstItemCachePkg, false, byQua))
		{
			ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_ITEM);
			return 0;
		}
		return 0;
	}
	else if (IS_DIVINE(itemCfg))
	{
		if (E_SUCC == role->m_packmgr.makeItem(itemCfg, itemNum, &lstItemCachePkg))
		{
			ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_FUWEN);
			return 0;
		}
	}
	else if (IS_EQUIP(itemCfg))
	{
		if (E_SUCC == role->m_packmgr.makeItem(itemCfg, itemNum, &lstItemCachePkg, false, byQua))
		{
			ItemCtrl::notifyClientItem(role, lstItemCachePkg, PKG_TYPE_EQUIP);
			return 0;
		}
	}


	return -1;
}



int gm(ROLE* role, const string& strContent)
{
	vector<string> vecString;
	strtoken2Vec(strContent.c_str(), SPLIT_SPACE, vecString, NULL, NULL);

	if ( vecString.size() < 2 )
	{
		return 0;
	}

	if (!vecString.empty())
	{
		logonline("usegm:%d  %s", role->dwRoleID, vecString[0].c_str());
	}

	if (service::gm_no_need_name == 0)
	{
		if (vecString[0] == "$reloadgmnamecfg")
		{
			return CONFIG::readCfgGmName();
		}

		if (!CONFIG::isGmName(role->roleName))
		{
			return 0;
		}
	}

	if (vecString[0] == "$reload")
	{
		return CONFIG::reloadCfg(vecString[1]);

	}
	else if (vecString[0] == "$addgmname")
	{
		CONFIG::add_gm_name(vecString[1]);
	}
	else if (vecString[0] == "$item")
	{
		return gmMakeItem(role, ATOW_(vecString[1]), (vecString.size() >= 3 ? ATOUL_(vecString[2]) : 1), BYTE(vecString.size() >= 4 ? ATOUL_(vecString[3]) : 1));

	}
	else if (vecString[0] == "$clean")// ��������������
	{
		RoleMgr::getMe().timerDailyClean(role, time(nullptr), true);
	}
	else if (vecString[0] == "$signtimes")
	{
		role->bySignTimes = ATOB_(vecString[1]);
	}
	else if (vecString[0] == "$charge")
	{
		DWORD dwChargeNum = ATOUL_(vecString[1]);
		PROTOCAL::chargeRole( dwChargeNum, role, true);
	}
	else if (vecString[0] == "$level")//�ȼ�����
	{
		if ( ATOW_(vecString[1]) > 79 )
			return -1;

		role->wLevel = ATOW_(vecString[1]);
		RoleMgr::getMe().onLevelUP(role, role->wLevel);
		notifyLevel(role);
	}
	else if (vecString[0] == "$viplevel")
	{
		if ( ATOW_(vecString[1]) > 10)
			return -1;

		role->wVipLevel = ATOW_(vecString[1]);
		notifywVipLevel(role);
	}
	else if (vecString[0] == "$repute")
	{
		ADDUP(role->dwHonour, atoi(vecString[1].c_str()));
		notifyHonour(role);
	}
	else if (vecString[0] == "$smelt")
	{
		ADDUP(role->dwSmeltValue, atoi(vecString[1].c_str()));
		notifySmeltValue(role);
	}
	else if (vecString[0] == "$money" || vecString[0] == "$coin")//wm ��Ǯ
	{
		auto value = atoi(vecString[1].c_str());
		ADDUP(role->dwCoin, value);
		//logs_db_thread::singleton.consume(service::id, role->roleName, role->dwRoleID, value, 0, purpose::gm, 0);
		notifyCoin(role);
	}
	else if (vecString[0] == "$gold" || vecString[0] == "$ingot")//wm ��Ԫ��
	{
		auto value = atoi(vecString[1].c_str());
		ADDUP(role->dwIngot, value);
		//logs_db_thread::singleton.consume(service::id, role->roleName, role->dwRoleID, 0, value, purpose::gm, 0);
		notifyIngot(role);
	}
	else if (vecString[0] == "$openmap")//�������
	{
		auto mapid = ATOW_(vecString[1]);
		if ( mapid < 1 || mapid > 80 )
		{
			return 0;
		}
		role->wWinMaxMapID = mapid;
		role->wCanFightMaxMapID = ++mapid;
	}
	else if (vecString[0] == "$task")//���
	{
		role->mapTask.clear();
		TASK::autoUnlockTask(role);
	}
	else if (vecString[0] == "$platform_task")//���
	{
		role->map_platform_task.clear();
		TASK::init_tencent_tasks(role);
	}
	else if (vecString[0] == "$activity")//���
	{
		role->mapCompleteActivityIDs.clear();
		TASK::autoUnlockOpenActivitiyIDs(role);
	}
	else if (vecString[0] == "$mail")//$mail rolename mailtype content prize
	{
		if (( vecString[1] == "-h") || vecString.size() == 1)
		{
			output_stream out(S_GAME_CHAT);
			make_chat(role, 1, "$mail rolename mailtype content prize", out);
			role->send(out);
			return 0;
		}

		if (vecString.size() < 5)
			return -1;

		const string& roleName = vecString[1];
		const string& mailType = vecString[2];
		const string& content = vecString[3];
		const string& prize = vecString[4];

		gmSendMail(roleName, mailType, content, prize);
	}
	else if (vecString[0] == "$mail_all")//$mail rolename mailtype content prize
	{
		if (vecString.size() < 4)
			return -1;
		const string& mailType = vecString[1];
		const string& content = vecString[2];
		const string& prize = vecString[3];
		vector<ITEM_INFO> vecItemInfo;
		CONFIG::parseItemInfoStr(prize, vecItemInfo);

		MailCtrl::send_full_scale_mail(ATOB_(mailType), content, vecItemInfo);
	}
	else if (vecString[0] == "$other" || vecString[0] == "$otherid")
	{
		if (vecString.size() < 4)//�����ṩ�ĸ����ϲ���
		{
			return 0;
		}

		ROLE* roleOther = NULL;
		if (vecString[0] == "$other")
		{
			roleOther = RoleMgr::getMe().getRoleByName(vecString[1]);
		}
		else if (vecString[0] == "$otherid")
		{
			roleOther = RoleMgr::getMe().getRoleByID(ATOUL_(vecString[1]));
		}
		if (roleOther == NULL)
		{
			return 0;
		}

		if (vecString[2] == "item")
		{
			return gmMakeItem(roleOther, ATOW_(vecString[3]), (vecString.size() >= 5 ? ATOUL_(vecString[4]) : 1), BYTE(vecString.size() >= 6 ? ATOUL_(vecString[5]) : 1));
		}
		else if (vecString[2] == "clean")//��������������
		{
			RoleMgr::getMe().timerDailyClean(roleOther, time(nullptr), true);
		}
		else if (vecString[2] == "level")//�ȼ�����
		{
			if (ATOW_(vecString[3]) > 79)
				return -1;

			roleOther->wLevel = ATOW_(vecString[3]);
			RoleMgr::getMe().onLevelUP(roleOther, roleOther->wLevel);
		}
		else if (vecString[2] == "viplevel")
		{
			if (ATOW_(vecString[3]) > 15)
				return -1;

			roleOther->wVipLevel = ATOW_(vecString[3]);
		}
		else if (vecString[2] == "money" || vecString[2] == "coin")//��Ǯ
		{
			auto value = atoi(vecString[3].c_str());
			ADDUP(roleOther->dwCoin, value);
			//logs_db_thread::getMe().consume(service::id, role->roleName, role->dwRoleID, value, 0, purpose::gm, 0);
		}
		else if (vecString[2] == "gold" || vecString[2] == "ingot")//��Ԫ��
		{
			auto value = atoi(vecString[3].c_str());
			ADDUP(roleOther->dwIngot, value);
			//logs_db_thread::getMe().consume(service::id, role->roleName, role->dwRoleID, 0, value, purpose::gm, 0);
		}
		else if (vecString[2] == "charge")
		{
			DWORD dwChargeNum = ATOUL_(vecString[3]);
			PROTOCAL::chargeRole( dwChargeNum, roleOther, true);
		}
		else if (vecString[2] == "tao")
		{
			if (vecString.size() >= 4)
			{
				auto value = atoi(vecString[3].c_str());
				activity_taobao::charge(roleOther->dwRoleID, value * 100, true);
			}
		}
		else if (vecString[2] == "can_get_charge_prize")
		{
			roleOther->byRechargeState = E_TASK_COMPLETE_CAN_GET_PRIZE;
		}
	}
	else if (vecString[0] == "$bc")
	{
		if (vecString.size() >= 2)
			BroadcastCtrl::systemBroadcast(vecString[1]);
	}
	else if (vecString[0] == "$save_data")
	{
		if (vecString.size() >= 2)
		{
			if (vecString[1] == "all")
			{
				PROTOCAL::saveAllToDb(true);
			}
			else if (vecString[1] == "online")
			{
				service::log_online_info();
			}
		}
	}
	else if (vecString[0] == "$test")// ����ģʽ������رգ� $test ϵͳ�� ֵ
	{
		if (vecString.size() >= 2)
		{
			if (vecString[1] == "worldboss")// ���״̬ 0-3
			{
				if (vecString.size() >= 3)
				{
					auto status = atoi(vecString[2].c_str());
					WorldBoss::getMe().set_status(status);;
				}
			}
			else if (vecString[1] == "treasure")// ���ʱ���⿪��(on/off)
			{
				if (vecString.size() >= 3 && vecString[2] == "off")
					treasure::check = false;
				else
					treasure::check = true;
			}
			else if (vecString[1] == "bodyguard")// �����ڳ�ʱ�䣬Ĭ����0��δ��ʼ״̬������λСʱ
			{
				if (vecString.size() >= 3)
					bodyguard::debug_set_time(role, (time_t)atoi(vecString[2].c_str()));
				else
					bodyguard::debug_set_time(role);
			}
			else if (vecString[1] == "bodyguard_reload_config")// ���¼��������ļ�
			{
				bodyguard::load();
			}
		}
	}
	else if (vecString[0] == "$dump")// ����
	{
		if (vecString.size() >= 2)
		{
			if (vecString[1] == "treasure_rank")// ����ϵͳ
			{
				treasure::dump_rank(role);
			}
		}
	}

	return -1;//
}

void ROLE::chat(input_stream& stream)
{
	// ����
	if (deny_chat)
		return;

	enum
	{
		E_CHAT_TOO_FAST = 1,
		E_CHAT_BAN = 2,
		E_CHAT_NO_SUCH_CHANNEL = 3,

		E_GM_NO_SUCH_CMD = 11,
		E_GM_NO_SUCH_INDEX = 12,
		E_GM_INPUT = 13,
	};

	byte channel;
	string content;

	if (!stream.read_byte(channel))return;
	if (!stream.read_string(content))return;

	if (CONFIG::isInnerSieldWord(content))
	{
		return;
	}

	if (!content.empty() && content[0] == '$')
	{
		gm(this, content);
		return;
	}

	channel &= 0x7f;
	if (channel != 1 && channel != 2)
		return;

	output_stream out(S_GAME_CHAT);
	make_chat(this, channel, content, out);

	if (channel == 1)//����
	{
		service::broadcast(out);
		service::chats.push_back(string(out.get_buffer(), out.length()));
		if (service::chats.size() >20)
		{
			service::chats.pop_front();
		}
	}
	else if (channel == 2)//���
	{
		if (faction == NULL)
			return;

		faction->addChat(this, content);

		guild::broadcast(faction, out);
	}
	return;
}

void ROLE::send_firstcharge_list()
{
	string strData;
	for (auto ele : vecFirstCharge )
	{
		S_APPEND_DWORD(strData, ele);
	}
	PROTOCAL::sendClient(this->client, PROTOCAL::cmdPacket(S_REQUEST_INGOT_INFO, strData));
}


ITEM_CACHE* ROLE::get_best_equip()
{
	ITEM_CACHE* result = nullptr;
	uint fightvalue = 0;
	for (auto item : vBodyEquip)
	{
		if (item == nullptr)
			continue;

		if (item->dwFightValue > fightvalue)
		{
			result = item;
			fightvalue = item->dwFightValue;
		}
	}
	return result;
}

void ROLE::login_broad()
{
	if ( is_low_off_time(5*60) )
	{
		return;
	}

	e_top_type type = e_top_type::e_none;

	if ( this->dwRoleID == guild::get_first_leader_id() )
	{
		type = e_top_type::e_faction;
	}
	else if (this->dwRoleID == RANK::get_fightvalue_top_id() )
	{
		type = e_top_type::e_fightvalue;
	}
	else if (this->dwRoleID == RANK::get_level_top_id() )
	{
		type = e_top_type::e_level;
	}
	else if (this->dwRoleID == RANK::get_weapon_top_id() )
	{
		type = e_top_type::e_weapon;
	}
	else if (this->dwRoleID == ARENA::get_arena_top_id() )
	{
		type = e_top_type::e_arena;
	}
	else if (this->dwRoleID == service::get_first_meet_pk_id())
	{
		type = e_top_type::e_lastkill;
	}
	else if (this->dwRoleID == service::get_first_meet_combo_id())
	{
		type = e_top_type::e_kill;
	}

	if ( type != e_top_type::e_none )
	{
		BroadcastCtrl::rank_top_login( type, this);
	}
}

void ROLE::send_offline_report()
{
	string strData;
	DWORD tNow = (DWORD)time(nullptr);
	BYTE byRet = 0;//�ɹ�
	if (is_low_off_time(2 * 60))
	{
		return;
	}

	DWORD expOrig = this->dwExp;
	DWORD coinOrig = this->dwCoin;
	DWORD ingotOrig = this->dwIngot;

	list<ITEM_CACHE*> lstNotifyItem, lstNotifyProps;

	QUICKFIGHT::QUICK_REPORT stcReport;
	//����Ҫnotify item
	byRet = QUICKFIGHT::offlineEarning(this, (tNow - this->tLogoutTime), &lstNotifyItem, &lstNotifyProps, stcReport);
	S_APPEND_BYTE(strData, byRet);
	S_APPEND_NBYTES(strData, &stcReport, sizeof(stcReport));
	PROTOCAL::sendClient(this->client, PROTOCAL::cmdPacket(S_OFFLINE_FIGHT_REPORT, strData));

	notify_X_If(this, ROLE_PRO_CUREXP, dwExp, expOrig);
	notify_X_If(this, ROLE_PRO_COIN, dwCoin, coinOrig);
	notify_X_If(this, ROLE_PRO_INGOT, dwIngot, ingotOrig);
	ItemCtrl::notifyClientItem(this, lstNotifyItem, PKG_TYPE_EQUIP);
	ItemCtrl::notifyClientItem(this, lstNotifyProps, PKG_TYPE_ITEM);
}

bool ROLE::is_low_off_time(uint32_t atime)
{
	uint32 now = (uint32)time(nullptr);
	return (now < this->tLogoutTime + atime);
}


byte ROLE::set_battle_info(eFIGHTTYPE type, uint target_id, uint16 map_id)
{
	if (battle_info.type != E_FIGHT_NONE)
		return 1;

	battle_info.type = type;
	battle_info.target_roleid = target_id;
	battle_info.mapid = map_id;

	return 0;
}


void ROLE::send_activitys_info()
{
	enum type : byte
	{
		worldboss,// �����Ϲ�
		treasure,// ����
		bodyguard,// ����
		team,// �Ŷ�ս
		fund,// ����
		signup,// ǩ��
		max_count,
	};

	auto worldboss_activity_cfg = CONFIG::getActivityInfoCfg(1);
	auto treasure_activity_cfg = CONFIG::getActivityInfoCfg(2);
	auto bodyguard_activity_cfg = CONFIG::getActivityInfoCfg(3);
	auto team_activity_cfg = CONFIG::getActivityInfoCfg(4);
	if (!(worldboss_activity_cfg && treasure_activity_cfg && bodyguard_activity_cfg && team_activity_cfg))
	{
		logERROR("get activity info failed from send_activity_info()");
		return;
	}

	output_stream out(S_ACTIVITY_INFO);
	byte count = 0;
	auto pos = out.length();
	out.write_byte(type::max_count);

	// �����Ϲ�
	auto& tc = WorldBoss::getMe().timeconfig;
	out.write_byte(type::worldboss);
	out.write_byte((byte)worldboss_activity_cfg->open_level);
	out.write_byte((byte)WorldBoss::getMe().isSignUpWorldBoss(this));
	out.write_byte((byte)tc[0][0]);// ʱ��
	out.write_byte((byte)tc[0][1]);
	out.write_byte((byte)tc[1][0]);
	out.write_byte((byte)tc[1][1]);
	out.write_byte((byte)tc[2][0]);
	out.write_byte((byte)tc[2][1]);
	out.write_byte((byte)tc[3][0]);
	out.write_byte((byte)tc[3][1]);
	count++;
	
	// ����
	out.write_byte(type::treasure);
	out.write_byte((byte)treasure_activity_cfg->open_level);
	out.write_byte(treasure::get_remain(this));
	out.write_byte((byte)(treasure_activity_cfg->begin_time / 3600));
	out.write_byte((byte)(treasure_activity_cfg->begin_time % 3600 / 60));
	out.write_byte((byte)(treasure_activity_cfg->end_time / 3600));
	out.write_byte((byte)(treasure_activity_cfg->end_time % 3600 / 60));
	count++;
	
	// �ھ�
	out.write_byte(type::bodyguard);
	out.write_byte((byte)bodyguard_activity_cfg->open_level);
	byte bg_state; // bodyguard state
	byte bg_remain; // bodyguard remain receive
	time_t bg_start_time;// bodyguard start time
	bodyguard::get_info(this, bg_state, bg_remain, bg_start_time);
	out.write_int(bg_start_time);
	out.write_byte(bg_state);
	out.write_byte(bg_remain);
	count++;
	
	// ����
	int deliver;
	if (activity_fund::get_user_deliver(dwRoleID, deliver))
	{
		out.write_byte(type::fund);
		out.write_byte((byte)1);
		out.write_uint(deliver);
		byte levels[] = {10,20,30,40,50,55};
		out.write_byte((byte)sizeof(levels));
		out.write_data(levels, sizeof(levels));
		count++;
	}
	
	// ǩ��
	out.write_byte(type::signup);
	out.write_byte(0);
	out.write_byte(((1 << (SIGN::getDay() - 1)) & dwDaySignStatus) > 0 ? 1 : 0);
	count++;

	out.write_data(&count, 1, pos);
	send(out);
}

void ROLE::leaveTeam()
{
	int backCost = jiange_team::CJiangeTeamManager::getBuffCost(this->teamBuff);
	this->teamBuff = 0;
	this->teamID = 0;
	ADDUP(this->dwIngot, backCost);
	notifyIngot(this);
}