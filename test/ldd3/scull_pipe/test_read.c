#include <unistd.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#include <stdio.h>

int main(int argc, char **argv) {
  int fd = open("/dev/scull_pipe0", O_RDONLY);
  perror("open()");
  
  int nr_read = atoi(argv[1]);

  char buf[2048];
  memset(buf, 0, sizeof(buf));
  int retval = read(fd, buf, nr_read);
  printf("retval = %d\n", retval);
  printf("buf = %s\n", buf);
  return 0;
  }
