#include "pch.h"
#include "activity.h"
#include "jsonpacket.h"
#include "skill.h"
#include "roleCtrl.h"
#include "mailCtrl.h"
#include "itemCtrl.h"
#include "db.h"
#include "service.h"
#include "logs_db.h"
#include "TimerMgr.h"
#include "broadCast.h"
#include "arena.h"

USE_EXTERN_CONFIG;


#define THROW_EXCEPTION(x) {cout << x << endl;\
	exit(9); }

uint activity_top_upgrade::end_time = 0;
uint activity_taobao::end_time = 0;
unsigned activity_top_upgrade::min_rank_level = 0;
list<pair<unsigned short, pair<unsigned, string>>> activity_top_upgrade::rank;
unsigned treasure::min_value = 0;
bool treasure::check = true;
list<treasure::record_ptr> treasure::rank;
map<uint, treasure::info> treasure::records;


void activity_top_upgrade::load()
{
	SQLSelect sqlx( DBCtrl::getSQL(), "select level, roleid, rolename from activity_top_upgrade order by level desc");

	rank.clear();
	while(MYSQL_ROW row = sqlx.getRow())
	{
		rank.push_back(make_pair((unsigned short)atoi(row[0]), make_pair(unsigned(atoi(row[1])), row[2])));
	}
	if(rank.size() >= 20)
		min_rank_level = rank.back().first;

	auto cfg = CONFIG::getOpenActivityCfg(12);
	if (cfg == nullptr)
	{
		logFATAL("CONFIG::getOpenActivityCfg return nullptr %d", 12);
		return;
	}
	end_time = service::open_time + 3600 * 24 * cfg->end_time;
}


void activity_top_upgrade::save()
{
	execSQL(DBCtrl::getSQL(), "truncate table activity_top_upgrade");

	for(auto & item : rank)
	{
		execSQL(DBCtrl::getSQL(), "insert into activity_top_upgrade(level, roleid, rolename) values(%u, %u, '%s')",
			item.first, item.second.first, item.second.second.c_str());
	}
}


void activity_top_upgrade::update(ROLE* role)
{
	if ((uint)time(nullptr) > end_time)
		return;

	if(role->wLevel < min_rank_level)
		return;

	for(auto iter = rank.begin(); iter != rank.end();iter++)
	{
		if(iter->second.first == role->dwRoleID)
		{
			iter = rank.erase(iter);
			break;
		}
	}

	//rank.sort([](const pair<unsigned short, pair<unsigned, string>>& left, const pair<unsigned short, pair<unsigned, string>>& right){
	//	return left.
	//});
	for(auto iter = rank.begin(); iter != rank.end();++iter)
	{
		if(role->wLevel > iter->first)
		{
			rank.insert(iter, make_pair(role->wLevel, make_pair(role->dwRoleID, role->roleName)));

			if(rank.size() > 20)
			{
				rank.pop_back();
				min_rank_level = rank.back().first;
			}
			save();
			return;
		}
	}

	if(rank.size() < 20)
	{
		rank.push_back(make_pair(role->wLevel, make_pair(role->dwRoleID, role->roleName)));
	}

	save();
}

void activity_top_upgrade::send_data(ROLE* role, byte* pkt, size_t len)
{
	output_stream out(S_GET_LEVEL_PRISE_DATA);

	out.write_byte(0);
	auto now = (uint)time(nullptr);
	if (end_time < now)
		out.write_uint(0);
	else
		out.write_uint(end_time - now);

	out.write_uint(0);
	for(auto& item : rank)
	{
		out.write_ushort(item.first);
		out.write_uint(item.second.first);
		out.write_string(item.second.second);
	}
	role->send(out);
}

void activity_top_upgrade::send_award()
{
	logonline("7day_send_award::%u", time(0));
	uint role_id = 0;
	uint rank_num = 0;
	uint money_num = 0;
	uint16 item_id = 0;
	string item_name1;
	uint16 item_num1 = 0;
	string item_name2;
	uint16 item_num2 = 0;
	//ROLE *role = NULL;
	CONFIG::SEND_7DAY_AWARD_CFG * send_award_cfg = NULL;
	vector<ITEM_INFO> vecItemInfo;
	for (auto it : activity_top_upgrade::rank )
	{
		vecItemInfo.clear();
		rank_num++;
		role_id = it.second.first;
		send_award_cfg = CONFIG::getSend7DayAwardCfg( rank_num );
		COND_CONTINUE(send_award_cfg == NULL);

		if (send_award_cfg->goldNum > 0)
		{
			money_num = send_award_cfg->goldNum;
			vecItemInfo.push_back(ITEM_INFO(send_award_cfg->goldID, money_num));
		}

		if (send_award_cfg->itemNum1 > 0)
		{
			CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg( send_award_cfg->itemID1);
			if (itemCfg != NULL)
			{
				item_name1 = itemCfg->name;
				item_id = itemCfg->id;
				item_num1 = send_award_cfg->itemNum1;
				vecItemInfo.push_back(ITEM_INFO(item_id, item_num1));
			}
		}

		if (send_award_cfg->itemNum2 > 0)
		{
			CONFIG::ITEM_CFG* itemCfg = CONFIG::getItemCfg(send_award_cfg->itemID2);
			if (itemCfg != NULL)
			{
				item_name2 = itemCfg->name;
				item_id = itemCfg->id;
				item_num2 = send_award_cfg->itemNum2;
				vecItemInfo.push_back(ITEM_INFO(item_id, item_num2));
			}
		}

		if (!vecItemInfo.empty())
		{
			char szMsg[512];
			sprintf(szMsg, (CONFIG::getMsgSend7DayAward()).c_str(), rank_num, money_num, item_name1.c_str(), item_num1, item_name2.c_str(), item_num2);
			//logonline("::%s", szMsg);
			MailCtrl::sendMail( role_id, E_MAIL_TYPE_PRIZE, szMsg, vecItemInfo);
		}
	}
}

bool activity_fund::get_user_deliver(unsigned roleid, int& deliver)
{
	SQLSelect sqlx(DBCtrl::getSQL(), "select got from activity_fund where roleid=%u", roleid);

	if (MYSQL_ROW row = sqlx.getRow())
	{
		deliver = atoi(row[0]);
		return true;
	}
	return false;
}

void activity_fund::add_user_deliver(unsigned roleid, e_status deliver)
{
	if (!execSQL(DBCtrl::getSQL(), "insert into activity_fund(roleid, got) value(%u, %u) \
				on duplicate key update got = got | %u", roleid, deliver, deliver))
	{
		logFATAL("add_user_deliver error %s", mysql_error(DBCtrl::getSQL()));
	}
}

void activity_fund::buy(ROLE* role)
{
	enum result : byte
	{
		successful,
		viplevel_error,
		money_not_enough,
	};

	int status = 0;
	if (activity_fund::get_user_deliver(role->dwRoleID, status))
	{
		return;
	}

	output_stream out(S_REQUEST_TO_BUY_FUND);
	if (role->wVipLevel < 2)
	{
		out.write_byte(result::viplevel_error);
		role->send(out);
		return;
	}

	if (role->dwIngot < 1000)
	{
		out.write_byte(result::money_not_enough);
		role->send(out);
		return;
	}

	add_user_deliver(role->dwRoleID, e_status::none);
	ADDUP(role->dwIngot, -1000);
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -1000, purpose::activity_buy_fund, 0);
	out.write_byte(result::successful);
	role->send(out);
}

void activity_fund::get(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,
		has_got,
		level_error,
	};

	byte level;
	if (!stream.read_byte(level))return;

	output_stream out(S_REQUEST_FUND_GETFUND);
	if (role->wLevel < level)
	{
		out.write_byte(result::level_error);
		role->send(out);
		return;
	}
	
	int left_move = 0;
	uint gold = 0;
	switch (level)
	{
	case 10:
		left_move = 0; gold = 500; break;
	case 20:
		left_move = 1; gold = 1000; break;
	case 30:
		left_move = 2; gold = 1500; break;
	case 40:
		left_move = 3; gold = 2000; break;
	case 50:
		left_move = 4; gold = 2500; break;
	case 55:
		left_move = 5; gold = 2500; break;
		break;
	default:
		out.write_byte(result::level_error);
		role->send(out);
		return;
	}

	e_status deliver = e_status(1 << left_move);
	int old_deliver = 0;
	if (!get_user_deliver(role->dwRoleID, old_deliver))
		return;
	if (old_deliver & (int)deliver)
	{
		out.write_byte(result::has_got);
		role->send(out);
		return;
	}

	add_user_deliver(role->dwRoleID, deliver);

	ADDUP(role->dwIngot, gold);
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, gold, purpose::activity_get_fund, 0);
	notifyIngot(role);

	out.write_byte(result::successful);
	out.write_byte(level);
	role->send(out);
}


void activity_taobao::load()
{
	auto cfg = CONFIG::getOpenActivityCfg(11);
	if (cfg == nullptr)
	{
		logFATAL("CONFIG::getOpenActivityCfg return nullptr %d", 11);
		return;
	}
	end_time = service::open_time + 3600 * 24 * cfg->end_time;
}


void activity_taobao::charge(unsigned roleid, unsigned count, bool force)
{
	if (!force && (uint)time(nullptr) > end_time)
		return;

	unsigned old_count = 0, old_remain = 0, reward = 0, used_count = 0;
	get_count(roleid, old_count, old_remain, reward, used_count);

	count = count + old_remain;// 加上上次余量
	auto new_count = count / 100;
	auto new_remain = count % 100;

	set_info(roleid, new_count + old_count, new_remain, reward, used_count);
}


void activity_taobao::set_info(unsigned roleid, unsigned count, unsigned remain, unsigned reward, unsigned used_count)
{
	if (!execSQL(DBCtrl::getSQL(), "insert into activity_taobao(roleid, count, remain, reward, used_count) value(%u, %u, %u, %u, %u) \
on duplicate key update count=%u, remain=%u, reward=%u, used_count=%u",
roleid, count, remain, reward, used_count, count, remain, reward, used_count))
	{
		logFATAL("activity_taobao::add_count error %s", mysql_error(DBCtrl::getSQL()));
	}
}


bool activity_taobao::get_count(unsigned roleid, unsigned& count, unsigned& remain, unsigned& reward, unsigned& used_count)
{
	SQLSelect sqlx(DBCtrl::getSQL(), "select count, remain, reward, used_count from activity_taobao where roleid=%u", roleid);

	if (MYSQL_ROW row = sqlx.getRow())
	{
		count = atoi(row[0]);
		remain = atoi(row[1]);
		reward = atoi(row[2]);
		used_count = atoi(row[3]);
		return true;
	}
	return false;
}

void activity_taobao::tao(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,
		not_enough,// 次数不足
	};

	uint16_t count;
	if (!stream.read_ushort(count))return;

	output_stream out(S_REQUEST_TAO_DRAW);

	unsigned old_count = 0, old_remain = 0, reward = 0, used_count = 0;
	get_count(role->dwRoleID, old_count, old_remain, reward, used_count);

	if (old_count < count)
	{
		out.write_byte(result::not_enough);
		out.write_ushort(count);
		role->send(out);
		return;
	}

	out.write_byte(result::successful);
	out.write_ushort(count);
	struct{
		byte id;
		unsigned item;
		unsigned count;
		float g;
	} data[] = { { 1, 402, 1, 0.23f },
	{ 2, 402, 5, 0.09f },
	{ 3, 402, 10, 0.03f },
	{ 4, 706, 1, 0.18f },
	{ 5, 706, 2, 0.1f },
	{ 6, 706, 5, 0.02f },
	{ 7, 20002, 100000, 0.23f },
	{ 8, 20002, 500000, 0.12f },
	};

	for (uint16_t i = 0; i < count; i++)
	{
		auto r = BASE::randBetween(0.0f, 1.0f);
		float cur = 0;
		for (auto& item : data)
		{
			cur += item.g;
			if (r < cur)
			{
				role->rewards_for_activity_tao.push_back(make_pair(item.item, item.count));
				out.write_byte(item.id);
				break;
			}
		}
	}

	role->send(out);

	set_info(role->dwRoleID, old_count - count, old_remain, reward, used_count + count);
}

void activity_taobao::get_result(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,
		not_result,// 没有结果
	};

	output_stream out(S_REQUEST_TAO_RESULT);
	if (role->rewards_for_activity_tao.empty())
	{
		out.write_byte(result::not_result);
		role->send(out);
		return;
	}
	out.write_byte(result::successful);
	role->send(out);
	for (auto& item : role->rewards_for_activity_tao)
	{
		role->reward_item(item.first, item.second);
	}
	role->rewards_for_activity_tao.clear();
}


void activity_taobao::get_reward(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,
		has_got,
		level_error,
	};

	struct{
		unsigned item;
		unsigned count;
	}  items[] = {
		{ 20002, 5000000 },//  铜钱
		{ 402, 50 }, // 黑铁矿
		{ 414, 1 }, // 5级红宝石
		{ 403, 50 }, // 套装碎片
		{ 402, 500 }, // 强化石
	};

	unsigned level;
	if (!stream.read_uint(level))return;

	unsigned old_count = 0, old_remain = 0, reward = 0, used_count = 0;
	if (!get_count(role->dwRoleID, old_count, old_remain, reward, used_count))
		return;

	output_stream out(S_REQUEST_TAO_ITEM);
	if (used_count < level)
	{
		out.write_byte(result::level_error);
		role->send(out);
		return;
	}

	int left_move = 0;
	switch (level)
	{
	case 50:
		left_move = 0;
		break;
	case 100:
		left_move = 1;
		break;
	case 200:
		left_move = 2;
		break;
	case 500:
		left_move = 3;
		break;
	case 1000:
		left_move = 4;
		break;
	default:
		out.write_byte(result::level_error);
		role->send(out);
		return;
	}

	unsigned status = (1 << left_move);
	if (status & reward)
	{
		out.write_byte(result::has_got);
		out.write_uint(reward);
		role->send(out);
		return;
	}
	role->reward_item(items[left_move].item, items[left_move].count);

	set_info(role->dwRoleID, old_count, old_remain, status | reward, used_count);
	out.write_byte(result::successful);
	reward = status | reward;
	out.write_uint(reward);
	role->send(out);
}


void activity_taobao::get_info(ROLE* role, input_stream& stream)
{
	unsigned old_count = 0, old_remain = 0, reward = 0, used_count = 0;
	get_count(role->dwRoleID, old_count, old_remain, reward, used_count);
	output_stream out(S_REQUEST_TAO_LEFTTIME);
	auto now = (uint)time(0);
	if (end_time < now)
		out.write_uint(0);
	else
	{
		auto remain_time = end_time - now;
		out.write_uint(remain_time);
	}
	out.write_uint(old_count);
	out.write_uint(reward);
	out.write_uint(used_count);
	role->send(out);
}




void treasure::load()
{
	SQLSelect sqlx(DBCtrl::getSQL(), "select roleid, count, hurt, coin, rolename, job, sex, item, quality from rank_treasure order by hurt desc");

	uint r = 1;
	while (MYSQL_ROW row = sqlx.getRow())
	{
		auto temp = new record();
		temp->roleid = (uint)atoi(row[0]);
		temp->count = (uint)atoi(row[1]);
		temp->hurt = (uint)atoi(row[2]);
		temp->coin = (uint)atoi(row[3]);
		temp->name = row[4];
		temp->job = (byte)atoi(row[5]);
		temp->sex = (byte)atoi(row[6]);
		temp->item = (uint16)atoi(row[7]);
		temp->quality = (byte)atoi(row[8]);
		rank.push_back(record_ptr(temp));

		auto& info = records[temp->roleid];
		info.rank = r++;
		info.history.push_back(make_pair(temp->count, temp->hurt));
	}
	if (rank.size() >= max_rank_count)
		min_value = rank.back()->hurt;

	auto now = time(0) - service::tzone;
	auto escape = now % (24 * 3600);// 今天过去的时间

	if (escape > 20 * 3600 + 600)
	{
		TimerMgr::getMe().AddOnceTimer(time(nullptr) + 24 * 3600 - (escape - (20 * 3600 + 600)), on_end_timer);
		//service::set_timer(24 * 3600 - (escape - (20 * 3600 + 600)), on_end_timer);
	}
	else
	{
		TimerMgr::getMe().AddOnceTimer(time(nullptr) + (20 * 3600 + 600) - escape, on_end_timer);
		//service::set_timer((20 * 3600 + 600) - escape, on_end_timer);
	}
}


void treasure::treasure_done(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,// 成功
		close,// 活动关闭
		error,// 参数范围违法错误
		not_enough_count,// 没有足够游戏次数
	};

	auto now = time(0) - service::tzone;
	auto escape = now % (24 * 3600);// 今天过去的时间

	output_stream out(S_REQUEST_BAO_RESULT);

	if (check && (escape < 16 * 3600 || escape > 20 * 3600 + 600))// 延后10分钟关闭活动
	{
		out.write_byte(result::close);
		role->send(out);
		return;
	}

	uint32 count;
	uint32 hurt;
	uint32 coin;
	if (!stream.read_uint(count) || !stream.read_uint(hurt) || !stream.read_uint(coin))
	{
		logFATAL("treasure protocal receive error.");
		return;
	}

	auto iter = records.find(role->dwRoleID);
	if (iter != records.end())
	{
		if (iter->second.history.size() >= 3)
		{
			out.write_byte(result::not_enough_count);
			role->send(out);
			return;
		}
	}

	ITEM_CACHE* best = role->get_best_equip();

	// 校验数值
	if (count > 700 || coin > 150000 || (best && hurt > best->dwFightValue * 1000))
	{
		out.write_byte(result::error);
		role->send(out);
		return;
	}

	logINFO("treasure done(roleid:%u). click:%u, hurt:%u, coin:%u", role->dwRoleID, count, hurt, coin);
	out.write_byte(result::successful);

	ADDUP(role->dwCoin, coin);
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, coin, 0, purpose::treasure, 0);

	auto temp = new record();
	temp->roleid = role->dwRoleID;
	temp->count = count;
	temp->hurt = hurt;
	temp->coin = coin;
	temp->name = role->roleName;
	temp->job = role->byJob;
	temp->sex = role->bySex;
	temp->item = best->itemCfg->id;
	temp->quality = best->byQuality;
	auto temp_ptr = record_ptr(temp);

	int rank = treasure_ranking(temp_ptr);
	if (rank > 0)
		update(temp_ptr);

	auto& info = records[role->dwRoleID];
	info.history.push_back(make_pair(count, hurt));
	info.rank = rank;

	if (rank < 5)
		out.write_byte(0);
	else if (rank < 10)
		out.write_byte(8);
	else if (rank < 20)
		out.write_byte(7);
	else if (rank < 30)
		out.write_byte(6);
	else if (rank < 50)
		out.write_byte(5);
	else if (rank < 100)
		out.write_byte(4);
	else if (rank < 150)
		out.write_byte(3);
	else if (rank < 200)
		out.write_byte(2);
	else
		out.write_byte(1);
	role->send(out);
}


// 获取宝藏记录
void treasure::get_record(ROLE* role, input_stream& stream)
{
	output_stream out(S_REQUEST_BAO_INFO);
	auto iter = records.find(role->dwRoleID);
	if (iter != records.end())
	{
		for (auto item : iter->second.history)
		{
			out.write_uint(item.first);
			out.write_uint(item.second);
		}
		for (size_t i = 0; i < 3 - iter->second.history.size(); i++)
		{
			out.write_uint(0);
			out.write_uint(0);
		}
	}
	else
	{
		out.write_uint(0);
		out.write_uint(0);
		out.write_uint(0);
		out.write_uint(0);
		out.write_uint(0);
		out.write_uint(0);
	}
	role->send(out);
}

// 获取宝藏排行
void treasure::get_rank(ROLE* role, input_stream& stream)
{
	output_stream out(S_REQUEST_BAO_RANK);
	// 排名
	auto iter = records.find(role->dwRoleID);
	if (iter != records.end())
		out.write_byte((byte)(iter->second.rank));
	else
		out.write_byte(0);

	byte i = 0;
	for (auto& item : rank)
	{
		if (++i == 20)break;
		out.write_uint(item->roleid);
		out.write_byte(item->job);
		out.write_byte(item->sex);
		out.write_string(item->name);
		out.write_uint(item->count);
		out.write_uint(item->hurt);
		out.write_ushort(item->item);
		out.write_byte(item->quality);
	}
	role->send(out);
}


void treasure::cost(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,// 成功
		not_enough,// 不足
	};

	uint16 cost;
	if (!stream.read_ushort(cost))
		return;

	output_stream out(S_REQUEST_BAO_COST);
	if (role->dwIngot < cost)
	{
		out.write_byte(result::not_enough);
		role->send(out);
		return;
	}

	ADDUP(role->dwIngot, -(int)cost);
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)cost, purpose::treasure, 0);

	out.write_byte(result::successful);
	role->send(out);
}

// 处理排名
uint treasure::treasure_ranking(record_ptr record)
{
	if (record->hurt < min_value)
		return 0;

	int rolerank = 0;
	for (auto iter = rank.begin(); iter != rank.end(); iter++)
	{
		rolerank++;
		if (iter->get()->roleid == record->roleid)
		{
			if (record->hurt > iter->get()->hurt)
				iter = rank.erase(iter);
			else
				return rolerank;
			break;
		}
	}

	rolerank = 0;
	for (auto iter = rank.begin(); iter != rank.end(); ++iter)
	{
		rolerank++;
		if (record->hurt > iter->get()->hurt)
		{
			rank.insert(iter, record);// 插入新纪录，影响records中其他玩家排名

			if (rank.size() > max_rank_count)
			{
				// 重置最后一名玩家排名
				auto& last = rank.back();
				auto r_iter = records.find(last->roleid);
				if (r_iter != records.end())
					r_iter->second.rank = 0;

				rank.pop_back();
				min_value = rank.back()->hurt;
			}

			auto newrank = rolerank;
			for (; iter != rank.end();++iter)// 之后的玩家赋予新排行
			{
				auto r_iter = records.find(iter->get()->roleid);
				if (r_iter != records.end())
					r_iter->second.rank = ++newrank;
			}
			return rolerank;
		}
	}

	if (rank.size() < max_rank_count)
	{
		rank.push_back(record);
		return (int)rank.size();
	}
	return 0;
}


byte treasure::get_remain(ROLE* role)
{
	return (byte)(3 - records[role->dwRoleID].history.size());
}


void treasure::update(record_ptr record)
{
	MYSQL_ESCAPE_N(rolename, record->name.c_str(), min<size_t>(record->name.size(), ROLE_NAME_LEN), ROLE_NAME_LEN);
	if (!execSQL(DBCtrl::getSQL(), "insert into rank_treasure(roleid, rolename, count, hurt, coin, item, quality, job, sex) value(%u, '%s', %u, %u, %u, %u, %u, %u, %u) \
		on duplicate key update rolename='%s', count=%u, hurt=%u, coin=%u, item=%u, quality=%u, job=%u, sex=%u",
		record->roleid, rolename, record->count, record->hurt, record->coin, record->item, record->quality, record->job, record->sex
		, rolename, record->count, record->hurt, record->coin, record->item, record->quality, record->job, record->sex))
	{
		logFATAL("add_user_deliver error %s", mysql_error(DBCtrl::getSQL()));
	}
}

void treasure::dump_rank(ROLE* role)
{
	int i = 0;
	{
		output_stream out(S_SYSTEM_BROADCAST);
		out.write_byte(e_broadcast_type::text);
		out.write_string("all player:");
		role->send(out);
	}
	for (auto& item : records)
	{
		i++;
		output_stream out(S_SYSTEM_BROADCAST);
		out.write_byte(e_broadcast_type::text);
		string text = to_string(item.second.rank) + " " + to_string(item.first) + " ";
		for (auto& his : item.second.history)
		{
			text += "[" + to_string(his.first) + "," + to_string(his.second) + "]";
		}
		out.write_string(text);
		role->send(out);
	}

	i = 0;
	{
		output_stream out(S_SYSTEM_BROADCAST);
		out.write_byte(e_broadcast_type::text);
		out.write_string("rank:");
		role->send(out);
	}
	for (auto& item : rank)
	{
		i++;
		output_stream out(S_SYSTEM_BROADCAST);
		out.write_byte(e_broadcast_type::text);
		out.write_string(to_string(i) + " " + to_string(item->roleid) + " " +
		"[" + to_string(item->count) + "," + to_string(item->hurt) + "," + to_string(item->coin) + "]");
		cout << endl;
		role->send(out);
	}
}


void treasure::on_end_timer()
{
	logonline("send treasure time:%d", time(0));
	// send mail
	for (auto& item : records)
	{
		char szMsg[512];
		auto rank = item.second.rank > 0 ? item.second.rank : (201 + rand() % 100);
		uint reward = 0;
		if (rank <= 1)
			reward = 300;
		else if (rank <= 2)
			reward = 250;
		else if (rank <= 3)
			reward = 200;
		else if (rank <= 10)
			reward = 150;
		else if (rank <= 30)
			reward = 100;
		else if (rank <= 50)
			reward = 50;
		else if (rank <= 100)
			reward = 30;
		else if (rank <= 200)
			reward = 20;
		else
			reward = 10;
		sprintf(szMsg, (CONFIG::get_treasure()).c_str(), rank, reward);

		vector<ITEM_INFO> items;
		items.push_back(ITEM_INFO(ITEM_ID_COIN, reward * 10000));
		MailCtrl::sendMail(item.first, E_MAIL_TYPE_PRIZE, szMsg, items);
	}

	if (!rank.empty())
	{
		auto& first = rank.front();
		output_stream out(S_SYSTEM_BROADCAST);
		out.write_byte(e_broadcast_type::treasure_first);
		out.write_uint(first->roleid);
		out.write_string(first->name);
		out.write_uint(first->count);
		out.write_uint(first->hurt);
		service::broadcast(out);
	}

	// clear
	rank.clear();
	records.clear();
	execSQL(DBCtrl::getSQL(), "truncate table rank_treasure");

	// 设置下一次定时器
	auto now = time(0) - service::tzone;
	auto escape = now % (24 * 3600);// 今天过去的时间

	if (escape > 20 * 3600 + 600)
	{
		service::set_timer(24 * 3600 - (escape - (20 * 3600 + 600)), on_end_timer);
	}
	else
	{
		service::set_timer((20 * 3600 + 600) - escape, on_end_timer);
	}
}


// =========================镖局==========================


time_t bodyguard::round_time = 2;// 一次运镖时间(单位：小时)
uint bodyguard::hidden_price = 5;// 在列表隐藏的价格
uint bodyguard::item_plus_price = 10;// 道具提升价格
uint bodyguard::divination_price = 10;// 占卜价格
uint bodyguard::list_count = 5;// 列表数量
float bodyguard::loot_item_probability = 0.25;// 抢劫道具概率
uint bodyguard::double_start_time = 12, bodyguard::double_end_time = 19;// 双倍奖励开始、结束时间(单位：小时)
vector<bodyguard::reward_config> bodyguard::reward_configs;// 奖励配置
vector<bodyguard::divination_config> bodyguard::divination_configs;// 占卜配置
vector<bodyguard::vip_loot_count_config> bodyguard::vip_loot_count_configs;// VIP奖励抢劫次数配置
map<uint, bodyguard::info> bodyguard::records;// 记录


bodyguard::e_state bodyguard::info::state()
{
	auto now = time(0) - service::tzone;
	auto today_escape_time = now % (24 * 3600);// 今天过去的时间
	auto today_begin_time = now - today_escape_time;// 今天开始的时间

	// 状态
	e_state state;
	if (start_time == 0)
		state = e_state::not_start;
	else if (today_begin_time > start_time)
		state = e_state::history_done;
	else if (now - start_time > round_time * 3 * 3600)// 开始超过总运镖时间
		state = e_state::today_done;
	else if (now - start_time < round_time * 1 * 3600)
		state = e_state::inprogress_1;
	else if (now - start_time < round_time * 2 * 3600)
		state = e_state::inprogress_2;
	else if (now - start_time < round_time * 3 * 3600)
		state = e_state::inprogress_3;

	return state;
}


bool bodyguard::info::add_looter(uint index, uint looterid, uint looted_itemid)
{
	if (index >= guards.size())
		return false;

	if (guards[index].looters[0] == 0)
	{
		guards[index].looters[0] = looterid;
		history.push_back(history_item((byte)index + 1, looterid, looted_itemid));
		if (history.size() > 10)
			history.pop_front();
		return true;
	}
	if (guards[index].looters[1] == 0)
	{
		guards[index].looters[1] = looterid;
		history.push_back(history_item((byte)index + 1, looterid, looted_itemid));
		if (history.size() > 10)
			history.pop_front();
		return false;
	}
	return false;
}


void bodyguard::info::reset()
{
	coin = 0;
	exp = 0;
	hidden = 0;
	item_plus = 0;
	divination = -1;
	data_changed = true;
	start_time = 0;
	guards = {};
}


// 开始保镖
int bodyguard::rpc_start(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,
		started,// 已经开始
		not_enough_count,// 次数不足
		not_enough_money,// 元宝不足
	};

	output_stream out(S_BODYGUARD_START);

	auto now = time(0) - service::tzone;

	auto iter = records.find(role->dwRoleID);
	if (iter != records.end() && iter->second.start_time)
	{
		auto state = iter->second.state();
		if (state != e_state::not_start)
		{
			out.write_byte(result::started);// 已经开始
		}
		role->send(out);
		return 0;
	}

	byte hidden;// 在列表隐藏
	byte item_plus;// 道具提升
	if (!stream.read_byte(hidden) || !stream.read_byte(item_plus))
		return 0;

	auto need_ingot = 0u;// 需要的元宝数
	if (hidden > 0)
		need_ingot += hidden_price;
	if (item_plus > 0)
		need_ingot += item_plus_price;

	if (role->dwIngot < need_ingot)
	{
		out.write_byte(result::not_enough_money);
		role->send(out);
		return 0;
	}

	logINFO("bodyguard start roleid(%u) hidden(%u) item_plus(%u).", role->dwRoleID, hidden, item_plus);

	if (need_ingot > 0)
	{
		ADDUP(role->dwIngot, -(int)need_ingot);
		notifyIngot(role);
		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)need_ingot, purpose::bodyguard_start, 0);
	}

	auto& info = records[role->dwRoleID];
	info.start_time = now;
	info.coin = role->wLevel * 3000 + 40000;
	info.exp = uint(500.f * pow(role->wLevel, 0.72f) / 100.f) * 100;
	info.hidden = hidden > 0;
	info.item_plus = item_plus > 0;
	if (info.divination == -1)
		info.divination = BASE::randTo(divination_configs.size());
	info.data_changed = true;

	divination_config* divination = nullptr;
	if((uint)info.divination < divination_configs.size())
		divination = &divination_configs[info.divination];


	auto proba = 0.5f;
	if (divination)
	{
		info.coin = uint(info.coin * divination->coin_up);
		info.exp = uint(info.exp * divination->exp_up);
	}
	for (auto& item : info.guards)// 遍历每次保镖，初始化奖励道具
	{
		if (divination)
			proba *= divination->item_up;// 计算占卜后的道具获取几率

		if (item_plus > 0 || BASE::randBetween(0.f, 1.f) < proba)// 幸运的取到了道具,花钱了必得
		{
			auto rd = BASE::randBetween(0.f, 1.f);
			float cur = 0.f;
			for (auto& ritem : reward_configs)
			{
				cur += item_plus ? ritem.probability_plus : ritem.probability;// 如果消费了元宝，则提升道具池的数量
				if (rd < cur)
				{
					item.reward_item_id = ritem.item_id;
					break;
				}
			}
		}
	}
	out.write_byte(result::successful);
	out.write_uint(info.coin);
	out.write_uint(info.exp);
	out.write_uint(info.guards[0].reward_item_id);
	out.write_uint(round_time * 3600);
	role->send(out);

	return 0;
}

// 抢劫
bodyguard::pre_loot_info bodyguard::get_state(uint roleid, uint targetid)
{
	auto state = records[roleid].state();
	auto& tinfo = records[targetid];
	if (state < bodyguard::e_state::inprogress_1 || state > bodyguard::e_state::inprogress_3)
	{
		return pre_loot_info::not_running;
	}
	if ((state == bodyguard::e_state::inprogress_1 && tinfo.guards[0].looted_count() == 2) ||
		(state == bodyguard::e_state::inprogress_2 && tinfo.guards[1].looted_count() == 2) ||
		(state == bodyguard::e_state::inprogress_3 && tinfo.guards[2].looted_count() == 2))
	{
		return pre_loot_info::target_can_not_loot;
	}
	return pre_loot_info::ok;
}


// 获取运镖奖励
int bodyguard::been_loot(ROLE* role, ROLE* looter, const byte result)
{
	if (result != E_MATCH_STATUS_LOSS && result != E_MATCH_STATUS_WIN)
		return 0;

	vector<DROP_SEQ> drop_items;
	if (result == E_MATCH_STATUS_LOSS)// 输了，减次数
	{
		if (looter->bodyguard_loot_count > 0)looter->bodyguard_loot_count--;
		looter->send_battle_report(E_FIGHT_BODYGUARD, result, 0, 0, 0, 0, drop_items);
		return 0;
	}

	auto& info = records[role->dwRoleID];
	auto state = info.state();
	if (state < e_state::inprogress_1 || state > e_state::inprogress_3)
	{
		looter->send_battle_report(E_FIGHT_BODYGUARD, result, 0, 0, 0, 0, drop_items);
		return 0;
	}

	if (state == e_state::inprogress_1 && !info.guards[0].can_loot())
	{
		looter->send_battle_report(E_FIGHT_BODYGUARD, result, 0, 0, 0, 0, drop_items);
		return 0;
	}
	else if (state == e_state::inprogress_2 && !info.guards[1].can_loot())
	{
		looter->send_battle_report(E_FIGHT_BODYGUARD, result, 0, 0, 0, 0, drop_items);
		return 0;
	}
	else if (state == e_state::inprogress_3 && !info.guards[2].can_loot())
	{
		looter->send_battle_report(E_FIGHT_BODYGUARD, result, 0, 0, 0, 0, drop_items);
		return 0;
	}

	auto now = time(0) - service::tzone;
	auto today_escape = now % (24 * 3600);// 今天过去的时间
	auto scale = (today_escape > (time_t)(double_start_time * 3600) && today_escape < (time_t)(double_end_time * 3600))?2u:1u;// 是否双倍
	auto coin = uint(info.coin * 0.1f * scale);
	uint rewarded_coin = coin;
	ADDUP(looter->dwCoin, coin);
	notifyCoin(looter);
	logs_db_thread::singleton().consume(looter->roleName, looter->dwCoin, coin, 0, purpose::bodyguard_loot, 0);
	auto rd = BASE::randTo(1.0f);
	if (rd < loot_item_probability)
	{
		if (state == e_state::inprogress_1)
		{
			auto& item = info.guards[0];
			if (!item.looted_item)
			{
				item.looted_item = true;
				looter->reward_item(drop_items, item.reward_item_id, item.reward_item_count * scale);

				output_stream out(S_SYSTEM_BROADCAST);
				out.write_byte(e_broadcast_type::bodyguard_loot_successful);
				out.write_uint(looter->dwRoleID);
				out.write_string(looter->roleName);
				out.write_uint(role->dwRoleID);
				out.write_string(role->roleName);
				out.write_uint(item.reward_item_id);
				service::broadcast(out);
			}
		}
		else if (state == e_state::inprogress_2)
		{
			auto& item = info.guards[1];
			if (!item.looted_item)
			{
				item.looted_item = true;
				looter->reward_item(drop_items, item.reward_item_id, item.reward_item_count * scale);
				output_stream out(S_SYSTEM_BROADCAST);
				out.write_byte(e_broadcast_type::bodyguard_loot_successful);
				out.write_uint(looter->dwRoleID);
				out.write_string(looter->roleName);
				out.write_uint(role->dwRoleID);
				out.write_string(role->roleName);
				out.write_uint(item.reward_item_id);
				service::broadcast(out);
			}
		}
		else if (state == e_state::inprogress_3)
		{
			auto& item = info.guards[2];
			if (!item.looted_item)
			{
				item.looted_item = true;
				looter->reward_item(drop_items, item.reward_item_id, item.reward_item_count * scale);
				output_stream out(S_SYSTEM_BROADCAST);
				out.write_byte(e_broadcast_type::bodyguard_loot_successful);
				out.write_uint(looter->dwRoleID);
				out.write_string(looter->roleName);
				out.write_uint(role->dwRoleID);
				out.write_string(role->roleName);
				out.write_uint(item.reward_item_id);
				service::broadcast(out);
			}
		}
	}
	if (state == e_state::inprogress_1)
	{
		info.add_looter(0, looter->dwRoleID, info.guards[0].looted_item ? info.guards[0].reward_item_id : 0);
	}
	else if (state == e_state::inprogress_2)
	{
		info.add_looter(1, looter->dwRoleID, info.guards[1].looted_item ? info.guards[1].reward_item_id : 0);
	}
	else if (state == e_state::inprogress_3)
	{
		info.add_looter(2, looter->dwRoleID, info.guards[2].looted_item ? info.guards[2].reward_item_id : 0);
	}
	info.data_changed = true;
	if (looter->bodyguard_loot_count > 0)looter->bodyguard_loot_count--;

	looter->send_battle_report(E_FIGHT_BODYGUARD, result, 0, rewarded_coin, 0, 0, drop_items);
	return 0;
}


void bodyguard::send_reward(ROLE* role, info& reward, single& item)
{
	uint coin = reward.coin / 10 * (10 - item.looted_count());
	uint exp = reward.exp;
	ADDUP(role->dwCoin, coin);
	notifyCoin(role);
	CONFIG::addRoleExp(role, role->dwExp, exp);
	notifyExp(role);
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, coin, 0, purpose::bodyguard_reward, 0);
	if (!item.looted_item)
	{
		role->reward_item(item.reward_item_id);
	}
}


// 获取运镖奖励
int bodyguard::rpc_get_reward(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,
		not_start,// 未开始
		not_finish,// 没有完成的任务
		got,// 已经领取
		package_full,// 背包满
	};

	output_stream out(S_BODYGUARD_GET_REWARD);

	auto& info = records[role->dwRoleID];
	auto state = info.state();
	if (state == e_state::not_start)
	{
		out.write_byte(result::not_start);// 未开始
		role->send(out);
		return 0;
	}
	else if (state < e_state::inprogress_2)
	{
		out.write_byte(result::not_finish);// 没有完成的任务
		role->send(out);
		return 0;
	}

	int index = -1;// 没有可领取
	int allow = 3;// 全部可领取
	if (state == e_state::history_done)// 判断残存的
	{
		auto end = info.start_time - info.start_time % (24 * 3600) + (24 * 3600);// 开始那天的结束时间
		allow = (end - info.start_time) / (round_time * 3600);// 开始那天可以跑的次数
	}
	if (state > e_state::inprogress_1)// 满足第一次领取时间
	{
		if (!info.guards[0].got)
		{
			index = 0;
		}
		else if (state > e_state::inprogress_2 && allow > 0)// 满足第二次领取时间
		{
			if (!info.guards[1].got)
			{
				index = 1;
			}
			else if (state > e_state::inprogress_3 && allow > 1)// 满足第三次领取时间
			{
				if (!info.guards[2].got)
				{
					index = 2;
				}
			}
		}
	}

	if (index == -1)
	{
		out.write_byte(result::not_finish);
		role->send(out);
		return 0;
	}

	logINFO("bodyguard get reward roleid(%u) order(%d).", role->dwRoleID, index);

	out.write_byte(result::successful);
	auto& item = info.guards[index];
	send_reward(role, info, item);
	out.write_uint(info.coin);
	out.write_uint(info.exp);
	if (!item.looted_item)
	{
		role->reward_item(item.reward_item_id);
		out.write_uint(item.reward_item_id);
	}
	else
	{
		out.write_uint(0);
	}
	item.got = true;
	info.data_changed = true;

	role->send(out);
	return 0;
}

// 获取运镖奖励信息
int bodyguard::rpc_get_status(ROLE* role, input_stream& stream)
{

	output_stream out(S_BODYGUARD_STATUS);

	auto now = time(0) - service::tzone;

	// 状态
	auto& info = records[role->dwRoleID];
	e_state state = info.state();
	if (state == e_state::history_done && info.guards[0].got && info.guards[1].got && info.guards[2].got)
	{
		info.reset();
		state = e_state::not_start;
	}

	out.write_byte(state);

	// 卦象
	if (info.divination == -1)
		info.divination = BASE::randTo(divination_configs.size());
	out.write_byte((byte)info.divination);
	out.write_byte((byte)info.hidden ? 1 : 0);
	out.write_byte((byte)info.item_plus ? 1 : 0);

	// 镖次，奖励
	if (state == e_state::not_start)// 未开始
	{
		out.write_byte(0);// 镖次
		out.write_uint(0);// 金币
		out.write_uint(0);// 经验
		out.write_uint(0);// 道具ID
		out.write_uint(0);// 道具数量
		out.write_byte(0);// 被劫次数
		out.write_byte(0);// 道具被劫
		out.write_uint(0);// 剩余时间
		out.write_byte(0);// 是否有可领奖励
	}
	else if (state == e_state::inprogress_1)
	{
		auto item = &info.guards[0];
		out.write_byte(1);
		out.write_uint(info.coin / 10 * (10 - item->looted_count()));
		out.write_uint(info.exp);
		out.write_uint(item->reward_item_id);
		out.write_uint(item->reward_item_count);
		out.write_byte((byte)item->looted_count());
		out.write_byte((byte)item->looted_item);
		out.write_uint((uint)(info.start_time + round_time * 3600 - now));
		out.write_byte(0);// 是否有可领奖励
	}
	else if (state == e_state::inprogress_2)
	{
		auto item = &info.guards[1];
		out.write_byte(2);
		out.write_uint(info.coin / 10 * (10 - item->looted_count()));
		out.write_uint(info.exp);
		out.write_uint(item->reward_item_id);
		out.write_uint(item->reward_item_count);
		out.write_byte((byte)item->looted_count());
		out.write_byte((byte)item->looted_item);
		out.write_uint((uint)(info.start_time + round_time * 2 * 3600 - now));
		out.write_byte((byte)!info.guards[0].got);// 是否有可领奖励
	}
	else if (state == e_state::inprogress_3)
	{
		auto item = &info.guards[2];
		out.write_byte(3);
		out.write_uint(info.coin / 10 * (10 - item->looted_count()));
		out.write_uint(info.exp);
		out.write_uint(item->reward_item_id);
		out.write_uint(item->reward_item_count);
		out.write_byte((byte)item->looted_count());
		out.write_byte((byte)item->looted_item);
		out.write_uint((uint)(info.start_time + round_time * 3 * 3600 - now));
		out.write_byte((byte)(!info.guards[0].got || !info.guards[1].got));// 是否有可领奖励
	}
	else// 今天结束或历史有结束均可领取奖励
	{
		out.write_byte(0);// 镖次
		out.write_uint(0);// 金币
		out.write_uint(0);// 经验
		out.write_uint(0);// 道具ID
		out.write_uint(0);// 道具数量
		out.write_byte(0);// 被劫次数
		out.write_byte(0);// 道具被劫
		out.write_uint(0);// 剩余时间
		auto end = info.start_time - info.start_time % (24 * 3600) + (24 * 3600);// 开始那天的结束时间
		auto allow = (end - info.start_time) / (round_time * 3600);// 开始那天可以跑的次数

		out.write_byte((byte)(!info.guards[0].got || (allow > 0 && !info.guards[1].got) || (allow > 1 && !info.guards[2].got)));// 是否有可领奖励
	}
	role->send(out);

	return 0;
}

// 购买抢劫次数
int bodyguard::rpc_buy_loot_count(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,
		nonvip_cant_buy,// 非VIP不能购买
		not_enough_count,// 没有足够的购买次数
		not_enough_money,// 元宝不足
	};

	output_stream out(S_BODYGUARD_BUY_LOOT_COUNT);

	if (role->wVipLevel == 0)
	{
		out.write_byte(result::nonvip_cant_buy);
		role->send(out);
		return 0;
	}

	if (role->wVipLevel > vip_loot_count_configs.size())// 索引保护
	{
		logFATAL("(role:%u)vip out of index.(vip:%u)", role->dwRoleID, role->wVipLevel);
		return 0;
	}
	auto& cfg = vip_loot_count_configs[role->wVipLevel - 1];
	if (role->bodyguard_buy_loot_count + 1 > cfg.count)// 购买次数保护
	{
		out.write_byte(result::not_enough_count);
		role->send(out);
		return 0;
	}

	// 元宝是否满足
	if (role->dwIngot < cfg.ingod)
	{
		out.write_byte(result::not_enough_money);
		role->send(out);
		return 0;
	}

	logINFO("bodyguard divination roleid(%u) .", role->dwRoleID);

	// 扣钱
	ADDUP(role->dwIngot, -(int)cfg.ingod);
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)cfg.ingod, purpose::bodyguard_buy_loot_count, 0);
	notifyIngot(role);

	role->bodyguard_buy_loot_count++;
	role->bodyguard_loot_count++;

	out.write_byte(result::successful);
	out.write_byte((byte)role->bodyguard_loot_count);
	if (role->wVipLevel == 0)// 索引保护,下一次的价格
	{
		out.write_uint(0);
		out.write_uint(0);
	}
	else if (role->wVipLevel > vip_loot_count_configs.size())// 索引保护,下一次的价格
	{
		out.write_uint(0);
		out.write_uint(0);
	}
	else
	{
		auto& cfg = vip_loot_count_configs[role->wVipLevel - 1];
		out.write_uint(cfg.count > role->bodyguard_buy_loot_count ? cfg.ingod : 0);
		out.write_uint(cfg.count > role->bodyguard_buy_loot_count ? cfg.count - role->bodyguard_buy_loot_count : 0);
	}
	role->send(out);
	return 0;
}

// 卜一卦
int bodyguard::rpc_buy_divination(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,
		not_enough_money,// 元宝不足
	};

	output_stream out(S_BODYGUARD_DIVINATION);

	if (role->dwIngot < divination_price)
	{
		out.write_byte(result::not_enough_money);
		role->send(out);
		return 0;
	}

	logINFO("bodyguard divination roleid(%u) .", role->dwRoleID);

	ADDUP(role->dwIngot, -(int)divination_price);
	notifyIngot(role);
	logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)divination_price, purpose::bodyguard_divination, 0);

	auto& info = records[role->dwRoleID];
	info.divination = BASE::randTo(divination_configs.size());
	info.data_changed = true;
	
	out.write_byte(result::successful);
	divination_config* divination = nullptr;
	if ((uint)info.divination < divination_configs.size())
		divination = &divination_configs[info.divination];
	if (divination == nullptr)
	{
		logFATAL("can not find divination config from id(%u)", info.divination);
		return 0;
	}

	// 卜卦后改变
	//auto now = time(0) - service::tzone;
	//info.coin = role->wLevel * 3000 + 40000;
	//info.exp = uint(pow(500.f * role->wLevel, 0.72f) / 100.f) * 100;
	//info.coin *= divination->coin_up;
	//info.exp *= divination->exp_up;

	//if (now > info.start_time)
	//{
	//	if (info.item_plus > 0 || BASE::randBetween(0.f, 1.f) < 0.5f * divination->item_up)// 幸运的取到了道具(花钱必得)
	//	{
	//		auto rd = BASE::randBetween(0.f, 1.f);
	//		float cur = 0.f;
	//		for (auto& ritem : reward_configs)
	//		{
	//			cur += info.item_plus ? ritem.probability_plus : ritem.probability;// 如果消费了元宝，则提升道具池的数量
	//			if (rd < cur)
	//			{
	//				.reward_item_id = ritem.item_id;
	//				break;
	//			}
	//		}
	//	}
	//}
	out.write_byte((byte)info.divination);
	role->send(out);

	return 0;
}

// 列表
int bodyguard::rpc_list(ROLE* role, input_stream& stream)
{
	output_stream out(S_BODYGUARD_LIST);
	out.write_byte((byte)role->bodyguard_loot_count);
	if (role->wVipLevel == 0)
	{
		out.write_uint(0);
		out.write_uint(0);
	}
	else if (role->wVipLevel > vip_loot_count_configs.size())
	{
		logERROR("role(%u) vip level(%u) is large than vip_loot_count_configs", role->dwRoleID, role->wVipLevel);
		out.write_uint(0);
		out.write_uint(0);
	}
	else
	{
		auto& cfg = vip_loot_count_configs[role->wVipLevel - 1];
		out.write_uint(cfg.count > role->bodyguard_buy_loot_count ? cfg.ingod : 0);
		out.write_uint(cfg.count > role->bodyguard_buy_loot_count ? cfg.count - role->bodyguard_buy_loot_count : 0);
	}
	auto pos = out.length();
	out.write_byte(0);
	byte final_count = 0;

	set<uint> existed;
	for (; final_count < list_count;)// 添加代码小心死循环
	{
		if (existed.size() >= records.size())// 超额保护
			break;
		auto rd = BASE::randTo(records.size());
		if (existed.find(rd) != existed.end())
			continue;
		existed.insert(rd);
		uint cur = 0;
		for (auto& item : records)
		{
			if (cur++ == rd)
			{
				auto state = item.second.state();
				// 屏蔽自己、隐藏的，不在运镖状态的
				if (item.first == role->dwRoleID || item.second.hidden || state < e_state::inprogress_1 || state > e_state::inprogress_3)// 自己或隐藏的略过
				{
					break;
				}
				auto temp = RoleMgr::getMe().getRoleByID(item.first);
				if (temp == nullptr)// 不能找到角色信息时中断此次处理
				{
					logFATAL("bodyguard error: can not find role(%u)", item.first);
					break;
				}
				out.write_uint(temp->dwRoleID);
				out.write_string(temp->roleName);
				out.write_byte(temp->byJob);
				out.write_byte(temp->bySex);
				out.write_uint(item.second.coin);
				final_count++;
				break;
			}
		}
	}

	out.write_data(&final_count, 1, pos);
	role->send(out);
	return 0;
}

// 列表
int bodyguard::rpc_been_loot_record(ROLE* role, input_stream& stream)
{
	output_stream out(S_BODYGUARD_BEEN_LOOT_RECORD);

	auto iter = records.find(role->dwRoleID);
	if (iter == records.end())
	{
		out.write_byte(0);// 已经开始
		role->send(out);
		return 0;
	}

	out.write_byte((byte)iter->second.history.size());
	for (auto& item : iter->second.history)
	{
		auto looter = RoleMgr::getMe().getRoleByID(item.looterid);
		if (!looter)
		{
			logFATAL("rpc_looters_list can not find role from roleid(%u).", item.looterid);
			return 0;
		}
		out.write_byte(item.order);
		out.write_uint(item.time);
		out.write_string(looter->roleName);
		out.write_uint(item.looted_itemid);
	}
	role->send(out);
	return 0;
}


void bodyguard::get_info(ROLE* role, byte& state, byte& remain_receive, time_t& start_time)
{
	auto& info = records[role->dwRoleID];
	auto _state = info.state();
	if (_state == e_state::not_start || _state == e_state::inprogress_1)
	{
		remain_receive = 0;
	}
	else if (_state == e_state::inprogress_2)
	{
		remain_receive = 1;
	}
	else if (_state == e_state::inprogress_2)
	{
		remain_receive = 2;
	}
	else
	{
		auto end = info.start_time - info.start_time % (24 * 3600) + (24 * 3600);// 开始那天的结束时间
		auto allow = (end - info.start_time) / (round_time * 3600) + 1;// 开始那天可以跑的次数
		remain_receive = allow;
	}
	state = _state;
	start_time = info.start_time;
}


void bodyguard::debug_set_time(ROLE* role, time_t time)
{
	auto iter = records.find(role->dwRoleID);
	if (iter == records.end())
	{
		return;
	}

	if (time != 0)
		iter->second.start_time += -time * 3600;
	else
		iter->second.start_time = 0;
}

// 保存
void bodyguard::save_data()
{
	static char sql_full[4096];
	static char sql_history[2048];
	static char sql_temp[100];
	for (auto& item : records)
	{
		if (item.second.data_changed)
		{
			auto& info = item.second;
			if (info.history.empty())
				sprintf(sql_history, "''");
			else
			{
				sprintf(sql_history, "0x%02x%02x", 1, (byte)info.history.size());// 版本1(byte),数量(byte)
				for (auto& hitem : info.history)
				{
					sprintf(sql_temp, "%08x%02x%08x%08x", htonl(hitem.time), hitem.order, htonl(hitem.looterid), htonl(hitem.looted_itemid));
					strcat(sql_history, sql_temp);
				}
			}
			sprintf(sql_full, "insert into activity_bodyguard(`roleid`, `start_time`, `hidden`, `item_plus`, `divination`, `coin`, `exp`,\
						   `order1_reward_itemid`, `order1_reward_itemcount`, `order1_got`, `order1_looted_item`, `order1_looter1`, `order1_looter2`,\
						    `order2_reward_itemid`, `order2_reward_itemcount`, `order2_got`, `order2_looted_item`, `order2_looter1`, `order2_looter2`,\
							 `order3_reward_itemid`, `order3_reward_itemcount`, `order3_got`, `order3_looted_item`, `order3_looter1`, `order3_looter2`, `history`) \
							  values(%u,%d,%u,%u,%d,%u,%u,  %u, %u, %u, %u, %u, %u,  %u, %u, %u, %u, %u, %u,  %u, %u, %u, %u, %u, %u, %s) on duplicate key update \
							  `start_time`=%d, `hidden`=%u, `item_plus`=%u, `divination`=%d, `coin`=%u, `exp`=%u,\
						   `order1_reward_itemid`=%u, `order1_reward_itemcount`=%u, `order1_got`=%u, `order1_looted_item`=%u, `order1_looter1`=%u, `order1_looter2`=%u,\
						    `order2_reward_itemid`=%u, `order2_reward_itemcount`=%u, `order2_got`=%u, `order2_looted_item`=%u, `order2_looter1`=%u, `order2_looter2`=%u,\
							 `order3_reward_itemid`=%u, `order3_reward_itemcount`=%u, `order3_got`=%u, `order3_looted_item`=%u, `order3_looter1`=%u, `order3_looter2`=%u, `history`=%s",
							  item.first, (int)info.start_time, info.hidden, info.item_plus, info.divination, info.coin, info.exp,
							  info.guards[0].reward_item_id, info.guards[0].reward_item_count, info.guards[0].got, info.guards[0].looted_item, info.guards[0].looters[0], info.guards[0].looters[1],
							  info.guards[1].reward_item_id, info.guards[1].reward_item_count, info.guards[1].got, info.guards[1].looted_item, info.guards[1].looters[0], info.guards[1].looters[1],
							  info.guards[2].reward_item_id, info.guards[2].reward_item_count, info.guards[2].got, info.guards[2].looted_item, info.guards[2].looters[0], info.guards[2].looters[1], sql_history,
							  (int)info.start_time, info.hidden, info.item_plus, info.divination, info.coin, info.exp,
							  info.guards[0].reward_item_id, info.guards[0].reward_item_count, info.guards[0].got, info.guards[0].looted_item, info.guards[0].looters[0], info.guards[0].looters[1],
							  info.guards[1].reward_item_id, info.guards[1].reward_item_count, info.guards[1].got, info.guards[1].looted_item, info.guards[1].looters[0], info.guards[1].looters[1],
							  info.guards[2].reward_item_id, info.guards[2].reward_item_count, info.guards[2].got, info.guards[2].looted_item, info.guards[2].looters[0], info.guards[2].looters[1], sql_history);
			if (!execSQL(DBCtrl::getSQL(), sql_full))
			{
				logFATAL("update bodyguard info error %s roleid(%u)", mysql_error(DBCtrl::getSQL()), item.first);
			}
		}
	}
}

void bodyguard::load_data()
{
	//auto last = GetTickCount();
	// 从数据库加载数据，服务时间长将导致加载缓慢，耗费内存.经分析，可满足几万用户量的需求，暂不用修改为实时数据存取。
	SQLSelect sqlx(DBCtrl::getSQL(), "select `roleid`, `start_time`, `hidden`, `item_plus`, `divination`, `coin`, `exp`,\
						   `order1_reward_itemid`, `order1_reward_itemcount`, `order1_got`, `order1_looted_item`, `order1_looter1`, `order1_looter2`,\
						    `order2_reward_itemid`, `order2_reward_itemcount`, `order2_got`, `order2_looted_item`, `order2_looter1`, `order2_looter2`,\
							 `order3_reward_itemid`, `order3_reward_itemcount`, `order3_got`, `order3_looted_item`, `order3_looter1`, `order3_looter2`, `history` from activity_bodyguard");

	while (MYSQL_ROW row = sqlx.getRow())
	{
		auto roleid = (uint)atoi(row[0]);
		auto& info = records[roleid];
		info.start_time = atoi(row[1]);
		info.hidden = atoi(row[2])>0;
		info.item_plus = atoi(row[3])>0;
		info.divination = atoi(row[4]);
		info.coin = (uint)atoi(row[5]);
		info.exp = atoi(row[6]);
		info.guards[0].reward_item_id = atoi(row[7]);
		info.guards[0].reward_item_count = atoi(row[8]);
		info.guards[0].got = atoi(row[9])>0;
		info.guards[0].looted_item = atoi(row[10])>0;
		info.guards[0].looters[0] = (uint)atoll(row[11]);
		info.guards[0].looters[1] = (uint)atoll(row[12]);
		info.guards[1].reward_item_id = atoi(row[13]);
		info.guards[1].reward_item_count = atoi(row[14]);
		info.guards[1].got = atoi(row[15])>0;
		info.guards[1].looted_item = atoi(row[16])>0;
		info.guards[1].looters[0] = (uint)atoll(row[17]);
		info.guards[1].looters[1] = (uint)atoll(row[18]);
		info.guards[2].reward_item_id = atoi(row[19]);
		info.guards[2].reward_item_count = atoi(row[20]);
		info.guards[2].got = atoi(row[21])>0;
		info.guards[2].looted_item = atoi(row[22])>0;
		info.guards[2].looters[0] = (uint)atoll(row[23]);
		info.guards[2].looters[1] = (uint)atoll(row[24]);
		auto blob_len = sqlx.lengths[25];
		if (blob_len > 2)
		{
			string blob(row[25], blob_len);
			switch (blob[0])
			{
			case 1:
				for (byte i = 0; i < blob[1]; i++)
				{
					auto cur_data = (info::history_item*)&blob[2 + i * 13];
					info.history.push_back(*cur_data);
				}
				break;
			}
		}
	}
	//cout << "bodyguard cost time during load from database." << GetTickCount() - last << endl;
	//auto& info = records[100001];
	//info.data_changed = true;
	//info.add_looter(1, 99999, 123);
	//info.add_looter(1, 99999, 0);
	//info.add_looter(2, 99999, 0);
	//info.add_looter(2, 99999, 0);
	//info.history.push_back(info::history_item((byte)3, 123, 12323));
	//info.history.push_back(info::history_item((byte)3, 123, 12323));
	//info.history.push_back(info::history_item((byte)3, 123, 12323));
	//info.history.push_back(info::history_item((byte)3, 123, 12323));
	//info.history.push_back(info::history_item((byte)3, 123, 12323));
	//info.history.push_back(info::history_item((byte)3, 123, 12323));

	//save_data();
}

// 加载配置
void bodyguard::load()
{
	string config_file = string(getProgDir()) + STR_FSEP + "config/bodyguard.xml";

	TiXmlDocument doc;
	if (!doc.LoadFile(config_file.c_str()))
		THROW_EXCEPTION("can't load bodyguard.xml");

	auto root = doc.RootElement();

	decltype(reward_configs) temp_reward_configs;
	decltype(divination_configs) temp_divination_configs;
	decltype(vip_loot_count_configs) temp_vip_loot_count_configs;

	auto reward = root->FirstChild("reward");
	if (reward == nullptr)
	{
		cout << "can't load reward node." << endl;
		THROW_EXCEPTION("can't load reward node");
	}

	// 逐条加载奖励配置
	auto node = reward->FirstChild();
	while (node)
	{
		reward_config cfg;
		if (!tixml_value(node->FirstChild("item_id"), cfg.item_id))
			THROW_EXCEPTION("can't read reward item_id");
		if (!tixml_value(node->FirstChild("probability"), cfg.probability))
			THROW_EXCEPTION("can't read reward probability");
		if (!tixml_value(node->FirstChild("probability_plus"), cfg.probability_plus))
			THROW_EXCEPTION("can't read reward probability_plus");
		temp_reward_configs.push_back(cfg);
		node = node->NextSibling();
	}

	auto luck = root->FirstChild("divination");
	if (luck == nullptr)
	{
		cout << "can't load divination node." << endl;
		THROW_EXCEPTION("can't load divination node");
	}

	// 逐条加载占卜配置
	node = luck->FirstChild();
	while (node)
	{
		divination_config cfg;
		if (!tixml_value(node->FirstChild("coin_up"), cfg.coin_up))
			THROW_EXCEPTION("can't read divination coin_up");
		if (!tixml_value(node->FirstChild("exp_up"), cfg.exp_up))
			THROW_EXCEPTION("can't read divination exp_up");
		if (!tixml_value(node->FirstChild("item_up"), cfg.item_up))
			THROW_EXCEPTION("can't read divination item_up");
		if (!tixml_value(node->FirstChild("probability"), cfg.probability))
			THROW_EXCEPTION("can't read divination probability");
		temp_divination_configs.push_back(cfg);
		node = node->NextSibling();
	}

	auto vip_loot_count = root->FirstChild("vip_loot_count");
	if (vip_loot_count == nullptr)
	{
		cout << "can't load vip_loot_count node." << endl;
		THROW_EXCEPTION("can't load vip_loot_count node");
	}

	// 逐条加载VIP抢劫次数配置
	node = vip_loot_count->FirstChild();
	while (node)
	{
		vip_loot_count_config cfg;
		if (!tixml_value(node->FirstChild("count"), cfg.count))
			THROW_EXCEPTION("can't read coin_up");
		if (!tixml_value(node->FirstChild("price"), cfg.ingod))
			THROW_EXCEPTION("can't read price");
		temp_vip_loot_count_configs.push_back(cfg);
		node = node->NextSibling();
	}

	auto db = root->FirstChild("config");
	if (db == nullptr)
	{
		cout << "can't load config node." << endl;
		THROW_EXCEPTION("can't load config node");
	}

	if (!tixml_value(db->FirstChild("round_time"), (int&)round_time))
		THROW_EXCEPTION("can't read round_time");
	if (!tixml_value(db->FirstChild("hidden_price"), hidden_price))
		THROW_EXCEPTION("can't read hidden_price");
	if (!tixml_value(db->FirstChild("item_plus_price"), item_plus_price))
		THROW_EXCEPTION("can't read item_plus_price");
	if (!tixml_value(db->FirstChild("divination_price"), divination_price))
		THROW_EXCEPTION("can't read divination_price");
	if (!tixml_value(db->FirstChild("list_count"), list_count))
		THROW_EXCEPTION("can't read list_count");
	if (!tixml_value(db->FirstChild("loot_item_probability"), loot_item_probability))
		THROW_EXCEPTION("can't read loot_item_probability");
	if (!tixml_value(db->FirstChild("double_start_time"), double_start_time))
		THROW_EXCEPTION("can't read double_start_time");
	if (!tixml_value(db->FirstChild("double_end_time"), double_end_time))
		THROW_EXCEPTION("can't read double_end_time");

	reward_configs = temp_reward_configs;
	divination_configs = temp_divination_configs;
	vip_loot_count_configs = temp_vip_loot_count_configs;
}


//using namespace rapidjson;
//
//#define   MAX_BOSSFIGHT_TIME   30000
//#define   HELPER_NUM		   6
//
//namespace ACTIVITY
//{ 
//	enum E_NOTICE_STATUS
//	{
//		E_NOTICE_STATUS_NO,//0无通知
//		E_NOTICE_STATUS_REGISTER,//1报名通知
//		E_NOTICE_STATUS_START,//2战斗开始
//		E_NOTICE_STATUS_END,//3战斗结束
//	};
//
//	static E_NOTICE_STATUS _bossFightStatus = E_NOTICE_STATUS_NO;
//
//
//
//	//1物理 2魔法 3道术
//	enum E_ATK
//	{
//		E_SKILL_ATK_TYPE_WARRIOR = 1,
//		E_SKILL_ATK_TYPE_MAGIC = 2,
//		E_SKILL_ATK_TYPE_MONK = 3,
//	};
//
//	struct SKILL_INFO
//	{
//		WORD  wID;			//技能id
//		BYTE  type;			//技能类型
//		DWORD cooldown;		//冷却时间
//		DWORD coolLeft;		//剩余冷却时间
//		WORD  priority;		//优先级
//		BYTE  skillType;	//属性类型
//		WORD  skillAtt;		//属性项目加成值
//		float skillCoff;	//属性值系数
//		SKILL_INFO(WORD  wID, BYTE  type, DWORD cooldown, WORD priority, BYTE  skillType, WORD skillAtt, float skillCoff)
//		{
//			this->wID=wID;		
//			this->cooldown=cooldown;
//			this->type = type;
//			this->priority=priority;
//			this->skillType = skillType;
//			this->skillAtt=skillAtt;
//			this->skillCoff=skillCoff;
//			this->coolLeft = cooldown;
//		}
//	};
//
//	struct  BOSSHURT
//	{
//		DWORD dwRoleID;
//		DWORD dwHurt;
//		ZERO_CONS(BOSSHURT);
//		BOSSHURT(DWORD dwRoleID, DWORD dwHurt)
//		{
//			this->dwRoleID = dwRoleID;
//			this->dwHurt = dwHurt;
//		}
//		bool operator == (const DWORD& dst)const
//		{
//			return ( this->dwRoleID == dst );
//		}
//	};
//
//	static vector< BOSSHURT > _vecBossHurt;
//	static deque<DWORD> _dequeWarriors;
//	static deque<DWORD> _dequeMagics;
//	static deque<DWORD> _dequeMonks;
//	static DWORD _bossFightHP;
//	//static bool _bBossFight = false;
//	static WORD _bossFightID;
//	//static DWORD _RankHonour[TOP_THREE] = {100, 80, 60, 10};
//	static DWORD _lastFightID;
//	int insertBossfightArmy( ROLE* role);
//	int insertArmyByJob( ROLE* role, deque<DWORD>& dequeArmy);
//	int bossFightPre( ROLE* role, unsigned char * data, size_t dataLen );
//	void getArmy( ROLE* role, vector<DWORD>& vecArmy);
//	DWORD getRandIDs(  ROLE* role, deque<DWORD> dequeIDs, size_t nums, vector<DWORD>& vecArmy);
//	//int clientGetBossFightInfos( ROLE* role );
//	//int clientBossFightInfos( ROLE* role, vector<DWORD>& vecArmy, int nTag = 0 );
//	WORD getFightSkill(vector<SKILL_INFO>&  SkillsInfo);
//	//WORD initFightSkills(const vector<WORD>& FightSkills, vector<SKILL_INFO>&  SkillsInfo);
//	//DWORD getMonAtkValue( ROLE*role, CONFIG::MONSTER_CFG* monCfg, CONFIG::SKILL_CFG2* monSkillCfg, int& intMonHurt);
//	//DWORD getRoleAtkValue( ROLE*role,CONFIG::MONSTER_CFG* monCfg,DWORD& comID,SKILL_INFO skillInfo,int& intHurt,DWORD& dwDelay);
//	//void setClothWeaponSkillLucky( ROLE* comRole,WORD& cloth, WORD& weapon, WORD& lucky, vector<SKILL_INFO>& SkillsInfo);
//	
//	//void initHelpers( ROLE* role );//找伙伴
//	void countBossFightHurt( ROLE* role, DWORD nHurt );
//	void registerBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg, const DWORD& nowSecond );
//	void startBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg, const DWORD& nowSecond );
//	void endBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg );
//	void offerBossFightReward( );
//	void sortHurtRank( );
//	void getArmyByJob( ROLE* role, const BYTE& job );
//	void getArmyByAllJob( ROLE* role, vector<vector<DWORD>>& vvecArmy);
//	DWORD getOneHelpByJob( ROLE* role, const BYTE& job, const int& nPos);
//}
//
//void ACTIVITY::sortHurtRank( )
//{
//	sort(_vecBossHurt.begin(), _vecBossHurt.end(), [](BOSSHURT one, BOSSHURT two)
//	{
//		return 	one.dwHurt > two.dwHurt;	
//	}
//	);
//}
//
DWORD ACTIVITY::getTodayNowSecond( const time_t now)//今天秒数
{
	struct tm tmNow;
	LOCAL_TIME(now, tmNow);
	DWORD nowSecond = tmNow.tm_hour * 60 * 60 + tmNow.tm_min * 60 + tmNow.tm_sec;
	return nowSecond;
}
//
////void ACTIVITY::setClothWeaponSkillLucky( ROLE* comRole,WORD& cloth, WORD& weapon, WORD& lucky, vector<SKILL_INFO>& SkillsInfo)//设置默认人物信息
////{
////	WORD fightSkill;
////	vector<WORD>  FightSkills;
////	CONFIG::ITEM_CFG* itemTempCfg = NULL;
////	ITEM_CACHE* itemTempCache = comRole->vBodyEquip[BODY_INDEX_WEAPON];//穿武器
////
////	if ( itemTempCache != NULL )
////	{
////		itemTempCfg = itemTempCache->itemCfg;
////		if ( itemTempCfg != NULL )
////		{
////			weapon = itemTempCfg->id;
////		}
////	}
////
////	itemTempCache = comRole->vBodyEquip[BODY_INDEX_CLOTH];//穿衣服
////	if ( itemTempCache != NULL )
////	{
////		itemTempCfg = itemTempCache->itemCfg;
////		if (itemTempCfg != NULL)
////		{
////			cloth = itemTempCfg->id;
////		}
////		else
////		{
////			cloth = (comRole->bySex == E_SEX_MALE)?E_MALE_CLOTH:E_FMALE_CLOTH;
////		}
////	}
////	else
////	{
////		cloth = (comRole->bySex == E_SEX_MALE)?E_MALE_CLOTH:E_FMALE_CLOTH;
////	}
////
////	for (size_t i = 0; i < FIGHT_SKILL_INDEXS; i++)//提出快捷技能ids
////	{
////		if ( (fightSkill = SKILL::getSkillIDByIndex(comRole, comRole->byFightSkill[i])) != 0)
////		{
////			FightSkills.push_back(fightSkill);
////		}
////	}
////
////	for ( int i=0; i<BODY_GRID_NUMS; i++)//幸运值
////	{
////		itemTempCache = comRole->vBodyEquip[i];
////		if ( itemTempCache == NULL )
////		{
////			continue;
////		}
////		itemTempCfg = itemTempCache->itemCfg;
////		if ( itemTempCfg == NULL )
////		{
////			continue;
////		}
////		
////		lucky += itemTempCfg->luck;		
////		if ( i == BODY_INDEX_WEAPON )
////			lucky += itemTempCache->byLuckPoint;
////	}
////
////	if (comRole->byJob == E_JOB_WARRIOR)//特殊技能
////	{
////		FightSkills.push_back( FIGHT_SKILL_WARRIOR );
////	}
////	if (comRole->byJob == E_JOB_MAGIC)
////	{
////		FightSkills.push_back( FIGHT_SKILL_MAGIC );
////	}
////	if (comRole->byJob == E_JOB_MONK)
////	{
////		FightSkills.push_back( FIGHT_SKILL_MONK );
////	}
////	initFightSkills( FightSkills, SkillsInfo);
////}
//
//WORD ACTIVITY::getBossFightID()//get bossfight monster id
//{
//	return _bossFightID;
//}
//
//void ACTIVITY::setBossFightID(WORD bossID)//set bossfight monster id
//{
//	_bossFightID = bossID;
//}
//
////DWORD ACTIVITY::getMonAtkValue( ROLE*role, CONFIG::MONSTER_CFG* monCfg, CONFIG::SKILL_CFG2* monSkillCfg, int& intMonHurt)//get monster attack hurt value 
////{
////	float fHurt = 0.0f;
////	if ( monSkillCfg->type == E_SKILL_TYPE_ATTACK )
////	{
////		if ( monSkillCfg->skillPara.skillType == E_SKILL_ATK_TYPE_WARRIOR )
////		{
////			//fHurt = -( BASE::randBetween( monCfg->atk_min, monCfg->atk_max+1) +  monSkillCfg->skillPara.skillAtt)*(monSkillCfg->skillPara.skillCoff/100.0f) - BASE::randBetween( role->roleAttr[E_ATTR_PHYDEF_MIN],role->roleAttr[E_ATTR_PHYDEF_MAX]+1);
////		}
////		else if ( monSkillCfg->skillPara.skillType == E_SKILL_ATK_TYPE_MAGIC )
////		{
////			//fHurt = -( BASE::randBetween( monCfg->mag_min, monCfg->mag_max+1) + monSkillCfg->skillPara.skillAtt)*(monSkillCfg->skillPara.skillCoff/100.0f) - BASE::randBetween( role->roleAttr[E_ATTR_MAGDEF_MIN], role->roleAttr[E_ATTR_MAGDEF_MAX]+1);
////		}
////		else if ( monSkillCfg->skillPara.skillType == E_SKILL_ATK_TYPE_MONK )
////		{
////			//fHurt = -( BASE::randBetween( monCfg->sol_min, monCfg->sol_max+1) + monSkillCfg->skillPara.skillAtt)*(monSkillCfg->skillPara.skillCoff/100.0f) - BASE::randBetween( role->roleAttr[E_ATTR_MAGDEF_MIN], role->roleAttr[E_ATTR_MAGDEF_MAX]+1);
////		}
////	}
////	//else if ( monSkillCfg->type == E_SKILL_TYPE_TREAT ) //暂定怪物无治疗
////	//{
////	//	//道术治疗		
////	//	//最终治疗值＝（攻击者道术攻击值÷3＋技能加成值）×（技能系数÷100）		
////	//	//攻击者道术攻击值=上限与下限的
////
////	//	fHurt = ( BASE::randBetween( monCfg->sol_min, monCfg->sol_max+1) / 3.0f + monSkillCfg->skillPara.skillAtt ) * (monSkillCfg->skillPara.skillCoff / 100.0f);
////	//}
////
////	intMonHurt = (int)fHurt;
////
////	return 0;
////}
//
////DWORD ACTIVITY::getRoleAtkValue(ROLE*role,CONFIG::MONSTER_CFG* monCfg,DWORD& comID,SKILL_INFO skillInfo,int& intHurt, DWORD& dwDelay)//计算技能伤害
////{
////
////	float fHurt = 0.0f;
////	if ( skillInfo.type == E_SKILL_TYPE_ATTACK )
////	{
////		if ( skillInfo.skillType == E_SKILL_ATK_TYPE_WARRIOR )
////		{
////			//fHurt = -( BASE::randBetween( role->roleAttr[E_ATTR_ATK_MIN], role->roleAttr[E_ATTR_ATK_MAX]+1) + skillInfo.skillAtt)*(skillInfo.skillCoff/100.0f) - BASE::randBetween( monCfg->phydef_min, monCfg->phydef_max+1);
////		}
////		else if ( skillInfo.skillType == E_SKILL_ATK_TYPE_MAGIC )
////		{
////			//fHurt = -( BASE::randBetween( role->roleAttr[E_ATTR_MAG_MIN], role->roleAttr[E_ATTR_MAG_MAX]+1) + skillInfo.skillAtt)*(skillInfo.skillCoff/100.0f) - BASE::randBetween( monCfg->magdef_min, monCfg->magdef_max+1);
////		}
////		else if ( skillInfo.skillType == E_SKILL_ATK_TYPE_MONK )
////		{
////			fHurt = -( BASE::randBetween( role->roleAttr[E_ATTR_SOL_MIN], role->roleAttr[E_ATTR_SOL_MAX]+1) + skillInfo.skillAtt)*(skillInfo.skillCoff/100.0f) - BASE::randBetween( monCfg->magdef_min, monCfg->magdef_max+1);
////		}
////	}
////	else if ( skillInfo.type == E_SKILL_TYPE_SUMMON )
////	{
////		comID = role->dwRoleID;
////	}
////	else if ( skillInfo.type == E_SKILL_TYPE_PASSIVE )
////	{
////		comID = role->dwRoleID;
////	}
////	else if ( skillInfo.type == E_SKILL_TYPE_TREAT )
////	{
//////道术治疗		
//////最终治疗值＝（攻击者道术攻击值÷3＋技能加成值）×（技能系数÷100）		
//////攻击者道术攻击值=上限与下限的
////
////		fHurt = ( BASE::randBetween( role->roleAttr[E_ATTR_SOL_MIN], role->roleAttr[E_ATTR_SOL_MAX]+1) / 3.0f + skillInfo.skillAtt ) * (skillInfo.skillCoff / 100.0f);
////		comID = role->dwRoleID;
////	}
////
////	fHurt += fHurt * (role->byEnhanceTime) * 0.1f;
////	intHurt = (int)fHurt;
////	//dwDelay += skillInfo.coolLeft;
////	//skillInfo.coolLeft = skillInfo.cooldown;
////	return 0;
////}
//
////WORD ACTIVITY::initFightSkills(const vector<WORD>& FightSkills, vector<SKILL_INFO>&  SkillsInfo)//找到技能
////{
////	for (auto it : FightSkills)
////	{
////		CONFIG::SKILL_CFG2 *skillCfg = CONFIG::getSkillCfg(it);
////		if (skillCfg == NULL)
////		{
////			continue;
////		}
////		SkillsInfo.push_back( SKILL_INFO(it,skillCfg->type,skillCfg->cooldown,skillCfg->priority,skillCfg->skillPara.skillType,skillCfg->skillPara.skillAtt,skillCfg->skillPara.skillCoff));
////	}
////	return 0;
////}
//
//WORD ACTIVITY::getFightSkill(vector<SKILL_INFO>&  SkillsInfo)//找到优先进攻技能
//{
//	std::sort(SkillsInfo.begin(), SkillsInfo.end(), 
//				[](SKILL_INFO one, SKILL_INFO two)
//				{
//					if ( one.coolLeft == two.coolLeft )
//					{
//						return one.priority < two.priority;
//					}
//					else
//					{
//						return one.coolLeft < two.coolLeft;
//					}
//				}
//			);
//	return 0;
//}
//
//
//DWORD ACTIVITY::getRandIDs( ROLE* role, deque<DWORD> dequeIDs, size_t nums, vector<DWORD>& vecArmy)
//{
//	deque<DWORD> dequeTempIDs( dequeIDs );
//	vecArmy.clear();
//	auto it = std::find(dequeTempIDs.begin(), dequeTempIDs.end(), role->dwRoleID);
//	if (it != dequeTempIDs.end())
//	{
//		dequeTempIDs.erase(it);
//	}
//	for (size_t i = 0; i < nums && dequeTempIDs.size() > 0; ++i )
//	{
//		size_t pos = rand() % dequeTempIDs.size();
//		DWORD id = dequeTempIDs[ pos ];
//		dequeTempIDs.erase( dequeTempIDs.begin() + pos );
//		vecArmy.push_back(id);
//	}
//
//	return 0;
//}
//
//
//DWORD ACTIVITY::getOneHelpByJob( ROLE* role, const BYTE& job, const int& nPos)
//{
//	if ( job < E_JOB_WARRIOR || job > E_JOB_MONK)
//	{
//		return 0;
//	}
//	vector<DWORD> vecIDs( role->vvecFightSourceHelpers[job - 1] );
//
//	role->vecBossFightHelpers[ nPos - 1 ] = 0;
//
//	for (size_t i = 0; i < HELPER_NUM; i++)
//	{
//		auto it = std::find(vecIDs.begin(), vecIDs.end(), role->vecBossFightHelpers[i]);
//		if (it != vecIDs.end())
//		{
//			vecIDs.erase(it);
//		}
//	}
//
//	if ( vecIDs.empty() )
//	{
//		return 0;
//	}
//	size_t pos = rand() % vecIDs.size();
//	return vecIDs[ pos ];
//}
//
//void ACTIVITY::getArmyByJob( ROLE* role, const BYTE& job)
//{
//	if ( job < E_JOB_WARRIOR || job > E_JOB_MONK)
//	{
//		return;
//	}
//	if (role->vvecFightSourceHelpers[job-1].size() >= 10)
//	{
//		return;
//	}
//	size_t min_nums;
//
//	switch ( job )
//	{
//	case E_JOB_WARRIOR:
//		min_nums = std::min<size_t>( _dequeWarriors.size(), 10 );
//		getRandIDs( role, _dequeWarriors, min_nums, role->vvecFightSourceHelpers[ job-1 ]);
//		break;
//
//	case E_JOB_MAGIC:
//		min_nums = std::min<size_t>( _dequeMagics.size(), 10 );
//		getRandIDs( role, _dequeMagics, min_nums, role->vvecFightSourceHelpers[ job-1 ]);
//		break;
//	case E_JOB_MONK:
//		min_nums = std::min<size_t>( _dequeMonks.size(), 10 );
//		getRandIDs( role,_dequeMonks, min_nums, role->vvecFightSourceHelpers[ job-1 ]);
//		break;
//	}
//}
//
//void ACTIVITY::getArmyByAllJob( ROLE* role, vector<vector<DWORD>>& vvecArmy)
//{
//	vector<DWORD> vecArmyWarriorr, vecArmyMagic, vecArmyMonk;
//	size_t min_nums_warriorr = std::min<size_t>( _dequeWarriors.size(), 10 );
//	size_t min_nums_magic = std::min<size_t>( _dequeMagics.size(), 10 );
//	size_t min_nums_monk = std::min<size_t>( _dequeMonks.size(), 10 );
//	getRandIDs( role, _dequeWarriors, min_nums_warriorr, vecArmyWarriorr);
//	getRandIDs( role, _dequeMagics, min_nums_magic, vecArmyMagic);
//	getRandIDs( role,_dequeMonks, min_nums_monk, vecArmyMonk);
//	vvecArmy.push_back(vecArmyWarriorr);
//	vvecArmy.push_back(vecArmyMagic);
//	vvecArmy.push_back(vecArmyMonk);
//}
//
//
//void ACTIVITY::getArmy( ROLE* role, vector<DWORD>& vecArmy)
//{
//	size_t min_nums_warriorr = std::min<size_t>( _dequeWarriors.size(), 10 );
//	size_t min_nums_magic = std::min<size_t>( _dequeMagics.size(), 10 );
//	size_t min_nums_monk = std::min<size_t>( _dequeMonks.size(), 10 );
//	getRandIDs( role, _dequeWarriors, min_nums_warriorr, vecArmy);
//	getRandIDs( role, _dequeMagics, min_nums_magic, vecArmy);
//	getRandIDs( role,_dequeMonks, min_nums_monk, vecArmy);
//}
//
//int ACTIVITY::insertArmyByJob( ROLE* role, deque<DWORD>& dequeArmy)
//{
//	if (std::find(dequeArmy.begin(), dequeArmy.end(),role->dwRoleID) != dequeArmy.end())
//	{
//		return ALREADY_ENLIST;
//	}
//
//	dequeArmy.push_back(role->dwRoleID);
//	return ACTIVITY_SUCCESS;
//}
//
//int ACTIVITY::insertBossfightArmy( ROLE* role)
//{
//	switch (role->byJob)
//	{
//	case E_JOB_WARRIOR:
//		return insertArmyByJob(role, _dequeWarriors);
//	case E_JOB_MAGIC:
//		return insertArmyByJob(role, _dequeMagics);
//	case E_JOB_MONK:
//		return insertArmyByJob(role, _dequeMonks);
//	default: return ACTIVITY_NO_EXSIST;
//	}
//}
//
//
///*
//@//#define	    S_GET_ACTIVITY		0x0800		//get activities list
//*/
int ACTIVITY::clientGetActivities( ROLE* role, unsigned char * data, size_t dataLen )
{
	static const byte open_state = 1;
	static const byte close_state = 0;
	static const byte switch_close = 0;
	string strData;
	DWORD nowSecond = getTodayNowSecond( time(nullptr) );
	for (auto &it : mapActivityInfoCfg)
	{
		COND_CONTINUE(it.second.type == switch_close);
		S_APPEND_WORD(strData, it.first);
		
		if (role->wLevel >= it.second.open_level && nowSecond >= it.second.advance_time && nowSecond < it.second.end_time)
		{
			S_APPEND_BYTE( strData, open_state );
		}
		else
		{
			S_APPEND_BYTE(strData, close_state);
		}
	}
	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_GET_ACTIVITY, strData));
	return 0;
}

/////*
////@参加活动#define	    S_JOIN_ACTIVITY			0x0801		//join activity
////*/
////int ACTIVITY::registerActivity( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	BYTE byRet = ACTIVITY_NO_EXSIST;
////	WORD actID;
////	string strData;
////	if ( !BASE::parseWORD( data, dataLen, actID) )
////		return -1;
////	CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg(actID);
////	if ( activityCfg == NULL || role->wLevel < activityCfg->open_level)
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		S_APPEND_BYTE( strData, byRet );
////		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_JOIN_ACTIVITY, strData));
////		return 0;
////	}
////
////	DWORD nowSecond = getTodayNowSecond( time(nullptr) );
////
////	if (nowSecond < activityCfg->advance_time || nowSecond >= activityCfg->end_time)//不在报名战斗时间区
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		S_APPEND_BYTE( strData, byRet );
////		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_JOIN_ACTIVITY, strData));
////	}
////	else if (nowSecond < activityCfg->begin_time)//报名
////	{
////		if ( actID == WORLD_BOSS_FIGHT )
////		{
////			byRet = insertBossfightArmy( role );
////		}
////		byRet = ACTIVITY_SUCCESS;
////		S_APPEND_BYTE( strData, byRet );
////		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_JOIN_ACTIVITY, strData));
////	}
////	//else if (nowSecond < activityCfg->end_time)
////	//{	
////		//insertBossfightArmy( role );
////		//clientGetBossFightInfos( role );//boss 战斗  wm这里只有报名
////	//}
////	return 0;
////}
//
/////*
////@获取战斗序列信息
////*/
////int ACTIVITY::clientGetBossFightInfos( ROLE* role )
////{
////	if (role  == NULL)//到点通知用
////	{
////		return 0;
////	}
////	if (role->client == NULL)//到点通知用
////	{
////		return 0;
////	}
////	vector<DWORD> vecArmy;
////	vecArmy.push_back(role->dwRoleID);//自己在第一个
////	getArmy(role, vecArmy);
////	clientBossFightInfos( role, vecArmy);
////	return 0;
////	//while (vecArmy.size() < 7)
////	//{
////	//	vecArmy.push_back(role->dwRoleID);
////	//}
////	//CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );
////	//if ( activityCfg == NULL )
////	//{
////	//	return 0;
////	//}
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(activityCfg->monster_id);
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( _bossFightID );
////	//if ( monCfg == NULL )
////	//{
////	//	return 0;
////	//}
////
////	//CONFIG::SKILL_CFG2* monSkillCfg = CONFIG::getSkillCfg(monCfg->skill_id);
////	//if (monSkillCfg == NULL)
////	//{
////	//	return 0;
////	//}
////
////	//BYTE byFinal = 0;
////	//BYTE byRet = 0;
////	//DWORD dwFightTime = 0;
////	//WORD wRoleHP = role->roleAttr[E_ATTR_HP_MAX];
////	//char xxx[1024*20]={0};
////	//char xxxMon[10240]={0};
////	//char* p = xxx;
////	//char* pMon = xxxMon;
////	//if (_bossFightHP == 0)
////	//{
////	//	byRet = ACTIVITY_BOSS_DIE;
////	//}
////	//
////	//p += sprintf( p, "{\"error\":%u,", byRet );
////	//p += sprintf( p, "\"m\":%u,", monCfg->id );
////	//p += sprintf( p, "\"mb\":%u,", _bossFightHP );
////	//p += sprintf( p, "\"hf\":[");
////	//for (size_t roleIndex = 0; roleIndex < vecArmy.size(); ++roleIndex)
////	//{
////	//	DWORD id = vecArmy[roleIndex];
////
////	//	ROLE* comRole = RoleMgr::getMe().getRoleByID( id );
////	//	if (comRole == NULL)
////	//	{
////	//		continue;
////	//	}
////
////	//	WORD cloth = 0;
////	//	WORD weapon = 0;
////	//	WORD lucky = 0;
////	//	DWORD dwAllDelay = 0;
////	//	DWORD dwOneDelay = 0;
////	//	DWORD comID;
////	//	int intHurt;
////	//	vector<SKILL_INFO> SkillsInfo;
////	//	setClothWeaponSkillLucky( comRole, cloth,  weapon,  lucky, SkillsInfo );
////
////	//	p += sprintf( p, "{");
////	//	p += sprintf( p, "\"w\":%u,", weapon );
////	//	p += sprintf( p, "\"c\":%u,", cloth );
////	//	p += sprintf( p, "\"j\":%u,", comRole->byJob );
////	//	p += sprintf( p, "\"s\":%u,", comRole->bySex );
////	//	p += sprintf( p, "\"id\":%u,", comRole->dwRoleID );
////	//	p += sprintf( p, "\"q\":[");
////
////	//	if (roleIndex == 0 )//self
////	//	{
////	//		DWORD dwTempAllDelay = 0;
////	//		int intMonHurt = 0;
////
////	//		while (wRoleHP > 0 && _bossFightHP > 0 && dwAllDelay <=MAX_BOSSFIGHT_TIME && SkillsInfo.size() > 0 )
////	//		{
////	//			//self fight sequence
////	//			getFightSkill(SkillsInfo);
////	//			comID = monCfg->id;//治疗的时候可能会变成自己
////	//			getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//			p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////
////	//			dwOneDelay = SkillsInfo[0].coolLeft;
////	//			dwAllDelay += dwOneDelay;
////
////	//			for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//			{
////	//				if (i == 0)//使用的
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//				}
////	//				else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//				{
////	//					SkillsInfo[i].coolLeft = 0;
////	//				}
////	//				else
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//				}
////	//			}
////
////	//			if (comID == monCfg->id && intHurt >= 0)//role hurt monster
////	//			{
////	//				if ( _bossFightHP <= (DWORD)intHurt)
////	//				{
////	//					_bossFightHP = 0;//怪物被打死 记下  dwAllDelay
////	//					byFinal = 1;
////	//				}
////	//				else
////	//				{
////	//					_bossFightHP -= (DWORD)intHurt;
////	//				}
////	//			}
////
////	//			//moster fight sequence
////	//			if (dwAllDelay >= MAX_BOSSFIGHT_TIME)
////	//			{
////	//				intMonHurt = -(int)wRoleHP;
////	//				wRoleHP = 0;
////	//				pMon += sprintf( pMon, "[%u,%u,%u,%d,%u],", monCfg->id,4,role->dwRoleID,intMonHurt,dwAllDelay);//4 is kill skill
////	//				break;
////	//			}
////
////	//			dwTempAllDelay += dwOneDelay;
////	//			size_t count = dwTempAllDelay/3000;
////
////	//			for (size_t i = count; i > 0; --i)
////	//			{
////	//				getMonAtkValue( role, monCfg, monSkillCfg, intMonHurt);
////	//						
////	//				pMon += sprintf( pMon, "[%u,%u,%u,%d,%u],", monCfg->id,monSkillCfg->id,role->dwRoleID,intMonHurt, (dwAllDelay - i*3000));
////
////	//				if (intMonHurt > 0 )
////	//				{
////	//					wRoleHP -= (WORD)intMonHurt;
////	//				}
////	//				dwTempAllDelay -= 3000;
////	//			}
////	//		}
////	//		dwFightTime = dwAllDelay;
////	//	}
////	//	else//other  fight sequence
////	//	{
////	//		while ( dwAllDelay <=dwFightTime && SkillsInfo.size() > 0 && _bossFightHP > 0)
////	//		{
////	//			getFightSkill(SkillsInfo);
////
////	//			comID = monCfg->id;//治疗的时候可能会变成自己
////	//			getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//			p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////
////	//			dwOneDelay = SkillsInfo[0].coolLeft;
////	//			dwAllDelay += dwOneDelay;
////
////	//			for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//			{
////	//				if (i == 0)//使用的
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//				}
////	//				else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//				{
////	//					SkillsInfo[i].coolLeft = 0;
////	//				}
////	//				else
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//				}
////	//			}
////	//		}
////
////	//	}
////	//	*(--p) = 0;
////	//	p += sprintf( p, "]},");
////	//}
////	//*(--p) = 0;
////	//p += sprintf( p, "],");
////	//*(--pMon) = 0;
////	//pMon = xxxMon;
////	//p += sprintf( p, "\"mf\":[{\"q\":[%s]}]",pMon);
////	//p += sprintf( p, "\"f\":%u}",byFinal);
////	//logplan("bossfight:\n %s",xxx);
////
////	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_FIGHT, xxx) );
////	//return 0;
////}
//
/////*
////@boss战 鼓舞#define	    S_ENHANCE_BOSS_FIGHT		0x0803		//
////*/
////int ACTIVITY::enhanceBossFightActtivity( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	//WORD wID;
////	//if ( !BASE::parseWORD( data, dataLen, wID) )
////	//	return -1;
////
////	BYTE byRet = ACTIVITY_SUCCESS;
////	string strData;
////
////	if (role->byEnhanceTime >= 10)
////	{
////		byRet = BOSSFIGHT_ENHANCE_ENOUGH;
////	}
////	else if (role->dwIngot < 20)
////	{
////		byRet = COLD_NOT_ENOUGH;
////	}
////	else
////	{
////		role->byEnhanceTime++;
////	}
////	
////	S_APPEND_BYTE( strData, byRet);
////	S_APPEND_WORD( strData, (WORD)role->byEnhanceTime * 10);
////	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_ENHANCE_BOSS_FIGHT, strData));
////
////	return 0;
////}
//
//*
//@#define	    S_BOSS_TIME_LEFT		0x0804		//获取活动开始否
//*/
int ACTIVITY::getActtivityLeftTime( ROLE* role, unsigned char * data, size_t dataLen )
{
	BYTE byRet;
	WORD actID;
	string strData;
	if ( !BASE::parseWORD( data, dataLen, actID) )
		return 0;
	CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg(actID);
	if ( activityCfg == NULL )
	{
		byRet = ACTIVITY_NOT_OPEN;
		S_APPEND_BYTE( strData, byRet );
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_BOSS_TIME_LEFT, strData));
		return 0;
	}

	DWORD nowSecond = getTodayNowSecond( time(nullptr) );

	if (nowSecond < activityCfg->advance_time || nowSecond >= activityCfg->end_time)//还未开放
	{
		byRet = ACTIVITY_NOT_OPEN;
	    S_APPEND_BYTE( strData, byRet );
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_BOSS_TIME_LEFT, strData));
	}
	else if (nowSecond < activityCfg->begin_time)//可以报名
	{
		//role->wBossFightActivityID = actID;
		byRet = ACTIVITY_WAITING;
		S_APPEND_BYTE( strData, byRet );
		S_APPEND_DWORD( strData, activityCfg->begin_time - nowSecond);
		S_APPEND_WORD( strData, (WORD)role->byEnhanceTime * 10);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_BOSS_TIME_LEFT, strData));
	}
	else if (nowSecond < activityCfg->end_time)//可以参加
	{
		//role->wBossFightActivityID = actID;
		byRet = ACTIVITY_SUCCESS;
		S_APPEND_BYTE( strData, byRet );
		S_APPEND_WORD( strData, (WORD)role->byEnhanceTime * 10);
		PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_BOSS_TIME_LEFT, strData));
	}

	return 0;
}
//
//
//////new define	    S_BOSS_FIGHT				0x0802		//Boss战斗
////int ACTIVITY::clientBossFight2( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	rapidjson::Document document;
////	if (document.Parse<0>( string( (char*)data, dataLen ).c_str() ).HasParseError())
////		return 0;
////
////	int nTag = JSON::getJsonValueInt( document, "tag" );
////	if (nTag <= 0)
////	{
////		logwm("5nTag = %d",nTag);
////		//return 0;
////	}
////	return getBossFightHelperMe( role, nTag );
////}
//
//////new S_BOSS_GET_HELPER			0x0805		//补人
////int ACTIVITY::getBossFightHelper2( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	rapidjson::Document document;
////	if (document.Parse<0>( string( (char*)data, dataLen ).c_str() ).HasParseError())
////		return 0;
////
////	int nTag = JSON::getJsonValueInt( document, "tag" );
////	int nPos = JSON::getJsonValueInt( document, "pos" );
////	if ( nTag <= 0 )
////	{
////		logwm("helper error nTag = %d",nTag);
////		return 0;
////	}
////	//if ( nPos <= 0 || (WORD)nPos > role->vecBossFightHelpers.size() )
////	//{
////	//	logwm("helper error nPos = %d",nPos);
////	//	return 0;
////	//}
////
////
////	BYTE job;
////	switch (nPos)
////	{
////	case 1:
////	case 2:
////		job = E_JOB_WARRIOR;break;
////	case 3:
////	case 4:
////		job = E_JOB_MAGIC;break;
////	case 5:
////	case 6:
////		job = E_JOB_MONK;break;
////	default:
////		return 0;
////	}
////
////	getArmyByJob(role, job);
////
////	DWORD helpID = getOneHelpByJob(role, job, nPos);
////	if (helpID != 0)
////	{
////		role->vecBossFightHelpers[ nPos - 1 ] = helpID;
////		getBossFightHelperOther( role, helpID, nTag);
////	}
////	
////	return 0;
////}
//
//
//
/////*
////@old define	    S_BOSS_GET_HELPER		0x0805		//补人
////*/
////int ACTIVITY::getBossFightHelper( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	rapidjson::Document document;
////	if (document.Parse<0>( string( (char*)data, dataLen ).c_str() ).HasParseError())
////		return 0;
////	DWORD dwHelpID;
////	if ( ( dwHelpID = JSON::getJsonValueInt( document, "id" )) <= 0 )
////	{
////		logwm("helper: %u",dwHelpID);
////		return 0;
////	}
////
////	if (role->dwRoleID == dwHelpID)//补自己
////	{
////		getBossFightHelperMe( role );
////	}
////	else
////	{
////		getBossFightHelperOther( role, dwHelpID );
////	}
////	return 0;
////	//BYTE byJob;
////	//if ( !BASE::parseBYTE( data, dataLen, byJob) )
////	//	return -1;
////	//switch (byJob)
////	//{
////	//case E_JOB_WARRIOR:
////	//	getRandIDs( role, _dequeWarriors, 1, vecArmy);break;
////	//case E_JOB_MAGIC:
////	//	getRandIDs( role, _dequeMagics, 1, vecArmy);break;
////	//case E_JOB_MONK:
////	//	getRandIDs( role,_dequeMonks, 1, vecArmy);break;
////	//default: return 0;
////	//}
////
////	//WORD actID = role->wBossFightActivityID;
////	//CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );
////	//if ( activityCfg == NULL )
////	//{
////	//	return 0;
////	//}
////
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(activityCfg->monster_id);
////	//if ( monCfg == NULL )
////	//{
////	//	return 0;
////	//}
////
////	//BYTE byFinal = 0;
////	//BYTE byRet = 0;
////	//DWORD monFightTime = 0;
////	//char xxx[2048]={0};
////	//char* p = xxx;
////	//if (_bossFightHP == 0)
////	//{
////	//	byRet = ACTIVITY_BOSS_DIE;
////	//}
////
////	//ROLE* comRole = RoleMgr::getMe().getRoleByID( dwRoleID );
////	//if (comRole == NULL)
////	//{
////	//	return 0;
////	//}
////
////	//WORD cloth = 0;
////	//WORD weapon = 0;
////	//WORD lucky = 0;
////	//int intHurt;
////	//DWORD dwAllDelay = 0;
////	//DWORD dwOneDelay = 0;
////	//DWORD comID;
////	//vector<SKILL_INFO> SkillsInfo;
////	//setClothWeaponSkillLucky( comRole, cloth,  weapon,  lucky, SkillsInfo );
////	//p += sprintf( p, "{\"error\":%u,", byRet );
////	//p += sprintf( p, "\"member\":{");
////	//p += sprintf( p, "\"w\":%u,", weapon );
////	//p += sprintf( p, "\"c\":%u,", cloth );
////	//p += sprintf( p, "\"j\":%u,", comRole->byJob );
////	//p += sprintf( p, "\"s\":%u,", comRole->bySex );
////	//p += sprintf( p, "\"id\":%u,", comRole->dwRoleID );
////	//p += sprintf( p, "\"q\":[");
////
////	//while ( dwAllDelay <=MAX_BOSSFIGHT_TIME && SkillsInfo.size() > 0 && _bossFightHP>0 )
////	//{
////	//	getFightSkill(SkillsInfo);
////	//	comID = monCfg->id;//治疗的时候可能会变成自己
////	//	getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//	p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////	//	dwOneDelay = SkillsInfo[0].coolLeft;
////	//	dwAllDelay += dwOneDelay;
////	//	for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//	{
////	//		if (i == 0)
////	//		{
////	//			SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//		}
////	//		else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//		{
////	//			SkillsInfo[i].coolLeft = 0;
////	//		}
////	//		else
////	//		{
////	//			SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//		}
////	//	}
////	//}
////	//*(--p) = 0;
////	//p += sprintf( p, "]}}");
////	//logplan("buren:  %s",xxx);
////	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//return 0;
////}
//
/////*
////@//补自己 相当于重新战斗
////*/
////int ACTIVITY::getBossFightHelperMe( ROLE* role, int nTag )
////{
////	BYTE byRet ;
////	string strData;
////	DWORD nowSecond;
////	CONFIG::ACTIVITY_INFO_CFG* activityCfg;
////	vector<DWORD> vecArmy;
////	char xxx[1024];
////	if ( _bossFightStatus != E_NOTICE_STATUS_START )//未开始
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		goto end;
////	}
////
////	activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );
////	if ( activityCfg == NULL || role->wLevel < activityCfg->open_level)
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		goto end;
////	}
////
////	nowSecond = getTodayNowSecond( time(nullptr) );
////
////	if( nowSecond <= role->dwFinalWorldBossTime + MAX_BOSSFIGHT_TIME/1000 )
////	{
////		byRet = NO_FIGHT_COLD_ENOUGH;
////		goto end;
////	}
////	if (nowSecond < activityCfg->begin_time || nowSecond >= activityCfg->end_time)//不在战斗时间区
////	{
////		byRet = ACTIVITY_NOT_OPEN;
////		goto end;
////	}
////	if ( _bossFightHP == 0 )
////	{
////		byRet = ACTIVITY_BOSS_DIE;
////		goto end;
////	}
////
////	vecArmy.push_back(role->dwRoleID);
////	clientBossFightInfos( role, vecArmy, nTag);
////	RoleMgr::getMe().judgeCompleteTypeTask(role, E_TASK_WORLD_BOSS);
////	//initHelpers( role );//找伙伴
////	insertBossfightArmy( role );
////	sortHurtRank();
////	return 0;
////
////end:
////	//sprintf(xxx, "{\"error\":%u}", byRet );
////	sprintf(xxx, "{ \"tag\":%d,\"error\": %u }", nTag, byRet );
////	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_FIGHT, xxx) );
////	return 0;
////}
/////*
////@//补其他人
////*/
////int ACTIVITY::getBossFightHelperOther( ROLE* role, DWORD& helperID, int nTag)
////{
////
////	//BYTE byRet = 0;
////	//char xxx[20480]={0};
////	//CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );
////	//if ( activityCfg == NULL  || role->wLevel < activityCfg->open_level)
////	//{
////	//	byRet = ACTIVITY_NO_EXSIST;
////	//	sprintf( xxx, "{\"tag\":%d,\"error\":%u}", nTag, byRet );
////	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//	return 0;
////	//}
////
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg(activityCfg->monster_id);
////	//if ( monCfg == NULL )
////	//{
////	//	byRet = ACTIVITY_NO_EXSIST;
////	//	sprintf( xxx, "{\"tag\":%d,\"error\":%u}", nTag, byRet );
////	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//	return 0;
////	//}
////
////	//BYTE byFinal = 0;
////	//
////	//DWORD monFightTime = 0;
////
////	//char* p = xxx;
////	//if (_bossFightHP == 0)
////	//{
////	//	byRet = ACTIVITY_BOSS_DIE;
////	//	sprintf( xxx, "{\"tag\":%d,\"error\":%u}", nTag, byRet );
////	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//	return 0;
////	//}
////
////	//ROLE* comRole = RoleMgr::getMe().getRoleByID( helperID );
////	//if (comRole == NULL)
////	//{
////	//	byRet = ACTIVITY_NO_EXSIST;
////	//	sprintf( xxx, "{\"tag\":%d,\"error\":%u}", nTag, byRet );
////	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	//	return 0;
////	//}
////
////	//WORD cloth = 0;
////	//WORD weapon = 0;
////	//WORD lucky = 0;
////	//int intHurt;
////	//DWORD dwAllDelay = 0;
////	//DWORD dwOneDelay = 0;
////	//DWORD comID;
////	//vector<SKILL_INFO> SkillsInfo;
////	//setClothWeaponSkillLucky( comRole, cloth,  weapon,  lucky, SkillsInfo );
////	//p += sprintf( p, "{ \"tag\":%d,\"error\":%u,", nTag, byRet );
////	//p += sprintf( p, "\"member\":{");
////	//p += sprintf( p, "\"n\":\"%s\",", comRole->roleName.c_str());
////	//p += sprintf( p, "\"w\":%u,", weapon );
////	//p += sprintf( p, "\"c\":%u,", cloth );
////	//p += sprintf( p, "\"j\":%u,", comRole->byJob );
////	//p += sprintf( p, "\"s\":%u,", comRole->bySex );
////	//p += sprintf( p, "\"id\":%u,", comRole->dwRoleID );
////	//p += sprintf( p, "\"q\":[");
////
////	//while ( dwAllDelay <=MAX_BOSSFIGHT_TIME && SkillsInfo.size() > 0 && _bossFightHP>0 )
////	//{
////	//	getFightSkill(SkillsInfo);
////	//	comID = monCfg->id;//治疗的时候可能会变成自己
////	//	getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//	dwOneDelay = SkillsInfo[0].coolLeft;
////	//	dwAllDelay += dwOneDelay;
////	//	p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////	//	for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//	{
////	//		if (i == 0)
////	//		{
////	//			SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//		}
////	//		else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//		{
////	//			SkillsInfo[i].coolLeft = 0;
////	//		}
////	//		else
////	//		{
////	//			SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//		}
////	//	}
////	//}
////	//*(--p) = 0;
////	//p += sprintf( p, "]}}");
////	//logwm("buren:\n  %s",xxx);
////	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HELPER, xxx) );
////	return 0;
////
////}
//
/////*
////@战斗序列信息
////*/
////int ACTIVITY::clientBossFightInfos( ROLE* role, vector<DWORD>& vecArmy, int nTag )
////{
////	//if (role  == NULL)//到点通知用
////	//{
////	//	return 0;
////	//}
////	//if (role->client == NULL)//到点通知用
////	//{
////	//	return 0;
////	//}
////
////	//CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( _bossFightID );
////	//if ( monCfg == NULL )
////	//{
////	//	return 0;
////	//}
////
////	//CONFIG::SKILL_CFG2* monSkillCfg = CONFIG::getSkillCfg(monCfg->skill_id);
////	//if (monSkillCfg == NULL)
////	//{
////	//	return 0;
////	//}
////
////	//BYTE byFinal = 0;
////	//BYTE byRet = 0;
////	//DWORD dwFightTime = 0;
////	//DWORD dwRoleHP = role->roleAttr[E_ATTR_HP_MAX2];
////	//char xxx[1024*20]={0};
////	//char xxxMon[10240]={0};
////	//char* p = xxx;
////	//char* pMon = xxxMon;
////	//if (_bossFightHP == 0)
////	//{
////	//	byRet = ACTIVITY_BOSS_DIE;
////	//}
////
////	////p += sprintf( p, "{\"error\":%u,", byRet );
////	//p += sprintf(p, "{\"tag\":%d,\"error\":%u",nTag, byRet );
////	//p += sprintf( p, "\"m\":%u,", monCfg->id );
////	//p += sprintf( p, "\"mb\":%u,", _bossFightHP );
////	//p += sprintf( p, "\"hf\":[ ");
////	//for (size_t roleIndex = 0; roleIndex < vecArmy.size(); ++roleIndex)
////	//{
////	//	DWORD id = vecArmy[roleIndex];
////
////	//	ROLE* comRole = RoleMgr::getMe().getRoleByID( id );
////	//	if (comRole == NULL)
////	//	{
////	//		continue;
////	//	}
////
////	//	WORD cloth = 0;
////	//	WORD weapon = 0;
////	//	WORD lucky = 0;
////	//	DWORD dwAllDelay = 0;
////	//	DWORD dwOneDelay = 0;
////	//	DWORD comID;
////	//	int intHurt;
////	//	vector<SKILL_INFO> SkillsInfo;
////	//	setClothWeaponSkillLucky( comRole, cloth,  weapon,  lucky, SkillsInfo );
////
////	//	p += sprintf( p, "{");
////	//	p += sprintf( p, "\"w\":%u,", weapon );
////	//	p += sprintf( p, "\"c\":%u,", cloth );
////	//	p += sprintf( p, "\"j\":%u,", comRole->byJob );
////	//	p += sprintf( p, "\"s\":%u,", comRole->bySex );
////	//	p += sprintf( p, "\"id\":%u,", comRole->dwRoleID );
////	//	p += sprintf( p, "\"q\":[ ");
////
////	//	if (roleIndex == 0 )//self
////	//	{
////	//		DWORD dwTempAllDelay = 0;
////	//		int intMonHurt = 0;
////
////	//		while (dwRoleHP > 0 && _bossFightHP > 0 && dwAllDelay <=MAX_BOSSFIGHT_TIME && SkillsInfo.size() > 0 )
////	//		{
////	//			//self fight sequence
////	//			getFightSkill(SkillsInfo);
////	//			comID = monCfg->id;//治疗的时候可能会变成自己
////	//			getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//			
////	//			dwOneDelay = SkillsInfo[0].coolLeft;
////	//			dwAllDelay += dwOneDelay;
////	//			p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);
////
////	//			
////	//			for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//			{
////	//				if (i == 0)//使用的
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//				}
////	//				else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//				{
////	//					SkillsInfo[i].coolLeft = 0;
////	//				}
////	//				else
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//				}
////	//			}
////
////	//			if (comID == monCfg->id && intHurt < 0)//role hurt monster
////	//			{
////	//				if ( _bossFightHP <= (DWORD)(-intHurt) )
////	//				{
////	//					_bossFightHP = 0;//怪物被打死 记下  dwAllDelay
////	//					byFinal = 1;
////	//					_lastFightID = role->dwRoleID;
////	//				}
////	//				else
////	//				{
////	//					_bossFightHP -= (DWORD)(-intHurt);
////	//				}
////	//
////	//				countBossFightHurt( role, -(intHurt) );
////	//			}
////
////	//			//moster fight sequence
////	//			if (dwAllDelay >= MAX_BOSSFIGHT_TIME)
////	//			{
////	//				intMonHurt = -(int)dwRoleHP;
////	//				dwRoleHP = 0;
////	//				pMon += sprintf( pMon, "[%u,%u,%u,%d,%u],", monCfg->id,4,role->dwRoleID,intMonHurt,dwAllDelay);//4 is kill skill
////	//				break;
////	//			}
////
////	//			dwTempAllDelay += dwOneDelay;
////	//			size_t count = dwTempAllDelay/3000;
////
////	//			for (size_t i = count; i > 0; --i)
////	//			{
////	//				getMonAtkValue( role, monCfg, monSkillCfg, intMonHurt);
////
////	//				pMon += sprintf( pMon, "[%u,%u,%u,%d,%u],", monCfg->id,monSkillCfg->id,role->dwRoleID,intMonHurt, (dwAllDelay - i*3000));
////
////	//				if (intMonHurt > 0 )
////	//				{
////	//					dwRoleHP -= (WORD)intMonHurt;
////	//				}
////	//				dwTempAllDelay -= 3000;
////	//			}
////	//		}
////	//		dwFightTime = dwAllDelay;
////	//	}
////	//	else//other  fight sequence
////	//	{
////	//		while ( dwAllDelay <=dwFightTime && SkillsInfo.size() > 0 && _bossFightHP > 0)
////	//		{
////	//			getFightSkill(SkillsInfo);
////
////	//			comID = monCfg->id;//治疗的时候可能会变成自己
////	//			getRoleAtkValue(comRole,monCfg,comID,SkillsInfo[0],intHurt,dwAllDelay);
////	//			dwOneDelay = SkillsInfo[0].coolLeft;
////	//			dwAllDelay += dwOneDelay;
////	//			p += sprintf( p, "[%u,%u,%u,%d,%u],", comRole->dwRoleID,SkillsInfo[0].wID,comID,intHurt,dwAllDelay);			
////	//			for (size_t i = 0; i < SkillsInfo.size(); i++)
////	//			{
////	//				if (i == 0)//使用的
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].cooldown;
////	//				}
////	//				else if (SkillsInfo[i].coolLeft < dwOneDelay)
////	//				{
////	//					SkillsInfo[i].coolLeft = 0;
////	//				}
////	//				else
////	//				{
////	//					SkillsInfo[i].coolLeft = SkillsInfo[i].coolLeft - dwOneDelay;
////	//				}
////	//			}
////	//		}
////
////	//	}
////	//	*(--p) = 0;
////	//	p += sprintf( p, "]},");
////	//}
////	//*(--p) = 0;
////	//p += sprintf( p, "],");
////
////	//if (pMon != xxxMon)
////	//{
////	//	*(--pMon) = 0;
////	//}
////	//
////	////pMon = xxxMon;
////	//p += sprintf( p, "\"mf\":[{\"q\":[%s]}]",xxxMon);
////	//p += sprintf( p, "\"f\":%u}",byFinal);
////	//logwm("bossfight:\n %s",xxx);
////
////	//PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_FIGHT, xxx) );
////	return 0;
////}
//
///*
//@战斗到点开始和关闭
//*/
//void ACTIVITY::startTodaySec(const time_t& curTime)
//{
//	//	E_NOTICE_STATUS_NO,
//	//	E_NOTICE_STATUS_REGISTER,
//	//	E_NOTICE_STATUS_START,
//
//	DWORD nowSecond = getTodayNowSecond( curTime );
//
//	CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );//现在只有世界boss战
//	if ( activityCfg == NULL )
//	{
//		return;
//	}
//
//	if ( _bossFightStatus == E_NOTICE_STATUS_NO && nowSecond >= activityCfg->advance_time )
//	{
//		registerBossFight(activityCfg, nowSecond);
//	}
//	
//	if ( _bossFightStatus == E_NOTICE_STATUS_REGISTER && nowSecond >= activityCfg->begin_time )
//	{
//		startBossFight(activityCfg, nowSecond);
//	}
//
//	if ( _bossFightStatus == E_NOTICE_STATUS_START && nowSecond >= activityCfg->end_time )//CONFIG:: 怪物 打死id增加1  不死id加1
//	{
//		endBossFight(activityCfg);
//	}
//}
//void ACTIVITY::notifyRoleActivityStatus( ROLE* role )
//{
//	DWORD nowSecond = getTodayNowSecond( time(nullptr) );
//
//	CONFIG::ACTIVITY_INFO_CFG* activityCfg = CONFIG::getActivityInfoCfg( WORLD_BOSS_FIGHT );//boss战
//
//	if ( _bossFightStatus == E_NOTICE_STATUS_REGISTER || _bossFightStatus == E_NOTICE_STATUS_START )
//	{
//		notifyActivityStatus( role, 2, activityCfg->id );
//	}
//}
//
//void ACTIVITY::notifyActivityStatus( ROLE* role, const BYTE& status, const WORD& actID, DWORD dwTime )
//{
//	if ( role->client == NULL )
//	{
//		return;
//	}
//	
//	string strData;
//	S_APPEND_BYTE( strData, 3 );//活动模块
//	S_APPEND_BYTE( strData, status );//该活动状态通知
//	S_APPEND_BYTE( strData, (BYTE)actID );//活动ID
//	S_APPEND_DWORD( strData, dwTime );//活动ID
//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_FUNCTION_NOTIFY, strData) );
//}
//
//void ACTIVITY::judgeNewActivity( ROLE* role, const WORD& wOldLevel )//新活动解锁
//{
//	for (auto &it : mapActivityInfoCfg)
//	{
//		if ( wOldLevel < it.second.open_level && role->wLevel >= it.second.open_level )
//		{
//			notifyActivityStatus( role, 1, it.first );
//		}
//	}
//}
//
////void ACTIVITY::initHelpers( ROLE* role )//找伙伴
////{
////	//getArmy(role, role->vecBossFightHelpers);
////	//getArmyByJob(role, role->vvecFightSourceHelpers);
////	
////	//size_t job_num = min<size_t>(role->vvecFightSourceHelpers.size(), 3);
////	//for( size_t i = 0; i < job_num; i++ )
////	//{
////	//	size_t help_num = min<size_t>(role->vvecFightSourceHelpers[i].size(), 2);
////	//	for (size_t j = 0; j < help_num; j++)
////	//	{
////	//		role->vvecFightSourceHelpers[i][j];
////	//	}
////	//}
////
////	//DWORD id = 1;
////	//while ( role->vecBossFightHelpers.size() < HELPER_NUM )
////	//{
////	//	if (role->dwRoleID != id && find(role->vecBossFightHelpers.begin(), role->vecBossFightHelpers.end(), id) == role->vecBossFightHelpers.end())
////	//	{
////	//		role->vecBossFightHelpers.push_back(id);
////	//	}
////	//	id++;
////	//}
////}
//
//void ACTIVITY::countBossFightHurt( ROLE* role, DWORD nHurt )//
//{
//	auto it = find(_vecBossHurt.begin(), _vecBossHurt.end(), role->dwRoleID);
//	if ( it == _vecBossHurt.end() )
//	{
//		_vecBossHurt.push_back( BOSSHURT(role->dwRoleID,nHurt));
//	}
//	else
//	{
//		it->dwHurt += nHurt;
//	}
//}
//
//void ACTIVITY::registerBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg, const DWORD& nowSecond )
//{
//	DWORD dwTimeLeft = activityCfg->begin_time >= nowSecond ? (activityCfg->begin_time - nowSecond) : 0;
//	ROLE * role;
//	PROTOCAL::forEachOnlineRole( [ & ] ( const std::pair<DWORD, ROLE_CLIENT> &x )
//	{
//		if ((role = x.second.role) != NULL)
//		{
//			notifyActivityStatus( role, 2, activityCfg->id, dwTimeLeft );
//		}	
//	}
//	);
//	_bossFightStatus = E_NOTICE_STATUS_REGISTER;
//}
//
//void ACTIVITY::startBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg, const DWORD& nowSecond )
//{
//	CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( _bossFightID );
//	if (monCfg == NULL)
//	{
//		return;
//	}
//	_bossFightHP = monCfg->hp_max;
//	_vecBossHurt.clear();
//	_lastFightID = 0;
//	DWORD dwTimeLeft = activityCfg->end_time >= nowSecond ? (activityCfg->end_time - nowSecond) : 0;
//	ROLE * role;
//
//	for (auto it:_dequeWarriors)
//	{
//		role = RoleMgr::getMe().getRoleByID(it);
//		notifyActivityStatus( role, 2, activityCfg->id, dwTimeLeft );
//	}
//	for (auto it:_dequeMagics)
//	{
//		role = RoleMgr::getMe().getRoleByID(it);
//		notifyActivityStatus( role, 2, activityCfg->id, dwTimeLeft );
//	}
//	for (auto it:_dequeMonks)
//	{
//		role = RoleMgr::getMe().getRoleByID(it);
//		notifyActivityStatus( role, 2, activityCfg->id, dwTimeLeft );
//	}
//
//	_bossFightStatus = E_NOTICE_STATUS_START;
//}
//
//void ACTIVITY::endBossFight( CONFIG::ACTIVITY_INFO_CFG* activityCfg )
//{
//	if ( _bossFightHP == 0 && CONFIG::getMonsterCfg( _bossFightID+1 ) != NULL )
//	{
//		++_bossFightID;
//	}
//	else if( CONFIG::getMonsterCfg( _bossFightID-1 ) != NULL )
//	{
//		--_bossFightID;
//	}
//
//	offerBossFightReward( );
//
//	ROLE * role;
//	PROTOCAL::forEachOnlineRole( [ & ] ( const std::pair<DWORD, ROLE_CLIENT> &x )
//	{
//		if ((role = x.second.role) != NULL)
//		{
//			notifyActivityStatus( role, 0, activityCfg->id );
//		}	
//	}
//	);
//	_bossFightStatus = E_NOTICE_STATUS_END;
//
//	_bossFightHP = 0;
//	_dequeWarriors.clear();
//	_dequeMagics.clear();
//	_dequeMonks.clear();
//	_vecBossHurt.clear();
//}
//
//#define   TOP_THREE			   3
//
//#define   LAST_HURT_REWARD	   3
//
//#define   CHEST_REWARD		   0
//#define   CHEST_KEY_REWARD	   1
//#define   HONOUR_REWARD		   2
//
//#define   REWARD_ITEM_NUMS	   3
//
//#define   JOIN_HONOUR_REWARD   30
//
//void ACTIVITY::offerBossFightReward( )
//{
//	CONFIG::MONSTER_CFG* monCfg = CONFIG::getMonsterCfg( _bossFightID );
//	if (monCfg == NULL || monCfg->hp_max < 100)
//	{
//		return;
//	}
//	CONFIG::WORLD_BOSS_REWARD_CFG* worldBossRewardCfg = CONFIG::getWorldBossRewardCfg( _bossFightID );
//	if ( worldBossRewardCfg == NULL )
//	{
//		return;
//	}
//
//	DWORD id = 0;
//	DWORD honour = 0;
//	DWORD coin = 0;
//	DWORD coinOrig  = 0;
//	ROLE* role = NULL;
//	DWORD bossHP = monCfg->hp_max;
//	size_t nums = std::min<size_t>( _vecBossHurt.size(), TOP_THREE );
//	vector<ITEM_INFO> vecItemInfo;
//	CONFIG::ITEM_CFG* itemCfg0, *itemCfg1;
//	ITEM_INFO* pVItemReward[REWARD_ITEM_NUMS];
//
//	for (size_t i = 0; i < nums; i++)//top three
//	{
//		pVItemReward[CHEST_REWARD] = &worldBossRewardCfg->vvecReward[i][CHEST_REWARD];//宝箱
//		pVItemReward[CHEST_KEY_REWARD] = &worldBossRewardCfg->vvecReward[i][CHEST_KEY_REWARD];//钥匙
//		pVItemReward[HONOUR_REWARD] = &worldBossRewardCfg->vvecReward[i][HONOUR_REWARD];//声望
//		itemCfg0 = CONFIG::getItemCfg(pVItemReward[0]->itemIndex);
//		itemCfg1 = CONFIG::getItemCfg(pVItemReward[1]->itemIndex);
//		if (itemCfg1==NULL || itemCfg1==NULL )
//		{
//			continue;
//		}
//		id = _vecBossHurt[i].dwRoleID;
//		char szMsg[256];
//		sprintf( szMsg,(CONFIG:: getMsgBossHurtRank()).c_str(), i+1, pVItemReward[HONOUR_REWARD]->itemNums,itemCfg0->name.c_str(), pVItemReward[CHEST_REWARD]->itemNums,itemCfg1->name.c_str(), pVItemReward[CHEST_KEY_REWARD]->itemNums);
//		MailCtrl::sendMail(id, E_MAIL_TYPE_PRIZE, szMsg, worldBossRewardCfg->vvecReward[i]);
//		logwm("世界boss:%s",szMsg);
//	}
//
//	if ( _lastFightID != 0 )
//	{
//		vector<ITEM_INFO>& vecLastHurtItem = worldBossRewardCfg->vvecReward[LAST_HURT_REWARD];
//		pVItemReward[CHEST_REWARD] = &vecLastHurtItem[CHEST_REWARD];//宝箱
//		pVItemReward[CHEST_KEY_REWARD] = &vecLastHurtItem[CHEST_KEY_REWARD];//钥匙
//		pVItemReward[HONOUR_REWARD] = &vecLastHurtItem[HONOUR_REWARD];//声望
//		itemCfg0 = CONFIG::getItemCfg(pVItemReward[0]->itemIndex);
//		itemCfg1 = CONFIG::getItemCfg(pVItemReward[1]->itemIndex);
//		if (itemCfg1 != NULL || itemCfg1 != NULL )
//		{
//			char szMsg[256];
//			sprintf( szMsg,(CONFIG:: getMsgBossLastHurt()).c_str(), pVItemReward[HONOUR_REWARD]->itemNums,itemCfg0->name.c_str(), pVItemReward[CHEST_REWARD]->itemNums,itemCfg1->name.c_str(), pVItemReward[CHEST_KEY_REWARD]->itemNums);//暂用
//			MailCtrl::sendMail(id, E_MAIL_TYPE_PRIZE, szMsg, vecLastHurtItem );
//					logwm("世界boss:%s",szMsg);
//		}
//	}
//
//	for ( auto it : _vecBossHurt )
//	{
//		id = it.dwRoleID;
//		role = RoleMgr::getMe().getRoleByID( id );
//		if (role == NULL)
//		{
//			continue;
//		}
//
//		vecItemInfo.clear();
//		vecItemInfo.push_back( ITEM_INFO(ITEM_ID_HONOUR, JOIN_HONOUR_REWARD ) );
//
//		if ( !vecItemInfo.empty() )
//		{
//			char szMsg[256];
//			sprintf( szMsg,(CONFIG::getMsgBossFightJoin()).c_str(), JOIN_HONOUR_REWARD );
//			MailCtrl::sendMail(id, E_MAIL_TYPE_PRIZE, szMsg, vecItemInfo);
//					logwm("世界boss:%s",szMsg);
//		}
//
//		coin = DWORD( it.dwHurt/(bossHP * 0.0001f) * (1000.0f + 2000.0f * pow(role->wLevel,2.22f) / (role->wLevel + 1030.0f)) );
//		if (coin > 0)
//		{
//			ADDUP(role->dwCoin, (int)coin);
//			notifyTypeValuebyRoleID(role->dwRoleID, ROLE_PRO_COIN, role->dwCoin);
//			vecItemInfo.clear();
//			char szMsg2[256];
//			sprintf( szMsg2,(CONFIG::getMsgBossHurt()).c_str(),it.dwHurt,coin );
//			MailCtrl::sendMail(id, E_MAIL_TYPE_MSG, szMsg2, vecItemInfo);
//					logwm("世界boss:%s",szMsg2);
//		}
//	}
//}
//
//void ACTIVITY::setBossFightStatusNO( )
//{
//	_bossFightStatus = E_NOTICE_STATUS_NO;
//}
//
/////*
////@#define	 S_BOSS_GET_HURT_RANK		0x0806		//伤害排行
////*/
////int ACTIVITY::getBossFightRank( ROLE* role, unsigned char * data, size_t dataLen )
////{
////	string strData;
////	DWORD selfRank = 0;
////	DWORD selfHurt = 0;
////	auto it = find( _vecBossHurt.begin(), _vecBossHurt.end(), role->dwRoleID );//bool opeartor == (const DWORD&	id)const{}
////	if ( it != _vecBossHurt.end() )
////	{
////		selfRank = it - _vecBossHurt.begin();
////		selfHurt = it->dwHurt;
////	}
////
////	S_APPEND_DWORD( strData, selfRank);
////	S_APPEND_DWORD( strData, selfHurt);
////
////	size_t nums = min<size_t>( 5, _vecBossHurt.size() );
////	for ( size_t i = 0; i < nums; i++ )
////	{
////		ROLE* rankOther = RoleMgr::getMe().getRoleByID( _vecBossHurt[i].dwRoleID );
////		if ( rankOther != NULL )
////		{
////			S_APPEND_DWORD( strData, rankOther->dwRoleID );
////			S_APPEND_DWORD( strData, _vecBossHurt[i].dwHurt );
////			S_APPEND_BYTE( strData, rankOther->bySex );
////			S_APPEND_BYTE( strData, rankOther->byJob );
////			S_APPEND_BSTR( strData, rankOther->roleName );
////		}
////	}
////
////	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_BOSS_GET_HURT_RANK, strData) );
////	return 0;
////}