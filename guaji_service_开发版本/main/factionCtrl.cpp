#include "pch.h"
#include "log.h"
#include "base.h"
#include "protocal.h"
#include "db.h"
#include "zlibapi.h"
#include "roleCtrl.h"
#include "itemCtrl.h"
#include "broadCast.h"
#include "factionCtrl.h"
#include "logs_db.h"

map<unsigned, guild*> guild::guilds_id_index;
map<string, guild*> guild::guilds_name_index;
map<unsigned, guild*> guild::guilds_roleid_index;
vector<guild*> guild::guilds;
list<guild*> guild::sorted_guilds;
bool guild::_needSortFaction = false;
const static uint _max_cost_times = 5;
static uint16 _wordship_cost[_max_cost_times] = { 0, 10, 20, 50, 75 };

void update_member(guild_member* member)
{
	EXEC_SQL("update " TABLE_FACTION_MEMBER  " set facjob=%u, contribute=%u, contribute_count=%u where roleid=%u ",
		member->job, member->contribute, member->contribute_count, member->roleid);
}



void update_guild(guild* source)
{
	MYSQL_ESCAPE_CPP(szNotify, source->notify);
	EXEC_SQL("update " TABLE_FACTION  " set level=%u, contribute=%u, board='%s', exp=%u, shrine_level=%u, shrine_exp=%u,wordship_time where id=%u ",
		source->level, source->contribute, szNotify, source->exp, source->shrine_level, source->shrine_exp,source->wordship_shrine_times, source->id);
}

void guild::daily_reset_guilds()
{
	for (auto it : guilds)
	{
		if (it != nullptr)
		{
			it->wordship_shrine_times = 0;
		}
	}
}

void guild::save_guilds()
{
	for (auto it : guilds)
	{
		if (it != nullptr && it->need_save)
		{
			update_guild(it);
			it->need_save = false;
		}
	}
}

guild::guild(const string& strFactionName)//, const string& leader_name, unsigned leader_roleid)
{
	name = strFactionName;
	//	this->leader_name = leader_name;

	rank = 1;
}


void guild::addChat(ROLE* role, const string& strContent)
{
	if (role->faction == nullptr)
		return;
	auto member = role->faction->get_member(role->dwRoleID);
	if (member == nullptr)
		return;
	chat chat;
	chat.time = time(nullptr);
	chat.job = member->job;
	chat.roleid = role->dwRoleID;
	chat.name = role->roleName;
	chat.content = strContent;

	chats.push_back(chat);

	if (chats.size() > 30)
	{
		chats.pop_front();
	}
}


void guild::add_member(ROLE* role)
{
	guild_member_ptr temp = new guild_member();
	temp->roleid = role->dwRoleID;
	temp->name = role->roleName;
	temp->vocation = role->byJob;
	temp->level = role->wLevel;
	members.insert(make_pair(role->dwRoleID, temp));
	sorted_members.push_back( temp );
	temp->contribute_rank = sorted_members.size();

	if (!EXEC_SQL("insert into " TABLE_FACTION_MEMBER "(roleid, factionid, facjob) values(%u,  %u, %u)",
		role->dwRoleID, id, temp->job))
	{
		logFATAL("roleid(%u) can not insert into " TABLE_FACTION_MEMBER " guild id:%u", role->dwRoleID, id);
	}

	guilds_roleid_index[role->dwRoleID] = this;
}


void guild::remove_member(guild_member_ptr member)
{
	members.erase(member->roleid);
	sorted_members.remove(member);
	EXEC_SQL("delete from " TABLE_FACTION_MEMBER  " where roleid=%u ", member->roleid);
	delete member;
}


unsigned guild::get_max_member()
{
	//CONFIG::GUILD_BUILD_CFG* builderCfg = getGuildBuildCfg2(E_BUILDER_TYPE_MAIN, builderLevel[E_BUILDER_TYPE_MAIN - 1]);
	//if (builderCfg == NULL)
	//	return 0;
	//return builderCfg->union_para.one_value;
	auto info = guild::get_level_info(level);
	if (info == nullptr)
		return 0;
	else
		return info->max_member;
}


guild_member_ptr guild::get_member(unsigned roleid)
{
	auto it = members.find(roleid);
	return (it != members.end()) ? it->second : nullptr;
}


size_t guild::get_member_count()
{
	return members.size();
}

void guild::shrine_wordship(ROLE* role)
{
	enum result : byte
	{
		successful,
		not_enough_gold,// 没有足够元宝
		not_enough_times,//次数不足
	};

	auto member = get_member(role->dwRoleID);
	if (member == nullptr)
	{
		return;
	}

	result ret = successful;
	DWORD add_shrine_exp = 0;
	do
	{
		CONFIG::VIP_ADDITION_CFG* vip_cfg = CONFIG::getVipAddCfg(role->getVipLevel());
		COND_BREAK(vip_cfg == NULL || vip_cfg->worship_time < role->shrine_times, ret, not_enough_times);

		uint index = role->shrine_times;
		uint cost_gold = index <= (_max_cost_times - 1) ? _wordship_cost[index] : _wordship_cost[_max_cost_times - 1];
		COND_BREAK(role->dwIngot < cost_gold, ret, not_enough_times);
		int add_guild_exp = 5;
		int add_person_guild_contribute = 5;
		CONFIG::guild_shrine* shrine_cfg = CONFIG::get_guild_shrine_cfg(this->shrine_level);
		if (shrine_cfg != nullptr  && shrine_cfg->time >= this->wordship_shrine_times )
		{
			DWORD rand = BASE::randTo(100);
			if (rand <= 55)
			{
				add_shrine_exp = 10;
			}
			else if (rand <= 85)
			{
				add_shrine_exp = 20;
			}
			else
			{
				add_shrine_exp = 30;
			}
			CONFIG::addExp(this->shrine_level, this->shrine_exp, add_shrine_exp, CONFIG::get_guild_shrine_max_level(), CONFIG::get_guild_shrine_upgrade_exp);
			this->need_save = true;
		}
		ADDUP(role->dwIngot, -int(cost_gold));
		ADDUP(role->shrine_times, 1);
		ADDUP(this->wordship_shrine_times, 1);
		ADDUP(this->exp, add_guild_exp);
		ADDUP(member->contribute, add_person_guild_contribute);
		ADDUP(contribute, add_person_guild_contribute);
		contribute_up(member);
		notifyIngot(role);

	} while (0);

	output_stream notify(S_GUILD_WORSHIP);
	notify.write_byte(ret);
	notify.write_int(add_shrine_exp);
	role->send(notify);
}

float guild::get_shrine_addcoin_coff(ROLE* role)
{
	if (role->faction != nullptr )
	{
		CONFIG::guild_shrine* shrine_cfg = CONFIG::get_guild_shrine_cfg(role->faction->shrine_level);
		if (shrine_cfg != nullptr )
		{
			return shrine_cfg->percent;
		}
	}
	return 0.0f;
}
void guild::get_wordship_info(ROLE* role)
{
	uint16 level = this->shrine_level;
	uint16 shrine_time = 0;
	uint16 person_time = 0;
	uint exp = this->shrine_exp;
	uint cost_gold = 0;

	CONFIG::guild_shrine* shrine_cfg = CONFIG::get_guild_shrine_cfg(this->shrine_level);
	if (shrine_cfg != nullptr  && shrine_cfg->time >= this->wordship_shrine_times)
	{
		shrine_time = shrine_cfg->time - this->wordship_shrine_times;
	}
	CONFIG::VIP_ADDITION_CFG* vip_cfg = CONFIG::getVipAddCfg(role->getVipLevel());
	if (vip_cfg != nullptr && vip_cfg->worship_time > role->shrine_times)
	{
		person_time = vip_cfg->worship_time - role->shrine_times;
	}
	uint index = role->shrine_times;
	cost_gold = index <= (_max_cost_times - 1) ? _wordship_cost[index] : _wordship_cost[_max_cost_times - 1];

	output_stream notify(S_GUILD_WORSHIPINFO);
	notify.write_ushort(level);
	notify.write_ushort(shrine_time);
	notify.write_ushort(person_time);
	notify.write_uint(cost_gold);
	notify.write_int(exp);
	role->send(notify);
}

void guild::do_contribute(ROLE* role, byte type)
{
	enum result : byte
	{
		successful,// 成功
		not_guild,// 无公会
		type_error,// 类型错误
		not_enough_coin,// 没有足够游戏币
		not_enough_gold,// 没有足够元宝
	};

	enum contribute_type : byte
	{
		none,// 未使用
		coin,// 游戏币
		gold,// 元宝
	};

	output_stream out(S_GUILD_CONTRIBUTE);

	auto member = get_member(role->dwRoleID);
	if (member == nullptr)
	{
		out.write_byte(result::not_guild);
		role->send(out);
		return;
	}

	auto cfg = guild::get_contribute_reward(member->contribute_count);
	if (cfg == nullptr)
	{
		out.write_byte(result::not_guild);
		role->send(out);
		return;
	}

	if (type == contribute_type::coin)
	{
		if (role->dwCoin < cfg->coin)
		{
			out.write_byte(result::not_enough_coin);
			role->send(out);
			return;
		}
		ADDUP(role->dwCoin, -(int)cfg->coin);
		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, -(int)cfg->coin, 0, purpose::guild_contribute, 0);
		member->contribute += cfg->person_contribute;
		exp += cfg->guild_exp;
		contribute += cfg->guild_exp;
		member->contribute_count++;
		notifyCoin(role);
	}
	else if (type == contribute_type::gold)
	{
		if (role->dwIngot < cfg->gold)
		{
			out.write_byte(result::not_enough_gold);
			role->send(out);
			return;
		}
		ADDUP(role->dwIngot, -(int)cfg->gold);
		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -(int)cfg->gold, purpose::guild_contribute, 0);
		member->contribute += cfg->person_contribute;
		exp += cfg->guild_exp;
		contribute += cfg->guild_exp;
		member->contribute_count++;
		notifyIngot(role);
	}
	else
	{
		out.write_byte(result::type_error);
		role->send(out);
		return;
	}

	contribute_up(member);

	output_stream notify(S_UPDATE_ROLE_DATA);
	notify.write_byte(ROLE_PRO_FACTION_CONTRIBUTE);
	notify.write_uint(member->contribute_count);
	role->send(notify);

	out.write_byte(result::successful);
	role->send(out);
}


bool guild::full()
{
	auto info = guild::get_level_info(level);
	if (info == nullptr)
	{
		logFATAL("公会出现不存在的等级");
		return true;
	}

	if (members.size() >= info->max_member)
		return true;

	return false;
}

bool compare(guild_member_ptr a, guild_member_ptr b)
{
	return (a->contribute) > (b->contribute);
}

void guild::sort_contribute()
{
	sorted_members.sort(compare);

	int i = 1;
	for (auto& item : sorted_members)
		item->contribute_rank = i++;
}


void guild::broadcast(guild* faction, output_stream& stream)
{
	for(auto& item : faction->members)
	{
		auto role= service::get_online_role(item.first);
		if (role == nullptr)
			continue;

		role->send(stream);
	}
}


void guild::add(guild* value)
{
	guilds_name_index.insert(make_pair(value->name, value));
	guilds_id_index.insert(make_pair(value->id, value));
	guilds.push_back(value);
	sorted_guilds.push_back(value);
}


void guild::remove(guild* faction)
{
	assert(faction);

	EXEC_SQL("delete from " TABLE_FACTION_MEMBER  " where factionid=%u ", faction->id);
	EXEC_SQL("delete from " TABLE_FACTION  " where id=%u ", faction->id);

	guilds_id_index.erase(faction->id);
	guilds_name_index.erase(faction->name);
	sorted_guilds.remove(faction);

	for (auto it = guilds.begin(); it != guilds.end(); ++it)
	{
		if (*it == faction)
		{
			guilds.erase(it);
			break;
		}
	}

	string strData;
	S_APPEND_BYTE(strData, ROLE_PRO_FACTION_JOB);
	S_APPEND_DWORD(strData, E_NONE);
	string strPkt = PROTOCAL::cmdPacket(S_UPDATE_ROLE_DATA, strData);

	for(auto& item : faction->members)
	{
		ROLE* role = RoleMgr::getMe().getCacheRoleByID(item.first);
		if (role)
			role->faction = nullptr;

		session* client = service::get_online_session(item.first);
		if (client == nullptr)
			return;

		PROTOCAL::sendClient(client, strPkt);
	}

	CC_SAFE_DELETE(faction);
}

guild* guild::get_guild(unsigned id)
{
	auto it = guilds_id_index.find(id);
	if (it == guilds_id_index.end())
		return NULL;

	return it->second;
}

guild* guild::get_guild(const string& name)
{
	auto it = guilds_name_index.find(name);
	if (it == guilds_name_index.end())
		return NULL;

	return it->second;
}

guild* guild::get_guild_from_roleid(unsigned roleid)
{
	auto it = guilds_roleid_index.find(roleid);
	if (it == guilds_roleid_index.end())
		return NULL;

	return it->second;
}

uint guild::get_first_leader_id()
{
	if (sorted_guilds.empty())
		return 0;

	return sorted_guilds.front()->leader_roleid;
}

void guild::contribute_up(guild_member* member)
{
	for (auto iter = sorted_guilds.rbegin(); iter != sorted_guilds.rend(); ++iter)
	{
		if (*iter == this)
		{
			auto cur = iter;
			while (++iter != sorted_guilds.rend())
			{
				if (contribute <= (*iter)->contribute)break;
				(*iter)->rank++;
				(*cur)->rank--;
				swap(*cur, *iter);
				cur = iter;
			}
			break;
		}
	}

	auto info = guild::get_level_info(level);
	if (info)
	{
		if (info->exp > 0 && exp >= info->exp)
		{
			exp = exp - info->exp;
			level++;
		}
	}

	sort_contribute();
	update_guild(this);
	update_member(member);
}

guild* guild::create(ROLE* role, const string& factionName)
{
	assert(role->faction == nullptr);

	MYSQL_ESCAPE_N(szFacName, factionName.data(), min<size_t>(factionName.size(), ROLE_NAME_LEN), ROLE_NAME_LEN);
	//	MYSQL_ESCAPE_N(szRoleName, role->roleName.data(), min<size_t>(role->roleName.size(), ROLE_NAME_LEN), ROLE_NAME_LEN );

	if (!EXEC_SQL("insert into " TABLE_FACTION "(name) values('%s')", szFacName))
		return nullptr;

	guild *faction = new guild(factionName);
	faction->id = (unsigned)mysql_insert_id(DBCtrl::getSQL());
	faction->leader_roleid = role->dwRoleID;
	faction->leader_name = role->roleName;
	faction->shrine_level = 1;
	if (!EXEC_SQL("insert into " TABLE_FACTION_MEMBER "(roleid, factionid, facjob) values(%u,  %u, %u)",
		role->dwRoleID, faction->id, E_LEADER))
	{
		CC_SAFE_DELETE(faction);
		return nullptr;
	}

	guild_member_ptr fm = new guild_member();
	fm->roleid = role->dwRoleID;
	fm->name = role->roleName;
	fm->job = E_LEADER;
	fm->vocation = role->byJob;
	fm->level = role->wLevel;
	faction->members[role->dwRoleID] = fm;
	faction->sorted_members.push_back(fm);

	faction->rank = guilds_id_index.size() + 1;
	add(faction);

	role->faction = faction;
	return faction;
}


void guild::load()
{

	SQLSelect sqlx(DBCtrl::getSQL(), "select id, name,contribute,level,board,exp,shrine_level,shrine_exp,wordship_time from " TABLE_FACTION " order by contribute desc");

	while (MYSQL_ROW row = sqlx.getRow())
	{
		guild *faction = new guild(row[1]);//, row[2], ATOUL(row[3]) );
		faction->id = ATOUL(row[0]);
		faction->contribute = ATOUL(row[2]);
		faction->level = ATOUL(row[3]);
		faction->notify = row[4];
		faction->exp = ATOUL(row[5]);
		faction->shrine_level = ATOW(row[6]);
		if (faction->shrine_level == 0)
		{
			faction->shrine_level = 1;
		}
		faction->shrine_exp = ATOUL(row[7]);
		faction->wordship_shrine_times = ATOW(row[7]);
		faction->rank = guilds_id_index.size() + 1;
		guilds_name_index.insert(make_pair(faction->name, faction));
		guilds_id_index.insert(make_pair(faction->id, faction));
		guilds.push_back(faction);
		sorted_guilds.push_back(faction);
	}

	{
		SQLSelect sqlx(DBCtrl::getSQL(), " select roleid, rolename, job, level, factionid, facjob, contribute from " TABLE_FACTION_MEMBER
			" left join roleinfo on factionmember.roleid = roleinfo.id ");

		while (MYSQL_ROW row = sqlx.getRow())
		{
			unsigned id = ATOUL(row[4]);

			auto it = guilds_id_index.find(id);
			if (it == guilds_id_index.end())
			{
				continue;
			}

			guild *faction = it->second;

			guild_member_ptr fm =new guild_member();

			fm->roleid = ATOUL(row[0]);
			fm->name = row[1];
			fm->vocation = ATOB(row[2]);
			fm->level = ATOW(row[3]);
			fm->job = (E_FACTION_JOB)ATOUL(row[5]);
			fm->contribute = ATOUL(row[6]);

			if (fm->job == E_LEADER)
			{
				faction->leader_roleid = fm->roleid;
				faction->leader_name = fm->name;
			}
			else if (fm->job == E_LEADER2)
			{
				faction->leader2_roleid = fm->roleid;
				faction->leader2_name = fm->name;
			}
			else if (fm->job == E_GUADIAN_LEFT)
			{
				faction->admin1_roleid = fm->roleid;
				faction->admin1_name = fm->name;
			}
			else if (fm->job == E_GUADIAN_RIGHT)
			{
				faction->admin2_roleid = fm->roleid;
				faction->admin2_name = fm->name;
			}

			faction->members.insert(make_pair(fm->roleid, fm));
			faction->sorted_members.push_back(fm);
			guilds_roleid_index[fm->roleid] = faction;
		}
	}

	list<guild*> empty_guilds;
	// 公会成员贡献排名，清理空公会
	for (auto& it : guilds_id_index)
	{
		if (it.second->get_member_count() == 0)
			empty_guilds.push_back(it.second);
		else
			it.second->sort_contribute();
	}

	for (auto item : empty_guilds)
	{
		//logINFO("remove empty guild:(%s, %u, %u, %u)", item->name.c_str(), item->contribute, item->exp, item->id);
		//remove(item);
		guilds_id_index.erase(item->id);
		guilds_name_index.erase(item->name);
		sorted_guilds.remove(item);
	}
}


int guild::rpc_get_recommend_list(ROLE* role, unsigned char * data, size_t dataLen)
{
	output_stream out(S_GUILD_LIST);
	if (guilds.size() <= 15)
	{
		for (auto item : guilds)
		{
			guild* faction = item;
			out.write_uint(faction->id);
			out.write_string(faction->name);
			out.write_byte((byte)(faction->level + 1));
			out.write_ushort(faction->members.size());
			out.write_ushort((uint16_t)faction->get_max_member());
		}
	}
	else
	{
		set<guild*> sets;
		int count = 15;
		while(count > 0)
		{
			auto guild = guilds[BASE::randTo(guilds.size())];
			if (sets.count(guild) > 0)continue;
			out.write_uint(guild->id);
			out.write_string(guild->name);
			out.write_byte((byte)(guild->level + 1));
			out.write_ushort(guild->members.size());
			out.write_ushort((uint16_t)guild->get_max_member());
			count--;
		}
	}


	role->send(out);
	return 0;
}



int guild::rpc_get_rank_list(ROLE* role, unsigned char * data, size_t dataLen)
{
	size_t nums = min(sorted_guilds.size(), 20u);

	output_stream out(S_GET_GUILD_RANK_LIST);
	auto iter = sorted_guilds.begin();
	for (size_t i = 0; i < nums && iter != sorted_guilds.end(); i++, ++iter)
	{
		guild* faction = *iter;
		if (faction == nullptr)
		{
			cout << "faction == nullptr" << endl;
			continue;
		}
		if (faction->get_member_count() == 0)
		{
			cout << time(nullptr) << " - empty guild." << endl;
			continue;
		}
		out.write_uint(faction->id);
		out.write_byte(i + 1);
		out.write_string(faction->name);
		out.write_uint(faction->contribute);
		out.write_byte((byte)(faction->level + 1));
		out.write_string(faction->leader_name);
	}

	role->send(out);
	return 0;
}



int guild::rpc_exchange(ROLE* role, input_stream& stream)
{
	enum result : byte
	{
		successful,// 成功
		error,// 贡献不足
	};

	uint16 id;
	if (!stream.read_ushort(id))
		return 0;

	struct pr
	{
		uint itemid;
		uint count;
		uint consume;
		uint level;
	}def[]=
	{
		{0, 0, 0, 0},
		{401, 1, 5, 1},
		{400, 1, 10, 2},
		{500, 1, 15, 3},
		{902, 1, 50, 4},
		{501, 1, 30, 5},
		{502, 1, 60, 6},
		{403, 1, 100, 7},
	};

	output_stream out(S_GUILD_SIGEXCHANGE);
	if (id == 0 || id > sizeof(def) / sizeof(pr))
	{
		out.write_byte(result::error);
		role->send(out);
		return 0;
	}

	auto faction = role->faction;
	if (faction == nullptr)
	{
		out.write_byte(result::error);
		role->send(out);
		return 0;
	}

	if ((faction->level + 1) < def[id].level)
	{
		out.write_byte(result::error);
		role->send(out);
		return 0;
	}

	auto member = faction->get_member(role->dwRoleID);
	if (member == nullptr)
	{
		out.write_byte(result::error);
		role->send(out);
		return 0;
	}

	if (member->contribute < def[id].consume)
	{
		out.write_byte(result::error);
		role->send(out);
		return 0;
	}

	out.write_byte(result::successful);
	role->reward_item(def[id].itemid, def[id].count);
	ADDUP(member->contribute, -(int)def[id].consume);
	faction->contribute_up(member);

	role->send(out);
	return 0;
}


//创建行会
int guild::rpc_create(ROLE* role, unsigned char * data, size_t dataLen)
{
	enum result : byte
	{
		successful,// 成功
		exist,// 已存在
		name_empty,// 名字为空
		name_too_long,// 名字太长
		in_guild,// 在公会中不能创建公会
		failed,// 失败
		not_enough_money,// 元宝不足
		not_enough_vip,// VIP等级不足
	};
	string  strFactionName;
	if (!BASE::parseBSTR(data, dataLen, strFactionName))
		return 0;

	string strData;
	BYTE errCode = 0;

	do
	{
		COND_BREAK(role->faction != NULL, errCode, result::in_guild);
		COND_BREAK(role->wVipLevel < 2, errCode, result::not_enough_vip);
		COND_BREAK(strFactionName.empty(), errCode, result::name_empty);
		COND_BREAK(strFactionName.size() > ROLE_NAME_LEN - 1, errCode, result::name_too_long);
		COND_BREAK(guilds_name_index.find(strFactionName) != guilds_name_index.end(), errCode, result::exist);
		COND_BREAK(role->dwIngot < 500, errCode, result::not_enough_money);
		guild* faction = create(role, strFactionName);
		COND_BREAK(faction == NULL, errCode, result::failed);
		guilds_roleid_index[role->dwRoleID] = faction;
		output_stream out(S_UPDATE_ROLE_DATA);
		out.write_byte(ROLE_PRO_FACTION);
		out.write_uint(faction->id);
		out.write_byte(ROLE_PRO_FACTION_JOB);
		out.write_uint(E_LEADER);
		role->send(out);
	} while (0);

	S_APPEND_BYTE(strData, errCode);
	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_GUILD_create, strData));


	if (errCode == 0)
	{
		ADDUP(role->dwIngot, -500);
		logs_db_thread::singleton().consume(role->roleName, role->dwRoleID, 0, -500, purpose::guild_create, 0);
		notifyIngot(role);

		BroadcastCtrl::faction(E_BC_CREATE, guilds_id_index.size(), role->roleName, role->faction->name);
	}
	return 0;
}


//公会详情
int guild::rpc_getinfo(ROLE* role, unsigned char * data, size_t dataLen)
{
	enum result : byte
	{
		successful,// 成功
		not_guild,// 没有行会
		not_member,// 不是成员
	};
	//只看自己的
	output_stream out(S_GUILD_INFO);

	guild* faction = role->faction;

	if (faction == nullptr)
	{
		//out.write_byte(result::not_guild);
		role->send(out);
		return 0;
	}

	auto member = faction->get_member(role->dwRoleID);
	if (member == nullptr)
	{
		//out.write_byte(result::not_member);
		role->send(out);
		return 0;
	}

	//out.write_byte(result::successful);
	out.write_string(faction->name);
	out.write_byte((byte)(faction->level + 1));
	out.write_uint(faction->id);
	out.write_uint(faction->exp);
	out.write_string(faction->leader_name);
	out.write_ushort(faction->rank);//排行
	out.write_uint(faction->contribute);
	out.write_ushort(faction->members.size());
	out.write_ushort(faction->get_max_member());
	out.write_byte(member->job);
	out.write_byte(member->contribute_rank);		//(贡献排名),char[30](公告)]
	out.write_string(faction->notify);
	out.write_uint(faction->join_condition);
	out.write_uint(member->contribute);

	role->send(out);
	return 0;

}



//公告编辑
int guild::rpc_create_content(ROLE* role, unsigned char * data, size_t dataLen)
{
	string  notify;
	if (!BASE::parseBSTR(data, dataLen, notify))
		return -1;

	if (notify.size() >= 256)
		return 0;

	string strData;

	BYTE errCode = 0;
	do
	{
		COND_BREAK(role->faction == nullptr, errCode, 1);
		auto member = role->faction->get_member(role->dwRoleID);
		COND_BREAK(member == nullptr, errCode, 1);
		COND_BREAK(member->job == E_MEMBER, errCode, 1);
		role->faction->notify = notify;
		update_guild(role->faction);
	} while (0);


	S_APPEND_BYTE(strData, errCode);
	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_GUILD_create_content, strData));
	return 0;

}



int guild::rpc_get_member_list(ROLE* role, unsigned char * data, size_t dataLen)
{
	output_stream out(S_GUILD_INFO_MEMLIST);

	if (role->faction)
	{
		for (auto& it : role->faction->members)
		{
			out.write_uint(it.first);
			out.write_byte(it.second->job);
			out.write_string(it.second->name);

			out.write_byte(it.second->vocation);
			out.write_byte((byte)it.second->level);
			out.write_uint(it.second->contribute);
		}
	}

	role->send(out);
	return 0;

}

int guild::rpc_get_mem_equip(ROLE* role, unsigned char * data, size_t dataLen)
{
	//		C:[word(成员id)]
	//		s:[
	//		(这个应该是杨威的统一返回接口)
	//		]

	//	string strData;

	//	BYTE errCode = 0;

	//	S_APPEND_BYTE( strData, errCode );
	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_create_content, strData) );

	return 0;

}

int guild::rpc_add_friend(ROLE* role, unsigned char * data, size_t dataLen)
{

	//	string strData;

	//	BYTE errCode = 0;
	//	
	//	S_APPEND_BYTE( strData, errCode );
	//	PROTOCAL::sendClient( role->client, PROTOCAL::cmdPacket(S_GUILD_create_content, strData) );
	//	
	return 0;
}



//职位任命
//会长可以对除自己外的所有人进行职位任命，
//在弹出的职位任命窗口中，从4种职位中选择1种职位
//副会长、左护法、右护法 均只能有一个，将该职位任命给其他人后，之前的那个将自动变成成员职位
int guild::rpc_appoint_job(ROLE* role, unsigned char * data, size_t dataLen)
{
	DWORD roleid;
	BYTE byFacJob;

	if (!BASE::parseDWORD(data, dataLen, roleid))
		return -1;

	if (!BASE::parseBYTE(data, dataLen, byFacJob))
		return -1;

	E_FACTION_JOB dstJob = (E_FACTION_JOB)byFacJob;

	string strData;
	BYTE errCode = 0;

	do
	{
		if (dstJob == E_LEADER)
			errCode = 1;

		//不是帮主
		if (role->faction == nullptr &&
			role->faction->leader_roleid != role->dwRoleID &&
			role->faction->leader2_roleid != role->dwRoleID)
			errCode = 2;

		//委派自己
		COND_BREAK(role->dwRoleID == roleid, errCode, 3);

		//不是同一帮派
		auto member = role->faction->get_member(roleid);
		COND_BREAK(member == nullptr, errCode, 4);

		//同一职位
		COND_BREAK(member->job == dstJob, errCode, 5);

		//需要把原职务卸任
		if (dstJob != E_MEMBER)
		{
			guild_member* pFmOrig = nullptr;
			for (auto & it : role->faction->members)
			{
				if (it.second->job == dstJob)
				{
					pFmOrig = it.second;
					break;
				}
			}

			if (pFmOrig)
			{
				pFmOrig->job = E_MEMBER;
				notifyTypeValuebyRoleID(pFmOrig->roleid, ROLE_PRO_FACTION_JOB, E_MEMBER);
				update_member(pFmOrig);
			}

		}

		if (member->job == E_LEADER2)
		{
			role->faction->leader2_roleid = 0;
			role->faction->leader2_name = "";
		}
		else if (member->job == E_GUADIAN_LEFT)
		{
			role->faction->admin1_roleid = 0;
			role->faction->admin1_name = "";
		}
		else if (member->job == E_GUADIAN_RIGHT)
		{
			role->faction->admin2_roleid = 0;
			role->faction->admin2_name = "";
		}

		if (dstJob == E_LEADER2)
		{
			role->faction->leader2_roleid = member->roleid;
			role->faction->leader2_name = member->name;
		}
		else if (dstJob == E_GUADIAN_LEFT)
		{
			role->faction->admin1_roleid = member->roleid;
			role->faction->admin1_name = member->name;
		}
		else if (dstJob == E_GUADIAN_RIGHT)
		{
			role->faction->admin2_roleid = member->roleid;
			role->faction->admin2_name = member->name;
		}

		member->job = dstJob;
		notifyTypeValuebyRoleID(roleid, ROLE_PRO_FACTION_JOB, dstJob);
		update_member(member);

	} while (0);


	S_APPEND_BYTE(strData, errCode);
	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_MEM_JOB, strData));

	return 0;
}



//逐出行会
	//逐出行会
	//行会中，拥有5种职位身份，分别为：会长、副会长、左护法、右护法、成员
	//会长和副会长可以将行会中的普通成员逐出行会，当逐出的对象职位不为"成员"时
	//提示：抱歉，只有职位为成员的可以被逐出
int guild::rpc_kick_member(ROLE* role, unsigned char * data, size_t dataLen)
{
	DWORD roleid;

	if (!BASE::parseDWORD(data, dataLen, roleid))
		return -1;

	string strData;
	BYTE errCode = 0;

	if (role->faction == nullptr)
		return 0;

	do
	{
		//驱逐自己
		COND_BREAK(role->dwRoleID == roleid, errCode, 1);

		//不是帮主
		if (role->faction == nullptr &&
			role->faction->leader_roleid != role->dwRoleID &&
			role->faction->leader2_roleid != role->dwRoleID)
			errCode = 2;

		//不是同一帮派
		auto member = role->faction->get_member(roleid);
		COND_BREAK(member == NULL, errCode, 3);
		//当逐出的对象职位不为"成员"时
		COND_BREAK(member->job != E_MEMBER, errCode, 4);

		auto target_role = service::get_online_role(roleid);
		if (target_role)
		{
			output_stream out(S_UPDATE_ROLE_DATA);
			out.write_byte(ROLE_PRO_FACTION);
			out.write_uint(0);
			out.write_byte(ROLE_PRO_FACTION_CONTRIBUTE);
			out.write_uint(0);
			out.write_byte(ROLE_PRO_FACTION_JOB);
			out.write_uint(E_NONE);
			target_role->send(out);
		}

		role->faction->remove_member(member);

		auto target = RoleMgr::getMe().getRoleByID(roleid);
		if (target)
		{
			target->faction = nullptr;
			target->exit_guild_time = time(nullptr);
		}

		guilds_roleid_index.erase(roleid);
	} while (0);

	S_APPEND_BYTE(strData, errCode);
	PROTOCAL::sendClient(role->client, PROTOCAL::cmdPacket(S_GUILD_INFO_MEM_QUIT, strData));

	return 0;
}

//退出行会
int guild::rpc_quit(ROLE* role, unsigned char * data, size_t dataLen)
{
	enum result : byte
	{
		successful,// 成功
		not_guild,// 不在公会
		not_member,// 不是成员
		not_inherit,// 没有继承者
	};
	//		C:[]
	//		s:[
	//		byte(error返回值 0是成功)
	//		]

	//退出行会
	//点击退出行会按钮，将弹出对话框
	//会长：离开行会后，会长之位将传给副会长，或者解散，是否继续？
	//其他成员：是否确定要离开行会：行会名称？

	//会长退出行会后，副会长直接变成会长，若无副会长，则左护法 > 右护法，变成会长，若副会长、左护法、右护法均没有，则直接解散行会
	//行会解散后，行会中所有会员都将变成未入行会状态
	output_stream out(S_GUILD_INFO_QUIT);
	if (role->faction == nullptr)
	{
		out.write_byte(result::not_guild);
		role->send(out);
		return 0;
	}

	auto member = role->faction->get_member(role->dwRoleID);
	if (member == nullptr)
	{
		out.write_byte(result::not_member);
		role->send(out);
		return 0;
	}

	if (member->job == E_LEADER)
	{
		auto new_leader_id = role->faction->leader2_roleid ? role->faction->leader2_roleid :
			role->faction->admin1_roleid ? role->faction->admin1_roleid :
			role->faction->admin2_roleid ? role->faction->admin2_roleid : 0;

		//帮主退出，没有继任者
		if (new_leader_id == 0)
		{
			if (role->faction->get_member_count() > 1)// 公会还有其他成员，不能解散
			{
				out.write_byte(result::not_inherit);
				role->send(out);
				return 0;
			}
			else
			{
				remove(role->faction);
				role->faction = nullptr;
			}
		}
		else
		{
			//帮主退出，有继任者
			auto new_member = role->faction->get_member(new_leader_id);
			if (new_member)
			{
				new_member->job = E_LEADER;
				update_member(new_member);
				notifyTypeValuebyRoleID(new_leader_id, ROLE_PRO_FACTION_JOB, E_LEADER);
				notifyTypeValue(role, ROLE_PRO_FACTION_JOB, E_NONE);

				role->faction->leader_roleid = new_member->roleid;
				role->faction->leader_name = new_member->name;

				if (role->faction->leader2_roleid == new_leader_id)
				{
					role->faction->leader2_roleid = 0;
					role->faction->leader2_name = "";
				}
				else if (role->faction->admin1_roleid == new_leader_id)
				{
					role->faction->admin1_roleid = 0;
					role->faction->admin1_name = "";
				}
				else if (role->faction->admin2_roleid == new_leader_id)
				{
					role->faction->admin2_roleid = 0;
					role->faction->admin2_name = "";
				}

				role->faction->remove_member(member);
			}
			role->faction = nullptr;
		}
	}
	else
	{
		//非帮主退出
		role->faction->remove_member(member);
		notifyTypeValue(role, ROLE_PRO_FACTION_JOB, E_NONE);
		role->faction = nullptr;
	}

	role->exit_guild_time = time(nullptr);

	notifyTypeValue(role, ROLE_PRO_FACTION, 0);
	guilds_roleid_index.erase(role->dwRoleID);

	out.write_byte(result::successful);
	role->send(out);
	return 0;
}



int guild::rpc_get_chatlist(ROLE* role, unsigned char * data, size_t dataLen)
{
	output_stream out(S_GUILD_INFO_SAY_LIST);

	if (role->faction == nullptr)
		return 0;

	for (auto it = role->faction->chats.rbegin();
		it != role->faction->chats.rend(); ++it)
	{
		out.write_uint(it->roleid);
		out.write_byte(it->job);
		out.write_string(it->name);
		out.write_uint(it->time);
		out.write_string(it->content);
	}

	role->send(out);
	return 0;
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
vector<guild::contribute_reward> guild::contribute_rewards;
vector<guild::level_info> guild::level_exps;

void guild::load_config()
{
	{
		TiXmlDocument doc;
		string config_file = string(getProgDir()) + STR_FSEP + "config/guild_contribute.xml";
		doc.LoadFile(config_file.c_str());
		if (doc.Error())
		{
			logERROR("%s", doc.ErrorDesc());
			exit(0);
		}

		auto root = doc.RootElement();

		auto sub = root->FirstChild();
		while (sub)
		{
			if (sub->Type() != TiXmlNode::TINYXML_ELEMENT)
			{
				sub = sub->NextSibling();
				continue;
			}
			auto element = sub->ToElement();

			contribute_reward temp;
			temp.coin = atoi(element->Attribute("coin"));
			temp.gold = atoi(element->Attribute("gold"));
			temp.guild_exp = atoi(element->Attribute("guild_exp"));
			temp.person_contribute = atoi(element->Attribute("person_contribute"));

			contribute_rewards.push_back(temp);
			sub = sub->NextSibling();
		}
	}


	{
		TiXmlDocument doc;
		string config_file = string(getProgDir()) + STR_FSEP + "config/guild_level.xml";
		doc.LoadFile(config_file.c_str());
		if (doc.Error())
		{
			logERROR("%s", doc.ErrorDesc());
			exit(0);
		}

		auto root = doc.RootElement();

		auto sub = root->FirstChild();
		while (sub)
		{
			if (sub->Type() != TiXmlNode::TINYXML_ELEMENT)
			{
				sub = sub->NextSibling();
				continue;
			}
			auto element = sub->ToElement();

			level_info temp;
			element->QueryUnsignedAttribute("max_member", &temp.max_member);
			element->QueryUnsignedAttribute("exp", &temp.exp);
			element->QueryUnsignedAttribute("reward_coin", &temp.reward_coin);
			element->QueryUnsignedAttribute("reward_gold", &temp.reward_gold);

			level_exps.push_back(temp);
			sub = sub->NextSibling();
		}
	}
}


guild::contribute_reward* guild::get_contribute_reward(unsigned id)
{
	if (id >= contribute_rewards.size())
		return &contribute_rewards.back();
	return &contribute_rewards[id];
}


guild::level_info* guild::get_level_info(unsigned id)
{
	if (id >= level_exps.size())
		return nullptr;
	return &level_exps[id];
}

unsigned guild::get_max_level()
{
	return level_exps.size();
}