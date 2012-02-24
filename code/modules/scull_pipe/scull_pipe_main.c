#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>

#include "scull_pipe_fops.h"

extern struct scull_pipe_dev dev;
extern struct file_operations scull_pipe_fops;

MODULE_LICENSE("Dual BSD/GPL");

static dev_t devno;
static int major = 0;
static int minor = 0;
static int nr_devs = 4;

//init
static int scull_pipe_init(void);
//exit
static void scull_pipe_exit(void);
//分配设备
static int alloc_dev(void);
//注册设备
static int register_dev(void);

module_init(scull_pipe_init);
module_exit(scull_pipe_exit);

static int 
scull_pipe_init(void) {
  DEBUG_LOG_VALUE(current->comm, %s);
  DEBUG_LOG_VALUE(current->pid, %d);
  if (alloc_dev() == -1) {
    DEBUG_LOG("alloc_dev() failed");
    return -1;
  }
  if (register_dev() == -1) {
    DEBUG_LOG("register_dev() failed");
    return -1;
  }
  return 0;
}

static void 
scull_pipe_exit(void) {
  DEBUG_LOG("Goodbye %d:%d", major, minor);
  unregister_chrdev_region(devno, nr_devs);
  cdev_del(&dev.cdev);
}


static int 
alloc_dev(void) {
  int result = 0;
  DEBUG_LOG_NULL;
  if (major == 0) {
    result = alloc_chrdev_region(&devno, minor, nr_devs, "scull_pipe");
    major = MAJOR(devno);
  } else {
    devno = MKDEV(major, minor);
    result = register_chrdev_region(devno, nr_devs, "scull_pipe");
  }
  return result;
}

static int
register_dev(void) {
  DEBUG_LOG_NULL;
  //init wait queue
  init_waitqueue_head(&dev.inq);
  init_waitqueue_head(&dev.outq);
  dev.buffersize = 1024;
  dev.buffer = kmalloc(dev.buffersize, GFP_KERNEL);
  dev.end = dev.buffer + dev.buffersize;
  dev.rp = dev.wp = dev.buffer;
  dev.nreaders = dev.nwriters = 0;
  init_MUTEX(&dev.sem);
  
  cdev_init(&dev.cdev, &scull_pipe_fops);
  dev.cdev.owner = THIS_MODULE;
  dev.cdev.ops = &scull_pipe_fops;
  return cdev_add(&dev.cdev, devno, 1);
}

