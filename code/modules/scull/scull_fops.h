#ifndef _SCULL_FOPS_H_
#define _SCULL_FOPS_H_

#include <linux/sched.h>
#include <linux/types.h>
//file file_operations etc.
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
//copy
#include <asm/uaccess.h>

#include "klog.h"

#define SCULL_QUANTUM 4000
#define SCULL_QSET 1000

struct scull_qset {
  void **data;
  struct scull_qset *next;
};

struct scull_dev {
  struct scull_qset *data;  /* Pointer to first quantum set */ 
  int quantum;  /* the current quantum size */ 
  int qset;  /* the current array size */ 
  unsigned long size;  /* amount of data stored here */ 
  unsigned int access_key;  /* used by sculluid and scullpriv */ 
  struct semaphore sem;  /* mutual exclusion semaphore  */ 

  struct cdev cdev; /* Char device structure */
};

static loff_t scull_llseek(struct file *, loff_t, int);
static ssize_t scull_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t scull_write(struct file *, const char __user *, size_t, loff_t *);
static int scull_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static int scull_open(struct inode *, struct file *);
static int scull_release(struct inode *, struct file *);


static int scull_trim(struct scull_dev *dev);
static struct scull_qset *scull_follow(struct scull_dev *dev, int item);



#include <asm/ioctl.h>

/*
type
    魔数. 只是选择一个数(在参考了 ioctl-number.txt之后)并且使用它在整个驱动中. 这个成员是 8 位宽(_IOC_T                                                      YPEBITS).
number
    序(顺序)号. 它是 8 位(_IOC_NRBITS)宽.
direction
    数据传送的方向,如果这个特殊的命令涉及数据传送. 可能的值是 _IOC_NONE(没有数据传输), _IOC_READ, _IOC_                                                      WRITE, 和 _IOC_READ|_IOC_WRITE (数据在2个方向被传送). 数据传送是从应用程序的观点来看待的; _IOC_READ 意                                                      思是从设备读, 因此设备必须写到用户空间. 注意这个成员是一个位掩码, 因此 _IOC_READ 和 _IOC_WRITE 可使用一                                                      个逻辑 AND 操作来抽取.
size
    涉及到的用户数据的大小. 这个成员的宽度是依赖体系的, 但是常常是 13 或者 14 位. 你可为你的特定体系在                                                      宏 _IOC_SIZEBITS 中找到它的值. 你使用这个 size 成员不是强制的 - 内核不检查它 -- 但是它是一个好主意. 正                                                      确使用这个成员可帮助检测用户空间程序的错误并使你实现向后兼容, 如果你曾需要改变相关数据项的大小. 如果你                                                      需要更大的数据结构, 但是, 你可忽略这个 size 成员. 我们很快见到如何使用这个成员.

*/

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

#endif
