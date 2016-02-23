#ifndef HEAD_H
#define HEAD_H


#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#define _USE_MATH_DEFINES
#include <math.h>

//#if defined(__linux__)
#ifndef _WIN32

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/io.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>

#include <sys/io.h>

#else
#include <winsock2.h>
#include <process.h>
#include <windows.h>
#include <io.h>

#endif


using namespace std;

#include "typedef.h"
#include "log.h"


#endif
