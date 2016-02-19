#ifndef __FACTIONCTRL_H__
#define __FACTIONCTRL_H__


#include "typedef.h"
#include "config.h"
#include <deque>
using namespace std;

#include "role.h"
class ROLE_BLOB;

enum E_FACTION_JOB
{

	E_NONE = 0,			//没有加入帮派

	E_MEMBER = 1,		//普通帮众

	E_LEADER = 2,		//帮主
	E_LEADER2 = 3,		//副帮主

	E_GUADIAN_LEFT =4,	//左护法
	E_GUADIAN_RIGHT =5,	//右护法

};


#define MAX_BUILDER_LEVEL (30)

enum E_BUILDER_TYPE
{

	E_BUILDER_TYPE_MAIN = 1,		//1行会	最大人数
	E_BUILDER_TYPE_WELFARE = 2,		//2	福利	道具索引,数量
	E_BUILDER_TYPE_TEMPLE = 3,		//3	圣殿	0
	E_BUILDER_TYPE_HP = 4,			//4	生命值	增加hp_max的值
	E_BUILDER_TYPE_MP = 5,			//5	法力值	增加mp_max的值
	E_BUILDER_TYPE_ATK = 6,			//6	物理攻击	增加物理攻击的值，格式：物攻下限,物攻上限
	E_BUILDER_TYPE_MAGIC_ATK = 7,	//7	魔法攻击	增加魔理攻击的值，格式：魔法下限,魔法上限
	E_BUILDER_TYPE_SOL_ATK = 8,		//8	道术攻击	增加道理攻击的值，格式：道术下限,道术上限
	E_BUILDER_TYPE_DEF = 9,			//9	物理防御	增加物理攻击的值，格式：物防下限,物防上限
	E_BUILDER_TYPE_MAG_DEF = 10,	//10	魔法防御	增加物理攻击的值，格式：魔防下限,魔防上限


	E_BUILDER_NUMS = 10,
};



struct FACTION_MEMBER
{
	DWORD dwRoleID;

	string strRoleName;

	BYTE byJob;

	WORD wLevel;
	
	E_FACTION_JOB facJob;

	DWORD dwContribute;


	size_t contributeRank;//暂存,帮贡排名
	
	FACTION_MEMBER()
	{
		facJob = E_MEMBER;
		dwRoleID = 0;
		dwContribute = 0;

		wLevel = 1;

		contributeRank = 1;

	}


};

#define IS_VALID_BUILDER_ID(id)		(id>=E_BUILDER_TYPE_MAIN && id<=E_BUILDER_TYPE_MAG_DEF)


#define GET_FACTION_LEVEL(faction)	( faction->builderLevel[E_BUILDER_TYPE_MAIN] )
#define IS_LEADER(role)			( role->faction && role->faction->dwLeaderID == role->dwRoleID )
#define IS_VICE_LEADER(role)			( role->faction && role->faction->dwLeaderID2 == role->dwRoleID )

//		WORD(成员id),char[22](成员名字),BYTE(等级),BYTE(职业),

class ROLE_SIMPLE_INFO
{
public:
	DWORD dwRoleID;
	string strRolename;
	WORD wLevel;
	BYTE byJob;
	BYTE bySex;

	ROLE_SIMPLE_INFO(DWORD dwRoleID, const string& strRolename, WORD wLevel, BYTE byJob, BYTE bySex)
	{
		this->dwRoleID = dwRoleID;
		this->strRolename = strRolename;
		this->wLevel = wLevel;
		this->byJob = byJob;
		this->bySex = bySex;

	}
	
	ROLE_SIMPLE_INFO(ROLE * role)
	{

		this->dwRoleID = role->dwRoleID;
		this->strRolename = role->roleName;
		this->wLevel = role->wLevel;
		this->byJob = role->byJob;
		this->bySex = role->bySex;
	
	}
};

struct FACTION_CHAT
{
	time_t tTime;
	
	E_FACTION_JOB facJob;

	DWORD dwRoleID;

	string strRoleName;

	string strContent;

};


struct FACTION
{
	DWORD dwFactionID;

	string strName;
		
	string strLeaderName;
	DWORD dwLeaderID;
	
	string strLeaderName2;
	DWORD dwLeaderID2;

	string strGuardianLeftName;//左护法
	DWORD dwGuardianLeftID;
	string strGuardianRightName;
	DWORD dwGuardianRightID;//右护法

	bool needSortContribute;
	
	size_t facRank;	//帮派排名

	WORD builderLevel[E_BUILDER_NUMS];		//base 0
	DWORD builderExp[E_BUILDER_NUMS];

	//下面的参数从配置表更新
	WORD wMemberLimit;
	WORD wWelfare;
	WORD wTemple;
	WORD wHp;
	WORD wMp;
	WORD wAtk_max;
	WORD wAtk_min;
	WORD wMagic_max;
	WORD wMagic_min;
	WORD wSol_max;
	WORD wSol_min;
	WORD wDef_max;
	WORD wDef_min;
	WORD wMdef_max;
	WORD wMdef_min;

	DWORD dwContribute;
	DWORD dwExp;


	map<DWORD, FACTION_MEMBER> mapFactionMember;//key : roleid
	deque<FACTION_CHAT> deqFactionChat;
	
	
	list<ROLE_SIMPLE_INFO> lstReqJoin;

	string strNoti;//帮派公告

	void initBuilderPro( );
	void addChat( ROLE* role, const string& strContent );
//	bool upgradeBuilder( E_BUILDER_TYPE e );
	
	DWORD getBuilderUpgradeExp(WORD wLevel, E_BUILDER_TYPE eType);
	bool addBuilderExp( E_BUILDER_TYPE e, DWORD dwAddExp );

	DWORD getMemberLimit();

	size_t getContributeRank(DWORD roleid);


	FACTION(const string& strFactionName);//, const string& strLeaderName, DWORD dwLeaderID);

};



namespace FactionCtrl
{

	int clientGetGuildRecommendList( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGetGuildRankList( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGetGuildReq( ROLE* role, unsigned char * data, size_t dataLen );
	int clientGuildFind( ROLE* role, unsigned char * data, size_t dataLen );


	DECLARE_CLIENT_FUNC( clientGuildReqJoin );

	
	DECLARE_CLIENT_FUNC( clientGuildCreate );

	DECLARE_CLIENT_FUNC( clientGuildGetInfo );


	DECLARE_CLIENT_FUNC( clientGuildCreateContent );

	DECLARE_CLIENT_FUNC( clientGuildGetMemList );
	DECLARE_CLIENT_FUNC( clientGuildGetMemEquip );
	DECLARE_CLIENT_FUNC( clientGuildAddFriend );

	DECLARE_CLIENT_FUNC( clientGuildAppointJob );
	DECLARE_CLIENT_FUNC( clientGuildKickMember );
	DECLARE_CLIENT_FUNC( clientGuildQuit );


	DECLARE_CLIENT_FUNC( clientGuildGetReqList );
	DECLARE_CLIENT_FUNC( clientGuildReqListOp );
	DECLARE_CLIENT_FUNC( clientGuildReqAcceptALL );


	DECLARE_CLIENT_FUNC( clientGuildGetSayList );
	DECLARE_CLIENT_FUNC( clientGuildGetTechList );
	DECLARE_CLIENT_FUNC( clientGuildDonateEquip );
	DECLARE_CLIENT_FUNC( clientGuildDonateCoin );


	E_FACTION_JOB getRoleFacJob( ROLE* role );

	FACTION_MEMBER* getFacMember( ROLE* role );
	FACTION_MEMBER* getFacMemberByRoleID( FACTION* faction, DWORD dwRoleID );


	void loadFaction( );
	FACTION* createFaction( ROLE* role, const string& factionName);
	FACTION* getFaction( DWORD dwFactionID );
	FACTION* getFactionByRoleID( DWORD dwRoleID );

	void broadCast( FACTION* faction, const string& strPkt );
	void factionDisband( FACTION* faction );

	void saveApplyList();

	bool addRoleFacContribute(ROLE* role, DWORD dwAddContribute, FACTION_MEMBER* fm=NULL );

};

#endif		//__FACTIONCTRL_H__
