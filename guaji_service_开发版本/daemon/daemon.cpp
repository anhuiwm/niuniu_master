#ifdef __linux__
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <signal.h>


using namespace std;


void sig_int(int id)
{
	cout << "sig int" << endl;
//	_exit(0);
}


int main(int argc,char* args[])
{
	if(argc < 2)
	{
		cout << "paramater count error, please add child process name." << endl;
		return 0;
	}
	signal(SIGINT, sig_int);
	int pid = fork();
	while(1)
	{
		if(pid == 0)
		{
			execv(args[1], args);
			break;
		}
		else
		{
			int status;
			wait(&status);
			if(WIFEXITED(status))
			{
				cout << "child process exit normally." << endl;
				return 0;
			}
			pid = fork();
		}
	}

	return 0;
}
#else
#include <winsock2.h>
#include <Windows.h>
#include "../thirdparty/event2/event.h"
#include "../thirdparty/event2/buffer.h"
#include "../thirdparty/event2/http.h"
//#include "../thirdparty/event2/event_compat.h"
//#include "../thirdparty/event2/listener.h"
//#include "../thirdparty/event2/util.h"
//#include "../thirdparty/event2/http_compat.h"

void process_request(evhttp_request* request, void * )
{
	auto uri = evhttp_request_get_evhttp_uri(request);

	auto output_headers = evhttp_request_get_output_headers(request);

	evhttp_add_header(output_headers, "Server", "guaji");
	evhttp_add_header(output_headers, "Content-Type", "text/plain; charset=gb2312");
	evhttp_add_header(output_headers, "Connection", "close");

	struct evbuffer *buf = evbuffer_new();
	evbuffer_add_printf(buf, "It works!\n%s\n", "this is ok");
	evhttp_send_reply(request, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

int main(int argc, char* args[])
{
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
	//event_init();
	auto base = event_base_new();
	auto http = evhttp_new(base);
	evhttp_bind_socket(http, "0.0.0.0", 800);
	evhttp_set_cb(http, "/", process_request, 0);
	event_base_dispatch(base);
	//event_dispatch();
	evhttp_free(http);
	event_base_free(base);
	return 0;
}
#endif