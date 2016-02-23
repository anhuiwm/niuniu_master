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
	TFightDrop monster_battle_info;											// ��ͨս����Ϣ
	CMercenaryMgr cMerMgr;													//Ӷ������
	CMarketMgr cMarMgr;														//�̳ǹ���
	string uuid;
	string serial;
	string roleName;
	string phone_imei;														//�ֻ�Ψһʶ���
	string roleZone;
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
	map<WORD, MAIN_TASK_INFO> map_platform_task;							//��Ѷ��ƽ̨����
	map<WORD,DOUBLE_WORD> mapElite;											//��Ӣ�ؿ�<�飬���ڵ�ǰID>
	vector<WORD> vecTempGetItems;											//�������ݿ� ����ʹ�ò�����item
	map<WORD, MAIN_TASK_INFO> mapCompleteActivityIDs;						//��ɵĿ����
	vector<DWORD> vecFirstCharge;											//�״γ�ֵ����
	string charge_account;													//��ֵ��ˮ�˺Ŵ�
	string email;															//�����˺�
	vector<BYTE> vecCodeAwardType;											//already get code award types
	list<pair<unsigned, unsigned>> rewards_for_activity_tao;				// �Ա���������
	vector<jiange_team::TeamFightProcess> vecTeamFightProcess;				//���ս����
	list<TeamFightRecord> listTeamFightRecord;								//���ս��¼
	vector<DWORD> vecTeamRoleIDs;//�����ж�Աid
	time_t last_guild_signin = 0;

	ITEM_CACHE* vBodyEquip[BODY_GRID_NUMS];									//memset �ָ���
	DWORD  vecBossFightHelpers[WORLD_BOSS_HELP_NUM];						//����bossս���б� ����
	guild* faction;															//�����浽blob
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
	DWORD ____dwComID;															//����������id ��Ҫ����
	BYTE battle_result;														// ս�����
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
	WORD wLoginVip = 0;
	DWORD dwSmeltValue;														//����ֵ
	BYTE byForgeRreshTimes;													//��������Ѿ�ˢ�´���,����
	ITEM_CACHE *itemForge;													//������浱ǰװ��
	BYTE byNewMapKillMonNum;												//�����µ�ͼͳ��ɱ�����������100
	BYTE  byNewbieGuideStep;												//������������
	DWORD dwFisrtLoginTime;													//��ʼ��¼ʱ��
	BYTE  byCurLevelAwardStep;												//���ȼ���ȡ����
	BYTE  byRechargeState;													//�׳佱��״̬ 0
	time_t close_user_valid_time = 0;										// �����Чʱ��
	bool deny_chat = 0;															// ����
	unsigned pkvalue = 0;													// PKֵ
	unsigned pk_count = 0;													// PK����
	unsigned pk_buy_count = 0;													// PK����
	unsigned pk_dead_count = 0;													// PK��������
	unsigned ____meet_pk_target_roleid = 0;															//ż��PKĿ��ID ��Ҫ����
	unsigned last_pk_count_inc_time = 0;						// ���pk��������ʱ��
	unsigned meet_pk_combo_count = 0;						// ż��PK������
	unsigned last_pk_protect_inc_time = 0;						// ���pk��������ʱ��
	unsigned meet_pk_protect_count = 0;						// ż��������
	BYTE login_day_num = 0;									//�ۼƵ�½����
	class battle_info battle_info;					// ���ս������ʱ��
	bool first_battle = true;
	bool gm_account = false;
	unsigned meet_pk_max_combo_count = 0;						// ż��PK������
	int exit_guild_time = 0;						// �˳�����ʱ��
	uint16 shrine_times = 0;//ʥ��Ĥ�ݴ���
	uint bodyguard_loot_count = 2;// �ھֿ����ٴ���
	uint bodyguard_buy_loot_count = 0;// ������ھֿ����ٴ���
	BYTE jiangeState = 0;//0δ�μ� 1�Ѿ�ս��δ�콱״̬ 2���콱  3δ�콱δ�μ�
	DWORD teamID = 0;//��ӵĶ���id
	BYTE teamBuff = 0;//���ս�����buff
	int teamFightValue = 0;//��ӵ�ս���� ����

	byte set_battle_info(eFIGHTTYPE type, uint target_id, uint16 map_id);// ����ս����Ϣ
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
	BYTE roleGetCharge( const int nChargeNum, const int buyRealNum, bool bGm = false );
	void resetVip();
	void resetEliteMap();													//���þ�Ӣ�ؿ�
	BYTE getMinEquipLevel();												//��ȡ��Сװ���ȼ� item.ini����
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

	// ս��
	void send_battle_report(byte type, byte result, uint16 delay_time, uint coin, uint ingot, uint exp, vector<DROP_SEQ>& drop_items);// ����ս������
	int arena_fight_report(uint target_role_id, byte result);// ������ս������
	int meet_fight_report(uint target_role_id, byte result);// ż��ս������
	int bodyguard_fight_report(uint target_role_id, byte result);// ����ս������
	int monster_fight_report(byte result);// ��ͨս������
	int elite_fight_report(byte result);// ��Ӣս������
	int boss_fight_report(byte result);// �Ϲ�ս������

	meet_pk_record* get_meet_pk_result(unsigned roleid, bool attack);
	void update_meet_pk_count();
	void setVipLevel( const WORD wLevel  );
	void setLoginVipLevel(const WORD wLevel);
	void equip_get_forging_info(input_stream& stream);// ��������
	void equip_forging(input_stream& stream);// ��������
	void equip_star_up(input_stream& stream);// ��������
	void equip_inherit(input_stream& stream);// ��������
	void send(output_stream& out);
	bool reward_item( unsigned item_id, unsigned count = 1);// ��������
	void reward_item(vector<DROP_SEQ>& drop_items, unsigned item_id, unsigned count = 1);// ��������
	void battle_over(eFIGHTTYPE battle_type);
	void battle_tip(input_stream& stream);
	void guild_contribute(input_stream& stream);// ����
	void guild_wordship(input_stream& stream);//ʥ��Ĥ��
	void guild_get_contribute_info(input_stream& stream);// ��ȡ������Ϣ
	void guild_join_guild(input_stream& stream);// ������빫��
	void guild_set_join_condition(input_stream& stream);// �᳤�����������
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
