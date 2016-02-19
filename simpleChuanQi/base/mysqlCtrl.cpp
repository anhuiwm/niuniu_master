#include "base.h"
#include "log.h"
#include "mysqlCtrl.h"
#include <stdarg.h>

//#include <mysql/errmsg.h>
#include <errmsg.h>
//#include <mysql/mysqld_error.h>
#include <mysqld_error.h>
using namespace std;

//=====================================================================

#define EXIT_INIT 9
#define EXIT(exit_code)		\
	do{\
		exit(exit_code);\
	}while(0)

//bt(BACKTRACE_LEVEL);

#if defined( MYSQL_USE_RESULT )

#define MYSQL_GET_RESULT(a) mysql_use_result(a)


#else
#define MYSQL_GET_RESULT(a) mysql_store_result(a)

#endif

//=====================================================================
bool execSQL(MYSQL* conn, const char* fmt, ...)
{
	//char m_szSqlQuery[4096] ;
	char m_szSqlQuery[40960]={0};
	
	va_list ap;
	va_start(ap, fmt);

	int len = vsnprintf(m_szSqlQuery, sizeof(m_szSqlQuery) - 1, fmt, ap);
	//PUTS("len:%d", len);

	va_end(ap);

//PUTS("%s", m_szSqlQuery);
	return MYSQL_REAL_QUERY_SUCC == mysqlRealQuery(conn, m_szSqlQuery, len);
	
}


void EXECSQL_Else_Exit(MYSQL* conn, const char* fmt, ...)
{
	char m_szSqlQuery[409600] ;
	
	va_list ap;
	va_start(ap, fmt);

	int len = vsnprintf(m_szSqlQuery, sizeof(m_szSqlQuery) - 1, fmt, ap);

	va_end(ap);

	if ( MYSQL_REAL_QUERY_SUCC != mysqlRealQuery(conn, m_szSqlQuery, len) ) {

		PRINT_LOG("%s", m_szSqlQuery);
		exit(9);
	}
	
	//PRINT_LOG("%s", m_szSqlQuery);

	
}


//=====================================================================

void mysqlClose(MYSQL **mysql)
{
	if( mysql ) {
		mysql_close( *mysql );
		*mysql = NULL;
	}
}


//this function alway succ
MYSQL *mysqlInit(void)
{
	MYSQL*	mysql = mysql_init(NULL);
	if(mysql==NULL) {
		logFATAL("init error");
		EXIT(EXIT_INIT);
	}

	mysql->reconnect = 1;
	//mysql_options(m_sqlConn, MYSQL_OPT_RECONNECT, "1");
		
	#if MYSQL_VERSION_ID >= 50013
		my_bool trueval = 1;
		mysql_options(mysql, MYSQL_OPT_RECONNECT, &trueval);
	#endif
	
	return mysql;
}


MYSQL_STMT* mysqlStmtInit(MYSQL* mysql)
{
	
	MYSQL_STMT* x=NULL;

	if(mysql==NULL || (x = mysql_stmt_init(mysql))==NULL ) {
		logFATAL("db init error");
		EXIT(EXIT_INIT);
	}
	
	return x;
}
//=====================================================================

MYSQL *mysqlRealConnect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag)
{
	if( NULL == mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag) ) {
		PRINT_LOG("mysql_real_connect %s, %s %s %s %s %d", mysql_error(mysql), host, user, passwd, db, port);
		mysql_close(mysql);
		EXIT(EXIT_INIT);
	}

	return mysql;	
}

//=====================================================================
int mysqlRealQuery(MYSQL *mysql, const char *q, unsigned long length)
{

	if( mysql == NULL ) {
		logFATAL("Mysql CONNECTION NULL");
		return -1;
	}
	
	if ( SQLSelect::m_bLogSelect )
		logFATAL("%s", q);


	int ret = mysql_real_query(mysql, q, length);
	if ( ret != 0 )
	{

		int err = mysql_errno(mysql);

		if ( err == ER_DUP_ENTRY && !SQLSelect::m_bLogDup ) 
		{
			return -1;
		}
		
		logFATAL( "mysql_real_query(%d):==%s==", err, 
//			( err != ER_NO_SUCH_TABLE && err != ER_TABLEACCESS_DENIED_ERROR ) ? 
			( err != ER_TABLEACCESS_DENIED_ERROR ) ? 
					//?
					q : "" 
					);

		//if ( err==1065 )
		//	return MYSQL_REAL_QUERY_SUCC;

		return -1;
	}
	return MYSQL_REAL_QUERY_SUCC;
}


int mysqlRealQueryCreateTbl(MYSQL *mysql, const char *q, unsigned long length, const char* szCreateFmt, ... )
{
	if( mysql == NULL ) {
		logff("Mysql CONNECTION NULL");
		return -1;
	}

	int ret = mysql_real_query(mysql, q, length);
	if ( ret == 0 ) 
		return MYSQL_REAL_QUERY_SUCC;

	int err = mysql_errno(mysql);

	if( err != ER_NO_SUCH_TABLE) {
		logff("mysql_real_query(%d):%s", err, q);
		return ret;
	}
	
	char m_szSqlCreateTbl[2048]={0};
	va_list ap;
	va_start(ap, szCreateFmt);
	int len = vsnprintf(m_szSqlCreateTbl, sizeof(m_szSqlCreateTbl) - 1, szCreateFmt, ap);
	va_end(ap);
	
	if ( MYSQL_REAL_QUERY_SUCC == mysql_real_query(mysql, m_szSqlCreateTbl, len) ) {
		return mysql_real_query(mysql, q, length);
	}
	else
		logff("%s", m_szSqlCreateTbl);

	return -1;
}


//=====================================================================
#if 0
MYSQL * SwitchToDB(const char* dbip, const char* user, const char* passwd, const char* dbname, short dbport ,bool drop)
{
	char query[DB_QUERY_BUF_SIZE];

	MYSQL * mysql = Mysql_init();
	mysql = Mysql_real_connect(mysql, dbip, user, passwd, "mysql", dbport, NULL, 0);

	if( mysql !=NULL) {
		
		mysql_autocommit(mysql, TRUE);

		if(drop) {
			
			sprintf(query, "drop DATABASE %s ", dbname);	
			debugf("%s\n", query);
			mysql_query(mysql, query);
			//exit(1);
		}

		sprintf(query, "CREATE DATABASE IF NOT EXISTS %s ", dbname);
		mysql_query(mysql, query);

		sprintf(query, "USE %s ", dbname);
		mysql_query(mysql, query);
	}
	else {
		mysql = Mysql_init();
		mysql = Mysql_real_connect(mysql, dbip, user, passwd, dbname, dbport, NULL, 0);
	}
	
	return mysql;
}

#else

MYSQL * switchToDB(const char* dbip, const char* user, const char* passwd, const char* dbname, short dbport, bool bCreateIfNotExist)
{
	char query[1024];		
	MYSQL * mysql = mysqlInit();

//logFATAL();
	if( NULL != mysql_real_connect(mysql, dbip, user, passwd, dbname, dbport, NULL, 0) )
	{
		//logFATAL();
//		mysql_autocommit(mysql, TRUE);

		MYSQL_RES*  result = mysql_store_result(mysql);
		if (result)
		{
		   mysql_free_result(result);
		}

		//mysql_query(mysql,"SET NAMES utf8");

		return mysql;
	}

	if( NULL == mysql_real_connect(mysql, dbip, user, passwd, NULL, dbport, NULL, 0) )
	{		
		logFATAL("%s\t%s\t%s\t%d", dbip, user, passwd, dbport);
		return NULL;
	}
	if (!mysql_set_character_set(mysql, "utf8"))
	{
		//exit(9);
	}

	if ( bCreateIfNotExist ) 
	{
		sprintf(query, "CREATE DATABASE IF NOT EXISTS %s", dbname);
		if ( mysql_query(mysql, query) )
		{
			logFATAL("");
			return NULL;
		}
	
		sprintf(query, "USE %s ", dbname);
		if ( mysql_query(mysql, query) ) 
		{			
			logFATAL("");
			return NULL;
		}
	}
	
	mysql_autocommit(mysql, 1);
	//logFATAL("");
	return mysql;
}

#endif

void mysqlGetTables(MYSQL *mysql)
{
	MYSQL_RES *res = mysql_list_tables(mysql,NULL);
	
	if ( res == NULL )
		return;

	while( MYSQL_ROW row = mysql_fetch_row(res) ) {
			PRINTLN("%s", row[0]);
	}
	
}
//=====================================================================
string BadWhere(const char* szRole)
{
	char szWhere[128] = {0};
	char szRoleName[22*2+1] = {0};
	if( szRole )
		mysql_escape_string( szRoleName, szRole, strlen(szRole) );

	if( szRoleName[0] )
		sprintf(szWhere, "where rolename = '%s' ", szRoleName);

	return szWhere;
}

bool forEachRow(MYSQL *mysql, Each_Row_Func erf, void* v, const char* fmt,...)
{
	char m_szSqlQuery[2048];
	
	va_list ap;
	va_start(ap, fmt);
	int len = vsnprintf(m_szSqlQuery, sizeof(m_szSqlQuery) - 1, fmt, ap);
	va_end(ap);

	MYSQL_RES* res = NULL;
	if (MYSQL_REAL_QUERY_SUCC == mysqlRealQuery(mysql, m_szSqlQuery, len) )	
		res = MYSQL_GET_RESULT(mysql);
	
	if( res == NULL ) {
		logFATAL("%s", mysql_error(mysql));
		return false;
	}

	MYSQL_ROW row;
	unsigned long* lengths;

	while( (row = mysql_fetch_row(res)) ) {
		lengths = mysql_fetch_lengths(res);
			
		erf(mysql, row, lengths, v);	
	}
	
	if (res)
		mysql_free_result(res);
	
	return true;	
}



//=====================================================================

int SQLSelect::m_nEachInsert = DEFAULT_INSERT_LEN;
bool SQLSelect::m_bLogDup = false;
bool SQLSelect::m_bLogSelect = false;


SQLSelect::SQLSelect(MYSQL* conn, const char* fmt, ...)
{
	char m_szSqlQuery[2048];

	va_list ap;
	va_start(ap, fmt);
	int len = vsnprintf(m_szSqlQuery, sizeof(m_szSqlQuery) - 1, fmt, ap);
	va_end(ap);

	m_result = NULL;
	//row = NULL;
	lengths = NULL;

//	if ( m_bLogSelect )
//		logFATAL("%s", m_szSqlQuery);
		
	if (MYSQL_REAL_QUERY_SUCC == mysqlRealQuery(conn, m_szSqlQuery, len) ) 
	{
		m_result = MYSQL_GET_RESULT(conn);
		if(m_result == NULL )
			logff("m_result error (%d) :%s", mysql_errno(conn), mysql_error(conn));//CR_OUT_OF_MEMORY 
	}
	else
	{
		logff("query sql(%s) error (%d) :%s", m_szSqlQuery, mysql_errno(conn), mysql_error(conn)); 
	}
}


MYSQL_ROW SQLSelect::getRow(void)
{
	MYSQL_ROW row;
	
	if( m_result && (row=mysql_fetch_row(m_result)) &&	(lengths=mysql_fetch_lengths(m_result)) )
		return row;

	return NULL;
}

//MYSQL_TYPE_VARCHAR

#define Is_Esc_String_Type(t) \
	( t == MYSQL_TYPE_VAR_STRING || t == MYSQL_TYPE_TINY_BLOB || t == MYSQL_TYPE_MEDIUM_BLOB || t == MYSQL_TYPE_LONG_BLOB ||\
		t == MYSQL_TYPE_BLOB || t == MYSQL_TYPE_VAR_STRING || t == MYSQL_TYPE_STRING )

#define Is_Date_String_Type(t) ( t == MYSQL_TYPE_TIMESTAMP || t == MYSQL_TYPE_DATE || t ==MYSQL_TYPE_TIME || t == MYSQL_TYPE_DATETIME )



void SQLSelect::printFields(MYSQL* conn, const char* szTableName)
{

	char szSqlQuery[512];

	if ( MYSQL_REAL_QUERY_SUCC != mysqlRealQuery(conn, szSqlQuery, sprintf ( szSqlQuery, "select * from %s limit 1", szTableName ) ))
		return;

	MYSQL_RES* result = MYSQL_GET_RESULT(conn);

	if(result == NULL ) {
		logff("m_result error (%d) :%s", mysql_errno(conn), mysql_error(conn));//CR_OUT_OF_MEMORY 
		return;
	}

	printf("%s\t\"", szTableName);

	for( MYSQL_FIELD *field; (field = mysql_fetch_field( result )); ) {
		
		printf ( "%s,", field->org_name );
		
	}

	printf("\b\"\n");
		
	mysql_free_result(result);
	

}

void SQLSelect::insert2(const char* szTable, MYSQL* sqlTgt, map<string, FuncFieldPara2>* m)
{
	static char szGlobal[MAX_INSERT_BUF];

	if ( m_result==NULL )
		return;

	MYSQL_FIELD *field;
		
	for( int nRowIndex = 0; (field = mysql_fetch_field( m_result )); nRowIndex++ ) {

//		FieldInfo &fi = m_mapFieldInfo[ !STREQL( field->name, field->org_name ) ? field->org_name : field->name ];
		
		FieldInfo2 &fi = m_mapFieldInfo2[ field->org_name ];

		fi.nDefineLen	= field->length;
		fi.nRowIndex	= nRowIndex;
		fi.type			= field->type;
		fi.func 		= NULL;
		fi.para 		= NULL;

		//需要特殊处理的，必须用别名传进来
		if ( !STREQL( field->name, field->org_name ) &&  m ) {
			map<string, FuncFieldPara2>::iterator it = m->find( field->org_name );
			if ( it != m->end() ) {
				fi.func = it->second.func;
				fi.para = it->second.para;
				//printf("fi.strFixValue:%s  field->org_name:%s\n", fi.strFixValue.c_str(), field->org_name);
			}
		}
				
	}

	char *head = szGlobal;
	head += sprintf(head, "insert into %s( ", szTable);
	for ( MAP_FIELD_INFO2::iterator it = m_mapFieldInfo2.begin(); it!=m_mapFieldInfo2.end(); ++it) {
	
		head += sprintf(head, "%s,", it->first.c_str() );
	}
	
	--head;
	head += sprintf(head, ") values ");

	char *p	= head;
	
	while ( MYSQL_ROW row = getRow() ) {
		
		p += sprintf( p, " (" );
		
		for ( MAP_FIELD_INFO2::iterator it = m_mapFieldInfo2.begin(); it!=m_mapFieldInfo2.end(); ++it) {

			const FieldInfo2 &fi = it->second;
		
			if ( row[fi.nRowIndex]==NULL ) {
				p += sprintf( p, "NULL," );
			}			
			else if ( fi.func ) {
				p += fi.func( p, row[fi.nRowIndex], lengths[fi.nRowIndex], fi.para );
			}
			else if ( Is_Date_String_Type(fi.type) ) {
				p += sprintf( p, "'%s',", row[fi.nRowIndex] );
			}
			else if ( Is_Esc_String_Type(fi.type) ) {
				p += sprintf( p, "'" );
				p += mysql_escape_string( p, row[fi.nRowIndex], lengths[fi.nRowIndex] );
				p += sprintf( p, "'," );
			}
			else
				p += sprintf( p, "%s,", row[fi.nRowIndex] );

		}
		
		--p;
		p += sprintf( p, ")," );
		
		int nLen = p - szGlobal-1;
		if ( nLen > m_nEachInsert ) {
			*--p = 0;
			Mysql_real_query_ElseExit(sqlTgt, szGlobal, nLen);
			p = head;
		}	
	}
		
	if ( p != head ) {
		int nLen = p - szGlobal-1;
		*--p = 0;
		Mysql_real_query_ElseExit(sqlTgt, szGlobal, nLen);
	}

}



int SQLSelect::getNumFields(void)
{
	if( m_result )
		return mysql_num_fields(m_result);

	return 0;
}


int SQLSelect::getNumRows(void)
{
	if( m_result )
		return (int)mysql_num_rows(m_result);

	return 0;
}

SQLSelect::~SQLSelect(void) 
{
	if (m_result)
		mysql_free_result(m_result);
	//printf("~SQLSelect\n" );

	for ( MAP_FIELD_INFO::iterator it =	m_mapFieldInfo.begin(); it!=m_mapFieldInfo.end(); ++it) {
//		free(it->second.szFieldData);
	}

	
}


MYSQL_RES * SQLSelect::getResult(void)
{
	return m_result;
}



