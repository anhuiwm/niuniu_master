#include "pch.h"
#include "jsonpacket.h"
#include "config.h"
#include "protocal.h"
#include "activity.h"


using namespace rapidjson;

//namespace	JSON
//{
//	static time_t gsLastAccess;
//}

//bool JSON::isNewDay()
//{
//	time_t now = time(nullptr);
//	return !( BASE::inTheSameDay(gsLastAccess, now) );
//}

//void JSON::setLastAccess(time_t now)
//{	
//	gsLastAccess = now;
//}

//void JSON::writeGsTime()
//{
//	time_t now = time(nullptr);
//	gsLastAccess = now;
//	string timeFile = getConfigDir() + "whole.data";
//	char xxx[256] = {0};
//	char* p = xxx;
//	FILE* fp = fopen(timeFile.c_str(), "wb");
//	if ( fp == NULL )
//	{
//		return;
//	}
//	p += sprintf(p, "{\"gstime\":%ld}",gsLastAccess);
//	fwrite( (void*)xxx, p-xxx, 1, fp);
//	fclose(fp);
//}
/*
@读取配置文件中gameserver最后运行时间
*/
//void JSON::initGstime()
//{
//	string strIniFile = getConfigDir() + "whole.data";
//	FILE* fp = fopen(strIniFile.c_str(), "rb");
//	if (fp == NULL)
//	{
//		return;
//	}
//	char xxx[256] = {0};
//	char *p = xxx;
//	int bytes = fread(p,1,256,fp);
//	rapidjson::Document document;
//	if (document.Parse<0>( string( (char*)p, bytes ).c_str() ).HasParseError())
//		return ;
//	int res;
//	//WORD monID;
//	if ( ( res = getJsonValueInt( document, "gstime" )) != -1 )
//	{
//		gsLastAccess = (time_t)res;
//	}
//	//CONFIG::ACTIVITY_INFO_CFG* actCfg = CONFIG::getActivityInfoCfg( ACTIVITY::WORLD_BOSS_FIGHT );
//	//if ( actCfg )
//	//{
//	//	monID = actCfg->monster_id;
//	//}
//	//
//	//if ( ( res = getJsonValueInt( document, "bossid" )) != -1 && res != 0 )
//	//{
//	//	monID = WORD(res);
//	//}
//	//assert( monID != 0);
//	//ACTIVITY::setBossFightID( monID );
//	fclose(fp);
//}

//错误-1
int JSON::getJsonValueInt(rapidjson::Document &document, const char* key)
{

	if ( !document.IsObject())
		return -1;

	if ( !document.HasMember(key))
		return -1;

	if ( !document[key].IsInt() )
		return -1;

	return document[key].GetInt();
}