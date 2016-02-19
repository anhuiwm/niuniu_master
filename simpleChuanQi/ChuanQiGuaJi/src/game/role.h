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


//�����Ϣ
class ROLE
{

public:
	ROLE();
	~ROLE();

	RoleSkill role_skill;													//��Ҽ������
	PackMgr   m_packmgr;													//����������
	RoleRune  role_rune;													//��ҷ������
	RoleWorldBoss role_worldboss;											//����BOSS���
	TFightDrop stFightDrop;													//����
	CMercenaryMgr cMerMgr;													//Ӷ������
	CMarketMgr cMarMgr;														//�̳ǹ���
	string uuid;
	string serial;
	string roleName;
	string roleZone;
	list<WORD> lstMonsterIndex;
	//map<WORD,PRACTISE_SKILL> mapSkills;									//��ż���< �� + ������Ϣ��>
	vector<DWORD>  vecDianZhanIDs;											//����dwRoleID�б�
	vector<DWORD>  vecFriends;												//����dwRoleID�б�
	vector<DWORD> vecReanaArmy;												//����������ID
	list<meet_pk_record> meet_pk_records;									// ż��PK��¼
	map<WORD, WORD> mapSuit;												//ӵ����װID��������
	vector< WORD > vecSuit;													//�Ѿ��õ�����װ
	vector<DWORD>  vvecFightSourceHelpers[E_JOB_MONK];						//����bossս���б�� ����
	list<ARENA_RECORD> listRenaRecord;										//��������¼
	map<DWORD, MAIL> mapMail;
	map<WORD, MAIN_TASK_INFO> mapTask;										//����
	map<WORD,DOUBLE_WORD> mapElite;											//��Ӣ�ؿ�<�飬���ڵ�ǰID>
	vector<WORD> vecTempGetItems;											//�������ݿ� ����ʹ�ò�����item
	map<WORD, MAIN_TASK_INFO> mapCompleteActivityIDs;						//��ɵĿ����
	vector<DWORD> vecFirstCharge;											//�״γ�ֵ����
	string charge_account;													//��ֵ��ˮ�˺Ŵ�
	string email;															//�����˺�
	vector<BYTE> vecCodeAwardType;											//already get code award types


	ITEM_CACHE* vBodyEquip[BODY_GRID_NUMS];									//memset �ָ���
	DWORD  vecBossFightHelpers[WORLD_BOSS_HELP_NUM];						//����bossս���б� ����
	FACTION* faction;														//�����浽blob
	WORD suitStrengthLevel;													//����װ���ȼ���
	WORD wLastFightMapID;													//�һ���ͼ
	WORD wCanFightMaxMapID;													//��ǰ���Դ�ĵ�ͼ(��Ŵ�������ĵ�ͼ�����ܴ�)
	WORD wWinMaxMapID;														//ʤ������ͼ
	time_t tLastSaveTime;													//������
	DWORD tLogoutTime;														//�˳�ʱ��
	BYTE byTitleId;
	session* client;
	WORD wLevel;															//�ȼ�
	DWORD dwFightValue;														//ս����
	BYTE byJob;
	BYTE bySex;
	WORD wVipLevel;															//VIP�ȼ�
	DWORD dwExp;															//��ǰ����
	DWORD dwHP;																//����
	DWORD dwCoin;															//����(��Ϸ��)
	DWORD dwIngot;															//Ԫ��(��ֵ��)
	DWORD dwCharge;															//�ۼƳ�ֵ
	DWORD tLastAccess;														//�������ʱ��
	DWORD tIncHPTime;														//����������ʱ��
	DWORD dwHonour;															//��ҫ
	BYTE byMailLoaded;
	BYTE byDirty;
	DWORD dwRoleID;
	BYTE byFightSkill[QUICK_SKILL_INDEXS];									//ս������������ָ��_skillId[10]��1-10λ�ã�0��ʾû��
	WORD wOnUpSkill;														//������������id,0��ʾ��������
	DWORD roleAttr[E_ATTR_MAX];												//0����,1-21
	BYTE byArenaDayCount;													//ÿ���Ѿ��μӾ�������������
	BYTE byBuyArenaDayCount;												//ÿ���Ѿ����򾺼�����������
	DWORD dwDianZhan;														//�������ʱ��
	BYTE byPredictIndex;
	PREDICT_EARNING predictEarning[PREDICT_NUMS];
	time_t lastKillMonAddExpTime;											//���������
	BYTE prizeMailNums;
	BYTE msgMailNums;
	BYTE systemMailNums;
	DWORD dwComID;															//����������id ��Ҫ����
	BYTE byArenaResult;														//���������
	BYTE bySignTimes;														//sign times per month
	DWORD dwDaySignStatus;													//day sign status bit
	BYTE byEnhanceTime;														//�bossս�������

	BYTE bySendFightBossTimes;												//��սboss����ÿ������0������
	BYTE byDailyBuyBossTimes;												//������սboss����������
	BYTE byAllBuyBossTimes;

	BYTE bySendFightHeroTimes;												//��Ӣ�ؿ�
	BYTE byDailyBuyHeroTimes;												//������սhero����������
	BYTE byAllBuyHeroTimes;													

	BYTE byUseMoneyTreeTimes;												//moneytree����0������
	BYTE byBuyQuickFightTimes;												//�������ս������
	DWORD dwAutoSellOption;
	DWORD dwMiningRemainTime;												//�����ڿ�ʣ��ʱ��
	BYTE byMiningSlotIndex;													//�����ڿ�Ŀ�λ�����λ�ú�vMiner��Ŀ�λ��ͬ��������Ŀ�λ
	BYTE byRes____;															//�л��󳡣���ʱˢ�����ģ��ڿ�Ŀ�λ��
	MINER vMiner[ROOM_MINER_NUMS];
	MINER vMinerTmpRefresh[ROOM_MINER_NUMS];								//����л�ʱˢ�³�������ʱ�б�
	WORD  wCurMaxTaskID;
	DWORD dwCanEnterFightMapTime;											//���Խ����ͼˢ��ʱ�䣬������
	DWORD dwCanFightMapTime;												//���Խ����ͼ���ʱ�䣬������
	DWORD dwFinalWorldBossTime;												//����������bossʱ��
	WORD wTmpVipLevel;
	DWORD dwTmpVipEndTime;													//��ʱVIP ʧЧʱ��
	DWORD dwSmeltValue;														//����ֵ
	BYTE byForgeRreshTimes;													//��������Ѿ�ˢ�´���,����
	ITEM_CACHE *itemForge;													//������浱ǰװ��
	BYTE byNewMapKillMonNum;												//�����µ�ͼͳ��ɱ�����������100
	BYTE  byNewbieGuideStep;												//������������
	DWORD dwFisrtLoginTime;													//��ʼ��¼ʱ��
	BYTE  byCurLevelAwardStep;												//���ȼ���ȡ����
	BYTE  byRechargeState;													//�׳佱��״̬ 0
	time_t close_user_valid_time;										// �����Чʱ��
	bool deny_chat;															// ����
	unsigned pkvalue;													// PKֵ
	unsigned pk_count;													// PK����
	unsigned pk_buy_count;													// PK����
	unsigned pk_dead_count;													// PK��������
	unsigned meet_pk_target_roleid;															//����������id ��Ҫ����
	unsigned last_pk_count_inc_time;						// ���pk��������ʱ��
	unsigned meet_pk_combo_count;						// ż��PK������
	BYTE login_day_num;									//�ۼƵ�½����
	

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
	void online();															// ����
	void offline();															// ����
	void onLevelUp();														// �������ʱ
	bool hasEnoughCoin(DWORD need_coin);									// �Ƿ����㹻�Ľ��
	bool subCoin(DWORD need_coin, bool notify = true);						// ����ָ�������Ľ��
	bool hasEnoughGold(DWORD need_gold);									// �Ƿ����㹻��Ԫ��
	bool subGold(DWORD need_gold, bool notify = true);						// ����ָ��������Ԫ��
	void addSmeltValue(DWORD value);										// ��������ֵ
	bool hasEnoughSmeltValue(DWORD value);									// �Ƿ����㹻������ֵ
	bool subSmeltValue(DWORD value);										// �۳�����ֵ
	BYTE roleGetCharge( const int nChargeNum, const int buyRealNum );
	void resetVip();
	void resetEliteMap();													//���þ�Ӣ�ؿ�
	BYTE getMinEquipLevel();												//��ȡ��Сװ���ȼ� item.ini����
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
