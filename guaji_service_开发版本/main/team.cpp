#include "pch.h"
#include "team.h"
#include "role.h"
#include "roleCtrl.h"
#include "factionCtrl.h"
#include "TimerMgr.h"
#include "itemCtrl.h"
#include "mailCtrl.h"

#define  ACT_TEAM_FIGHT_ID  4
#define  MAX_TEAM_RANK_NUM  30
#define  NOTFULL(list)     (list.size() < MAX_TEAM_RANK_NUM)

namespace jiange_team
{
	const static int g_base = 1;
	const static WORD g_baseMonster = 1;
	list<TeamFightResultRank> CJiangeTeamManager::g_listTeamFightRank;


	enum E_RETURN_STATE : BYTE
	{
		E_SUCCESS = 0,
		E_NO_ROLE = 1,
		E_NO_TEAM = 2,
		E_FULL    = 3,
		E_CREATED = 4,
		E_FIGHTED = 5,
		E_JOINED  = 6,
		E_NO_MEMBER=7,
		E_NO_GOLD  =8,
		E_NO_OPEN  =9,
		E_END	   =10,
		E_ERROR,
	};

	enum E_BUFF_STATE : BYTE
	{
		E_FIGHT = 0,//战力
		E_REWARD = 1,//掉宝
		E_DROP = 2,//经验金币
	};

	/*********************************** Team **************************************/

	void CJiangeTeam::setState(E_TEAM_STATE state)
	{
		m_byState = state;
	}

	E_TEAM_STATE CJiangeTeam::getState()
	{
		return m_byState;
	}

	BYTE CJiangeTeam::getMemberNum()
	{
		return m_vecMembers.size();
	}

	BYTE CJiangeTeam::joinTeam(DWORD roleID)
	{
		//RETURN_COND(getState() == E_TEAM_STATE::E_FIGHT, E_FIGHTED);
		RETURN_COND(getMemberNum() >= m_byMaxNum, E_FULL);
		if (std::find(m_vecMembers.begin(),m_vecMembers.end(),roleID) != m_vecMembers.end())
		{
			return E_JOINED;
		}
		m_vecMembers.push_back(roleID);

		if (getMemberNum() == m_byMaxNum)
		{
			prepareStartFight();
		}

		notifyJoin(roleID);

		return E_SUCCESS;
	}

	DWORD CJiangeTeam::getTeamID()
	{
		return m_dwLeaderID;
	}
	bool CJiangeTeam::isLeader(DWORD roleID)
	{
		return getTeamID() == roleID;
	}

	BYTE CJiangeTeam::leaderLeave()
	{

		forEachMember([](const DWORD dwRoleID){
			ROLE* role = RoleMgr::getMe().getRoleByID(dwRoleID);
			if (role != nullptr)
			{
				role->leaveTeam();
			}
		});

		notifyBreakUp();

		timer_mgr::getInstance().del_timer(getTeamID());
		return E_SUCCESS;
	}

	BYTE CJiangeTeam::generalLeave(DWORD roleID)
	{
		auto it = std::find(m_vecMembers.begin(), m_vecMembers.end(), roleID);
		if (it != m_vecMembers.end())
		{
			notifyLeave(roleID);
			ROLE* role = RoleMgr::getMe().getRoleByID(roleID);
			if (role != nullptr)
			{
				role->leaveTeam();
			}

			m_vecMembers.erase(it);

			timer_mgr::getInstance().del_timer(getTeamID());

			return E_SUCCESS;
		}
		return E_NO_MEMBER;
	}

	void CJiangeTeam::notifyPrepareFight()
	{
		//NOTI_TEAM_FIGHTING_STATE	0x0842 --通知状态
		//S : [fight_state(战斗状态):byte]
		//fight_state : 1战斗倒计时开始
		//		  fight_state : 2战斗开始
		string strData;
		S_APPEND_BYTE(strData, 1);
		WORD wProtocal = NOTI_TEAM_FIGHTING_STATE;
		notifyTeam(wProtocal, strData);
	}

	void CJiangeTeam::notifyStartFight()
	{
		string strData;
		S_APPEND_BYTE(strData, 2);
		WORD wProtocal = NOTI_TEAM_FIGHTING_STATE;
		notifyTeam(wProtocal, strData);
	}

	void CJiangeTeam::prepareStartFight()
	{
		m_dwFightTimeStamp = time(nullptr) + 10;
		timer_mgr::getInstance().add_timer(10 * 1000, m_dwLeaderID);
		notifyPrepareFight();
	}

	void CJiangeTeam::notifyJoin(DWORD role)
	{
		//NOTI_MATE_ADDED	0x083E --有队员加入队伍
		//S:[team_mate_info]

		string strData;
		S_APPEND_DWORD(strData, role);
		WORD wProtocal = NOTI_MATE_ADDED;
		notifyTeam(wProtocal, strData);
	}

	void CJiangeTeam::notifyLeave(DWORD role)//踢人或者离开
	{
		//NOTI_MATE_KICKED_OFF	0x083F --有队员被踢出
		//S : [player_id(玩家ID):dword]

		string strData;
		S_APPEND_DWORD(strData, role);
		WORD wProtocal = NOTI_MATE_KICKED_OFF;
		notifyTeam(wProtocal, strData);
	}

	void CJiangeTeam::notifyBreakUp()
	{
		//NOTI_TEAM_BREAKED	0x0840 --队伍解散
		//S : []
		string strData;
		WORD wProtocal = NOTI_TEAM_BREAKED;
		notifyTeam(wProtocal, strData);
	}

	vector<DWORD> CJiangeTeam::getMembers()
	{
		return m_vecMembers;
	}

	DWORD CJiangeTeam::getFightTimeStamp()
	{
		return m_dwFightTimeStamp;
	}

	DWORD CJiangeTeam::getTeamFightvalue(vector<ROLE*> vecRoles)
	{
		DWORD value = 0;
		forEachMember([&](const DWORD dwRoleID){
			ROLE* role = RoleMgr::getMe().getRoleByID(dwRoleID);
			RETURN_VOID(role == nullptr);
			int roleValue = int(role->dwFightValue);
			roleValue = int( roleValue * 0.1f * (g_base<<E_FIGHT && role->teamBuff) );
			roleValue += int( roleValue * (BASE::randBetween(-10, 10)/100.0f) );
			role->teamFightValue = roleValue;
			ADDUP(value, roleValue);
			vecRoles.push_back(role);
		}
		);
		return value;
	}

	DWORD CJiangeTeam::getMonsterFightvalue(WORD monsterID, E_MONSTER_ATTR attrType)
	{
		DWORD value = 0;
		CONFIG::TD_MONSTER_CFG* monCfg = CONFIG::getTdMonsterCfg(monsterID);
		RETURN_COND(monCfg == nullptr, 0);
		if (attrType == E_MONSTER_ATTR::E_FIGHT_VALUE)
		{
			value = monCfg->power;
			value += int(value * (BASE::randBetween(-5, 5) / 100.0f));
		}
		else if (attrType == E_MONSTER_ATTR::E_DROP_ODDS)
		{
			value = DWORD(monCfg->item_odds*100.0f);
		}
		else if (attrType == E_MONSTER_ATTR::E_DROP_ITEM)
		{
			value = monCfg->item;
		}
		else if (attrType == E_MONSTER_ATTR::E_DROP_MONEY)
		{
			value = monCfg->money;
		}
		else if (attrType == E_MONSTER_ATTR::E_DROP_EXP)
		{
			value = monCfg->exp;
		}
		return value;
	}

	void CJiangeTeam::startTeamFight()
	{
		//服务器战斗过程
		WORD monsterID = g_baseMonster;
		vector<ROLE*> vecRoles;
		DWORD dwTeamFightValue = getTeamFightvalue(vecRoles);
		RETURN_VOID(vecRoles.empty());
		DWORD dwMonsterfightValue = getMonsterFightvalue(monsterID, E_MONSTER_ATTR::E_FIGHT_VALUE);
		DWORD now = time(nullptr);
		vector<DWORD> vecDropRoles;
		vector<DWORD> vecNoDropRoles;

		vector<TeamFightResult> vecFight;
		vector<TeamFightProcess> vecFightProcess;

		while (dwMonsterfightValue != 0 && dwTeamFightValue >= dwMonsterfightValue)
		{
			//每一波
			CONFIG::TD_MONSTER_CFG* monCfg = CONFIG::getTdMonsterCfg(monsterID);
			RETURN_VOID(monCfg == nullptr);
			FLOAT odds = monCfg->item_odds;
			WORD  item = monCfg->item;
			DWORD money = monCfg->money;
			DWORD exp = monCfg->exp;
			DWORD kill = vecRoles[0]->dwRoleID;
			for (auto it : vecRoles)
			{
				TeamFightResult fight;
				fight.role = it->dwRoleID;
				fight.hurt = DWORD(it->teamFightValue / dwTeamFightValue * dwMonsterfightValue * 10.0f);
				fight.coin = DWORD(money * (1.0f + 0.3f * (g_base << E_DROP && it->teamBuff)));
				fight.exp = DWORD(exp * (1.0f + 0.3f * (g_base << E_DROP && it->teamBuff)));
				odds += odds * 0.5f * (g_base << E_REWARD && it->teamBuff);
				if ( RAND_HIT(odds) )
				{
					fight.item = item;
					vecDropRoles.push_back(fight.role);
				}
				else
				{
					vecNoDropRoles.push_back(fight.role);
				}


				if (!vecDropRoles.empty())
				{
					auto index = BASE::randTo(vecDropRoles.size());
					kill = vecDropRoles[index];
				}
				else if (!vecNoDropRoles.empty())
				{
					auto index = BASE::randTo(vecNoDropRoles.size());
					kill = vecDropRoles[index];
				}

				auto itr = find(vecFight.begin(), vecFight.end(), kill);
				if (itr != vecFight.end() )
				{
					itr->kill = 1;
				}
				vecFight.push_back(fight);
			}
			
			vecFightProcess.push_back(TeamFightProcess(now, vecFight));
			monsterID++;
			now += 5;
			dwMonsterfightValue = getMonsterFightvalue(monsterID, E_MONSTER_ATTR::E_FIGHT_VALUE);
		}

		endTeamFight(vecRoles, vecFightProcess);
	}
	void CJiangeTeam::endTeamFight(vector<ROLE*>& vecRoles, vector<TeamFightProcess> vecFightProcess)
	{
		DWORD now = time(nullptr);
		//服务器战斗结束
		notifyStartFight();
		for (auto role : vecRoles)
		{
			role->teamID = 0;
			role->jiangeState = E_TASK_COMPLETE_CAN_GET_PRIZE;
			role->vecTeamFightProcess = vecFightProcess;
			role->listTeamFightRecord.push_back(TeamFightRecord(now, vecFightProcess.size()));
			if (role->listTeamFightRecord.size() > 7)
			{
				role->listTeamFightRecord.pop_front();
			}
		}
		SINGJIANGE()->insertRank(vecFightProcess.size(), this->m_vecMembers);
	}

	void CJiangeTeam::notifyTeam(WORD wProtocal, string& strData)
	{
		forEachMember([&](const DWORD dwRoleID){
			ROLE* role = RoleMgr::getMe().getCacheRoleByID(dwRoleID);
			RETURN_VOID(role == nullptr);
			PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(wProtocal, strData));
		}
		);
	}

	void CJiangeTeam::forEachMember(std::function<void(const DWORD dwRoleID)> func)
	{
		std::for_each(m_vecMembers.begin(), m_vecMembers.end(), func);
	}

	//BYTE CJiangeTeam::leaveTeam(DWORD roleID)
	//{
	//	BYTE res;
	//	if ( isLeader(roleID) )
	//	{
	//		res =  leaderLeave();
	//		if (E_SUCCESS == res)
	//		{
	//			notifyBreakUp();
	//		}
	//	}
	//	else
	//	{
	//		res = generalLeave(roleID);
	//		if (E_SUCCESS == res)
	//		{
	//			notifyLeave(roleID);
	//			ROLE* role = RoleMgr::getMe().getRoleByID(roleID);
	//			if (role != nullptr)
	//			{
	//				role->leaveTeam();
	//			}
	//		}
	//	}

	//	if (E_SUCCESS == res)
	//	{
	//		timer_mgr::getInstance().del_timer(getTeamID());
	//	}
	//}
	
	/*********************************** Team Maneger **************************************/
	CJiangeTeamManager* CJiangeTeamManager::g_pJiangeManage = nullptr;


	CJiangeTeamManager* CJiangeTeamManager::getJiangeManageInstance()
	{
		if (g_pJiangeManage == nullptr)
		{
			g_pJiangeManage = new CJiangeTeamManager();
		}
		return g_pJiangeManage;
	}

	int CJiangeTeamManager::getBuffCost(BYTE buff)
	{
		//E_FIGHT = 0,//战力 30 10%
		//E_REWARD = 1,//掉宝  60 50%
		//E_DROP = 2,//经验金币 90  30%
		return (buff&(g_base << E_FIGHT)) * 30 + (buff&(g_base << E_REWARD)) * 60 + (buff&(g_base << E_DROP)) * 90;
	}

	BYTE CJiangeTeamManager::getActivityState()
	{
		CONFIG::ACTIVITY_INFO_CFG* actCfg = CONFIG::getActivityInfoCfg(ACT_TEAM_FIGHT_ID);
		RETURN_COND(actCfg == nullptr, E_NO_OPEN);
		DWORD now = time(nullptr);
		RETURN_COND(now < actCfg->advance_time || now >actCfg->end_time, E_NO_OPEN);
		return E_SUCCESS;
	}

	BYTE CJiangeTeamManager::newTeam(ROLE* role)
	{
		RETURN_COND(role == nullptr, E_NO_ROLE);
		RETURN_COND(getActivityState() != E_SUCCESS, E_NO_OPEN);
		RETURN_COND(role->jiangeState != E_TASK_UNCOMPLETE, E_JOINED);

		RETURN_COND(role->hasEnoughGold(g_dwCreate_Cost), E_NO_GOLD);
		DWORD dwLeaderID = role->dwRoleID;
		RETURN_COND(this->getTeam(dwLeaderID) != nullptr, E_CREATED);
		RETURN_COND(this->getTeamNum() >= g_wLimitTeamNum, E_FULL);
		CJiangeTeam* pTeam = new CJiangeTeam(dwLeaderID);
		RETURN_COND(pTeam == nullptr, E_ERROR);
		m_mapPrepareTeams.insert(make_pair(dwLeaderID, pTeam));
		pTeam->setState( E_TEAM_STATE::E_PREPARE);
		BYTE res = pTeam->joinTeam(dwLeaderID);

		if (res == E_SUCCESS)
		{
			role->teamID = pTeam->getTeamID();
			role->vecTeamRoleIDs = pTeam->getMembers();
			role->subGold(g_dwCreate_Cost, true);
		}
		return 0;
	}

	BYTE CJiangeTeamManager::joinTeam(ROLE* role, DWORD teamID)
	{
		RETURN_COND(role == nullptr, E_NO_ROLE);
		RETURN_COND(getActivityState() != E_SUCCESS, E_NO_OPEN);
		RETURN_COND(role->jiangeState != E_TASK_UNCOMPLETE, E_JOINED);

		CJiangeTeam* pTeam = getTeam(teamID);
		RETURN_COND(pTeam == nullptr, E_NO_TEAM);
		BYTE res = pTeam->joinTeam(role->dwRoleID);

		if (res == E_SUCCESS)
		{
			role->teamID = pTeam->getTeamID();
			role->vecTeamRoleIDs = pTeam->getMembers();
		}
		return res;
	}

	BYTE CJiangeTeamManager::leaveTeam(ROLE* role, DWORD teamID)
	{
		RETURN_COND(role == nullptr, E_NO_ROLE);
		CJiangeTeam* pTeam = getTeam(teamID);
		RETURN_COND(pTeam == nullptr, E_NO_TEAM);
		if ( pTeam->isLeader(teamID) )
		{
			return leaderLeave(pTeam);
		}
		else
		{
			return pTeam->generalLeave(role->dwRoleID);
		}
	}

	BYTE CJiangeTeamManager::leaderLeave(CJiangeTeam* pTeam)
	{
		pTeam->leaderLeave();
		DWORD teamID = pTeam->getTeamID();
		CC_SAFE_DELETE(pTeam);
		m_mapPrepareTeams.erase(teamID);
		return E_SUCCESS;
	}

	CJiangeTeam* CJiangeTeamManager::getTeam(DWORD teamID)
	{
		auto itr = m_mapPrepareTeams.find(teamID);
		if (itr != m_mapPrepareTeams.end())
		{
			return itr->second;
		}
		return nullptr;
	}

	WORD CJiangeTeamManager::getTeamNum()
	{
		return  m_mapPrepareTeams.size();
	}

	void CJiangeTeamManager::forEachTeam(std::function<void(const pair<DWORD, CJiangeTeam*>& x)> func)
	{
		std::for_each(m_mapPrepareTeams.begin(), m_mapPrepareTeams.end(), func);
	}

	void CJiangeTeamManager::insertRank(WORD rounds, const vector<DWORD>& roles)
	{
		if (NOTFULL(g_listTeamFightRank))
		{
			g_listTeamFightRank.push_back(TeamFightResultRank(rounds, roles));
			goto end;
		}


		for (auto it = g_listTeamFightRank.begin(); it != g_listTeamFightRank.end(); ++it)
		{
			if (rounds > it->rounds)
			{
				g_listTeamFightRank.insert(it, TeamFightResultRank(rounds, roles));
				if (g_listTeamFightRank.size() > MAX_TEAM_RANK_NUM)
				{
					g_listTeamFightRank.pop_back();
				}
				goto end;
			}
		}

	end:
		g_listTeamFightRank.sort([](TeamFightResultRank one, TeamFightResultRank two)
		{
			return one.rounds > two.rounds;
		}
		);//降序
	}

	void CJiangeTeamManager::startTeamFight(DWORD teamID)
	{
		CJiangeTeam* pTeam = getTeam(teamID);
		if (pTeam)
		{
			pTeam->startTeamFight();

			endTeamFight(pTeam);
		}
	}

	void CJiangeTeamManager::endTeamFight(CJiangeTeam* pTeam)
	{
		if (pTeam)
		{
			CC_SAFE_DELETE(pTeam);
			m_mapPrepareTeams.erase(pTeam->getTeamID());
		}
	}

	/*************************** NET **********************************/
	int getJiangeState(ROLE* role, unsigned char * data, size_t dataLen)
	{
		//TEAM_DEFENCE_STATE	0x0830 --请求活动状态信息
		//C : []
		//S : [result(错误码):byte, act_state(活动状态) : byte, my_state(我的状态) : byte]
		//result : 0成功
		//		 1 : 等级不足
		//	 act_state : 1今天活动未开启
		//				 2 : 今天活动进行中
		//				 3 : 今天活动已结束
		//			 my_state : 1 : 有奖励未领取(也可能是昨天的奖励未领取)
		//						2 : 未参与活动且不在组队
		//						3 : 未参与活动且在队伍中
		//						4 : 正在战斗中
		string strData;
		BYTE byRes = 0;
		BYTE byActState = 0;
		BYTE byMyState = 0;
		S_APPEND_BYTE(strData, byRes);
		do
		{
			COND_BREAK(role->wLevel < CJiangeTeamManager::g_wLimitLevel, byRes, 1);
			if (CJiangeTeamManager::getActivityState() == E_NO_OPEN)
			{
				byActState = 1;
			}

			if (role->jiangeState==E_TASK_COMPLETE_CAN_GET_PRIZE || role->jiangeState==E_TASK_HAVEPRIZE_UNJOIN)
			{
				byMyState = 1;
			}
			else if (role->jiangeState == E_TASK_UNCOMPLETE)
			{
				if (role->teamID == 0)
				{
					byMyState = 2;
				}
				else
				{
					byMyState = 3;
				}
			}
			else
			{
				byMyState = 5;
			}
			S_APPEND_BYTE(strData, byActState);
			S_APPEND_BYTE(strData, byMyState);

		} while (0);
		*((BYTE*)&strData[0]) = byRes;
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(TEAM_DEFENCE_STATE, strData));
		return 0;
	}


	int getJiangeTeams(ROLE* role, unsigned char * data, size_t dataLen)
	{
		//	----------------------队伍----------------------------
		//		--队员信息
		//		team_mate_info
		//	{
		//	player_id:dword; --玩家Id
		//		  pos : byte; --队伍中位置编号, 队长位 1
		//			job:byte; --职业
		//			sex : byte; --性别
		//			  level : byte; --等级
		//				  player_name : bstr; --名称
		//							faction_name : bstr; --帮派名称
		//									   strategy : byte; --策略选择开关标记, bit位标识
		//												  --00000 | 策略3 | 策略2 | 策略1;   位值1 表示 勾选
		//	}

		//	--队伍信息
		//	team_info
		//	{
		//	team_mates:[team_mate_info]
		//	}

		//	REQUEST_TEAM_LIST	0x0832 --请求队伍列表信息
		//C : []
		//S : [[num(队伍中队员数量):byte, [team_mate_info]]]

		string strData;
		SINGJIANGE()->forEachTeam([&](const std::pair<DWORD, CJiangeTeam*>& x){
				CJiangeTeam* pTeam = x.second;
				RETURN_VOID(pTeam == nullptr);
				S_APPEND_BYTE(strData, pTeam->getMemberNum());
				pTeam->forEachMember([&](const DWORD dwRoleID){
					ROLE* role = RoleMgr::getMe().getRoleByID(dwRoleID);
					RETURN_VOID(role == nullptr);
					S_APPEND_DWORD(strData, role->dwRoleID);
					S_APPEND_BYTE(strData, role->byJob);
					S_APPEND_BYTE(strData, role->bySex);
					S_APPEND_BSTR(strData, role->roleName);
					S_APPEND_BYTE(strData, role->teamBuff);
					string strFactionName;
					if (role->faction)
					{
						strFactionName = role->faction->name;
					}
					S_APPEND_BSTR(strData, strFactionName);
				}
				);
			}			
			);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_TEAM_LIST, strData));
		return 0;
	}

	int createJiangeTeam(ROLE* role, unsigned char * data, size_t dataLen)
	{
		//C:[]
		//S : [result(返回结果):byte]
		//result : 0成功
		//		 1 : 活动队伍已满
		//		 2 : 元宝不足

		string strData;
		BYTE byRes = SINGJIANGE()->newTeam(role);
		if (byRes == E_FULL)
		{
			byRes = 1;
		}
		else if (byRes == E_NO_GOLD)
		{
			byRes = 2;
		}
		else if (byRes == E_CREATED)
		{
			byRes = 3;
		}
		S_APPEND_BYTE(strData, byRes);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_CREATE_TEAM, strData));
		return 0;
	}

	int joinJiangeTeam(ROLE* role, unsigned char * data, size_t dataLen)
	{
	//REQUEST_APPLY_TEAM	0x0834 --请求加入队伍
	//C:[team_leader_id(队长ID):dword]
	//S : [result(返回结果):byte]
	//result : 0成功
		//1:申请队伍已满
		//2 : 申请队伍不存在或已解散
		//3 : 已经参加过今天的活动

		DWORD teamID;
		RETURN_COND( !BASE::parseDWORD(data, dataLen, teamID), 0);
		string strData;
		BYTE byRes = 0;
		do 
		{
			COND_BREAK(role->jiangeState != E_TASK_UNCOMPLETE, byRes, 3);
			byRes = SINGJIANGE()->joinTeam(role, teamID);
			if (byRes == E_FULL)
			{
				byRes = 1;
			}
			else if (byRes == E_NO_TEAM)
			{
				byRes = 2;
			}
			else if (byRes == E_JOINED)
			{
				byRes = 3;
			}
		} while (0);


		
		S_APPEND_BYTE(strData, byRes);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_APPLY_TEAM, strData));
		return 0;
	}

	int leaderStartfight(ROLE* role, unsigned char * data, size_t dataLen)
	{
		//REQUEST_GO_FIGHTING	0x0839 --请求去战斗
		//C : []
		//S : [result(返回结果):byte]
			//result:0成功
			//	1 : 没有队伍
			//	2 : 不是队长不能开始战斗

		string strData;
		BYTE byRes = 0;
		DWORD teamID = role->teamID;
		do 
		{
			CJiangeTeam* pTeam = SINGJIANGE()->getTeam(teamID);
			COND_BREAK(pTeam == nullptr, byRes, 1);
			COND_BREAK(pTeam->isLeader(role->dwRoleID), byRes, 2);
			pTeam->prepareStartFight();
		} while (0);


		S_APPEND_BYTE(strData, byRes);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_GO_FIGHTING, strData));
		return 0;
	}

	int getJiangeFightRank(ROLE* role, unsigned char * data, size_t dataLen)
	{
		//REQUEST_DEFEND_RANK	0x083D --排行
		//C : []
		//S : [[rank_item]]

		size_t count = 0;
		size_t nums = std::min<size_t>(CJiangeTeamManager::g_listTeamFightRank.size(), MAX_TEAM_RANK_NUM);
		string strData;
		for (auto it : CJiangeTeamManager::g_listTeamFightRank)
		{
			S_APPEND_BYTE(strData, it.rounds);
			S_APPEND_BYTE(strData, it.roles.size());
			for (auto id : it.roles)
			{
				ROLE* role = RoleMgr::getMe().getRoleByID(id);
				RETURN_COND(role == nullptr, 0);
				S_APPEND_DWORD(strData, role->dwRoleID);
				S_APPEND_BYTE(strData, role->byJob);
				S_APPEND_BYTE(strData, role->bySex);
				S_APPEND_BSTR(strData, role->roleName);
				S_APPEND_BYTE(strData, role->teamBuff);
				string strFactionName;
				if (role->faction)
				{
					strFactionName = role->faction->name;
				}
				S_APPEND_BSTR(strData, strFactionName);
			}

			if (count++ >= nums)
			{
				break;
			}
		}
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_DEFEND_RANK, strData));
		return 0;
	}

	int leaveJiangeTeam(ROLE* role, unsigned char * data, size_t dataLen)
	{
		RETURN_COND(role == nullptr, 0);
		//CJiangeTeam* pTeam = SINGJIANGE()->getTeam(role->teamID);
		//RETURN_COND(pTeam == nullptr, 0);
		SINGJIANGE()->leaveTeam(role, role->teamID);
		//pTeam->leaveTeam(role->dwRoleID);
		return 0;
	}

	int leaderKick(ROLE* role, unsigned char * data, size_t dataLen)
	{
		DWORD kickID;
		RETURN_COND(!BASE::parseDWORD(data, dataLen, kickID), 0);
		ROLE* kickRole = RoleMgr::getMe().getRoleByID(kickID);
		RETURN_COND(role == nullptr, 0);
		SINGJIANGE()->leaveTeam(kickRole, kickRole->teamID);
		return 0;
	}



	int getMyJiangeTeam(ROLE* role, unsigned char * data, size_t dataLen)
	{
	//	REQUEST_MY_TEAM_INFO	0x0835 --请求我的队伍信息
	//	C : []
	//	S : [result(返回结果):byte[, go_fighting_timer(战斗开始倒计时剩余时间):byte, [team_mate_info]]]]
	//result:0成功
	//	1 : 没有队伍

		string strData;
		BYTE byRes = 0;
		BYTE byActState = 0;
		BYTE byMyState = 0;
		S_APPEND_BYTE(strData, byRes);
		do
		{
			COND_BREAK(role->vecTeamRoleIDs.empty(), byRes, 1);
			BYTE leftTime = 0;
			CJiangeTeam* pTeam = SINGJIANGE()->getTeam(role->teamID);
			if (pTeam != nullptr)
			{
				DWORD now = time(nullptr);
				DWORD fightTime = pTeam->getFightTimeStamp();
				if (now < fightTime)
				{
					leftTime = BYTE(fightTime - now);
				}
			}
			S_APPEND_BYTE(strData, leftTime);
			S_APPEND_BYTE(strData, role->vecTeamRoleIDs.size());
			for (auto dwRoleID : role->vecTeamRoleIDs)
			{
				ROLE* role = RoleMgr::getMe().getRoleByID(dwRoleID);
				COND_CONTINUE(role == nullptr);
				S_APPEND_DWORD(strData, role->dwRoleID);
				S_APPEND_BYTE(strData, role->byJob);
				S_APPEND_BYTE(strData, role->bySex);
				S_APPEND_BSTR(strData, role->roleName);
				S_APPEND_BYTE(strData, role->teamBuff);
				string strFactionName;
				if (role->faction)
				{
					strFactionName = role->faction->name;
				}
				S_APPEND_BSTR(strData, strFactionName);
			}
		} while (0);
		*((BYTE*)&strData[0]) = byRes;
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_MY_TEAM_INFO, strData));
		return 0;
	}


	int setBuff(ROLE* role, unsigned char * data, size_t dataLen)
	{
		//REQUEST_CHANGE_STRATEGY	0x0838 --改变策略
		//C : [strategy(策略):byte]
		BYTE buffState;
		RETURN_COND(!BASE::parseBYTE(data, dataLen, buffState), 0);
		string strData;
		CJiangeTeam* pTeam = SINGJIANGE()->getTeam(role->teamID);
		RETURN_COND(pTeam == nullptr, 0);
		int backCost = CJiangeTeamManager::getBuffCost(role->teamBuff);
		int getCost = CJiangeTeamManager::getBuffCost(buffState);
		int needcost = getCost - backCost;
		BYTE byRes = 0;
		do 
		{
			COND_BREAK(needcost > 0 && role->hasEnoughGold(needcost), byRes, 1);
			role->teamBuff = buffState;
			ADDUP(role->dwIngot, needcost);
			notifyIngot(role);
		} while (0);
		S_APPEND_BYTE(strData, byRes);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_CHANGE_STRATEGY, strData));
		return 0;
	}

	int getJiangeFightProcess(ROLE* role, unsigned char * data, size_t dataLen)
	{
		vector<jiange_team::TeamFightProcess> vecTeamFightProcess;
		//REQUEST_FIGHTING_LOG	0x083A --战斗过程日志
		//C : []
		//S : [server_time(服务器当前时间):dword, count(战斗波次总数量) : byte, [fight_process]]
		string strData;
		S_APPEND_DWORD(strData, time(nullptr));
		S_APPEND_BYTE(strData, role->vecTeamFightProcess.size());
		for (auto process : role->vecTeamFightProcess)
		{
			S_APPEND_DWORD(strData, process.time);
			S_APPEND_BYTE(strData, process.vecFightEle.size());
			for (auto ele : process.vecFightEle)
			{
				S_APPEND_NBYTES(strData, (char*)&ele, sizeof(TeamFightResult));
			}
		}

		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_FIGHTING_LOG, strData));
		return 0;
	}
	int getReward(ROLE* role, unsigned char * data, size_t dataLen)
	{
		//REQUEST_EETCH_DEFEND_AWARDS	0x083B --领取奖励
		//C : []
		//S : [result:byte]
		//result:
		//	0 : 成功
		//		1 : 背包已满
		//		2 : 已经领取过
		//		3 : 未参加团战
		string strData;
		BYTE byRes = 0;
		do
		{
			COND_BREAK(E_TASK_UNCOMPLETE == role->jiangeState, byRes, 3);
			COND_BREAK(E_TASK_COMPLETE_ALREADY_GET_PRIZE == role->jiangeState, byRes, 2);

			vector<ITEM_INFO>vecItems;
			DWORD addCoin = 0;
			DWORD addExp = 0;
			for (auto process : role->vecTeamFightProcess)
			{
				for (auto ele : process.vecFightEle)
				{
					if (ele.role == role->dwRoleID)
					{
						ADDUP(addCoin, ele.coin);
						ADDUP(addExp, ele.exp);
						auto it = std::find(vecItems.begin(), vecItems.end(), ele.item);
						if ( it == vecItems.end())
						{
							vecItems.push_back(ITEM_INFO(ele.item, 1));
						}
						else
						{
							it->itemNums++;
						}
						break;
					}
				}
			}
			COND_BREAK(!MailCtrl::makeMailItems(role, vecItems), byRes, 1);

			ADDUP(role->dwCoin, addCoin);
			ADDUP(role->dwExp, addExp);
			if (role->jiangeState == E_TASK_COMPLETE_CAN_GET_PRIZE)
			{
				role->jiangeState = E_TASK_COMPLETE_ALREADY_GET_PRIZE;
			}
			else
			{
				role->jiangeState = E_TASK_UNCOMPLETE;
			}

		} while (0);
		S_APPEND_BYTE(strData, byRes);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_EETCH_DEFEND_AWARDS, strData));
		return 0;
	}

	int getJiangeFightHistory(ROLE* role, unsigned char * data, size_t dataLen)
	{
		//REQUEST_DEFEND_FIGHT_RECORD	0x083C --战斗历史记录
		//C : []
		//S : [[fight_record]]
		string strData;
		for (auto history : role->listTeamFightRecord)
		{
			S_APPEND_NBYTES(strData, (char*)&history, sizeof(TeamFightRecord));
		}
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(REQUEST_EETCH_DEFEND_AWARDS, strData));
		return 0;
	}

	
}


