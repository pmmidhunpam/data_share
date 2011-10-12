#ifndef _SCULL_FOPS_H_
#define _SCULL_FOPS_H_

#include <linux/sched.h>
#include <linux/types.h>
//file file_operations etc.
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/poll.h>
//copy
#include <asm/uaccess.h>

#include "klog.h"

struct scull_pipe_dev
{
        wait_queue_head_t inq, outq; /* read and write queues */
        char *buffer, *end; /* begin of buf, end of buf */
        int buffersize; /* used in pointer arithmetic */
        char *rp, *wp; /* where to read, where to write */
        int nreaders, nwriters; /* number of openings for r/w */
        struct fasync_struct *async_queue; /* asynchronous readers */
        struct semaphore sem;  /* mutual exclusion semaphore */
        struct cdev cdev;  /* Char device structure */
};


static loff_t scull_pipe_llseek(struct file *, loff_t, int);
static ssize_t scull_pipe_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t scull_pipe_write(struct file *, const char __user *, size_t, loff_t *);
static int scull_pipe_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static int scull_pipe_open(struct inode *, struct file *);
static int scull_pipe_release(struct inode *, struct file *);
static unsigned int scull_pipe_poll(struct file *filp, poll_table *wait);
static int scull_pipe_trim(struct scull_pipe_dev *dev);
static int scull_pipe_freespace(struct scull_pipe_dev *dev);

#endif
