
#include "logIntf.h"
#include "log.h"

#include <algorithm>
#include "zlibapi.h"
#include <time.h>

#include <fcntl.h>


#include "../game/logDefine.hpp"


#define BREAK_IF(structname) 	\
	if ( pktLen < sizeof(structname) )\
	{\
		logff("command error:" #structname ", nDataLen:%u, %u", pktLen, sizeof(structname) );\
		break;\
	}


#define BREAK_IF_CHILD(structname) 	\
	if( pktLen < sizeof(structname) + p->childCount*sizeof(structname::__CHILD) )\
	{\
		logff("command error:" #structname);\
		break;\
	}

#define GetLogTime getTimeStr


//inline  , => \,       \ => \\     \n => \E   
static string EscapeLogStr2(const char* szSrc)
{
	char szTgt[1024]={0};
	
	char* p = szTgt;
	char c;
	
	while( (c = *szSrc++) ) {
		
		if( c == ',' ) {
			*p++ = '\\';
			*p++ = ',';
		}
		else if( c == '\\' ) {
			*p++ = '\\';
			*p++ = '\\';
		}
		else if( c == '\n' ) {
			//*p++ = '\t';
			*p++ = '\\';
			*p++ = 'E';
		}
		/*else if( *szSrc == '\t' ) {
			*p++ = '\t';
			*p++ = '\t';
		}*/
		else
			*p++ = c;

	}

	return string( szTgt, p-szTgt);
}

#if 0
static string PRINT_BLOB2(const char* src, int n)
{
	char tgt[2048]={0};
	
	char *pp = tgt;
	for(int i=0; i<n; i++)
		pp += sprintf(pp, "%02x", (unsigned char)src[i]);

	return string( tgt, pp-tgt);

}
#endif

//2009-05-12-13:59:30"
//2009-05-12-13.txt"
#define Y_OFFSET (szXXXX+0)
#define M_OFFSET (szXXXX+5)
#define D_OFFSET (szXXXX+8)
#define HH_OFFSET (szXXXX+11)
#define MM_OFFSET (szXXXX+14)
#define SS_OFFSET (szXXXX+17)

static void GetEventLogFileName(char* szLogfile, int nChannel, int nMinInterval)
{
	char szXXXX[MAX_FORMAT_LEN]={0};


	time_t tNow = time(NULL);

	if( nMinInterval!=0 ) {
		int secs = nMinInterval * 60;
		tNow = ( tNow ) / secs * secs;
	}
	
	//GetTime(szXXXX, "%Y-%m-%d-%H:%M:%S", nMinInterval);

	getTimeStr2( szXXXX, tNow );

	*(M_OFFSET-1) = *(D_OFFSET-1) = *(HH_OFFSET-1) = *(MM_OFFSET-1) = *(SS_OFFSET-1) = 0;
	
	char *p = szLogfile;
	p += sprintf(p, "%s/Log/%s%s%s/", getProgDir(), Y_OFFSET, M_OFFSET, D_OFFSET);

	//if (CreateDir(szLogfile) < 0)
	if ( -1 == MKDIR(szLogfile, 0755) && errno != EEXIST )
		return;

	//sprintf(p,"%d_%s%s%s_%s%s.txt", nChannel, Y_OFFSET, M_OFFSET, D_OFFSET, HH_OFFSET, MM_OFFSET );
	sprintf(p,"%s%s%s_%s%s.txt", Y_OFFSET, M_OFFSET, D_OFFSET, HH_OFFSET, MM_OFFSET );

}

static void writeLocal( WORD wCmdType, const char* data, WORD pktLen )
{
	//printfV("wCmdType:%d, nDataLen:%d", wCmdType, nDataLen);

//char x[1000]={0};
//HEX2ASCII_SPACE(data,nDataLen,x);
//printfV("\n%s", x);

	char szLogfile[512]={0};
	GetEventLogFileName( szLogfile, 65535, 60 );


	int fd = open(szLogfile, O_WRONLY|O_CREAT|O_APPEND, 0666);
	if ( fd == -1 )
		return;
	
	switch(wCmdType & ~0x8000) {
		
		#define xfprintf(fp, fmt, type, ...) \
			do {\
				char buf[4096];\
				int nLen = snprintf(buf, 4096, fmt, type, ##__VA_ARGS__);\
				if ( nLen != writeNBytes(fd, buf, nLen) )\
					printf("write log break:%d", type);\
			}while(0)

#include "../game/logCase.hpp"

	}

	close(fd);

}


//-------------------------------------------------LogIntf-----------------------------------------------------

LogIntf *LogIntf::_logIntf = nullptr; // pointer to singleton

LogIntf::LogIntf()
{

	_logSock= INVALID_SOCKET;
	_needQuit = false;
	_logIP = "";
	_logPort = 0;
	
	auto t = std::thread(std::bind(&LogIntf::logThread, this));
	t.detach();
}


LogIntf::~LogIntf()
{
	_needQuit = true;
	_sleepCondition.notify_one();

}


LogIntf* LogIntf::getInstance()
{
	if (_logIntf == nullptr) 
	{
		_logIntf = new LogIntf();
	}

	return _logIntf;
}

void LogIntf::send(const LogIntf::LOG_MESSAGE& message)
{

	_queueMutex.lock();
	_queue.push_back(message);
	_queueMutex.unlock();

	_sleepCondition.notify_one();

}

void LogIntf::sendToLogsvr( const list<LogIntf::LOG_MESSAGE > &queue )
{

	for ( auto it = queue.begin(); it!=queue.end(); ++it )
	{

		const LOG_MESSAGE& msg = *it;

		//reload 改变logserver地址
		LOG_RESET_LOGSERVER* logProto = (LOG_RESET_LOGSERVER*)msg.pkt;
		
		if ( logProto->wCmdType == RESET_LOGSERVER_CMD )	
		{
			//ip(dword),port(int32),不检查LEN

			_logIP = BigendianIP2Str( logProto->IP );
			_logPort = logProto->port;

			if ( _logSock != INVALID_SOCKET )
				closesocket( _logSock );

			_logSock = BASE::ConnectServer( _logIP.c_str(), _logPort );
			continue;
		}
		

		if ( _logSock != INVALID_SOCKET )
		{
			if ( msg.pktLen == BASE::Write( _logSock, msg.pkt, msg.pktLen ) )
			{
				free(msg.pkt);
				continue;
			}
			
			closesocket( _logSock );
			_logSock = INVALID_SOCKET;
		}

		//writeFile
		writeLocal( *(WORD*)( msg.pkt + sizeof(WORD) ), msg.pkt, msg.pktLen );

		free(msg.pkt);
	
	}

	if ( _logSock == INVALID_SOCKET && _logPort!=0 )
	{
		_logSock = BASE::ConnectServer( _logIP.c_str(), _logPort );
	}


}


void LogIntf::logThread()
{

	std::mutex sleepMutex;
	std::unique_lock<std::mutex> lk(sleepMutex);
	
	while (!_needQuit)
	{

		if( _sleepCondition.wait_for(lk, std::chrono::seconds(10)) != std::cv_status::timeout )
		{

			_queueMutex.lock();
				_queue.swap( _queue2 );
			_queueMutex.unlock();

			sendToLogsvr( _queue2 );
			_queue2.clear();
	
		}
		else	//10 seconds reconnect
		{

			//如果logserver主动关闭...
			if ( _logSock != INVALID_SOCKET )
			{				
				static LOG_HEARTBEAT heartBeat;
				if ( sizeof(heartBeat) != BASE::Write( _logSock, &heartBeat, sizeof(heartBeat) ) )
				{
					closesocket( _logSock );					
					_logSock = INVALID_SOCKET;
				}
			}

			
			if ( _logSock == INVALID_SOCKET && _logPort!=0 )
			{
				_logSock = BASE::ConnectServer( _logIP.c_str(), _logPort );
			}

		}
	}

//防止


}


