#ifdef __linux__

#ifndef J2_H
#define J2_H


#include <list>
#include <string>
#include <vector>
#include <deque>
#include <map>

#include <sys/stat.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>


#include <semaphore.h>
#include <sys/epoll.h>
#include <setjmp.h>

#include <signal.h>


#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#if 0
typedef unsigned long long UINT64;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef void *HANDLE;
typedef int SOCKET;
typedef void *HINSTANCE;
typedef unsigned int UINT;
typedef unsigned int WPARAM;
typedef long LPARAM;
typedef long long __int64;

#else
#include "typedef.h"

#endif


enum
{
	LEN_BYTE					= 1,
	LEN_WORD					= 2,
	LEN_DWORD					= 4,
	LEN_FLOAT					= 4,
	LEN_INT64					= 8,
};
	
typedef enum _E_SOCKETPAIR_CMD{

	E_SOCKETPAIR_NONE, 

	E_SOCKETPAIR_EXIT_THREAD,

	E_SOCKETPAIR_UPDATE_TRUSTIP,

	E_SOCKETPAIR_CLEAR_CONN,
	E_SOCKETPAIR_AGENT_ADD,


	E_SOCK_HEARTBEAT_CMD,



	E_SOCKETPAIR_TIMER_BEGIN  = 100,
	E_SOCKETPAIR_TIMER_END  = 150,

	E_SOCKETPAIR_CMD_MAX = 255,

}E_SOCKETPAIR_CMD;
typedef enum _E_SOCK_PROPERTY { //take care , sequence


	E_SOCK_PROPERTY_LOGGER_LISTEN = 0x40,//no use
	E_SOCK_PROPERTY_LOGGER_SOCK = 0x80,


	E_SOCK_PROPERTY_CMD_LISTEN = 0x400,
	E_SOCK_PROPERTY_CMD_SOCK = 0x800,

	
	E_SOCK_PROPERTY_ERROR

}E_SOCK_PROPERTY;


typedef pthread_mutex_t CRITICAL_SECTION;

typedef struct _STC_CMD_GS
{
//	char	parentheses; //'('
	WORD	wLen;
	WORD	wCmdType;
	char	data[0];
}__attribute__((packed)) STC_CMD_GS, *PSTC_CMD_GS;


typedef struct _STC_Socket_And_CMD_GS {
	SOCKET	from;
	STC_CMD_GS stc_cmd_gs;
}__attribute__((packed)) STC_Socket_And_CMD_GS , *PSTC_Socket_And_CMD_GS;


typedef deque<PSTC_Socket_And_CMD_GS> DEQ_RGPKT;


typedef void (*GS_PKT_EXEC)( SOCKET nGs, WORD wCmdType, const char* data, int nDataLen);
typedef map<WORD, GS_PKT_EXEC> MAP_GS_PKT_EXEC;

typedef	void	Sigfunc(int);

typedef struct _STC_SOCK_PRO {

	E_SOCK_PROPERTY sock_flag;

	char *szBuffer;//[ MAX_READ_LEN*2 ];
	char * szCur;
	
	int offset;
	pthread_mutex_t lock;
	struct msghdr * msghdr;

	char* szSendBuf;
	char* szFrom;//FROM
	char* szTo;//TO
	bool bFull;

	int nTick;

	_STC_SOCK_PRO() : szBuffer(NULL), szCur(NULL), offset(0), msghdr(NULL) {};
	//_STC_SOCK_PRO() : szCur(szBuffer+MAX_READ_LEN), offset(0), msghdr(NULL) {};
	
}STC_SOCK_PRO;


typedef void (*CMD_EXEC)(void* pStcData);

typedef map<BYTE, CMD_EXEC> MAP_CMD_FUNC;
typedef map<SOCKET, STC_SOCK_PRO> MAP_SOCK_PRO;

typedef struct _STC_CMD_EXTRA {

	jmp_buf			*pJmpBuf;
	MAP_SOCK_PRO	*pMapSockPro;

	int				nHeartBeat;
	
	void			*pvIni;
	
	E_SOCK_PROPERTY	eTimeOutCare;

	MAP_CMD_FUNC	mapCmdFunc;

	int				nEpollFd;

	void Init(jmp_buf *pJmpBuf, MAP_SOCK_PRO	*pMapSockPro, int nHeartBeat, E_SOCK_PROPERTY eTimeOutCare, void *pvIni, int nEpollFd);	
	void Reg_Cmd_Func(BYTE byCmd, CMD_EXEC func);
	void ShutDown_No_HeartBeat(MAP_SOCK_PRO *m, int nHeartBeat, E_SOCK_PROPERTY e);
	//void CMD(SOCKET fd, int epfd, pthread_rwlock_t *lock);
	int CMD(SOCKET fd, int epfd, pthread_rwlock_t *lock);
	

}STC_CMD_EXTRA, *PSTC_CMD_EXTRA;


typedef	map<string, map<string,string> > SECTION_t;

typedef string& (*TRIM_FUNC)(string& str, const char* trim_char);

typedef struct _STC_IP_CHECK {
	DWORD dwBigEndIP;
	vector<int> vecStar;
}STC_IP_CHECK;


typedef int (*SERVER_INIT_UINIT)( int argc, char** argv );

typedef MAP_SOCK_PRO::iterator MAP_SOCK_PRO_IT;

typedef const char* (*ForgePktAndPost_t)(const char* pFrom, DWORD &nLen, SOCKET nSock);
typedef int (*EACH_LINE_FUNC)(const char*from, const char*to, void* para);

#if 0



#if 0
#define HEX2ASCII_SPACE(src,len,tgt) \
	do {\
		char *pxxx=tgt;\
		if ( (len) >= 1024) break;\
		const char*fk = (const char*)(src); \
		pxxx += sprintf(pxxx, "\n");\
		for(int ixxx__=0;ixxx__< (len);ixxx__++) {\
			pxxx += sprintf(pxxx, "%02x ", (unsigned char)fk[ixxx__]);\
			if( (ixxx__+1)%16 == 0 ) \
				pxxx += sprintf(pxxx, "\n");\
			else if( (ixxx__+1)%8 == 0 ) \
				pxxx += sprintf(pxxx, "  ");\
		}\
	}while(0)

#else
#define HEX2ASCII_SPACE(src,len,tgt) \
	do {\
		char *pxxx=tgt;\
		if ( (len) >= 1024) break;\
		const char*fk = (const char*)(src); \
		for(int ixxx__=0;ixxx__< (len);ixxx__++) {\
			pxxx += sprintf(pxxx, "%02x ", (unsigned char)fk[ixxx__]);\
			if( (ixxx__+1)%16 == 0 ) \
				pxxx += sprintf(pxxx, "\n");\
			else if( (ixxx__+1)%8 == 0 ) \
				pxxx += sprintf(pxxx, "  ");\
		}\
	}while(0)


#define HEX2ASCII_SPACE_CGI(src,len,tgt) \
	do {\
	char *pxxx=tgt;\
	if ( (len) >= 1024) break;\
	const char*fk = (const char*)(src); \
	for(int ixxx__=0;ixxx__< (len);ixxx__++) {\
		pxxx += sprintf(pxxx, "%02x ", (unsigned char)fk[ixxx__]);\
		if( (ixxx__+1)%16 == 0 ) \
			pxxx += sprintf(pxxx, HTML_NEWLINE );\
		else if( (ixxx__+1)%8 == 0 ) \
			pxxx += sprintf(pxxx, "  ");\
	}\
	}while(0)

#endif


#endif



#define debug_log(fmt, a...) \
	do {\
		debugv(fmt, ##a);\
		logv(fmt, ##a);\
	}while(0)


#define debugx_logx(fmt, a...) \
	do {\
		debugf(fmt "\n", ##a);\
		logx(fmt, ##a);\
	}while(0)

#define debug_logx(fmt, a...) \
	do {\
		debugv(fmt, ##a);\
		logx(fmt, ##a);\
	}while(0)



#define printf_log(fmt, a...) \
	do {\
		int xerrno = errno;\
		printf(fmt"\n", ##a);\
		errno = xerrno;\
		logv(fmt, ##a);\
	}while(0)



#define PRINT_BLOB(tgt, src, n) \
	do {\
		char *pp = tgt;\
		for(int i=0; i<n; i++)\
			pp += sprintf(pp, "%02x", (unsigned char)src[i]);\
	}while(0)

#define LogByTimeTheme(t,theme, fmt, a...) \
	do {\
		char szTimeBuf[MAX_FORMAT_LEN];\
		struct tm tm;\
		localtime_r(&t, &tm);\
		char szLogfile[MAX_PATH_LEN];\
		strftime(szTimeBuf, MAX_FORMAT_LEN, "%H:%M:%S", &tm);\
		GetXXXLogFileName(szLogfile, theme);\
		FILE* fp = fopen64(szLogfile, "a");\
		if (fp == NULL) \
			break;\
		fprintf(fp, "%s\t" fmt, szTimeBuf,##a);\
		fclose(fp);\
	} while(0)
	
#define LogByTheme(theme, fmt, a...) \
	do {\
		time_t tNow = time(NULL);\
		LogByTimeTheme(tNow, theme, fmt, a);\
	}while(0)

	
#define LogScanResult(t, fmt, a...) \
	do {\
		char szLogfile[MAX_PATH_LEN];\
		GetScanResultFileName(szLogfile, t);\
		FILE* fp = fopen(szLogfile, "a");\
		if (fp == NULL) \
			break;\
		fprintf(fp, fmt, ##a);\
		fclose(fp);\
	} while(0)


#define GET_MESSAGE(sem, lock, list, var) \
	(\
		Sem_wait(&sem),\
		EnterCriticalSection(&lock),\
		var = list.front(),\
		list.pop_front(),\
		LeaveCriticalSection(&lock),\
		1\
	)


#define POST_MESSAGE(lock, list, var, sem) \
	do {\
		EnterCriticalSection(&lock);\
		list.push_back( var );\
		LeaveCriticalSection(&lock);\
		Sem_post(&sem);\
	} while(0)



#define DEF_ForgePktAndPost(class, what, offset, padding, leastPktLen)\
	const char* ForgePktAndPost_##what(const char* pFrom, DWORD &nLen, SOCKET nSock)\
	{\
		WORD wDataLen;\
		while(1) {\
			if(nLen < leastPktLen )\
				return pFrom;\
			\
			wDataLen = *(WORD*)(pFrom+offset) + padding;\
			\
			if( nLen < wDataLen )\
				return pFrom;\
			\
			if(wDataLen<leastPktLen) {\
				debug_log("wrong format packet!(%u,%u)", wDataLen, leastPktLen);\
				shutdown(nSock, SHUT_RDWR);\
				pFrom += nLen;\
				nLen  =  0;\
				return pFrom;\
			}\
			else \
				class::Dispatch##what(nSock, pFrom, wDataLen);\
			\
			pFrom	+= wDataLen;\
			nLen	-= wDataLen;\
		}\
	}


#define SOCK_PAIR_CMD_READ 0
#define SOCK_PAIR_CMD_WRITE 1

#define READ 0
#define WRITE 1

//#define PATH_LENGTH (256ul)
//#define MAX_PATH_LEN (512ul)
//#define MAX_FORMAT_LEN (32ul)


#define EXIT_INIT 99

#define EXIT_GRACEFULLY 44
#define EXIT_ONESHOT 100

#define EXIT_WRONG_PARA 20


#define FEW_EPOLL_NEVENTS 1000

//#define MAX_LINE_LEN 1024


#define CORE_SIZE			(1024 * 1024 * 1024)
#define MSGQUEE_SIZE		( 64 * 1024 * 5000)
#define OPEN_FILES_NUM		( 4096 )
#define BUILD_TIMESTAMP	(__DATE__ " " __TIME__)


#define SUCCESS		"\E[60G[\E[0;32m  OK  \E[0;00m]\n"
#define FAIL		"\E[60G[\E[0;31mFAILED\E[0;00m]\n"


#ifdef _DEBUG

	#define debugf(a...) printf(a)

	//#define debugf(fmt,...) printf(fmt, __VA_ARGS__)

	#define PRINT_ARRAY(from,len) \
		do {\
			debugf("%s====%d====%s================\n",__FILE__,__LINE__,__FUNCTION__);\
			int xyz___len = len;\
			const char*fk = (const char*)(from); \
			for(int i=0;i<xyz___len;i++) {\
				debugf("%02x ", (unsigned char)fk[i]);\
				if( (i+1)%16 == 0 ) \
					debugf("\n");\
				else if( (i+1)%8 == 0 ) \
					debugf("  ");\
			}\
			debugf("\n");\
		}while(0)

	#define PRINT_ARRAYY(from,len) \
		do {\
			const char*fk = (const char*)(from); \
			for(int i=0;i<len;i++) {\
				debugf("%02x ", (unsigned char)fk[i]);\
			}\
			debugf("\n");\
		}while(0)


	#define PRINT_FILE(from,len,filename) \
		do {\
			FILE *fp = fopen(filename,"w");\
			fwrite(from, 1, len, fp);\
			fclose(fp);\
		}while(0)
		

#else

	#define debugf( ARGS... ) ((void)0)
	#define PRINT_ARRAY( ARGS... ) ((void)0)
	#define PRINT_FILE( ARGS... ) ((void)0)
	
#endif


#define debugv(fmt, a...) debugf("{%d, %s}: " fmt"\n", __LINE__, __FUNCTION__, ##a)
#define printfV(fmt, a...) printf("{%d, %s}: " fmt"\n", __LINE__, __FUNCTION__, ##a)



#ifdef USE_LOG_MACRO

#define logv(fmt, a...) \
	XLOG( fprintf(fp, "[%s]: {%s, %s, %d}: " fmt "\n", szHeadFormat, __FILE__, __FUNCTION__, __LINE__, ##a) )
	
#define logx(fmt, a...) \
	XLOG( fprintf(fp, "[%s]: " fmt "\n", szHeadFormat, ##a) )


	
#define XLOG(FPRINTF) \
	do {\
		int xerrno = errno;\
		char szLogfile[MAX_PATH_LEN];\
		char szHeadFormat[MAX_PATH_LEN];\
		GetLogFileName(szLogfile, szHeadFormat);\
		FILE* fp = fopen64(szLogfile, "ab");\
		if (fp == NULL) \
			break;\
		errno = xerrno;\
		FPRINTF;\
		fclose(fp);\
	} while(0)

#else

#define logv(fmt, a...) \
	X_Log( "{%s, %s, %d}: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##a)
	
#define logx(fmt, a...) \
	X_Log( fmt "\n", ##a)


	

#endif


#define SEND_CMD_TO_MAINLOOP(command)\
		( {\
			BYTE cmd = command;\
			sizeof(BYTE) == write(m_SocketPair[SOCK_PAIR_CMD_WRITE], &cmd, sizeof(BYTE));\
		})



#define InitializeCriticalSection(x) pthread_mutex_init( x, NULL )
//#define InitializeCriticalSection(x) x=PTHREAD_MUTEX_INITIALIZER

#define DeleteCriticalSection(x) pthread_mutex_destroy( x )

#define EnterCriticalSection(x) pthread_mutex_lock( x )
#define LeaveCriticalSection(x) pthread_mutex_unlock( x )

#define _close close
#define _tmain main


#define INI_SECTION_LINES_KEY "#"

#define STREQL(a,b)  ( strcmp( (const char*)a, (const char*)b ) == 0  )
#define STREQL_IGNORE_CASE(s1,s2) ( strcasecmp((const char *)s1, (const char *)s2) == 0)
#define STREQL_IGNORE_CASE_N(s1,s2,n) ( strncasecmp((const char *)s1, (const char *)s2, (size_t) n) == 0)
#define STREQL_N(s1,s2,n) ( strncmp((const char *)s1, (const char *)s2, (size_t) n) == 0)

#define ESTIMATE_COMPRESS_BOUND(sourceLen) (sourceLen + (sourceLen >> 12) + (sourceLen >> 14) + 11)

#define STRTOUL(s) strtoul(s, (char **) NULL, 10)

#define SSTRTOUL(s) STRTOUL( (s).c_str() )

#define suffix_log(suffix,fmt, a...) \
	SUFFIXLOG( suffix, fprintf(fp, "[%s]: " fmt "\n", szHeadFormat, ##a) )
			
#define SUFFIXLOG(suffix, FPRINTF) \
	do {\
		char szLogfile[MAX_PATH_LEN]={};\
		char szHeadFormat[MAX_PATH_LEN];\
		GetLogFileName(szLogfile, szHeadFormat);\
		strcat(szLogfile, suffix);\
		FILE* fp = fopen(szLogfile, "a");\
		if (fp == NULL) \
			break;\
		FPRINTF;\
		fclose(fp);\
	} while(0)



#define HEX2ASCII(src,len,tgt) \
	do {\
		char *pxxx=tgt;\
		if ( len >= 1024) break;\
		const char*fk = (const char*)(src); \
		for(int ixxx__=0;ixxx__< len;ixxx__++) {\
			pxxx += sprintf(pxxx, "%02x", (unsigned char)fk[ixxx__]);\
		}\
		pxxx += sprintf(pxxx, "\n");\
	}while(0)

#define HTML_NEWLINE	"<br/>"



#define STC_CMD_GS_HEAD_LEN 5 // sizeof STC_CMD_GS == 5

#define EMPTY_GS_PACKET_LEN 6 // (len,cmd)
#define EMPTY_LOG_PACKET_LEN 8

#define HEART_BEAT_INTERVAL_RDBGS2LOG 600
#define E_CENTRAL_STOP_THREAD			 	0x9999



#define SETNONBLOCK(fd)	(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0)| O_NONBLOCK))


#define EPOLL_ADD(epfd, epoll_flag, sock, sock_map, sockflag) \
	do {\
		if(sock<0) break;\
		struct epoll_event ev;\
		SETNONBLOCK(sock);\
		ev.data.fd					= sock;\
		ev.events					= epoll_flag;\
		sock_map[sock].sock_flag	= sockflag;\
		if ( -1 == epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) ) ;\
	}while(0)
	
#define EPOLL_MOD(epfd, epoll_flag, sock) \
	do {\
		if(sock<0) break;\
		struct epoll_event ev;\
		ev.data.fd					= sock;\
		ev.events					= epoll_flag;\
		epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &ev);\
	}while(0)
	

#define GET_SOCK_FLAG(u64) (u64>>32)

#define FREE_PKT(pkt) do { free(pkt); } while(0)
#define Uncompress(a,b,c,d) uncompress((Bytef*)(a), (uLongf*)(b), (const Bytef*)(c), (uLong)d)

#define GetIniStr(szSection, szKey, szDefault, strVal) GetPrivateProfileString_(szSection, szKey, szDefault, strVal, szFileName)

#define GetIniStr__(szSection, szKey, szDefault, func) \
(\
{\
	string strVal;\
	func( GetPrivateProfileString_(szSection, szKey, szDefault, strVal, szFileName) == 0 ? \
			szDefault : strVal.c_str() );\
}\
)

#define Str2BigendianIP(ip) CStr2BigendianIP( (ip).c_str() )


string& StringTrim(string& str, const char* trim_char);
string& StringReplaceAll(string & str, const string & strsrc, const string &strdst);
string& StringTrimAll(string& str, const char* trim_char);
int Strtoken2Vec( const char* str, const char* split, vector<string> &vec, TRIM_FUNC trim_func, const char* trim_char);
int EachLineToBigIP_Star_func(const char*from, const char*to, void* para);
DWORD CStr2BigendianIP(const char* ip);

int EachLineToBigIP_func(const char*from, const char*to, void* para);
int GetPrivateProfileInt(const char* section, const char* key, int nDefault, const char* file);
int ReadSectionLines(const string& strFileName, const string& strSection, EACH_LINE_FUNC each_line_func, void* para);


void ReadIniFile(void *pv);
//parse error : -1; no such section/key : 0;  else 1

int GetPrivateProfileString_(const char* section, const char* key, const char* strDefault, string &strVal, const char* file);
const char* GetConfigFile(void);

int Server_Main(int argc, char** argv, const char* prog, const char* VERSION, 
	SERVER_INIT_UINIT init_fun, SERVER_INIT_UINIT uninit_fun, SERVER_INIT_UINIT oneshot_fun, SERVER_INIT_UINIT main_loop,
	sighandler_t term_handler, sighandler_t hup_handler, sighandler_t quit_handler);


string GetLogTime(time_t nTime);

//inline  , => \,       \ => \\     \n => \E   
char* EscapeLogStr(char *szTgt, const char* szSrc);

void EXIT(int exit_code);
void *Malloc(size_t size);

void Allocate_Sock_Pro(MAP_SOCK_PRO &m, SOCKET sock);
int Recv_Until_EAGAIN(SOCKET nSock, char *szBuf, char * &szCur, ForgePktAndPost_t ForgePktAndPost);

int Epoll_create(int size);

int Sem_wait(sem_t *sem);
void Sem_post(sem_t *sem);

void Sem_init(sem_t *sem, int pshared, unsigned int value);


size_t GetTime(char *szTimeBuf, const char* szFormat, int nMinInterval = 0);


int CreateDir(char* szDirName);


//2009-05-12-13:59:30"
//2009-05-12-13.txt"
#define Y_OFFSET (szXXXX+0)
#define M_OFFSET (szXXXX+5)
#define D_OFFSET (szXXXX+8)
#define HH_OFFSET (szXXXX+11)
#define MM_OFFSET (szXXXX+14)
#define SS_OFFSET (szXXXX+17)

void GetLogFileName(char* szLogfile, char* szHeadFromat);

int WriteNBytes(int fd, char *buf, unsigned int nbyte);

void X_Log(const char* fmt, ... );




PSTC_Socket_And_CMD_GS MallocGSPacket(SOCKET nSock, WORD wCmdType, const char* data, int nDataLen);

void OnClose_(SOCKET fd, MAP_SOCK_PRO &m);
void OnClose_(SOCKET fd, int epfd, STC_SOCK_PRO &sp);


bool IPInList(DWORD dwBigEndIP, const list<STC_IP_CHECK>& lstStcIPCheck);


int Accept(SOCKET nSock, const list<STC_IP_CHECK>& lstStcIPCheck);

int pthread_create_detached(pthread_t * thread, void *(*start_routine)(void*), void * arg);

void Pthread_create_detached( void *(*start_routine)(void*), void * arg );

string BigendianIP2Str(DWORD bigend_ip);

SOCKET TCPServerListen(short start_port, int bind_try_times, unsigned int BigEndianIP);

void GetEventLogFileName(char* szLogfile, int nChannel, int nMinInterval=0);
int Setrlimit(int resource, int SIZE);

#endif


#endif