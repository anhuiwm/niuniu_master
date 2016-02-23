/*
** https://192.168.6.160/svn/art/策划/传奇挂机版/中手游方案1/修改与优化/0714_组队战.docx
** author: wm
** date  : 20150717
*/

#pragma  once
#include"config.h"
#include <list>
#include <unordered_map>
#define SINGJIANGE() ( jiange_team::CJiangeTeamManager::getJiangeManageInstance() )

namespace jiange_team
{
	enum class E_TEAM_STATE : BYTE
	{
		E_PREPARE = 1,
		E_FIGHT = 2,
	};

	//enum class E_ACTIVITY_STATE : BYTE
	//{
	//	E_UNOPEN = 1,
	//	E_ON = 2,
	//	E_END = 3,
	//};

	enum class E_MONSTER_ATTR : BYTE
	{
		E_FIGHT_VALUE,
		E_DROP_ODDS,
		E_DROP_ITEM,
		E_DROP_MONEY,
		E_DROP_EXP,
	};

	struct TeamFightProcess 
	{
		DWORD time;//战斗时间戳
		vector<TeamFightResult> vecFightEle;//战斗波序列
		TeamFightProcess(DWORD time, vector<TeamFightResult>& vFight)
		{
			this->time = time;
			this->vecFightEle = vFight;
		}
	};

	struct TeamFightResultRank
	{
		WORD rounds;
		vector<DWORD> roles;
		TeamFightResultRank(WORD rounds, const vector<DWORD>& roles)
		{
			this->rounds = rounds;
			this->roles = roles;
		}
	};

	class  CJiangeTeam
	{
	public:
		CJiangeTeam(DWORD leadID) :m_dwLeaderID(leadID), m_byMaxNum(3){};
		void setState(E_TEAM_STATE state);
		E_TEAM_STATE getState();
		BYTE joinTeam(DWORD roleID);
		BYTE getMemberNum();
		//BYTE leaveTeam(DWORD roleID);
		DWORD getTeamID();
		bool isLeader(DWORD roleID);
		BYTE leaderLeave();
		BYTE generalLeave(DWORD roleID);
		void forEachMember(std::function<void(const DWORD dwRoleID)> func);
		void notifyTeam(WORD wProtocal, string& strData);
		void notifyPrepareFight();
		void notifyStartFight();
		void notifyJoin(DWORD role);
		void notifyLeave(DWORD role);
		void notifyBreakUp();
		void startTeamFight();
		void endTeamFight(vector<ROLE*>& vecRoles, vector<TeamFightProcess> vecFightProcess);
		void prepareStartFight();
		DWORD getTeamFightvalue(vector<ROLE*> vecRoles);
		DWORD getMonsterFightvalue(WORD monsterID, E_MONSTER_ATTR attrType);
		vector<DWORD> getMembers();//队员IDs
		DWORD getFightTimeStamp();

	private:
		CJiangeTeam& operator=(const CJiangeTeam);
		CJiangeTeam(const CJiangeTeam&);

	private:
		DWORD m_dwFightTimeStamp;
		DWORD m_dwLeaderID;//队长roleID 队伍编号
		BYTE m_byMaxNum;
		E_TEAM_STATE m_byState;//状态
		std::vector<DWORD> m_vecMembers;//队员IDs
	};

	
	class CJiangeTeamManager
	{
	public:

		void activityStart();
		void activityEnd();
		BYTE newTeam(ROLE* role);
		BYTE joinTeam(ROLE* role, DWORD teamID);
		BYTE leaveTeam(ROLE* role, DWORD teamID);
		BYTE leaderLeave(CJiangeTeam* pTeam);
		WORD getTeamNum();
		CJiangeTeam* getTeam(DWORD teamID);
		static CJiangeTeamManager* getJiangeManageInstance();
		void forEachTeam(std::function<void( const pair<DWORD,CJiangeTeam*>& x)> func);
		void startTeamFight(DWORD teamID);
		void endTeamFight(CJiangeTeam* pTeam);
		void insertRank(WORD rounds, const vector<DWORD>& roles);
		static int getBuffCost(BYTE buff);
		static BYTE getActivityState();
	public:
		const static DWORD g_dwCreate_Cost = 20;
		const static WORD g_wLimitLevel = 15;
		const static WORD g_wLimitTeamNum = 50;
		static list<TeamFightResultRank> g_listTeamFightRank;

	private:
		CJiangeTeamManager(){};
		virtual ~CJiangeTeamManager(){};
		CJiangeTeamManager& operator=(const CJiangeTeamManager);
		CJiangeTeamManager(const CJiangeTeamManager&);

	private:
		static CJiangeTeamManager* g_pJiangeManage;
		std::unordered_map<DWORD, CJiangeTeam*> m_mapPrepareTeams;
	};

	/*----------------------NET----------------------------*/
	DECLARE_CLIENT_FUNC(getJiangeState);
	DECLARE_CLIENT_FUNC(getJiangeTeams);
	DECLARE_CLIENT_FUNC(createJiangeTeam);
	DECLARE_CLIENT_FUNC(joinJiangeTeam);
	DECLARE_CLIENT_FUNC(getMyJiangeTeam);
	DECLARE_CLIENT_FUNC(leaveJiangeTeam);
	DECLARE_CLIENT_FUNC(leaderKick);
	DECLARE_CLIENT_FUNC(setBuff);
	DECLARE_CLIENT_FUNC(leaderStartfight);
	DECLARE_CLIENT_FUNC(getJiangeFightProcess);
	DECLARE_CLIENT_FUNC(getReward);
	DECLARE_CLIENT_FUNC(getJiangeFightHistory);
	DECLARE_CLIENT_FUNC(getJiangeFightRank);


//组队
//#define		TEAM_DEFENCE_STATE				0x0830		//请求活动状态信息
//#define		REQUEST_TEAM_LIST				0x0832		//请求队伍列表信息
//#define		REQUEST_CREATE_TEAM				0x0833		//创建队伍
//#define		REQUEST_APPLY_TEAM				0x0834		//请求加入队伍
//#define		REQUEST_MY_TEAM_INFO			0x0835		//请求我的队伍信息
//#define		REQUEST_LEAVE_MY_TEAM			0x0836		//请求离开队伍
//#define		KICK_OFF_TEAM_MATE				0x0837		//队长踢人
//#define		REQUEST_CHANGE_STRATEGY			0x0838		//改变策略
//#define		REQUEST_GO_FIGHTING				0x0839		//请求去战斗
//#define		REQUEST_FIGHTING_LOG			0x083A		//战斗过程日志
//#define		REQUEST_EETCH_DEFEND_AWARDS		0x083B		//领取奖励
//#define		REQUEST_DEFEND_FIGHT_RECORD		0x083C		//战斗历史记录
//#define		REQUEST_DEFEND_RANK				0x083D		//排行

//#define		NOTI_MATE_ADDED					0x083E		//有队员加入队伍
//#define		NOTI_TEAM_BREAKED				0x0840		//队伍解散
//#define		NOTI_TEAM_MATE_STRATEGY			0x0841		//队员策略变化通知
//#define		NOTI_TEAM_FIGHTING_STATE		0x0842		//通知状态
}