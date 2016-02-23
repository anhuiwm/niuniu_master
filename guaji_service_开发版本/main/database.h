#pragma once

class database
{
	MYSQL* mysql = nullptr;
	MYSQL_RES* result = nullptr;
public:
	database(void);
	~database(void);
	bool exe(const string& sql);
	bool has_result();
	bool read_row();
	const char** get_row();
	string get_error();

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