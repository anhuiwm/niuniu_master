#pragma once
#include <mysql.h>

class database
{
	MYSQL*	mysql;
public:
	database(void);
	~database(void);
	bool exe(const string& sql);
	string get_error();

protected:
	void init(const string& server, const string& db, const string& user, const string& pwd, unsigned port = 3306);
};


class db_result
{
	MYSQL_RES*  result;

public:
	db_result(MYSQL_RES*  result):result(result)
	{
	}

	~db_result()
	{
		mysql_free_result(result);
	}

	MYSQL_RES* operator->()
	{
		return result;
	}
};