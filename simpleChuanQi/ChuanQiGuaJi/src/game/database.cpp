#include "base.h"
#include "database.h"


database::database(void)
{
}


database::~database(void)
{
	if(mysql!=nullptr)
	{
		mysql_close(mysql);
	}
}

void database::init(const string& server, const string& db, const string& user, const string& pwd, unsigned port)
{
	mysql = mysql_init(NULL);
	if(mysql==NULL) 
	{
		logFATAL("");
		//EXIT(EXIT_INIT);
	}

	mysql->reconnect = 1;
	//mysql_options(m_sqlConn, MYSQL_OPT_RECONNECT, "1");

	//logFATAL();
	if(!mysql_real_connect(mysql, server.c_str(), user.c_str(), pwd.c_str(), db.c_str(), port, NULL, 0))
	{
		string err = "database connect failed(errid:" + to_string(mysql_errno(mysql)) + ",desc:" + mysql_error(mysql) + ")";
		xLog("Error_", err.c_str());
		throw new logic_error(err);
	}
		//mysql_autocommit(mysql, 1);
	if (!mysql_set_character_set(mysql, "utf8"))
	{
		//logwm("Error New client character set: %s\n",mysql_character_set_name(mysql));
	}
}


bool database::exe(const string& sql)
{
	if(mysql_real_query(mysql, sql.c_str(), sql.size()))
		return false;
	return true;
}


string database::get_error()
{
	return string(mysql_error(mysql)) + "(" + to_string(mysql_errno(mysql)) + ")";
}