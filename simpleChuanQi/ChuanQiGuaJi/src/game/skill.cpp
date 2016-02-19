#include "skill.h"
#include "itemCtrl.h"
#include "role.h"
//
///*
//@升级
//*/
////void SKILL::onLevelUp(ROLE* role, WORD wOldLevel)//升级函数
////{
////	autoUnlockSkills( role );
////}
//
//
///*
//@初始化技能列表，创建和登陆时候用
//*/
//void SKILL::InitRoleSkills(ROLE *role, WORD* skills, BYTE* fightSkills)
//{
//	//SKILL::autoUnlockSkills(role);//wm 应该在升级下
//	DWORD times[10];
//	getSkillsInfo(role, skills, times, fightSkills);
//}
//
//
///*
//@解锁技能，使用技能书
//*/
//int SKILL::unlockSkillByItem( ROLE* role, CONFIG::ITEM_CFG* itemCfg )
//{
//	if (itemCfg == NULL)
//	{
//		return 1;
//	}
//
//	if (role->wLevel < OPEN_SKILL_LEVEL)//技能七级开放
//	{
//		return 2;
//	}
//	if (itemCfg->sub_type != 5)//不是技能书
//	{
//		return 3;
//	}
//	WORD skillID = (WORD)itemCfg->hp_max;
//	CONFIG::SKILL_CFG2* skillCfg = CONFIG::getSkillCfg((WORD)itemCfg->hp_max);
//	if ( skillCfg == NULL || skillID >= SPECIAL_SKILL_MIN_ID )
//	{
//		return 4;
//	}
//	if (role->mapSkills.find( getGroupByID(skillID) ) != role->mapSkills.end())//控制已经有
//	{
//		return 5;
//	}
//
//	vector<CONFIG::SKILL_CFG2> vecNewSkill;
//
//	if ( (role->wLevel >= skillCfg->unlock_level)  &&  (role->byJob == skillCfg->job) )//职业和等级限制
//	{
//		vecNewSkill.push_back( *skillCfg);
//		levelUpdateSkills( role, vecNewSkill);
//	}
//
//	return 0;
//}
//
//
/////*
////@解锁技能，创建和升级时候用
////*/
////void SKILL::autoUnlockSkills( ROLE* role )
////{
////	if (role->wLevel < OPEN_SKILL_LEVEL)//技能七级开放
////	{
////		return;
////	}
////	map<WORD,PRACTISE_SKILL> tempMapSkills = role->mapSkills;
////	vector<CONFIG::SKILL_CFG2> vecNewSkill;
////	for (auto &it : mapSkillCfg)
////	{
////		auto skill = it.second;
////		if ( (skill.study != 1) && (role->wLevel >= skill.unlock_level)  &&  (role->byJob == skill.job) )//职业和等级限制
////		{
////			if (tempMapSkills.find( getGroupByID(skill.id) ) == tempMapSkills.end())//控制已经修炼的  //wm注意技能书以后要加
////			{
////				if (skill.id >= SPECIAL_SKILL_MIN_ID)//special can't up
////				{
////					continue;
////				}
////				vecNewSkill.push_back(skill);
////				PRACTISE_SKILL praSkill(skill.id, 0, SIXTY(skill.levelup_exp));
////				tempMapSkills.insert( make_pair( getGroupByID(skill.id) , praSkill));//每组只存一个等级的
////				//wm这里要推送给客户端getSkillsInfo(ROLE *role, WORD* skills, DWORD* times, BYTE* fightSkills)
////			}
////		}
////	}
////	if (vecNewSkill.size() > 0)
////	{
////		levelUpdateSkills( role, vecNewSkill);
////	}
////}
//
///*
//@获取技能列表
//*/
//void SKILL::getSkillsInfo(ROLE *role, WORD* skills, DWORD* times, BYTE* fightSkills)
//{
//	//logwm("getskillinfo  role->wOnUpSkill:%d\n",role->wOnUpSkill); 
//	vector<WORD> vecSkills;
//	vector<DWORD> vecTimes;
//	PRACTISE_SKILL praSkill;
//
//	if (role->wOnUpSkill != 0)
//	{
//		WORD id = role->wOnUpSkill;
//		DWORD dwTime = PROTOCAL::getCurTime();
//		auto it = role->mapSkills.find( getGroupByID(id) );
//		if ( it == role->mapSkills.end())//找不到赋值0
//		{
//			role->wOnUpSkill = 0;
//		}
//		else
//		{
//			auto& skill = it->second;
//			if (skill.dwReqTime <= (dwTime - skill.dwStartTime))//已经修炼完成
//			{
//				CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(skill.wID + 1);
//				if (pSkill != NULL && role->wLevel>=pSkill->unlock_level)
//				{
//					skill.wID += 1;
//					skill.dwReqTime = SIXTY(pSkill->levelup_exp);
//				}
//				else
//				{
//					skill.dwReqTime = ZERO_TIME;
//				}
//				skill.dwStartTime = ZERO_TIME;
//				role->wOnUpSkill = 0;
//			}
//			else
//			{
//				skill.dwReqTime = skill.dwReqTime - (dwTime - skill.dwStartTime);
//				skill.dwStartTime = dwTime;
//			}	
//		}
//	}
//
//	for (auto& it:role->mapSkills)
//	{	
//		praSkill = it.second;
//		if (it.second.wID >= SPECIAL_SKILL_MIN_ID)//special can't up
//		{
//			continue;
//		}
//		
//		vecSkills.push_back(praSkill.wID);
//		vecTimes.push_back( praSkill.dwReqTime );
//	}
//	for (size_t i = 0; i < vecSkills.size(); i++)
//	{
//		skills[i] = vecSkills[i];
//		times[i] = vecTimes[i];
//	}
//	memcpy(fightSkills, role->byFightSkill,sizeof(role->byFightSkill));
//}
//
///*
//@获取正在修炼技能在Index
//*/
//BYTE SKILL::getIndexBySkillID(ROLE *role, WORD id)
//{
//	if (id == 0)
//	{
//		return 0;
//	}
//	size_t  fightIndex = 0;
//	PRACTISE_SKILL praSkill;
//	for (auto& it:role->mapSkills)
//	{
//		fightIndex++;//客户端索引是1~10
//		praSkill = it.second;
//		if (praSkill.wID == id)
//		{
//			return fightIndex;
//		}
//	}
//	return 0;
//}
//WORD SKILL::getSkillIDByIndex(ROLE *role, BYTE index)
//{
//	if (index == 0 || index > role->mapSkills.size())
//	{
//		return 0;
//	}
//	size_t  fightIndex = 0;
//	PRACTISE_SKILL praSkill;
//	for (auto& it:role->mapSkills)
//	{
//		fightIndex++;//客户端索引是1~10
//		praSkill = it.second;
//		if (fightIndex == index)
//		{
//			return praSkill.wID;
//		}
//	}
//	return 0;
//}
//BYTE SKILL::setFightSkills(ROLE *role, BYTE* idIndexs)
//{
//	size_t maxIndex = role->mapSkills.size();
//	for (size_t i=0; i < FIGHT_SKILL_INDEXS; i++)//不能超过做大索引
//	{
//		if (idIndexs[i] > maxIndex)
//		{
//			return SKILL_NO_EXISTS;
//		}
//		
//	}
//
//	for (size_t i=0; i < FIGHT_SKILL_INDEXS - 1;i++)//互不相同
//	{
//		for (size_t j = 0; j < FIGHT_SKILL_INDEXS- i - 1; j++)
//		{
//			if (idIndexs[j] != 0 && idIndexs[j] == idIndexs[j+1])
//			{
//				return SET_SKILL_REPEAT;
//			}
//		}
//	}
//	memcpy(role->byFightSkill, idIndexs, sizeof(role->byFightSkill));
//	return SKILL_SUCCESS;
//}
//
///*
//@暂停修炼技能,玩家自动暂停，但是下线继续修炼
//*/
//BYTE SKILL::pauseUpSkill(ROLE *role, WORD id)
//{
//	//logwm("pause id =%d",id);
//	if(id != role->wOnUpSkill)
//	{ 
//		//logwm("NO_EXSISTS:role->wOnUpSkill:%d\n",role->wOnUpSkill); 
//		return SKILL_NO_EXISTS;
//	}
//	DWORD dwTime = PROTOCAL::getCurTime();
//
//	auto it = role->mapSkills.find( getGroupByID(id));
//	if ( it == role->mapSkills.end())
//	{
//		return SKILL_NO_EXISTS;
//	}
//	auto& skill = it->second;
//
//	if (skill.dwReqTime <= (dwTime - skill.dwStartTime))//已经修炼完成
//	{
//		CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(skill.wID + 1);
//		if (pSkill != NULL && role->wLevel>=pSkill->unlock_level)
//		{
//			skill.wID += 1;
//			skill.dwReqTime = SIXTY(pSkill->levelup_exp);
//		}
//		else
//		{
//			skill.dwReqTime = ZERO_TIME;
//		}
//	}
//	else
//	{
//		skill.dwReqTime = skill.dwReqTime - (dwTime - skill.dwStartTime);
//	}
//
//	skill.dwStartTime = ZERO_TIME;											//设置修炼开始时间为0
//	role->wOnUpSkill = 0;
//
//	return UP_SKILL_SUCCESS;
//}
//
//
//
///*
//@修炼技能
//*/
//DWORD SKILL::upSkill(ROLE *role, WORD id, BYTE& byErrorCode)
//{
//	CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(id);
//	if (pSkill == NULL)//不存在
//	{
//		byErrorCode = SKILL_NO_EXISTS;
//		return ZERO_TIME;
//	}
//	if (pSkill->levelup_exp == ZERO_TIME)//最高等级,注意是配置表的值
//	{
//		byErrorCode = MAX_SKILL_LEVEL;
//		return ZERO_TIME;
//	}
//	
//	if ( pSkill->level > role->wLevel)//未达到经验
//	{
//		byErrorCode = SKILL_NO_EXISTS;
//		return ZERO_TIME;
//	}
//
//	if (role->wOnUpSkill > 0)//正在修炼其他技能
//	{
//		byErrorCode = SKILL_ON_UPING;
//		return ZERO_TIME;
//	}
//
//	auto it = role->mapSkills.find( getGroupByID(id));
//	if ( it == role->mapSkills.end())
//	{
//		byErrorCode = SKILL_NO_EXISTS;
//		return ZERO_TIME;
//	}
//
//	DWORD dwTime = PROTOCAL::getCurTime();
//	auto& skill = it->second;
//
//	skill.dwStartTime = dwTime;
//
//	role->wOnUpSkill = id;
//
//	return skill.dwReqTime;
//}
//
///*
//@购买技能所需元宝
//*/
//DWORD SKILL::buySkillCost(ROLE *role, WORD id)
//{
//	auto it = role->mapSkills.find( getGroupByID(id) );
//	if ( it == role->mapSkills.end())
//	{
//		return 0;
//	}
//	auto& skill = it->second;
//	CONFIG::SKILL_CFG2* pNowSkill = CONFIG::getSkillCfg(skill.wID);
//	if (pNowSkill == NULL)
//	{
//		return 0;
//	}
//
//	DWORD cost ;
//	time_t now = PROTOCAL::getCurTime();
//	if (role->wOnUpSkill == id)
//	{
//		cost = (  skill.dwReqTime - (now - skill.dwStartTime) ) / 60;
//	}
//	else
//	{
//		cost = skill.dwReqTime / 60;
//	}
//
//	return cost;
//}
//
//
/////*
////@购买技能
////*/
////WORD SKILL::buySkill(ROLE *role, WORD id, BYTE&  byCode )
////{
////	CONFIG::VIP_ADDITION_CFG* vipCfg = CONFIG::getVipAddCfg(role->getVipLevel());
////	if (vipCfg != NULL && vipCfg->skill_speed == 1)
////	{
////	}
////	else
////	{
////		byCode = VIP_LEVEL_NOT_ENOUGH;
////		return 0;//vip等级不足
////	}
////
////
////	auto it = role->mapSkills.find( getGroupByID(id) );
////	if ( it == role->mapSkills.end())
////	{
////		byCode = SKILL_NO_EXISTS;
////		return 0;
////	}
////	auto& skill = it->second;
////	CONFIG::SKILL_CFG2* pNowSkill = CONFIG::getSkillCfg(skill.wID);
////	if (pNowSkill == NULL)
////	{
////		byCode = SKILL_NO_EXISTS;
////		return 0;
////	}
////	
////	CONFIG::SKILL_CFG2* pNextSkill = CONFIG::getSkillCfg(skill.wID + 1);
////	if (pNextSkill == NULL)
////	{
////		if (pNowSkill->levelup_exp == 0)
////		{
////			byCode = SKILL_LEVEL_FULL;
////		}
////		else
////		{
////			byCode = SKILL_NO_EXISTS;
////		}
////		return 0;
////	}
////
////	if ( role->wLevel < pNextSkill->level )
////	{
////		byCode = LEVEL_NOT_ENOUGH;
////		return 0;
////	}
////
////	DWORD cost ;
////	time_t now = PROTOCAL::getCurTime();
////	if (role->wOnUpSkill == id)
////	{
////	  cost = (  skill.dwReqTime - (now - skill.dwStartTime) ) / 60;
////	}
////	else
////	{
////		 cost = skill.dwReqTime / 60;
////	}
////
////	if ( cost > role->dwIngot )
////	{
////		byCode = COLD_NOT_ENOUGH;
////		return  0;
////	}
////
////	//if (role->wLevel>=pSkill->unlock_level)
////	//{
////		skill.wID += 1;
////		skill.dwReqTime = SIXTY(pNextSkill->levelup_exp);
////	//}
////	//else
////	//{
////	//	skill.dwReqTime = ZERO_TIME;
////	//}
////	skill.dwStartTime = ZERO_TIME;
////
////	//role->dwIngot -= cost * CONFIG::skillcost;
////	ADDUP(role->dwIngot, -(int)(cost * CONFIG::skillcost) );
////	notifyIngot(role);
////	if (role->wOnUpSkill == id)
////	{
////		role->wOnUpSkill = 0;
////	}
////	return id+1;
////}
//
///*
//@修炼技能成功
//*/
//WORD SKILL::upSkillOK(ROLE *role, WORD id)
//{
//	if (role->wOnUpSkill == 0)//没有技能在修炼
//	{
//		return 0;
//	}
//	DWORD dwTime = PROTOCAL::getCurTime();
//	auto it = role->mapSkills.find( getGroupByID(id));
//	if ( it == role->mapSkills.end())
//	{
//		return 0;
//	}
//	auto& skill = it->second;
//
//	if ( skill.dwReqTime > (dwTime - skill.dwStartTime) )
//	{
//		return 0;
//	}
//
//	CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(skill.wID + 1);
//	if (pSkill != NULL && role->wLevel>=pSkill->unlock_level)
//	{
//		skill.wID += 1;
//		skill.dwReqTime = SIXTY(pSkill->levelup_exp);
//	}
//	else
//	{
//		skill.dwReqTime = ZERO_TIME;
//	}
//	skill.dwStartTime = ZERO_TIME;
//
//	return id+1;
//}
//
//
//WORD SKILL::getGroupByID(WORD id)
//{
//	CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(id);
//	if ( pSkill == NULL)
//	{
//		return 0;
//	}
//	return pSkill->group;
//}
//
////WORD SKILL::getIDByGroup(WORD group)
////{
////	return 0;
////}
//
//
///*
//@时间更新技能列表
//*/
//void SKILL::timeUpdateSkills(ROLE *role)
//{
//	if (role->wOnUpSkill == 0)
//	{
//		return;
//	}
//
//	WORD id = role->wOnUpSkill;
//	DWORD dwTime = PROTOCAL::getCurTime();
//
//	auto it = role->mapSkills.find( getGroupByID(id) );
//	if ( it == role->mapSkills.end())//找不到赋值0
//	{
//		role->wOnUpSkill = 0;
//		return;
//	}
//
//	auto& skill = it->second;
//
//	if (skill.dwReqTime > (dwTime - skill.dwStartTime))//未修炼完成
//	{
//		return;
//	}
//
//	CONFIG::SKILL_CFG2* pSkill = CONFIG::getSkillCfg(skill.wID + 1);
//	if (pSkill == NULL && role->wLevel < pSkill->unlock_level)//满级或者等级未到
//	{
//		skill.dwReqTime = ZERO_TIME;//升级时候会用到
//		return;
//	}
//
//	skill.wID += 1;
//	skill.dwReqTime = SIXTY(pSkill->levelup_exp);
//	skill.dwStartTime = ZERO_TIME;
//	role->wOnUpSkill = 0;
//
//	vector<WORD> vecSkills;
//	vector<DWORD> vecTimes;
//	PRACTISE_SKILL praSkill;
//	string strData;
//	WORD skills[MAX_SKILL_NUM] = {0};
//
//	for (auto& it:role->mapSkills)
//	{	
//		praSkill = it.second;
//		if (it.second.wID >= SPECIAL_SKILL_MIN_ID)//special can't up
//		{
//			continue;
//		}
//
//		vecSkills.push_back(praSkill.wID);
//		vecTimes.push_back( praSkill.dwReqTime );
//	}
//	for (size_t i = 0; i < vecSkills.size(); i++)
//	{
//		skills[i] = vecSkills[i];
//	}
//
//	for (size_t i = 0; i < vecSkills.size(); i++)
//	{
//		skills[i] = vecSkills[i];
//	}
//
//	for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)
//	{
//		S_APPEND_BYTE( strData, role->byFightSkill[i]);
//	}
//	for (size_t i = 0; i < MAX_SKILL_NUM; i++)
//	{
//		S_APPEND_WORD( strData, skills[i]);
//	}
//
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_STUDY_NEW_SKILL, strData) );	
//
//	if (pSkill->type == E_SKILL_TYPE_PASSIVE)
//	{
//		ItemCtrl::updateNotifyRoleAttr(role);
//	}
//}
//
///*
//@等级更新技能列表
//*/
//void SKILL::levelUpdateSkills(ROLE *role, vector<CONFIG::SKILL_CFG2>& vecNewSkill)
//{
//	vector<WORD> vecSkills;
//	WORD  FightSkills[FIGHT_SKILL_INDEXS] = {0};
//	PRACTISE_SKILL praSkill;
//	string strData;
//	WORD skills[MAX_SKILL_NUM] = {0};
//	bool bCalRoleAttr = false;
//
//	WORD fightID;
//
//	for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)//提出快捷技能ids
//	{
//		if ( (fightID = getSkillIDByIndex(role, role->byFightSkill[i])) != 0)
//		{
//			 FightSkills[i] = fightID;
//		}
//	}
//
//	for (size_t i = 0; i < vecNewSkill.size(); i++)//插入mapskills
//	{
//		PRACTISE_SKILL praSkill(vecNewSkill[i].id, 0, SIXTY(vecNewSkill[i].levelup_exp));
//		role->mapSkills.insert( make_pair( getGroupByID(vecNewSkill[i].id) , praSkill));//每组只存一个等级的
//		if (vecNewSkill[i].type == E_SKILL_TYPE_PASSIVE)
//		{
//			bCalRoleAttr = true;
//		}
//	}
//
//	for (auto& it:role->mapSkills)
//	{	
//		praSkill = it.second;
//		if (it.second.wID >= SPECIAL_SKILL_MIN_ID)//special can't up
//		{
//			continue;
//		}
//		vecSkills.push_back(praSkill.wID);
//	}
//
//
//
//	for (size_t i = 0; i < vecSkills.size(); i++)
//	{
//		skills[i] = vecSkills[i];
//	}
//
//
//	for (size_t i = 0; i < vecNewSkill.size(); i++)//空位设置id
//	{
//		for (size_t j = 0; j < FIGHT_SKILL_INDEXS; j++)
//		{
//			if ( FightSkills[j] == 0)
//			{
//				FightSkills[j] = vecNewSkill[i].id;
//				break;
//			}
//		}
//	}
//
//	for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)//提出快捷技能ids
//	{
//		role->byFightSkill[i] = getIndexBySkillID(role, FightSkills[i]);
//	}
//
//	for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)
//	{
//		S_APPEND_BYTE( strData, role->byFightSkill[i]);
//	}
//	for (size_t i = 0; i < MAX_SKILL_NUM; i++)
//	{
//		S_APPEND_WORD( strData, skills[i]);
//	}
//
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_STUDY_NEW_SKILL, strData) );
//
//	if (bCalRoleAttr)
//	{
//		ItemCtrl::updateNotifyRoleAttr(role);
//	}
//}
//
//int SKILL::dealSkill(ROLE *role, unsigned char * data, size_t dataLen ,WORD cmd)
//{
//	return 0;//wm
//
//	if (role == NULL)
//	{
//		return 0;
//	}
//
//	if (cmd == S_GET_SKILL_CUR_TIME)
//	{
//		string strData;
//		WORD skills[MAX_SKILL_NUM] = {0};
//		DWORD times[MAX_SKILL_NUM] = {0};
//		BYTE  fightSkills[FIGHT_SKILL_INDEXS] = {0};
//		SKILL::getSkillsInfo(role, skills, times, fightSkills);
//		for (size_t i = 0; i < MAX_SKILL_NUM; i++)
//		{
//			logwm("skills[i] = %d  times[i] = %d ",skills[i],times[i]);
//			S_APPEND_WORD( strData, skills[i]);
//			S_APPEND_DWORD( strData, times[i]);
//		}
//		for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)
//		{
//			S_APPEND_BYTE( strData, role->byFightSkill[i]);
//		}
//		BYTE index = SKILL::getIndexBySkillID(role, role->wOnUpSkill);
//		S_APPEND_BYTE( strData, index );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GET_SKILL_CUR_TIME, strData) );	
//	}
//
//	if (cmd == S_SKILL_UP)
//	{
//
//		WORD id;
//		BYTE byErrorCode = 0;
//		string strData;
//		if ( !BASE::parseWORD( data, dataLen, id) )
//			return -1;
//		DWORD  dwTime =  SKILL::upSkill(role, id ,byErrorCode);
//		S_APPEND_BYTE(strData, byErrorCode);
//		S_APPEND_WORD( strData, id );
//		S_APPEND_DWORD( strData, dwTime );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SKILL_UP, strData) );
//	}
//
//	if (cmd == S_SKILL_PAUSE)
//	{
//		WORD id;
//		if ( !BASE::parseWORD( data, dataLen, id) )
//			return -1;
//		BYTE  byCode =  SKILL::pauseUpSkill(role, id );
//		string strData;
//		S_APPEND_BYTE( strData, byCode );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SKILL_PAUSE, strData) );
//
//	}
//
//	if (cmd == S_SKILL_BUY)
//	{
//		//WORD id;
//		//if ( !BASE::parseWORD( data, dataLen, id) )
//		//	return -1;
//		//BYTE  byCode = 0;
//		//WORD  reID =  SKILL::buySkill(role, id, byCode);
//		//string strData;
//		//S_APPEND_BYTE( strData, byCode );
//		//S_APPEND_WORD( strData, reID );
//		//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SKILL_BUY, strData) );
//	}
//
//	if (cmd == S_SKILL_BUY_COST)
//	{
//		WORD id;
//		if ( !BASE::parseWORD( data, dataLen, id) )
//			return -1;
//		DWORD  dwCost =  SKILL::buySkillCost(role, id);
//		string strData;
//		S_APPEND_DWORD( strData, dwCost );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SKILL_BUY_COST, strData) );
//	}
//
//	if (cmd == S_SKILL_QUICK)
//	{
//		BYTE idIndexs[FIGHT_SKILL_INDEXS];
//		for(size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)
//		{
//			if ( !BASE::parseBYTE( data, dataLen, idIndexs[i]) )
//				return -1;
//		}
//		BYTE  byCode = SKILL::setFightSkills(role, idIndexs);
//		string strData;
//		S_APPEND_BYTE( strData, byCode );
//		S_APPEND_NBYTES( strData, idIndexs, sizeof(idIndexs));
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SKILL_QUICK, strData) );
//	}
//	if (cmd == S_SKILL_UPOK)
//	{
//		WORD id;
//		if ( !BASE::parseWORD( data, dataLen, id) )
//			return -1;
//		WORD  reID = SKILL::upSkillOK(role, id);
//		string strData;
//		S_APPEND_WORD( strData, reID );
//		PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_SKILL_UPOK, strData) );
//	}
//
//	return 0;
//}

int SKILL::clientGetSkillsIndexs(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientGetSkillsIndexs(data,dataLen);
}

int SKILL::clientSkillMoveUp(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientSkillMoveUp(data,dataLen);
}

int SKILL::clientGetUnlockSkillsList(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientGetUnlockSkillsList(data,dataLen);
}

int SKILL::clientSaveSkillsIndexs(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientSaveSkillsIndexs(data,dataLen);
}

int SKILL::clientUpSkillLevel(ROLE *role, unsigned char * data, size_t dataLen)
{
	if (role == NULL)
	{
		return 0;
	}

	return role->role_skill.clientUpSkillLevel(data,dataLen);
}