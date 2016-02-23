#include "pch.h"
#include "log.h"
#include "base.h"

#include "config.h"

#include "db.h"
#include "service.h"


namespace DBCtrl
{

	//string _dbIP = "192.168.6.165";
	//string _dbUser = "root";
	//string _dbPass = "passwd";
	//string _dbName = "football";

	//int _dbPort = 3306;

	bool _createIfNotExist = true;


	MYSQL* _mysql;

	//bool readCfgDB( void );

};


#define FIELD_AUTO(fname)				#fname" INT(11) UNSIGNED not null auto_increment, "
#define FIELD_INT_UNSIGNED(fname)		#fname" INT(11) UNSIGNED not null default 0, "
#define FIELD_SMALL(fname)			#fname" SMALLINT(6) not null default 0,"
#define FIELD_TINY(fname, default_value)	#fname" TINYINT(6) not null default "#default_value", "

//#define FIELD_ROLENAME(fname)		#fname" VARCHAR(20) character set utf8 collate utf8_bin not null DEFAULT '',"
#define FIELD_ROLENAME(fname)		#fname" VARCHAR(22) character set utf8 collate utf8_bin DEFAULT NULL,"
#define FIELD_DATETIME(fname)			#fname" DATETIME NOT NULL DEFAULT '1970-01-01 00:00:00', "
#define FIELD_FLOAT(fname)			#fname" FLOAT not null default 1, "
#define FIELD_INT(fname)		#fname" INT(11) not null default 0, "


#define FIELD_INT_UNSIGNED_DEFAULT(fname, default_value)		#fname" int(11) unsigned not null default "#default_value", "
#define FIELD_SMALL_DEFAULT(fname, default_value)				#fname" SMALLINT(6) not null default "#default_value", "

MYSQL* DBCtrl::getSQL( void )
{
	return _mysql;
}

//bool DBCtrl::readCfgDB( void )
//{
//	string strIniFile = //CONFIG::getConfigDir() + 
//					string( getProgDir() )+ STR_FSEP + "server.ini";
//	
//	_dbIP =  BASE::readIni<string>( strIniFile, "db", "ip", "192.168.2.239" );
//	_dbUser =  BASE::readIni<string>( strIniFile, "db", "user", "root" );
//	_dbPass =  BASE::readIni<string>( strIniFile, "db", "password", "passwd" );
//	_dbName =  BASE::readIni<string>( strIniFile, "db", "database", "gj" );
//
//	_dbPort =  BASE::readIni<int>( strIniFile, "db", "port", 3306 );
//
//	return true;
//
//}

#define MODIFY_UNSIGNED(tbname, fname)	\
	execSQL(_mysql, "alter table " tbname " modify column "#fname" int(11) unsigned not null default 0")
	
bool DBCtrl::dbInit( void )
{
	//const char* engine = "InnoDB";//"MyISAM";

	_mysql = switchToDB( service::main_db_config.ip.c_str(), service::main_db_config.user.c_str(), 
								service::main_db_config.pwd.c_str(), service::main_db_config.schema.c_str(),
								service::main_db_config.port, _createIfNotExist);

	if ( _mysql == NULL )
	{
		logFATAL(" init db error: %s %s %s %s %u", service::main_db_config.ip.c_str(), service::main_db_config.user.c_str(), 
			service::main_db_config.pwd.c_str(), service::main_db_config.schema.c_str(),
			service::main_db_config.port);
		exit(9);
	}

	if (mysql_set_character_set(_mysql, "utf8"))
	{
		logFATAL("New client character set: %s\n", mysql_character_set_name(_mysql));
	}

	//execSQL(_mysql, "CREATE TABLE IF NOT EXISTS " TABLE_ROLE_INFO " ("

	//					FIELD_AUTO(id)
	//					FIELD_ROLENAME(rolename)

	//					" passwd char(16) character set utf8 collate utf8_bin not null DEFAULT '',"
	//					" uuid char(32) character set utf8 collate utf8_bin not null DEFAULT '',"
	//					" serial varchar(32) character set utf8 collate utf8_bin DEFAULT NULL,"
	//					
	//					FIELD_TINY(account_status, 0)
	//					FIELD_TINY(job, 1)
	//					FIELD_TINY(sex, 0)
	//					FIELD_TINY(phone_type, 0)
	//					FIELD_TINY(platform_id, 0)
	//					FIELD_SMALL_DEFAULT(level, 1)
	//					FIELD_SMALL(viplevel)

	//					FIELD_INT_UNSIGNED(exp)
	//					FIELD_INT_UNSIGNED_DEFAULT(hp, 0)
	//					FIELD_INT_UNSIGNED_DEFAULT(money, 0)
	//					FIELD_INT_UNSIGNED(gold)
	//					FIELD_INT_UNSIGNED(charge)
	//					FIELD_INT_UNSIGNED_DEFAULT(mapid,1)
	//					
	//					FIELD_SMALL_DEFAULT(cur_sceneid, 1)
	//					FIELD_SMALL_DEFAULT(fromid, 1)
	//					
	//					FIELD_DATETIME(lastaccess)
	//					FIELD_DATETIME(inc_hp_time)
	//					
	//					FIELD_INT_UNSIGNED(ip)

	//					" email char(64) character set utf8 collate utf8_bin DEFAULT NULL,"

	//					"roleblob blob default NULL,"
	//					" PRIMARY KEY  (`id`),"
	//					" KEY  (`uuid`),"
	//					" UNIQUE KEY (`platform_id`,`serial`),"
	//					" UNIQUE KEY (`rolename`),"
	//					" UNIQUE KEY (`email`)"
	//					" ) ENGINE=`%s` DEFAULT CHARSET=utf8 ;", engine);

//	execSQL(_mysql, "alter table " TABLE_ROLE_INFO " add column factionid int(11) unsigned not null default 0");



//行会拥有15项科技，分别为：
//	等级：行会的等级，决定行会的人数限制，同时作为圣殿科技的升级条件
//	福利：影响行会成员每天领取的奖励内容，等级越高奖励越好
//	圣殿：影响行会战及行会活动的相关内容，同时作为其他科技的升级条件
//	生命值：为所有行会成员提供生命值属性加成，等级越高，加成数值越大
//	法力值：为所有行会成员提供法力值属性加成，等级越高，加成数值越大
//	攻击下限、攻击上限：为行会所有成员分别提供攻击下限和攻击上限的数值加成
//	魔法下限、魔法上限：为行会所有成员分别提供魔法下限和魔法上限的数值加成
//	道术下限、道术上限：为行会所有成员分别提供道术下限和道术上限的数值加成
//	防御下限、防御上限：为行会所有成员分别提供防御下限和防御上限的数值加成
//	魔防下限、魔防上限：为行会所有成员分别提供魔防下限和魔防上限的数值加成
//	guardian_left

//	execSQL(_mysql, "CREATE TABLE IF NOT EXISTS " TABLE_FACTION " ("
//
//						FIELD_AUTO(id)
//						FIELD_ROLENAME(factionname)
//						
////						FIELD_ROLENAME(leadername)
////						FIELD_INT_UNSIGNED(leaderid)
////						FIELD_ROLENAME(leader2name)
////						FIELD_INT_UNSIGNED(leader2id)
////						FIELD_ROLENAME(guardian_left_name)
////						FIELD_INT_UNSIGNED(guardian_left_id)
////						FIELD_ROLENAME(guardian_right_name)
////						FIELD_INT_UNSIGNED(guardian_right_id)
//
//						FIELD_SMALL_DEFAULT(main, 1)
//						FIELD_INT_UNSIGNED(main_exp)
//
//						FIELD_SMALL_DEFAULT(welfare, 1)
//						FIELD_INT_UNSIGNED(welfare_exp)
//
//						FIELD_SMALL_DEFAULT(temple, 1)
//						FIELD_INT_UNSIGNED(temple_exp)
//
//						FIELD_SMALL_DEFAULT(hp, 1)
//						FIELD_INT_UNSIGNED(hp_exp)
//
//						FIELD_SMALL_DEFAULT(mp, 1)
//						FIELD_INT_UNSIGNED(mp_exp)
//
//						FIELD_SMALL_DEFAULT(atk, 1)
//						FIELD_INT_UNSIGNED(atk_exp)
//
//						FIELD_SMALL_DEFAULT(mag, 1)
//						FIELD_INT_UNSIGNED(mag_exp)
//
//						FIELD_SMALL_DEFAULT(sol, 1)
//						FIELD_INT_UNSIGNED(sol_exp)
//
//						FIELD_SMALL_DEFAULT(def, 1)
//						FIELD_INT_UNSIGNED(def_exp)
//
//						FIELD_SMALL_DEFAULT(magdef, 1)
//						FIELD_INT_UNSIGNED(magdef_exp)
//
//
//						FIELD_INT_UNSIGNED(contribute)
//						
//						"board varchar(280) binary default '', "
//
//						" PRIMARY KEY  (`id`),"
//						" KEY  (`main`),"
//						" UNIQUE KEY (`factionname`)"
//						" ) ENGINE=`%s` DEFAULT CHARSET=utf8;", engine);



//	execSQL(_mysql, "CREATE TABLE IF NOT EXISTS "  TABLE_FACTION_BUILDER " ("
//										FIELD_INT_UNSIGNED(factionid)
//										FIELD_INT_UNSIGNED(builderid)
//										FIELD_INT_UNSIGNED(level)
//										FIELD_INT_UNSIGNED(exp)

//										   "PRIMARY KEY (factionid, builderid)"
//										   ")engine=innodb character set=latin1");



//	//这个表里的记录只update, 不delete
//	execSQL(_mysql, "CREATE TABLE IF NOT EXISTS " TABLE_FACTION_MEMBER " ("
//
//						FIELD_INT_UNSIGNED(roleid)
//
//						FIELD_INT_UNSIGNED(factionid)
//						FIELD_INT_UNSIGNED(facjob)
//
//						FIELD_INT_UNSIGNED(contribute)
//
//						" PRIMARY KEY  (`roleid`)"
//						" ) ENGINE=`%s` DEFAULT CHARSET=utf8;", engine);
//
//
//	execSQL(_mysql, "CREATE TABLE IF NOT EXISTS " TABLE_FACTION_APPLY " ("
//
//						FIELD_INT_UNSIGNED(roleid)
//						FIELD_INT_UNSIGNED(factionid)
//
////						FIELD_ROLENAME(rolename)
////						FIELD_SMALL_DEFAULT(level, 1)
////						FIELD_INT_UNSIGNED(job)
//
//						" PRIMARY KEY  (`roleid`,`factionid`)"
//						" ) ENGINE=`%s` DEFAULT CHARSET=utf8;", engine);

	//execSQL(_mysql, "CREATE TABLE IF NOT EXISTS " TABLE_RANK " ("
	//	FIELD_AUTO(id)
	//	FIELD_INT_UNSIGNED(roleid)
	//	FIELD_INT_UNSIGNED(itemid)
	//	FIELD_INT_UNSIGNED(level)
	//	FIELD_INT_UNSIGNED(rolevalue)
	//	FIELD_INT_UNSIGNED(itemvalue)
	//	FIELD_INT_UNSIGNED(arenarank)
	//	" PRIMARY KEY  (`id`) , "
	//	"CONSTRAINT `rank_ibfk_id` FOREIGN KEY(`roleid`) REFERENCES `roleinfo` (`id`)"
	//	" ) ENGINE=`%s` DEFAULT CHARSET=utf8;", engine);


	////FIELD_INT_UNSIGNED_DEFAULT(prizeid, 0)

	//execSQL(_mysql, "CREATE TABLE IF NOT EXISTS " TABLE_MAIL " ("
	//							FIELD_AUTO(id)
	//							FIELD_INT_UNSIGNED(roleid)
	//							FIELD_INT_UNSIGNED_DEFAULT(senderid, 0)
	//							
	//							"recvtime int(11) not null default 0,"
	//							"mailtype tinyint (4) unsigned not null default 0, "//0=奖励，1=消息，2=系统。
	//							"isread tinyint (4) unsigned not null default 0, "//0:未读;1:已读
	//							
	//							"prize varchar(100) binary default '', "
	//							"content varchar(200) binary default '', "

	//							"PRIMARY KEY id(id),	"
	//							"KEY	recverid(roleid)"
	//							") engine=innodb character set=utf8");






	logFATAL("dbinit ok ");

	return true;

}


