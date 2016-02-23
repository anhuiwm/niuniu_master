#pragma once

// libevent
#include <event2/util.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/event_struct.h>
#include <event2/event_compat.h>

// mysql
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>

// std
#include <set>
#include <list>
#include <tuple>
#include <array>
#include <regex>
#include <thread>
#include <queue>
#include <mutex>
#include <deque>
#include <fstream>
#include <sstream>
#include <numeric>
#include <memory>
#include <iostream>
#include <algorithm>

// typedef
#ifdef __linux__
typedef unsigned char byte;
typedef unsigned char uint8;
typedef char int8;
typedef unsigned uint;
typedef unsigned uint32;
typedef unsigned dword;
typedef unsigned short uint16;
typedef unsigned short word;
typedef unsigned short ushort;
typedef unsigned long long uint64;
typedef unsigned short uint16;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned long ulong;
#else
typedef unsigned char byte;
typedef unsigned char uint8;
typedef char int8;
typedef unsigned uint;
typedef unsigned uint32;
typedef unsigned dword;
typedef unsigned short uint16;
typedef unsigned short word;
typedef unsigned short ushort;
typedef unsigned __int64 uint64;
typedef unsigned short uint16;
typedef short int16;
typedef int int32;
typedef __int64 int64;
typedef unsigned long ulong;
#endif


// windows c api
#ifdef __linux__
#define stricmp strcasecmp
#endif


#include "base.h"
#include "log.h"
#include "item.h"
#include "config.h"
#include "session.h"

#include "input_stream.h"
#include "output_stream.h"

#include "tinyxml.h"
#include "tinyxml_extend.h"

using namespace std;
