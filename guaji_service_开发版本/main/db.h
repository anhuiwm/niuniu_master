#pragma once
#include "typedef.h"
#include "mysqlCtrl.h"



#define TABLE_DENY_IP			"denyip"
#define TABLE_ROLE_INFO			"roleinfo"
#define TABLE_CHARGE_INFO			"rolecharge"
#define TABLE_TACTICS				"tactics"

#define TABLE_ROLEDICT			"roledict"
#define TABLE_ROLEDID			"roleid"
#define TABLE_ONLINE_NUM			"online_num"

#define TABLE_EXCHANGE			"exchange"


#define TABLE_FACTION				"faction"
#define TABLE_FACTION_MEMBER		"factionmember"
//#define TABLE_FACTION_BUILDER		"factionbuilder"

#define TABLE_FACTION_APPLY		"factionapply"		//��������

#define TABLE_RANK		"rank"		//���а�
#define TABLE_MAIL		"mail"

#define CODE_AWARD			"activation_code"

#define EXEC_SQL( fmt, ... ) execSQL( DBCtrl::getSQL(), fmt, ##__VA_ARGS__)


namespace DBCtrl
{

	bool dbInit( void );
	MYSQL* getSQL( void );

};
