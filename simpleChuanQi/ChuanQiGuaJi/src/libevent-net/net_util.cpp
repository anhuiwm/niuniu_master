#include "net_util.h"
#include <sys/types.h>


unsigned int peerSock2BigendianIP(evutil_socket_t sock)
{
	struct sockaddr_in sAddr; 

#ifdef _WIN32
	int nLen= sizeof(sAddr);
#else
	socklen_t nLen= sizeof(sAddr);
#endif



	//if ( getpeername(sock, ( struct sockaddr *)&sAddr, (int*)&nLen) == -1 )
	if ( getpeername(sock, ( struct sockaddr *)&sAddr, &nLen) == -1 )
		return 0;

	return sAddr.sin_addr.s_addr;
}





