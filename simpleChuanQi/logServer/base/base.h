#ifndef MISC_H
#define MISC_H


#ifdef WIN32

#ifndef _USE_32BIT_TIME_T  
#define _USE_32BIT_TIME_T 
#endif

#endif


#include "head.h"


//template<typename _T>
//string StrItoa(_T i)
//{
//    ostringstream o;
//    o<<i;
//    return o.str();
//}


#if 0
template<typename T>
void randShuffle(T& v)
{
 	std::random_device rd;
	std::mt19937 g(rd());
 	std::shuffle(v.begin(), v.end(), g);
// 	std::shuffle(v.begin(), v.end(), less() );
//random_shuffle

}

#else

#define randShuffle(v) 	random_shuffle(v.begin(), v.end())

#endif


#define SEPERATE_CHAR		"\t"

#ifdef WIN32
#define LOCAL_TIME(time, time_member)	 _localtime32_s( &time_member, &time)
#else
#define LOCAL_TIME(time, time_member)	 localtime_r(&time, &time_member)
#endif


#if defined(__linux__)
#define GET_DLL_FUNC(func_type, handle, dll, funcname)		(func_type)BASE::getDllFunc(handle, #funcname)
#else
#define GET_DLL_FUNC(func_type, handle, dll, funcname)		dll::funcname
#endif


#ifndef IP4_ADDRESS_STRING_LENGTH
#define IP4_ADDRESS_STRING_LENGTH           (16)
#endif


#define COREDUMP(cond)	do{ if ((cond)) {	srand( *(int *)0); } }while(0)

#define STREQL(a,b)  ( strcmp( (const char*)a, (const char*)b ) == 0  )


const char* ProgramDir(void);

//#define GET_SO_FULL_NAME(soname)	({ string PET_SO_PATH;	PET_SO_PATH += ProgramDir();	PET_SO_PATH += "/lib/"; PET_SO_PATH += soname;PET_SO_PATH;})

#define GET_SO_FULL_NAME(soname)	({ string PET_SO_PATH;	PET_SO_PATH += getProgDir();	PET_SO_PATH += "/lib/"; PET_SO_PATH += soname;PET_SO_PATH;})


#define SYSTEM_ZHCN_UTF8	("\xe7\xb3\xbb\xe7\xbb\x9f")


#define getTime00(tNow)	getTimeHMS(tNow, 0, 0, 0)


#ifdef WIN32
#define strcasecmp _stricmp
#define STREQL_IGNORE_CASE(s1,s2) ( strcasecmp((const char *)s1, (const char *)s2) == 0)

#else
#define STREQL_IGNORE_CASE(s1,s2) ( strcasecmp((const char *)s1, (const char *)s2) == 0)

#endif

#define FIND_ELSE_RETURN(it, container, key)	\
	auto it = (container).find( key );\
	if ( it == (container).end() ) return

#define FOR_EACH( container, it ) for( auto it = (container).begin(); it!= (container).end();  ++it )
#define WHILE_EACH( container, it ) for( auto it = (container).begin(); it!= (container).end();  )

#define TIME_BETWEEN(tNow, tStart, tEnd)	( (DWORD)tNow >= tStart  && (DWORD)tNow < tEnd )

#define ZERO_CONS(type) type(void){ memset(this, 0, sizeof(*this) ); }



//#define Compress(a,b,c,d) compress2((Bytef*)a, (uLongf*)b, (const Bytef*)c, (uLong)d, -1)


//#define UnCompress(dst, dst_size, src, src_size) uncompress((Bytef*)(dst), (uLongf*)(dst_size), (const Bytef*)(src), (uLong)(src_size))

	
#define UNCOMPRESS_ROLE_BLOB(dst, src, src_size, ret) \
		do{\
		unsigned long dst_size = sizeof(ROLE_BLOB);\
		if ( UnCompress(dst, &dst_size, src, src_size) != Z_OK || dst_size != sizeof(ROLE_BLOB)) {\
			logFATAL("%d != %d", dst_size, sizeof(ROLE_BLOB));\
			ret = false;\
		}}while(0)

#define ESTIMATE_COMPRESS_BOUND(sourceLen) (sourceLen + (sourceLen >> 12) + (sourceLen >> 14) + 11)


#define RM_CCONST(cstr) (const_cast<char*>(cstr))
#define RM_CONST(str) (const_cast<char*>(str.c_str()))

#define FIVE_MIN_SECS (5*60)
#define ONE_MIN_SECS (1*60)



#define CORE_SIZE			(1024 * 1024 * 1024)
#define MSGQUEE_SIZE		( 64 * 1024 * 5000)
#define OPEN_FILES_NUM		( 4096 )


#define strcpy_s(to, len, from) strncpy(to, from, len)

#define SPLIT_SPACE	"\x20\r\t"


//#define ADDUP(orig, delta) 	do { orig=GET_ADDUP(orig, delta); }while(0)

#define ADDUP(orig, delta) 	do { orig=GET_ADDUP_HELPER((orig), (delta), 0, VALUE_MAX); }while(0)
#define ADDUP3(orig, delta, maX) 	do { orig=GET_ADDUP_HELPER((orig), (delta), 0, (maX)); }while(0)

#define RAND_0_1 ( rand()%2 )
#define RAND( num )		( BASE::randTo(num) )


#define RAND_HIT( per )		( BASE::randTo( 10000 ) < (DWORD)(per*10000) )

#define RAND_NOT_HIT( per )		( !RAND_HIT( per ) )

//#define GET_ADDUP(orig, delta) GET_ADDUP_HELPER(orig, delta, 0, VALUE_MAX)

#ifdef WIN32

#define GET_ADDUP_HELPER(orig, delta, min, max) \
	( ((long long)orig + (int)delta)<min ) ? min : ((long long)orig + (int)delta) > max ? max : ((long long)orig + (int)delta)
	
#else
#define GET_ADDUP_HELPER(orig, delta, min, max) \
	(\
		{\
			long long x=(long long)orig + (int)delta; \
			( x<min ) ? min : x > max ? max : x;\
		}\
	)
			
#endif

#define COND_BREAK(cond, ref, ret) \
	if ( (cond) )\
	{\
		ref = ret;\
		break;\
	}


namespace BASE
{

	enum
	{
		E_READ_CONTINUE = 1,
		E_READ_BREAK = 2,
	};
	
	enum
	{

		E_LAST_PKT_FILE_NONE = 0,
		E_LAST_PKT_FILE_REWIND = 1,
		E_LAST_PKT_FILE_TRUNCATE = 2,
		E_LAST_PKT_FILE_RECORD_ALL = 3,
	};

//	string md5(const string &str);

	bool parseBYTE( unsigned char*& p, size_t &leftLen, BYTE& out );
	bool parseWORD( unsigned char*& p, size_t &leftLen, WORD& out );
	bool parseDWORD( unsigned char*& p, size_t &leftLen, DWORD& out );

	bool parseNBYTE( unsigned char*& p, size_t &leftLen, string& out, size_t nBytes );
	bool parseBSTR( unsigned char*& p, size_t &leftLen, string& out );
	

	void* getDllFunc( void* handle, const char* funcName );
	void* loadLibFile(const char* soname, void* handle);

	int setrlimit(int resource, int SIZE);
	
#ifndef WIN32
void sleepUninteruptable( time_t sec, long milli_sec);
#define Sleep(millisec) BASE::sleepUninteruptable(millisec/1000, millisec%1000)

#endif
	unsigned long dataCompress(const char* src, unsigned long srcSize, char* dst );

	DWORD randTo(DWORD dwMax);
	DWORD getTickCount();


	int randBetween(int nMin, int nMax);
	int getIndexOfVectorSum(const std::vector<DWORD> &vPartialSum);

	string& getUUID(string& uuid);
	string getUUID2();

	void writeLastPktFile(DWORD dwRoleID, const char* szBuf, int nLen);

bool eachLine(const std::string& strFile, std::function<void ( const char* )> func );
bool isComment( const std::string& strLine);
bool eachIniSecLine(const std::string& strFile, const std::string& strSecName, std::function<void ( const char* )> func );
bool eachIniSecLine2(const string& strFile, const string& strSecName, std::function<int ( char* )> func );

bool eachIniSecLine3(FILE *fp, const string& strSecName, std::function<int ( char* )> func );


void setTimeInfo(time_t tWhen, int& y, int& mon, int& d, int& h, int& m, int& s);


typedef std::string& (*TRIM_FUNC)(std::string& str, const char* trim_char);
std::string& stringTrimAll(std::string& str, const char* trim_char);
int strtoken2Vec( const char* str, const char* split, std::vector<std::string> &vec, TRIM_FUNC trim_func=NULL, const char* trim_char=NULL);

std::string& stringReplaceAll(std::string & str, const std::string & strsrc, const std::string &strdst);

std::string& stringTrim(std::string& str, const char* trim_char);

int initWsadata();
bool inTheSameDay(time_t a, time_t b);
bool inTheSameMonth(time_t a, time_t b);
//bool isNewDay();
SOCKET ConnectServer(const char*ip, short port, int timeout_secs=5);
int inet_aton( const char *cp, struct in_addr *pin );
bool SocketOk(SOCKET sock);
int Write(SOCKET hSocket, const void* szBuf, int nLen);


int initWsadata();

template<typename T>
T convertSTR( const char* str);

template<>
int convertSTR( const char* str);


template<>
float convertSTR( const char* str);

template<>
string convertSTR( const char* str);


template<typename T>
T readIni( const string& fullPathFile, const string& sec, const string& key, const T& defaultValue )
{
	T v = defaultValue;
	
	eachIniSecLine2(fullPathFile, sec, [&](  char* line )
		{
			char* right = strchr( line, '=' );
			if (!right)
				return E_READ_CONTINUE;

			*right++=0;

			string left(line);

			if ( stringTrimAll(left, SPLIT_SPACE) == key )
			{
				//v = ATOI_( right );
				v = convertSTR<T>( right );
				return E_READ_BREAK;
			}

			return E_READ_CONTINUE;

		}
	);

	return v;

}




template<typename T>
T readIni3( FILE* fp, const string& sec, const string& key, const T& defaultValue )
{

	if ( fp==NULL )
		return defaultValue;
	
	T v = defaultValue;
	eachIniSecLine3(fp, sec, [&](  char* line )
		{
			char* right = strchr( line, '=' );
			if (!right)
				return E_READ_CONTINUE;

			*right++=0;

			string left(line);

			if ( stringTrimAll(left, SPLIT_SPACE) == key )
			{
				//v = ATOI_( right );
				v = convertSTR<T>( right );
				return E_READ_BREAK;
			}

			return E_READ_CONTINUE;

		}
	);

	return v;

}


}


string BigendianIP2Str(DWORD bigend_ip);
char* BigendianIP2Str2(DWORD bigend_ip, char* ip);

DWORD CStr2BigendianIP(const char* ip);


#if defined(__linux__)

int bind_socket(const char *addr, unsigned short port, int backlog);


#endif

#define GET_EVENT_ID()	((SNOW_FLAKE_UUID::getInstance())->generate())

class SNOW_FLAKE_UUID
{
public:

	static SNOW_FLAKE_UUID* getInstance();
	U64 generate();
		
private:

	//Ã¿´ÎÆô¶¯
	SNOW_FLAKE_UUID( U64 machine=0, U64 sequence=0);

	static SNOW_FLAKE_UUID *_snow_flake_uuid; // pointer to singleton

	U64 getTime();

	U64 _time;
	U64 _machine;
	U64 _sequence;
};





static_assert( sizeof(time_t) == 4,	"====time_t size error===");






#endif
