#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>

#include "scull_fops.h"

extern struct scull_dev dev;
extern struct file_operations scull_fops;

MODULE_LICENSE("Dual BSD/GPL");

static dev_t devno;
static int major = 0;
static int minor = 0;
static int nr_devs = 4;

//init
static int scull_init(void);
//exit
static void scull_exit(void);
//分配设备
static int alloc_dev(void);
//注册设备
static int register_dev(void);

module_init(scull_init);
module_exit(scull_exit);

static int 
scull_init(void) {
  DEBUG_LOG("%s", "hello world");
  DEBUG_LOG_NULL;
  int a = 100;
  DEBUG_LOG_VALUE(a, %d);
  //printk("%s:%d\n", __FILE__, __LINE__);
  printk("The process is \"%s\" (pid %i)\n", current->comm, current->pid);
  if (alloc_dev() == -1) {
    printk("scull: can't alloc devno, %i\n", major);
    return -1;
  }
  if (register_dev() == -1) {
    printk("scull, register_dev() failed\n");
    return -1;
  }
  return 0;
}

static void 
scull_exit(void) {
  printk("Goodbye, scull %i,%i\n", major, minor);
  unregister_chrdev_region(devno, nr_devs);
  cdev_del(&dev.cdev);
}


static int 
alloc_dev(void) {
  int result = 0;
  printk("scull, alloc_dev() start\n");
  if (major == 0) {
    result = alloc_chrdev_region(&devno, minor, nr_devs, "scull");
    major = MAJOR(devno);
  } else {
    devno = MKDEV(major, minor);
    result = register_chrdev_region(devno, nr_devs, "scull");
  }
  return result;
}

static int
register_dev(void) {
  printk("scull, register_dev() start\n");
  dev.data = NULL;
  dev.quantum = SCULL_QUANTUM;
  dev.qset = SCULL_QSET;
  dev.size = 0;
  init_MUTEX(&dev.sem);
  
  cdev_init(&dev.cdev, &scull_fops);
  dev.cdev.owner = THIS_MODULE;
  dev.cdev.ops = &scull_fops;
  return cdev_add(&dev.cdev, devno, 1);
}

