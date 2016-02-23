#pragma once
#include "typedef.h"
#include "role.h"

#include <unordered_map>


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
	
	ROLE* loadRoleInfoFromDB(const string& uuid, DWORD dwRoleID = 0, const string* pRoleName = NULL, const string* ptr_imei = NULL );
	string loadZoneInfoFromDB( ROLE* role );

	ROLE* getRoleByName(const string& roleName);
	ROLE* getCacheRoleByName(const string& roleName);

	ROLE* getRoleByID( DWORD dwRoleID );
	ROLE* getCacheRoleByID( DWORD dwRoleID );

	ROLE* getRoleByImei(const string& phone_imei);
	ROLE* getCacheRoleByImei(const string& phone_imei);

    ROLE* findRole( DWORD dwRoleID);;

	void  getRecommondIDs(  ROLE*role, vector<DWORD> &recommondIDs );
	void onLevelUP( ROLE* role, WORD wOldLevel );

	void completeTask(ROLE* role, const WORD& wTaskID, BYTE byNums = 0);
	void judgeCompleteTask(ROLE* role, const WORD& wTaskID, ITEM_INFO* pPara = NULL );
	void judgeCompleteTypeTask(ROLE* role, const BYTE& byType,  ITEM_INFO* pPara = NULL );

	// 占卜次数增加1
	void addOneDivineTime();
	// 占卜次数增加5
	void addFiveDivineTime();
	// 重置占卜次数
	void resetDivineTime();

	void triggerAtTwelve();

	void triggerEight();

	void triggerOpenGS7Day();

	void triggerTwenty();

	typedef std::function<void (RolePtr)> RoleFunction;
	void ExecAllRoles(RoleFunction func);

	void judgeCompleteTypeOpenActivity(ROLE* role, const BYTE& byType,  ITEM_INFO* pPara = NULL );

	void judgeCompleteOpenActivity(ROLE* role, const WORD& wTaskID, ITEM_INFO* pPara = NULL );

	void resetVipByLogin( ROLE* role );
	size_t get_cached_role_count();
	void save_data( bool bForce = false );
	void insert_imei_uuids( ROLE* role );
protected:
	unordered_map<string, string> _mapRoleName2Uuids;
	unordered_map<DWORD, string> _mapRoleId2Uuids;
	unordered_map<string, RolePtr> _mapUuid2Roles;
	unordered_map<string, string> _map_imei_uuids;
};

