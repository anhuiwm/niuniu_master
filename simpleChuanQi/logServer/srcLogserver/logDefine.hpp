enum E_LOG_CMD
{
	E_LOG_FIRST = 80,
	E_LOG_S_CREATE_ROLE = 81,		//create role
	E_LOG_S_LOGIN = 82,		//create role
	E_LOG_S_REQUEST_BUY_INGOT = 83,		//client charge
	E_LOG_S_PACKAGE_EXTEND = 84,		//client extend package
	E_LOG_S_SELL_ITEMS = 85,		//client sell item
	E_LOG_S_CONFIRM_STRENGTHEN = 86,		//client strength euqip
	E_LOG_S_CONNECT_PLAYER = 87,		//active
	E_LOG_S_ONLINE_NUMS = 88,		//active
	E_LOG_S_DISCONNECT_PLAYER = 89,		//disconnect
	E_LOG_LAST,
};

#pragma pack(push, 1)

//create role
//
struct LOG_S_CREATE_ROLE
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		protocal[32];		//protocal type name
	__int64		dwTime;		//operate happen time
	DWORD		dwRoleID;		//mysql role id
	char		uuid[64];		//new role cookie
	char		rolename[32];		//new role name
	LOG_S_CREATE_ROLE () { memset(this,0,sizeof(*this)); }
	LOG_S_CREATE_ROLE (const LOG_S_CREATE_ROLE& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//create role
//
struct LOG_S_LOGIN
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		protocal[32];		//protocal type name
	__int64		dwTime;		//operate happen time
	DWORD		dwRoleID;		//mysql role id
	LOG_S_LOGIN () { memset(this,0,sizeof(*this)); }
	LOG_S_LOGIN (const LOG_S_LOGIN& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//client charge
//
struct LOG_S_REQUEST_BUY_INGOT
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		protocal[32];		//protocal type name
	__int64		dwTime;		//operate happen time
	DWORD		dwRoleID;		//mysql role id
	DWORD		dwChargeNum;		//charge gold num
	LOG_S_REQUEST_BUY_INGOT () { memset(this,0,sizeof(*this)); }
	LOG_S_REQUEST_BUY_INGOT (const LOG_S_REQUEST_BUY_INGOT& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//client extend package
//
struct LOG_S_PACKAGE_EXTEND
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		protocal[32];		//protocal type name
	__int64		dwTime;		//operate happen time
	DWORD		dwRoleID;		//mysql role id
	DWORD		dwNeedCoin;		//use coin
	DWORD		dwNeedGold;		//use gold
	DWORD		dwRoleCoin;		//role coin
	DWORD		dwRoleGold;		//role gold
	BYTE		byCurEquipPkgLimit;		//role equip package max size
	LOG_S_PACKAGE_EXTEND () { memset(this,0,sizeof(*this)); }
	LOG_S_PACKAGE_EXTEND (const LOG_S_PACKAGE_EXTEND& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//client sell item
//
struct LOG_S_SELL_ITEMS
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		protocal[32];		//protocal type name
	__int64		dwTime;		//operate happen time
	DWORD		dwRoleID;		//mysql role id
	DWORD		dwGetCoin;		//get coin by sell
	DWORD		dwRoleCoin;		//role coin
	WORD		wItemID;		//sell item id
	WORD		wItemNum;		//sell item number
	LOG_S_SELL_ITEMS () { memset(this,0,sizeof(*this)); }
	LOG_S_SELL_ITEMS (const LOG_S_SELL_ITEMS& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//client strength euqip
//
struct LOG_S_CONFIRM_STRENGTHEN
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		protocal[32];		//protocal type name
	__int64		dwTime;		//operate happen time
	DWORD		dwRoleID;		//mysql role id
	DWORD		dwNeedCoin;		//use coin
	DWORD		dwRoleCoin;		//role coin
	WORD		wItemID;		// item id
	BYTE		byStrengthLevel;		//item strength level
	LOG_S_CONFIRM_STRENGTHEN () { memset(this,0,sizeof(*this)); }
	LOG_S_CONFIRM_STRENGTHEN (const LOG_S_CONFIRM_STRENGTHEN& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//active
//
struct LOG_S_CONNECT_PLAYER
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	__int64		dwTime;		//operate happen time
	char		ip[32];		//connect ip
	LOG_S_CONNECT_PLAYER () { memset(this,0,sizeof(*this)); }
	LOG_S_CONNECT_PLAYER (const LOG_S_CONNECT_PLAYER& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//active
//
struct LOG_S_ONLINE_NUMS
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	__int64		dwTime;		//operate happen time
	DWORD		num;		//online player nums
	LOG_S_ONLINE_NUMS () { memset(this,0,sizeof(*this)); }
	LOG_S_ONLINE_NUMS (const LOG_S_ONLINE_NUMS& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//disconnect
//
struct LOG_S_DISCONNECT_PLAYER
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	__int64		dwTime;		//operate happen time
	char		ip[32];		//disconnect ip
	DWORD		dwRoleID;		//mysql role id
	LOG_S_DISCONNECT_PLAYER () { memset(this,0,sizeof(*this)); }
	LOG_S_DISCONNECT_PLAYER (const LOG_S_DISCONNECT_PLAYER& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

#pragma pack(pop)
