#define _CRT_RAND_S
#include <stdlib.h>

extern "C"
{
//#include "md5_private.h"
}


#ifndef _WIN32
//#include <openssl/md5.h>
#include <dlfcn.h>
#endif

#if defined(__linux__)
#include <uuid/uuid.h>
#endif

#include "base.h"
#include "log.h"
#include <algorithm>
#include "zlibapi.h"

unsigned long BASE::dataCompress(const char* src, unsigned long srcSize, char* dst )
{
	unsigned long dstSize = ESTIMATE_COMPRESS_BOUND(srcSize);
	Compress((BYTE*)dst, &dstSize, (BYTE*)src, srcSize);
	return dstSize;	
}

#define DEFINE_PARSE_TYPE(type)	\
	bool BASE::parse##type( unsigned char*& p, size_t &leftLen, type& out )\
	{\
		if ( leftLen< sizeof( type ))\
			return false;\
		\
		leftLen -= sizeof( type );\
		out = *(type*)p;\
		p += sizeof( type );\
		return true;\
	}


DEFINE_PARSE_TYPE(BYTE);
DEFINE_PARSE_TYPE(WORD);
DEFINE_PARSE_TYPE(DWORD);


bool BASE::parseNBYTE( unsigned char*& p, size_t &leftLen, string& out, size_t nBytes )
{

	if ( leftLen< nBytes )
		return false;

	leftLen -= nBytes;
	out = string( (char*)p, nBytes);
	
	p += nBytes;
	return true;

}


bool BASE::parseBSTR( unsigned char*& p, size_t &leftLen, string& out )
{

	if ( leftLen< sizeof(WORD) )
		return false;

	WORD len = *(WORD*)p;

	if ( leftLen< sizeof(WORD) + len)
		return false;
	
	leftLen -= sizeof(WORD)+len;

	p += sizeof(WORD);

	out = string( (char*)p, len );
	
	p += len;
	return true;
}

//bool parseByte( unsigned char*& p, size_t &leftLen, BYTE& out )
//{
//	if ( leftLen< sizeof( BYTE ))
//		return false;

//	leftLen -= sizeof( BYTE );
//	out = *(BYTE*)p;
//	p += sizeof( BYTE );
//	return true;
//}




#ifndef WIN32

void BASE::sleepUninteruptable( time_t sec, long milli_sec)
{
	struct timespec req ;
	struct timespec rem ;

	req.tv_sec  = sec;
	req.tv_nsec = milli_sec * 1000000;

	//assert(req.tv_nsec>0 && req.tv_nsec<999999999);
	while( -1 == nanosleep(&req, &rem) )
	{
		if(errno != EINTR )
			break;
		//debugf("%u--%u\n", rem.tv_sec, rem.tv_nsec);
		req = rem;
	}
}
#endif



int BASE::setrlimit(int resource, int SIZE)
{
#ifndef WIN32
	struct rlimit x = { (rlim_t)SIZE, (rlim_t)SIZE };
	return setrlimit(resource, &x);
#else
	return 0;
#endif
	
}

DWORD BASE::getTickCount()
{
#if defined(WIN32)
	return ::GetTickCount();
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
}


DWORD BASE::randTo(DWORD dwMax)
{
	DWORD dwMin = 0;
	if (dwMin == dwMax)
		return dwMin;
	unsigned int dwRand;
#if defined(WIN32)
	if (rand_s(&dwRand))
		return dwMin;
#else
	dwRand = rand();
	if( dwMax-dwMin > INT_MAX )
	{
		dwRand += rand();
	}
#endif
	dwRand = dwRand % (dwMax-dwMin) + dwMin;
	return dwRand;
}

int BASE::randBetween(int nMin, int nMax)
{
	if (nMin >= nMax)
		return nMax;
	unsigned int dwRand;
#if defined(WIN32)
	if (rand_s(&dwRand))
		return nMin;
#else
	dwRand = rand();
#endif
	return dwRand % (nMax-nMin) + nMin;
}

int BASE::randBetween(unsigned nMin, unsigned nMax)
{
	if (nMin >= nMax)
		return nMax;
	unsigned int dwRand;
#if defined(WIN32)
	if (rand_s(&dwRand))
		return nMin;
#else
	dwRand = rand();
#endif
	return dwRand % (nMax-nMin) + nMin;
}

int BASE::randBetween(unsigned long nMin, unsigned long nMax)
{
	if (nMin >= nMax)
		return nMax;
	unsigned int dwRand;
#if defined(WIN32)
	if (rand_s(&dwRand))
		return nMin;
#else
	dwRand = rand();
#endif
	return dwRand % (nMax-nMin) + nMin;
}

float BASE::randBetween(float nMin, float nMax)
{
	if (nMin >= nMax)
		return nMax;
	unsigned int dwRand;
#if defined(WIN32)
	if (rand_s(&dwRand))
		return nMin;
#else
	dwRand = rand();
#endif
	return (float)((double)dwRand / MAXUINT) * (nMax-nMin) + nMin;
}

int BASE::randInter(int min, int max)
{
	return randBetween(min,max+1);
}

bool BASE::randHit(int num, int total)
{
	int nRand = randBetween(0,total);
	if (nRand < num)
	{
		return true;
	}

	return false;
}

int BASE::getIndexOfVectorSum(const vector<DWORD> &vPartialSum)
{
	//assert vPartialSum.size()

	size_t nSize = vPartialSum.size();
	DWORD dwRand = BASE::randTo( vPartialSum[ nSize-1 ] +1);

	auto vit = find_if(vPartialSum.begin(), vPartialSum.end(), [dwRand](DWORD x){  return dwRand <= x; } ) ;

	//
	if ( vit == vPartialSum.end() )
		return 0;

	int i = vit - vPartialSum.begin();
	return i;//vit - vPartialSum.begin();
}


#if defined(__linux__)
#include <dlfcn.h>

void* BASE::getDllFunc( void* handle, const char* funcName )
{
	if ( handle == NULL )
		return NULL;
	
	void *func=dlsym(handle, funcName);
	const char* hError=dlerror();
	if (hError)
	{
		logFATAL("dlsym %s Error!%s", func, hError);
		return NULL;
	}

	return func;
}


void* BASE::loadLibFile(const char* soname, void* handle)
{

	if ( handle )
	{
		dlclose( handle );
		//free here
		logFATAL("unload %s!", soname);
	}

	string strPetSo = GET_SO_FULL_NAME(soname);

	//handlePet =dlopen(PET_SO_NAME, RTLD_LAZY);
	handle =dlopen(strPetSo.c_str(), RTLD_NOW);
	
	const char* hError=dlerror();
	if (hError)
	{
		//PRINTLN( S_OUT, "dlopen %s Error!%s", strPetSo.c_str(), hError );
		PRINT_LOG("dlopen %s Error!%s", strPetSo.c_str(), hError );
		exit(9);
		return NULL;
	}

	logFATAL( "load %s ok!", strPetSo.c_str() );
	return handle;
	
}
#else

void* BASE::getDllFunc( void* handle, const char* funcName ){return NULL;}
void* BASE::loadLibFile(const char* soname, void* handle){return NULL;}

#endif


#if defined(__linux__)

//#include <sys/types.h>
//#include <unistd.h>

# include <sys/socket.h>
# include <sys/ioctl.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <sys/un.h>
# include <arpa/inet.h>
# include <netdb.h>

static mode_t read_umask(void) {
	mode_t mask = umask(0);
	umask(mask);
	return mask;
}
//int bind_socket(const char *addr, unsigned short port, const char *unixsocket, uid_t uid, gid_t gid, mode_t mode, int backlog)
int bind_socket(const char *addr, unsigned short port, int backlog)
{

	int fcgi_fd, socket_type, val;

const char *unixsocket = NULL;
uid_t uid=1;
gid_t gid = 1;

mode_t mode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) & ~read_umask();


//printf("fuckkkk\n");

	struct sockaddr_un fcgi_addr_un;
	struct sockaddr_in fcgi_addr_in;
#ifdef USE_IPV6
	struct sockaddr_in6 fcgi_addr_in6;
#endif
	struct sockaddr *fcgi_addr;

	socklen_t servlen;

	if (unixsocket) {
		memset(&fcgi_addr_un, 0, sizeof(fcgi_addr_un));

		fcgi_addr_un.sun_family = AF_UNIX;
		/* already checked in main() */
		if (strlen(unixsocket) > sizeof(fcgi_addr_un.sun_path) - 1) return -1;
		strcpy(fcgi_addr_un.sun_path, unixsocket);

#ifdef SUN_LEN
		servlen = SUN_LEN(&fcgi_addr_un);
#else
		/* stevens says: */
		servlen = strlen(fcgi_addr_un.sun_path) + sizeof(fcgi_addr_un.sun_family);
#endif
		socket_type = AF_UNIX;
		fcgi_addr = (struct sockaddr *) &fcgi_addr_un;

		/* check if some backend is listening on the socket
		 * as if we delete the socket-file and rebind there will be no "socket already in use" error
		 */
		if (-1 == (fcgi_fd = socket(socket_type, SOCK_STREAM, 0))) {
			fprintf(stderr, "spawn-fcgi: couldn't create socket: %s\n", strerror(errno));
			return -1;
		}

		if (0 == connect(fcgi_fd, fcgi_addr, servlen)) {
			fprintf(stderr, "spawn-fcgi: socket is already in use, can't spawn\n");
			close(fcgi_fd);
			return -1;
		}

		/* cleanup previous socket if it exists */
		if (-1 == unlink(unixsocket)) {
			switch (errno) {
			case ENOENT:
				break;
			default:
				fprintf(stderr, "spawn-fcgi: removing old socket failed: %s\n", strerror(errno));
				close(fcgi_fd);
				return -1;
			}
		}

		close(fcgi_fd);
	} else {
		memset(&fcgi_addr_in, 0, sizeof(fcgi_addr_in));
		fcgi_addr_in.sin_family = AF_INET;
		fcgi_addr_in.sin_port = htons(port);

		servlen = sizeof(fcgi_addr_in);
		socket_type = AF_INET;
		fcgi_addr = (struct sockaddr *) &fcgi_addr_in;

#ifdef USE_IPV6
		memset(&fcgi_addr_in6, 0, sizeof(fcgi_addr_in6));
		fcgi_addr_in6.sin6_family = AF_INET6;
		fcgi_addr_in6.sin6_port = fcgi_addr_in.sin_port;
#endif

		if (addr == NULL) {
			fcgi_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
#ifdef HAVE_INET_PTON
		} else if (1 == inet_pton(AF_INET, addr, &fcgi_addr_in.sin_addr)) {
			/* nothing to do */
#ifdef HAVE_IPV6
		} else if (1 == inet_pton(AF_INET6, addr, &fcgi_addr_in6.sin6_addr)) {
			servlen = sizeof(fcgi_addr_in6);
			socket_type = AF_INET6;
			fcgi_addr = (struct sockaddr *) &fcgi_addr_in6;
#endif
		} else {
			fprintf(stderr, "spawn-fcgi: '%s' is not a valid IP address\n", addr);
			return -1;
#else
		} else {
			if ((in_addr_t)(-1) == (fcgi_addr_in.sin_addr.s_addr = inet_addr(addr))) {
				fprintf(stderr, "spawn-fcgi: '%s' is not a valid IPv4 address\n", addr);
				return -1;
			}
#endif
		}
	}


	if (-1 == (fcgi_fd = socket(socket_type, SOCK_STREAM, 0))) {
		fprintf(stderr, "spawn-fcgi: couldn't create socket: %s\n", strerror(errno));
		return -1;
	}

	val = 1;
	if (setsockopt(fcgi_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
		fprintf(stderr, "spawn-fcgi: couldn't set SO_REUSEADDR: %s\n", strerror(errno));
		close(fcgi_fd);
		return -1;
	}

	if (-1 == bind(fcgi_fd, fcgi_addr, servlen)) {
		fprintf(stderr, "spawn-fcgi: bind failed: %s\n", strerror(errno));
		close(fcgi_fd);
		return -1;
	}

	if (unixsocket) {
		if (-1 == chmod(unixsocket, mode)) {
			fprintf(stderr, "spawn-fcgi: couldn't chmod socket: %s\n", strerror(errno));
			close(fcgi_fd);
			unlink(unixsocket);
			return -1;
		}

		if (0 != uid || 0 != gid) {
			if (0 == uid) uid = -1;
			if (0 == gid) gid = -1;
			if (-1 == chown(unixsocket, uid, gid)) {
				fprintf(stderr, "spawn-fcgi: couldn't chown socket: %s\n", strerror(errno));
				close(fcgi_fd);
				unlink(unixsocket);
				return -1;
			}
		}
	}

	if (-1 == listen(fcgi_fd, backlog)) {
		fprintf(stderr, "spawn-fcgi: listen failed: %s\n", strerror(errno));
		close(fcgi_fd);
		if (unixsocket) unlink(unixsocket);
		return -1;
	}

	return fcgi_fd;
}


#endif


int BASE::inet_aton( const char *cp, struct in_addr *pin )
{
#if defined(WIN32)
	unsigned long rc;

	rc = inet_addr( cp );
	if ( rc == INADDR_NONE && strcmp( cp, "255.255.255.255" ) )
		return 0;
	pin->s_addr = rc;
	return 1;
#else
	return ::inet_aton(cp, pin);
#endif
}

// 加载socket动态链接库(dll)
int BASE::initWsadata()
{

#ifdef WIN32  
	WORD wVersionRequested;  
	WSADATA wsaData;    // 这结构是用于接收Wjndows Socket的结构信息的  
	int err;  

	wVersionRequested = MAKEWORD( 2, 2 );   // 请求2.2版本的WinSock库  

	err = WSAStartup( wVersionRequested, &wsaData );  
	if ( err != 0 ) 
	{  
		exit(9);          // 返回值为零的时候是表示成功申请WSAStartup  
	}  

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) {  
		// 检查这个低字节是不是2，高字节是不是2以确定是否我们所请求的2.2版本  
		// 否则的话，调用WSACleanup()清除信息，结束函数  
		WSACleanup( );  
		exit(9);   
	}
#endif
	return 0;
}

SOCKET BASE::ConnectServer(const char*ip, short port, int timeout_secs)
{
	struct sockaddr_in addr;
	SOCKET sock;
	if( (sock = socket(AF_INET, SOCK_STREAM, 0))== INVALID_SOCKET ) 
	{
		logii("socket error! %d.", socket_errno);
		return INVALID_SOCKET;
	}

	memset(&addr, 0x00, sizeof(addr));
	BASE::inet_aton(ip, &addr.sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	int rc = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	if( rc == SOCKET_ERROR )
	{
		//PRINTF(S_ERR, "ConnectServer: connect(%s:%d) failed(%d).\n", ip, port, socket_errno);
		logii( "ConnectServer: connect(%s:%d) failed(%d).", ip, port, socket_errno);
		closesocket(sock);
		return INVALID_SOCKET;
	}
	else
	{
		logff("connect %s:%u ok.", ip, port);
	}
	return sock;
}

bool BASE::SocketOk(SOCKET sock)
{
	int err;
	socklen_t len = sizeof( err );
	int ret = getsockopt( sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len);
	return ( errno = err ) == 0 && ret==0;
}


// block write (nLen) bytes
int BASE::Write(SOCKET hSocket, const void* szBuf, int nLen)
{

	if ((nLen < 0) || (szBuf ==NULL))
		return 0;

	int nLeft, nWrite, nWriteTimes = 0;

	const BYTE* pBuf = (const BYTE*)szBuf;
	nLeft = nLen;

	while ( nLeft > 0 )
	{
		nWriteTimes += 1;
#if defined(__linux__)
		if ( ( nWrite = send(hSocket, pBuf, nLeft, MSG_NOSIGNAL)) < 0) 
#else
		if ( ( nWrite = send(hSocket, (const char*)pBuf ,nLeft,0)) <= 0)
#endif
		{
#if defined(__linux__)
			if( errno == EINTR || IS_E_AGAIN )
			{
				nWrite = 0; // call write again
			}else
#else
			if( IS_E_AGAIN )
			{
				nWrite = 0; // call write again
			}else
#endif
				return (-1); // error
		}

		//if ( nWriteTimes > 200 ) {
		if ( nWriteTimes > 10000 ) {

			logff("hSocket %d write too more.", hSocket );
			return nLen - nLeft;
		}

		nLeft -= nWrite;
		pBuf   += nWrite;
	}
	
	if (nWriteTimes > 2)
	{
		logff("succ after WriteTimes=%d", nWriteTimes);
	}

	return nLen;		
}

void BASE::writeLastPktFile(DWORD dwRoleID, const char* szBuf, int nLen)//, int nOper )
{
#ifndef WIN32

	static int g_nLastPktFile = -1;

//	if ( nOper == E_LAST_PKT_FILE_NONE )
//		return;
	if ( g_nLastPktFile == -1 )
	{
		char szFilename[32]={0};		
		sprintf(szFilename, "/tmp/tmpfs/%d.txt", (int)time(NULL) );
		
		g_nLastPktFile = creat(szFilename, 0);

		//return;
	}

//	if ( nOper == E_LAST_PKT_FILE_REWIND )
//	{
//		lseek(g_nLastPktFile, 0, SEEK_SET);
//	}
//	else if ( nOper == E_LAST_PKT_FILE_TRUNCATE )
//	{
//		ftruncate(g_nLastPktFile, 0);
//	}

	//E_LAST_PKT_FILE_RECORD_ALL

	ftruncate(g_nLastPktFile, 0);
	lseek(g_nLastPktFile, 0, SEEK_SET);

	struct iovec iov[2] = { {&dwRoleID, 4}, { (char*)szBuf, (size_t)nLen} };

	writev( g_nLastPktFile, iov, 2 );

#endif
}

string& BASE::getUUID(string& uuid)
{
		
	char TMP_UUID[64]={0};
		
#if defined(__linux__)
	uuid_t out;
	uuid_generate(out);
	HEX2ASCII_SPACE2( out, 16, TMP_UUID );	
#else

	UUID win_uuid;
	UuidCreate( &win_uuid);
	//uuid.assign( (char*)&Uuid, 16);
	HEX2ASCII_SPACE2( &win_uuid, 16, TMP_UUID );
	
#endif


	uuid.assign( TMP_UUID, 32 );

//	LOG_GETUUID *pLog = (LOG_GETUUID *)malloc(sizeof(LOG_GETUUID));
//	memset(pLog, 0 ,sizeof(LOG_GETUUID));
//	pLog->wLen = sizeof(LOG_GETUUID);
//	pLog->wCmdType = E_LOG_GETUUID;
//	pLog->eventID = GET_EVENT_ID();
//	strcpy(pLog->cmdName, "getuuid");
//	pLog->dwTime = time(NULL);
//	strcpy(pLog->uuid, uuid.c_str());

//	LogIntf::LOG_MESSAGE message(pLog);
//	LOGINTF->send(message);
//	//GETUUID loge("getuuid", time(NULL), uuid.c_str());
//	//logee("%s,%u,%s",loge.cmdName, loge.dwTime,loge.uuid);
	return uuid;

}


string BASE::getUUID2()
{

	char TMP_UUID[64]={0};
		
#if defined(__linux__)
	uuid_t out;
	uuid_generate(out);
	HEX2ASCII_SPACE2( out, 16, TMP_UUID );	
#else

	UUID win_uuid;
	UuidCreate( &win_uuid);
	//uuid.assign( (char*)&Uuid, 16);
	HEX2ASCII_SPACE2( &win_uuid, 16, TMP_UUID );
	
#endif

	return string(TMP_UUID, 32 );

}
//========================================================================

bool BASE::inTheSameDay(time_t a, time_t b)
{
	struct tm tm1, tm2;
	LOCAL_TIME(a, tm1);

	LOCAL_TIME(b, tm2);
	return tm1.tm_mday == tm2.tm_mday && tm1.tm_year == tm2.tm_year;
}

bool BASE::inTheSameMonth(time_t a, time_t b)
{
	struct tm tm1, tm2;
	LOCAL_TIME(a, tm1);

	LOCAL_TIME(b, tm2);
	return tm1.tm_mon == tm2.tm_mon && tm1.tm_year == tm2.tm_year;
}

bool BASE::eachLine(const string& strFile, std::function<void ( const char* )> func )
{
	FILE *fp =  fopen(strFile.c_str(), "rb");
	if( fp == NULL )
	{
		logFATAL( "file %s doesn't exist!", strFile.c_str() );
		return false;
	}
	
	char szBuf[4096];
	while( fgets(szBuf, sizeof(szBuf), fp) )
	{
		szBuf[ strlen(szBuf) -1 ] = 0;
		func( szBuf );
	}
	fclose(fp);
	return true;
}

bool BASE::isComment( const string& strLine)
{
	const char* p = strLine.c_str();
	while( *p )
	{
		if ( *p == ' ' || *p == '\t' )
		{
			p++;
			continue;
		}
		if ( *p++ == '#' )
			return true;
		return false;
	}
	return false;
}

string getIniSec(const string& strLineInput)
{
	string::size_type pos1, pos2,pos;

	string strLine = strLineInput;
	BASE::stringTrim(strLine,"\t\r");
	BASE::stringReplaceAll(strLine, "\xef\xbb\xbf", "");

//password=abcd#123

	if(  ( pos = strLine.find_first_of('#') ) != string::npos ) 
		return "";

	if  (( pos1 = strLine.find_first_of('[') ) == string::npos )
		return "";
		
	if ( ( pos2 = strLine.find_first_of(']') ) == string::npos )
		return "";
				
	if ( pos2-pos1 <1 )
		return "";

	for ( string::size_type i=0; i<pos1; i++) 
	{
		if ( strLine[i] != '\t' || strLine[i] != 0x20 )
			return "";
	}
	
	return strLine.substr(pos1+1,pos2-pos1-1);
}

static bool isIniSecName(const string& strLineInput, const string& strSecName )
{
	return getIniSec( strLineInput ) == strSecName;
}

bool BASE::eachIniSecLine(const string& strFile, const string& strSecName, std::function<void ( const char* )> func )
{
	FILE *fp =  fopen(strFile.c_str(), "rb");
	if( fp == NULL )
	{
		logFATAL( "file %s doesn't exist!", strFile.c_str() );
		return false;
	}
	
	bool bSecStart = false;
	
	char szBuf[4096];
	while( fgets(szBuf, sizeof(szBuf), fp) )
	{

		szBuf[ strlen(szBuf) -1 ] = 0;

		if ( bSecStart )
		{
			//another section start
			if ( getIniSec(szBuf) != "" )
			{
				fclose(fp);
				return true;
			}
			
			if ( isComment( szBuf ) )
				continue;

			func( szBuf );
		}
		else if ( isIniSecName( szBuf, strSecName ) )
		{
			bSecStart = true;
		}

	}
	fclose(fp);
	return true;
}



bool BASE::eachIniSecLine2(const string& strFile, const string& strSecName, std::function<int ( char* )> func )
{
	FILE *fp =  fopen(strFile.c_str(), "rb");
	if( fp == NULL )
	{
		logFATAL( "file %s doesn't exist!", strFile.c_str() );
		return false;
	}
	
	bool bSecStart = false;
	
	char szBuf[4096];
	while( fgets(szBuf, sizeof(szBuf), fp) )
	{

		szBuf[ strlen(szBuf) -1 ] = 0;

		if ( bSecStart )
		{
			//another section start
			if ( getIniSec(szBuf) != "" )
			{			
				goto READ_BREAK;
			}
			
			if ( isComment( szBuf ) )
				continue;

			if ( E_READ_BREAK == func( szBuf ) )
			{
				goto READ_BREAK;
			}
		}
		else if ( isIniSecName( szBuf, strSecName ) )
		{
			bSecStart = true;
		}

	}

READ_BREAK:
	fclose(fp);
	return true;
	
}




bool BASE::eachIniSecLine3(FILE *fp, const string& strSecName, std::function<int ( char* )> func )
{
	rewind(fp);
	
	bool bSecStart = false;
	
	char szBuf[4096];
	while( fgets(szBuf, sizeof(szBuf), fp) )
	{

		szBuf[ strlen(szBuf) -1 ] = 0;

		if ( bSecStart )
		{
			//another section start
			if ( getIniSec(szBuf) != "" )
			{			
				goto READ_BREAK;
			}
			
			if ( isComment( szBuf ) )
				continue;

			if ( E_READ_BREAK == func( szBuf ) )
			{
				goto READ_BREAK;
			}
		}
		else if ( isIniSecName( szBuf, strSecName ) )
		{
			bSecStart = true;
		}

	}

READ_BREAK:

	return true;
	
}

string& BASE::stringTrim(string& str, const char* trim_char)
{
	string::size_type pos = str.find_last_not_of(trim_char);

	if(pos != string::npos) 
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(trim_char);
		if(pos != string::npos)
			str.erase(0, pos);
	}
	else
		str.erase(str.begin(), str.end());
	
	return str;
}

string& BASE::stringReplaceAll(string & str, const string & strsrc, const string &strdst) 
{
	string::size_type pos=0;
	string::size_type srclen=strsrc.size();
	string::size_type dstlen=strdst.size();
	while( (pos=str.find(strsrc, pos)) != string::npos)
	{
		str.replace(pos, srclen, strdst);
		pos += dstlen;
	}
	
	return str;
}

string& BASE::stringTrimAll(string& str, const char* trim_char)
{
	string::size_type pos;
	while( (pos=str.find_first_of(trim_char)) != string::npos)
	{
		str.erase(pos,1);
	}
	return str;
}


#ifdef WIN32
static char *strtok_r(char *s, const char *delim, char **state)
{
	return strtok(s, delim);
}
#endif


int BASE::strtoken2Vec( const char* str, const char* split, vector<string> &vec, TRIM_FUNC trim_func, const char* trim_char)
{
	char *token, *saveptr;
	char * s = strdup(str);
	char * ss =s;

	int i=0;
	while( (token = strtok_r(s, split, &saveptr)) )
	{
		vec.push_back(token);

		if( trim_func && trim_char )
			trim_func( vec.back(), trim_char);
			
		s = NULL;
		i++;
	}

	free(ss);
	return i;
}


template<typename T>
T BASE::convertSTR( const char* str)
{
	return T(str);
}

template<>
int BASE::convertSTR( const char*  str)
{
	return ATOI(str);
}


template<>
float BASE::convertSTR( const char* str)
{
	return ATOF(str);
}

template<>
string BASE::convertSTR( const char* str)
{
	string s(str);
	return stringTrim( s, SPLIT_SPACE );

}


int inet_aton_( const char *cp, struct in_addr *pin )
{
#if defined(WIN32)
	unsigned long rc;

	rc = inet_addr( cp );
	if ( rc == INADDR_NONE && strcmp( cp, "255.255.255.255" ) )
		return 0;
	pin->s_addr = rc;
	return 1;
#else
	return inet_aton(cp, pin);
#endif
}



const char * inet_ntop_(int family, const void *addrptr, char *strptr)
{
#if defined(WIN32)
	const u_char *p = (const u_char *) addrptr;

	if (family == AF_INET) 
	{
		char temp[32] = {0};
		snprintf(temp, sizeof(temp), "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
		
		strcpy(strptr, temp);
		return (strptr);
	}
	return (NULL);
#else
	return inet_ntop(family, addrptr, strptr, IP4_ADDRESS_STRING_LENGTH);
#endif
}




DWORD CStr2BigendianIP(const char* ip)
{
	struct in_addr a ;
	inet_aton_(ip, &a);
	return a.s_addr;
}

string BigendianIP2Str(DWORD bigend_ip)
{
	struct in_addr a ;
	a.s_addr = bigend_ip;

	string str; 
	//char cstr[INET_ADDRSTRLEN];
	char cstr[IP4_ADDRESS_STRING_LENGTH];
	if ( NULL != inet_ntop_(AF_INET, &a, cstr))
	{
		str.assign(cstr);
	}
	return str;
}

char* BigendianIP2Str2(DWORD bigend_ip, char* ip)
{
	struct in_addr a ;
	a.s_addr = bigend_ip;

	if ( NULL == inet_ntop_(AF_INET, &a, ip))
	{	
		ip[0]=0;
	}
	
	return ip;
}



SNOW_FLAKE_UUID *SNOW_FLAKE_UUID::_snow_flake_uuid = nullptr;

U64 SNOW_FLAKE_UUID::getTime()
{
#if defined(__linux__)
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec / 1000 + ( (unsigned long long ) tv.tv_sec * 1000);
#else
	FILETIME filetime;
	U64 time = 0;
	GetSystemTimeAsFileTime(&filetime);

	time |= filetime.dwHighDateTime;
	time <<= 32;
	time |= filetime.dwLowDateTime;

	time /= 10;
	return time / 1000;
#endif
}

//每次启动
SNOW_FLAKE_UUID::SNOW_FLAKE_UUID( U64 machine, U64 sequence)
{
	_machine = machine;
	_sequence = sequence;
}

U64 SNOW_FLAKE_UUID::generate()
{
	U64 value = 0;

	// 保留后41位时间
	value = getTime() << 22;

	// 中间10位是机器ID
	value |= (_machine & 0x3FF) << 12;

	// 最后12位是sequenceID
	value |= _sequence++ & 0xFFF;

	return value;
}

SNOW_FLAKE_UUID* SNOW_FLAKE_UUID::getInstance()
{
	if (_snow_flake_uuid == nullptr) 
	{
		_snow_flake_uuid = new SNOW_FLAKE_UUID();
	}

	return _snow_flake_uuid;
}


