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

#define		S_BREAK							0x0007		//断开
#define		S_CHARGE_ROLE					0x0008		//充值
#define		S_CHECK_ROLE					0x0009		//360账号找人
#define		S_CREATE_ROLE					0x000A		//创建角色
#define		S_LOGIN							0x000B		//进入游戏
#define		S_UPDATE_ROLE_DATA				0x000C		//人物属性改变
#define     S_STUDY_NEW_SKILL				0x000D      //技能服务器推送
#define		S_FIGHT_AWARDS					0x000E		//战斗奖励预估
#define		S_EQUIP_AWARD_FETCH				0x0010		//装备奖励领取
#define		S_GAME_CHAT 					0x0011		//聊天
#define		S_REQUEST_BUY_INGOT				0x0012		//充值元宝
#define		S_NEWBIE_GUIDE					0x0013		//新手引导
#define		S_GET_ITEMS_BRIEF				0x0020		//获取装备道具简要信息
#define		S_GET_ITEM_DETAIL				0x0021		//信息装备道具信息
#define		S_NOTIFY_NEW_ITEM				0x0022		//服务器推送,获得新道具
#define		S_PACKAGE_EXTEND				0x002B		//包裹扩展
#define		S_MOVE_ITEM_TO					0x0023		//道具包裹间移动
#define		S_EQUIP_ATTACH					0x0024		//穿装备
#define		S_EQUIP_DETACH					0x0025		//脱装备
#define		S_SELL_ITEMS					0x0026		//道具出售
#define		S_LOOK_OTHER_ITEM				0x0029		//查看其他玩家道具信息
#define		S_LOOK_OTHER					0x002A		//查看其它玩家信息
#define		S_STRENGTHEN_PREVIEW			0x0034		//强化预览
#define		S_CONFIRM_STRENGTHEN			0x0035		//强化确认
#define		S_MAKE_HOLE						0x0036		//装备打孔
#define		S_INJECT_DIAMOND_AT				0x0037		//镶嵌宝石
#define		S_UNINJECT_DIAMOND_AT			0x0038		//卸下宝石
#define		S_UNINJECT_ALL_DIAMOND			0x0039		//卸载所有宝石
#define		S_EQUIP_RESET					0x003A		//装备洗练
#define		S_SMELT_EQUIP_REMAIN			0x003B		//熔炼预期
#define		S_SMELT_EQUIP_NEW				0x003C		//熔炼
#define		S_REQUEST_FORGE_SUIT_NUM		0x003D		//请求打造套装剩余次数
#define		S_FORGE_ITEM					0x003E		//打造装备
#define		S_EXCHANGE_EQUIP_NEW			0x003F		//兑换装备
#define		S_REQUEST_SUIT_INFO				0x0040		//套装信息上线后服务器推送
#define		S_ITEM_USE						0x004A		//最新道具使用
#define		S_FETCH_RECHARGE_AWARD			0x004B		//领取首充奖励
#define		S_ENTER_MAP						0x0050		//--进地图协议
#define		S_FIGHT_PROCESS  				0x0051		//注意原来的需要改掉
#define		S_FIGHT_REQUEST					0x0052		//BOSS挑战请求
#define		S_BUY_FIGHT 					0x0053		//购买BOSS挑战次数
#define		S_BUY_FIGHT_COST 				0x0054		//购买BOSS挑战需要金币
#define		S_GET_FIGHT_TIMES				0x0055		//请求次数
#define		S_FIGHT_END_REPORT				0x0056		//战斗结束
#define		S_REQUEST_ELITE_INFO			0x0057		//精英关卡信息请求	
#define		S_REQUEST_SELECTED_ELITE_INFO	0x0058		//选择挑战关卡	
#define		S_BUY_CHALLENGE_TIME			0x0059		//购买挑战次数	
#define		S_ELITE_CHALLENGE				0x005A		//请求挑战关卡		  
#define		S_QUICK_CHALLENGE				0x005B		//快速扫荡
#define	    S_MEET_PLAYER					0x005C		//获取偶遇玩家
#define	    S_MEET_BE_KILLED				0x005D		//偶遇PK被攻击后失败
#define		S_OUYU_FIGHT_PRE				0x005E		//地图偶遇战斗前
#define	    S_MEET_BUY_PK_TIMES				0x005F		//购买PK次数
#define		S_TODAY_GUIDE					0x0060		//客户端请求的固定8个今日引导
#define	    S_MEET_PK_RECORD				0x0061		//获取偶遇PK记录
#define	    S_MEET_PK_RANK 					0x0062		//获取偶遇PK排行
#define		S_TIME_GUIDE					0x0063		//客户端20min请求的引导
#define		S_FUNCTION_NOTIFY				0x0070		//上线功能通知,服务器推送.
#define		S_REQUEST_EMAILS				0x0071		//请求邮件内容
#define		S_DELETE_EMAIL					0x0072		//删除邮件
#define		S_NOTIFY_EMAIL					0x0073		//收到新邮件, 服务器在线推送
#define		S_DROPSET						0x0080		//打开向服务器请求当前的设置
#define		S_REQUEST_DROPSET				0x0081		//请求保存设置
#define		S_GET_SKILLS_INDEXS  			0x0107		//获取技能索引信息
#define		S_SKILL_MOVE_UP 				0x0108		//技能向上移动
#define		S_GET_UNLOCK_SKILLS_LIST		0x0109		//获得开启技能列表
#define		S_SAVE_SKILLS_INDEXS			0x010A		//保存技能索引顺序
#define		S_UP_SKILL_LEVEL				0x010B		//升级技能
#define		S_REQUEST_MINE					0x0150		//请求矿场信息
#define		S_DIG_MINE						0x0151		//请求挖矿/切换矿点
#define		S_REQUEST_CHANGE_MINE			0x0152		//切换矿场
#define		S_ONE_KEY_OVER					0x0153		//一键加速
#define		S_ONE_KEY_CONFIRM				0x0154
#define		S_NEW_MINER						0x0155		//补位,服务器推送
#define		S_MINER_LEAVE					0x0156
#define		S_NOTIFY_FETCH_MINE				0x0157		//获得/卖出矿石通知, 服务器推送
#define		S_GET_FRIENDS					0x0200		//获取当前好友信息列表
#define		S_ADD_FRIEND					0x0201		//add
#define		S_DEL_FRIEND					0x0202		//delete
#define		S_SEARCH_FRIEND					0x0203		//search
#define		S_RECOMMOND_FRIENDS				0x0204		//recommond
#define		S_FRIEND_INFO					0x0205		//get one friend infomation
#define		S_DIANZAN						0x0206		//点赞
#define		S_GET_FUWEN_STATUS				0x0230		//获得占卜状态
#define		S_REQ_DIVINE					0x0231		//进行一次占卜
#define		S_PUT_ON_FUWEN					0x0232		//符文穿戴
#define		S_FUWEN_LEVELUP					0x0233		//符文升级
#define		S_REQUES_MERCENARY_INFO			0x0250		//请求佣兵信息
#define		S_SLAVE_UP_DOWN					0x0251		//佣兵上下阵
#define		S_SLAVE_SKILL_LIST				0x0252		//请求佣兵技能列表
#define		S_SLAVE_REFRESH_SKILLS			0x0253		//设置
#define		S_SLAVE_PRO_UPDATE				0x0254		//佣兵属性更新通知,服务器推送
#define		S_SLAVE_TRAIN_PREVIEW			0x0255		//佣兵培养预览
#define		S_SLAVE_TRAIN_SAVE				0x0256		//培养保存
#define		S_SLAVE_STAR_PRO_PREVIEW		0x0257		//佣兵升星属性预览
#define		S_SLAVE_UPGRADE_STAR			0x0258		//立即升星
#define		S_REQUEST_COMBO_BOOKS_INFO		0x0259		//请求合成书本信息
#define		S_COMBO_BOOK					0x025A		//合成书本
#define		S_REQUEST_HALO_LIST				0x025B		//获取佣兵光环列表
#define		S_ACTIVE_HALO					0x025C		//激活佣兵光环技能
#define		S_SLAVE_EQUIP_ATTACH			0x025D		//佣兵穿戴装备
#define		S_SLAVE_BASR_ATTR   			0x025E		//培养显示
#define		S_GET_WORLDBOSS_STATUS			0x0270		//获得世界BOSS状态
#define		S_SIGNUP_WORLDBOSS				0x0271		//报名世界BOSS
#define		S_ATTACK_WORLDBOSS				0x0272		//攻击世界BOSS
#define		S_EMBRAVE_WORLDBOSS				0x0273		//鼓舞世界BOSS
#define		S_GET_WORLDBOSS_RANK			0x0274		//获得伤害排行
#define		S_GET_WORLDBOSS_COOLTIME		0x0275		//获得冷却剩余时间
#define		S_GET_ROLE_RANK_LEVEL			0x0301		//rank list by level
#define		S_GET_ROLE_RANK_FIGHTVALUE		0x0302		//rank list by fightvalue
#define		S_GET_ROLE_WEAPON				0x0303		//rank list by weapon
#define	    S_GET_ROLE_RANK_MATCH			0x0304		//rank list by match
#define		S_GUILD_LIST					0x0400		//获取当前推荐的公会信息列表
#define		S_GET_GUILD_RANK_LIST			0x0401		//获取当前的公会排行信息列表
#define		S_GET_GUILD_req					0x0402		//点击搜索工会 点击搜索,显示的已请求申请的工会列表
#define		S_GET_GUILD_find				0x0403		//点击ok输入搜索的公会请求
#define		S_GUILD_add						0x0404		//加入行会申请  对于已申请的就是撤销申请
#define		S_GUILD_create					0x0405		//创建行会
#define		S_GUILD_INFO					0x0406		//公会详情
#define		S_GUILD_create_content			0x0407		//公告编辑
#define		S_GUILD_INFO_MEMLIST			0x0408		//公会成员列表
#define		S_GUILD_INFO_MEM_EQUIP			0x0409		//查看成员装备
#define		S_GUILD_INFO_MEM_FRIEND			0x0410		//添加成员为好友
#define		S_GUILD_INFO_MEM_JOB			0x0411		//成员职位任命
#define		S_GUILD_INFO_MEM_QUIT			0x0412		//逐出行会
#define		S_GUILD_INFO_QUIT				0x0413		//退出行会
#define		S_GUILD_INFO_REQ_LIST			0x0414		//行会审核列表
#define		S_GUILD_INFO_REQ_YES			0x0415		//同意或者拒绝id加入行会
#define		S_GUILD_INFO_REQ_ALL			0x0416		//全部同意加入
#define		S_GUILD_INFO_SAY_LIST			0x0417		//公会留言列表
#define		S_GUILD_INFO_TEACH_LIST			0x0418		//公会科技
#define		S_GUILD_INFO_TEACH_UP_LIST		0x0419		//捐献装备(科技升级)
#define		S_GUILD_INFO_TEACH_UP_GOLD		0x0420		//捐献金币
#define	    S_GET_ARENA_RECOMMEND			0x0500		//recommend arena list
#define	    S_ARENA_FIGHT					0x0501		//area fight
#define	    S_ARENA_BUY_COUNT				0x0502		//buy count
#define	    S_GET_ARENA_RECORD				0x0503		//require arena record
#define	    S_ARENA_COMPETITORINFO			0x0504		//require arena competitor information
#define	    S_ARENA_FIGHT_PRE				0x0505		//area fight pre
#define		S_FIGHT_ARENA_REPORT			0x0506		//竞技场战斗报告
#define		S_ARENA_BUY_GOLD   				0x0507		//竞技场购买需要多少元宝
#define		S_GET_SIGN_INFO     			0x0600		//get sign times
#define		S_CLIENT_SIGN   				0x0601		//client sign
#define	    S_GET_ACTIVITY					0x0800		//get activities
#define	    S_JOIN_ACTIVITY					0x0801		//join activity
#define	    S_BOSS_FIGHT					0x0802		//Boss战斗
#define	    S_ENHANCE_BOSS_FIGHT			0x0803		//鼓舞
#define	    S_BOSS_TIME_LEFT				0x0804		//战斗开始倒计时
#define	    S_BOSS_GET_HELPER				0x0805		//补人
#define		S_BOSS_GET_HURT_RANK			0x0806		//伤害排行
#define	    S_GET_GOODS_LIST				0x0901		//get goods list
#define	    S_REQUEST_BUY					0x0902		//request buy 
#define	    S_REQUEST_BUY_ALL				0x0903		//request buy all
#define	    S_REQUEST_UPDATE_COST			0x0904		//request update fee
#define	    S_REQUEST_UPDATE				0x0905		//request update
#define	    S_REQUEST_VIPCOIN				0x0906		//request vipMoney
#define	    S_USE_MONEYTREE					0x0907		//
#define	    S_REQUEST_MONEYTREE				0x0908		//
#define	    S_REQUEST_MISTERY_SHOP			0x090A		//神秘商城
#define	    S_BUY_MISTERY_SHOP				0x090B		//神秘商城
#define	    S_GET_TASK_LIST					0x0910		//get task list
#define	    S_GET_TASK_GIFTS				0x0911		//get task reward 
#define		S_OFFLINE_FIGHT_REPORT			0x0920		//离线战斗报告
#define		S_QUICK_FIGHT_REPORT			0x0921		//离线和快速战斗报告
#define		S_BUY_QUICK_FIGHT 				0x0923		//确认购买快速战斗报告次数
#define		S_GET_QUICK_FIGHT_COST 			0x0924		//获取购买快速战斗报告需要金币
#define		S_GET_QUICK_FIGHT_TIMES			0x0925		//获取购买得快速战斗次数
#define	    S_GET_OPEN_REWARD_LIST			0x0930		//get open activity list
#define	    S_GET_OPEN_REWARD_GIFTS			0x0931		//get open activity reward 
#define	    S_GET_OPEN_ACTIVITY_INFO		0x0932		//get open activity info by ID
#define		S_REQUEST_BIND_EMAIL			0x0934		//请求充值绑定的邮箱
#define     S_MODIFY_BIND_EMAIL				0x0935		//绑定邮箱信息	
#define		S_EXCHANGE_AWARDS_BY_KEY		0x0936		//兑换码(CDKey)兑换奖励

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
