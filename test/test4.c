#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
        pid_t pid;

        if((pid = vfork()) < 0)
        {
                printf("fork error\n");

                return 1;
        }
        else if(pid == 0)
        {
                sleep(1);
                printf("this is child process,ID:%d,PPID:%d\n", getpid(),getppid());
                return 0;
        }
        else
        {
                printf("this is father process,ID:%d\n", getpid());
                return 0;
        }
	return 0;
}

