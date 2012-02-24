#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

int main(void)
{

  int fd = open("/dev/scull_pipe0", O_RDONLY);
  perror("open()");

  fd_set fdsr; //文件描述符集的定义

  while(1)
  {
    //初始化文件描述符集合
    FD_ZERO(&fdsr); //清除描述符集
    FD_SET(fd,&fdsr); //把sock_fd加入描述符集

    printf("now wait to select ...\n");
    int ret = select(fd+1,&fdsr,NULL,NULL, NULL);
    printf("select res = %d\n", ret);
    if(ret <0) //没有找到有效的连接 失败
    {
      perror("select error!\n");
      break;
    }
    //循环判断有效的连接是否有数据到达
    if(FD_ISSET(fd,&fdsr))
    {
      printf("fd now can read\n");
      char buf[1024];
      memset(buf, 0, sizeof(buf));
      int n = read(fd, buf, 1023);
      printf("read success %d bytes: %s\n", n, buf);
    }

  }
 return 0;
}

