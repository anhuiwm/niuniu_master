#ifndef JSON_H
#define JSON_H

//#include "protocal.h"
//#include "config.h"
#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"	// for stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output

namespace JSON
{
	enum eJSON
	{
		JSON_SUCCESS = 0,     		//³É¹¦
		JSON_NO_EXISTS = 1,
	};

	int getJsonValueInt(rapidjson::Document &document, const char* key);
	void initGstime();
	void writeGsTime();
	bool isNewDay();
	void setLastAccess(time_t now);
}


#endif //JSON_H
