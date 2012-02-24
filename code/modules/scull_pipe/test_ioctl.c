#include <unistd.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#include <stdio.h>

#define SCULL_IOC_MAGIC 'k'
#define SCULL_IOCRESET _IO(SCULL_IOC_MAGIC, 0)
/*
 * S means "Set" through a ptr,
  * T means "Tell" directly with the argument value
   * G means "Get": reply by setting through a pointer
    * Q means "Query": response is on the return value
     * X means "eXchange": switch G and S atomically
      * H means "sHift": switch T and Q atomically
       */
       #define SCULL_IOCSQUANTUM _IOW(SCULL_IOC_MAGIC, 1, int)
       #define SCULL_IOCSQSET _IOW(SCULL_IOC_MAGIC, 2, int)
       #define SCULL_IOCTQUANTUM _IO(SCULL_IOC_MAGIC, 3)
       #define SCULL_IOCTQSET _IO(SCULL_IOC_MAGIC, 4)
       #define SCULL_IOCGQUANTUM _IOR(SCULL_IOC_MAGIC, 5, int)
       #define SCULL_IOCGQSET _IOR(SCULL_IOC_MAGIC, 6, int)
       #define SCULL_IOCQQUANTUM _IO(SCULL_IOC_MAGIC, 7)
       #define SCULL_IOCQQSET _IO(SCULL_IOC_MAGIC, 8)
       #define SCULL_IOCXQUANTUM _IOWR(SCULL_IOC_MAGIC, 9, int)
       #define SCULL_IOCXQSET _IOWR(SCULL_IOC_MAGIC,10, int)
       #define SCULL_IOCHQUANTUM _IO(SCULL_IOC_MAGIC, 11)
       #define SCULL_IOCHQSET _IO(SCULL_IOC_MAGIC, 12)

       #define SCULL_IOC_MAXNR 14


int main() {
  int fd = open("/dev/scull0", O_RDONLY);
  perror("open()");
  int quantum= 100;
  ioctl(fd,SCULL_IOCSQUANTUM, &quantum);  /* Set by pointer */
  ioctl(fd,SCULL_IOCGQUANTUM, &quantum);  /* Get by pointer */
  printf("quantum = %d\n", quantum);
  quantum = ioctl(fd,SCULL_IOCQQUANTUM);  /* Get by return value */
  printf("quantum = %d\n", quantum);

  ioctl(fd,SCULL_IOCTQUANTUM, 200);  /* Set by value */
  ioctl(fd,SCULL_IOCGQUANTUM, &quantum);  /* Get by pointer */
  printf("quantum = %d\n", quantum);
  quantum = ioctl(fd,SCULL_IOCQQUANTUM);  /* Get by return value */
  printf("quantum = %d\n", quantum);


  quantum = 300;
  int ret = ioctl(fd, SCULL_IOCXQUANTUM, &quantum);  /* Exchange by pointer */
  printf("quantum = %d\n", quantum);
  ioctl(fd, SCULL_IOCGQUANTUM, &quantum);  /* Get by pointer */
  printf("quantum = %d\n", quantum);
  

  //quantum = ioctl(fd,SCULL_IOCHQUANTUM, quantum); /* Exchange by value */
  return 0;
  }
