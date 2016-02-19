
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
#include "db.h"
#include "task.h"
#include "itemCtrl.h"
#include "logs_db.h"
#include "service.h"
#include "WorldBoss.h"
#include "sign.h"
#include <regex>  
#include "broadCast.h"
#include "common.h"


#define  MAX_NEWBIE_GUIDE_STEP  50

using namespace BASE;


 bool isEmailAddress( const string& client_email )
{
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
	if ( this->wVipLevel >= wLevel )
	{
		this->wVipLevel = wLevel;
		notifywVipLevel(this);
	}
}
void ROLE::addFiveDivineTime()
{

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

////装备奖励领取
//#define S_EQUIP_AWARD_FETCH	0x0010
//C:[]
//S:[BYTE, BYTE]
//[error, nextAward]
//error:返回码
//		  0:成功
//		  1:等级未达到
//		  2:装备背包满
//		  3:道具背包满
//		  4:奖励不存在
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

		ADDUP3( byAwardStep, 1, BYTE_MAX );

	} while (0);

	string strData;
	S_APPEND_BYTE(strData, byRet);
	S_APPEND_BYTE(strData, byAwardStep);
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_EQUIP_AWARD_FETCH, strData) );
	return 0 ;
}

////领取首充奖励
//#define	S_FETCH_RECHARGE_AWARD	0x004B
//C:[]
//S:[WORD]
//[error]
//error:0:成功
//		  1:领取条件未达到,首次充值后可领取
//		  2:装备背包满
//		  3:道具背包满
//		  4:已经领取
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

//#define	S_TODAY_GUIDE					0x0060//今日引导
//C:[ ]
//S: [BYTE]8个
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

	const DWORD dwDayCode = dwCode << byDay;//今天

	if( dwDayCode & this->dwDaySignStatus )//已经领取
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
	DWORD now = PROTOCAL::getCurTime();

	MYSTERY_SHOP_INFO& mysInfo = this->cMarMgr.cMarMys.m_vMysInfo[E_MYSTERY_SHOP_COMMON - 1];

	RETURN_COND( mysInfo.wFreeTimes > 0 && mysInfo.dwCanTreeTime < now ,1 )//one free

	return 0;
}

BYTE ROLE::isMysteryHignFree()	
{
	DWORD now = PROTOCAL::getCurTime();

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
//#define		S_TIME_GUIDE				0x0063		//客户端20min请求的引导
//C:[ ]
//S: [BYTE]20个
int ROLE::getTimeGuide( ROLE* role, unsigned char * data, size_t datalen )
{
//101	2	12	80	一个新的技能槽开启了，赶紧配置一个技能吧。
//102	2	12	80	有新技能可以升级了，升级技能更强力！
//103	2	12	80	活动奖励可以领了哦。
//104	2	12	80	任务奖励可以领了哦。
//105	2	12	80	主人，今天还没有签到哦。
//106	2	12	80	主人，主人，你有奖励邮件没有领哦。
//107	2	12	80	世界boss能够报名了，赶紧报名吧。
//108	2	12	80	商城的普通商人又免费了哦，赶紧抽奖去吧。
//109	2	12	80	商城的高级商人又免费了哦，赶紧抽奖去吧。
//110	2	12	80	要是缺钱了，别忘记了摇钱树哦。
//111	2	12	80	一符在手，天下我有。可以免费占卜啦。
//112	2	12	80	每天不挑战几个对手，心里总是觉得缺点什么。
//113	2	12	80	击败BOSS，每天日常。
//114	2	12	80	佣兵一直希望和你一起去打精英副本。
//115	2	12	80	想升级快，快速战斗不能少。
//116	2	12	80	打造装备，打着打着说不定就是绿装了。
//117	2	12	80	背包都塞满了，赶紧熔炼一下吧。
//118	2	12	80	礼包放在背包里不用，就是浪费资源。
//119	2	12	80	主人，您还没有好友呢，赶紧找些朋友吧。

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



////修改绑定邮箱
//S_MODIFY_BIND_EMAIL = 0x0935
//C:[BSTR]
//[bindEmail]
//bindEmail:绑定的邮箱地址
//S:[WORD]
//error
//	0:成功
//	1:没有充值,不能绑定
//	2:非法的邮箱地址
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

////请求充值绑定邮箱信息
//S_REQUEST_BIND_EMAIL = 0x0934
//	C[]
//S:[DWORD, BSTR]
//[chargeTotal, bindEmail]
//chargeTotal:累计充值元宝数量
//bindEmail:绑定的邮箱地址
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

////兑换码(CDKey)兑换奖励
//S_EXCHANGE_AWARDS_BY_KEY = 0x0936
//C:[BSTR]
//[cdkey]
//cdkey:兑换码
//S:[WORD]
//error
//	0:成功
//	1:该兑换码无效
//	2:装备背包已满
//	3:道具背包已满
//	4:已领取过该类礼包，不能重复领取
//	5:该激活码已被使用
//	6:领取道具不存在

int ROLE::getCodeAward( ROLE* role, unsigned char * data, size_t datalen )
{
	string strCode;
	string strData;
	WORD   wError = 0;
	RETURN_COND( !BASE::parseBSTR( data, datalen, strCode ), 0 );

	do 
	{
		SQLSelect sqlx( DBCtrl::getSQL(), "select item_id,type,is_use from "CODE_AWARD " where code = '%s' limit 1;", strCode.c_str() );
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

		execSQL(DBCtrl::getSQL(), "update "CODE_AWARD" set is_use = 1 where code = '%s'", strCode.c_str() );

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
	//身上
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


BYTE ROLE::roleGetCharge( const int nChargeNum, const int buyRealNum )
{
	RETURN_COND( nChargeNum <= 0, 1 );

	ADDUP( this->dwIngot, nChargeNum );
	
	ADDUP( this->dwCharge, buyRealNum );

	WORD oldLevel = this->wVipLevel;

	this->resetVip();

	notifyIngot( this );

	notifyCharge( this );

	if ( oldLevel != this->wVipLevel )
	{
		notifywVipLevel( this );
	}

	logs_db_thread::singleton().consume(service::id, roleName, dwRoleID, 0, buyRealNum, purpose::charge, 0);
	return 0;
}

/*
@人的战斗力 
@属性值除以系数和再相加，参数见attRe定义处
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

	//身上
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

		//基础属性
		for ( int j=0; j < MAX_EQUIP_BASE_ATTR_NUMS; j++)
		{
			BYTE attr = itemCache->attrValueBase[ j ].attr;

			if ( !IS_VALID_ATTR( attr  ) )
				continue;

			this->roleAttr[ attr ] += itemCache->attrValueBase[ j ].value;

		}

		for(size_t i = 0; i < sizeof(itemCache->equip_extends)/sizeof(equip_extend); i++)
		{
			auto& ee = itemCache->equip_extends[i];
			if(ee.type == 0)break;
			roleAttr[ee.type] += ee.value;
		}

		//附加属性,不受强化等级影响
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

						break;//找到最大等级跳出
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
	if ( type < E_ATTR_STAMINA || type > E_ATTR_SPIRIT )//保证 一级属性  基础属性
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

	MYSQL_ESCAPE_CPP( rolename2, this->roleName );

	char*compressed;
	unsigned long comLen;
	tie( compressed, comLen ) = compressRoleblob(&roleBlob);

	MYSQL_ESCAPE_ALLOCA_N(roleblob, compressed, comLen );

	EXEC_SQL( "update " TABLE_ROLE_INFO  " set level=%u, viplevel=%u, exp=%u, hp=%u, money=%u, gold=%u, charge=%u, mapid=%u,cur_sceneid=%u, "
		"fromid=%u, lastaccess=from_unixtime(%u), inc_hp_time=from_unixtime(%u), roleblob='%s'  where id=%u ", 
		this->wLevel, this->wVipLevel, this->dwExp, this->dwHP, this->dwCoin, this->dwIngot, this->dwCharge, this->wLastFightMapID, 0,
		0, this->tLastAccess, this->tIncHPTime, roleblob, this->dwRoleID );
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
	
	roleBlob.byNewMapKillMonNum = this->byNewMapKillMonNum;

	//// 已经熔炼的装备数量
	//roleBlob.bySmeltEquipCount = this->bySmeltEquipCount; 
	//// 已经熔炼的品质值
	//roleBlob.dwQualityValue = this->dwQualityValue;

	m_packmgr.cache2Blob(roleBlob);
	role_rune.cache2Blob(roleBlob);

	if ( this->itemForge )	//打造界面当前装备
	{
		roleBlob.itemDataForge.initFromItemCache( this->itemForge );
	}

	//身上
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

	
	//技能
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

	//挖矿
	memcpy( roleBlob.vMiner, this->vMiner, sizeof(roleBlob.vMiner) );

	//好友
	{
		
		for (size_t i=0; i<this->vecFriends.size(); i++)
		{
			BREAK_COND( i == MAX_FRIENDS_NUMS );
			roleBlob.arrFriendsID[i] = this->vecFriends[i];
		}
	}


	//竞技场记录
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

	//点赞
	{
		for (size_t i=0; i < this->vecDianZhanIDs.size(); i++)
		{
			BREAK_COND( i == MAX_DIANZHAN_NUMS );
			roleBlob.arrDianZhanIDs[i] = this->vecDianZhanIDs[i];
		}
	}

	//神秘商城
	{
		memcpy( roleBlob.m_vMysInfo, this->cMarMgr.cMarMys.m_vMysInfo, sizeof(roleBlob.m_vMysInfo) );
	}


	//珍宝商城
	{
		size_t index = 0;
		for ( auto &it : this->cMarMgr.cMarJew.m_listShopItem )
		{
			BREAK_COND( index == MAX_SHOP_ITEM );
			roleBlob.vShopItem[index] = it;
			++index;
		}
	}

	//任务
	{
		size_t index = 0;
		for ( auto &it : this->mapTask )
		{
			BREAK_COND( index == MAX_TASK_NUMS );
			roleBlob.tasksInfo[index] = it.second;
			//logplan("savetask:%u %u %u ",roleBlob.tasksInfo[index].wTaskID,roleBlob.tasksInfo[index].byTaskCompleteNums,roleBlob.tasksInfo[index].byTaskStatus);
			++index;
		}

	}	

	//roleBlob.dwComID = this->dwComID;

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
	roleBlob.deny_chat = ( this->deny_chat == false ? 0 : 1 );															// 禁言
	ASSIGN_CACHE_2_BLOB(pkvalue);
	ASSIGN_CACHE_2_BLOB(pk_count);
	ASSIGN_CACHE_2_BLOB(pk_buy_count);
	ASSIGN_CACHE_2_BLOB(pk_dead_count);
	ASSIGN_CACHE_2_BLOB(last_pk_count_inc_time);
	ASSIGN_CACHE_2_BLOB(login_day_num);
	
	// 偶遇
	int i = 0;
	for(auto& item : meet_pk_records)
	{
		roleBlob.meet_pk_records[i] = item;
		i++;
		if(i>= sizeof(roleBlob.meet_pk_records)/sizeof(meet_pk_record))
			break;
	}

	//佣兵
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

	//精英关卡
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

	//开服活动
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

	//首次充值类型
	{
		for (size_t i=0; i<this->vecFirstCharge.size(); i++)
		{
			BREAK_COND( i == MAX_CHARGE_TYPE );

			roleBlob.arrFirstCharge[i] = this->vecFirstCharge[i];
		}
	}

	//兑换码领取奖励
	{
		for ( size_t i = 0; i < this->vecCodeAwardType.size(); i++ )
		{
			BREAK_COND( i == MAX_CODE_AWARD_NUM );

			roleBlob.arrCodeAwardTypes[i] = this->vecCodeAwardType[i];
		}
	}
	
	roleBlob.world_boss_battle_attack_index = role_worldboss.attack_index;
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

	role->byNewMapKillMonNum = roleBlob.byNewMapKillMonNum;

	//// 已经熔炼的装备数量
	//role->bySmeltEquipCount = roleBlob.bySmeltEquipCount; 
	//// 已经熔炼的品质值
	//role->dwQualityValue = roleBlob.dwQualityValue;

	//打造界面的装备
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

	//身上	
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

	//技能
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

	//挖矿
	memcpy( role->vMiner, roleBlob.vMiner, sizeof(roleBlob.vMiner) );


	//好友
	for (size_t i=0; i<MAX_FRIENDS_NUMS; i++)
	{
		if (roleBlob.arrFriendsID[i] == 0)
		{
			break;
		}
		role->vecFriends.push_back(roleBlob.arrFriendsID[i]);
	}

	//竞技场记录
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

	//点赞
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

	// 偶遇
	for(auto& item : roleBlob.meet_pk_records)
	{
		if(item.id > 0)
			meet_pk_records.push_back(item);
	}

	//神秘商城
	{
		memcpy( this->cMarMgr.cMarMys.m_vMysInfo, roleBlob.m_vMysInfo, sizeof(roleBlob.m_vMysInfo) );
	}
	//商城
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

	//任务
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

	m_packmgr.patch();

	//佣兵
	{
		for (size_t i=0; i < MERCENARY_NUM; i++)
		{
			if (roleBlob.arrMerData[i].wID != 0)
			{
				role->cMerMgr.m_vecCMercenary.push_back( CMercenary(&roleBlob.arrMerData[i]) );
			}
		}
	}

	//精英关卡
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

	//开服活动
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

	//首次充值类型
	{
		for (size_t i=0; i < MAX_CHARGE_TYPE; i++)
		{
			BREAK_COND( roleBlob.arrFirstCharge[i] == 0 );

			role->vecFirstCharge.push_back(roleBlob.arrFirstCharge[i]);
		}
	}

	//兑换码领取
	{
		for (size_t i=0; i < MAX_CODE_AWARD_NUM; i++)
		{
			BREAK_COND( roleBlob.arrCodeAwardTypes[i] == 0 );

			role->vecCodeAwardType.push_back(roleBlob.arrCodeAwardTypes[i]);
		}
	}

	role_worldboss.attack_index = roleBlob.world_boss_battle_attack_index;

	return true;
}

// 上线
void ROLE::online()
{
	role_skill.autoUnlockSkills();
	role_rune.autoUnlockDivineGrids();
	this->dwCanFightMapTime = 0;
	this->dwComID = ARENA_NO_FIGHT;
	this->meet_pk_target_roleid = ARENA_NO_FIGHT;
	this->byArenaResult = 0;
}

// 下线
void ROLE::offline()
{

}

// 升级时
void ROLE::onLevelUp()
{
	role_skill.autoUnlockSkills();
	role_rune.autoUnlockDivineGrids();
}

// 是否有足够的金币
bool ROLE::hasEnoughCoin(DWORD need_coin)
{
	return this->dwCoin >= need_coin;
}

// 消耗指定数量的金币
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

// 是否有足够的元宝
bool ROLE::hasEnoughGold(DWORD need_gold)
{
	return this->dwIngot >= need_gold;
}

// 消耗指定数量的元宝
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

// 增加熔炼值
void ROLE::addSmeltValue(DWORD value)
{
	this->dwSmeltValue += value;

	notifyX( this, ROLE_PRO_SMELTVAR, dwSmeltValue );
}

// 是否有足够的熔炼值
bool ROLE::hasEnoughSmeltValue(DWORD value)
{
	return this->dwSmeltValue >= value;
}

// 扣除熔炼值
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

////  增加品质值
//void ROLE::addQualityValue(DWORD value)
//{
//	this->dwQualityValue += value;
//}

//// 重置品质值
//void ROLE::resetQualityValue()
//{
//	this->dwQualityValue = 0;
//}

//// 增加熔炼的装备数量
//void ROLE::addSmeltEquipCount(BYTE count)
//{
//	this->bySmeltEquipCount += count;
//}

//// 重置熔炼的装备数量
//void ROLE::resetSmeltEquipCount()
//{
//	this->bySmeltEquipCount = 0;
//}

WORD ROLE::getVipLevel()
{
	if ( wVipLevel >= wTmpVipLevel )
		return wVipLevel;

	if ( NNOW >= dwTmpVipEndTime )
		return wVipLevel;

	return wTmpVipLevel;

}

WORD ROLE::getExchangeSuitLimit()
{
	//非vip 每天1次。
	//vip1-vip3 每天2次。
	//vip4-vip5 每天3次。
	//vip6-vip8 ,每天4次。
	//vip8以上，每天6次。
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

//战斗力=0.2*生命最大值+1魔法最大值+2*攻击上限+5*防御上限+2*技能攻击上限+2*技能防御上限+5*命中值+5*闪避值+2.5*暴击值+2.5*暴抗值+1*暴伤伤害值+1*暴伤减免值+0.3*魔法恢复+0.3*生命恢复

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
	size_t offset = offsetof( ROLE, vBodyEquip );
	memset( ((char*)this) + offset,  0, sizeof(*this)-offset );

	memset(vBodyEquip, 0, sizeof(vBodyEquip));									//memset 分隔符
	memset(vecBossFightHelpers, 0, sizeof(vecBossFightHelpers));						//世界boss战友列表 不存
	faction = NULL;														//不保存到blob
	suitStrengthLevel= 0;													//身上装备等级和
	wLastFightMapID  = 0;													//挂机地图
	wCanFightMaxMapID= 0;													//当前可以打的地图(编号大于这个的地图都不能打)
	wWinMaxMapID     = 0;														//胜的最大地图
	tLastSaveTime    = 0;													//不保存
	tLogoutTime      = 0;														//退出时间
	byTitleId        = 0;
	client=NULL;
	wLevel      =0;															//等级
	dwFightValue=0;														//战斗力
	byJob       =0;
	bySex       =0;
	wVipLevel   =0;															//VIP等级
	dwExp       =0;															//当前经验
	dwHP        =0;																//体力
	dwCoin      =0;															//货币(游戏币)
	dwIngot     =0;															//元宝(充值币)
	dwCharge    =0;															//累计充值
	tLastAccess =0;														//最近访问时间
	tIncHPTime  =0;														//补充体力的时间
	dwHonour    =0; 															//荣耀
	byMailLoaded=0;
	byDirty     =0;
	dwRoleID    =0;
	memset(byFightSkill, 0, sizeof(byFightSkill));									//战斗技能索引，指向_skillId[10]中1-10位置，0表示没有
	wOnUpSkill =0;														//正在修炼技能id,0表示不在修炼
	memset( roleAttr, 0,sizeof(roleAttr) );												//0不用,1-21
	byArenaDayCount   =0;  													//每日已经参加竞技场比赛次数
	byBuyArenaDayCount=0;												//每日已经购买竞技场比赛次数
	dwDianZhan        =0;														//最近访问时间
	byPredictIndex    =0;
	memset(predictEarning, 0,sizeof(predictEarning) );
	lastKillMonAddExpTime;											//这个不保存
	prizeMailNums =0;
	msgMailNums   =0;
	systemMailNums=0;
	dwComID       =0;															//竞技场对手id 需要保存
	byArenaResult =0;														//竞技场结果
	bySignTimes   =0;														//sign times per month
	dwDaySignStatus=0;													//day sign status bit
	byEnhanceTime =0;														//活动boss战鼓舞次数
	bySendFightBossTimes = 0;												//挑战boss次数每天三次0点重置
	byDailyBuyBossTimes  = 0;												//购买挑战boss次数不重置
	byAllBuyBossTimes	   = 0;
	bySendFightHeroTimes = 0;												//精英关卡
	byDailyBuyHeroTimes  = 0;												//购买挑战hero次数不重置
	byAllBuyHeroTimes    = 0;													
	byUseMoneyTreeTimes  = 0;												//moneytree次数0点重置
	byBuyQuickFightTimes = 0;												//购买快速战斗次数
	dwAutoSellOption     = 0;
	dwMiningRemainTime   = 0;												//本人挖矿剩余时间
	byMiningSlotIndex    = 0;													//本人挖矿的坑位，这个位置和vMiner里的空位共同决定房间的空位
	byRes____            = 0;															//切换矿场，临时刷出来的，挖矿的坑位，
	memset(vMiner, 0, sizeof(vMiner) );
	memset(vMinerTmpRefresh, 0,sizeof(vMinerTmpRefresh) );								//点击切换时刷新出来的临时列表
	wCurMaxTaskID			=0;
	dwCanEnterFightMapTime=0;											//可以进入地图刷怪时间，不保存
	dwCanFightMapTime		=0;												//可以进入地图打怪时间，不保存
	dwFinalWorldBossTime	=0;												//最后参与世界boss时间
	wTmpVipLevel			=0;
	dwTmpVipEndTime		=0;													//暂时VIP 失效时间
	dwSmeltValue			=0;														//熔炼值
	byForgeRreshTimes		=0;													//打造界面已经刷新次数,日清
	itemForge=NULL;													//打造界面当前装备
	byNewMapKillMonNum = 0;												//进入新地图统计杀怪数量，最高100
	//bySmeltEquipCount  = 0;													// 已经熔炼的装备数量
	//dwQualityValue     = 0;													// 已经熔炼的品质值
	byNewbieGuideStep  = 0;												//新手引导步骤
	dwFisrtLoginTime   = 0;													//初始登录时间
	close_user_valid_time = 0;										// 封号有效时间
	deny_chat = false;															// 禁言
	pkvalue = 0;													// PK值
	pk_count = 0;													// PK次数
	pk_buy_count = 0;													// PK次数
	pk_dead_count = 0;													// PK死亡次数
	meet_pk_target_roleid = 0;															//竞技场对手id 需要保存
	last_pk_count_inc_time = 0;						// 最后pk数量递增时间
	meet_pk_combo_count = 0;						// 偶遇PK连击数
	login_day_num = 0;

	//额外注意的
	wLevel = 1;

	wCanFightMaxMapID = 1;
	wLastFightMapID = 1;

	tLastSaveTime = PROTOCAL::getCurTime() + 60;
	tLogoutTime = PROTOCAL::getCurTime()+10;

	byCurLevelAwardStep = 1;
	byRechargeState = 0;
	meet_pk_target_roleid = 0;
}



ROLE::~ROLE()
{
	CC_SAFE_DELETE(itemForge);

	//if ( itemForge != NULL)
	//{
	//	delete itemForge;
	//}
}




int ROLE::fight_report_for_meet(const DWORD target_role_id, const BYTE result, vector<DROP_SEQ>& drop_items)
{
	RETURN_COND( result!=E_MATCH_STATUS_LOSS && result!=E_MATCH_STATUS_WIN, 0 );

	ROLE* target = RoleMgr::getMe().getRoleByID( target_role_id );

	RETURN_COND( target == NULL, 0 );

	dwComID = ARENA_NO_FIGHT;//战斗结束

	int reward_pkvalue;
	int reward_pkvalue_t;

	auto mine_record = get_meet_pk_result(target_role_id, false);
	if(pk_count == 5)last_pk_count_inc_time = time(0);
	if(pk_count > 0)pk_count--;
	if ( result == E_MATCH_STATUS_LOSS )//输了给钱
	{
		meet_pk_combo_count = 0;
		target->meet_pk_combo_count++;
		if(target->wLevel >= wLevel)
			reward_pkvalue = 1;
		else
			reward_pkvalue = (4 + (wLevel - target->wLevel) * 4) / 4;

		pk_dead_count++;

		// 如果有防守记录则改变记录为复仇失败
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
			strcpy(record.name, target->roleName.c_str());
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
		strcpy(record.name, roleName.c_str());
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

		// 奖励
		auto reward_coin = reward_pkvalue * 400 + target->wLevel * 20;
		pkvalue += reward_pkvalue;
		dwCoin += reward_coin;
		meet_pk_combo_count++;
		target->meet_pk_combo_count = 0;
		if(target->pkvalue >= (unsigned)reward_pkvalue_t)
			target->pkvalue -= reward_pkvalue_t;
		else
			target->pkvalue = 0;

		service::meet_pk_rank_update(this, pkvalue);

		if(target->wLevel + 10 >= wLevel)
		{
			auto probability = 0.3f - (float)(wLevel - target->wLevel) * 0.02f;
			if(probability>0.4f)probability = 0.4f;//（最高不超过0.4）
			auto rv = BASE::randTo(10000)/10000.f;
			if(rv < probability)
			{
				unsigned reward_item_id = 0;
				// 70%概率为青铜宝箱（物品id=510），25%概率为白银宝箱（物品id=511），5%概率为黄金宝箱（物品id=512）
				auto rv = BASE::randTo(10000)/10000.f;
				if(rv < 0.7f)
					reward_item_id = 510;
				else if(rv < 0.95f)
					reward_item_id = 511;
				else
					reward_item_id = 512;

				auto itemcfg = CONFIG::getItemCfg(reward_item_id);
				this->m_packmgr.makeItem(itemcfg, 1);
				drop_items.push_back(DROP_SEQ( reward_item_id,  1, 0, 2, 0));

				auto item = ITEM_CACHE(itemcfg, 1);
				output_stream stream(S_NOTIFY_NEW_ITEM);
				stream.write_byte((byte)PKG_TYPE_ITEM);
				stream.write_byte(1);
				ItemBriefInfo brefInfo(&item);
				stream.write_data((char*)&brefInfo, sizeof(ItemBriefInfo));
				if(client)bufferevent_write(client->client_bev, stream.get_buffer(), stream.length());
			}
			else
			{
				probability = 0.15f - (float)(wLevel - target->wLevel) * 0.01f;
				if(probability>0.2f)probability = 0.2f;// 0.15-（自身等级-对方等级）*0.01（最高不超过0.2）
				auto rv = BASE::randTo(10000)/10000.f;
				if(rv < probability)
				{
					unsigned reward_item_id = 0;
					// 70%概率为青铜钥匙（物品id=500），25%概率为白银钥匙（物品id=501），5%概率为黄金钥匙（物品id=502）
					auto rv = BASE::randTo(10000)/10000.f;
					if(rv < 0.7f)
						reward_item_id = 500;
					else if(rv < 0.95f)
						reward_item_id = 501;
					else
						reward_item_id = 502;

					auto itemcfg = CONFIG::getItemCfg(reward_item_id);
					this->m_packmgr.makeItem(itemcfg, 1);
					
					drop_items.push_back(DROP_SEQ( reward_item_id,  1, 0, 2, 0));

					auto item = ITEM_CACHE(itemcfg, 1);
					output_stream stream(S_NOTIFY_NEW_ITEM);
					stream.write_byte((byte)PKG_TYPE_ITEM);
					stream.write_byte(1);
					ItemBriefInfo brefInfo(&item);
					stream.write_data((char*)&brefInfo, sizeof(ItemBriefInfo));
					if(client)bufferevent_write(client->client_bev, stream.get_buffer(), stream.length());
				}
			}
		}
		
		// 如果有防守记录则改变记录为复仇失败
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
			strcpy(record.name, target->roleName.c_str());
			record.time = time(0);
			record.reward_pkvalue = reward_pkvalue;
			record.result = meet_pk_result::attack_win;
			meet_pk_records.push_back(record);
		}


		logs_db_thread::singleton().consume(service::id, roleName, dwRoleID, reward_coin, 0,
			purpose::meet_battle_report, 0);

		meet_pk_record record;
		record.coin = 0;
		record.id = dwRoleID;
		record.job = byJob;
		record.sex = bySex;
		strcpy(record.name, roleName.c_str());
		record.time = time(0);
		record.reward_pkvalue = -reward_pkvalue_t;
		record.result = meet_pk_result::defence_lose;
		target->meet_pk_records.push_back(record);

		// 
		if(target->client)
		{
			output_stream stream(S_MEET_BE_KILLED);
			stream.write_string(roleName);
			stream.write_uint((unsigned)reward_coin);
			bufferevent_write(target->client->client_bev, stream.get_buffer(), stream.length());
		}
		if(meet_pk_combo_count >= 5)
		{
			output_stream stream(S_SYSTEM_BROADCAST);
			stream.write_byte(12);
			stream.write_ushort(meet_pk_combo_count);
			stream.write_string(roleName);
			PROTOCAL::forEachOnlineRole( [&stream] ( const std::pair<DWORD, ROLE_CLIENT> &x )
				{
					if(x.second.client)
						bufferevent_write( x.second.client->client_bev, stream.get_buffer(), stream.length());
				}
			);
		}
	}

	meet_pk_target_roleid = ARENA_NO_FIGHT;//战斗结束
	return 0;
}

// 获取偶遇PK记录，attack 为是否请求攻击类型的
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
}