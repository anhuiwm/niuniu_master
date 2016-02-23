#ifndef _MYLOG_H
#define _MYLOG_H

#include "typedef.h"

//#define MAX_FORMAT_LEN 32
//#define MAX_LINE_LEN 256

//#define PATH_LENGTH 256

//#include <iostream>

#ifdef WIN32
#include <direct.h>
#define MKDIR(a, b)		_mkdir(a)

#define LOG_OPEN_FLAG	(O_WRONLY|O_CREAT|O_APPEND)

#else
#define MKDIR(a, b)		mkdir(a,b)

#define LOG_OPEN_FLAG	(O_WRONLY|O_CREAT|O_APPEND|O_LARGEFILE)
#endif


#define HEX2ASCII_SPACE2(src,len,tgt) \
	do {\
		char *pxxx=tgt;\
		const char*fk = (const char*)(src); \
		for(int ixxx__=0;ixxx__< (int)(len);ixxx__++) {\
			pxxx += sprintf(pxxx, "%02x", (unsigned char)fk[ixxx__]);\
		}\
	}while(0)


#define HEX2ASCII_SPACE(src,len,tgt) \
	do {\
		char *pxxx=tgt;\
		if ( (int)(len) >= 64024) break;\
		const char*fk = (const char*)(src); \
		pxxx += sprintf(pxxx, "\n");\
		for(int ixxx__=0;ixxx__< (int)(len);ixxx__++) {\
			pxxx += sprintf(pxxx, "%02x ", (unsigned char)fk[ixxx__]);\
			if( (ixxx__+1)%16 == 0 ) \
				pxxx += sprintf(pxxx, "\n");\
			else if( (ixxx__+1)%8 == 0 ) \
				pxxx += sprintf(pxxx, "  ");\
		}\
	}while(0)

#define log_bin2(from,len) \
		do {\
			if (len>64024)break;\
			char buf[64024*4];\
			HEX2ASCII_SPACE(from,(len),buf);\
			logii("%s", buf);\
		}while(0)


#define log_printf_bin2(from,len) \
		do {\
			if (len>64024)break;\
			char buf[64024*4];\
			HEX2ASCII_SPACE(from,(len),buf);\
			fprintf(stderr, "%s\n",buf);\
			fflush(stderr);\
			logii("%s", buf);\
		}while(0)

#define X2 log_bin2
#define XP2 log_printf_bin2

const char* __FILENAME__(const char* fname);
#define V_MUST_BETWEEN( v, mIn, mAx ) ( v>mAx ? mAx : (v<mIn ? mIn : v) )



#define PRINT_LOG(fmt,...) \
	do {\
		xLog( "PRINTF", "{%s, %d}: " fmt "\n", __FILENAME__(__FILE__), __LINE__, ##__VA_ARGS__);\
		logFATAL( fmt, ##__VA_ARGS__);\
	}while(0)
	

//#define PRINTLN(fmt,...) xLog( "PRINTF", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__), __LINE__, ##__VA_ARGS__)
//#define PUTS(fmt,...) xLog( "PRINTF", fmt"\n", ##__VA_ARGS__)

#ifdef _DEBUG
//#define PUTS(fmt,...) xLog( "PRINTF", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__), __LINE__, ##__VA_ARGS__)


#if 0
		fprintf( stderr, "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__), __LINE__, ##__VA_ARGS__);


#define PUTS(fmt,...)\
	do{ \
		xLog( "Info_", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__), __LINE__, ##__VA_ARGS__);\
	}while(0)

#define PUTS(fmt,...) \
	do{ \
	std::cerr<<"{"<<__FILENAME__<<",("<<__FILE__ <<"),"<<__LINE__<<"}", ##__VA_ARGS__);\
	}while(0)




#endif

#if 1	
#define PUTS(fmt,...) \
	do{ \
		xLog( "PRINTF", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__), __LINE__, ##__VA_ARGS__);\
	}while(0)

#else
#define PUTS(fmt,...)
#endif





#else



#define PUTS(fmt,...)

#endif

#define PRINTLN(fmt,...) fprintf( stderr, "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__), __LINE__, ##__VA_ARGS__)


#define lognn() xLog( "Info_", "{%s, %d}: \n", __FILENAME__(__FILE__), __LINE__)

#define logii(fmt, ...) xLog( "Info_", fmt"\n", ##__VA_ARGS__)
#define logff(fmt, ...) xLog( "Fatal_", fmt"\n", ##__VA_ARGS__)
#define logee(fmt, ...) xLog( "Error_", fmt"\n", ##__VA_ARGS__)
#define logww(fmt, ...) xLog( "Warn_", fmt"\n", ##__VA_ARGS__)
#define logxx(fmt, ...) xLog( "Exep_", fmt"\n", ##__VA_ARGS__)

#if 0
#define logplan(fmt, ...) xLog( "planner_", fmt"\n", ##__VA_ARGS__)
#define lognew(fmt, ...) xLog( "new_", fmt"\n", ##__VA_ARGS__)
#else
#define lognew(fmt, ...) 
#define logplan(fmt, ...)
#endif

#if 1
#define logonline(fmt, ...) xLog( "online_", fmt"\n", ##__VA_ARGS__)
#else
#define logonline(fmt, ...)
#endif

#define logg(fmt, ...) xLog( "Info_", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__), __LINE__, ##__VA_ARGS__)


#define logINFO(fmt, ...) xLog( "Info_", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__),  __LINE__, ##__VA_ARGS__)
#define logFATAL(fmt, ...) xLog( "Fatal_", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__),  __LINE__, ##__VA_ARGS__)
#define logERROR(fmt, ...) xLog( "Error_", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__),  __LINE__, ##__VA_ARGS__)
#define logWARN(fmt, ...) xLog( "Warn_", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__),  __LINE__, ##__VA_ARGS__)
#define logEXEP(fmt, ...) xLog( "Exep_", "{%s, %d}: " fmt"\n", __FILENAME__(__FILE__),  __LINE__, ##__VA_ARGS__)



#include <string>
#include <map>
#include <vector>
#include <functional>

std::string getTimeStr(time_t nTime);
char* getTimeStr2(char* szTimeBuf, time_t nTime);

//void logInit(int argc, char** argv);

void xLog(const char* szLogLv, const char* fmt, ... );

const char* getProgDir(void);
const char *getProgName(void);
const char* getProgLogDir(int nWhat);
void logSQLDiff(char* alQuery, unsigned int dwDiff);


int readNBytes(int fd, void* buf, int nLen);
int writeNBytes(int fd, char *buf, unsigned int nbyte);

class CLog
{
public:

	enum E_LOG_TYPE
	{
		LOG_FATAL				= 0,
		LOG_ERROR				= 1,
		LOG_WARN				= 3,
		LOG_INFO				= 3,
		LOG_ALL					= 4,
		LOG_OFF					= 5,	//¹Ø±Õ
	};
};

#endif
