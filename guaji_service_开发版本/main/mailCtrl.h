#ifndef __MAILCTRL_H__
#define __MAILCTRL_H__


#include "typedef.h"
#include "config.h"

using namespace std;

//#include "role.h"
class ROLE;
class ROLE_BLOB;
struct PlayerInfo;


#pragma pack(push, 1)
				
#pragma pack(pop)



enum MAIL_TYPE
{
	//"mailtype tinyint (4) unsigned not null default 0, "//0=奖励，1=消息，2=系统。
	E_MAIL_TYPE_PRIZE = 0,
	E_MAIL_TYPE_MSG = 1,
	E_MAIL_TYPE_SYSTEM = 2,

};

namespace MailCtrl
{

	
	DECLARE_CLIENT_FUNC( clientRequestMails );
	DECLARE_CLIENT_FUNC( clientDelMail );

	void notifyNewMail( ROLE* role, int mailTypeMask);

	void onlineNotifyMail( ROLE* role );

	void loadMailFromDB(ROLE* role);

	void sendMail( DWORD dwRoleID, BYTE byMailType, const string& strContent, const vector<ITEM_INFO> &vecItemInfo );

	bool send_full_scale_mail(BYTE byMailType, const string& strContent, const vector<ITEM_INFO> &vecItemInfo);

	bool makeMailItems( ROLE* role, const vector<ITEM_INFO> &vecItemInfo );
};

#endif		//__MAILCTRL_H__
