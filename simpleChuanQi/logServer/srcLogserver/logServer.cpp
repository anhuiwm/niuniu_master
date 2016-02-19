
#include <stdio.h>
#include <zlib.h>


#include <algorithm>

#include <stdlib.h>
//#include <mysql/mysql.h>
#include <sys/time.h>
#include <sys/param.h>
#include <string.h>


#include <parallel/compatibility.h>


#include "j2.h"
#include "timer.h"


#include "logDefine.hpp"

#define LOGGER_VERSION                  "1.0.1.1"

//#include <boost/detail/sp_counted_base_gcc_x86.hpp>


#define BREAK_IF(structname) 	\
	if ( nDataLen < sizeof(structname) )\
	{\
		logv("command error:" #structname ", nDataLen:%u, %u", nDataLen, sizeof(structname) );\
		break;\
	}


#define BREAK_IF_CHILD(structname) 	\
	if( nDataLen < sizeof(structname) + p->childCount*sizeof(structname::__CHILD) )\
	{\
		logv("command error:" #structname);\
		break;\
	}

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

static string PRINT_BLOB2(const char* src, int n)
{
	char tgt[2048]={0};
	
	char *pp = tgt;
	for(int i=0; i<n; i++)
		pp += sprintf(pp, "%02x", (unsigned char)src[i]);

	return string( tgt, pp-tgt);

}


//========================================================================

#define LOGGER_PROG_NAME				("LOGGER SERVER")
#define LOGS_PARSE_THREAD_COUNT		1

namespace LoggerServer {

	short				m_nPort;
	DWORD				m_dwListenBigEndianIP;
	list<DWORD>			m_lstTrustIP;

	list<STC_IP_CHECK>	m_lstTrustIP_Star;


	sem_t				m_semHavePacket[LOGS_PARSE_THREAD_COUNT];
	
	CRITICAL_SECTION	m_csDequeue[LOGS_PARSE_THREAD_COUNT];
	DEQ_RGPKT			m_deqPacket[LOGS_PARSE_THREAD_COUNT];

	sem_t				m_semSenderExit;

	int					m_SocketPair[2];
	bool				m_bMainloopExit;

	MAP_GS_PKT_EXEC 	m_mapPktFunc;

//	map<WORD, const char*>	m_mapEventDirName;

	MAP_SOCK_PRO 		m_mapSockPro;
	
//	static jmp_buf		m_jbExitCmd;

	static __int64 nEventId;
	static int m_nLogFileInterval;
	static FILE* m_fpEventID;
	
	SOCKET				m_SockListen;

	void ReadIniFile(void* pv);
	void ExecLogEvent( SOCKET nSock, WORD wCmdType, STC_CMD_GS* pStcCmd, WORD pktLen);

//========================================================================
inline void DispatchLoggerServer(SOCKET nSock, const char* pFrom, WORD wDataLen)
{
	static DWORD nRoundRobin;
	DWORD i = 0;//nRoundRobin++ % LOGS_PARSE_THREAD_COUNT;

	PSTC_Socket_And_CMD_GS pkt = (PSTC_Socket_And_CMD_GS) malloc( sizeof(SOCKET) + wDataLen );
	if(pkt==NULL)
		return;
	
	pkt->from = nSock;
	memcpy( &pkt->stc_cmd_gs, pFrom, wDataLen);

	POST_MESSAGE(m_csDequeue[i], m_deqPacket[i], pkt, m_semHavePacket[i]);

}


//DEF_ForgePktAndPost(LoggerServer, LoggerServer, 1, 2, STC_CMD_GS_HEAD_LEN+1);

DEF_ForgePktAndPost(LoggerServer, LoggerServer, 0, 0, 4);

int EpollRead_LOGS(int argc, char** argv)
{
	pthread_rwlock_t	m_rwlockBig;
	pthread_rwlock_init(&m_rwlockBig, NULL);

	struct epoll_event events[FEW_EPOLL_NEVENTS];
	int epfd = Epoll_create(FEW_EPOLL_NEVENTS);

	EPOLL_ADD(epfd, EPOLLIN, m_SocketPair[SOCK_PAIR_CMD_READ], m_mapSockPro,  E_SOCK_PROPERTY_CMD_SOCK);
	EPOLL_ADD(epfd, EPOLLIN, m_SockListen, m_mapSockPro, E_SOCK_PROPERTY_LOGGER_LISTEN);
	
	STC_CMD_EXTRA 		m_stcCmdExt;
//	m_stcCmdExt.Init(&m_jbExitCmd, &m_mapSockPro, HEART_BEAT_INTERVAL_RDBGS2LOG, E_SOCK_PROPERTY_LOGGER_SOCK, NULL, epfd);
	m_stcCmdExt.Init(NULL, &m_mapSockPro, HEART_BEAT_INTERVAL_RDBGS2LOG+30, E_SOCK_PROPERTY_LOGGER_SOCK, NULL, epfd);
	m_stcCmdExt.Reg_Cmd_Func(E_SOCKETPAIR_UPDATE_TRUSTIP, ReadIniFile);
	
//	if (setjmp(m_jbExitCmd) != 0)
//		return -1;

	while ( int nfds = epoll_wait(epfd, events, FEW_EPOLL_NEVENTS, -1 ) ) { /*no timeout, no zero return */ //epoll_pwait

		if( nfds == -1 && errno == EINTR )
			continue;
		
		for(int i=0;i<nfds;i++) {
			int fd					= events[i].data.fd;
			E_SOCK_PROPERTY flag	= m_mapSockPro[fd].sock_flag;
			switch(flag) {
				
				case E_SOCK_PROPERTY_LOGGER_SOCK:
					if( -1 == Recv_Until_EAGAIN(fd, m_mapSockPro[fd].szBuffer, m_mapSockPro[fd].szCur, ForgePktAndPost_LoggerServer)) {
						OnClose_(fd, m_mapSockPro);
					}
					else
						m_mapSockPro[fd].nTick = time(NULL);
						
					
					break;
					
				case E_SOCK_PROPERTY_CMD_SOCK:
					if ( -1 == m_stcCmdExt.CMD(fd, epfd, &m_rwlockBig) )
						return 0;
					
					break;
					
				case E_SOCK_PROPERTY_LOGGER_LISTEN:
//					if( ( fd = Accept(fd, m_lstTrustIP)) != -1 ) {

					if( (fd = Accept(fd, m_lstTrustIP_Star)) != -1 ) {
						
						Allocate_Sock_Pro(m_mapSockPro, fd);
						EPOLL_ADD( epfd, EPOLLIN, fd, m_mapSockPro, (E_SOCK_PROPERTY)(flag<<1) );// take care, sequence of E_SOCK_PROPERTY
					}
					break;
					
			}
		}
	}
}

//========================================================================
#define LOG_HEART_BEAT E_LOG_FIRST

void* ThdLoggerParseData(void* arg)
{

	int nThread = (int)arg;

	PSTC_Socket_And_CMD_GS pkt;
	while ( GET_MESSAGE(m_semHavePacket[nThread], m_csDequeue[nThread], m_deqPacket[nThread], pkt) ) {

		WORD wCmdType = pkt->stc_cmd_gs.wCmdType;
		
		//MAP_GS_PKT_EXEC::iterator it = m_mapPktFunc.find(wCmdType);
		//if (it != m_mapPktFunc.end())
		//	it->second(mysql, pkt->from, &(pkt->stc_cmd_gs), pkt->stc_cmd_gs.nLen-4);

		WORD wBig = wCmdType & ~0x8000;
		
		if( wBig > E_LOG_FIRST && wBig < E_LOG_LAST ) {

			//printfV("pkt->stc_cmd_gs.nLen:%d\n", pkt->stc_cmd_gs.nLen);
			//ExecLogEvent( pkt->from, wCmdType, pkt->stc_cmd_gs.data, pkt->stc_cmd_gs.nLen-4);
			ExecLogEvent( pkt->from, wCmdType, &(pkt->stc_cmd_gs), pkt->stc_cmd_gs.wLen );
		}
		else if( wCmdType == LOG_HEART_BEAT ) {

		
			//debugv("LOG_HEART_BEAT");
			//system("date");
		}
		else if( wCmdType == E_CENTRAL_STOP_THREAD ) {
			break;
		}
		else
			logx("unknown wCmdType:%d", wCmdType);

		FREE_PKT(pkt);
	}
	
	//MYSQL_UNINIT(mysql);
	Sem_post(&m_semSenderExit);
}

//========================================================================
//int nn = (int) src[n-1];\

//void ExecLogEvent( SOCKET nSock, WORD wCmdType, const char* pkt, int pktLen)
void ExecLogEvent( SOCKET nSock, WORD wCmdType, STC_CMD_GS* pStcCmd, WORD pktLen)
{
	//printfV("wCmdType:%d, nDataLen:%d", wCmdType, nDataLen);

//char x[1000]={0};
//HEX2ASCII_SPACE(data,nDataLen,x);
//printfV("\n%s", x);

	char szLogfile[MAX_PATH_LEN]={0};
	GetEventLogFileName( szLogfile, 65535, m_nLogFileInterval );


	int fd = open(szLogfile, O_WRONLY|O_CREAT|O_APPEND, 0666);
	if ( fd == -1 )
		return;

/*
	char *dst = NULL;
	nDataLen -= LEN_WORD;

	if( wCmdType & 0x8000 ) {

		unsigned long orig_len = *(WORD*)data;
		unsigned long dst_size = ESTIMATE_COMPRESS_BOUND( orig_len );
		dst = new char[ dst_size ];

		if( Uncompress(dst, &dst_size, data+LEN_WORD, nDataLen) != Z_OK ) {
			debug_log("uncompress error:%d", dst_size);
			delete [] dst;
			return;
		}
		data = dst;
		nDataLen = dst_size;
	}
	else {
		data += LEN_WORD;
	}	
*/

	const char* data = (const char* )pStcCmd;
	WORD nDataLen = pktLen;// - 4;
	
	switch(wCmdType & ~0x8000) {
		
		#define xfprintf(fp, fmt, type, a...) \
			do {\
				char buf[4096];\
				int nLen = snprintf(buf, 4096, fmt, type, a);\
				if ( nLen != WriteNBytes(fd, buf, nLen) )\
					debug_log("write log break:%d", type);\
			}while(0)

				
#include "logCase.hpp"



	}

#if 0
	fclose(fp);
#else

	close(fd);


#endif

	//delete [] dst;
	
}


//========================================================================

void CloseDeadConn_CB(const string * s)
{
	SEND_CMD_TO_MAINLOOP(E_SOCKETPAIR_CLEAR_CONN);
}

//========================================================================

void ReadIniFile(void *pv)
{
	const char* szFileName = GetConfigFile();

	m_nPort = GetPrivateProfileInt("server", "ListenPort", 6400, szFileName);

	m_dwListenBigEndianIP = GetIniStr__("server", "ipself", "0.0.0.0", CStr2BigendianIP);

	//--------------
	m_lstTrustIP.clear();
	ReadSectionLines(szFileName, "trustip", EachLineToBigIP_func, &m_lstTrustIP);


	m_lstTrustIP_Star.clear();
	ReadSectionLines(szFileName, "trustip", EachLineToBigIP_Star_func, &m_lstTrustIP_Star);


//for(list<DWORD>::iterator it =	m_lstTrustIP.begin(); it!=m_lstTrustIP.end(); ++it) 
//debugv("%u", *it);

	m_nLogFileInterval = GetPrivateProfileInt("log_file_interval", "mins", 60, szFileName);
	//debugv("%d, %s", m_nLogFileInterval, szFileName);


	static TIMERID_t t_CloseDead_CB	= TADD( TRIGGER(time(NULL)+10, CloseDeadConn_CB, NULL, HEART_BEAT_INTERVAL_RDBGS2LOG+60, true) );

}
//========================================================================

int UnInit(int argc, char** argv)
{
	for(int i=0;i<LOGS_PARSE_THREAD_COUNT;i++) {
		PSTC_Socket_And_CMD_GS pkt = MallocGSPacket(0, E_CENTRAL_STOP_THREAD, NULL, 0);	
		POST_MESSAGE(m_csDequeue[i], m_deqPacket[i], pkt, m_semHavePacket[i]);
	}

	int threads = LOGS_PARSE_THREAD_COUNT;
	while( threads-- > 0)
		Sem_wait(&m_semSenderExit);


	//sem_destroy(&m_semHavePacket);
	//sem_destroy(&m_semSenderExit);
	//fclose(m_fpEventID);	
}
//========================================================================


int Init(int argc, char** argv)
{

	ReadIniFile(NULL);

	m_bMainloopExit = false;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0,  m_SocketPair) == -1)
		EXIT(EXIT_INIT);

	Sem_init(&m_semSenderExit, 0, 0);
		
	for (int i=0; i<LOGS_PARSE_THREAD_COUNT; i++) {
		InitializeCriticalSection(&m_csDequeue[i]);
		Sem_init(&m_semHavePacket[i], 0, 0);
	}
	
	
//#include "log_dirs.hpp"


	for(int i=0; i<LOGS_PARSE_THREAD_COUNT; i++)
		Pthread_create_detached( ThdLoggerParseData, (void*)i);

	//SOCKET listen_sock = TCPServerListen(LoggerServer::m_stcBaseData.nPortGS, 1, LoggerServer::m_stcBaseData.dwListenGsBigEndianIP);
	m_SockListen = TCPServerListen(m_nPort, 1, m_dwListenBigEndianIP);
	if(m_SockListen == -1)
		EXIT(EXIT_INIT);
}

static void term_handler(int signo)
{
	LoggerServer::m_bMainloopExit = true;
	
	SEND_CMD_TO_MAINLOOP(E_SOCKETPAIR_EXIT_THREAD);
}

static void hup_handler(int signo)
{
	SEND_CMD_TO_MAINLOOP(E_SOCKETPAIR_UPDATE_TRUSTIP);
}

static void quit_handler(int signo)
{
	char szLogfile[MAX_PATH_LEN];
	GetEventLogFileName( szLogfile, 65535, LoggerServer::m_nLogFileInterval );
	debugv("%s", szLogfile);
}

}//namesapce LoginServer

//========================================================================



int main(int argc, char**argv)
{

	return Server_Main(argc, argv, LOGGER_PROG_NAME, LOGGER_VERSION, 
							LoggerServer::Init, LoggerServer::UnInit, NULL, LoggerServer::EpollRead_LOGS,
							LoggerServer::term_handler, LoggerServer::hup_handler, LoggerServer::quit_handler);

	
}

