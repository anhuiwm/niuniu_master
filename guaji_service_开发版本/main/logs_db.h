#pragma once
#include "database.h"

class logs_db : public database
{
public:
	void init();
};


template<class T>	class double_queue
{
	std::mutex mutex;
	std::queue<T> msgs_1;
	std::queue<T> msgs_2;
	std::queue<T>* msgs_read;
	std::queue<T>* msgs_write;

public:
	double_queue():msgs_read(&msgs_1), msgs_write(&msgs_2)
	{
	}
	void push(T value)
	{
		mutex.lock();
		msgs_write->push(value);
		mutex.unlock();
	}
	bool empty()
	{
		return msgs_read->empty() && msgs_write->empty();
	}
	T pop()
	{
		if(!msgs_read->empty())
		{
			auto var = msgs_read->front();
			msgs_read->pop();
			return var;
		}
		else if(!msgs_write->empty())
		{
			mutex.lock();
			auto temp = msgs_read;
			msgs_read = msgs_write;
			msgs_write = temp;

			auto var = msgs_read->front();
			msgs_read->pop();
			mutex.unlock();
			return var;
		}
		else
		{
			throw new logic_error("empty double_queue, please check empty before pop.");
		}
	}
};


enum class purpose
{
	buy_arena_battle_count = 1,// 购买竞技场战斗次数
	arena_battle_report,// 竞技场战斗报告
	kill_monster,// 杀怪
	sell_item,// 卖出道具
	reward,// 奖励
	package_extend,// 背包扩展
	coin_card,// 金币卡
	ingot_card,// 元宝卡
	bless_oil,// 祝福油
	use_gem,// 使用宝石
	active_halo,// 激活光环
	synthesis_book,// 合成书本
	train_mercenary,// 训练佣兵
	auto_sell,// 自动销售
	gm,// GM直接操作
	offline_earning,// 离线赚钱
	buy_item,// 购买道具
	ingot_to_coin,// 元宝转金币
	opening_activity_reward,// 开服活动奖励
	task_reward,// 任务奖励
	refresh_skill,// 刷新技能
	buy_quick_fight,// 购买快速战斗
	quick_fight_report,// 快速战斗报告
	buy_elite_time,// 购买挑战时间
	charge,// 充值
	request_update,// 请求刷新珍宝商城
	meet_battle_report,// 偶遇战斗
	equip_forging,// 装备锻造
	equip_star_up,// 装备升星
	activity_buy_fund,// 购买基金
	guild_contribute,// 公会贡献
	guild_signin,// 公会签到
	guild_create,// 创建
	activity_get_fund,// 领取基金
	treasure, // 宝藏
	bodyguard_divination, // 镖局 占卜
	bodyguard_start, // 镖局 开始
	bodyguard_reward, // 镖局 领取奖励
	bodyguard_buy_loot_count, // 镖局 购买抢劫次数
	bodyguard_loot, // 镖局 抢劫
};


class logs_db_thread : public double_queue<string>
{
	logs_db db;
	volatile bool closed;
	std::thread thread;
private:
	static void s_thread_proc(void* arg);
public:
	//static logs_db_thread singleton;
	static logs_db_thread& singleton()
	{
		static logs_db_thread singleton;
		return singleton;
	}

	logs_db_thread();
	~logs_db_thread();

	void close()
	{
		closed = true;
	}

	void consume(const string& username, int roleid, int coin, int ingot, purpose purpose, int itemid);
	
	void thread_proc();
};