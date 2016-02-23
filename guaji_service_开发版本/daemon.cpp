#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

using namespace std;


int main(int argc,char* args[])
{
	int pid = fork();
	while(1)
	{
		if(pid == 0)
		{
			execv("child", args);
			break;
		}
		else
		{
			int status;
			wait(&status);
			if(WIFEXITED(status))
				return 0;
			pid = fork();
		}
	}

	return 0;
}
