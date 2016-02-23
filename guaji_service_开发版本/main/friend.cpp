#include "pch.h"
#include "friend.h"
#include "roleCtrl.h"



FriendInfo::FriendInfo(ROLE * role)
{
	memset(this, 0, sizeof(*this));
	if (role == NULL)
	{
		return;
	}
	strncpy(this->szName, role->roleName.c_str(), sizeof(this->szName)-1);
	this->dwID =  role->dwRoleID;
	this->bySex = role->bySex;
	this->byJob = role->byJob;
	this->dwFightValue = role->dwFightValue;
	this->wLevel = role->wLevel; 
}

OneFriendInfo::OneFriendInfo(ROLE * role)
{
	memset(this, 0, sizeof(*this));
	if (role == NULL)
	{
		return;
	}
	strncpy(this->szName, role->roleName.c_str(), sizeof(this->szName)-1);
	this->dwID =  role->dwRoleID;
	this->bySex = role->bySex;
	this->byJob = role->byJob;
	this->dwFightValue = role->dwFightValue;
	this->wLevel = role->wLevel; 
}

int FRIEND::compare(const FriendInfo &frione, const FriendInfo &fritwo)//是先按战力降序，若值相等则按等级
{
	if (frione.dwFightValue > fritwo.dwFightValue)
	{
		return 1;
	}
	else if(frione.dwFightValue == fritwo.dwFightValue)
	{
		if( frione.wLevel > fritwo.wLevel )
			return 1;
		else
			return 0;
	}
	else
	{
		return 0;
	}
}
void FRIEND::getFriendsInfo(ROLE* role, string &strData)
{
	vector<FriendInfo> vevFriendInfos;
	for (size_t i=0; i < role->vecFriends.size(); i++)
	{
		ROLE* frinendRole = RoleMgr::getMe().getRoleByID( role->vecFriends[i] );
		if (frinendRole == NULL)
		{
			continue;
		}
		FriendInfo friInfo(frinendRole);
		vevFriendInfos.push_back(friInfo);
		//S_APPEND_NBYTES( strData, (char*)&friInfo, sizeof(friInfo) );
	}

	FriendInfo friInfoMe(role);
	vevFriendInfos.push_back(friInfoMe);

	sort(vevFriendInfos.begin(), vevFriendInfos.end(),compare);
	for (size_t i=0; i < vevFriendInfos.size(); i++)
	{
		//logwm("get vevFriendInfos[%d].dwID = %d",i,vevFriendInfos[i].dwID);
		S_APPEND_NBYTES( strData, (char*)&vevFriendInfos[i], sizeof(FriendInfo) );
	}
}

void FRIEND::addFriend(ROLE* role, string &strData, DWORD id)
{
	BYTE byCode;
	if (role->vecFriends.size() >= MAX_FRIEND_NUM)
	{
		byCode = FRIEND_FULL;
		S_APPEND_BYTE(strData, byCode);
		return;
	}

	if (role->dwRoleID == id)
	{
		byCode = FRIEND_YOURSELF;
		S_APPEND_BYTE(strData, byCode);
		return;
	}
	vector<DWORD>::iterator itdword = find(role->vecFriends.begin(), role->vecFriends.end(), id);
	if (itdword != role->vecFriends.end())
	{
		byCode = FRIEND_ALREADY;
		S_APPEND_BYTE(strData, byCode);
		return;
	}
	ROLE* frinendRole = RoleMgr::getMe().getRoleByID( id );
	if (frinendRole == NULL)
	{
		byCode = FRIEND_NO_EXISTS;
		S_APPEND_BYTE(strData, byCode);
		return;
	}
	//logwm("add id = %d",id);
	byCode = FRIEND_SUCCESS;
	S_APPEND_BYTE(strData, byCode);
	role->vecFriends.push_back(id);
}

void FRIEND::delFriend(ROLE* role, string &strData, DWORD id)
{
	vector<DWORD>::iterator it = find(role->vecFriends.begin(), role->vecFriends.end(), id);
	if (it != role->vecFriends.end())
	{
		role->vecFriends.erase(it);
	}
	//logwm("del id = %d",id);
}

void FRIEND::searchFriend(ROLE* role, string &strData, string name)
{
	BYTE byCode = FRIEND_SUCCESS;
	ROLE* frinendRole = RoleMgr::getMe().getRoleByName( name );
	if (frinendRole == NULL)
	{
		byCode = FRIEND_NO_EXISTS;
		S_APPEND_BYTE(strData, byCode);
		return;
	}
	//vector<DWORD>::iterator itdword = find(role->vecFriends.begin(), role->vecFriends.end(), frinendRole->dwRoleID);
	//if (itdword != role->vecFriends.end())
	//{
	//	byCode = FRIEND_ALREADY;
	//	S_APPEND_BYTE(strData, byCode);
	//}
	//else
	//{
	//	byCode = FRIEND_NEW;
	//	S_APPEND_BYTE(strData, byCode);
	//}
	
	S_APPEND_BYTE(strData, byCode);
	FriendInfo friendInfo(frinendRole);
	S_APPEND_SZ( strData, (char*)&friendInfo, sizeof(FriendInfo) );
	//logwm("search fid = %d",frinendRole->dwRoleID);
}

void FRIEND::dianZhan(ROLE* role, string &strData, DWORD id)
{
	BYTE byCode = FRIEND_SUCCESS;
	ROLE* frinendRole = RoleMgr::getMe().getRoleByID( id );
	if (frinendRole == NULL)
	{
		byCode = FRIEND_NO_EXISTS;
		S_APPEND_BYTE(strData, byCode);
		return;
	}
	if (role->vecDianZhanIDs.size() >= DIANZHAN_NUM)
	{
		byCode = ENOUGH_YIDIAN;
		S_APPEND_BYTE(strData, byCode);
		return;
	}
	for (auto tempid : role->vecDianZhanIDs)
	{
		if (tempid == id)
		{
			byCode = FRIEND_YIDIAN;
			S_APPEND_BYTE(strData, byCode);
			return;
		}
	}

	role->vecDianZhanIDs.push_back(id);
	S_APPEND_BYTE(strData, byCode);
}

void FRIEND::recommondFriends(ROLE* role, string &strData)
{
//return;

	vector<DWORD> recommondIDs;
	RoleMgr::getMe().getRecommondIDs( role, recommondIDs );
	for (size_t i=0; i < recommondIDs.size(); i++)
	{
		ROLE* frinendRole = RoleMgr::getMe().getRoleByID( recommondIDs[i] );
		if (frinendRole == NULL)
		{
			continue;
		}
		FriendInfo friInfo(frinendRole);
		S_APPEND_NBYTES( strData, (char*)&friInfo, sizeof(FriendInfo) );
		//logwm("recommond friInfo[%d].dwID = %d",i,friInfo.dwID);
	}
	//logwm("role.dwID = %d",role->dwRoleID);
}

int FRIEND::dealFriends(ROLE *role, unsigned char * data, size_t dataLen ,WORD cmd)
{
	//logwm("\n cmd:%x",cmd);
	if (role == NULL)
	{
		return 0;
	}

	if (cmd == S_GET_FRIENDS)
	{
		string strData;
		FRIEND::getFriendsInfo(role, strData);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_FRIENDS, strData) );	
	}

	if (cmd == S_ADD_FRIEND)
	{
		DWORD id;
		if ( !BASE::parseDWORD( data, dataLen, id) )
			return -1;
		string strData;
		FRIEND::addFriend(role, strData, id);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_ADD_FRIEND, strData) );	
	}

	if (cmd == S_DEL_FRIEND)
	{
		DWORD id;
		if ( !BASE::parseDWORD( data, dataLen, id) )
			return -1;
		string strData;
		FRIEND::delFriend(role, strData, id);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_DEL_FRIEND, strData) );	
	}

	if (cmd == S_SEARCH_FRIEND)
	{
		string  roleName;
		if ( !BASE::parseBSTR( data, dataLen, roleName) )
			return -1;

		string strData;
		FRIEND::searchFriend(role, strData, roleName);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SEARCH_FRIEND, strData) );	
	}

	if (cmd == S_RECOMMOND_FRIENDS)
	{
		string strData;
		FRIEND::recommondFriends(role, strData);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_RECOMMOND_FRIENDS, strData) );	
	}
	if (cmd == S_DIANZAN)
	{
		DWORD id;
		if ( !BASE::parseDWORD( data, dataLen, id) )
			return -1;
		string strData;
		FRIEND::dianZhan(role, strData, id);
		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_DIANZAN, strData) );	
	}

	return 0;
}
