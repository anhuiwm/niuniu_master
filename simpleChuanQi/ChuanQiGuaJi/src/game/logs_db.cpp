#include "base.h"
#include "logs_db.h"
#include "service.h"

//logs_db_thread logs_db_thread::singleton;


void logs_db::init()
{
	//logwm(" logs_db::init() %s %s %s %s %u", service::logs_db_config.ip.c_str(), service::logs_db_config.user.c_str(), 
	//	service::logs_db_config.pwd.c_str(), service::logs_db_config.schema.c_str(),
	//	service::logs_db_config.port);
	database::init(service::logs_db_config.ip.c_str(), service::logs_db_config.schema.c_str(),
		service::logs_db_config.user.c_str(), service::logs_db_config.pwd.c_str(), service::logs_db_config.port);
}


logs_db_thread::logs_db_thread():closed(false), thread(logs_db_thread::s_thread_proc, this)
{
	db.init();
}


logs_db_thread::~logs_db_thread()
{
	close();
	thread.join();
}



void logs_db_thread::s_thread_proc(void* arg)
{
	auto _this = (logs_db_thread*)arg;
	_this->thread_proc();

#ifdef WIN32
	ExitThread(0);
#endif
}


void logs_db_thread::thread_proc()
{
#ifdef WIN32
	cout << "logs_db_thread proc entry" << endl;
#endif
	while(true)
	{
		Sleep(1);

		if(!empty())
		{
			const auto& msg = pop();

			lognew("%s",msg.c_str());

			if(!db.exe(msg))
			{
				xLog("Error_", db.get_error().c_str());
			}
		}

		if(closed)
		{
			return;
		}
	}
}

void logs_db_thread::consume(unsigned serverid, const string& username, int roleid, int coin, int ingot, purpose purpose, int itemid)
{
	char temp[500];
	sprintf(temp, "insert into log_coin_consume values(0, %u,%u,'%s',%d,%d,%d,%d,%d)",
		serverid, time(0), username.c_str(), roleid, coin, ingot, (int)purpose, itemid);

	xLog("sql_pre", temp);

	singleton().push(temp);
}