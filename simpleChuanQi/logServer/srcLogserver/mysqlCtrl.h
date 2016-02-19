#ifndef JY_MYSQL_H
#define JY_MYSQL_H


//#include <mysql/mysql.h>
#include <mysql.h>
#include <string>
using namespace std;

#define MAX_INSERT_BUF (819200)
#define DEFAULT_INSERT_LEN (409600)

typedef void (*Each_Row_Func)(MYSQL *mysql, MYSQL_ROW &row, unsigned long* &lengths, void* v);

#define MYSQL_REAL_QUERY_SUCC (0)



#define MYSQL_INIT2(mysql, X) \
	mysql = mysqlInit();\
	mysqlRealConnect(mysql, X.strDBIP.c_str(), \
							X.strDBUser.c_str(), X.strDBPass.c_str(),\
							X.strDBName.c_str(), X.nDBPort, NULL, 0)




#define MYSQL_INIT__(mysql, mysql_gs, X) \
	mysql = mysqlInit();\
	mysqlRealConnect(mysql, X.strDBIP.c_str(), \
							X.strDBUser.c_str(), X.strDBPass.c_str(),\
							X.strDBName.c_str(), X.nDBPort, NULL, 0);\
	mysql_gs = mysqlInit();\
	mysqlRealConnect(mysql_gs, X.strDBIP.c_str(), \
							X.strDBUser.c_str(), X.strDBPass.c_str(),\
							X.strDBNameGame.c_str(), X.nDBPort, NULL, 0);



#define MYSQL_INIT(mysql) \
	mysql = mysqlInit();\
	mysqlRealConnect(mysql, m_stcBaseData.strDBIP.c_str(), \
							m_stcBaseData.strDBUser.c_str(), m_stcBaseData.strDBPass.c_str(),\
							m_stcBaseData.strDBName.c_str(), m_stcBaseData.nDBPort, NULL, 0)


/*
		
#define MYSQL_INIT_GBK(mysql) \
	mysql = mysqlInit();\
	mysqlRealConnect(mysql, m_stcBaseData.strDBIP.c_str(), \
							m_stcBaseData.strDBUser.c_str(), m_stcBaseData.strDBPass.c_str(),\
							m_stcBaseData.strDBName.c_str(), m_stcBaseData.nDBPort, NULL, 0);\
	mysql_set_character_set(mysql,"gbk");
*/



#define MYSQL_INIT_GBK(mysql) \
	MYSQL_INIT(mysql);\
	mysql_set_character_set(mysql,"gbk");



#define MYSQL_INIT3()\
	({\
		MYSQL *mysql = mysqlInit();\
		mysqlRealConnect(mysql, m_stcBaseData.strDBIP.c_str(), \
							m_stcBaseData.strDBUser.c_str(), m_stcBaseData.strDBPass.c_str(),\
							m_stcBaseData.strDBName.c_str(), m_stcBaseData.nDBPort, NULL, 0);\
	})

		
		
#define MYSQL_UNINIT(mysql) \
	do {\
		mysql_close(mysql);\
	}while(0)
	//mysql_library_end();

	
	
#define Mysql_real_query_ElseExit(mysql, q, length) \
		do {\
			if ( MYSQL_REAL_QUERY_SUCC != mysqlRealQuery(mysql, q, length) ) \
				exit(9);\
		}while(0)

#define ROLE_NAME_LEN 22

#define MYSQL_ESCAPE_CPPROLE_ELSE_RET(szParam, strRoleName, a, ...)\
					if( (strRoleName).size()>=ROLE_NAME_LEN ) return a;\
					char szParam[ROLE_NAME_LEN*2+1] = {0};\
					mysql_escape_string( szParam, (strRoleName).data(), (strRoleName).size() )
			
			
#define MYSQL_ESCAPE_ROLE_NLEN_ELSE_RET(szParam, szRoleName, nLen, a, ...)\
					if( nLen>=ROLE_NAME_LEN ) return a;\
					char *szParam = (char *)alloca(nLen*2+1);\
					szParam[0]=0;\
					mysql_escape_string( szParam, szRoleName, nLen )
			
			
#define MYSQL_ESCAPE_C(szParam, szCstr)\
					int nLen = strlen(szCstr);\
					char *szParam = (char *)alloca(nLen*2+1);\
					szParam[0]=0;\
					mysql_escape_string( szParam, szCstr, nLen)
			
			
#define MYSQL_ESCAPE_CPP(szParam, strCPPStr)\
					char *szParam = (char *)alloca(strCPPStr.size()*2+1);\
					szParam[0]=0;\
					mysql_escape_string( szParam, strCPPStr.data(), strCPPStr.size())
			
			
#define MYSQL_ESCAPE_STRARRAY(szEscaped, szArray) \
					char szEscaped[ sizeof(szArray)*2+1 ]={0};\
					mysql_escape_string(szEscaped, szArray, strlen(szArray) )
				
#define MYSQL_ESCAPE_ARRAY(szEscaped, szArray) \
					char szEscaped[ sizeof(szArray)*2+1 ]={0};\
					mysql_escape_string(szEscaped, szArray, sizeof(szArray) )
			
			
#define MYSQL_ESCAPE_N(szParam, szCstr, real, max)\
					char szParam[max*2+1] = {0};\
					mysql_escape_string( szParam, (char*)(szCstr), real)


#define MYSQL_ESCAPE_ALLOCA_N(szParam, szCstr, real )\
					char *szParam = (char *)alloca(real*2+1);\
					mysql_escape_string( szParam, (char*)(szCstr), real)
					
#define MYSQL_ESCAPE_BLOB_N(szParam, szCstr, real, max)\
					char szParam[max*2+1] = {0};\
					if ( szCstr == NULL ) strcpy(szParam, "NULL");\
					else {\
						int i = mysql_escape_string( szParam+1, szCstr, real);\
						szParam[0]='\'';\
						szParam[ 1 +i ]='\'';\
					}

string BadWhere(const char* szRole);
bool forEachRow(MYSQL *mysql, Each_Row_Func erf, void* v, const char* fmt,...);

void mysqlClose(MYSQL **mysql);

MYSQL *mysqlInit(void);
MYSQL_STMT* mysqlStmtInit(MYSQL* mysql);
MYSQL *mysqlRealConnect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag);
int mysqlRealQuery(MYSQL *mysql, const char *q, unsigned long length);
int mysqlRealQueryCreateTbl(MYSQL *mysql, const char *q, unsigned long length, const char* szCreateFmt, ... );
bool execSQL( MYSQL* conn, const char* fmt, ...);
void EXECSQL_Else_Exit(MYSQL* conn, const char* fmt, ...);


MYSQL * switchToDB(const char* dbip, const char* user, const char* passwd, const char* dbname, short dbport, bool bCreateIfNotExist=false);

typedef int (*FuncField)(char* szTgt, const char* szFieldValue, void* para);


typedef int (*FuncField2)(char* szTgt, const char* szData, int nDataLen, void* para);


typedef struct _FuncFieldPara {
	FuncField func;
	void * para;
}FuncFieldPara;


typedef struct _FuncFieldPara2 {
	FuncField2 func;
	void * para;
}FuncFieldPara2;

typedef struct {

	int nDefineLen;
	int nRowIndex;
	enum_field_types type;
	
	//string strFixValue;
	
	FuncField func;
	void * para;
	
//	char* szFieldData;

} FieldInfo;


typedef struct {

	int nDefineLen;
	int nRowIndex;
	enum_field_types type;

	FuncField2 func;
	void * para;

} FieldInfo2;



typedef map<string, FieldInfo> MAP_FIELD_INFO;
typedef map<string, FieldInfo2> MAP_FIELD_INFO2;

class SQLSelect
{
public:

	
	//MYSQL_ROW row;
	unsigned long* lengths;


	SQLSelect(MYSQL* conn, const char* fmt, ...);
//	SQLSelect(const char* szTable, MYSQL* conn, const char* fmt, ...);
	~SQLSelect(void);
	
	//operator MYSQL_RES*() {return result;}
	MYSQL_RES * getResult(void);
	
	MYSQL_ROW getRow(void);
	MYSQL_ROW getRow2(void);
	
	int getNumFields(void);
	int getNumRows(void);
	
	void insert2(const char* szTable, MYSQL* sqlTgt, map<string, FuncFieldPara2>* m=NULL);

	
	static int m_nEachInsert;
	static bool m_bLogDup;
	static bool m_bLogSelect;

	static void printFields(MYSQL* conn, const char* szTableName);

	
	
private:
	MYSQL_RES* m_result;
	MAP_FIELD_INFO m_mapFieldInfo;
	MAP_FIELD_INFO2 m_mapFieldInfo2;
	

	/*SQLRES& operator=(MYSQL_RES* Result)
	{
		if(result == Result) 
			return *this;

		if (result)
			mysql_free_result(result);
		result = Result;
		return *this;
	}*/
	
};


class CLogDup 
{
public:
	CLogDup (bool bLogDup)
	{
		this->bLogDup_Orig = SQLSelect::m_bLogDup;
		SQLSelect::m_bLogDup = bLogDup;
	}

	~CLogDup (void)
	{
		SQLSelect::m_bLogDup = this->bLogDup_Orig;
	}

private:
	bool bLogDup_Orig;
	
};




#endif
