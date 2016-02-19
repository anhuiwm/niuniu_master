enum E_LOG_CMD
{
	E_LOG_FIRST = 80,
	E_LOG_GETUUID = 81,		//require cookie
	E_LOG_GMCUPCOUNT = 82,		//set the cup count
	E_LOG_CUPCOUNTRYID = 83,		//set the cup cupcountryid
	E_LOG_CUPNEWSEASON = 84,		//set cup cupnewseason
	E_LOG_CUPCHALLENGE = 85,		//cup cupchallenge
	E_LOG_SELLITEM = 86,		//sell items
	E_LOG_CREATEBYTYPE = 87,		//create items by type 
	E_LOG_USEITEM = 88,		//use items
	E_LOG_MATCHFIGHT = 89,		//match fight
	E_LOG_ADDEXP = 90,		//add experience
	E_LOG_CHARGE = 91,		//charge
	E_LOG_MTREEUSE = 92,		//use money tree
	E_LOG_VISITFRIEND = 94,		//visit friend
	E_LOG_SELLCARD = 95,		//sell card
	E_LOG_ADDCARDEXP = 96,		//add card experience
	E_LOG_CARDTRAIN = 97,		//card train
	E_LOG_CARDRESET = 98,		//reset cards position
	E_LOG_UPCARDQ = 99,		//upgrade card quality
	E_LOG_EQUIP = 100,		//equip for card
	E_LOG_UPGRADEEQUIP = 101,		//upgrade equipment
	E_LOG_UPGRADESKILL = 102,		//upgrade skill
	E_LOG_RELOAD = 103,		//reload config
	E_LOG_RANDCARD = 104,		//rand cards
	E_LOG_UPGRADETAC = 106,		//upgrade tactics
	E_LOG_SWITCHFORM = 107,		//switch form
	E_LOG_SWITCHTAC = 108,		//switch tactic
	E_LOG_UNLOCKFORM = 109,		//unlock form
	E_LOG_UNLOCKTAC = 110,		//unlock tactic
	E_LOG_SWAPCARD = 111,		//swap card index
	E_LOG_SHOPEXCHANGE = 112,		//exchange items
	E_LOG_BUYMATCHCOUNT = 113,		//buy match count
	E_LOG_FIGHT = 114,		//fight scene
	E_LOG_CREATETEAM = 115,		//create team
	E_LOG_ADDFRIEND = 116,		//add friend
	E_LOG_DELFRIEND = 117,		//delete friend
	E_LOG_LAST,
};

#pragma pack(push, 1)

//require cookie
//
struct LOG_GETUUID
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	char		uuid[64];		//cookie
	LOG_GETUUID () { memset(this,0,sizeof(*this)); }
	LOG_GETUUID (const LOG_GETUUID& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//set the cup count
//
struct LOG_GMCUPCOUNT
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	BYTE		count;		//number of match
	LOG_GMCUPCOUNT () { memset(this,0,sizeof(*this)); }
	LOG_GMCUPCOUNT (const LOG_GMCUPCOUNT& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//set the cup cupcountryid
//
struct LOG_CUPCOUNTRYID
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	BYTE		byCup;		//country id
	LOG_CUPCOUNTRYID () { memset(this,0,sizeof(*this)); }
	LOG_CUPCOUNTRYID (const LOG_CUPCOUNTRYID& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//set cup cupnewseason
//
struct LOG_CUPNEWSEASON
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	BYTE		byUp;		//upgrade or not
	BYTE		byLevel;		//level of the cup
	LOG_CUPNEWSEASON () { memset(this,0,sizeof(*this)); }
	LOG_CUPNEWSEASON (const LOG_CUPNEWSEASON& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//cup cupchallenge
//
struct LOG_CUPCHALLENGE
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	BYTE		byCupDayCount;		//Cup daily Count
	BYTE		byCupCount;		//match counts of the cup season
	LOG_CUPCHALLENGE () { memset(this,0,sizeof(*this)); }
	LOG_CUPCHALLENGE (const LOG_CUPCHALLENGE& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//sell items
//
struct LOG_SELLITEM
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		sellPriceSum;		//the total money to sell items
	LOG_SELLITEM () { memset(this,0,sizeof(*this)); }
	LOG_SELLITEM (const LOG_SELLITEM& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//create items by type 
//
struct LOG_CREATEBYTYPE
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	BYTE		boxItemType;		//boxItemType
	WORD		wItemID;		//wItemID
	DWORD		dwNums;		//dwNums
	LOG_CREATEBYTYPE () { memset(this,0,sizeof(*this)); }
	LOG_CREATEBYTYPE (const LOG_CREATEBYTYPE& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//use items
//
struct LOG_USEITEM
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		type;		//type of item
	LOG_USEITEM () { memset(this,0,sizeof(*this)); }
	LOG_USEITEM (const LOG_USEITEM& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//match fight
//
struct LOG_MATCHFIGHT
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwMatchCount;		//Match Count
	BYTE		byUsedChallengeCount;		//Used Challenge Count
	LOG_MATCHFIGHT () { memset(this,0,sizeof(*this)); }
	LOG_MATCHFIGHT (const LOG_MATCHFIGHT& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//add experience
//
struct LOG_ADDEXP
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	WORD		wLevel;		//Level after add experience
	DWORD		dwExp;		//experience after
	DWORD		dwAddExp;		//value of increased experience
	LOG_ADDEXP () { memset(this,0,sizeof(*this)); }
	LOG_ADDEXP (const LOG_ADDEXP& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//charge
//
struct LOG_CHARGE
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwGold;		//number of gold after charging
	DWORD		dwCharge;		//accumulative of charged values
	DWORD		dwChargeNum;		//current charge values
	LOG_CHARGE () { memset(this,0,sizeof(*this)); }
	LOG_CHARGE (const LOG_CHARGE& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//use money tree
//
struct LOG_MTREEUSE
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwGold;		//number of dwgold after excharge
	DWORD		dwMoney;		//number of dwMoney after exchange
	BYTE		byUsedMoneyTreeTimes;		//exchange times
	LOG_MTREEUSE () { memset(this,0,sizeof(*this)); }
	LOG_MTREEUSE (const LOG_MTREEUSE& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//visit friend
//
struct LOG_VISITFRIEND
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		friendID;		//friend ID
	DWORD		dwFriendPoints;		//Friend Points
	DWORD		dwHP;		//strength
	LOG_VISITFRIEND () { memset(this,0,sizeof(*this)); }
	LOG_VISITFRIEND (const LOG_VISITFRIEND& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//sell card
//
struct LOG_SELLCARD
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	WORD		wChairID;		//client chair ID
	WORD		wUniqID;		//UniqID
	DWORD		sellPriceSum;		//Price
	LOG_SELLCARD () { memset(this,0,sizeof(*this)); }
	LOG_SELLCARD (const LOG_SELLCARD& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//add card experience
//
struct LOG_ADDCARDEXP
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	WORD		wChairID;		//client chair ID
	WORD		wUniqID;		//UniqID
	DWORD		dwExpGet;		//experience after add
	LOG_ADDCARDEXP () { memset(this,0,sizeof(*this)); }
	LOG_ADDCARDEXP (const LOG_ADDCARDEXP& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//card train
//
struct LOG_CARDTRAIN
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwMoney;		//role money after train
	WORD		wChairID;		//client chair ID
	WORD		wUniqID;		//UniqID
	DWORD		dwAddExp;		//value of experience that food card can exchange
	WORD		wLevel;		//main card Level
	LOG_CARDTRAIN () { memset(this,0,sizeof(*this)); }
	LOG_CARDTRAIN (const LOG_CARDTRAIN& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//reset cards position
//
struct LOG_CARDRESET
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	WORD		wChairID1;		//client chair ID1
	WORD		wUniqID1;		//UniqID
	WORD		wChairID2;		//client chair ID2
	WORD		wUniqID2;		//UniqID
	WORD		wChairID3;		//client chair ID3
	WORD		wUniqID3;		//UniqID
	WORD		wChairID4;		//client chair ID4
	WORD		wUniqID4;		//UniqID
	WORD		wChairID5;		//client chair ID5
	WORD		wUniqID5;		//UniqID
	WORD		wChairID6;		//client chair ID6
	WORD		wUniqID6;		//UniqID
	WORD		wChairID7;		//client chair ID7
	WORD		wUniqID7;		//UniqID
	WORD		wChairID8;		//client chair ID8
	WORD		wUniqID8;		//UniqID
	WORD		wChairID9;		//client chair ID9
	WORD		wUniqID9;		//UniqID
	WORD		wChairID10;		//client chair ID10
	WORD		wUniqID10;		//UniqID
	WORD		wChairID11;		//client chair ID11
	WORD		wUniqID11;		//UniqID
	LOG_CARDRESET () { memset(this,0,sizeof(*this)); }
	LOG_CARDRESET (const LOG_CARDRESET& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//upgrade card quality
//
struct LOG_UPCARDQ
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	WORD		wChairID;		//client chair ID
	WORD		wUniqID;		//UniqID
	BYTE		byQuality;		//upgraded card quality
	LOG_UPCARDQ () { memset(this,0,sizeof(*this)); }
	LOG_UPCARDQ (const LOG_UPCARDQ& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//equip for card
//
struct LOG_EQUIP
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	WORD		wChairID;		//client chair ID
	WORD		wUniqID;		//UniqID
	WORD		wEquipItemID;		//Equip Item ID
	LOG_EQUIP () { memset(this,0,sizeof(*this)); }
	LOG_EQUIP (const LOG_EQUIP& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//upgrade equipment
//
struct LOG_UPGRADEEQUIP
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	WORD		wUniqID;		// Equip UniqID
	WORD		wEquipItemID;		//Equip Item ID
	BYTE		byLevel;		//level of equipment after upgrade
	LOG_UPGRADEEQUIP () { memset(this,0,sizeof(*this)); }
	LOG_UPGRADEEQUIP (const LOG_UPGRADEEQUIP& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//upgrade skill
//
struct LOG_UPGRADESKILL
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwMoney;		//role money
	WORD		wChairID;		//client chair ID
	WORD		wUniqID;		//UniqID
	BYTE		bySkillID;		//skill ID
	BYTE		byLevel;		//level of skill after upgrade
	LOG_UPGRADESKILL () { memset(this,0,sizeof(*this)); }
	LOG_UPGRADESKILL (const LOG_UPGRADESKILL& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//reload config
//
struct LOG_RELOAD
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	LOG_RELOAD () { memset(this,0,sizeof(*this)); }
	LOG_RELOAD (const LOG_RELOAD& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//rand cards
//
struct LOG_RANDCARD
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwDropid;		//dropid in drop.ini
	DWORD		dwGold;		//role gold
	LOG_RANDCARD () { memset(this,0,sizeof(*this)); }
	LOG_RANDCARD (const LOG_RANDCARD& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//upgrade tactics
//
struct LOG_UPGRADETAC
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwTid;		//tactic ID
	DWORD		dwToLevel;		//the tactic upgraded level
	LOG_UPGRADETAC () { memset(this,0,sizeof(*this)); }
	LOG_UPGRADETAC (const LOG_UPGRADETAC& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//switch form
//
struct LOG_SWITCHFORM
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwTeamFightValue;		//TeamFightValue after switch
	BYTE		byCurFormID;		//form ID
	LOG_SWITCHFORM () { memset(this,0,sizeof(*this)); }
	LOG_SWITCHFORM (const LOG_SWITCHFORM& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//switch tactic
//
struct LOG_SWITCHTAC
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwTeamFightValue;		//TeamFightValue after switch
	BYTE		byCurTactics;		//tactic ID
	LOG_SWITCHTAC () { memset(this,0,sizeof(*this)); }
	LOG_SWITCHTAC (const LOG_SWITCHTAC& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//unlock form
//
struct LOG_UNLOCKFORM
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwFormID;		//the unlock form ID
	DWORD		dwGold;		//role gold after unlock
	LOG_UNLOCKFORM () { memset(this,0,sizeof(*this)); }
	LOG_UNLOCKFORM (const LOG_UNLOCKFORM& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//unlock tactic
//
struct LOG_UNLOCKTAC
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwTacID;		//the unlock tactic ID
	DWORD		dwGold;		//role gold after unlock
	LOG_UNLOCKTAC () { memset(this,0,sizeof(*this)); }
	LOG_UNLOCKTAC (const LOG_UNLOCKTAC& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//swap card index
//
struct LOG_SWAPCARD
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwCardup;		//the card id go up
	DWORD		dwCarddown;		//the card id go down
	LOG_SWAPCARD () { memset(this,0,sizeof(*this)); }
	LOG_SWAPCARD (const LOG_SWAPCARD& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//exchange items
//
struct LOG_SHOPEXCHANGE
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwPricetype;		//currency
	DWORD		dwPriceNum;		//price
	LOG_SHOPEXCHANGE () { memset(this,0,sizeof(*this)); }
	LOG_SHOPEXCHANGE (const LOG_SHOPEXCHANGE& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//buy match count
//
struct LOG_BUYMATCHCOUNT
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	BYTE		byBuyChallengeCount;		//buy challenge count
	BYTE		byUsedChallengeCount;		//used challenge count
	WORD		wBuyMatchCountCost;		//next buy cost
	LOG_BUYMATCHCOUNT () { memset(this,0,sizeof(*this)); }
	LOG_BUYMATCHCOUNT (const LOG_BUYMATCHCOUNT& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//fight scene
//
struct LOG_FIGHT
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwSceneID;		//scene ID
	LOG_FIGHT () { memset(this,0,sizeof(*this)); }
	LOG_FIGHT (const LOG_FIGHT& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//create team
//
struct LOG_CREATETEAM
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwTeamID;		//team ID
	LOG_CREATETEAM () { memset(this,0,sizeof(*this)); }
	LOG_CREATETEAM (const LOG_CREATETEAM& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//add friend
//
struct LOG_ADDFRIEND
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwFriendID;		//friend ID
	LOG_ADDFRIEND () { memset(this,0,sizeof(*this)); }
	LOG_ADDFRIEND (const LOG_ADDFRIEND& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

//delete friend
//
struct LOG_DELFRIEND
{
	WORD wLen;
	WORD wCmdType;

	__int64		eventID;		//event ID
	char		cmdName[32];		//GM commend
	DWORD		dwTime;		//happen time
	DWORD		dwRoleID;		//role id
	DWORD		dwFriendID;		//friend ID
	LOG_DELFRIEND () { memset(this,0,sizeof(*this)); }
	LOG_DELFRIEND (const LOG_DELFRIEND& rhs) { memcpy(this,&rhs,sizeof(*this)); }
};
//==================================================================

#pragma pack(pop)
