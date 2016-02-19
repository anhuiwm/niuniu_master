#ifndef __RoleMgr_H__
#define __RoleMgr_H__

#include "typedef.h"
#include "role.h"

#include <unordered_map>

//#include "proto.h"

using namespace std;

//#include "role.h"
class ROLE;
class ROLE_BLOB;
struct PlayerInfo;

#define TEST_ROLE_LEVEL		(1)

class RoleMgr
{
public:
	static RoleMgr& getMe()
	{
		static RoleMgr s_role;
		return s_role;
	}

	void init();

	string findUuidBySerial( const string& serial, const BYTE platform_id );

	ROLE* findRoleByUUID( const string& uuid );

	ROLE* newRole( const string& serial, const string& roleName, int job, int sex, int phone_type, int platform_id );

	void timerSaveRole();

	void DailyCleanAll();
	void timerDailyClean( ROLE* role, time_t now, bool bForce = false);
	void timerClearRoleCache( DWORD now );
	
	ROLE* loadRoleInfoFromDB(const string& uuid, DWORD dwRoleID=0, const string* pRoleName=NULL );
	string loadZoneInfoFromDB( ROLE* role );

	ROLE* getRoleByName(const string& roleName);
	ROLE* getCacheRoleByName(const string& roleName);

	ROLE* getRoleByID( DWORD dwRoleID );
	ROLE* getCacheRoleByID( DWORD dwRoleID );

    ROLE* findRole( DWORD dwRoleID);;

	void  getRecommondIDs(  ROLE*role, vector<DWORD> &recommondIDs );
	void onLevelUP( ROLE* role, WORD wOldLevel );

	void completeTask(ROLE* role, const WORD& wTaskID, BYTE byNums = 0);
	void judgeCompleteTask(ROLE* role, const WORD& wTaskID, ITEM_INFO* pPara = NULL );
	void judgeCompleteTypeTask(ROLE* role, const BYTE& byType,  ITEM_INFO* pPara = NULL );

	// ռ����������1
	void addOneDivineTime();
	// ռ����������5
	void addFiveDivineTime();
	// ����ռ������
	void resetDivineTime();

	void triggerAtTwelve();

	void triggerEleven();

	void triggerTwentyone();

	typedef std::function<void (RolePtr)> RoleFunction;
	void ExecAllRoles(RoleFunction func);

	void judgeCompleteTypeOpenActivity(ROLE* role, const BYTE& byType,  ITEM_INFO* pPara = NULL );

	void judgeCompleteOpenActivity(ROLE* role, const WORD& wTaskID, ITEM_INFO* pPara = NULL );

	void resetVipByLogin( ROLE* role );
	size_t get_cached_role_count();
protected:
	unordered_map<string, string> _mapRoleName2Uuids;
	unordered_map<DWORD, string> _mapRoleId2Uuids;
	unordered_map<string, RolePtr> _mapUuid2Roles;
};

#endif		//__RoleMgr_H__
