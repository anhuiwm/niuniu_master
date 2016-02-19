#pragma once
#include <thread>
#include "database.h"
#include <queue>
#include <mutex>

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
			auto var = msgs_read->back();
			msgs_read->pop();
			return var;
		}
		else if(!msgs_write->empty())
		{
			mutex.lock();
			auto temp = msgs_read;
			msgs_read = msgs_write;
			msgs_write = temp;

			auto var = msgs_read->back();
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
	buy_arena_battle_count = 1,// ���򾺼���ս������
	arena_battle_report,// ������ս������
	kill_monster,// ɱ��
	sell_item,// ��������
	reward,// ����
	package_extend,// ������չ
	coin_card,// ��ҿ�
	ingot_card,// Ԫ����
	bless_oil,// ף����
	use_gem,// ʹ�ñ�ʯ
	active_halo,// ����⻷
	synthesis_book,// �ϳ��鱾
	train_mercenary,// ѵ��Ӷ��
	auto_sell,// �Զ�����
	gm,// GMֱ�Ӳ���
	offline_earning,// ����׬Ǯ
	buy_item,// �������
	ingot_to_coin,// Ԫ��ת���
	opening_activity_reward,// ���������
	task_reward,// ������
	refresh_skill,// ˢ�¼���
	buy_quick_fight,// �������ս��
	quick_fight_report,// ����ս������
	buy_elite_time,// ������սʱ��
	charge,// ��ֵ
	request_update,// ����ˢ���䱦�̳�
	meet_battle_report,// ż��ս��
};


class logs_db_thread : public double_queue<string>
{
	logs_db db;
	std::thread thread;
	volatile bool closed;

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

	void consume(unsigned serverid, const string& username, int roleid, int coin, int ingot, purpose purpose, int itemid);
	
	void thread_proc();
};