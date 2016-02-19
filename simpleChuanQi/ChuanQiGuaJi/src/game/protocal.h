#ifndef __PROTOCAL_H__
#define __PROTOCAL_H__


#include "libevent-net/net_util.h"

#include "base.h"
#include "log.h"
#include "item.h"
#include "config.h"
#include <list>
#include "session.h"

using namespace std;

//=======================================================================================

#define		S_BREAK							0x0007		//�Ͽ�
#define		S_CHARGE_ROLE					0x0008		//��ֵ
#define		S_CHECK_ROLE					0x0009		//360�˺�����
#define		S_CREATE_ROLE					0x000A		//������ɫ
#define		S_LOGIN							0x000B		//������Ϸ
#define		S_UPDATE_ROLE_DATA				0x000C		//�������Ըı�
#define     S_STUDY_NEW_SKILL				0x000D      //���ܷ���������
#define		S_FIGHT_AWARDS					0x000E		//ս������Ԥ��
#define		S_EQUIP_AWARD_FETCH				0x0010		//װ��������ȡ
#define		S_GAME_CHAT 					0x0011		//����
#define		S_REQUEST_BUY_INGOT				0x0012		//��ֵԪ��
#define		S_NEWBIE_GUIDE					0x0013		//��������
#define		S_GET_ITEMS_BRIEF				0x0020		//��ȡװ�����߼�Ҫ��Ϣ
#define		S_GET_ITEM_DETAIL				0x0021		//��Ϣװ��������Ϣ
#define		S_NOTIFY_NEW_ITEM				0x0022		//����������,����µ���
#define		S_PACKAGE_EXTEND				0x002B		//������չ
#define		S_MOVE_ITEM_TO					0x0023		//���߰������ƶ�
#define		S_EQUIP_ATTACH					0x0024		//��װ��
#define		S_EQUIP_DETACH					0x0025		//��װ��
#define		S_SELL_ITEMS					0x0026		//���߳���
#define		S_LOOK_OTHER_ITEM				0x0029		//�鿴������ҵ�����Ϣ
#define		S_LOOK_OTHER					0x002A		//�鿴���������Ϣ
#define		S_STRENGTHEN_PREVIEW			0x0034		//ǿ��Ԥ��
#define		S_CONFIRM_STRENGTHEN			0x0035		//ǿ��ȷ��
#define		S_MAKE_HOLE						0x0036		//װ�����
#define		S_INJECT_DIAMOND_AT				0x0037		//��Ƕ��ʯ
#define		S_UNINJECT_DIAMOND_AT			0x0038		//ж�±�ʯ
#define		S_UNINJECT_ALL_DIAMOND			0x0039		//ж�����б�ʯ
#define		S_EQUIP_RESET					0x003A		//װ��ϴ��
#define		S_SMELT_EQUIP_REMAIN			0x003B		//����Ԥ��
#define		S_SMELT_EQUIP_NEW				0x003C		//����
#define		S_REQUEST_FORGE_SUIT_NUM		0x003D		//���������װʣ�����
#define		S_FORGE_ITEM					0x003E		//����װ��
#define		S_EXCHANGE_EQUIP_NEW			0x003F		//�һ�װ��
#define		S_REQUEST_SUIT_INFO				0x0040		//��װ��Ϣ���ߺ����������
#define		S_ITEM_USE						0x004A		//���µ���ʹ��
#define		S_FETCH_RECHARGE_AWARD			0x004B		//��ȡ�׳佱��
#define		S_ENTER_MAP						0x0050		//--����ͼЭ��
#define		S_FIGHT_PROCESS  				0x0051		//ע��ԭ������Ҫ�ĵ�
#define		S_FIGHT_REQUEST					0x0052		//BOSS��ս����
#define		S_BUY_FIGHT 					0x0053		//����BOSS��ս����
#define		S_BUY_FIGHT_COST 				0x0054		//����BOSS��ս��Ҫ���
#define		S_GET_FIGHT_TIMES				0x0055		//�������
#define		S_FIGHT_END_REPORT				0x0056		//ս������
#define		S_REQUEST_ELITE_INFO			0x0057		//��Ӣ�ؿ���Ϣ����	
#define		S_REQUEST_SELECTED_ELITE_INFO	0x0058		//ѡ����ս�ؿ�	
#define		S_BUY_CHALLENGE_TIME			0x0059		//������ս����	
#define		S_ELITE_CHALLENGE				0x005A		//������ս�ؿ�		  
#define		S_QUICK_CHALLENGE				0x005B		//����ɨ��
#define	    S_MEET_PLAYER					0x005C		//��ȡż�����
#define	    S_MEET_BE_KILLED				0x005D		//ż��PK��������ʧ��
#define		S_OUYU_FIGHT_PRE				0x005E		//��ͼż��ս��ǰ
#define	    S_MEET_BUY_PK_TIMES				0x005F		//����PK����
#define		S_TODAY_GUIDE					0x0060		//�ͻ�������Ĺ̶�8����������
#define	    S_MEET_PK_RECORD				0x0061		//��ȡż��PK��¼
#define	    S_MEET_PK_RANK 					0x0062		//��ȡż��PK����
#define		S_TIME_GUIDE					0x0063		//�ͻ���20min���������
#define		S_FUNCTION_NOTIFY				0x0070		//���߹���֪ͨ,����������.
#define		S_REQUEST_EMAILS				0x0071		//�����ʼ�����
#define		S_DELETE_EMAIL					0x0072		//ɾ���ʼ�
#define		S_NOTIFY_EMAIL					0x0073		//�յ����ʼ�, ��������������
#define		S_DROPSET						0x0080		//�������������ǰ������
#define		S_REQUEST_DROPSET				0x0081		//���󱣴�����
#define		S_GET_SKILLS_INDEXS  			0x0107		//��ȡ����������Ϣ
#define		S_SKILL_MOVE_UP 				0x0108		//���������ƶ�
#define		S_GET_UNLOCK_SKILLS_LIST		0x0109		//��ÿ��������б�
#define		S_SAVE_SKILLS_INDEXS			0x010A		//���漼������˳��
#define		S_UP_SKILL_LEVEL				0x010B		//��������
#define		S_REQUEST_MINE					0x0150		//�������Ϣ
#define		S_DIG_MINE						0x0151		//�����ڿ�/�л����
#define		S_REQUEST_CHANGE_MINE			0x0152		//�л���
#define		S_ONE_KEY_OVER					0x0153		//һ������
#define		S_ONE_KEY_CONFIRM				0x0154
#define		S_NEW_MINER						0x0155		//��λ,����������
#define		S_MINER_LEAVE					0x0156
#define		S_NOTIFY_FETCH_MINE				0x0157		//���/������ʯ֪ͨ, ����������
#define		S_GET_FRIENDS					0x0200		//��ȡ��ǰ������Ϣ�б�
#define		S_ADD_FRIEND					0x0201		//add
#define		S_DEL_FRIEND					0x0202		//delete
#define		S_SEARCH_FRIEND					0x0203		//search
#define		S_RECOMMOND_FRIENDS				0x0204		//recommond
#define		S_FRIEND_INFO					0x0205		//get one friend infomation
#define		S_DIANZAN						0x0206		//����
#define		S_GET_FUWEN_STATUS				0x0230		//���ռ��״̬
#define		S_REQ_DIVINE					0x0231		//����һ��ռ��
#define		S_PUT_ON_FUWEN					0x0232		//���Ĵ���
#define		S_FUWEN_LEVELUP					0x0233		//��������
#define		S_REQUES_MERCENARY_INFO			0x0250		//����Ӷ����Ϣ
#define		S_SLAVE_UP_DOWN					0x0251		//Ӷ��������
#define		S_SLAVE_SKILL_LIST				0x0252		//����Ӷ�������б�
#define		S_SLAVE_REFRESH_SKILLS			0x0253		//����
#define		S_SLAVE_PRO_UPDATE				0x0254		//Ӷ�����Ը���֪ͨ,����������
#define		S_SLAVE_TRAIN_PREVIEW			0x0255		//Ӷ������Ԥ��
#define		S_SLAVE_TRAIN_SAVE				0x0256		//��������
#define		S_SLAVE_STAR_PRO_PREVIEW		0x0257		//Ӷ����������Ԥ��
#define		S_SLAVE_UPGRADE_STAR			0x0258		//��������
#define		S_REQUEST_COMBO_BOOKS_INFO		0x0259		//����ϳ��鱾��Ϣ
#define		S_COMBO_BOOK					0x025A		//�ϳ��鱾
#define		S_REQUEST_HALO_LIST				0x025B		//��ȡӶ���⻷�б�
#define		S_ACTIVE_HALO					0x025C		//����Ӷ���⻷����
#define		S_SLAVE_EQUIP_ATTACH			0x025D		//Ӷ������װ��
#define		S_SLAVE_BASR_ATTR   			0x025E		//������ʾ
#define		S_GET_WORLDBOSS_STATUS			0x0270		//�������BOSS״̬
#define		S_SIGNUP_WORLDBOSS				0x0271		//��������BOSS
#define		S_ATTACK_WORLDBOSS				0x0272		//��������BOSS
#define		S_EMBRAVE_WORLDBOSS				0x0273		//��������BOSS
#define		S_GET_WORLDBOSS_RANK			0x0274		//����˺�����
#define		S_GET_WORLDBOSS_COOLTIME		0x0275		//�����ȴʣ��ʱ��
#define		S_GET_ROLE_RANK_LEVEL			0x0301		//rank list by level
#define		S_GET_ROLE_RANK_FIGHTVALUE		0x0302		//rank list by fightvalue
#define		S_GET_ROLE_WEAPON				0x0303		//rank list by weapon
#define	    S_GET_ROLE_RANK_MATCH			0x0304		//rank list by match
#define		S_GUILD_LIST					0x0400		//��ȡ��ǰ�Ƽ��Ĺ�����Ϣ�б�
#define		S_GET_GUILD_RANK_LIST			0x0401		//��ȡ��ǰ�Ĺ���������Ϣ�б�
#define		S_GET_GUILD_req					0x0402		//����������� �������,��ʾ������������Ĺ����б�
#define		S_GET_GUILD_find				0x0403		//���ok���������Ĺ�������
#define		S_GUILD_add						0x0404		//�����л�����  ����������ľ��ǳ�������
#define		S_GUILD_create					0x0405		//�����л�
#define		S_GUILD_INFO					0x0406		//��������
#define		S_GUILD_create_content			0x0407		//����༭
#define		S_GUILD_INFO_MEMLIST			0x0408		//�����Ա�б�
#define		S_GUILD_INFO_MEM_EQUIP			0x0409		//�鿴��Աװ��
#define		S_GUILD_INFO_MEM_FRIEND			0x0410		//��ӳ�ԱΪ����
#define		S_GUILD_INFO_MEM_JOB			0x0411		//��Աְλ����
#define		S_GUILD_INFO_MEM_QUIT			0x0412		//����л�
#define		S_GUILD_INFO_QUIT				0x0413		//�˳��л�
#define		S_GUILD_INFO_REQ_LIST			0x0414		//�л�����б�
#define		S_GUILD_INFO_REQ_YES			0x0415		//ͬ����߾ܾ�id�����л�
#define		S_GUILD_INFO_REQ_ALL			0x0416		//ȫ��ͬ�����
#define		S_GUILD_INFO_SAY_LIST			0x0417		//���������б�
#define		S_GUILD_INFO_TEACH_LIST			0x0418		//����Ƽ�
#define		S_GUILD_INFO_TEACH_UP_LIST		0x0419		//����װ��(�Ƽ�����)
#define		S_GUILD_INFO_TEACH_UP_GOLD		0x0420		//���׽��
#define	    S_GET_ARENA_RECOMMEND			0x0500		//recommend arena list
#define	    S_ARENA_FIGHT					0x0501		//area fight
#define	    S_ARENA_BUY_COUNT				0x0502		//buy count
#define	    S_GET_ARENA_RECORD				0x0503		//require arena record
#define	    S_ARENA_COMPETITORINFO			0x0504		//require arena competitor information
#define	    S_ARENA_FIGHT_PRE				0x0505		//area fight pre
#define		S_FIGHT_ARENA_REPORT			0x0506		//������ս������
#define		S_ARENA_BUY_GOLD   				0x0507		//������������Ҫ����Ԫ��
#define		S_GET_SIGN_INFO     			0x0600		//get sign times
#define		S_CLIENT_SIGN   				0x0601		//client sign
#define	    S_GET_ACTIVITY					0x0800		//get activities
#define	    S_JOIN_ACTIVITY					0x0801		//join activity
#define	    S_BOSS_FIGHT					0x0802		//Bossս��
#define	    S_ENHANCE_BOSS_FIGHT			0x0803		//����
#define	    S_BOSS_TIME_LEFT				0x0804		//ս����ʼ����ʱ
#define	    S_BOSS_GET_HELPER				0x0805		//����
#define		S_BOSS_GET_HURT_RANK			0x0806		//�˺�����
#define	    S_GET_GOODS_LIST				0x0901		//get goods list
#define	    S_REQUEST_BUY					0x0902		//request buy 
#define	    S_REQUEST_BUY_ALL				0x0903		//request buy all
#define	    S_REQUEST_UPDATE_COST			0x0904		//request update fee
#define	    S_REQUEST_UPDATE				0x0905		//request update
#define	    S_REQUEST_VIPCOIN				0x0906		//request vipMoney
#define	    S_USE_MONEYTREE					0x0907		//
#define	    S_REQUEST_MONEYTREE				0x0908		//
#define	    S_REQUEST_MISTERY_SHOP			0x090A		//�����̳�
#define	    S_BUY_MISTERY_SHOP				0x090B		//�����̳�
#define	    S_GET_TASK_LIST					0x0910		//get task list
#define	    S_GET_TASK_GIFTS				0x0911		//get task reward 
#define		S_OFFLINE_FIGHT_REPORT			0x0920		//����ս������
#define		S_QUICK_FIGHT_REPORT			0x0921		//���ߺͿ���ս������
#define		S_BUY_QUICK_FIGHT 				0x0923		//ȷ�Ϲ������ս���������
#define		S_GET_QUICK_FIGHT_COST 			0x0924		//��ȡ�������ս��������Ҫ���
#define		S_GET_QUICK_FIGHT_TIMES			0x0925		//��ȡ����ÿ���ս������
#define	    S_GET_OPEN_REWARD_LIST			0x0930		//get open activity list
#define	    S_GET_OPEN_REWARD_GIFTS			0x0931		//get open activity reward 
#define	    S_GET_OPEN_ACTIVITY_INFO		0x0932		//get open activity info by ID
#define		S_REQUEST_BIND_EMAIL			0x0934		//�����ֵ�󶨵�����
#define     S_MODIFY_BIND_EMAIL				0x0935		//��������Ϣ	
#define		S_EXCHANGE_AWARDS_BY_KEY		0x0936		//�һ���(CDKey)�һ�����

//=======================================================================================


#define NNOW	( (DWORD)PROTOCAL::getCurTime() )

#define TIMEOUT(start, cd) ( NNOW >= start + cd ) 



namespace PROTOCAL
{

	int netInit();

	int execPacket(session *client, unsigned char * pkt, size_t len);
	int onAccept(session *client, int succ);
	int onDisconnect(session *client);

	void timerLongTimeNoLogin(int fd, short what, void *arg);
	//int clientActive( session *client, unsigned char * data, size_t dataLen );
	int roleLogin( session *client, unsigned char * data, size_t dataLen);
	int roleCreate( session *client, unsigned char * data, size_t dataLen );
	int roleCheckRole( session *client, unsigned char * data, size_t dataLen );
	int chargeRole( session *client, unsigned char * data, size_t dataLen );

	int chargeRole( session *client, const DWORD dwChargeRoleID, DWORD dwChargeNum, ROLE* role, bool bGm  );
	
	bool phpChargeRole(DWORD roleid, DWORD dwChargeNum, string charge_account );

	int gm(ROLE* role, const string& strContent);


	int clientChat( ROLE* role, unsigned char * data, size_t dataLen );

	string cmdPacket(WORD wCmd, const string& strInfo);
	string& makePkt3(string& strPKT, WORD wCmd, const string& strInfo);

	void sendClient(session* client, const string& str);



	void forEachOnlineRole( std::function<void ( const std::pair<DWORD, ROLE_CLIENT> &x )> func );

	session* getOnlineClient(DWORD dwRoleID);
	ROLE* getOnlineRole(DWORD dwRoleID);
	ROLE_CLIENT* getOnlineRoleClient(DWORD dwRoleID);

	//void selectSkillUpOk();
	void getOnlineID(ROLE* role, vector<DWORD>& vecIDs, WORD num);


	time_t getCurTime();
	void refreshTime();

	size_t get_online_count();

}
void saveAllToDb(bool bForce);
int gmSendMail( const string& roleName, const string& mailType, const string& content, const string& prize );

#endif	//__PROTOCAL_H__
