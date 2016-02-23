/*
** https://192.168.6.160/svn/art/�߻�/����һ���/�����η���1/�޸����Ż�/0714_���ս.docx
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
		DWORD time;//ս��ʱ���
		vector<TeamFightResult> vecFightEle;//ս��������
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
		vector<DWORD> getMembers();//��ԱIDs
		DWORD getFightTimeStamp();

	private:
		CJiangeTeam& operator=(const CJiangeTeam);
		CJiangeTeam(const CJiangeTeam&);

	private:
		DWORD m_dwFightTimeStamp;
		DWORD m_dwLeaderID;//�ӳ�roleID ������
		BYTE m_byMaxNum;
		E_TEAM_STATE m_byState;//״̬
		std::vector<DWORD> m_vecMembers;//��ԱIDs
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


//���
//#define		TEAM_DEFENCE_STATE				0x0830		//����״̬��Ϣ
//#define		REQUEST_TEAM_LIST				0x0832		//��������б���Ϣ
//#define		REQUEST_CREATE_TEAM				0x0833		//��������
//#define		REQUEST_APPLY_TEAM				0x0834		//����������
//#define		REQUEST_MY_TEAM_INFO			0x0835		//�����ҵĶ�����Ϣ
//#define		REQUEST_LEAVE_MY_TEAM			0x0836		//�����뿪����
//#define		KICK_OFF_TEAM_MATE				0x0837		//�ӳ�����
//#define		REQUEST_CHANGE_STRATEGY			0x0838		//�ı����
//#define		REQUEST_GO_FIGHTING				0x0839		//����ȥս��
//#define		REQUEST_FIGHTING_LOG			0x083A		//ս��������־
//#define		REQUEST_EETCH_DEFEND_AWARDS		0x083B		//��ȡ����
//#define		REQUEST_DEFEND_FIGHT_RECORD		0x083C		//ս����ʷ��¼
//#define		REQUEST_DEFEND_RANK				0x083D		//����

//#define		NOTI_MATE_ADDED					0x083E		//�ж�Ա�������
//#define		NOTI_TEAM_BREAKED				0x0840		//�����ɢ
//#define		NOTI_TEAM_MATE_STRATEGY			0x0841		//��Ա���Ա仯֪ͨ
//#define		NOTI_TEAM_FIGHTING_STATE		0x0842		//֪ͨ״̬
}