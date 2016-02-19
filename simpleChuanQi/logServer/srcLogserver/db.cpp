#include "log.h"
#include "base.h"

//#include "config.h"

#include "db.h"



namespace DBCtrl
{

	string _dbIP = "192.168.6.165";
	string _dbUser = "root";
	string _dbPass = "passwd";
	string _dbName = "football";

	int _dbPort = 3306;

	bool _createIfNotExist = true;


	MYSQL* _mysql;

	bool readCfgDB( void );

};


#define FIELD_AUTO(fname)				#fname" INT(11) UNSIGNED not null auto_increment, "
#define FIELD_INT_UNSIGNED(fname)		#fname" INT(11) UNSIGNED not null default 0, "
#define FIELD_SMALL(fname)			#fname" SMALLINT(6) not null default 0,"
#define FIELD_TINY(fname, default_value)	#fname" TINYINT(6) not null default "#default_value", "

//#define FIELD_ROLENAME(fname)		#fname" VARCHAR(20) character set latin1 collate latin1_bin not null DEFAULT '',"
#define FIELD_ROLENAME(fname)		#fname" VARCHAR(22) character set latin1 collate latin1_bin DEFAULT NULL,"
#define FIELD_DATETIME(fname)			#fname" DATETIME NOT NULL DEFAULT '1970-01-01 00:00:00', "
#define FIELD_FLOAT(fname)			#fname" FLOAT not null default 1, "
#define FIELD_INT(fname)		#fname" INT(11) not null default 0, "


#define FIELD_INT_UNSIGNED_DEFAULT(fname, default_value)		#fname" int(11) unsigned not null default "#default_value", "
#define FIELD_SMALL_DEFAULT(fname, default_value)				#fname" SMALLINT(6) not null default "#default_value", "

MYSQL* DBCtrl::getSQL( void )
{
	return _mysql;
}

bool DBCtrl::readCfgDB( void )
{
	string strIniFile = //CONFIG::getConfigDir() + 
					string( getProgDir() )+ STR_FSEP + "server.ini";
	
	_dbIP =  BASE::readIni<string>( strIniFile, "db", "ip", "192.168.2.239" );
	_dbUser =  BASE::readIni<string>( strIniFile, "db", "user", "root" );
	_dbPass =  BASE::readIni<string>( strIniFile, "db", "password", "passwd" );
	_dbName =  BASE::readIni<string>( strIniFile, "db", "database", "gj" );

	_dbPort =  BASE::readIni<int>( strIniFile, "db", "port", 3306 );

	return true;

}

#define MODIFY_UNSIGNED(tbname, fname)	\
	execSQL(_mysql, "alter table " tbname " modify column "#fname" int(11) unsigned not null default 0")
	
bool DBCtrl::dbInit( void )
{
	readCfgDB( );

	const char* engine = "InnoDB";

	_mysql = switchToDB( _dbIP.c_str(), _dbUser.c_str(), 
								_dbPass.c_str(), _dbName.c_str(),
								_dbPort, _createIfNotExist);

	if ( _mysql == NULL )
	{
		exit(9);
	}

	return true;

}


