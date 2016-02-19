#ifndef FRIEND_H
#define FRIEND_H

#include "protocal.h"
#include "config.h"

#define MAX_FRIEND_NUM			30
#define RECOMMOND_NUM			6
#define DIANZHAN_NUM			5
#pragma pack(push, 1)
class FriendInfo
{
public:
	char  szName[NAME_MAX_LEN];
	DWORD dwID;
	BYTE  bySex;
	BYTE  byJob;
	WORD  wLevel;
	DWORD dwFightValue;
	FriendInfo(){memset(this, 0, sizeof(FriendInfo));}
	FriendInfo(ROLE * role);
};

class OneFriendInfo//wm 暂留 以后改为获取人物信息界面
{
public:
	char  szName[NAME_MAX_LEN];
	DWORD dwID;
	BYTE  bySex;
	BYTE  byJob;
	WORD  wLevel;
	DWORD dwFightValue;
	BYTE  byMyFrind;
	OneFriendInfo(){memset(this, 0, sizeof(FriendInfo));}
	OneFriendInfo(ROLE * role);
};
#pragma pack(pop)

namespace FRIEND
{
	enum eFRIEND
	{
		FRIEND_SUCCESS = 0,//成功
		FRIEND_NO_EXISTS = 1,//不存在
		FRIEND_ALREADY = 2, //已经是好友
		FRIEND_NEW = 3,		//不是好友
		FRIEND_FULL = 4,	//好友已经满30个
		FRIEND_YOURSELF = 5,	//自己
		FRIEND_YIDIAN = 6,	//已经点
		ENOUGH_YIDIAN = 7,	//已满5次
	};

	void getFriendsInfo(ROLE* role, string &strData);
	void addFriend(ROLE* role, string &strData, DWORD id);
	void delFriend(ROLE* role, string &strData, DWORD id);
	void searchFriend(ROLE* role, string &strData, string name);
	void dianZhan(ROLE* role, string &strData, DWORD id);
	void recommondFriends(ROLE* role, string &strData);
	int compare(const FriendInfo &frione, const FriendInfo &fritwo);
	int dealFriends(ROLE *role, unsigned char * data, size_t dataLen  ,WORD cmd);
}



#endif