#include <iostream>
//#include <sys/types.h>
#include <signal.h>
#include <thread>
#ifdef linux
#include <unistd.h>
#include <sys/syscall.h> /*此头必须带上*/
#include <termios.h>


void echo_off()
{
	struct termios sg; 
	tcgetattr(0, &sg); /* get settings */
	sg.c_lflag &= ~ECHO; /* turn echo off */
	tcsetattr(0,TCSAFLUSH,&sg); /* set settings */
}

void echo_on()
{
	struct termios sg;
	tcgetattr(0, &sg); /* get settings */
	sg.c_lflag |= ECHO; /* turn echo on */
	tcsetattr(0,TCSAFLUSH,&sg); /* set settings */
}
#endif

using namespace std;

int count = 0;
void sig_proc(int id)
{
	cout << id << " sig_proc " << syscall(SYS_gettid) << endl;
	//if(++count>3)_exit(0);
}

void proc(void* p)
{
	sigset_t sigs;
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGINT);

	int sig;
	while(1)
	{
		sigwait(&sigs, &sig);
		cout << syscall(SYS_gettid) << endl;
		echo_on();
		cout << "please input command:" << endl;
		string cmd;
		cin >> cmd;
		if(cmd == "q" || cmd == "quit" || cmd == "exit")
		{
			cout << "exit" << endl;
			exit(0);
			break;
		}
		cout << "your inputed command is " + cmd << endl;
		echo_off();
	}
}

int main()
{
	echo_off();
	//kill(getpid(), SIGINT);
	sigset_t sigs;
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGINT);
	if(pthread_sigmask(SIG_BLOCK, &sigs, &sigs))cout << "sigmask failed!" << endl;
	thread t(proc, (void*)0);
	t.detach();
	cout << syscall(SYS_gettid) << endl;
	//signal(SIGINT, sig_proc);
	while(1){sleep(1);}
	return 0;
}
