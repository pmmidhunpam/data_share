#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

struct share_data {
int id;
pthread_mutex_t mutex;
} *data;

pthread_t ntid;

void *thr_fn(void *arg){
  printf("new thread:\n");
  sleep(100);
  return ((void *)0);
}

int main(int argc, char **argv)
{
  int err;

  err = pthread_create(&ntid,NULL,thr_fn,NULL);
  if(err != 0){
   printf("can't create thread: %s\n",strerror(err));
   return 1;
  }


    int pid,shmid;//后者为共享内存识别代号
    char *write_address;
    char *read_address;
    struct shmid_ds dsbuf;
   
    shmid = atoi(argv[1]);	 
    write_address=(char *)shmat(shmid,NULL,0);//连接共享内存
    if((int)write_address==-1)
    {
	perror("shmat");
        exit(3);
    }
    else
    {
        printf("shmat连接共享内存成功。\n");
	data = (struct share_data *)write_address;
	printf("read from share mem  data.id = %d\n", data->id);

	pthread_mutex_lock(&data->mutex);
	printf("oh yeah, success\n");
	pthread_mutex_unlock(&data->mutex);
	sleep(100);
    }
    return 0;
	
}
