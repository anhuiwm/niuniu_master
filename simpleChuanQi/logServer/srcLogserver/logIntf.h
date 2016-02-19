#ifndef LOGINTF_H
#define LOGINTF_H




using namespace std;



#include <list>
#include <thread>
#include <queue>
#include <condition_variable>


#include "base.h"


#pragma pack(push, 1)
struct LOG_PROTO_HEAD
{
	WORD wLen;
	WORD wCmdType;
	char data[0];
};

struct LOG_RESET_LOGSERVER
{
	WORD wLen;
	WORD wCmdType;
	
	DWORD IP;
	int port;	
};

struct LOG_HEARTBEAT
{
	WORD wLen;
	WORD wCmdType;
	LOG_HEARTBEAT()
	{
		wLen = 4;
		wCmdType = 80;
	}
};


#pragma pack(pop)


#define LOG_PROTO_HEAD_LEN (sizeof(LOG_PROTO_HEAD))

#define RESET_LOGSERVER_CMD		0XFFFF

#define LOGINTF ( LogIntf::getInstance() )

class LogIntf
{
public:

	virtual ~LogIntf();

	struct LOG_MESSAGE
	{
		char* pkt;
		WORD pktLen;

//		LOG_MESSAGE(void* pkt, WORD pktLen)
//		{
//			this->pkt = (char*)pkt;
//			this->pktLen = pktLen;
//		}
		
		LOG_MESSAGE(void* pkt)
		{
			this->pkt = (char*)pkt;
			this->pktLen = *(WORD*)( (char*)pkt );
		}

		
	};


	static LogIntf* getInstance();

	void send(const LOG_MESSAGE& message);


private:

	static LogIntf *_logIntf; // pointer to singleton


	bool _needQuit;

	
	SOCKET _logSock;

	LogIntf();

	LogIntf( const LogIntf& ){};

	LogIntf& operator = (const LogIntf& ){};

	string _logIP;
	int _logPort;
	
	std::mutex       _queueMutex;

	std::condition_variable      _sleepCondition;

	std::list<LOG_MESSAGE > _queue;
	std::list<LOG_MESSAGE > _queue2;

	void logThread();

	void sendToLogsvr( const std::list<LOG_MESSAGE>& queue );

};






#endif
