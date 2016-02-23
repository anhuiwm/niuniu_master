#include "pch.h"
#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"Rpcrt4")

#else
#include <limits.h>
#include "j2.h"
#endif

#include "service.h"



int doMain(int argc, char **argv)
{
	service service;
	return service.run(argc, argv);
//#if defined(__linux__)
//	BASE::setrlimit(RLIMIT_CORE, 1024 * 1024 * 1024);
//#endif
}

#ifndef _WIN32

int					m_SocketPair[2];
sem_t				m_semSenderExit;


int UnInit(int argc, char** argv)
{
//	for(int i=0;i<LOGS_PARSE_THREAD_COUNT;i++) {
//		PSTC_Socket_And_CMD_GS pkt = MallocGSPacket(0, E_CENTRAL_STOP_THREAD, NULL, 0);	
//		POST_MESSAGE(m_csDequeue[i], m_deqPacket[i], pkt, m_semHavePacket[i]);
//	}

//	int threads = LOGS_PARSE_THREAD_COUNT;
//	while( threads-- > 0)
//		Sem_wait(&m_semSenderExit);


	//sem_destroy(&m_semHavePacket);
	//sem_destroy(&m_semSenderExit);
	//fclose(m_fpEventID);

	//logv("xxxxxxxxxxx");

	return 0;
}


int Init(int argc, char** argv)
{
	//m_bMainloopExit = false;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0,  m_SocketPair) == -1)
		EXIT(EXIT_INIT);

	Sem_init(&m_semSenderExit, 0, 0);

	return 0;
}

static void term_handler(int signo)
{
	//LoggerServer::m_bMainloopExit = true;
	
	SEND_CMD_TO_MAINLOOP(E_SOCKETPAIR_EXIT_THREAD);
}

static void hup_handler(int signo)
{
	SEND_CMD_TO_MAINLOOP(E_SOCKETPAIR_UPDATE_TRUSTIP);
}

static void quit_handler(int signo)
{
	char szLogfile[MAX_PATH_LEN];
	GetEventLogFileName( szLogfile, 65535, 60 );
	debugv("%s", szLogfile);
}


//========================================================================

#define PROG_NAME				("MAIN SERVER")
#define MAIN_SERVER_VERSION		"1.0.1.1"


#endif

int main(int argc, char **argv)
{
#ifdef __linux__
#define CORE_SIZE			(1024 * 1024 * 1024)
#define MSGQUEE_SIZE		( 64 * 1024 * 5000)
#define OPEN_FILES_NUM		( 4096 )
	Setrlimit(RLIMIT_CORE, CORE_SIZE);
	Setrlimit(RLIMIT_MSGQUEUE, MSGQUEE_SIZE);
	Setrlimit(RLIMIT_NOFILE, OPEN_FILES_NUM * 2);// *(long*)0 = 9;
#endif
//#ifdef _WIN32
	return doMain( argc, argv );

//#else
//	return Server_Main(argc, argv, PROG_NAME, MAIN_SERVER_VERSION, 
//		Init, UnInit, NULL, doMain,
//		term_handler, hup_handler, quit_handler);
//#endif

}


//mount -t tmpfs tmpfs  /tmp/tmpfs



