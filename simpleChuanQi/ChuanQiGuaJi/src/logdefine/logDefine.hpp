enum E_LOG_CMD
{
	E_LOG_FIRST = 80,
	E_LOG_S_CREATE_ROLE = 81,		//create role
	E_LOG_S_LOGIN = 82,		//create role
	E_LOG_S_CONNECT_PLAYER = 83,		//active
	E_LOG_S_ONLINE_NUMS = 84,		//active
	E_LOG_S_DISCONNECT_PLAYER = 85,		//disconnect
	E_LOG_S_TICK_PLAYER_INFO = 86,		//tick get playerinfo
	E_LOG_LAST,
};

#pragma pack(push, 1)

//create role
//
struct LOG_S_CREATE_ROLE
{
	WORD wLen;
	WORD wCmdType;

	DWORD		eventID;		//event ID
	char		protocal[32];		//protocal type name
	DWORD		dwTime;		//operate happen time
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

	DWORD		eventID;		//event ID
	char		protocal[32];		//protocal type name
	DWORD		dwTime;		//operate happen time
	DWORD		dwRoleID;		//mysql role id
	LOG_S_LOGIN () { memset(this,0,sizeof(*this)); }
	LOG_S_LOGIN (const LOG_S_LOGIN& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//active
//
struct LOG_S_CONNECT_PLAYER
{
	WORD wLen;
	WORD wCmdType;

	DWORD		eventID;		//event ID
	DWORD		dwTime;		//operate happen time
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

	DWORD		eventID;		//event ID
	DWORD		dwTime;		//operate happen time
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

	DWORD		eventID;		//event ID
	DWORD		dwTime;		//operate happen time
	char		ip[32];		//disconnect ip
	DWORD		dwRoleID;		//mysql role id
	LOG_S_DISCONNECT_PLAYER () { memset(this,0,sizeof(*this)); }
	LOG_S_DISCONNECT_PLAYER (const LOG_S_DISCONNECT_PLAYER& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//tick get playerinfo
//
struct LOG_S_TICK_PLAYER_INFO
{
	WORD wLen;
	WORD wCmdType;

	DWORD		eventID;		//event ID
	DWORD		dwTime;		//operate happen time
	DWORD		id;		//mysql role id
	char		rolename[32];		//role name
	BYTE		job;		//job
	BYTE		sex;		//sex
	WORD		level;		//level
	DWORD		viplevel;		//viplevel
	DWORD		exp;		//exp
	DWORD		money;		//money
	DWORD		gold;		//now have gold
	DWORD		charge;		//all charge gold
	LOG_S_TICK_PLAYER_INFO () { memset(this,0,sizeof(*this)); }
	LOG_S_TICK_PLAYER_INFO (const LOG_S_TICK_PLAYER_INFO& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

#pragma pack(pop)
