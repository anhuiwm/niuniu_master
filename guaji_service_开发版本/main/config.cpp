#include "pch.h"
#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"
#include "jsonpacket.h"
#include "roleCtrl.h"
#include "itemCtrl.h"
#include "protocal.h"
#include "broadCast.h"
#include "activity.h"

using namespace BASE;

namespace CONFIG
{
	static const string STR_ALL = "all";
	static map<DWORD, ATTR_UPGRADE_CFG> _mapAttrUpgradeCfg;
	static map<WORD, MONSTER_CFG> _mapMonsterCfg;
	static map<DWORD, ARENA_BOUNS_CFG> _mapArenaBounsCfg;
	static map<DWORD, SEND_7DAY_AWARD_CFG> _mapSend7DayAwardCfg;
	static multimap<WORD, SUIT_CFG> _multimapSuitCfg;
	static map<DWORD, ARENA_REWARD_CFG> _mapArenaRewardCfg;
	static map <WORD, SKILL_CFG2> _mapSkillCfg;
	static map<WORD, SKILL_EFF_CFG> _mapSkillEffCfg;
	static map< BYTE, SKILL_STORAGE > _mapJobGroupSkillCfg;
	static map<WORD, ITEM_CFG> _mapItemCfg;// 道具表
	static map<DWORD, std::vector<WORD>> _mapRuneQualitys; // 符文品质分类
	static map<WORD, PLAYER_CFG> _mapPlayerCfg;
	static map<WORD, MAP_CFG> _mapMapCfg;
	static map<WORD, DOUBLE_WORD> _mapEliteCfg;
	static map<DWORD, COMPOSE_CFG>_mapComposeCfg;
	static map<WORD, DWORD>_mapComposeMaxExpCfg;
	static map<DWORD, DWORD>_mapComposeNowExpCfg;	
	static vector<ROLE_EXP_CFG> _vecExpCfg;	
	static map<DWORD, ATT_RAND_CFG> _mapAttRandCfg;
	static map<WORD, GUILD_BUILD_CFG> _mapGuildBuildCfg;
	static map<WORD, float> _mapComposeCoff;	
	static map<DWORD, DROP_INFO_CFG> _mapDropInfoCfg;	
	static map<DWORD, DROP_INFO_CFG> _mapDropInfoChestCfg;	
	static map<DWORD, SIGN_CFG> _mapSignCfg;	
	static map<BYTE, SIGN_CFG> _mapSignTimeCfg;	
	static BYTE _byMaxContinueSignTimes;	
	static map<WORD, VIP_ADDITION_CFG> _mapVipAddCfg;	
	static map<WORD, ACTIVITY_INFO_CFG> _mapActivityInfoCfg;	
	static map<WORD, DWORD> _mapMoneyTreeCfg;	
	static map<WORD, MONEY_TREE0_CFG> _mapMoneyTree0Cfg;	
	static map<WORD, MISSION_CFG> _mapMissionCfg;	
	static map<BYTE, vector<WORD>>_mapTaskTypeIDs;	
	static map<BYTE, vector<WORD>>_mapOpenActivityTypeIDs;	
	static vector<MINE_CFG> _vecMineCfg;
	static  map<DWORD, SHOP1_CFG> _mapShop1Cfg;	
	static  map<WORD, SHOP2_CFG> _mapShop2Cfg;
	static  map<BYTE, SHOP0_CFG> _mapVipShop0Cfg;	
	static  map<DWORD, SHOP1_CFG> _mapVipShop1Cfg;	
	static  map<WORD, SHOP2_CFG> _mapVipShop2Cfg;
	static  map<WORD, SHOP_STORAGE> _mapShopStorageCfg;
	static  map<WORD, SHOP_STORAGE> _mapVipShopStorageCfg;	
	static  map<WORD, SHOP_DISCOUNT> _mapDiscountCfg;	
	static map<DWORD, float> _mapLuckCfg;
	static map<DWORD, DWORD> _mapChargeCfg;
	static map<WORD, OPEN_ACTIVITY> _mapOpenActivityCfg;
	static DWORD _vDonatePoint[MAX_RARE_LEVEL][11];//7个稀有度,10种装备(1-10)
	static DWORD _coinDonate;
	static  map<WORD, PKG_EXTEND_CFG> _mapPkgExtendCfg;
	static size_t vvSmeltValue[7][11];//row: 0-6 品质,col: 0 quality, col: 1-10, 直接用sub_type作index
	static  map<DWORD, FORGE_REFRESH_CFG > _mapForgeRefreshCfg;
	static map<DWORD, RUNE_CFG_ITEM> _mapRuneCfg; // 符文配置表
	static map<DWORD, IP_INFO> _mapIPInfo;
	static map<WORD, MERDEF_CFG> _mapMercenaryDefinitionCfg;
	static map<WORD, MERHALO_CFG> _mapMercenaryHaloCfg;
	static map<BYTE, MERCENARY_TRAIN_CFG> _mapMercenaryTrainCfg;
	static map<WORD, DWORD> _mapMercenaryTrainMaxCfg;
	static map<DWORD, MERCENARY_CFG> _mapMercenaryCfg;
	static map<BYTE, ATTRDEFORM_CFG> _mapAttributeDeformCfg;
	static map<BYTE, MER_STAR_CFG> _mapMercenaryStarCfg;
	static DWORD _dwMercenaryStarMaxExpCfg;
	static map<BYTE, DWORD>_mapMercenaryStarNowExpCfg;
	static map<WORD, MER_COMPOSE_BOOK_CFG> _mapMerComposebookCfg;
	static map<WORD, GEM_CFG> _mapGemCfg;
	static map<WORD, TARGET_AWARD_CFG> _mapTargetAwardCfg;
	static vector<RECHARGE_AWARD_CFG> _vecRechargeAwardCfg;
	static vector<WORD> _vecMerComposeBookID;
	static vector<string> _vecGmName;
	static vector<string> _vecShieldWords;
	static map<uint8, tencent_task> _map_tencent_task_cfg;
	static map<uint16, guild_shrine> _map_guild_shrine_cfg;
	static map<WORD, TD_MONSTER_CFG> _mapTdMonsterCfg;

	static bool readCfgAttrUpgrade(void);
	static bool readCfgMonster(void);
	static bool readCfgArenaBouns(void);
	static bool readCfgSecd7DayAward(void);
	static bool readCfgSuit(void);
	static bool readCfgArenaReward(void);
	static bool readCfgSign(void);
	static bool readCfgVipAdd(void);
	static bool readCfgActivityInfo(void);
	static bool readCfgSkill(void);
	static bool readCfgEffect(void);
	static bool readCfgItem(void);
	static bool readCfgPlayer(void);
	static bool readCfgMap(void);
	static bool readCfgCompose(void);
	static bool readCfgRoleExp(void);
	static bool readCfgAttRandom(void);
	static bool readCfgGuildBuild(void);
	static bool readCfgDrop(void);
	static bool readCfgFactionDonation(void);
	static bool readCfgMine(void);
	static bool readCfgShop1(void);
	static bool readCfgShop2(void);
	static bool readCfgVipShop0(void);
	static bool readCfgVipShop1(void);
	static bool readCfgVipShop2(void);
	static bool readCfgDisCount(void);
	static bool readCfgMission(void);
	static bool readCfgOpenActivity(void);
	static bool readCfgMoneyTree(void);	
	static bool readCfgMoneyTree0(void);	
	static bool readCfgMsg(void);
	static bool readCfgExtendPkg();
	static bool readCfgSmelt();
	static bool readCfgForgeRresh();
	static bool readCfgRune();// 读取符文配置 
	static bool readCfgMercenary();
	static bool readCfgMercenaryDefinition();
	static bool readCfgMercenaryHalo();
	static bool readCfgMercenaryTrain();
	static bool readCfgMercenaryTrainMax();
	static bool readCfgMercenaryUpstar();
	static bool	readCfgMercenaryComposeBook();
	static bool	readCfgGem();
	static bool readCfgTargetAward();
	static bool readCfgLuck();
	static bool readCfgCharge();
	static bool readCfgIP();
	static bool readCfgAttributeDeform(void);
	static bool readCfgRechargeAward(void);
	static bool readCfgTecentTask(void);
	static bool readcfg_guild_shrine(void);
	static bool readCfgTdMonster(void);

	static ITEM_INFO getDropCreateInfo( CONFIG::DROP_INFO_CFG* dropInfo, WORD groupID);//, bool bChest=false );
	static string _msgJingjichang;
	static string _msgBossHurtRank;
	static string _msgBossLastHurt;
	static string _send_one;
	static string _send_two;
	static string _send_three;
	static string _7ripaihangbang;
	static string _treasure;// 宝藏
	static string _meet_breakcombo;// 偶遇终结连杀
	static string _platform_task_reward;

	typedef bool (*FUNC_CFG_READ)();
	typedef map<string, FUNC_CFG_READ> MAP_CFG_READ;
	static  MAP_CFG_READ m_mapCfgRead;
	
}

bool CONFIG::readCfgTdMonster(void)
{
	string strIniFile = getConfigDir() + "td_monster.ini";
	_mapTdMonsterCfg.clear();
	eachIniSecLine(strIniFile, "td_monster", [&](const char* line)
	{
		vector<string> vecString;
		if (strtoken2Vec(line, SPLIT_SPACE, vecString, NULL, NULL) < 7)
			return;

		TD_MONSTER_CFG tdMonsterCfg;
		tdMonsterCfg.id = ATOW_(vecString[0]);
		//tdMonsterCfg.name =  vecString[1];
		tdMonsterCfg.power = ATOUL_(vecString[2]);
		tdMonsterCfg.money = ATOUL_(vecString[3]);
		tdMonsterCfg.exp = ATOUL_(vecString[4]);
		tdMonsterCfg.item = ATOW_(vecString[5]);
		tdMonsterCfg.item_odds = ATOF_(vecString[6]);
		_mapTdMonsterCfg.insert(make_pair(tdMonsterCfg.id, tdMonsterCfg));

	}
	);

	return true;
}

bool CONFIG::readCfgAttrUpgrade(void)
{
	string strIniFile = getConfigDir() + "att_upgrade.ini";

	_mapAttrUpgradeCfg.clear();

	eachIniSecLine(strIniFile, "attr", [&](  const char* line )
		{
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 31 )	
				return;
			
			ATTR_UPGRADE_CFG attrCfg;
			attrCfg.job_id       =  ATOW_( vecString[ 0 ] )   ;//职业编号索引
			attrCfg.level		 =  ATOW_( vecString[ 1 ] )   ;//等级
			attrCfg.stamina	     =  ATOUL_( vecString[ 2 ] )  ;//耐力
			attrCfg.strength	 =  ATOUL_( vecString[ 3 ] )  ;//力量
			attrCfg.agility	     =  ATOUL_( vecString[ 4 ] )  ;//敏捷
			attrCfg.intellect    =  ATOUL_( vecString[ 5 ] )  ;//智慧
			attrCfg.spirit		 =  ATOUL_( vecString[ 6 ] )  ;//精神
			attrCfg.hp_max		 =  ATOUL_( vecString[ 7 ] )  ;//最大生命值
			attrCfg.mp_max		 =	ATOUL_( vecString[ 8 ] ) ;//最大魔法值
			attrCfg.atk_max	     =  ATOUL_( vecString[ 9 ] )  ;//攻击最大值
			attrCfg.atk_min	     =  ATOUL_( vecString[ 10 ] )  ;//攻击最小值
			attrCfg.def_max	     =  ATOUL_( vecString[ 11 ] )  ;//防御最大值
			attrCfg.def_min	     =  ATOUL_( vecString[ 12 ] )  ;//防御最小值
			attrCfg.spellatk_max =	ATOUL_( vecString[ 13 ] )  ;//技能攻击最大值
			attrCfg.spellatk_min =	ATOUL_( vecString[ 14 ] ) ;//技能攻击最小值
			attrCfg.spelldef_max =  ATOUL_( vecString[ 15 ] ) ;//技能防御最大值
			attrCfg.spelldef_min =  ATOUL_( vecString[ 16 ] ) ;//技能防御最小值
			attrCfg.hit			 =	ATOUL_( vecString[ 17 ] )  ;//命中值
			attrCfg.dodge		 =	ATOUL_( vecString[ 18 ] )  ;//闪避值
			attrCfg.crit		 =	ATOUL_( vecString[ 19 ] )  ;//暴击值
			attrCfg.crit_res	 =	ATOUL_( vecString[ 20 ] )  ;//暴击抵抗值
			attrCfg.crit_inc	 =	ATOUL_( vecString[ 21 ] )  ;//暴击伤害增加值
			attrCfg.crit_red	 =	ATOUL_( vecString[ 22 ] )  ;//暴击伤害减免值
			attrCfg.mp_rec		 =	ATOUL_( vecString[ 23 ] )  ;//每回合魔法恢复值
			attrCfg.hp_rec		 =	ATOUL_( vecString[ 24 ] )  ;//每回合生命恢复值
			attrCfg.luck		 =	ATOUL_( vecString[ 25 ] )  ;//幸运
			attrCfg.att_hem		 =	ATOUL_( vecString[ 26 ] )  ;//攻击吸血
			attrCfg.att_mana	 =	ATOUL_( vecString[ 27 ] )  ;//攻击吸蓝
			attrCfg.equ_drop	 =	ATOUL_( vecString[ 28 ] ) ;//装备掉率
			attrCfg.money_drop	 =	ATOUL_( vecString[ 29 ] ) ;//金币掉率
			attrCfg.luck_def	 =	ATOUL_( vecString[ 30 ] ) ;//幸运防御
			_mapAttrUpgradeCfg.insert( make_pair( (attrCfg.job_id<<16) | attrCfg.level, attrCfg ) );

		}
	);

	return true;
}

CONFIG::ATTR_UPGRADE_CFG* CONFIG::getAttrCfg( WORD job, WORD level)
{
	auto it = _mapAttrUpgradeCfg.find( (job<<16) | level );
	if ( it == _mapAttrUpgradeCfg.end() )
		return NULL;

	return &it->second;
}

BYTE CONFIG::getMaxContinueSignTimes()
{
	return _byMaxContinueSignTimes;
}

bool CONFIG::readCfgMonster(void)
{
//编号	名称	美术资源	等级	技能	经验	金币	掉落索引	战斗力	耐力	力量	敏捷	智慧	精神	生命值上限	魔法值上限	攻击最大值	攻击最小值	防御最大值	防御最小值	技能攻击最大值	技能攻击最小值	技能防御最大值	技能防御最小值	命中值	闪避值	暴击值	暴击抵抗值	暴击伤害增加值	暴击伤害减免值	生命恢复	魔法恢复	幸运	攻击吸血	攻击吸蓝

//id	name	asset	level	skill_id	exp	gold	drop_id	battle_point	stamina	strength	agility	intellect	spirit	hp_max	mp_max	atk_max	atk_min	def_max	def_min	spellatk_max	spellatk_min	spelldef_max	spelldef_min	hit	dodge	crit	crit_res	crit_inc	crit_red	hp_rec	mp_rec	luck	att_hem	att_mana

	string strIniFile = getConfigDir() + "monster.ini";

	_mapMonsterCfg.clear();

	eachIniSecLine(strIniFile, "monster", [&](  const char* line )
		{
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 36 )	
				return;
			
			MONSTER_CFG &monsterCfg = _mapMonsterCfg[ ATOW_( vecString[ 0 ] ) ];

			monsterCfg.id = ATOW_( vecString[ 0 ] );
			monsterCfg.name = vecString[ 1 ];
			monsterCfg.asset = ATOW_( vecString[ 2 ] );
			monsterCfg.level = ATOW_( vecString[ 3 ] );
			monsterCfg.skill_id = ATOW_( vecString[ 4 ] );
			monsterCfg.exp = ATOUL_( vecString[ 5 ]);
			monsterCfg.gold = ATOUL_( vecString[ 6 ]);
			monsterCfg.drop_id = ATOW_( vecString[ 7 ] );
			monsterCfg.dwFightValue = ATOUL_( vecString[ 8 ]);
			monsterCfg.stamina      = ATOUL_( vecString[ 9 ]);
			monsterCfg.strength		= ATOUL_( vecString[ 10 ]);
			monsterCfg.agility		= ATOUL_( vecString[ 11 ]);
			monsterCfg.intellect	= ATOUL_( vecString[ 12 ]);
			monsterCfg.spirit		= ATOUL_( vecString[ 13 ]);
			monsterCfg.hp_max		= ATOUL_( vecString[ 14 ]);
			monsterCfg.mp_max		= ATOUL_( vecString[ 15 ]);
			monsterCfg.atk_max		= ATOUL_( vecString[ 16 ]);
			monsterCfg.atk_min		= ATOUL_( vecString[ 17 ]);
			monsterCfg.def_max		= ATOUL_( vecString[ 18 ]);
			monsterCfg.def_min		= ATOUL_( vecString[ 19 ]);
			monsterCfg.spellatk_max	= ATOUL_( vecString[ 20 ]);
			monsterCfg.spellatk_min	= ATOUL_( vecString[ 21 ]);
			monsterCfg.spelldef_max	= ATOUL_( vecString[ 22 ]);
			monsterCfg.spelldef_min	= ATOUL_( vecString[ 23 ]);
			monsterCfg.hit			= ATOUL_( vecString[ 24 ]);
			monsterCfg.dodge		= ATOUL_( vecString[ 25 ]);
			monsterCfg.crit			= ATOUL_( vecString[ 26 ]);
			monsterCfg.crit_res		= ATOUL_( vecString[ 27 ]);
			monsterCfg.crit_inc		= ATOUL_( vecString[ 28 ]);
			monsterCfg.crit_red		= ATOUL_( vecString[ 29 ]);
			monsterCfg.hp_rec		= ATOUL_( vecString[ 30 ]);
			monsterCfg.mp_rec		= ATOUL_( vecString[ 31 ]);
			monsterCfg.luck			= ATOUL_( vecString[ 32 ]);
			monsterCfg.att_hem		= ATOUL_( vecString[ 33 ]);
			monsterCfg.att_mana		= ATOUL_( vecString[ 34 ]);
			monsterCfg.luck_def		= ATOUL_( vecString[ 35 ]);

		}
	);
	return true;
}

bool CONFIG::readCfgSuit(void)
{
	string strIniFile = getConfigDir() + "suit.ini";

	_multimapSuitCfg.clear();

	eachIniSecLine(strIniFile, "suit", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 16 )	
			return;

		SUIT_CFG suitCfg;
		suitCfg.ID = ATOW_( vecString[ 0 ] );
		suitCfg.suit_id  = ATOW_( vecString[ 1 ] );
		suitCfg.suit_number = ATOW_( vecString[ 2 ] );
		for (size_t i = 3; i < 10; i+=2 )
		{
			//suitCfg.vecStrength.push_back( ATOW_( vecString[ i ] ) );

			vector< SKILL_EFFECT >  vecEffectAttr;
			const string& para_effect = vecString[ i+1 ];
			{
				vector<string> vecEffectType;
				assert( strtoken2Vec( para_effect.c_str(), ";", vecEffectType, NULL, NULL) > 0 );
				for ( auto& it : vecEffectType )
				{
					vector<string> vecTypePara;

					assert( strtoken2Vec( it.c_str(), ",", vecTypePara, NULL, NULL) == 2 );

					BYTE addType = E_ATTR_ADD;     
					BYTE attr = ATOB_(vecTypePara[0]);
					assert( attr >= E_ATTR_HP_MAX2 && attr < E_ATTR_MAX );
					vecEffectAttr.push_back( SKILL_EFFECT( addType, attr,  ATOW_(vecTypePara[1]),  0.0f ) );

				}
			}
			//suitCfg.vvecEffectAttr.push_back( vecEffectAttr );
			suitCfg.mapStrengthEffAttr.insert( make_pair( ATOW_( vecString[1]), vecEffectAttr ));

		}

		_multimapSuitCfg.insert( make_pair( suitCfg.suit_id, suitCfg ) );
	}
	);
	return true;

}

bool CONFIG::readCfgArenaBouns(void)
{
	string strIniFile = getConfigDir() + "arena_bouns.ini";

	_mapArenaBounsCfg.clear();

	eachIniSecLine(strIniFile, "arena_bouns", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 4 )	
			return;

		ARENA_BOUNS_CFG &arenaBounsCfg = _mapArenaBounsCfg[ ATOUL_( vecString[ 1 ] ) ];
		arenaBounsCfg.rank = ATOUL_( vecString[ 1 ] );
		const string& paramone = vecString[ 2 ];
		vector<string> vAttr1;
		int nums = strtoken2Vec( paramone.c_str(), ",", vAttr1, NULL, NULL);
		assert(nums = 2);
		arenaBounsCfg.goldID = ATOW_( vAttr1[ 0 ] );
		arenaBounsCfg.goldNum = ATOUL_( vAttr1[ 1 ] );

		const string& paramtwo = vecString[ 3 ];
		vector<string> vAttr2;
		nums = strtoken2Vec( paramtwo.c_str(), ",", vAttr2, NULL, NULL);
		assert(nums = 2);
		arenaBounsCfg.itemID = ATOW_( vAttr2[ 0 ] );
		arenaBounsCfg.itemNum = ATOW_( vAttr2[ 1 ] );

	}
	);
	return true;
}

bool CONFIG::readCfgSecd7DayAward(void)
{
	string strIniFile = getConfigDir() + "7_day_ranking.ini";

	_mapSend7DayAwardCfg.clear();

	eachIniSecLine(strIniFile, "7_day_ranking", [&](const char* line)
	{
		vector<string> vecString;
		if (strtoken2Vec(line, SPLIT_SPACE, vecString, NULL, NULL) <5)
			return;

		SEND_7DAY_AWARD_CFG &sendAwardCfg = _mapSend7DayAwardCfg[ATOUL_(vecString[1])];
		sendAwardCfg.rank = ATOUL_(vecString[1]);
		const string& param1 = vecString[2];
		vector<string> vAttr1;
		int nums = strtoken2Vec(param1.c_str(), ",", vAttr1, NULL, NULL);
		assert(nums = 2);
		sendAwardCfg.goldID = ATOW_(vAttr1[0]);
		sendAwardCfg.goldNum = ATOUL_(vAttr1[1]);

		const string& param2 = vecString[3];
		vector<string> vAttr2;
		nums = strtoken2Vec(param2.c_str(), ",", vAttr2, NULL, NULL);
		assert(nums = 2);
		sendAwardCfg.itemID1 = ATOW_(vAttr2[0]);
		sendAwardCfg.itemNum1 = ATOW_(vAttr2[1]);

		const string& param3 = vecString[4];
		vector<string> vAttr3;
		nums = strtoken2Vec(param3.c_str(), ",", vAttr3, NULL, NULL);
		assert(nums = 2);
		sendAwardCfg.itemID2 = ATOW_(vAttr3[0]);
		sendAwardCfg.itemNum2 = ATOW_(vAttr3[1]);

	}
	);
	return true;
}


bool CONFIG::readCfgArenaReward(void)
{
	//rank	win_gold	win_money	lose_gold	lose_money

	string strIniFile = getConfigDir() + "arena_reward.ini";

	_mapArenaRewardCfg.clear();

	eachIniSecLine(strIniFile, "arena_reward", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 5 )	
			return;
		DWORD rank = ATOUL_(vecString[0]);
		ARENA_REWARD_CFG& arenaRewardCfg = _mapArenaRewardCfg[rank];
		arenaRewardCfg.win_gold = ATOUL_( vecString[ 1 ] );
		arenaRewardCfg.win_money = ATOUL_( vecString[ 2 ] );
		arenaRewardCfg.lose_gold = ATOUL_( vecString[ 3 ] );
		arenaRewardCfg.lose_money = ATOUL_( vecString[ 4 ] );
	}
	);
	return true;
}

bool CONFIG::readCfgSign(void)
{
	//month	times	description	asset	vip_bonus	item_id	
	string strIniFile = getConfigDir() + "sign.ini";

	_mapSignCfg.clear();

	eachIniSecLine(strIniFile, "sign", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 7 )	
			return;
		BYTE job = ATOB_( vecString[ 0 ] );
		BYTE sex = ATOB_( vecString[ 1 ] );
		BYTE month = ATOB_( vecString[ 2 ] );
		BYTE times = ATOB_( vecString[ 3 ] );

		//SIGN_CFG &signCfg = _mapSignCfg[ job<<24 | sex<<16 | month<<8 | times ];
		SIGN_CFG signCfg;
		signCfg.job = job;
		signCfg.sex = sex;
		signCfg.month = month;
		signCfg.times = times;
		//signCfg.vip_bonus = ATOB_( vecString[ 6 ] );
		const string& parameter = vecString[ 6 ];
		vector<string> vAtt;
		int nums = strtoken2Vec( parameter.c_str(), ",", vAtt, NULL, NULL);
		assert( nums == 2 );
		signCfg.item_id = ATOW_( vAtt[ 0 ] );
		signCfg.item_num = ATOUL_( vAtt[ 1 ] );

		if ( job!= 4 )//留着给连续签到奖励的
		{
			_mapSignCfg.insert( make_pair( job<<24 | sex<<16 | month<<8 | times , signCfg ) );
		}
		else
		{
			_byMaxContinueSignTimes = times;
			_mapSignTimeCfg.insert( make_pair( times, signCfg ) );
		}
	}
	);
	return true;
}

bool CONFIG::readCfgLuck(void)
{
	//等级	描述	升级经验	快速战斗购买次数	摇钱树次数	技能快速修炼	VIP称号	竞技场购买次数	VIP礼包购买	挖矿抢夺次数购买	挖矿加速购买	公会额外贡献度
	//level	description	upgrade_exp	time_buy	money_buy	skill_speed	vip_name	match_buy	gift_buy	rob_times	dig_times	bonus_contribution

	string strIniFile = getConfigDir() + "luck.ini";

	_mapLuckCfg.clear();

	eachIniSecLine(strIniFile, "luck", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 2 )	
			return;

		_mapLuckCfg[ ATOUL_( vecString[ 0 ] ) ] = ATOF_( vecString[ 1 ] ) / 10000.0f ;
	}
	);
	return true;
}

bool CONFIG::readCfgCharge(void)
{
	//等级	描述	升级经验	快速战斗购买次数	摇钱树次数	技能快速修炼	VIP称号	竞技场购买次数	VIP礼包购买	挖矿抢夺次数购买	挖矿加速购买	公会额外贡献度
	//level	description	upgrade_exp	time_buy	money_buy	skill_speed	vip_name	match_buy	gift_buy	rob_times	dig_times	bonus_contribution

	string strIniFile = getConfigDir() + "charge.ini";

	_mapChargeCfg.clear();

	eachIniSecLine(strIniFile, "charge", [&](const char* line)
	{
		vector<string> vecString;
		if (strtoken2Vec(line, SPLIT_SPACE, vecString, NULL, NULL) < 2)
			return;

		_mapChargeCfg[ATOUL_(vecString[0])] = ATOUL_(vecString[1]);
	}
	);
	return true;
}


bool CONFIG::readCfgMoneyTree(void)
{
	//等级	描述	升级经验	快速战斗购买次数	摇钱树次数	技能快速修炼	VIP称号	竞技场购买次数	VIP礼包购买	挖矿抢夺次数购买	挖矿加速购买	公会额外贡献度
	//level	description	upgrade_exp	time_buy	money_buy	skill_speed	vip_name	match_buy	gift_buy	rob_times	dig_times	bonus_contribution

	string strIniFile = getConfigDir() + "moneytree.ini";

	_mapMoneyTreeCfg.clear();

	eachIniSecLine(strIniFile, "moneytree", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 2 )	
			return;

		_mapMoneyTreeCfg[ ATOW_( vecString[ 0 ] ) ] = ATOUL_( vecString[ 1 ] ) ;
	}
	);
	return true;
}

bool CONFIG::readCfgMoneyTree0(void)
{
	//等级	描述	升级经验	快速战斗购买次数	摇钱树次数	技能快速修炼	VIP称号	竞技场购买次数	VIP礼包购买	挖矿抢夺次数购买	挖矿加速购买	公会额外贡献度
	//level	description	upgrade_exp	time_buy	money_buy	skill_speed	vip_name	match_buy	gift_buy	rob_times	dig_times	bonus_contribution

	string strIniFile = getConfigDir() + "moneytree0.ini";

	_mapMoneyTree0Cfg.clear();

	eachIniSecLine(strIniFile, "moneytree0", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 3 )	
			return;
		MONEY_TREE0_CFG &moneyTree0Cfg = _mapMoneyTree0Cfg[ ATOW_( vecString[ 0 ] ) ];
		moneyTree0Cfg.mul = ATOW_(vecString[2]);
		moneyTree0Cfg.cost = ATOUL_(vecString[1]);
	}
	);
	return true;
}

bool CONFIG::readCfgVipAdd(void)
{
	//等级	描述	升级经验	快速战斗购买次数	摇钱树次数	竞技场购买次数	BOSS挑战次数	快速战斗收益	最大占卜次数	商城额外物品栏	购买精英副本次数	符文槽额外开启	升级条件2	每日登录送元宝
	//level	description	upgrade_exp	time_buy	money_buy	match_buy	boss_challenge	battle_reward	divine_time	shop_basket	elite_time	rune_basket	require	free_gold

	string strIniFile = getConfigDir() + "vip.ini";

	_mapVipAddCfg.clear();

	eachIniSecLine(strIniFile, "vip", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 17 )	
			return;

		VIP_ADDITION_CFG &vipAddCfg = _mapVipAddCfg[ ATOW_( vecString[ 0 ] ) ];
		vipAddCfg.level = ATOW_( vecString[ 0 ] );
		vipAddCfg.description = vecString[ 1 ] ;
		vipAddCfg.upgrade_exp = ATOUL_( vecString[ 2 ] );
		vipAddCfg.time_buy = ATOB_( vecString[ 3 ] );
		vipAddCfg.money_buy = ATOB_( vecString[ 4 ] ); 
		vipAddCfg.match_buy = ATOB_( vecString[ 5 ] );
		vipAddCfg.boss_challenge = ATOB_( vecString[ 6 ] );
		vipAddCfg.battle_reward = ATOB_( vecString[ 7 ] );
		vipAddCfg.divine_time = ATOB_( vecString[ 8 ] );
		vipAddCfg.shop_basket = ATOB_( vecString[ 9 ] );
		vipAddCfg.elite_time = ATOB_( vecString[ 10 ] );
		vipAddCfg.rune_basket = ATOB_( vecString[ 11 ] );
		vipAddCfg.require =  vecString[ 12 ] ;
		vipAddCfg.free_gold = ATOUL_( vecString[ 13 ] );
		vipAddCfg.worship_time = ATOW_( vecString[ 16 ]);
	}
	);
	return true;
}

bool CONFIG::readCfgActivityInfo(void)
{
//编号	类型	名称	等级限制	活动提前报名时间	活动开启时间	活动关闭时间	怪物id
//id	type	name	open_level	advance_time	begin_time	end_time	monster_id

	string strIniFile = getConfigDir() + "campaign.ini";

	_mapActivityInfoCfg.clear();

	eachIniSecLine(strIniFile, "campaign", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 8 )	
			return;

		ACTIVITY_INFO_CFG &activityInfoCfg = _mapActivityInfoCfg[ ATOW_( vecString[ 0 ] ) ];

		activityInfoCfg.id = ATOW_( vecString[ 0 ] );
		activityInfoCfg.type = ATOB_( vecString[ 1 ] );
		activityInfoCfg.open_level = ATOW_( vecString[ 3 ] );

		const string& advance_time = vecString[ 4 ];
		vector<string> vAtt1;
		int nums1 = strtoken2Vec( advance_time.c_str(), ":", vAtt1, NULL, NULL);
		assert( nums1 == 3 );
		activityInfoCfg.advance_time = ATOW_( vAtt1[ 0 ] ) * 60 * 60 + ATOW_( vAtt1[ 1 ] ) * 60 + ATOW_( vAtt1[ 2 ] );

		const string& begin_time = vecString[ 5 ] ;
		vector<string> vAtt2;
		int nums2 = strtoken2Vec( begin_time.c_str(), ":", vAtt2, NULL, NULL);
		assert( nums2 == 3 );
		activityInfoCfg.begin_time = ATOW_( vAtt2[ 0 ] ) * 60 * 60 + ATOW_( vAtt2[ 1 ] ) * 60 + ATOW_( vAtt2[ 2 ] );
		
		const string& end_time = vecString[ 6 ] ;
		vector<string> vAtt3;
		int nums3 = strtoken2Vec( end_time.c_str(), ":", vAtt3, NULL, NULL);
		assert( nums3 == 3 );
		activityInfoCfg.end_time = ATOW_( vAtt3[ 0 ] ) * 60 * 60 + ATOW_( vAtt3[ 1 ] ) * 60 + ATOW_( vAtt3[ 2 ] );

		activityInfoCfg.monster_id = ATOW_( vecString[ 7 ] );
	}
	);
	return true;
}


bool CONFIG::readCfgEffect(void)
{
	//编号	效果类型编号	效果等级	效果名称	效果参数
	//id	type	level	name	parameter_add

	string strIniFile = getConfigDir() + "effect.ini";

	_mapSkillEffCfg.clear();
	eachIniSecLine(strIniFile, "effect", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 5 )	
			return;

		SKILL_EFF_CFG skillEffCfg;
		skillEffCfg.id = ATOW_( vecString[ 0 ] );
		skillEffCfg.name = vecString[ 3 ];
		skillEffCfg.type = ATOB_( vecString[ 1 ] );
		assert( skillEffCfg.type < E_SKILL_EFFECT_MAX );
		skillEffCfg.level = ATOB_( vecString[ 2 ] );

		const string& para_effect = vecString[ 4 ];
		{
			vector<string> vecEffectType;
			strtoken2Vec( para_effect.c_str(), ";", vecEffectType, NULL, NULL);
			for ( auto& it : vecEffectType )
			{
				vector<string> vecTypePara;

				strtoken2Vec( it.c_str(), ",", vecTypePara, NULL, NULL);
				if( vecTypePara.size() == 2 )
				{
					skillEffCfg.vecEffectAttr.push_back( SKILL_EFFECT( 0, 0,  ATOW_(vecTypePara[0]), ATOF_(vecTypePara[1]) ) );//构造会/100.0f
				}
				else if( vecTypePara.size() == 4 )
				{
					////增减类型,属性编号,值,百分比;
					BYTE addType = ATOB_(vecTypePara[0]);     
					BYTE attr = ATOB_(vecTypePara[1]);
					assert( addType == E_ATTR_ADD || addType == E_ATTR_REDUCE );
					assert( attr >= E_ATTR_HP_MAX2 && attr < E_ATTR_MAX );
					skillEffCfg.vecEffectAttr.push_back( SKILL_EFFECT( addType, attr,  ATOW_(vecTypePara[2]), ATOF_(vecTypePara[3]) ) );//构造会/100.0f
				}
			}
		}

		_mapSkillEffCfg.insert( make_pair(skillEffCfg.type << 8 | skillEffCfg.level, skillEffCfg ) );

	}
	);

	return true;
}


bool CONFIG::readCfgSkill(void)
{

//#编号	名称	职业限制	技能组	技能等级	解锁等级	学习方式	说明	类型	参数	消耗	冷却时间	优先级	修炼时间	作用目标类型	额外状态概率	额外状态类型	回合	额外状态参数
//#id	name	job	group	level	unlock_level	study	description	type	parameter	mp_cost	cooldown	priority	levelup_exp	target	eff_rate	eff_type	round	parameter_add
	string strIniFile = getConfigDir() + "skill.ini";

	_mapSkillCfg.clear();
	eachIniSecLine(strIniFile, "skill", [&](  const char* line )
		{
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 19 )	
				return;
			
			SKILL_CFG2 skillCfg;
			skillCfg.id = ATOW_( vecString[ 0 ] );
			skillCfg.name = vecString[ 1 ];
			skillCfg.job = ATOB_( vecString[ 2 ] );
			skillCfg.group = ATOW_( vecString[ 3 ] );
			skillCfg.level = ATOW_( vecString[ 4 ] );
			skillCfg.unlock_level = ATOW_( vecString[ 5 ] );
			skillCfg.study = ATOB_( vecString[ 6 ] );
			skillCfg.description = vecString[ 7 ];
			skillCfg.type = ATOB_( vecString[ 8 ] );

			assert( skillCfg.type <= E_SKILL_TYPE_LESSMP );

			const string& parameter = vecString[ 9 ];
			{
				//技能的类型：1攻击类技能,2治疗类技能,3附加效果类技能,4普通攻击,5攻击吸蓝
				skillCfg.skillPara.skillType = 0;
				skillCfg.skillPara.skillTarget = 1;
				skillCfg.skillPara.skillAtt = 0;
				skillCfg.skillPara.skillCoff = 1.0f;
				skillCfg.skillPara.skillMpAtt = 0;
				skillCfg.skillPara.skillMpCoff = 1.0f;

				vector<string> v;
				int nums = strtoken2Vec( parameter.c_str(), ",", v, NULL, NULL);

				if ( skillCfg.type == E_SKILL_TYPE_ATTACK || skillCfg.type == E_SKILL_TYPE_TREAT || skillCfg.type == E_SKILL_TYPE_GENERAL )
				{
					assert( nums == 5 );
					skillCfg.skillPara.skillType = ATOB_( v[1] );
					skillCfg.skillPara.skillTarget = ATOB_( v[2] );
					skillCfg.skillPara.skillAtt = ATOW_( v[3] );
					skillCfg.skillPara.skillCoff = ATOW_( v[4] )/100.0f;
				}
				else if ( skillCfg.type == E_SKILL_TYPE_EFFECT )
				{

				}
				else if ( skillCfg.type == E_SKILL_TYPE_LESSMP )
				{
					assert( nums == 7 );
					skillCfg.skillPara.skillType = ATOB_( v[1] );
					skillCfg.skillPara.skillTarget = ATOB_( v[2] );
					skillCfg.skillPara.skillAtt = ATOW_( v[3] );
					skillCfg.skillPara.skillCoff = ATOW_( v[4] )/100.0f;
					skillCfg.skillPara.skillMpAtt = ATOW_( v[5] );
					skillCfg.skillPara.skillMpCoff = ATOW_( v[6] )/100.0f;
				}
			}


			skillCfg.mp_cost = ATOW_( vecString[ 10 ] );
			skillCfg.cooldown = ATOW_(vecString[ 11 ]);
			skillCfg.priority = ATOW_(vecString[ 12 ]);
			skillCfg.levelup_exp = ATOW_(vecString[13]);
			skillCfg.target = ATOB_( vecString[ 14 ] );

			skillCfg.eff_rate = ATOW_( vecString[ 15 ] ) / 100.0f;
			skillCfg.eff_type = ATOB_( vecString[ 16 ] );

			assert( skillCfg.eff_type < E_SKILL_EFFECT_MAX );

			skillCfg.round = ATOB_( vecString[ 17 ] );

			//const string& para_effect = vecString[ 18 ];
			//{
			//	skillCfg.extra_coff = 1.0f;

			//	vector<string> vecEffectType;
			//	assert( strtoken2Vec( para_effect.c_str(), ";", vecEffectType, NULL, NULL) > 0 );

			//	if ( skillCfg.eff_type == E_SKILL_EFFECT_MONATTACKTWO )
			//	{
			//		assert( vecEffectType.size() == 1 );
			//		skillCfg.extra_coff = ATOW_( para_effect ) / 100.0f;
			//	}
			//	else
			//	{
			//		for ( auto& it : vecEffectType )
			//		{
			//			vector<string> vecTypePara;
			//			strtoken2Vec( it.c_str(), ",", vecTypePara, NULL, NULL);
			//			if( vecTypePara.size() == 4 )
			//			{
			//				BYTE addType =  ATOB_(vecTypePara[0]);     
			//				BYTE attr = ATOB_(vecTypePara[1]);
			//				assert( attr >= E_ATTR_HP && attr < E_ATTR_MAX );
			//				skillCfg.vecEffectAttr.push_back( SKILL_EFFECT( addType, attr, ATOW_(vecTypePara[2]),  ATOW_(vecTypePara[3]) ) );
			//			}
			//		}
			//	}

			//}

			// 消耗技能书数量 
			const string& update_condition = vecString[18];
			{
				vector<string> vAtt;
				strtoken2Vec( update_condition.c_str(), ",", vAtt, NULL, NULL);
				for (DWORD i=0; i<vAtt.size(); i++)
				{
					if (i==0)
					{
						skillCfg.skill_book.itemid = ATOW_( vAtt[i] );
					}
					else if (i==1)
					{
						skillCfg.skill_book.count = ATOW_( vAtt[i] );
					}
				}
				
			}

			_mapSkillCfg.insert( make_pair(skillCfg.id, skillCfg) );

			if ( skillCfg.type != E_SKILL_TYPE_GENERAL )
			{
				if ( skillCfg.job == E_JOB_WARRIOR || skillCfg.job == E_JOB_MAGIC || skillCfg.job == E_JOB_MONK )
				{
					SKILL_STORAGE& skillStorage = _mapJobGroupSkillCfg[ skillCfg.job ];

					vector<SKILL_CFG2*>& vecGroupSkillCfg = skillStorage.mapGroupSkills[ skillCfg.group ];

					if ( vecGroupSkillCfg.empty() )
					{
						skillStorage.vecGroupID.push_back( skillCfg.group );
					}

					vecGroupSkillCfg.push_back( &_mapSkillCfg[ skillCfg.id ] );
				}
				
			}

			
	
		}
	);


	return true;
}



bool CONFIG::readCfgItem()
{
	string strIniFile = getConfigDir() + "item.ini";

	_mapItemCfg.clear();
	_mapRuneQualitys.clear();

	eachIniSecLine(strIniFile, "item", [&](  const char* line )
		{
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 47 )	
				return;
			ITEM_CFG itemCfg;
			itemCfg.id = ATOW_( vecString[ 0 ] );		
			itemCfg.name = vecString[ 1 ] ;		
			itemCfg.asset = ATOW_( vecString[ 2 ] );		
			itemCfg.type = ATOW_( vecString[ 3 ] );		
			itemCfg.sub_type = ATOW_( vecString[ 4 ] );	
			//itemCfg.description = vecString[ 5 ] ;
			itemCfg.count = ATOW_( vecString[ 5 ] );		
			itemCfg.job_limit = ATOW_( vecString[ 6 ] );	
			itemCfg.sex_limit = ATOW_( vecString[ 7 ] );	
			itemCfg.level_limit = ATOW_( vecString[ 8 ] );
			itemCfg.rare = ATOW_( vecString[ 9 ] );		
			itemCfg.price = ATOUL_( vecString[ 10 ] );
			itemCfg.att_random = ATOUL_( vecString[ 11 ] );
			const string& parameter = vecString[ 12 ];
			vector<string> v;
			int nums = strtoken2Vec( parameter.c_str(), ",", v, NULL, NULL);
			assert( nums == 2 );
			itemCfg.att_times.min_times = ATOW_( v[0] );
			itemCfg.att_times.max_times = ATOW_( v[1] );
			itemCfg.att_rate = ATOUL_( vecString[ 13 ] );
			//itemCfg.addAttrType = ATOB_( vecString[ 14 ] );

			itemCfg.stamina   = ATOUL_( vecString[ 15 ] );
			itemCfg.strength  = ATOUL_( vecString[ 16 ] );
			itemCfg.agility	  = ATOUL_( vecString[ 17 ] );
			itemCfg.intellect = ATOUL_( vecString[ 18 ] );
			itemCfg.spirit	  = ATOUL_( vecString[ 19 ] );
			//if (IS_EQUIP_GEM(&itemCfg))
			//{
				if ( itemCfg.stamina > 0 )
				{
					itemCfg.addAttrType = E_ATTR_STAMINA;
					itemCfg.addAttrValue = itemCfg.stamina;
				}
				if ( itemCfg.strength > 0 )
				{
					itemCfg.addAttrType = E_ATTR_STRENGTH;
					itemCfg.addAttrValue = itemCfg.strength;
				}
				if ( itemCfg.agility > 0 )
				{
					itemCfg.addAttrType = E_ATTR_AGILITY;
					itemCfg.addAttrValue = itemCfg.agility;
				}
				if ( itemCfg.intellect > 0 )
				{
					itemCfg.addAttrType = E_ATTR_INTELLECT;
					itemCfg.addAttrValue = itemCfg.intellect;
				}
				if ( itemCfg.spirit > 0 )
				{
					itemCfg.addAttrType = E_ATTR_SPIRIT;
					itemCfg.addAttrValue = itemCfg.spirit;
				}
			//}

			itemCfg.mapBaseAttrValue[ E_ATTR_STAMINA   ] = itemCfg.stamina  ;
			itemCfg.mapBaseAttrValue[ E_ATTR_STRENGTH  ] = itemCfg.strength ;
			itemCfg.mapBaseAttrValue[ E_ATTR_AGILITY   ] = itemCfg.agility	 ;
			itemCfg.mapBaseAttrValue[ E_ATTR_INTELLECT ] = itemCfg.intellect;
			itemCfg.mapBaseAttrValue[ E_ATTR_SPIRIT	   ] = itemCfg.spirit	 ;
			//E_ATTR_STAMINA	     =  22,//耐力
			//E_ATTR_STRENGTH	     =  23,//力量
			//E_ATTR_AGILITY	     =  24,//敏捷
			//E_ATTR_INTELLECT     =  25,//智慧
			//E_ATTR_SPIRIT		 =  26,//精神

			itemCfg.hp_max       = ATOUL_( vecString[ 20 ] );	
			itemCfg.mp_max		 = ATOUL_( vecString[ 21 ] );	
			itemCfg.atk_max		 = ATOUL_( vecString[ 22 ] );	
			itemCfg.atk_min		 = ATOUL_( vecString[ 23 ] );	
			itemCfg.def_max		 = ATOUL_( vecString[ 24 ] );	
			itemCfg.def_min		 = ATOUL_( vecString[ 25 ] );	
			itemCfg.spellatk_max = ATOUL_( vecString[ 26 ] );	
			itemCfg.spellatk_min=  ATOUL_( vecString[ 27 ] );	
			itemCfg.spelldef_max=  ATOUL_( vecString[ 28 ] );	
			itemCfg.spelldef_min=  ATOUL_( vecString[ 29 ] );	
			itemCfg.hit			 = ATOUL_( vecString[ 30 ] );	
			itemCfg.dodge		 = ATOUL_( vecString[ 31 ] );	
			itemCfg.crit		 = ATOUL_( vecString[ 32 ] );	
			itemCfg.crit_res	 = ATOUL_( vecString[ 33 ] );	
			itemCfg.crit_inc	 = ATOUL_( vecString[ 34 ] );	
			itemCfg.crit_red	 = ATOUL_( vecString[ 35 ] );	
			itemCfg.mp_rec		 = ATOUL_( vecString[ 36 ] );	
			itemCfg.hp_rec		 = ATOUL_( vecString[ 37 ] );	
			itemCfg.luck =		    ATOW_( vecString[ 38 ] );
			itemCfg.att_hem	    =  ATOUL_( vecString[ 39 ] );	
			itemCfg.att_mana	=  ATOUL_( vecString[ 40 ] );	
			itemCfg.equ_drop	=  ATOUL_( vecString[ 41 ] );	
			itemCfg.money_drop	=  ATOUL_( vecString[ 42 ] );	
			itemCfg.luck_def	=  ATOUL_( vecString[ 43 ] );

			itemCfg.strengthen	=  vecString[ 44 ] ;
			{
				vector<string> vAtt;
				strtoken2Vec( itemCfg.strengthen.c_str(), ",", vAtt, NULL, NULL);
				if (vAtt.size() == 3)
				{
					itemCfg.attType	= ATOB_( vAtt[0] );
					itemCfg.attValue  = ATOUL_( vAtt[1] );
					itemCfg.strengthen_stones  = ATOUL_( vAtt[2] );	
				}
			}

			const string& strHole = vecString[45];
			vector<string> vecCfgHole;
			strtoken2Vec( strHole.c_str(), ",", vecCfgHole, NULL, NULL);
			if ( vecCfgHole.size() == 1 && ATOW_( vecCfgHole[ 0 ] ) == E_NO_HOLE )
			{
				itemCfg.zeroHole = E_NO_HOLE;
			}
			else
			{
				itemCfg.zeroHole = E_HAVE_HOLE;
				for ( size_t i = 0; i < vecCfgHole.size(); i++ )
				{
					itemCfg.vecHole.push_back( i );
					itemCfg.vecHoleRandSum.push_back( ATOW_( vecCfgHole[ i ] ) );
				}
				partial_sum( itemCfg.vecHoleRandSum.begin(), itemCfg.vecHoleRandSum.end(), itemCfg.vecHoleRandSum.begin() );
			}

			itemCfg.suit = ATOW_( vecString[ 46 ] );

			auto it = _mapItemCfg.find( itemCfg.id );
			if ( it != _mapItemCfg.end() )
			{
				it->second = itemCfg;
			}
			else
				_mapItemCfg.insert( make_pair( itemCfg.id,  itemCfg) );

			if (itemCfg.type == TYPE_FUWEN)
			{
				auto itr=_mapRuneQualitys.find(itemCfg.rare);
				if (itr == _mapRuneQualitys.end())
				{
					std::vector<WORD> vecItems;
					vecItems.push_back(itemCfg.id);
					_mapRuneQualitys.insert(std::make_pair(itemCfg.rare, vecItems));
				}
				else
				{
					itr->second.push_back(itemCfg.id);
				}
			}
		}
	);	
	return true;
}

bool CONFIG::readCfgPlayer()
{
	string strIniFile = getConfigDir() + "player.ini";

	_mapPlayerCfg.clear();
	eachIniSecLine(strIniFile, "player", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 12 )	
			return;

		PLAYER_CFG playerCfg;

		playerCfg.id = ATOW_( vecString[ 0 ] );			
		playerCfg.name = vecString[ 1 ] ;		
		playerCfg.level = ATOW_( vecString[ 2 ] );		
		playerCfg.exp = ATOW_( vecString[ 3 ] );		
		playerCfg.asset = ATOW_( vecString[ 4 ] );		
		playerCfg.money = ATOW_( vecString[ 5 ] );		
		playerCfg.gold = ATOW_( vecString[ 6 ] );		
		playerCfg.power = ATOW_( vecString[ 7 ] );		
		playerCfg.power_limit = ATOW_( vecString[ 8 ] );
		playerCfg.lead = ATOW_( vecString[ 9 ] );		
		playerCfg.lead_limit = ATOW_( vecString[ 10 ] );	
		playerCfg.vip_level = ATOW_( vecString[ 11 ] );	

		_mapPlayerCfg.insert( make_pair( playerCfg.id,  playerCfg) );

	}
	);
	return true;
}


//10,50;11,50
void CONFIG::parseItemInfoStr( const string& str, vector<ITEM_INFO>& vecItemInfo )
{
	vector<string> v;
	strtoken2Vec( str.c_str(), ";", v, NULL, NULL);
	
	for(auto& str : v)
	{
		vector<string> item;
		strtoken2Vec( str.c_str(), ",", item, NULL, NULL);
		if(item.size() < 2)continue;
		auto id = atoi(item[0].c_str());
		if(id == 0)continue;
		if(item.size() == 2)
			vecItemInfo.push_back( ITEM_INFO((uint16_t)id, ATOUL_( item[1] ) ) );
		else if(item.size() == 3)
			vecItemInfo.push_back( ITEM_INFO((uint16_t)id, ATOUL_( item[1] ), (byte)ATOUL_( item[2] ) ) );
	}
}

//1,11005,5,1;2,12005,5,1;3,11105,5,1
void CONFIG::parseJobEquipInfoStr( const string& str, vector<ITEM_INFO>& vecItemInfo, int size )
{
	vector<string> v;

	int nums = strtoken2Vec( str.c_str(), ",;", v, NULL, NULL );

	assert( nums / 4 == size );

	for ( int i = 0; i < nums; i += 4 )
	{
		vecItemInfo.push_back( ITEM_INFO( ATOW_( v[i+1] ), ATOUL_( v[i+2] ), ATOB_( v[i+3] ) ) );
	}
}

//11005,5,1
void CONFIG::parseEquipInfoStr( const string& str, vector<ITEM_INFO>& vecItemInfo )
{
	vector<string> v;

	int nums = strtoken2Vec( str.c_str(), ",;", v, NULL, NULL );
	
	RETURN_VOID( nums % 3 != 0 || nums < 3 );

	for ( int i = 0; i < nums; i += 3 )
	{
		vecItemInfo.push_back( ITEM_INFO( ATOW_( v[i] ), ATOUL_( v[i+1] ), ATOB_( v[i+2] ) ) );
	}
}

int CONFIG::itemInfo2Str( char *buf, const vector<ITEM_INFO>& vecItemInfo )
{
	char* p = buf;

	for ( auto it : vecItemInfo )
	{
		p += sprintf( p, "%u,%u;", it.itemIndex, it.itemNums );
	}

	return p-buf;
}


#if 1

//min,max
void parseMinMax( const string& str, DWORD& dwMin, DWORD& dwMax)
{
	dwMin = dwMax = 0;
	vector<string> v;
	int nums = strtoken2Vec( str.c_str(), ",", v, NULL, NULL);
	if ( nums != 2 )
		return;

	dwMin = ATOUL_(v[0]);
	dwMax = ATOUL_(v[1]);

}

#endif



bool CONFIG::readCfgMap()
{
	string strIniFile = getConfigDir() + "map.ini";

	_mapMapCfg.clear();
	_mapEliteCfg.clear();

	eachIniSecLine(strIniFile, "map", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 18 )	
			return;

		MAP_CFG &mapCfg = _mapMapCfg[ ATOW_( vecString[ 0 ] ) ];

		mapCfg.id = ATOW_( vecString[ 0 ] );		
		mapCfg.name = vecString[ 1 ] ;
		mapCfg.asset = ATOW_( vecString[ 2 ] );		
		mapCfg.level_limit = ATOW_( vecString[ 3 ] );

		vector<ITEM_INFO> vecMonsterInfo;
		parseItemInfoStr( vecString[ 4 ], vecMonsterInfo );


		DWORD monsterSum =0;
		for ( size_t i = 0; i<vecMonsterInfo.size(); i++)
		{
			mapCfg.vecMonsterIndex.push_back( vecMonsterInfo[i].itemIndex );
			mapCfg.vecMonsterRandSum.push_back( vecMonsterInfo[i].itemNums );

			monsterSum += vecMonsterInfo[i].itemNums;

		}
		
		for ( size_t i = 0; i<vecMonsterInfo.size(); i++)
		{
			mapCfg.vecMonsterRatio.push_back( (float) vecMonsterInfo[i].itemNums / monsterSum );
		}
	
		partial_sum( mapCfg.vecMonsterRandSum.begin() , mapCfg.vecMonsterRandSum.end() , mapCfg.vecMonsterRandSum.begin() );
		
		
		parseMinMax( vecString[ 5 ], mapCfg.dwMin, mapCfg.dwMax);


		mapCfg.event = ATOW_( vecString[ 6 ] );
		mapCfg.cool_down = ATOW_( vecString[ 7 ] );		
		mapCfg.boss_id = ATOW_( vecString[ 8 ] );

		const string& paraMonMoney = vecString[ 9 ];
		{
			vector<string> vecMoney;
			assert( strtoken2Vec( paraMonMoney.c_str(), ",", vecMoney, NULL, NULL) == 3 );
			DROP_MONEY tempDropMoney( ATOW_( vecMoney[0])/100.0f, ATOUL_(vecMoney[1]), ATOUL_(vecMoney[2]) );
			mapCfg.stDropMoney[ E_DROP_MON ] = tempDropMoney;
		}

		mapCfg.exp = ATOUL_( vecString[ 10 ] );
		mapCfg.dropID[E_DROP_MON] = ATOW_( vecString[ 11 ] );
		mapCfg.dropID[E_DROP_FIRST_BOSS] = ATOW_( vecString[ 12 ] );

		const string& paraBossFirstMoney = vecString[ 13 ];
		{
			vector<string> vecMoney;
			assert( strtoken2Vec( paraBossFirstMoney.c_str(), ",", vecMoney, NULL, NULL) == 3 );
			DROP_MONEY tempDropMoney( ATOW_( vecMoney[0])/100.0f, ATOUL_(vecMoney[1]), ATOUL_(vecMoney[2]) );
			mapCfg.stDropMoney[ E_DROP_FIRST_BOSS ] = tempDropMoney;
		}

		mapCfg.dropID[E_DROP_BOSS] = ATOW_( vecString[ 14 ] );	

		const string& paraBossMoney = vecString[ 15 ];
		{
			vector<string> vecMoney;
			assert( strtoken2Vec( paraBossMoney.c_str(), ",", vecMoney, NULL, NULL) == 3 );
			DROP_MONEY tempDropMoney( ATOW_( vecMoney[0])/100.0f, ATOUL_(vecMoney[1]), ATOUL_(vecMoney[2]) );
			mapCfg.stDropMoney[ E_DROP_BOSS ] = tempDropMoney;
		}

		mapCfg.group_id = ATOW_( vecString[ 16 ] );

		mapCfg.sub_order = ATOB_( vecString[ 17 ] );

		WORD wEliteID = mapCfg.id;

		if( !IS_ELITE_MAP(wEliteID) )
		{
			mapCfg.refreshMonsterDropLibrary( 4320 );//每日挂机最多掉落次数 不会挂精英地图
		}

		if( IS_ELITE_MAP(wEliteID) && IS_MAP_END(_mapEliteCfg, wEliteID) )//精英关卡 
		{
			_mapEliteCfg.insert( make_pair(mapCfg.group_id, DOUBLE_WORD( mapCfg.id ) ) );
		}
	}
	);
	return true;
}


bool CONFIG::readCfgCompose()
{
	//times	sub_type	job_limit	hp_max	mp_max	atk_max	atk_min	mag_max	mag_min	sol_max	sol_min	phydef_max	phydef_min	magdef_max	magdef_min
	string strIniFile = getConfigDir() + "compose.ini";
	_mapComposeCfg.clear();
	_mapComposeMaxExpCfg.clear();
	_mapComposeNowExpCfg.clear();
	eachIniSecLine(strIniFile, "compose", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 16 )	
			return;

		COMPOSE_CFG  composeCfg;
		composeCfg.times	  = ATOW_( vecString[ 0 ] );	
		composeCfg.sub_type	  = ATOB_( vecString[ 1 ] );	
		composeCfg.job_limit  = ATOB_( vecString[ 2 ] );	
		composeCfg.hp_max	  = ATOUL_( vecString[ 3 ] );	
		composeCfg.mp_max	  = ATOUL_( vecString[ 4 ] );	
		composeCfg.atk_max	  = ATOUL_( vecString[ 5 ] );	
		composeCfg.atk_min	  = ATOUL_( vecString[ 6 ] );	
		composeCfg.mag_max	  = ATOUL_( vecString[ 7 ] );	
		composeCfg.mag_min	  = ATOUL_( vecString[ 8 ] );	
		composeCfg.sol_max	  = ATOUL_( vecString[ 9 ] );	
		composeCfg.sol_min	  = ATOUL_( vecString[ 10 ] );	
		composeCfg.phydef_max = ATOUL_( vecString[ 11 ] );	
		composeCfg.phydef_min = ATOUL_( vecString[ 12 ] );	
		composeCfg.magdef_max = ATOUL_( vecString[ 13 ] );	
		composeCfg.magdef_min = ATOUL_( vecString[ 14 ] );
		composeCfg.exp = ATOUL_( vecString[ 15 ] );
		_mapComposeCfg.insert( make_pair( (composeCfg.times << 16) | (composeCfg.sub_type << 8) | composeCfg.job_limit,  composeCfg ) );
		_mapComposeMaxExpCfg[ (composeCfg.sub_type << 8) | composeCfg.job_limit ] += composeCfg.exp;
		_mapComposeNowExpCfg.insert( make_pair( (composeCfg.times << 16) | (composeCfg.sub_type << 8) | composeCfg.job_limit,  _mapComposeMaxExpCfg[ (composeCfg.sub_type << 8) | composeCfg.job_limit ] ) );

	}
	);
	return true;
}

bool CONFIG::readCfgMercenaryUpstar()
{
//星级	耐力	力量	敏捷	智力	精神	单次升级经验
//star_num	stamina	strength	agility	intellect	spirit	exp_need	

	string strIniFile = getConfigDir() + "mercenary_upstar.ini";
	_mapMercenaryStarCfg.clear();
	_mapMercenaryStarNowExpCfg.clear();
	_dwMercenaryStarMaxExpCfg = 0;
	eachIniSecLine(strIniFile, "mercenary_upstar", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 7 )	
			return;

		MER_STAR_CFG  mercenaryStarCfg;
		mercenaryStarCfg.star_num = ATOB_( vecString[ 0 ] );
		for (size_t i = 0; i < BASE_ATTR_NUM; i++ )
		{
			mercenaryStarCfg.baseAttrRatio[i] = ATOF_( vecString[ i + 1 ] );
		}
		mercenaryStarCfg.exp_need = ATOUL_( vecString[ 6 ] );	
		_mapMercenaryStarCfg.insert( make_pair(mercenaryStarCfg.star_num,  mercenaryStarCfg ) );
		_mapMercenaryStarNowExpCfg.insert(make_pair(mercenaryStarCfg.star_num, _dwMercenaryStarMaxExpCfg));
		_dwMercenaryStarMaxExpCfg += mercenaryStarCfg.exp_need;
	}
	);
	return true;
}

bool CONFIG::readCfgMercenaryComposeBook()
{
	//耐力	力量	敏捷	智力	精神    单级升级经验
	//star_num	stamina	strength	agility	intellect	spirit	exp_need

	string strIniFile = getConfigDir() + "mercenary_composebook.ini";
	_mapMerComposebookCfg.clear();
	_vecMerComposeBookID.clear();
	eachIniSecLine(strIniFile, "mercenary_composebook", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 7 )	
			return;
		MER_COMPOSE_BOOK_CFG  merComposeCfg;
		merComposeCfg.material_id  = ATOW_( vecString[ 0 ] );	
		merComposeCfg.number  = ATOW_( vecString[ 1 ] );	
		merComposeCfg.money_type  = ATOB_( vecString[ 2 ] );	
		merComposeCfg.money_cost  = ATOUL_( vecString[ 3 ] );	
		merComposeCfg.target_id  = ATOW_( vecString[ 4 ] );	
		merComposeCfg.vip_level_limit  = ATOW_( vecString[ 5 ] );	
		merComposeCfg.offer_exp  = ATOW_( vecString[ 6 ] );	

		_mapMerComposebookCfg.insert( make_pair( merComposeCfg.material_id,  merComposeCfg ) );

		_vecMerComposeBookID.push_back( merComposeCfg.material_id );

	}
	);
	assert( _vecMerComposeBookID.size() >= 5 );
	return true;
}

bool CONFIG::readCfgGem()
{

	string strIniFile = getConfigDir() + "gem.ini";
	_mapGemCfg.clear();
	eachIniSecLine(strIniFile, "gem", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, "\x20\r\t,", vecString, NULL, NULL) < 10 )	
			return;

		//宝石ID	宝石名称	升级条件	生成目标	分解获得金钱	分解获得碎片	分解tips描述
		//gem_id	gem_name	upgrade	taget	decompose_money	decompose_item	decompose_tips	

		GEM_CFG& gemCfg = _mapGemCfg[ ATOW_( vecString[ 0 ] )];
		gemCfg.gem_id = ATOW_( vecString[ 0 ] );
		gemCfg.upgrade_id = gemCfg.gem_id;
		gemCfg.upgrade_num = ATOW_( vecString[ 2 ] );
		gemCfg.target_id  = ATOW_( vecString[ 3 ] ) ;	
		gemCfg.decompose_coin_ratio = float(ATOW_( vecString[ 4 ] )/100.0f);
		gemCfg.decompose_coin_num = ATOW_( vecString[ 5 ] );
		gemCfg.decompose_item_ratio = float(ATOW_( vecString[ 6 ] )/100.0f);
		gemCfg.decompose_item_id = ATOW_( vecString[ 7 ] );
		gemCfg.decompose_item_num = ATOW_( vecString[ 8 ] );
	}
	);
	return true;
}

bool CONFIG::readCfgTargetAward()
{
	string strIniFile = getConfigDir() + "target_award.ini";
	_mapTargetAwardCfg.clear();
	eachIniSecLine(strIniFile, "target_award", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 3 )	
			return;
		//索引	奖励等级	奖励物品	说明
		//id	level	item	info
		
		TARGET_AWARD_CFG& targetAwardCfg = _mapTargetAwardCfg[  ATOW_(vecString[0]) ];

		targetAwardCfg.id = ATOW_(vecString[0]);
		
		targetAwardCfg.level = ATOW_(vecString[1]);
		
		const string strItem = vecString[2];
		{
			parseJobEquipInfoStr( strItem, targetAwardCfg.vecItem, 3 );
		}
	}
	);
	return true;
}

DWORD CONFIG::getExpDelta(WORD level1, WORD level2, DWORD exp1, DWORD exp2)
{

	if ( level1 > _vecExpCfg.size() || level1==0 )
		return 0;

	if ( level2 > _vecExpCfg.size() || level2==0 )
		return 0;

	return _vecExpCfg[ level2 -1].sum_exp+exp2 - ( _vecExpCfg[ level1 -1].sum_exp+exp1 ) ;

}

bool CONFIG::readCfgRoleExp()
{
	string strIniFile = getConfigDir() + "exp_upgrade.ini";

	_vecExpCfg.clear();
	DWORD sum_exp = 0;
	eachIniSecLine(strIniFile, "exp", [&](  const char* line )
		{
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 2 )	
				return;

			ROLE_EXP_CFG roleExpCfg;
			roleExpCfg.level = ATOW_( vecString[ 0 ] );			
			roleExpCfg.upgrade_exp = ATOUL_(vecString[ 1 ] );		
			roleExpCfg.sum_exp = sum_exp;

			sum_exp += roleExpCfg.upgrade_exp;
			_vecExpCfg.push_back( roleExpCfg );

		}
	);
	return true;
}

static CONFIG::ATT_RAND_PARA parseAttRandPara(const string& parameter)
{

	vector<string> v;
	int nums = strtoken2Vec( parameter.c_str(), ",", v, NULL, NULL);

	assert( nums == 3 );
	
	CONFIG::ATT_RAND_PARA x;

	x.attr = ATOB_( v[ 0 ] );
	x.valueMin = ATOUL_( v[ 1 ] );
	x.valueMax = ATOUL_( v[ 2 ] );

	return x;
}


bool CONFIG::readCfgAttRandom()
{
	string strIniFile = getConfigDir() + "att_random.ini";

	_mapAttRandCfg.clear();
	eachIniSecLine(strIniFile, "att_random", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 6 )	
			return;

		WORD id = ATOW_( vecString[ 0 ] );

		ATT_RAND_CFG &attRandCfg = _mapAttRandCfg[ id ];

		for (size_t i = 1; i < 6; i++ )
		{
			string parameter = vecString[ i ];
			ATT_RAND_PARA para = parseAttRandPara(parameter);
			attRandCfg.vAttRandPara.push_back( para );
		}
	}

	);

	return true;
}

bool CONFIG::readCfgGuildBuild()
{
	string strIniFile = getConfigDir() + "guild_build.ini";

	_mapGuildBuildCfg.clear();
	eachIniSecLine(strIniFile, "guild_build", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 4 )	
			return;

		GUILD_BUILD_CFG  guildBuildCfg;
		guildBuildCfg.type	       = ATOB_( vecString[ 0 ] );	
		guildBuildCfg.level	       = ATOB_( vecString[ 1 ] );	
		guildBuildCfg.upgrade_exp  = ATOUL_( vecString[ 2 ] );

//1	行会	最大人数
//2	福利	道具索引,数量
//3	圣殿	0
//4	生命值	增加hp_max的值
//5	法力值	增加mp_max的值
//6	物理攻击	增加物理攻击的值，格式：物攻下限,物攻上限
//7	魔法攻击	增加魔理攻击的值，格式：魔法下限,魔法上限
//8	道术攻击	增加道理攻击的值，格式：道术下限,道术上限
//9	物理防御	增加物理攻击的值，格式：物防下限,物防上限
//10	魔法防御	增加物理攻击的值，格式：魔防下限,魔防上限

		const string& parameter	   = vecString[ 3 ] ;

	
		if ( guildBuildCfg.type >= 6 && guildBuildCfg.type <= 10 )
		{
			vector<string> v;
			if ( strtoken2Vec( parameter.c_str(), ",", v, NULL, NULL) != 2 )
			{
				logee("");
				exit(99);
			}
			guildBuildCfg.union_para.min_max.min_value = ATOW_(v[0]);
			guildBuildCfg.union_para.min_max.max_value = ATOW_(v[1]);
		}
		else
		{
			guildBuildCfg.union_para.one_value = ATOW_(parameter);
		}
		
		_mapGuildBuildCfg.insert( make_pair( (guildBuildCfg.type << 8) | guildBuildCfg.level,  guildBuildCfg ) );
	}

	);


	return true;
}



bool CONFIG::readCfgDrop(void)
{
					
//drop.ini
//掉落库索引	掉落方式	掉落组	组概率	参数	掉落概率	掉落数量	合成次数
//id	group_type	group_id	group_rate	parameter	drop_rate	drop_number	compose_times
	
	string strIniFiles[] = { getConfigDir() + "drop.ini", getConfigDir() + "chest.ini" };

	map<DWORD, DROP_INFO_CFG>* m[] = { &_mapDropInfoCfg, &_mapDropInfoChestCfg };

	for( int i=0; i<2; i++)
	{
		m[i]->clear();
		eachIniSecLine(strIniFiles[i].c_str(), "drop", [&](  const char* line )
		{
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 8 )
				return;

// 			DROP_CFG dropCfg;
			DWORD id = ATOUL_( vecString[ 0 ] );
			WORD group_type = ATOW_( vecString[ 1 ] );
			WORD group_id = ATOW_( vecString[ 2 ] );
			DWORD group_rate = ATOUL_( vecString[ 3 ] );
			const string& paraDrop = vecString[ 4 ] ;
			WORD drop_rate = ATOW_( vecString[ 5 ] );
			const string& paraNum = vecString[ 6 ] ;
			const string& paraQua = vecString[ 7 ] ;


			DROP_PARA dropPara;			
			dropPara.groupID = group_id;

			{
				vector<string> vecItem;
				assert( strtoken2Vec( paraDrop.c_str(), ",", vecItem, NULL, NULL) > 0 );
				dropPara.itemIndex = ATOW_( vecItem[0] );
				for ( auto& it : vecItem )
				{
					dropPara.vecItem.push_back( ATOW_( it ) );
				}
			}

			{
				vector<string> vecNum;
				assert( strtoken2Vec( paraNum.c_str(), ",", vecNum, NULL, NULL) >= 2 );
				dropPara.dropMin = ATOW_( vecNum[ 0 ] );
				dropPara.dropMax = ATOW_( vecNum[ 1 ] );
			}

			{
				vector<string> vecQua;
				assert( strtoken2Vec( paraQua.c_str(), ";", vecQua, NULL, NULL) > 0 );
				for ( auto& it : vecQua )
				{
					vector<string> vecQuaPara;
					assert( strtoken2Vec( it.c_str(), ",", vecQuaPara, NULL, NULL) == 2 );
					dropPara.vecQuality.push_back( ATOB_( vecQuaPara[0]) );
					dropPara.vecQuaRand.push_back( ATOUL_( vecQuaPara[1]) );
				}

				//for ( auto &qit : dropPara.vecQuaRand )
				//{		
					partial_sum( dropPara.vecQuaRand.begin() , dropPara.vecQuaRand.end() , dropPara.vecQuaRand.begin() );
				//}

				dropPara.qualityMin = ATOB_( vecQua.front() );
				dropPara.qualityMax = ATOB_( vecQua.back() );
			}
	
//			if ( i == 1 )//chest.ini
//			{
//				dropPara.randTimesMin = ATOW_( vecString[ 10 ] );
//				dropPara.randTimesMax = ATOW_( vecString[ 11 ] );
//			}

			DROP_INFO_CFG &dropInfo = (*m[i])[ id ];
			GROUP_INFO &groupInfo = dropInfo.mapDropGroup[ dropPara.groupID ];
			dropInfo.dropID = id;
			dropInfo.groupType = group_type;

			if ( groupInfo.vDropPara.empty() )//|| groupInfo.vDropPara.back().groupID != group_id )
			{
				dropInfo.vRandSum.push_back( group_rate );
			}
			
			//同组必须连续配表
			groupInfo.vRandSum.push_back( drop_rate );
			groupInfo.vDropPara.push_back( dropPara );
		
		}
		);

		for ( auto &it : (*m[i]) )
		{
			DROP_INFO_CFG &dropInfo = it.second;
			//grouptype 1:组外单roll, 组内partsum
			//grouptype 2:partsum

			if ( dropInfo.groupType == CONFIG::E_DROP_ROLL_MULTI )
			{
				partial_sum( dropInfo.vRandSum.begin() , dropInfo.vRandSum.end() , dropInfo.vRandSum.begin() );
			}
		
			for ( auto &jt : dropInfo.mapDropGroup )
			{
				GROUP_INFO& groupInfo = jt.second;			
				partial_sum( groupInfo.vRandSum.begin() , groupInfo.vRandSum.end() , groupInfo.vRandSum.begin() );
			}
		}


	}

	return true;
}


//[coin]
//#金币捐献   5000    等于    1   贡献
//coin = 5000

//[equip_donate]
//#装备捐献   ——根据装备的品质和类型决定贡献值

//#   品质    武器    衣服    头盔    腰带    鞋子    项链    手镯    戒指    勋章    宝石
//    1   3   2   2   1   1   1   1   1   2   2
//    2   6   5   4   3   3   3   3   3   4   4
//    3   10  8   6   5   5   4   4   4   6   6
//    4   13  10  8   7   7   6   6   6   8   8
//    5   17  13  10  9   9   7   8   8   10  10
//    6   20  16  12  11  10  9   9   9   12  12
//    7   23  18  14  13  12  10  11  11  14  14

bool CONFIG::readCfgFactionDonation()
{
	//times	sub_type	job_limit	hp_max	mp_max	atk_max	atk_min	mag_max	mag_min	sol_max	sol_min	phydef_max	phydef_min	magdef_max	magdef_min
	string strIniFile = getConfigDir() + "donation.ini";

	_coinDonate = BASE::readIni<int>( strIniFile, "coin", "coin", 5000 );

	memset( _vDonatePoint, 0, sizeof( _vDonatePoint )  );

	int row =0;
	eachIniSecLine(strIniFile, "equip_donate", [&](  const char* line )
		{
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 11 )	
				return;

			for ( int i=SUB_TYPE_EQUIP_WEAPON; i<=SUB_TYPE_EQUIP_GEM; i++)
			{
				_vDonatePoint[row][ i ] = ATOUL_(vecString[ i ] );
			}
			row++;

		}
	);
	return true;
}

bool CONFIG::readCfgMine()
{
	string strIniFile = getConfigDir() + "mine.ini";

	_vecMineCfg.clear();

	eachIniSecLine(strIniFile, "mine", [&](  const char* line )
		{

			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 9 )
				return;

			MINE_CFG mineCfg;

			mineCfg.id = ATOW_( vecString[ 0 ] );
			mineCfg.level_min = ATOW_( vecString[ 3 ] );
			mineCfg.level_max = ATOW_( vecString[ 4 ] );
			mineCfg.time = ATOUL_( vecString[ 5 ] );
			mineCfg.count = ATOUL_( vecString[ 6 ] );

			mineCfg.speed = ATOW_( vecString[ 8 ] );
			mineCfg.event = ATOW_( vecString[ 9 ] );
			
			string &mineral = vecString[ 7 ];
			vector<string> v;
			int nums = strtoken2Vec( mineral.c_str(), ",;", v, NULL, NULL);
			if ( nums % 2 != 0 )
				return;

			float sum = 0;
			for ( int i=0; i<nums; i+=2 )
			{
				mineCfg.vecMineIndex.push_back( ATOW_(v[i]) );
				mineCfg.vecMineRandSum.push_back( ATOW_(v[i+1])*1000 );

				sum += (float)(ATOW_(v[i+1])*1000);
			}

			assert( sum != 0 );
	
			mineCfg.minRatio = 1;
			for ( auto &it : mineCfg.vecMineRandSum )
			{
				float ratio = (float) it/sum;
				mineCfg.vecMineIndexRatio.push_back( 	ratio );
				if ( ratio < mineCfg.minRatio )
					mineCfg.minRatio = ratio;
			}

			partial_sum( mineCfg.vecMineRandSum.begin() , mineCfg.vecMineRandSum.end() , mineCfg.vecMineRandSum.begin() );

			//保证顺着配
			_vecMineCfg.push_back(mineCfg);
			
		}
	);
	return true;

}


//编号	职业	最小等级	最大等级	索引1	索引2	索引3	索引4	索引5	索引6
//id	job_limit	min_level	max_level	drop_1	drop_2	drop_3	drop_4	drop_5	drop_6
bool CONFIG::readCfgShop1()
{

	string strIniFile = getConfigDir() + "shop1.ini";

	_mapShop1Cfg.clear();

	eachIniSecLine(strIniFile, "shop1", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 10)	
			return;
		WORD level_max = ATOW_( vecString[ 3 ] );
		BYTE job_limit = ATOB_( vecString[ 1 ] );
		SHOP1_CFG &shop1Cfg = _mapShop1Cfg[ job_limit<<16 | level_max ];
		shop1Cfg.id =  ATOW_( vecString[ 0 ] ) ;
		for (size_t i = 4; i <= 9; i++)//保证六个
		{
			shop1Cfg.vecDrop.push_back(ATOW_( vecString[ i ] ));
		}
		
	}
	);

	return true;
}

//库	掉落组	组概率	道具概率	道具数量	合成次数	货币类型	道具id	总价
//group	group_id	group_rate	item_rate	number	compose_times	currency_type	item_id	price
bool CONFIG::readCfgShop2()
{

	string strIniFile = getConfigDir() + "shop2.ini";

	_mapShop2Cfg.clear();

	eachIniSecLine(strIniFile, "shop2", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 10 )//wm
			return;

		SHOP2_CFG &shop2Cfg = _mapShop2Cfg[ATOW_( vecString[ 0 ] )];

		shop2Cfg.id = ATOW_( vecString[ 0 ] );
		shop2Cfg.group = ATOW_( vecString[ 1 ] );
		shop2Cfg.group_id = ATOW_( vecString[ 2 ] );
		shop2Cfg.group_rate = ATOUL_( vecString[ 3 ] );
		shop2Cfg.item_rate = ATOUL_( vecString[ 4 ] );
		shop2Cfg.number = ATOW_( vecString[ 5 ] );
		shop2Cfg.quality = ATOW_( vecString[ 6 ] );
		shop2Cfg.currency_type = ATOB_( vecString[ 7 ] );

		const string& paraDrop = vecString[ 8 ];
		{
			vector<string> vecItem;
			assert( strtoken2Vec( paraDrop.c_str(), ",", vecItem, NULL, NULL) > 0 );
			for ( auto& it : vecItem )
			{
				shop2Cfg.vecItem.push_back( ATOW_( it ) );
			}
		}

		shop2Cfg.price = ATOUL_( vecString[ 9 ] );


		SHOP_STORAGE &shopStorageCfg = _mapShopStorageCfg[shop2Cfg.group];//库的组
		SHOP_GRUOP &shopGroupCfg = shopStorageCfg.mapGroupShop[shop2Cfg.group_id];

		if ( shopGroupCfg.vecShop2Cfg.empty() )
		{
			shopStorageCfg.vecGroupID.push_back(shop2Cfg.group_id);
			shopStorageCfg.vecGroupRandSum.push_back(shop2Cfg.group_rate);
		}

		shopGroupCfg.vecShop2Cfg.push_back( &shop2Cfg );
		shopGroupCfg.vecItemRandSum.push_back(shop2Cfg.item_rate);
	}
	);

	for ( auto &shopStore : _mapShopStorageCfg )
	{
		partial_sum( shopStore.second.vecGroupRandSum.begin() , shopStore.second.vecGroupRandSum.end() ,shopStore.second.vecGroupRandSum.begin() );

		for (auto &shopGroup : shopStore.second.mapGroupShop)
		{
			partial_sum( shopGroup.second.vecItemRandSum.begin() , shopGroup.second.vecItemRandSum.end() ,shopGroup.second.vecItemRandSum.begin() );
		}
	}

	return true;
}

//神秘商人类型	第一次倒计时	1个售价	10个售价	10个赠送
//type	count	sell_one	sell_ten	gift
bool CONFIG::readCfgVipShop0()
{
	string strIniFile = getConfigDir() + "shop_myth0.ini";

	_mapVipShop0Cfg.clear();

	eachIniSecLine(strIniFile, "shop_myth0", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 8 )	
			return;
		BYTE id = ATOB_( vecString[ 0 ] );
		assert( id == 1 || id == 2);
		SHOP0_CFG &shop0Cfg = _mapVipShop0Cfg[ id ];

		shop0Cfg.firstCoolFreeTime = ATOUL_( vecString[ 1 ] );
		shop0Cfg.free_time = ATOB_( vecString[ 2 ] );

		const string&  sell_one  =	vecString[ 3 ];
		{
			vector<string> vecTypePara;
			assert( strtoken2Vec( sell_one.c_str(), ",", vecTypePara, NULL, NULL) == 2 );
			shop0Cfg.sell_one_money_type = ATOB_(vecTypePara[0]);
			assert( shop0Cfg.sell_one_money_type == E_COIN_MONEY || shop0Cfg.sell_one_money_type == E_GOLD_MONEY );
			shop0Cfg.sell_one_money_Num = ATOUL_(vecTypePara[1]);
		}

		const string&  sell_ten  =	vecString[ 4 ];
		{
			vector<string> vecTypePara;
			assert( strtoken2Vec( sell_ten.c_str(), ",", vecTypePara, NULL, NULL ) == 3 );
			shop0Cfg.sell_ten_money_type = ATOB_(vecTypePara[0]);
			assert( shop0Cfg.sell_ten_money_type == E_COIN_MONEY || shop0Cfg.sell_ten_money_type == E_GOLD_MONEY );
			shop0Cfg.sell_ten_money_Num = ATOUL_(vecTypePara[1]);
			shop0Cfg.sell_ten_money_discount = ATOB_(vecTypePara[2]);
		}

		const string&  gift  =	vecString[ 5 ];
		{
			vector<string> vecTypePara;
			assert( strtoken2Vec( gift.c_str(), ",", vecTypePara, NULL, NULL) == 3 );
			shop0Cfg.giftiD = ATOW_(vecTypePara[0]);
			shop0Cfg.giftNum = ATOW_(vecTypePara[1]);
			shop0Cfg.giftQua = ATOB_(vecTypePara[2]);
		}

		shop0Cfg.coolFreeTime = ATOUL_(vecString[ 6 ]);

		shop0Cfg.vip_limit = ATOW_(vecString[ 7 ]);

	}
	);

	return true;
}


//编号	职业	最小等级	最大等级	索引1	索引2	索引3	索引4
//id	job_limit	min_level	max_level	first_1	normal_1	first_2	normal_2

bool CONFIG::readCfgVipShop1()
{

	string strIniFile = getConfigDir() + "shop_myth1.ini";

	_mapVipShop1Cfg.clear();

	eachIniSecLine(strIniFile, "shop_myth1", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 8)	
			return;
		WORD max_level = ATOW_( vecString[ 3 ] );
		BYTE job_limit = ATOB_( vecString[ 1 ] );
		SHOP1_CFG &shop1Cfg = _mapVipShop1Cfg[ job_limit<<16 | max_level ];

		shop1Cfg.id =  ATOW_( vecString[ 0 ] ) ;
		shop1Cfg.job_limit = job_limit;
		shop1Cfg.min_level =  ATOW_( vecString[ 2 ] );
		shop1Cfg.max_level = max_level;

		for (size_t i = 4; i <= 7; i++)
		{
			shop1Cfg.vecDrop.push_back(ATOW_( vecString[ i ] ));
		}

	}
	);

	return true;
}

//库	掉落组	组概率	道具概率	道具数量	合成次数	货币类型	道具id	总价
//group	group_id	group_rate	item_rate	number	compose_times	currency_type	item_id	price
bool CONFIG::readCfgVipShop2()
{

	string strIniFile = getConfigDir() + "shop_myth2.ini";

	_mapVipShop2Cfg.clear();

	eachIniSecLine(strIniFile, "shop_myth2", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 10 )//wm
			return;

		SHOP2_CFG &shop2Cfg = _mapVipShop2Cfg[ATOW_( vecString[ 0 ] )];

		shop2Cfg.id = ATOW_( vecString[ 0 ] );
		shop2Cfg.group = ATOW_( vecString[ 1 ] );
		shop2Cfg.group_id = ATOW_( vecString[ 2 ] );
		shop2Cfg.group_rate = ATOUL_( vecString[ 3 ] );
		shop2Cfg.item_rate = ATOUL_( vecString[ 4 ] );
		shop2Cfg.number = ATOW_( vecString[ 5 ] );
		shop2Cfg.quality = ATOW_( vecString[ 6 ] );
		shop2Cfg.currency_type = ATOB_( vecString[ 7 ] );
		
		const string& paraDrop = vecString[ 8 ];
		{
			vector<string> vecItem;
			assert( strtoken2Vec( paraDrop.c_str(), ",", vecItem, NULL, NULL) > 0 );
			for ( auto& it : vecItem )
			{
				shop2Cfg.vecItem.push_back( ATOW_( it ) );
			}
		}

		shop2Cfg.price = ATOUL_( vecString[ 9 ] );

		SHOP_STORAGE &shopStorageCfg = _mapVipShopStorageCfg[shop2Cfg.group];//库的ID 相当于dropID
		SHOP_GRUOP &shopGroupCfg = shopStorageCfg.mapGroupShop[shop2Cfg.group_id];

		if ( shopGroupCfg.vecShop2Cfg.empty() )
		{
			shopStorageCfg.vecGroupID.push_back(shop2Cfg.group_id);
			shopStorageCfg.vecGroupRandSum.push_back(shop2Cfg.group_rate);
		}

		shopGroupCfg.vecShop2Cfg.push_back( &shop2Cfg );
		shopGroupCfg.vecItemRandSum.push_back(shop2Cfg.item_rate);
	}
	);

	for ( auto &shopStore : _mapVipShopStorageCfg )
	{
		partial_sum( shopStore.second.vecGroupRandSum.begin() , shopStore.second.vecGroupRandSum.end() ,shopStore.second.vecGroupRandSum.begin() );

		for (auto &shopGroup : shopStore.second.mapGroupShop)
		{
			partial_sum( shopGroup.second.vecItemRandSum.begin() , shopGroup.second.vecItemRandSum.end() ,shopGroup.second.vecItemRandSum.begin() );
		}
	}

	return true;
}

bool CONFIG::readCfgDisCount()
{
	string strIniFile = getConfigDir() + "discount.ini";

	eachIniSecLine(strIniFile, "discount", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 2 )
			return;
		SHOP_DISCOUNT& shopDiscountCfg = _mapDiscountCfg[ ATOW_( vecString[0] ) ];
		shopDiscountCfg.time = ATOW_( vecString[0] );
		const string& disPara = vecString[1];
		{
			vector<string> vecDispara;
			strtoken2Vec( disPara.c_str(), ";", vecDispara, NULL, NULL);
			for ( auto& dis : vecDispara )
			{
				vector<string> vecDis;
				assert( strtoken2Vec( dis.c_str(), ",", vecDis, NULL, NULL) == 2 );
				shopDiscountCfg.vecDiscount.push_back( ATOB_( vecDis[0] ) );
				shopDiscountCfg.vecDisRandSum.push_back( ATOW_( vecDis[1] ) );
			}
			partial_sum( shopDiscountCfg.vecDisRandSum.begin() , shopDiscountCfg.vecDisRandSum.end() ,shopDiscountCfg.vecDisRandSum.begin() );
		}

	}
	);

	return true;
}

//init role shop goods
void CONFIG::initShopItems(const WORD wLevel, const BYTE job, list<SHOP_ITEM>& shopItems, const DWORD updateTime  )
{
	shopItems.clear();

	CONFIG::SHOP1_CFG* shop1Cfg;

	shop1Cfg = getShop1Cfg2( job, wLevel );
	
	if (shop1Cfg == NULL )
	{
		return ;
	}

	vector<WORD> vecTempShopStorage(shop1Cfg->vecDrop);

	for ( auto &drop : vecTempShopStorage )//最多六个
	{
		SHOP2_CFG *pShopCfg  = CONFIG::getShopItem(  drop,  _mapShopStorageCfg );

		COND_CONTINUE( pShopCfg == NULL );

		BYTE byDiscount = getRandDiscount( WORD(updateTime) );

		WORD itemID = pShopCfg->vecItem[ randTo(pShopCfg->vecItem.size()) ];

		shopItems.push_back(  SHOP_ITEM(pShopCfg->id,itemID,pShopCfg->price,pShopCfg->number,pShopCfg->currency_type,byDiscount,pShopCfg->quality));
	}
}

void CONFIG::initVipShopItems( ITEM_INFO& itemInfo, const WORD drop )
{
	SHOP2_CFG *pShopCfg  = CONFIG::getShopItem(  drop,  _mapVipShopStorageCfg );

	RETURN_VOID( pShopCfg == NULL );

	WORD itemID = pShopCfg->vecItem[ randTo(pShopCfg->vecItem.size()) ];

	itemInfo.itemIndex = itemID;
	itemInfo.byQuality = (BYTE)pShopCfg->quality;
	itemInfo.itemNums = pShopCfg->number;
}

BYTE CONFIG::getRandDiscount( const WORD updateTime )
{
	auto it = _mapDiscountCfg.find(updateTime);

	vector<DWORD> vecDisRandSum;

	if( it == _mapDiscountCfg.end() )//找不到不打折
	{
		it = _mapDiscountCfg.end();
		it--;
	}
	
	RETURN_COND(it->second.vecDisRandSum.empty(), 10);

	int index =  BASE::getIndexOfVectorSum( it->second.vecDisRandSum );

	BYTE dis =  it->second.vecDiscount[index];

	return dis;
}

//init role mystery shop goods
CONFIG::SHOP2_CFG *CONFIG::getShopItem( const WORD drop,  map<WORD, SHOP_STORAGE>& mapShopStorageCfg)
{
	auto it = mapShopStorageCfg.find(drop);

	RETURN_COND( it == mapShopStorageCfg.end(), NULL );

	SHOP_STORAGE* shopStorageCfg = &mapShopStorageCfg[drop];

	RETURN_COND( shopStorageCfg == NULL , NULL );

	RETURN_COND( shopStorageCfg->vecGroupRandSum.empty() , NULL );

	int index =  BASE::getIndexOfVectorSum( shopStorageCfg->vecGroupRandSum );

	WORD group = shopStorageCfg->vecGroupID[index];

	SHOP_GRUOP &shopGroupCfg = shopStorageCfg->mapGroupShop[group];

	RETURN_COND( shopGroupCfg.vecItemRandSum.empty() , NULL );

	index =  BASE::getIndexOfVectorSum( shopGroupCfg.vecItemRandSum );

	SHOP2_CFG *pShopCfg = shopGroupCfg.vecShop2Cfg[index];

	return pShopCfg;
}



bool CONFIG::readCfgMission()
{
	string strIniFile = getConfigDir() + "mission.ini";

	_mapMissionCfg.clear();
	_mapTaskTypeIDs.clear();
	eachIniSecLine(strIniFile, "mission", [&](  const char* line )
		{
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 13 )
				return;

			MISSION_CFG missionCfg;
		
			missionCfg.id = ATOW_( vecString[ 0 ] );
			missionCfg.level_limit = ATOW_( vecString[ 3 ] );
			missionCfg.type = ATOB_( vecString[ 4 ] );

			if ( missionCfg.type == E_TASK_KILL_MON )
			{
				vector<string> v;
				int nums = strtoken2Vec( vecString[ 5 ].c_str(), ",;", v, NULL, NULL);
				assert( nums == 2 );

				missionCfg.parameter.itemIndex = ATOW_( v[ 1 ] );//配置mapID
				missionCfg.parameter.itemNums = ATOUL_( v[ 0 ] );//配置前一个是nums
			}
			if ( missionCfg.type == E_TASK_FIGHT_BOSS )
			{
				vector<string> v;
				int nums = strtoken2Vec( vecString[ 5 ].c_str(), ",;", v, NULL, NULL);
				assert( nums == 2 );

				missionCfg.parameter.itemIndex = ATOW_( v[ 0 ] );//配置mapID
				missionCfg.parameter.itemNums = ATOUL_( v[ 1 ] );//配置前一个是nums
			}
			else if ( missionCfg.type == E_TASK_HAS_EQUIP_RARE )
			{
				vector<string> v;
				int nums = strtoken2Vec( vecString[ 5 ].c_str(), ",;", v, NULL, NULL);
				assert( nums == 2 );

				missionCfg.parameter.itemIndex = ATOW_( v[ 0 ] );//子类型
				missionCfg.parameter.itemNums = ATOUL_( v[ 1 ] );//稀有度
			}
			else if ( missionCfg.type == E_TASK_HAS_EQUIP_COMPOSE )
			{
				vector<string> v;
				int nums = strtoken2Vec( vecString[ 5 ].c_str(), ",;", v, NULL, NULL);
				assert( nums == 2 );

				missionCfg.parameter.itemIndex = ATOW_( v[ 0 ] );//子类型
				missionCfg.parameter.itemNums = ATOUL_( v[ 1 ] );//合成次数
			}
			else
			{
					//需要完成次数
				missionCfg.parameter.itemNums = ATOW_( vecString[ 5 ] );
			}

			missionCfg.group = ATOW_( vecString[ 6 ] );
			missionCfg.front_id = ATOW_( vecString[ 7 ] );
			missionCfg.next_id = ATOW_( vecString[ 8 ] );
			//CONFIG::parseItemInfoStr( vecString[9], missionCfg.vecPrize );
			missionCfg.itemID = 0;//子类型
			missionCfg.itemNum = 0;//配置前一个是nums
			{
				vector<string> vItem;
				int nums = strtoken2Vec( vecString[ 9 ].c_str(), ",", vItem, NULL, NULL);
				if ( nums == 2 )
				{
					missionCfg.itemID = ATOW_( vItem[ 0 ] );//子类型
					missionCfg.itemNum = ATOUL_( vItem[ 1 ] );//配置前一个是nums
				}
			}

			missionCfg.coin = ATOUL_( vecString[ 10 ] );
			missionCfg.ingot = ATOUL_( vecString[ 11 ] );

			missionCfg.exp = ATOUL_( vecString[ 12 ] );
			missionCfg.repeat = ATOB_( vecString[ 13 ] );

			_mapMissionCfg.insert( make_pair( missionCfg.id,  missionCfg ) );
		
			vector<WORD>& vecIDs = _mapTaskTypeIDs[missionCfg.type];
			vecIDs.push_back( missionCfg.id );
		}
	);
	return true;

}

//编号	开关	名称	活动类型	标题美术资源	背景美术资源	每日重复	活动有效日期类型	活动开启时间	活动关闭时间	领奖有效日期类型	领取开启时间	领取结束时间	发放类型	奖励	元宝	金币
//id	switch	name	type	title	bkg	repeat	date_type	begin_time1	end_time1		begin_time2	end_time2	get_type	reward	gold	coin

bool CONFIG::readCfgOpenActivity()
{
	string strIniFile = getConfigDir() + "open_activity.ini";

	_mapOpenActivityCfg.clear();
	_mapOpenActivityTypeIDs.clear();
	eachIniSecLine(strIniFile, "open_activity", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 17 )
			return;

		OPEN_ACTIVITY openActivityCfg;
		openActivityCfg.begin_time = 0;
		openActivityCfg.end_time = 0;
		openActivityCfg.reward_begin_time = 0;
		openActivityCfg.reward_end_time = 0;

		openActivityCfg.id = ATOW_( vecString[ 0 ] );
		openActivityCfg.bySwitch = ATOB_( vecString[1] );

		RETURN_VOID(openActivityCfg.bySwitch == 0);//活动开关

		openActivityCfg.name = vecString[2];
		openActivityCfg.type = ATOB_( vecString[3] );
		openActivityCfg.repeat = ATOB_( vecString[6] );

		openActivityCfg.date_type = ATOB_( vecString[7] );
		string& strBegin = vecString[8];//活动操作动作时间段
		string& strEnd = vecString[9];

		openActivityCfg.reward_date_type = ATOB_( vecString[10] );
		string& strRewardBegin = vecString[11];//奖励操作动作时间段
		string& strRewardEnd = vecString[12];

		openActivityCfg.send_reward_type = ATOB_( vecString[13] );

		string& strItem = vecString[14];
		string& strGold = vecString[15];
		string& strCoin = vecString[16];

		if (openActivityCfg.date_type == 1 || openActivityCfg.date_type == 2 || openActivityCfg.date_type == 5)
		{
			openActivityCfg.begin_time = ATOUL_(strBegin);
			openActivityCfg.end_time = ATOUL_(strEnd);
		}

		if (openActivityCfg.reward_date_type == 1 || openActivityCfg.reward_date_type == 2 || openActivityCfg.reward_date_type == 5)
		{
			openActivityCfg.reward_begin_time = ATOUL_(strRewardBegin);
			openActivityCfg.reward_end_time = ATOUL_(strRewardEnd);
		}
		

		OPEN_REWARD& oneReward = openActivityCfg.stReward;
		oneReward.gold = ATOUL_(strGold);
		oneReward.coin = ATOUL_(strCoin);
		vector<string> v1;
		int nums = strtoken2Vec( strItem.c_str(), ";", v1, NULL, NULL);
		for ( int i = 0; i < nums; i++ )
		{
			vector<string> v2;
			assert( strtoken2Vec( v1[i].c_str(), ",", v2, NULL, NULL) == 3);
			oneReward.vecItems.push_back( ITEM_INFO( ATOW_(v2[0]), ATOUL_(v2[2]), ATOB_(v2[1]) ) );
		}

		_mapOpenActivityCfg.insert( make_pair( openActivityCfg.id,  openActivityCfg ) );

		vector<WORD>& vecIDs = _mapOpenActivityTypeIDs[openActivityCfg.type];
		vecIDs.push_back( openActivityCfg.id );

	}
	);
	return true;

}


//bool CONFIG::readCfgWorldBossReward()
//{
//	string strIniFile = getConfigDir() + "world_boss_reward.ini";
//	_vecWorldBossRewardCfg.clear();
//	eachIniSecLine(strIniFile,"world_boss_reward", [&]( const char* line )
//	{
//		vector<string> vecString;
//		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 7 )
//			return;
//		WORLD_BOSS_REWARD_CFG worldBossCfg;
//		worldBossCfg.id = ATOW_(vecString[0]);
//		worldBossCfg.min_boss = ATOW_(vecString[1]);
//		worldBossCfg.max_boss = ATOW_(vecString[2]);
//
//		vector<string> vItem, vOne;
//		vector<ITEM_INFO> vecItemInfo;
//		int nums;
//		for (size_t i = 3; i < 7; i++)
//		{
//			vecItemInfo.clear();
//			vItem.clear();
//			nums = strtoken2Vec( vecString[ i ].c_str(), ";", vItem, NULL, NULL);
//			assert(nums == 3);
//			for (size_t j = 0; j < 3; j++)
//			{
//				vOne.clear();
//				nums = strtoken2Vec( vItem[ j ].c_str(), ",", vOne, NULL, NULL);
//				assert(nums == 2);
//				vecItemInfo.push_back( ITEM_INFO( ATOW_(vOne[0]), ATOW_(vOne[1]) ) );
//			}
//			worldBossCfg.vvecReward.push_back( vecItemInfo );
//		}
//		_vecWorldBossRewardCfg.push_back( worldBossCfg );
//
//	}
//		
//		);
//	return true;
//}

size_t CONFIG::getSmeltValue(BYTE quality, WORD subType)
{
	//assert(subType ,1-10);
	//assert(quality ,1-7);

	//row: 0-6 品质,col: 0 quality, col: 1-10, 直接用sub_type作index

	return vvSmeltValue[ quality-1 ] [subType];
		
}

bool CONFIG::readCfgSmelt()
{

	string strIniFile = getConfigDir() + "smelt.ini";

	memset( vvSmeltValue, 0, sizeof(vvSmeltValue) );

	int row = 0;
	
	eachIniSecLine(strIniFile, "smelt", [&](  const char* line )
		{
			if ( row >= 7 )
				return;
			
			vector<string> vecString;
			if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 11 )
				return;
			
			for ( int i=0; i<11; i++)
			{
				vvSmeltValue[row][i] = ATOUL_(vecString[i]);
			}
			
			row++;

		}
	);

	return true;
}

bool CONFIG::readCfgForgeRresh()//打造刷新
{
	string strIniFile = getConfigDir() + "equip_index.ini";

	_mapForgeRefreshCfg.clear();
	
	eachIniSecLine(strIniFile, "equip_index", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 18 )
			return;

		WORD wJob = ATOW_(vecString[2]);
		//BYTE bySex = ATOB_(vecString[3]);
		WORD wLevel = ATOW_(vecString[4]);

		DWORD key = ( wJob << 16 ) | wLevel;

		FORGE_REFRESH_CFG &forgeRefreshCfg = _mapForgeRefreshCfg[key];
		
		for ( int i =0; i<10; i++)
		{
			forgeRefreshCfg.vEquip_1_10[i] = ATOW_(vecString[i+5]);
		}

		forgeRefreshCfg.dwSmeltValue = ATOUL_(vecString[15]);
		forgeRefreshCfg.dwSuitSmeltValue = ATOUL_(vecString[16]);
		forgeRefreshCfg.dwSuitPatchCount = ATOUL_(vecString[17]);
	}
	);

	return true;
}

// 读取符文配置
bool CONFIG::readCfgRune()
{
	string strIniFile = getConfigDir() + "rune.ini";

	_mapRuneCfg.clear();

	eachIniSecLine(strIniFile, "rune", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 21 )
			return;

		RUNE_CFG_ITEM item;
		item.itemid = ATOW_(vecString[0]);
		item.level = ATOW_(vecString[2]);

		DWORD key = ( item.itemid << 16 ) | item.level;

		item.need_exp = ATOUL_(vecString[3]); // 升级所需经验
		item.stamina = ATOUL_(vecString[4]); //耐力	
		item.strength = ATOUL_(vecString[5]); //力量										
		item.agility = ATOUL_(vecString[6]); //敏捷	
		item.intellect = ATOUL_(vecString[7]); //智慧
		item.spirit = ATOUL_(vecString[8]);	//精神	
		item.hp_max = ATOUL_(vecString[9]);	//最大生命值	
		item.mp_max = ATOUL_(vecString[10]);	//最大魔法值	
		item.atk_max = ATOUL_(vecString[11]); //攻击最大值	
		item.def_max = ATOUL_(vecString[12]);	//防御最大值
		item.spellatk_max = ATOUL_(vecString[13]);	 //技能攻击最大值
		item.spelldef_max = ATOUL_(vecString[14]); //技能防御最大值	
		item.hit = ATOUL_(vecString[15]); //命中值	
		item.dodge = ATOUL_(vecString[16]); //闪避值	
		item.crit = ATOUL_(vecString[17]); //暴击值	
		item.crit_res = ATOUL_(vecString[18]); //暴击抵抗值	
		item.crit_inc = ATOUL_(vecString[19]);	//暴击伤害增加值
		item.crit_red = ATOUL_(vecString[20]); //暴击伤害减免值

		_mapRuneCfg[key] = item;
	}
	);

	return true;
}

bool CONFIG::readCfgAttributeDeform()
{
	string strIniFile = getConfigDir() + "attribute_deform.ini";

	_mapAttributeDeformCfg.clear();

	eachIniSecLine(strIniFile, "attribute_deform", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 2 )
			return;

		ATTRDEFORM_CFG attrDeformCfg;
		attrDeformCfg.type		 =	ATOB_( vecString[0] );

		assert( attrDeformCfg.type < E_ATTR_MAX );

		const string&  param  =	vecString[1];
		{
			vector<string> vecEffectType;
			strtoken2Vec( param.c_str(), ";", vecEffectType, NULL, NULL);
			for ( auto& it : vecEffectType )
			{
				vector<string> vecTypePara;
				strtoken2Vec( it.c_str(), ",", vecTypePara, NULL, NULL);
				if( vecTypePara.size() == 2 )
				{
					attrDeformCfg.vecAttrDeform.push_back( AttrDeform( ATOB_(vecTypePara[0]), ATOF_(vecTypePara[1]) ) );
				}
			}
		}

		_mapAttributeDeformCfg.insert( make_pair(attrDeformCfg.type, attrDeformCfg));
	}
	);


	return true;
}

bool CONFIG::readCfgMercenary()
{
	string strIniFile = getConfigDir() + "mercenary.ini";

	_mapMercenaryCfg.clear();

	eachIniSecLine(strIniFile, "mercenary", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 31 )
			return;

		MERCENARY_CFG mercenaryCfg;
		mercenaryCfg.job_id		 =	ATOB_( vecString[0] );
		mercenaryCfg.level		 =	ATOW_( vecString[1] );
		mercenaryCfg.stamina	 =	ATOUL_( vecString[2] );
		mercenaryCfg.strength	 =	ATOUL_( vecString[3] );
		mercenaryCfg.agility	 =	ATOUL_( vecString[4] );
		mercenaryCfg.intellect	 =	ATOUL_( vecString[5] );
		mercenaryCfg.spirit		 =	ATOUL_( vecString[6] );
		mercenaryCfg.hp_max		 =	ATOUL_( vecString[7] );
		mercenaryCfg.mp_max		 =	ATOUL_( vecString[8] );
		mercenaryCfg.atk_max	 =	ATOUL_( vecString[9] );
		mercenaryCfg.atk_min	 =	ATOUL_( vecString[10] );
		mercenaryCfg.def_max	 =	ATOUL_( vecString[11] );
		mercenaryCfg.def_min	 =	ATOUL_( vecString[12] );
		mercenaryCfg.spellatk_max=	ATOUL_( vecString[13] );
		mercenaryCfg.spellatk_min=	ATOUL_( vecString[14] );
		mercenaryCfg.spelldef_max=	ATOUL_( vecString[15] );
		mercenaryCfg.spelldef_min=	ATOUL_( vecString[16] );
		mercenaryCfg.hit		 =	ATOUL_( vecString[17] );
		mercenaryCfg.dodge		 =	ATOUL_( vecString[18] );
		mercenaryCfg.crit		 =	ATOUL_( vecString[19] );
		mercenaryCfg.crit_res	 =	ATOUL_( vecString[20] );
		mercenaryCfg.crit_inc	 =	ATOUL_( vecString[21] );
		mercenaryCfg.crit_red	 =	ATOUL_( vecString[22] );
		mercenaryCfg.mp_rec		 =	ATOUL_( vecString[23] );
		mercenaryCfg.hp_rec		 =	ATOUL_( vecString[24] );
		mercenaryCfg.luck		 =	ATOUL_( vecString[25] );
		mercenaryCfg.att_hem	 =	ATOUL_( vecString[26] );
		mercenaryCfg.att_mana	 =	ATOUL_( vecString[27] );
		mercenaryCfg.equ_drop	 =	ATOUL_( vecString[28] );
		mercenaryCfg.money_drop	 =	ATOUL_( vecString[29] );
		mercenaryCfg.luck_def	 =	ATOUL_( vecString[30] );
		DWORD key = ( mercenaryCfg.job_id << 16 ) | mercenaryCfg.level;

		_mapMercenaryCfg.insert( make_pair(key, mercenaryCfg));
	}
	);


	return true;
}

bool CONFIG::readCfgMercenaryHalo()
{
	string strIniFile = getConfigDir() + "mercenary_halo.ini";

	_mapMercenaryHaloCfg.clear();

	eachIniSecLine(strIniFile, "mercenary_halo", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 6 )
			return;

		MERHALO_CFG merHaloCfg;

		merHaloCfg.id =	ATOW_( vecString[0] );

		const string&  strAttr  =	vecString[2];
		{
			vector<string> vecEffectType;
			assert( BASE::strtoken2Vec( strAttr.c_str(), ";", vecEffectType, NULL, NULL) > 0 );
			for ( auto& it : vecEffectType )
			{
				vector<string> vecTypePara;
				assert( BASE::strtoken2Vec( it.c_str(), ",", vecTypePara, NULL, NULL) == 3 );
				merHaloCfg.vecEffectAttr.push_back( SKILL_EFFECT( E_ATTR_ADD, ATOB_(vecTypePara[0]),  ATOW_(vecTypePara[1]),  ATOW_(vecTypePara[2]) ) ) ;
			}
		}

		const string&  strCond  =	vecString[5];
		{
			vector<string> vecCond;
			assert( BASE::strtoken2Vec( strCond.c_str(), ",", vecCond, NULL, NULL) == 3 );
			merHaloCfg.byStar = ATOB_( vecCond[ 0 ] );
			merHaloCfg.byMoneyType = ATOB_(vecCond[ 1 ]);
			merHaloCfg.dwMoneyCost = ATOUL_(vecCond[ 2 ]);
		}

		_mapMercenaryHaloCfg.insert( make_pair( merHaloCfg.id, merHaloCfg ) );

	}
	);


	return true;
}

bool CONFIG::readCfgMercenaryDefinition()
{

	string strIniFile = getConfigDir() + "mercenary_definition.ini";
	_mapMercenaryDefinitionCfg.clear();

	eachIniSecLine(strIniFile, "mercenary_definition", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 9 )
			return;

		MERDEF_CFG merdefCfg;
		merdefCfg.id =	ATOW_( vecString[0] );
		merdefCfg.job_id =	ATOB_( vecString[1] );
		merdefCfg.sex = ATOB_( vecString[5] );
		merdefCfg.level =	ATOW_( vecString[2] );
		merdefCfg.skill_id =	ATOW_( vecString[4] );
		const string&  param  =	vecString[8];
		{
			vector<string> vecStrHalo;
			assert( strtoken2Vec( param.c_str(), ",", vecStrHalo, NULL, NULL) == 3 );
			for ( auto& it : vecStrHalo )
			{
				merdefCfg.vecHalo.push_back(  ATOW_( it ) );
			}
		}

		_mapMercenaryDefinitionCfg.insert( make_pair( merdefCfg.id, merdefCfg ) );

	}
	);


	return true;
}

bool CONFIG::readCfgMercenaryTrainMax()
{
	string strIniFile = getConfigDir() + "mercenary_train_max.ini";

	_mapMercenaryTrainMaxCfg.clear();						

	eachIniSecLine(strIniFile, "mercenary_train_max", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 2 )
			return;

		WORD level = ATOW_( vecString[0] );

		DWORD train_max = ATOUL_( vecString[1] );

		_mapMercenaryTrainMaxCfg.insert( make_pair( level, train_max ) );
		//MERCENARY_TRAIN_MAX_CFG mercenaryTrainMaxCfg;

		//mercenaryTrainMaxCfg.level = ATOW_( vecString[0] );

		//mercenaryTrainMaxCfg.train_max  =	ATOUL_( vecString[1] );

		//_mapMercenaryTrainMaxCfg.insert( make_pair( mercenaryTrainMaxCfg.level, mercenaryTrainMaxCfg ) );
	}
	);


	return true;
}

bool CONFIG::readCfgMercenaryTrain()
{
	string strIniFile = getConfigDir() + "mercenary_train.ini";

	_mapMercenaryTrainCfg.clear();

	//培养类型	名称	vip等级限制	消耗钱币类型	消耗钱币数量	属性变更参数										
	//id	name	viplevel_limit	money_type	money_cost	attr_param										

	eachIniSecLine(strIniFile, "mercenary_train", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 6 )
			return;

		MERCENARY_TRAIN_CFG mercenaryTrainCfg;
		mercenaryTrainCfg.type		 =	ATOB_( vecString[0] );
		mercenaryTrainCfg.viplevel_limit  =	ATOB_( vecString[2] );
		mercenaryTrainCfg.money_type	 =	ATOB_( vecString[3] );

		assert( mercenaryTrainCfg.money_type == E_GOLD_MONEY || mercenaryTrainCfg.money_type == E_COIN_MONEY );

		mercenaryTrainCfg.money_cost	 =	ATOUL_( vecString[4] );

		const string&  param  =	vecString[5];
		{
			vector<string> vecEffectType;
			assert( strtoken2Vec( param.c_str(), ";", vecEffectType, NULL, NULL) > 0 );
			for ( auto& it : vecEffectType )
			{
				vector<string> vecTypePara;
				assert( strtoken2Vec( it.c_str(), ",", vecTypePara, NULL, NULL) == 3 );
				mercenaryTrainCfg.vecAddAttrValue.push_back( AddAttrValue( ATOI_(vecTypePara[1]), ATOI_(vecTypePara[2]) ) );
				mercenaryTrainCfg.vecRandSum.push_back(  ATOUL_(vecTypePara[0]) );
			}
		}

		partial_sum( mercenaryTrainCfg.vecRandSum.begin(), mercenaryTrainCfg.vecRandSum.end(),mercenaryTrainCfg.vecRandSum.begin() );
		_mapMercenaryTrainCfg.insert( make_pair( mercenaryTrainCfg.type, mercenaryTrainCfg ) );
	}
	);


	return true;
}


CONFIG::FORGE_REFRESH_CFG* CONFIG::getForgeCfg( WORD wJob, WORD wLevel )
{
	DWORD key = ( wJob << 16 ) | wLevel;

	auto it = _mapForgeRefreshCfg.find(key);
	if ( it == _mapForgeRefreshCfg.end() )
		return NULL;

	FORGE_REFRESH_CFG &v = it->second;
	return &v;
}


// 根据品质随机一个符文
WORD CONFIG::randOneRune(DWORD quality)
{
	auto itr = _mapRuneQualitys.find(quality);
	if (itr == _mapRuneQualitys.end())
	{
		return 0;
	}

	const std::vector<WORD> vecItems = itr->second;
	if (vecItems.empty())
	{
		return 0;
	}

	return vecItems[BASE::randBetween((size_t)0,vecItems.size())];
}

CONFIG::RUNE_CFG_ITEM* CONFIG::getRuneCfg(WORD itemid, WORD level)
{
	DWORD key = ( itemid << 16 ) | level;

	auto it = _mapRuneCfg.find(key);
	if ( it == _mapRuneCfg.end() )
		return NULL;

	RUNE_CFG_ITEM &v = it->second;
	return &v;
}

bool CONFIG::isExistRuneCfg(WORD itemid, WORD level)
{
	return NULL != getRuneCfg(itemid,level);
}

DWORD CONFIG::sumRuneExp(WORD itemid, WORD level)
{
	if (level < 2)
	{
		return 0;
	}

	DWORD result = 0;
	for (WORD i=1; i<level-1; i++)
	{
		RUNE_CFG_ITEM* item = getRuneCfg(itemid, i);
		if (item != NULL)
		{
			result += item->need_exp;
		}
	}

	return result;
}

//打造刷新
bool CONFIG::forgeRefresh( ROLE* role )
{

//刷新获得紫色装备的概率为75%，获得橙色装备的概率为25% 


//新建角色要刷出来?

//	if ( role->byForgeRreshTimes >= DAILY_MAX_REFRESH_NUMS )		//打造界面已经刷新次数,日清
//	{
//		
//	}

//	role->byForgeRreshTimes += 1;


	//CONFIG::FORGE_REFRESH_CFG* forgeRefreshCfg = CONFIG::getForgeRefreshCfg( role->byJob, role->bySex, role->wLevel );
	//if ( forgeRefreshCfg == NULL )
	//	return false;

	//WORD wNewIndex = forgeRefreshCfg->vEquip_1_10[ rand()%10 ];
	//CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(wNewIndex);
	//if ( itemCfg == NULL )
	//	return false;

	//delete role->itemForge;

	//打造刷出紫色或者橙色
	//BYTE byQuality = RAND_HIT( 0.25 ) ? E_QUALITY_GOLD : E_QUALITY_PUPLE;
	//role->itemForge = new ITEM_CACHE( itemCfg, 1, byQuality );

	return true;
}


//兑换刷新
bool CONFIG::exchangeRefresh( ROLE* role )
{
	//兑换刷出红色，刷10个

	return true;
}


void CONFIG::upgradeForgeRefresh( ROLE* role, WORD wOldLevel )
{

//等级触发刷新界限,免费刷新兑换界面

	//FORGE_REFRESH_CFG* cfg1 = getForgeRefreshCfg( role->byJob, role->bySex, wOldLevel );
	//FORGE_REFRESH_CFG* cfg2 = getForgeRefreshCfg( role->byJob, role->bySex, role->wLevel );
	//if ( cfg1 == cfg2 || cfg2 == NULL )
	//	return;

	//bool CONFIG::exchangeRefresh( ROLE* role )


}


//包裹扩展消耗
bool CONFIG::readCfgExtendPkg()
{

	string strIniFile = getConfigDir() + "extend.ini";

	//ID	Type	Currency_Type	Square_Num	money_price							

//	类型	Type	int	1背包，2仓库，3矿石背包															
//	货币类型	Currency_Type	int	1为元宝2为铜钱															
//	格子数	Square_Num	int	扩充后达到的格子数															

	_mapPkgExtendCfg.clear();
	
	eachIniSecLine(strIniFile, "extend", [&](  const char* line )
	{

		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 5 )//wm
			return;
		
		PKG_EXTEND_CFG extend;

		DWORD Currency_Type = ATOUL_(vecString[2]);

		if ( Currency_Type == 1 )
			extend.needIngot = ATOUL_(vecString[4]);
		else if ( Currency_Type == 2 )
			extend.needCoin = ATOUL_(vecString[4]);
		else
			return;

		BYTE Type = ATOB_(vecString[1]);//1背包，2仓库，3矿石背包
		BYTE Square_Num = ATOB_(vecString[3]);
		
		_mapPkgExtendCfg.insert( make_pair( (Type<<8)|Square_Num, extend ) );

	}
	);


	return true;
}


//ID	Type	Currency_Type	Square_Num	money_price
CONFIG::PKG_EXTEND_CFG* CONFIG::getPkgExtendCfg(ROLE* role, PKG_TYPE pkgType )
{

	E_PKG_EXTEND Type;
	BYTE nextLimit;
	
	if ( pkgType == PKG_TYPE_EQUIP )
	{
		Type = E_PKG_EXTEND_PACK;
		nextLimit = role->m_packmgr.curEquipPkgLimit+5;
	}
	else if ( pkgType == PKG_TYPE_FUWEN )
	{
		Type = E_PKG_EXTEND_STORAGE;
		//nextLimit = role->m_packmgr.curStorageLimit+5;
		nextLimit = role->m_packmgr.curFuwenPkgLimit;
	}
	//else if ( pkgType == PKG_TYPE_MINE )
	//{
	//	Type = E_PKG_EXTEND_MINE;
	//	nextLimit = role->m_packmgr.curMineLimit+5;
	//}
	else if ( pkgType == PKG_TYPE_ITEM )
	{
		Type = E_PKG_EXTEND_UTILITY;
		nextLimit = role->m_packmgr.curMineLimit+5;
	}
	else
		return NULL;

	auto it = _mapPkgExtendCfg.find( (Type<< 8) | nextLimit );
	if ( it == _mapPkgExtendCfg.end() )
		return NULL;

	return &it->second;

}

DWORD CONFIG::getPkgExtendCost(ROLE* role, PKG_TYPE pkgType )
{
	CONFIG::PKG_EXTEND_CFG* pkgExtendCfg = getPkgExtendCfg( role, pkgType );
	if ( pkgExtendCfg == NULL )
		return 0;

	return pkgExtendCfg->needCoin ? pkgExtendCfg->needCoin : pkgExtendCfg->needIngot;

}

WORD CONFIG::getMerComposeBookID( WORD type )
{
	if ( type >= _vecMerComposeBookID.size() )
	{
		return 0;
	}
	else
	{
		return _vecMerComposeBookID[type];
	}
}

void CONFIG::forEachComposeBookCfg( std::function<void ( const std::pair<WORD, MER_COMPOSE_BOOK_CFG> &x )> func )
{
	for_each(  _mapMerComposebookCfg.begin(),  _mapMerComposebookCfg.end(), func);
}
void CONFIG::for_each_tencent_task_cfg(std::function<void(const std::pair<uint8, tencent_task>& ten_task)> func)
{
	for_each(_map_tencent_task_cfg.begin(), _map_tencent_task_cfg.end(), func);
}
void CONFIG::forEachMercenaryDefCfg( std::function< void ( const std::pair<WORD, MERDEF_CFG> &x )> func )
{
	for_each( _mapMercenaryDefinitionCfg.begin(), _mapMercenaryDefinitionCfg.end(), func );
}

void CONFIG::forEachSuitCfg(  WORD id, std::function< void ( const SUIT_CFG& x )> func )
{
	typedef multimap<WORD, CONFIG::SUIT_CFG>::iterator   multiMapSuitItor;

	std::pair< multiMapSuitItor, multiMapSuitItor> pos = _multimapSuitCfg.equal_range( id );

	while ( pos.first != pos.second )
	{
		func( pos.first->second );
		++pos.first;
	}
}


void CONFIG::forEachRechargeAwardCfg( std::function< void ( const RECHARGE_AWARD_CFG& rechargeEle)> rechargeFunc )
{
	for_each( _vecRechargeAwardCfg.begin(), _vecRechargeAwardCfg.end(), rechargeFunc );
}

void  CONFIG::resetEliteMap( std::function< void ( const map<WORD, DOUBLE_WORD>&x )> func)
{
	func(_mapEliteCfg);
}
//
CONFIG::MINE_CFG* CONFIG::getMineCfg(WORD wLevel )
{

	for ( auto &it : _vecMineCfg )
	{
		if ( wLevel <= it.level_max )
		{			
			return &it;
		}
	}

	return NULL;
}

CONFIG::ITEM_CFG* CONFIG::getDigDropMineIndex(WORD wLevel )
{

	CONFIG::MINE_CFG* mineCfg = getMineCfg(wLevel );
	if ( mineCfg )
	{
		RETURN_COND( mineCfg->vecMineRandSum.empty() , NULL );

		int index = BASE::getIndexOfVectorSum( mineCfg->vecMineRandSum );

		return getItemCfg( mineCfg->vecMineIndex[index] );	
	}

	return NULL;
}


CONFIG::ITEM_CFG* CONFIG::getDigDropItemCfg(CONFIG::MINE_CFG* mineCfg )
{
	RETURN_COND( mineCfg->vecMineRandSum.empty() , NULL );

	int index = BASE::getIndexOfVectorSum( mineCfg->vecMineRandSum );

	return getItemCfg( mineCfg->vecMineIndex[index] );	

}


DWORD CONFIG::getDigDropMineSpeed(WORD wLevel )
{

	CONFIG::MINE_CFG* mineCfg = getMineCfg(wLevel );
	if ( mineCfg )
		return mineCfg->speed;

	return 0;
}

DWORD CONFIG::getEquipDonatePoint(DWORD quality, DWORD subType )
{
	if ( quality > MAX_RARE_LEVEL || quality == 0 )
		return 0;
	if ( subType > SUB_TYPE_EQUIP_GEM || subType < SUB_TYPE_EQUIP_WEAPON )
		return 0;
	
	return _vDonatePoint[ quality-1 ][ subType ];

}


DWORD CONFIG::getCoinDonateEach()
{
	return _coinDonate;
}


ITEM_INFO CONFIG::getDropCreateInfo( CONFIG::DROP_INFO_CFG* dropInfo, WORD groupID)//, bool bChest )
{

	auto it = dropInfo->mapDropGroup.find( groupID );
	if ( it == dropInfo->mapDropGroup.end() )
		return ITEM_INFO(0, 0);

	CONFIG::GROUP_INFO &groupInfo = it->second;		

	RETURN_COND( groupInfo.vRandSum.empty() , ITEM_INFO(0, 0) );

	int dropIndex = BASE::getIndexOfVectorSum( groupInfo.vRandSum );
	const CONFIG::DROP_PARA& dropPara = groupInfo.vDropPara[ dropIndex ];		

	DWORD num = (DWORD)BASE::randBetween( dropPara.dropMin, dropPara.dropMax+1);
	
	//BYTE byQuality = (BYTE)BASE::randBetween( dropPara.qualityMin, dropPara.qualityMax+1);  //old 

	RETURN_COND( dropPara.vecQuaRand.empty() , ITEM_INFO(0, 0) );

	int qIndex = BASE::getIndexOfVectorSum( dropPara.vecQuaRand );
	BYTE byQuality = dropPara.vecQuality[ qIndex ];		


	//WORD itemIndex = (WORD)dropPara.itemIndex;   //old

	//if ( dropPara.vecItem.size() > 0 )
	//{
		WORD itemIndex = dropPara.vecItem[ randTo( dropPara.vecItem.size() ) ];
	//}
	
	//如果使用chest,		高1位:使用chest,低7位属性条数
	
	return ITEM_INFO( itemIndex, num, byQuality );//, 
					//bChest ? ( (1<<7) | (BYTE)BASE::randBetween( dropPara.randTimesMin, dropPara.randTimesMax+1) ) : 0);
}

bool CONFIG::getRandItem(DWORD dropID, vector<ITEM_INFO>& tryV, bool bUseChest )
{

	CONFIG::DROP_INFO_CFG* dropInfo = bUseChest ? CONFIG::getDropInfoChestCfg( dropID ) : CONFIG::getDropInfoCfg( dropID );
	if ( dropInfo == NULL )
		return false;

	if ( dropInfo->groupType == CONFIG::E_DROP_ROLL_SINGLE ) //单roll
	{
//grouptype 1:组外单roll, 组内partial_sum
//grouptype 2:partial_sum

		for ( size_t i = 0;i<dropInfo->vRandSum.size(); i++)
		{

			if ( RAND( ONCE_ROLL_BASE ) >= dropInfo->vRandSum[ i ]  )
				continue;
			
			auto it = dropInfo->mapDropGroup.find( i+1);
			if ( it == dropInfo->mapDropGroup.end() )
				continue;

			//dropCreateByGroupID( role, dropInfo, i+1, vCreateItemInfo );
			ITEM_INFO cii = getDropCreateInfo( dropInfo, i+1);//, bUseChest);
			//if ( cii.boxType )
			{
				tryV.push_back( cii );
			}
		}
		
		//return 0;

	}
	else //2
	{
		RETURN_COND( dropInfo->vRandSum.empty() , false );

		int index =  BASE::getIndexOfVectorSum( dropInfo->vRandSum );
		
		//return dropCreateByGroupID( role, dropInfo, index+1, vCreateItemInfo );

		ITEM_INFO cii = getDropCreateInfo( dropInfo, index+1);//, bUseChest );
		//if ( cii.boxType )
		{
			tryV.push_back( cii );
		}
		
	}

	return true;
	//return createByVector( role, tryV, vCreateItemInfo);

}

DWORD CONFIG::getRoleUpgradeExp(WORD wLevel)
{
	if ( wLevel > _vecExpCfg.size() )
		return 0;
	
	return _vecExpCfg[ wLevel -1].upgrade_exp;
}


template<class map_key,class return_value, class map_value>
return_value getvalue_bykey(const map<map_key, map_value> level_map, const map_key key)
{
	auto it = level_map.find(key);
	if (it == level_map.end())
	{
		return 0;
	}
	else
	{
		return it->second.upgrade_exp;
	}
}

DWORD CONFIG::getVipUpgradeExp(WORD wLevel)
{
	return getvalue_bykey<uint16, ulong, VIP_ADDITION_CFG>(_mapVipAddCfg, wLevel);
}
DWORD CONFIG::get_guild_shrine_upgrade_exp(uint16 level)
{
	return getvalue_bykey<uint16, ulong, guild_shrine>(_map_guild_shrine_cfg, level);
}

uint16 CONFIG::get_guild_shrine_max_level()
{
	return _map_guild_shrine_cfg.size();
}
DWORD CONFIG::addRoleExp( ROLE* role, DWORD& dwCurExp, DWORD dwAddExp, bool bKillMon  )
{

	WORD wOldLevel = role->wLevel;
	DWORD dwAddexp = addExp( role->wLevel, dwCurExp, dwAddExp, _vecExpCfg.size(), getRoleUpgradeExp );
	if ( role->wLevel > wOldLevel  )
	{
		RoleMgr::getMe().onLevelUP( role, wOldLevel );
	}

	return dwAddexp;
}


//返回得到的经验
DWORD CONFIG::addExp(WORD& wCurLevel, DWORD& dwCurExp, DWORD dwAddExp, WORD maxLevel, std::function<DWORD(WORD)> funcGetUpgradeExp )
{
	DWORD dwOld = dwAddExp;

	while ( dwAddExp )
	{
		DWORD upgrade_exp = funcGetUpgradeExp( wCurLevel );
		if ( upgrade_exp==0 )//满级
		{
			dwCurExp = 0;//wm客户端需要显示的
			break;
		}

		DWORD dwMax = upgrade_exp;
		DWORD dwNeed = 0;
		
		if (dwMax > dwCurExp)
		{
			dwNeed = dwMax - dwCurExp;
		}

		if ( dwNeed > dwAddExp )
		{
			dwCurExp += dwAddExp;
			dwAddExp = 0;
			break;
		}
		else
		{
			dwAddExp  -= dwNeed;

			if ( wCurLevel >= maxLevel )
			{
				dwCurExp = dwMax;
				break;
			}

			dwCurExp = 0;
			wCurLevel++;
				
		}
	}

	return dwOld - dwAddExp;

}



//gm权限名单
int CONFIG::readCfgGmName(void)
{
	string strIniFile = getConfigDir() + "gmname.ini";

	_vecGmName.clear();
	//_vecGmName.resize(20);
	eachIniSecLine(strIniFile, "gmname", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 1)
		{
			return;
		}

		_vecGmName.push_back(vecString[ 0 ] );
	}
	);	
	return 0;
}

//bool read_xml_node_value(TiXmlNode* node, double& value)
//{
//	if(node == nullptr)
//		return false;
//	auto temp = node->FirstChild();
//	if(temp == nullptr)
//		return false;
//	value = atof(temp->Value());
//	return true;
//}
//shield words
int CONFIG::readCfgShieldWords(void)
{

	string config_file = string( getProgDir() )+ STR_FSEP + "config.xml";

	string strFileName = getConfigDir() + "shield_word.xml";
	TiXmlDocument doc;
	RETURN_COND( !doc.LoadFile(strFileName.c_str()), -1 );

	TiXmlElement* root = doc.RootElement();
	RETURN_COND( nullptr == root, -1 );

	TiXmlNode* node = root->FirstChild("word");
	RETURN_COND( nullptr == node, -1 );

	auto temp = node->FirstChild();
	RETURN_COND( temp == nullptr, -1 );

	string strWord = temp->Value();
	_vecShieldWords.clear();
	//_vecShieldWords.resize( 2048 );

	strtoken2Vec( strWord.c_str(), SPLIT_SPACE, _vecShieldWords, NULL, NULL);

	return 0;
}

//[mail]
//jingjichang = 您今日竞技场排名第%d位，获得奖励：%s

//boss_hurt = 今日世界boss已经结束。您总共造成：%u点伤害，获得了：%s
//boss_hurt_rank = 今日世界boss已经结束。您的伤害排名第%u位，获得了：%s，%s
//boss_last_hurt = 您对今日世界boss发动了最后一击，获得了：%s，%s

bool CONFIG::readCfgMsg(void)
{
	string strIniFile = getConfigDir() + "mail.ini";

	_msgJingjichang  = BASE::readIni<string>( strIniFile, "mail", "jingjichang", "" );
	_msgBossHurtRank = BASE::readIni<string>( strIniFile, "mail", "boss_hurt_rank", "" );
	_msgBossLastHurt = BASE::readIni<string>( strIniFile, "mail", "boss_last_hurt", "" );

	_send_one   = BASE::readIni<string>( strIniFile, "mail", "send_one", "" );
	_send_two   = BASE::readIni<string>( strIniFile, "mail", "send_two", "" );
	_send_three = BASE::readIni<string>( strIniFile, "mail", "send_three", "" );
	_7ripaihangbang = BASE::readIni<string>(strIniFile, "mail", "7ripaihangbang", "");
	_treasure = BASE::readIni<string>(strIniFile, "mail", "treasure", "");
	_meet_breakcombo = BASE::readIni<string>(strIniFile, "mail", "meet_breakcombo", "");
	_platform_task_reward = BASE::readIni<string>(strIniFile, "mail", "platform_task_reward", "");
	return true;
}


bool CONFIG::readCfgRechargeAward(void)
{
	string strIniFile = getConfigDir() + "recharge_award.ini";

	_vecRechargeAwardCfg.clear();

	eachIniSecLine(strIniFile, "recharge_award", [&](  const char* line )
	{
		vector<string> vecString;
		if ( strtoken2Vec( line, SPLIT_SPACE, vecString, NULL, NULL) < 13)
		{
			return;
		}
		RECHARGE_AWARD_CFG rechargeAwardCfg;
		rechargeAwardCfg.id = ATOW_(vecString[0]);
		rechargeAwardCfg.job = ATOB_(vecString[1]);
		rechargeAwardCfg.item = ATOW_(vecString[2]);
		rechargeAwardCfg.type = ATOB_(vecString[3]);
		rechargeAwardCfg.number = ATOW_(vecString[4]);
		rechargeAwardCfg.rare = ATOB_(vecString[5]);
		rechargeAwardCfg.strengthen = ATOB_(vecString[6]);
		rechargeAwardCfg.random[0] = ATOUL_(vecString[7]);
		rechargeAwardCfg.random[1] = ATOUL_(vecString[8]);
		rechargeAwardCfg.random[2] = ATOUL_(vecString[9]);
		rechargeAwardCfg.random[3] = ATOUL_(vecString[10]);
		rechargeAwardCfg.random[4] = ATOUL_(vecString[11]);
		rechargeAwardCfg.hole = (WORD)ATOUL_(vecString[12]);

		_vecRechargeAwardCfg.push_back( rechargeAwardCfg );		

	}
	);	
	return true;
}

static bool CONFIG::readCfgTecentTask(void)
{
	string file_name = getConfigDir() + "tencent_task.ini";
	_map_tencent_task_cfg.clear();
	eachIniSecLine(file_name, "tencent_task", [&](const char* line)
	{
		vector<string> vec_string;
		if (strtoken2Vec(line, SPLIT_SPACE, vec_string, NULL, NULL) < 5)
		{
			return;
		}
		tencent_task tecent_info_cfg;
		tecent_info_cfg.step = ATOB_(vec_string[0]);
		tecent_info_cfg.type = ATOB_(vec_string[2]);
		tecent_info_cfg.para = ATOB_(vec_string[3]);

		const string& str_award = vec_string[4];
		vector<string> item;
		assert(strtoken2Vec(str_award.c_str(), ",", item, NULL, NULL) == 2);
		tecent_info_cfg.item = ITEM_INFO(ATOW_(item[0]), ATOUL_(item[1]));

		_map_tencent_task_cfg.insert( make_pair(tecent_info_cfg.step, tecent_info_cfg) );
	}
	);
	return true;
}

static bool CONFIG::readcfg_guild_shrine(void)
{
	string file_name = getConfigDir() + "guild_shrine.ini";
	_map_guild_shrine_cfg.clear();
	eachIniSecLine(file_name, "guild_shrine", [&](const char* line)
	{
		vector<string> vec_str;
		if ( strtoken2Vec(line, SPLIT_SPACE, vec_str, NULL, NULL) < 5 )
		{
			return;
		}
		guild_shrine guild_shrine_cfg;
		guild_shrine_cfg.id = ATOW_(vec_str[0]);
		guild_shrine_cfg.level = ATOW_(vec_str[1]);	
		guild_shrine_cfg.upgrade_exp = ATOUL_(vec_str[2]);
		guild_shrine_cfg.time = ATOW_(vec_str[3]);
		guild_shrine_cfg.percent = ATOW_(vec_str[4]);
		_map_guild_shrine_cfg.insert(pair<uint16, guild_shrine>(guild_shrine_cfg.level, guild_shrine_cfg));
	}
	);

	return true;
}

bool CONFIG::isGmName( const string& strName )
{
	auto it = std::find( _vecGmName.begin(), _vecGmName.end(), strName );
	if (it != _vecGmName.end() )
	{
		return true;
	}

	return false;
}

void CONFIG::add_gm_name(const string& role_name)
{
	_vecGmName.push_back(role_name);
}

bool CONFIG::isSieldWord( const string& strName )
{

	auto it = std::find( _vecShieldWords.begin(), _vecShieldWords.end(), strName );

	if (it != _vecShieldWords.end() )
	{
		return true;
	}

	return false;
}

bool CONFIG::isInnerSieldWord( const string& strName )
{

	for ( auto& str : _vecShieldWords )
	{
		unsigned int loc = str.find( strName.c_str(), 0 );
		if( loc != string::npos )
		{
			return true;
		}
	}

	return false;
}


const string& CONFIG::getMsgJingjichang()
{
	return _msgJingjichang;
}

const string& CONFIG::getMsgBossHurtRank()
{
	return _msgBossHurtRank;

}
const string& CONFIG::getMsgBossLastHurt()
{
	return _msgBossLastHurt;
}

const string& CONFIG::getMsgSendOne()
{
	return _send_one;
}

const string& CONFIG::getMsgSendTwo()
{
	return _send_two;
}
const string& CONFIG::getMsgSendThree()
{
	return _send_three;
}

const string& CONFIG::getMsgSend7DayAward()
{
	return _7ripaihangbang;
}

const string& CONFIG::get_treasure()
{
	return _treasure;
}

const string& CONFIG::get_meet_breakcombo()
{
	return _meet_breakcombo;
}

const string& CONFIG::get_platform_task_reward()
{
	return _platform_task_reward;
}

CONFIG::DROP_EARNING* CONFIG::getRandMonDrop( WORD wMapID )
{
	auto it = _mapMapCfg.find( wMapID );
	if ( it == _mapMapCfg.end() )
		return NULL;

	return it->second.getDropItemCache();
}

CONFIG::DROP_EARNING* CONFIG::MAP_CFG::getDropItemCache()
{
	auto size = _vecDropEarning.size();
	if ( size == 0 )
		return NULL;

	if ( _curIndex >= size )
		_curIndex = 0;

	return &_vecDropEarning[_curIndex++];
}

DWORD CONFIG::getMerUpstarMaxExpCfg()
{
	return _dwMercenaryStarMaxExpCfg;
}

void CONFIG::MAP_CFG::refreshMonsterDropLibrary( size_t dropTimes )
{	
	for ( auto & it : this->_vecDropEarning )
	{
		CC_SAFE_DELETE(it.ic);
	}
	this->_vecDropEarning.clear();

	DWORD dwCoin = DWORD( this->stDropMoney[E_DROP_MON].moneyrate * randInter(this->stDropMoney[E_DROP_MON].minMoney,this->stDropMoney[E_DROP_MON].maxMoney) );

	for( size_t i=0; i<dropTimes; i++)
	{
		vector<ITEM_INFO> tryV;
		CONFIG::getRandItem( this->dropID[E_DROP_MON], tryV );
		if ( tryV.empty() )
		{
			this->_vecDropEarning.push_back( DROP_EARNING( this->exp, dwCoin, NULL ) );
		}
		else
		{
			auto& ii = tryV[0];
			CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( ii.itemIndex );
			if ( itemCfg==NULL )
				continue;

			//不会有矿石
			ITEM_CACHE* itemCache = new ITEM_CACHE( itemCfg, ii.itemNums, (BYTE)ii.byQuality );
			this->_vecDropEarning.push_back( DROP_EARNING( this->exp, dwCoin, itemCache ) );
		}
	}
}

const CONFIG::IP_INFO* CONFIG::findIP( DWORD ip )
{
	auto it = _mapIPInfo.lower_bound( ip );
	if ( it == _mapIPInfo.end() )
		return NULL;

	return &it->second;
}

const CONFIG::MERCENARY_CFG* CONFIG::findOpenMercenary( WORD wLevel )
{
	auto it = _mapMercenaryCfg.lower_bound( (DWORD)wLevel );

	if ( it == _mapMercenaryCfg.end() )
	{
		return NULL;
	}

	return &it->second;
}

const CONFIG::ARENA_REWARD_CFG* CONFIG::getArenaRewardCfg( DWORD dwRank )
{
	auto it = _mapArenaRewardCfg.lower_bound( dwRank );

	if ( it == _mapArenaRewardCfg.end() )
	{
		return NULL;
	}

	return &it->second;
}

bool CONFIG::readCfgIP()
{
	return true;
#ifdef WIN32
	array<char, 100> host;
	gethostname(host.data(), host.size());
	if (stricmp(host.data(), "FYTER") == 0)
		return true;
#endif
	string strIniFile = getConfigDir() + "ip.txt";

	FILE *fp =  fopen(strIniFile.c_str(), "rb");
	if ( fp == NULL )
		return false;

	char* data = (char*)malloc( 30*1024*1024);
	char* p = data;
	
	size_t fileSize = 4096;
	while( 1 )
	{
		fileSize = fread(p, sizeof(unsigned char), fileSize,fp);
		p += fileSize;

		if ( fileSize == 0 )
			break;
	}
	p = data;
	char* end;
	while( nullptr != (end = strchr( p, '\n' )) )
	{
		*end++ = 0;

#if 0
		vector<string> vecString;
		if ( strtoken2Vec( (char*)p, SPLIT_SPACE, vecString, NULL, NULL) != 4 )
			break;
#else

		DWORD StartIPNum=0, EndIPNum=0;
		char* end1;

		end1= strchr( p, '\t' );
		if ( end1 )
		{
			*end1++ = 0;
			StartIPNum = ATOUL(p);
			p = end1;
		}

		end1= strchr( p, '\t' );
		if ( end1 )
		{
			*end1++ = 0;
			EndIPNum = ATOUL(p);
			p = end1;
		}

		IP_INFO & ipinfo = _mapIPInfo[EndIPNum];
		ipinfo.StartIPNum = StartIPNum;
		end1= strchr( p, '\t' );
		if ( end1 )
		{
			*end1++ = 0;
			ipinfo.Country = p;
			p = end1;
		}

		ipinfo.Local = p;
#endif
		p = end;
	}
	free(data);
	fclose(fp);
	return true;
}



bool CONFIG::initCfg(void)
{
	std::cout << "config file load start." << std::endl;
	m_mapCfgRead.clear();
	m_mapCfgRead["att_upgrade"] = readCfgAttrUpgrade ;					
	m_mapCfgRead["monster"] = readCfgMonster ;						
	m_mapCfgRead["arena_reward"] = readCfgArenaReward ;
	m_mapCfgRead["7_day_ranking"] = readCfgSecd7DayAward;
	m_mapCfgRead["arena_bouns"] = readCfgArenaBouns ;
	m_mapCfgRead["suit"] = readCfgSuit ;
	m_mapCfgRead["sign"] = readCfgSign ;
	m_mapCfgRead["vip"] = readCfgVipAdd ;						
	m_mapCfgRead["campaign"] = readCfgActivityInfo ;					
	m_mapCfgRead["skill"] = readCfgSkill ;	
	m_mapCfgRead["effect"] = readCfgEffect ;							
	m_mapCfgRead["player"] = readCfgPlayer ;						
	m_mapCfgRead["compose"] = readCfgCompose ;						
	m_mapCfgRead["exp"] = readCfgRoleExp ;						
	m_mapCfgRead["att_random"] = readCfgAttRandom ;						
	m_mapCfgRead["guild_build"] = readCfgGuildBuild ;					
	m_mapCfgRead["drop"] = readCfgDrop ;							
	m_mapCfgRead["moneytree"] = readCfgMoneyTree ;	
	m_mapCfgRead["moneytree0"] = readCfgMoneyTree0 ;	
	m_mapCfgRead["equip_donate"] = readCfgFactionDonation ;				
	m_mapCfgRead["mine"] = readCfgMine ;												
	m_mapCfgRead["shop1"] = readCfgShop1 ;							
	m_mapCfgRead["shop2"] = readCfgShop2 ;	
	m_mapCfgRead["shop_myth0"] = readCfgVipShop0 ;
	m_mapCfgRead["shop_myth1"] = readCfgVipShop1 ;						
	m_mapCfgRead["shop_myth2"] = readCfgVipShop2 ;						
	m_mapCfgRead["discount"] = readCfgDisCount ;													
	m_mapCfgRead["mission"] = readCfgMission ;
	m_mapCfgRead["open_activity"] = readCfgOpenActivity ;		
	m_mapCfgRead["map"] = readCfgMap ;
	m_mapCfgRead["extend"] = readCfgExtendPkg ;
	m_mapCfgRead["smelt"] = readCfgSmelt ;
	m_mapCfgRead["equip_index"] = readCfgForgeRresh ;//打造刷新
	m_mapCfgRead["rune"] = readCfgRune ; // 读取符文表
	m_mapCfgRead["attribute_deform"] = readCfgAttributeDeform ;
	m_mapCfgRead["mercenary_train"] = readCfgMercenaryTrain ;
	m_mapCfgRead["mercenary_train_max"] = readCfgMercenaryTrainMax;
	m_mapCfgRead["mercenary"] = readCfgMercenary ;
	m_mapCfgRead["mercenary_definition"] = readCfgMercenaryDefinition ;
	m_mapCfgRead["mercenary_halo"] = readCfgMercenaryHalo ;
	m_mapCfgRead["mercenary_upstar"] = readCfgMercenaryUpstar ;
	m_mapCfgRead["mercenary_composebook"] = readCfgMercenaryComposeBook ;
	m_mapCfgRead["gem"] = readCfgGem ;
	m_mapCfgRead["target_award"] = readCfgTargetAward;
	m_mapCfgRead["charge"] = readCfgCharge;
	m_mapCfgRead["luck"] = readCfgLuck ;
	m_mapCfgRead["recharge_award"] = readCfgRechargeAward ;
	m_mapCfgRead["tencent_task"] = readCfgTecentTask;
	m_mapCfgRead["guild_shrine"] = readcfg_guild_shrine;
	m_mapCfgRead["td_monster"] = readCfgTdMonster;
	readCfgMsg();
	readCfgGmName();
	readCfgShieldWords();
	readCfgIP();
	readCfgItem();	
	reloadCfg(STR_ALL);

	std::cout << "config file load done." << std::endl;
	return true;
}

bool CONFIG::reloadCfg(const string& strCfgName)
{
	if ( strCfgName == STR_ALL )
	{
		for( auto& func : m_mapCfgRead )
		{
			func.second();
		}
		logonline("%s", strCfgName.c_str());
	}
	else
	{
		auto it = m_mapCfgRead.find( strCfgName );
		if ( it != m_mapCfgRead.end() )
		{
			it->second();
		}
		logonline("%s", strCfgName.c_str());
	}

	return true;
}

CONFIG::SHOP1_CFG*  CONFIG::getShop1Cfg( const DWORD id )
{
	auto it = _mapShop1Cfg.upper_bound( id );
	if ( it == _mapShop1Cfg.end() )
		return NULL;

	return &it->second;
}

CONFIG::SHOP1_CFG*  CONFIG::getVipShop1Cfg( const DWORD id )
{
	auto it = _mapVipShop1Cfg.upper_bound( id );
	if ( it == _mapVipShop1Cfg.end() )
		return NULL;

	return &it->second;
}


vector<WORD>* CONFIG::getTaskVecIDsByType( const BYTE& type)
{
	if ( _mapTaskTypeIDs.find(type) == _mapTaskTypeIDs.end() )
	{
		return NULL;
	}
	return &_mapTaskTypeIDs[type];
}

vector<WORD>* CONFIG::getOpenActivityVecIDsByType( const BYTE type)
{
	if ( _mapOpenActivityTypeIDs.find(type) == _mapOpenActivityTypeIDs.end() )
	{
		return NULL;
	}
	return &_mapOpenActivityTypeIDs[type];
}

template<typename KEY, typename VALUE>
VALUE* getEle( map<KEY, VALUE>& m, KEY k)
{
	auto it =  m.find(k);
	if ( it == m.end() )
		return NULL;
	
	return &it->second;			
}

template<typename KEY, typename VALUE>
VALUE getValueEle( map<KEY, VALUE>& m, KEY k)
{
	auto it =  m.find(k);
	if ( it == m.end() )
		return 0;

	return it->second;			
}

#define DEF_GET_CONFIG(vtype, ktype, funname, m)	\
vtype* CONFIG::funname(ktype id)\
{\
	return getEle<ktype, vtype>( m, id );\
}

#define DEF_GET_VALUE_CONFIG(vtype, ktype, funname, m)	\
vtype CONFIG::funname(ktype id)\
{\
	return getValueEle<ktype, vtype>( m, id );\
}
DEF_GET_CONFIG(CONFIG::guild_shrine, uint16, get_guild_shrine_cfg, _map_guild_shrine_cfg);
DEF_GET_CONFIG(CONFIG::tencent_task, uint8, getTencentTaskCfg, _map_tencent_task_cfg);
DEF_GET_CONFIG(CONFIG::MISSION_CFG, WORD, getMissionCfg, _mapMissionCfg);
DEF_GET_CONFIG(CONFIG::OPEN_ACTIVITY, WORD, getOpenActivityCfg, _mapOpenActivityCfg);
DEF_GET_CONFIG(CONFIG::ITEM_CFG, WORD, getItemCfg, _mapItemCfg);
DEF_GET_CONFIG(CONFIG::SKILL_CFG2, WORD, getSkillCfg, _mapSkillCfg);
DEF_GET_CONFIG(CONFIG::SKILL_EFF_CFG, WORD, getSkillEffCfg, _mapSkillEffCfg);
DEF_GET_CONFIG(CONFIG::ATT_RAND_CFG, DWORD, getAttRandCfg, _mapAttRandCfg);
DEF_GET_CONFIG(CONFIG::COMPOSE_CFG, DWORD, getComposeCfg, _mapComposeCfg);
DEF_GET_CONFIG(CONFIG::GUILD_BUILD_CFG, WORD, getGuildBuildCfg, _mapGuildBuildCfg);
DEF_GET_CONFIG(CONFIG::ATTRDEFORM_CFG, BYTE, getAttrDeformCfg, _mapAttributeDeformCfg);
DEF_GET_CONFIG(CONFIG::DROP_INFO_CFG, DWORD, getDropInfoCfg, _mapDropInfoCfg);
DEF_GET_CONFIG(CONFIG::DROP_INFO_CFG, DWORD, getDropInfoChestCfg, _mapDropInfoChestCfg);
DEF_GET_CONFIG(CONFIG::MERCENARY_CFG, DWORD, getMercenaryCfg, _mapMercenaryCfg);
DEF_GET_CONFIG(CONFIG::MERDEF_CFG, WORD, getMerDefCfg, _mapMercenaryDefinitionCfg);
DEF_GET_CONFIG(CONFIG::MERHALO_CFG, WORD, getMerHaloCfg, _mapMercenaryHaloCfg);
DEF_GET_CONFIG(CONFIG::MERCENARY_TRAIN_CFG, BYTE, getMercenaryTrainCfg, _mapMercenaryTrainCfg);
DEF_GET_CONFIG(CONFIG::MER_STAR_CFG, BYTE, getMerUpstarCfg, _mapMercenaryStarCfg);
DEF_GET_CONFIG(CONFIG::MAP_CFG, WORD, getMapCfg, _mapMapCfg);
DEF_GET_CONFIG(CONFIG::MONSTER_CFG, WORD, getMonsterCfg, _mapMonsterCfg);
DEF_GET_CONFIG(CONFIG::ARENA_BOUNS_CFG, DWORD, getArenaBounsCfg, _mapArenaBounsCfg);
DEF_GET_CONFIG(CONFIG::SEND_7DAY_AWARD_CFG, DWORD, getSend7DayAwardCfg, _mapSend7DayAwardCfg);
DEF_GET_CONFIG(DWORD, WORD, getMoneyTreeCfg, _mapMoneyTreeCfg);
DEF_GET_CONFIG(DWORD, WORD, getComposeMaxExpCfg, _mapComposeMaxExpCfg);
DEF_GET_CONFIG(DWORD, DWORD, getComposeNowExpCfg, _mapComposeNowExpCfg);
DEF_GET_CONFIG(CONFIG::SIGN_CFG, DWORD, getSignCfg, _mapSignCfg);
DEF_GET_CONFIG(CONFIG::SIGN_CFG, BYTE, getSignTimeCfg, _mapSignTimeCfg);
DEF_GET_CONFIG(CONFIG::VIP_ADDITION_CFG, WORD, getVipAddCfg, _mapVipAddCfg);
DEF_GET_CONFIG(CONFIG::ACTIVITY_INFO_CFG, WORD, getActivityInfoCfg, _mapActivityInfoCfg);
DEF_GET_CONFIG(CONFIG::SHOP2_CFG, WORD, getShop2Cfg, _mapShop2Cfg);
DEF_GET_CONFIG(CONFIG::SHOP2_CFG, WORD, getVipShop2Cfg, _mapVipShop2Cfg);
DEF_GET_CONFIG(CONFIG::SKILL_STORAGE, BYTE, getSkillStorage, _mapJobGroupSkillCfg);
DEF_GET_CONFIG(CONFIG::MER_COMPOSE_BOOK_CFG, WORD, getMerComposebookCfg, _mapMerComposebookCfg);
DEF_GET_CONFIG(CONFIG::GEM_CFG, WORD, getGemCfg, _mapGemCfg);
DEF_GET_CONFIG(CONFIG::SHOP0_CFG, BYTE, getVipShop0Cfg, _mapVipShop0Cfg);
DEF_GET_CONFIG(CONFIG::MONEY_TREE0_CFG, WORD, getMoneyTree0Cfg, _mapMoneyTree0Cfg);
DEF_GET_CONFIG(float, DWORD, getLuckCfg, _mapLuckCfg);
DEF_GET_CONFIG(CONFIG::TARGET_AWARD_CFG, WORD, getTargetAwardCfg, _mapTargetAwardCfg);
DEF_GET_VALUE_CONFIG(DWORD, BYTE, getMerUpstarNowExpCfg, _mapMercenaryStarNowExpCfg);
DEF_GET_VALUE_CONFIG(DWORD, WORD, getMercenaryTrainMaxCfg, _mapMercenaryTrainMaxCfg);
DEF_GET_VALUE_CONFIG(DWORD, DWORD, getChargeCfg, _mapChargeCfg);
DEF_GET_CONFIG(CONFIG::TD_MONSTER_CFG, WORD, getTdMonsterCfg, _mapTdMonsterCfg);
map<DWORD, CONFIG::ATTR_UPGRADE_CFG> &mapAttrUpgradeCfg = CONFIG::_mapAttrUpgradeCfg;
map<WORD, CONFIG::MONSTER_CFG> &mapMonsterCfg = CONFIG::_mapMonsterCfg;
map<WORD, CONFIG::SKILL_CFG2> &mapSkillCfg = CONFIG::_mapSkillCfg;
map<WORD, CONFIG::ITEM_CFG> &mapItemCfg = CONFIG::_mapItemCfg;
map<WORD, CONFIG::PLAYER_CFG> &mapPlayerCfg = CONFIG::_mapPlayerCfg;
map<WORD, CONFIG::ACTIVITY_INFO_CFG> &mapActivityInfoCfg = CONFIG::_mapActivityInfoCfg;
map<WORD, CONFIG::MISSION_CFG> &mapMissionCfg = CONFIG::_mapMissionCfg;
map<WORD, CONFIG::OPEN_ACTIVITY> &mapOpenActivityCfg = CONFIG::_mapOpenActivityCfg;

