#ifndef TYPEDEF_H
#define TYPEDEF_H

#pragma once

#define V_UNUSED(x)				((void)(x))


//#ifdef __cplusplus
//extern "C" {
//#endif

#ifdef WIN32

typedef __int64 I64;            /* 64 bit signed */
typedef unsigned __int64 U64;  /* 64 bit unsigned */


#define INT64_CON(c) c ## i64    /* signed 64 bit constant */
#define UINT64_CON(c) c ## ui64   /* unsigned 64 bit constant */

#else

typedef long long I64;           /* 64 bit signed */
typedef unsigned long long U64; /* 64 bit unsigned */

#define INT64_CON(c) static_cast<I64>(c ## LL)     /* signed 64 bit constant */
#define UINT64_CON(c) static_cast<U64>(c ## ULL) /* unsigned 64 bit constant */

#endif


#if defined(__linux__)

#define WINAPI
#define INVALID_HANDLE_VALUE ( (void*)(-1) )
#define memcpy_s(d, dl, s, sl) memcpy(d, s, sl)
#define sprintf_s sprintf
#define wsprintf_s wsprintf
typedef char TCHAR;

typedef unsigned int       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned char		byte;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL		        *PBOOL;
typedef BOOL                *LPBOOL;
typedef BYTE                *PBYTE;
typedef BYTE                *LPBYTE;
typedef int                 *PINT;
typedef int                 *LPINT;
typedef WORD                *PWORD;
typedef WORD                *LPWORD;
typedef long                *LPLONG;
typedef DWORD               *PDWORD;
typedef DWORD               *LPDWORD;
typedef void                *LPVOID;
typedef const void          *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef void				*HANDLE;
typedef void *HINSTANCE;

#if !defined(__int64)
typedef long long	    __int64;
#endif




typedef char CHAR;
typedef short SHORT;
typedef long LONG;
//
// ANSI (Multi-byte Character) types
//
typedef CHAR *PCHAR, *LPCH, *PCH;
typedef const CHAR *LPCCH, *PCCH;

typedef  CHAR *NPSTR, *LPSTR, *PSTR;
typedef  PSTR *PZPSTR;
typedef  const PSTR *PCZPSTR;
typedef  const CHAR *LPCSTR, *PCSTR;
typedef  PCSTR *PZPCSTR;

typedef LPSTR LPTCH, PTCH;
typedef LPSTR PTSTR, LPTSTR, PUTSTR, LPUTSTR;
typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;

typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character


typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;


#endif



#ifdef WIN32
// disable warning C4996: 'xxx' was declared deprecated
#pragma warning( disable : 4996 )
// disable warning C4220: zero elements in array definition
#pragma warning( disable : 4200 )

#if !defined(snprintf)
#define snprintf _snprintf
#endif

#endif



// other re-defines
#if defined(__linux__)

#define STR_FSEP				"/"
#define CHAR_FSEP				'/'
#define INVALID_SOCKET			(-1)
#define SOCKET_ERROR            (-1)
#define socket_errno			(errno)
#define general_errno			(errno)
#define PERCENT_I64d			"%lld"
#define PERCENT_U64d		"%llu"
//#define SOCKET					int
typedef int SOCKET;

#if !defined(__always_inline)
#define __always_inline		inline __attribute__((always_inline))
#endif

#define closesocket(x)			close(x)
#define IS_E_AGAIN				(socket_errno==EAGAIN)

#else

#define STR_FSEP				"\\"
#define CHAR_FSEP				'\\'
#define socket_errno			WSAGetLastError()
#define general_errno			GetLastError()
#define PERCENT_I64d			"%I64d"
#define PERCENT_U64d		"%I64u"
#define __always_inline		__forceinline
typedef int socklen_t;
typedef int ssize_t;

//#include <errno.h>
#define IS_E_AGAIN				(socket_errno==WSAEWOULDBLOCK)
//#ifndef EINPROGRESS
//#define EINPROGRESS			(socket_errno==WSAEINPROGRESS)
//#endif

#endif


#define MAX_LINE_LEN 1024
#define PATH_LENGTH 256
#define MAX_PATH_LEN (512ul)
#define MAX_FORMAT_LEN (32ul)

#define S_APPEND_INT(s,i)		s.append((char*)&i, sizeof(i))
#define S_APPEND_CONVERT_INT(s, source, des) {des = source; S_APPEND_INT(s, des);}
#define S_APPEND_STR(s,str)		s.append(str)
#define S_APPEND_SZ(s, sz, len) s.append(&(sz)[0], len);
#define S_APPEND_BSTR(s,str)	do{WORD __dwBstrApp=(WORD)str.size();s.append((char*)&__dwBstrApp, sizeof(__dwBstrApp));s.append(str);}while(0)
#define S_APPEND_BSZ(s, sz)		do {WORD __dwBszLen=(WORD)strlen(sz);s.append((char*)&__dwBszLen, sizeof(__dwBszLen));s.append(sz);}while(0)



#define S_APPEND_NBYTES(s, sz, len) s.append( (char*)(sz), len);


#define S_APPEND_BSZZZ(s, sz, len)		{WORD X=(WORD)len;s.append((char*)&X, sizeof(X));s.append(sz, (size_t)(len) );}

#define S_APPEND_BSTR_NULL(s)	do  { WORD X=0; s.append((char*)&X, sizeof(X)); } while(0)

#define S_APPEND_ZERO(s, length)	do  { s.resize( s.size() + (length), 0 ); } while(0)

#define S_APPEND_WORD(s,i)		do  { WORD X=(WORD)(i); s.append((char*)&X, sizeof(X)); } while(0)
#define S_APPEND_DWORD(s,i)		do  { DWORD X=(DWORD)(i); s.append((char*)&X, sizeof(X)); } while(0)
#define S_APPEND_BYTE(s,i)		do  { BYTE X=(BYTE)(i); s.append((char*)&X, sizeof(X)); } while(0)
#define S_APPEND_INT32(s,i)		do  { int X=(int)(i); s.append((char*)&X, sizeof(X)); } while(0)
#define S_APPEND_SHORT(s,i)		do  { short X=(short)(i); s.append((char*)&X, sizeof(X)); } while(0)
#define S_APPEND_CHAR(s,i)		do  { char X=(char)(i); s.append((char*)&X, sizeof(X)); } while(0)

#define S_APPEND_STRUCT(s, stu)		do  { s.append((char*)&(stu), sizeof(stu)); } while(0)


#define APPEND_SIZETYPE(sizetype,p,i)		do  { sizetype X=(sizetype)(i); *(sizetype*)(p) = X; p+=sizeof(sizetype);} while(0)

#define APPEND_BYTE(p,i)	APPEND_SIZETYPE(BYTE,p,i)
#define APPEND_WORD(p,i)	APPEND_SIZETYPE(WORD,p,i)
#define APPEND_DWORD(p,i)	APPEND_SIZETYPE(DWORD,p,i)
#define APPEND_FLOAT(p,i)	APPEND_SIZETYPE(float,p,i)
#define APPEND_INT(p,i)		APPEND_SIZETYPE(INT,p,i)
#define APPEND_BSTR(p, cppstr)	do  { WORD len=cppstr.size(); *(WORD*)(p) = len; p+=sizeof(WORD); memcpy(p, cppstr.data(),len); p+=len; } while(0)



#define PROTO_APPEND_SIZETYPE(sizetype,proto,i)		do  { sizetype X=(sizetype)(i); *(sizetype*)(proto->data+proto->dataLen) = X; proto->dataLen +=sizeof(sizetype);} while(0)


#define PROTO_APPEND_BYTE(i)		PROTO_APPEND_SIZETYPE(BYTE,proto,i)
#define PROTO_APPEND_WORD(i)	PROTO_APPEND_SIZETYPE(WORD,proto,i)
#define PROTO_APPEND_DWORD(i)	PROTO_APPEND_SIZETYPE(DWORD,proto,i)
#define PROTO_APPEND_FLOAT(i)	PROTO_APPEND_SIZETYPE(float,proto,i)

#define PROTO_APPEND_BSTR(cppstr)	do  { WORD len=cppstr.size(); *(WORD*)(proto->data+proto->dataLen) = len; proto->dataLen +=sizeof(WORD); memcpy(proto->data+proto->dataLen, cppstr.data(),len); proto->dataLen+=len; } while(0)


#define PROTO_APPEND_NBYTE(addr, len)	do  { memcpy( proto->data+proto->dataLen, (addr), (len) ); proto->dataLen+=(len); } while(0)

#define PROTO_APPEND_STRUCT(stu)	do  { PROTO_APPEND_NBYTE( &stu, sizeof(stu) ); } while(0)


#define ATOUL_16(s) strtoul(s, (char **) NULL, 16)

#define ATOL(s) strtol(s, (char **) NULL, 10)
#define ATOUL(s) strtoul(s, (char **) NULL, 10)

#define ATOLL(s) strtoll(s, (char **) NULL, 10)

#ifdef WIN32

#define ATOULL(s) _strtoui64(s, (char **) NULL, 10)

#else
#define ATOULL(s) strtoull(s, (char **) NULL, 10)
#endif


#define ATOW(s) ( (WORD)strtoul(s, (char **) NULL, 10))
#define ATOW_16(s) ( (WORD)strtoul(s, (char **) NULL, 16))
#define ATOB(s) ((BYTE)ATOUL( s ))



#define ATOL_(cppstr) ATOL( cppstr.c_str() )
#define ATOUL_(cppstr) ATOUL( (cppstr).c_str() )
#define ATOF(sz) ( (float)strtod( sz, (char **) NULL) )

//#define ATOINT(s) strtoint(s, (char **) NULL, 10)
#define ATOI(sz)	( strtol(sz, NULL, 10) )
#define ATOI_(cppstr) ATOI( cppstr.c_str() )

#define ATOW_(cppstr) ATOW( cppstr.c_str() )
#define ATOF_(cppstr) ATOF( cppstr.c_str() )

#define ATOB_(cppstr) ((BYTE)ATOUL( cppstr.c_str() ))


//#ifdef __cplusplus
//}
//#endif

#endif //_BASETYPES_H
