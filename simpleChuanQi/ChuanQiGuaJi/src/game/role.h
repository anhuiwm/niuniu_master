#ifndef __ROLE_H__
#define __ROLE_H__

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
#include <list>
#include <memory>

using namespace std;


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
	TFightDrop stFightDrop;													//掉落
	CMercenaryMgr cMerMgr;													//佣兵管理
	CMarketMgr cMarMgr;														//商城管理
	string uuid;
	string serial;
	string roleName;
	string roleZone;
	list<WORD> lstMonsterIndex;
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
	map<WORD,DOUBLE_WORD> mapElite;											//精英关卡<组，组内当前ID>
	vector<WORD> vecTempGetItems;											//不存数据库 道具使用产生的item
	map<WORD, MAIN_TASK_INFO> mapCompleteActivityIDs;						//完成的开服活动
	vector<DWORD> vecFirstCharge;											//首次充值序列
	string charge_account;													//充值流水账号串
	string email;															//邮箱账号
	vector<BYTE> vecCodeAwardType;											//already get code award types


	ITEM_CACHE* vBodyEquip[BODY_GRID_NUMS];									//memset 分隔符
	DWORD  vecBossFightHelpers[WORLD_BOSS_HELP_NUM];						//世界boss战友列表 不存
	FACTION* faction;														//不保存到blob
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
	DWORD dwComID;															//竞技场对手id 需要保存
	BYTE byArenaResult;														//竞技场结果
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
	DWORD dwSmeltValue;														//熔炼值
	BYTE byForgeRreshTimes;													//打造界面已经刷新次数,日清
	ITEM_CACHE *itemForge;													//打造界面当前装备
	BYTE byNewMapKillMonNum;												//进入新地图统计杀怪数量，最高100
	BYTE  byNewbieGuideStep;												//新手引导步骤
	DWORD dwFisrtLoginTime;													//初始登录时间
	BYTE  byCurLevelAwardStep;												//按等级领取奖励
	BYTE  byRechargeState;													//首充奖励状态 0
	time_t close_user_valid_time;										// 封号有效时间
	bool deny_chat;															// 禁言
	unsigned pkvalue;													// PK值
	unsigned pk_count;													// PK次数
	unsigned pk_buy_count;													// PK次数
	unsigned pk_dead_count;													// PK死亡次数
	unsigned meet_pk_target_roleid;															//竞技场对手id 需要保存
	unsigned last_pk_count_inc_time;						// 最后pk数量递增时间
	unsigned meet_pk_combo_count;						// 偶遇PK连击数
	BYTE login_day_num;									//累计登陆天数
	

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
	BYTE roleGetCharge( const int nChargeNum, const int buyRealNum );
	void resetVip();
	void resetEliteMap();													//重置精英关卡
	BYTE getMinEquipLevel();												//获取最小装备等级 item.ini表里
	void setFirstLoginTime( DWORD dwTime );												
	DWORD getFirstLoginTime();
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
	int fight_report_for_meet(const DWORD target_role_id, const BYTE result, vector<DROP_SEQ>& drop_items);
	meet_pk_record* get_meet_pk_result(unsigned roleid, bool attack);
	void update_meet_pk_count();
	void setVipLevel( const WORD wLevel  );
};

typedef std::shared_ptr<ROLE> RolePtr;

#endif	//__ROLE_H__
