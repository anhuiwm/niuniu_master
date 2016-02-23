#pragma once
#include "base.h"
#include "log.h"
#include "config.h"
#include "item.h"
#include "roleskill.h"
#include "packMgr.h"
#include "mercenary.h"
#include "rolerune.h"
#include "roleWorldBoss.h"
#include "fightCtrl.h"
#include "shop.h"
#include "team.h"

class guild;


#pragma pack(push, 1) 
class battle_info
{
public:
	time_t time = 0;
	eFIGHTTYPE type = eFIGHTTYPE::E_FIGHT_NONE;
	unsigned target_roleid = 0;
	byte result = 0;
	uint16 mapid = 0;

	void reset()
	{
		time = 0;
		type = eFIGHTTYPE::E_FIGHT_NONE;
		target_roleid = 0;
		result = 0;
		mapid = 0;
	}
};
#pragma pack(pop)

//玩家信息
class ROLE
{

public:
	ROLE();
	~ROLE();

	RoleSkill role_skill;													//玩家技能相关
	PackMgr   m_packmgr;													//包裹管理器
	RoleRune  role_rune;													//玩家符文相关
	RoleWorldBoss role_worldboss;											//世界BOSS相关
	TFightDrop monster_battle_info;											// 普通战斗信息
	CMercenaryMgr cMerMgr;													//佣兵管理
	CMarketMgr cMarMgr;														//商城管理
	string uuid;
	string serial;
	string roleName;
	string phone_imei;														//手机唯一识别号
	string roleZone;
	//map<WORD,PRACTISE_SKILL> mapSkills;									//存放技能< 组 + 技能信息表>
	vector<DWORD>  vecDianZhanIDs;											//好友dwRoleID列表
	vector<DWORD>  vecFriends;												//好友dwRoleID列表
	vector<DWORD> vecReanaArmy;												//竞技场对手ID
	list<meet_pk_record> meet_pk_records;									// 偶遇PK记录
	map<WORD, WORD> mapSuit;												//拥有套装ID及其数量
	vector< WORD > vecSuit;													//已经得到的套装
	vector<DWORD>  vvecFightSourceHelpers[E_JOB_MONK];						//世界boss战友列表库 不存
	list<ARENA_RECORD> listRenaRecord;										//竞技场记录
	map<DWORD, MAIL> mapMail;
	map<WORD, MAIN_TASK_INFO> mapTask;										//任务
	map<WORD, MAIN_TASK_INFO> map_platform_task;							//腾讯等平台任务
	map<WORD,DOUBLE_WORD> mapElite;											//精英关卡<组，组内当前ID>
	vector<WORD> vecTempGetItems;											//不存数据库 道具使用产生的item
	map<WORD, MAIN_TASK_INFO> mapCompleteActivityIDs;						//完成的开服活动
	vector<DWORD> vecFirstCharge;											//首次充值序列
	string charge_account;													//充值流水账号串
	string email;															//邮箱账号
	vector<BYTE> vecCodeAwardType;											//already get code award types
	list<pair<unsigned, unsigned>> rewards_for_activity_tao;				// 淘宝活动结果数据
	vector<jiange_team::TeamFightProcess> vecTeamFightProcess;				//组队战过程
	list<TeamFightRecord> listTeamFightRecord;								//组队战记录
	vector<DWORD> vecTeamRoleIDs;//队伍中队员id
	time_t last_guild_signin = 0;

	ITEM_CACHE* vBodyEquip[BODY_GRID_NUMS];									//memset 分隔符
	DWORD  vecBossFightHelpers[WORLD_BOSS_HELP_NUM];						//世界boss战友列表 不存
	guild* faction;															//不保存到blob
	WORD suitStrengthLevel;													//身上装备等级和
	WORD wLastFightMapID;													//挂机地图
	WORD wCanFightMaxMapID;													//当前可以打的地图(编号大于这个的地图都不能打)
	WORD wWinMaxMapID;														//胜的最大地图
	time_t tLastSaveTime;													//不保存
	DWORD tLogoutTime;														//退出时间
	BYTE byTitleId;
	session* client;
	WORD wLevel;															//等级
	DWORD dwFightValue;														//战斗力
	BYTE byJob;
	BYTE bySex;
	WORD wVipLevel;															//VIP等级
	DWORD dwExp;															//当前经验
	DWORD dwHP;																//体力
	DWORD dwCoin;															//货币(游戏币)
	DWORD dwIngot;															//元宝(充值币)
	DWORD dwCharge;															//累计充值
	DWORD tLastAccess;														//最近访问时间
	DWORD tIncHPTime;														//补充体力的时间
	DWORD dwHonour;															//荣耀
	BYTE byMailLoaded;
	BYTE byDirty;
	DWORD dwRoleID;
	BYTE byFightSkill[QUICK_SKILL_INDEXS];									//战斗技能索引，指向_skillId[10]中1-10位置，0表示没有
	WORD wOnUpSkill;														//正在修炼技能id,0表示不在修炼
	DWORD roleAttr[E_ATTR_MAX];												//0不用,1-21
	BYTE byArenaDayCount;													//每日已经参加竞技场比赛次数
	BYTE byBuyArenaDayCount;												//每日已经购买竞技场比赛次数
	DWORD dwDianZhan;														//最近访问时间
	BYTE byPredictIndex;
	PREDICT_EARNING predictEarning[PREDICT_NUMS];
	time_t lastKillMonAddExpTime;											//这个不保存
	BYTE prizeMailNums;
	BYTE msgMailNums;
	BYTE systemMailNums;
	DWORD ____dwComID;															//竞技场对手id 需要保存
	BYTE battle_result;														// 战斗结果
	BYTE bySignTimes;														//sign times per month
	DWORD dwDaySignStatus;													//day sign status bit
	BYTE byEnhanceTime;														//活动boss战鼓舞次数

	BYTE bySendFightBossTimes;												//挑战boss次数每天三次0点重置
	BYTE byDailyBuyBossTimes;												//购买挑战boss次数不重置
	BYTE byAllBuyBossTimes;

	BYTE bySendFightHeroTimes;												//精英关卡
	BYTE byDailyBuyHeroTimes;												//购买挑战hero次数不重置
	BYTE byAllBuyHeroTimes;													

	BYTE byUseMoneyTreeTimes;												//moneytree次数0点重置
	BYTE byBuyQuickFightTimes;												//购买快速战斗次数
	DWORD dwAutoSellOption;
	DWORD dwMiningRemainTime;												//本人挖矿剩余时间
	BYTE byMiningSlotIndex;													//本人挖矿的坑位，这个位置和vMiner里的空位共同决定房间的空位
	BYTE byRes____;															//切换矿场，临时刷出来的，挖矿的坑位，
	MINER vMiner[ROOM_MINER_NUMS];
	MINER vMinerTmpRefresh[ROOM_MINER_NUMS];								//点击切换时刷新出来的临时列表
	WORD  wCurMaxTaskID;
	DWORD dwCanEnterFightMapTime;											//可以进入地图刷怪时间，不保存
	DWORD dwCanFightMapTime;												//可以进入地图打怪时间，不保存
	DWORD dwFinalWorldBossTime;												//最后参与世界boss时间
	WORD wTmpVipLevel;
	DWORD dwTmpVipEndTime;													//暂时VIP 失效时间
	WORD wLoginVip = 0;
	DWORD dwSmeltValue;														//熔炼值
	BYTE byForgeRreshTimes;													//打造界面已经刷新次数,日清
	ITEM_CACHE *itemForge;													//打造界面当前装备
	BYTE byNewMapKillMonNum;												//进入新地图统计杀怪数量，最高100
	BYTE  byNewbieGuideStep;												//新手引导步骤
	DWORD dwFisrtLoginTime;													//初始登录时间
	BYTE  byCurLevelAwardStep;												//按等级领取奖励
	BYTE  byRechargeState;													//首充奖励状态 0
	time_t close_user_valid_time = 0;										// 封号有效时间
	bool deny_chat = 0;															// 禁言
	unsigned pkvalue = 0;													// PK值
	unsigned pk_count = 0;													// PK次数
	unsigned pk_buy_count = 0;													// PK次数
	unsigned pk_dead_count = 0;													// PK死亡次数
	unsigned ____meet_pk_target_roleid = 0;															//偶遇PK目标ID 需要保存
	unsigned last_pk_count_inc_time = 0;						// 最后pk数量递增时间
	unsigned meet_pk_combo_count = 0;						// 偶遇PK连击数
	unsigned last_pk_protect_inc_time = 0;						// 最后pk保护递增时间
	unsigned meet_pk_protect_count = 0;						// 偶遇保护数
	BYTE login_day_num = 0;									//累计登陆天数
	class battle_info battle_info;					// 最后战斗请求时间
	bool first_battle = true;
	bool gm_account = false;
	unsigned meet_pk_max_combo_count = 0;						// 偶遇PK连击数
	int exit_guild_time = 0;						// 退出公会时间
	uint16 shrine_times = 0;//圣地膜拜次数
	uint bodyguard_loot_count = 2;// 镖局可抢劫次数
	uint bodyguard_buy_loot_count = 0;// 购买的镖局可抢劫次数
	BYTE jiangeState = 0;//0未参加 1已经战斗未领奖状态 2已领奖  3未领奖未参加
	DWORD teamID = 0;//组队的队伍id
	BYTE teamBuff = 0;//组队战购买的buff
	int teamFightValue = 0;//组队的战斗力 不存

	byte set_battle_info(eFIGHTTYPE type, uint target_id, uint16 map_id);// 设置战斗信息
	BYTE isCanSetNewSkill()		;
	BYTE isCanUpSkill()			;
	BYTE isCanActivityReward()	;
	BYTE isCanTaskReward()		;
	BYTE isNotSign()			;	
	BYTE isMailReward()			;
	BYTE isNotJoinWorldBoss()	;	
	BYTE isMysteryCommonFree()	;
	BYTE isMysteryHignFree()	;	
	BYTE haveUseMoneyTreeTimes();	
	BYTE haveDivideTimes()		;
	BYTE haveFightArenaTimes()	;
	BYTE haveFightBossTimes()	;	
	BYTE haveFightEelteTimes()	;
	BYTE haveQuickFightTimes()	;
	BYTE canForgeEquip()		;	
	BYTE isFullEquipPkg()		;	
	BYTE haveItemBox()			;
	BYTE isFriendEmpty()		;
	void calRoleFightValue( );
	void calRoleAttr( );
	void tranformBaseAttr(const BYTE& type, const DWORD& value);
	void addSuitExtraAttr();
	WORD getVipLevel();
	WORD getExchangeSuitLimit();
	void roleSave2DB();
	void cache2Blob(ROLE_BLOB& roleBlob );
	bool blob2Cache( const ROLE_BLOB& roleBlob );
	void online();															// 上线
	void offline();															// 下线
	void onLevelUp();														// 玩家升级时
	bool hasEnoughCoin(DWORD need_coin);									// 是否有足够的金币
	bool subCoin(DWORD need_coin, bool notify = true);						// 消耗指定数量的金币
	bool hasEnoughGold(DWORD need_gold);									// 是否有足够的元宝
	bool subGold(DWORD need_gold, bool notify = true);						// 消耗指定数量的元宝
	void addSmeltValue(DWORD value);										// 增加熔炼值
	bool hasEnoughSmeltValue(DWORD value);									// 是否有足够的熔炼值
	bool subSmeltValue(DWORD value);										// 扣除熔炼值
	BYTE roleGetCharge( const int nChargeNum, const int buyRealNum, bool bGm = false );
	void resetVip();
	void resetEliteMap();													//重置精英关卡
	BYTE getMinEquipLevel();												//获取最小装备等级 item.ini表里
	void setFirstLoginTime( DWORD dwTime );												
	DWORD getFirstLoginTime();
	string get_phone_imei();
	int setNewbieStep( ROLE* role, unsigned char * data, size_t dataLen);
	int getLevelAward( ROLE* role, unsigned char * data, size_t dataLen);
	int getRechargeAward( ROLE* role, unsigned char * data, size_t datalen );
	int getTodayGuide( ROLE* role, unsigned char * data, size_t datalen );
	int getTimeGuide( ROLE* role, unsigned char * data, size_t datalen );
	int setEmailAddress( ROLE* role, unsigned char * data, size_t datalen );
	int getEmailAddress( ROLE* role, unsigned char * data, size_t datalen );
	int getCodeAward( ROLE* role, unsigned char * data, size_t datalen );
	void addFiveDivineTime();
	void patch();

	// 战斗
	void send_battle_report(byte type, byte result, uint16 delay_time, uint coin, uint ingot, uint exp, vector<DROP_SEQ>& drop_items);// 发送战斗报告
	int arena_fight_report(uint target_role_id, byte result);// 竞技场战斗报告
	int meet_fight_report(uint target_role_id, byte result);// 偶遇战斗报告
	int bodyguard_fight_report(uint target_role_id, byte result);// 劫镖战斗报告
	int monster_fight_report(byte result);// 普通战斗报告
	int elite_fight_report(byte result);// 精英战斗报告
	int boss_fight_report(byte result);// 老怪战斗报告

	meet_pk_record* get_meet_pk_result(unsigned roleid, bool attack);
	void update_meet_pk_count();
	void setVipLevel( const WORD wLevel  );
	void setLoginVipLevel(const WORD wLevel);
	void equip_get_forging_info(input_stream& stream);// 神器锻造
	void equip_forging(input_stream& stream);// 神器锻造
	void equip_star_up(input_stream& stream);// 神器升星
	void equip_inherit(input_stream& stream);// 神器传承
	void send(output_stream& out);
	bool reward_item( unsigned item_id, unsigned count = 1);// 奖励道具
	void reward_item(vector<DROP_SEQ>& drop_items, unsigned item_id, unsigned count = 1);// 奖励道具
	void battle_over(eFIGHTTYPE battle_type);
	void battle_tip(input_stream& stream);
	void guild_contribute(input_stream& stream);// 捐献
	void guild_wordship(input_stream& stream);//圣地膜拜
	void guild_get_contribute_info(input_stream& stream);// 获取捐献信息
	void guild_join_guild(input_stream& stream);// 申请加入公会
	void guild_set_join_condition(input_stream& stream);// 会长设置入会条件
	void guild_find(input_stream& stream);
	void guild_signin(input_stream& stream);
	void chat(input_stream& stream);
	void send_firstcharge_list();
	ITEM_CACHE* get_best_equip();
	void login_broad();
	void send_offline_report();
	bool is_low_off_time( uint32_t atime);

	int get_wordship_info(unsigned char* data_ptr, size_t data_len);
	int guild_wordship(unsigned char* data_ptr, size_t data_len);
	void send_activitys_info();
	void leaveTeam();
};

typedef std::shared_ptr<ROLE> RolePtr;
