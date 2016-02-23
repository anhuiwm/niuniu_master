#include "log.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 512
#endif


#ifndef _WIN32
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <algorithm>
//#define LOG_OPEN_FLAG	(O_WRONLY|O_CREAT|O_APPEND|O_LARGEFILE)

#else

#include "head.h"

//#define LOG_OPEN_FLAG	(O_WRONLY|O_CREAT|O_APPEND)

#endif

#include <numeric>
#include <functional>
#include <iostream>


using namespace std;


int readNBytes(int fd, void* buf, int nLen)
{
	int	nLeft, nGet;
	BYTE* pBuf;

	pBuf = (BYTE*)buf;
	nLeft = nLen;
	while (nLeft > 0)
	{
		if ( (nGet = read(fd, pBuf, nLeft)) < 0 ) 
		{
		
//#if defined(__linux__)
//			if( errno == EINTR || IS_E_AGAIN )
//			{
//				nGet = 0; // call read again
//			}else
//#else
//			if( IS_E_AGAIN )
//			{
//				nGet = 0; // call read again
//			}else
//#endif
//			{
//				//CLOG_INFO("Read error, code = %d", socket_errno);
//				return (-1);
//			}


			if( errno == EINTR || errno== EAGAIN )
			{
				nGet = 0; // call read again
			}


		}
		else
		{
			if (nGet == 0)
				break; // EOF
		}
		nLeft -= nGet;
		pBuf += nGet;
	}
	return (nLen - nLeft);
}


int writeNBytes(int fd, char *buf, unsigned int nbyte)
{
	int ret;
	char *p = buf;

	for (; nbyte; p += ret, nbyte -= ret) {
		
		ret = write(fd, p, nbyte);

		if ( ret < 0 ) {

//			if ( errno == EINTR || errno == EAGAIN ) {
//				ret = 0;
//				continue;
//			}
			
			return -1;
		}
	}
	
	return (p - buf);
}

#if 0

static const char *setProgName(const char * name)
{

	static const char *szProgramName;

	if ( name == NULL)
		return szProgramName;

	szProgramName = name;
	return NULL;
}

void logInit(int argc, char** argv)
{
#ifndef WIN32

	const char *szProgramName;

	srandom( time(NULL) + getpid() );
	srand( time(NULL) + getpid() );

	( szProgramName = strrchr( argv[ 0 ], '/' ) ) ? szProgramName++ :  szProgramName = argv[ 0 ];

	setProgName(szProgramName);
	
#endif
}

const char *getProgName(void)
{
	return setProgName(NULL);
}

#endif

#define ERROR_TIME "--error time--"

string getTimeStr(time_t nTime)
{
	char szTimeBuf[MAX_FORMAT_LEN];
	struct tm tm;


#ifdef WIN32

	if ( _localtime32_s( &tm, &nTime) !=0 )
		return ERROR_TIME;

#else

	if ( NULL == localtime_r(&nTime, &tm) )
		return ERROR_TIME;
	
#endif

	strftime(szTimeBuf, MAX_FORMAT_LEN, "%Y-%m-%d %H:%M:%S", &tm);
	return szTimeBuf;//copy out
}


char* getTimeStr2(char* szTimeBuf, time_t nTime)
{
	struct tm tm;

#ifdef WIN32

	if ( _localtime32_s( &tm, &nTime) !=0 )
		return NULL;

#else

	if ( NULL == localtime_r(&nTime, &tm) )
		return NULL;
	
#endif

	strftime(szTimeBuf, MAX_FORMAT_LEN, "%Y-%m-%d %H:%M:%S", &tm);
	return szTimeBuf;//copy out
}




void setTimeInfo(time_t tWhen, int& y, int& mon, int& d, int& h, int& m, int& s)
{
//	time_t tWhen = time(NULL);

	struct tm tm;

#ifdef WIN32

	if ( _localtime32_s( &tm, &tWhen) !=0 )
		return;

#else
	if ( NULL == localtime_r(&tWhen, &tm) )
		return;
	
#endif

	y = tm.tm_year+1900;
	mon = tm.tm_mon + 1;
	d = tm.tm_mday;

	h = tm.tm_hour;
	m = tm.tm_min;
	s = tm.tm_sec;
}


static int getLogInfo(const char*szLogLv, char* szLogfile, char* szHeadFromat)
{
	int y, mon, d, h, m, s;
	setTimeInfo( time(NULL), y, mon, d, h, m, s);

//#ifdef WIN32
//	sprintf(szLogfile,  "%s\\Log\\%04d-%02d\\%s%04d-%02d-%02d.txt", getProgDir(),  y, mon, szLogLv, y, mon, d );
//#else
//	sprintf(szLogfile,  "%s/Log/%04d-%02d/%s%04d-%02d-%02d.txt", getProgDir(),  y, mon, szLogLv, y, mon, d );
//#endif

	sprintf(szLogfile,  "%s" STR_FSEP "Log" STR_FSEP "%04d-%02d" STR_FSEP "%s%04d-%02d-%02d.txt", getProgDir(),  y, mon, szLogLv, y, mon, d );

	{
		char szDir[PATH_LENGTH];
		char* p=szDir;
	
		p += sprintf(p, "%s" STR_FSEP "Log" STR_FSEP, getProgDir());
		if ( -1 == MKDIR(szDir, 0755) && errno != EEXIST )
			return 0;

		p += sprintf(p, "%04d-%02d" STR_FSEP,  y, mon );
		if ( -1 == MKDIR(szDir, 0755) && errno != EEXIST )
			return 0;	
	}
	
	return sprintf(szHeadFromat, "[%02u:%02u:%02u]: ", h, m, s);

	//printf("%s\n", szHeadFromat);
}


void logSQLDiff(char* alQuery, unsigned int dwDiff)
{
	alQuery[40] = 0;

	char szLogfile[PATH_MAX];

	time_t tNow = time(NULL);

	int y, mon, d, h, m, s;
	setTimeInfo( tNow, y, mon, d, h, m, s);
	
	char *p = szLogfile;

	p += sprintf(szLogfile, "%s" STR_FSEP "%04d-%02d", getProgLogDir(2), y, mon );
	if ( -1 == MKDIR(szLogfile, 0755) && errno != EEXIST )
		return;
	
	p += sprintf(p,  STR_FSEP "%04d-%02d-%02d-%02d.txt", y, mon, d, h );
	

	*(DWORD*)(alQuery+40) = dwDiff;
	*(DWORD*)(alQuery+44) = tNow;
	
	int fd = open(szLogfile, LOG_OPEN_FLAG, 0666);
	if ( fd != -1 )
	{
		writeNBytes(fd, alQuery, 48);
		close(fd);
	}
	
}

static int procExeDir(char *program_dir)
{

#ifndef WIN32

	char source_link[PATH_LENGTH];
	char path[PATH_LENGTH];
	int ret;
	
	sprintf(path,"/proc/%d/exe",getpid());
	
	if ( (ret = readlink(path, source_link, PATH_LENGTH-1) )!= -1 ) {
		source_link[ret]=0;

		*(rindex(source_link, '/') +1)=0;
		
		strcpy(program_dir, source_link);		
		return 0;
	}
	return -1;

#else

	 ::GetCurrentDirectory( PATH_LENGTH, program_dir );
	return 0;

#endif
}


const char* getProgDir(void)
{
	static char szProgdir[PATH_LENGTH]={0};

	if ( szProgdir[0] )
		return szProgdir;


#ifdef WIN32
		 ::GetCurrentDirectory( PATH_LENGTH, szProgdir );
#else
		procExeDir(szProgdir);

#endif
	
	return szProgdir;
}


const char* getProgLogDir(int nWhat)
{
	static char szProgLogDir[PATH_LENGTH]={0};
	static char szProgSQLDir[PATH_LENGTH]={0};

 (
	sprintf( szProgLogDir, "%s" STR_FSEP "Log", getProgDir() ),
	sprintf( szProgSQLDir, "%s" STR_FSEP "SQL", getProgDir() ),
	MKDIR(szProgLogDir, 0755),
	MKDIR(szProgSQLDir, 0755),
	1
	);
	
	return nWhat == 1 ? szProgLogDir : szProgSQLDir ;
}

const char* __FILENAME__(const char* fname)
{
#ifdef __linux__
	return fname;
#endif

	char* _file = (char*)fname;
	
	//only vc need this
	char* _fn = strrchr(_file, '\\');
	if( !_fn )
		_fn = _file;
	else
		_fn++;
	
	return _fn;	
}

void xLog(const char* szLogLv, const char* fmt, ... )
{
//	char buf[4096+1024];
	char buf[64024];
	char* p = buf;

	char szLogfile[PATH_MAX];

	//getLogInfo(szLogfile, szHeadFormat);
	
	p += getLogInfo(szLogLv, szLogfile, p);

	//p += sprintf(p, "[%s]: ", szHeadFormat);

	va_list	ap;
	va_start(ap, fmt);
	
	int n = vsnprintf(p, sizeof(buf)-PATH_MAX, fmt, ap);

	//Thus, a return value of size or more means that the output was truncated. 
	if ( n >= (int)sizeof(buf)-PATH_MAX ) {
		p += (int)sizeof(buf)-PATH_MAX - 1;
		*(p-1) = '\n'; // 0 -> \n
	}
	else
		p += n;

	va_end(ap);

	if ( strcmp(szLogLv, "PRINTF") == 0  )
		writeNBytes(2, buf, p-buf);
	else
	{
		int fd = open(szLogfile, LOG_OPEN_FLAG, 0666);
		if ( fd == -1)
			return;

		writeNBytes(fd, buf, p-buf);
		close(fd);
	}
	

	
}

