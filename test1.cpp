#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>


#include <unistd.h>
#include <pthread.h>

struct share_data {
int id;
pthread_mutex_t mutex;
} ;


pthread_t ntid;

void *thr_fn(void *arg){
  printf("new thread:\n");
  sleep(100);
  return ((void *)0);
}

int main()
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
    if((shmid=shmget(IPC_PRIVATE,32,0))<0)//分配共享内存
    {
        printf("shmid共享内存分配出现错误。\n");
        exit(1);
    }
    else {
        printf("shmid共享内存分配成功，共享内存识别代号为：%d。\n",shmid);
    }
    
    write_address=(char *)shmat(shmid,NULL,0);//连接共享内存
    if((int)write_address==-1)
    {
	perror("shmat");
        exit(3);
    }
    else
    {
        printf("shmat连接共享内存成功。\n");
        //strcpy(write_address,"我是写入共享内存的测试数据");//将数据写入共享内存
	struct share_data *data = (struct share_data *)write_address;
	data->id = 888;
//	pthread_mutex_init(&data->mutex, NULL);
	data->mutex = PTHREAD_MUTEX_INITIALIZER;
	
	printf("lock \n");
        pthread_mutex_lock(&data->mutex);
	sleep(30);
	printf("unlock \n");
        pthread_mutex_unlock(&data->mutex);
	sleep(100);
	getchar();
        if((shmdt((void *)write_address))<0)//断开与共享内存的连接
            printf("shmdt共享内存断开错误。\n");
        else
            printf("shmdt共享内存断开成功。\n");
    }
    return 0;
	
}
