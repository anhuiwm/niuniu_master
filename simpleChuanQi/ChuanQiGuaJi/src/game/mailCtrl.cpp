#include "log.h"
#include "base.h"
#include "protocal.h"
#include "db.h"

#include "zlibapi.h"
#include "roleCtrl.h"
#include "factionCtrl.h"
#include "itemCtrl.h"

#include "skill.h"
#include "mailCtrl.h"

#include "arena.h"

#include <algorithm>
#include  <numeric> 


#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output
#include <cstdio>

using namespace rapidjson;


namespace MailCtrl
{

	static string& mkMailInfo(ROLE* role, string &strData);

}


void MailCtrl::onlineNotifyMail( ROLE* role )
{

//邮件系统功能通知, INFO:不定长度类型,根据系统功能不同,定义不同数据格式
//	S:[[BYTE, INFO]]
//		[[functionType, info]]
//		functionType:	系统功能.
//			1.邮件功能系统
//				INFO: BYTE
//				info:是否有未读邮件. bit位开关, 可多开关位或
//					0:没有未读邮件
//					1:有奖励邮件未读
//					2:有消息邮件未读
//					4:有系统邮件未读
//			2.xxx
//				INFO: xxx
//				info:xxxx
//					xxxx

	int mailTypeMask = 0;

	for ( auto it : role->mapMail )
	{
		if ( it.second.isRead )
			continue;

		if ( it.second.mailType == E_MAIL_TYPE_PRIZE )
			mailTypeMask |= 1;
		else if ( it.second.mailType == E_MAIL_TYPE_MSG )
			mailTypeMask |= 2;
		else	//E_MAIL_TYPE_SYSTEM
			mailTypeMask |= 4;
	}
	

	string strData;
	S_APPEND_BYTE( strData, 1 );
	S_APPEND_BYTE( strData, mailTypeMask );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FUNCTION_NOTIFY, strData) );
}

#define MAX_MAIL_NUMS		(15)

		
//收到新邮件, 服务器在线推送
void MailCtrl::sendMail( DWORD dwRoleID, BYTE byMailType, const string& strContent, const vector<ITEM_INFO> &vecItemInfo )
{
//	S:[BYTE, [DWORD, DWORD, BYTE, BSTR]]
//		[num, [emailId, emailTm, eType, content]]
//		num:邮件数量
//		emailId:邮件ID
//		emailTm:邮件生成时间
//		eType:邮件类型
//		content:邮件内容.BSTR.

	if ( strContent.size() >= 200 || vecItemInfo.size() > 30 )
		return;
	
	if ( byMailType >E_MAIL_TYPE_SYSTEM )
		return;

	ROLE* role = RoleMgr::getMe().getCacheRoleByID( dwRoleID );
	if ( role )
	{
		if ( byMailType == E_MAIL_TYPE_PRIZE )
		{
			if ( role->prizeMailNums >= MAX_MAIL_NUMS )
				return;
			role->prizeMailNums++;
		}
		else if ( byMailType == E_MAIL_TYPE_MSG )
		{
			if ( role->msgMailNums >= MAX_MAIL_NUMS )
				return;
			role->msgMailNums++;
		}
		else if ( byMailType == E_MAIL_TYPE_SYSTEM )
		{
			if ( role->systemMailNums >= MAX_MAIL_NUMS )
				return;
			role->systemMailNums++;
		}
	}
	else	//不在cache 里面的角色
	{
		//错误的roleid照样插入
		SQLSelect sqlx( DBCtrl::getSQL(), "select count(1) from " TABLE_MAIL  " where roleid=%u and mailtype=%u", dwRoleID, byMailType );

		MYSQL_ROW row = sqlx.getRow();
		if ( row == NULL || atoi(row[0]) >= MAX_MAIL_NUMS  )
			return;

	}

	//如果角色在线，发notify,设置为已读邮件
	session* client = role ? PROTOCAL::getOnlineClient(role->dwRoleID) : NULL;
	BYTE byIsRead = !!client;

	MYSQL_ESCAPE_CPP(szContent, strContent );

	char szPrize[100]={0};//no need to escape
	CONFIG::itemInfo2Str( szPrize, vecItemInfo );
	
	DWORD tNow = PROTOCAL::getCurTime();
	
	EXEC_SQL( "insert into " TABLE_MAIL  " set roleid=%u, isread=%u, mailtype=%u, recvtime=%u, content='%s', prize='%s'  ", 
						dwRoleID, byIsRead, byMailType, tNow,  szContent, szPrize );

	if ( role == NULL )
		return;

	DWORD mailID = (DWORD)mysql_insert_id( DBCtrl::getSQL() );

	MAIL &mail = role->mapMail[ mailID ];
	mail.mailID = mailID;
	mail.recvTime = tNow;
	mail.mailType = byMailType;
	mail.isRead = byIsRead;
	mail.strContent = strContent;

	if ( !vecItemInfo.empty() )
		mail.vecPrize = vecItemInfo;

	if ( client )
	{
		string strData;

		S_APPEND_BYTE( strData, 1 );

		S_APPEND_DWORD( strData, mail.mailID );
		S_APPEND_DWORD( strData, mail.recvTime );
		S_APPEND_BYTE( strData, mail.mailType );
		S_APPEND_BSTR( strData, mail.strContent );

		PROTOCAL::sendClient( client, PROTOCAL::cmdPacket(S_NOTIFY_EMAIL, strData) );
	}
	
}


void MailCtrl::loadMailFromDB(ROLE* role)
{
	if ( role->byMailLoaded )
		return;

	role->byMailLoaded = 1;
	
	role->prizeMailNums = 0;
	role->msgMailNums = 0;
	role->systemMailNums = 0;
	
	role->mapMail.clear();

	SQLSelect sqlx( DBCtrl::getSQL(), "select id, recvtime, mailtype, isread, content, prize  from " TABLE_MAIL 
								 " where roleid=%u ", role->dwRoleID );

	while( MYSQL_ROW row = sqlx.getRow() )
	{
		DWORD mailID = ATOUL(row[0]);

		MAIL &mail = role->mapMail[ mailID ];

		mail.mailID = mailID;
		
		mail.recvTime = ATOUL(row[1]);
		mail.mailType = ATOB(row[2]);
		mail.isRead = ATOB(row[3]);

		mail.strContent = row[4];

		CONFIG::parseItemInfoStr( row[5], mail.vecPrize );

		if ( mail.mailType == E_MAIL_TYPE_PRIZE )
			role->prizeMailNums++;
		else if ( mail.mailType == E_MAIL_TYPE_MSG )
			role->msgMailNums++;
		else if ( mail.mailType == E_MAIL_TYPE_SYSTEM )
			role->systemMailNums++;

	}

}


string& MailCtrl::mkMailInfo(ROLE* role, string &strData )
{
	S_APPEND_BYTE( strData, role->mapMail.size() );

	for ( auto it : role->mapMail )
	{
		S_APPEND_DWORD( strData, it.second.mailID );
		S_APPEND_DWORD( strData, it.second.recvTime );
		S_APPEND_BYTE( strData, it.second.mailType );
		S_APPEND_BSTR( strData, it.second.strContent );

//		const char* content = getMailContentByPrizeID(it.second.mailID);
//		if ( content )
//			S_APPEND_BSZ( strData, content );
//		else
//			S_APPEND_BSTR_NULL(strData);

	}

	return strData;	
}


int MailCtrl::clientRequestMails( ROLE* role, unsigned char * data, size_t dataLen )
{
//	C:[BYTE]
//		[eType]
//		eType:邮件类型.
//			0:奖励邮件
//			1:消息
//			2:系统
//	S:[BYTE, [DWORD, DWORD, BYTE, BSTR]]
//		[num, [emailId, emailTm, eType, content]]
//		num:邮件数量
//		emailId:邮件ID
//		emailTm:邮件生成时间
//		eType:邮件类型
//		content:邮件内容.BSTR.


	BYTE byMailType;
	if ( !BASE::parseBYTE( data, dataLen, byMailType) )
		return 0;

	MailCtrl::loadMailFromDB( role );

	for ( auto it : role->mapMail )
	{
		if (  it.second.mailType == byMailType && !it.second.isRead )
		{
			EXEC_SQL( "update " TABLE_MAIL  " set isread=1 where roleid=%u and mailtype=%u", role->dwRoleID, byMailType );
			break;
		}
	}

	string strData;
	mkMailInfo( role, strData );

	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_REQUEST_EMAILS, strData) );
	return 0;
}


time_t getTimeHMS(time_t tNow, int h, int m, int s)
{
	struct tm* pTm = localtime(&tNow);
	pTm->tm_hour = h;
	pTm->tm_min = m;
	pTm->tm_sec = s;
	return mktime(pTm);
}



bool MailCtrl::makeMailItems( ROLE* role, const vector<ITEM_INFO> &vecItemInfo )
{
	if ( vecItemInfo.empty() )
		return true;

//	//不考虑合并
//	size_t needSlot = 0;
//	size_t needMineSlot = 0;

//	getNeedSlot( vecItemInfo, needSlot , needMineSlot );

//	if ( role->m_packmgr.lstMinePkg.size() + needMineSlot  > role->m_packmgr.curMineLimit )
//		return false;

//	if ( role->m_packmgr.lstEquipPkg.size() + needSlot  > role->m_packmgr.curEquipPkgLimit )
//		return false;

	if ( ItemCtrl::getEnoughSlot( role, vecItemInfo ) != 0 )
		return false;

	ItemCtrl::makeAllTypeItem(role, vecItemInfo);

	return true;
}

int MailCtrl::clientDelMail( ROLE* role, unsigned char * data, size_t dataLen )
{
//	C:[DWORD]
//		[emailId]
//		emailId:邮件ID
//	S:[WORD]
//		[error]
//		error:错误码
//			0:成功
//			1:邮件不存在
//			2:包裹已满,无法领取

	DWORD dwMailID;
	if ( !BASE::parseDWORD( data, dataLen, dwMailID) )
		return 0;

	WORD wErrCode = 0;
	do
	{
		auto it = role->mapMail.find( dwMailID );

		COND_BREAK( it == role->mapMail.end(), wErrCode, 1 );

		COND_BREAK( !makeMailItems( role, it->second.vecPrize ), wErrCode, 2 );

		if ( it->second.mailType == E_MAIL_TYPE_PRIZE )
			role->prizeMailNums--;
		else if ( it->second.mailType == E_MAIL_TYPE_MSG )
			role->msgMailNums--;
		else if ( it->second.mailType == E_MAIL_TYPE_SYSTEM )
			role->systemMailNums--;

		role->mapMail.erase( it );
		EXEC_SQL( "delete from " TABLE_MAIL  " where id=%u", dwMailID );

	}while(0);

	string strData;
	S_APPEND_WORD( strData, wErrCode );
	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_DELETE_EMAIL, strData) );
	return 0;
}

