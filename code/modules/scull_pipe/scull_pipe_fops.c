#include "scull_pipe_fops.h"

struct scull_pipe_dev dev;
//EXPORT_SYMBOL(dev);

struct file_operations scull_pipe_fops = {
  .owner = THIS_MODULE,
  .llseek = scull_pipe_llseek,
  .read = scull_pipe_read,
  .write = scull_pipe_write,
  .ioctl = scull_pipe_ioctl,
  .open = scull_pipe_open,
  .release = scull_pipe_release
};
//EXPORT_SYMBOL(scull_pipe_fops);


static loff_t 
scull_pipe_llseek(struct file *filp, loff_t offp, int count) {
  return 0;
}

static ssize_t 
scull_pipe_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
  DEBUG_LOG_NULL;
  struct scull_pipe_dev *dev = (struct scull_pipe_dev *)filp->private_data;
 
  //lock
  if (down_interruptible(&dev->sem) != 0) {
    DEBUG_LOG("down_interruptible() failed");
    return -ERESTARTSYS;
  }

  while (dev->rp == dev->wp) {
    //unlock
    up(&dev->sem); 

    //open nonblocked
    if (filp->f_flags & O_NONBLOCK != 0) {
      DEBUG_LOG("no resources current");
      return -EAGAIN;
    }

    //wait for event
    if (wait_event_interruptible(dev->inq, (dev->rp != dev->wp)) != 0) {
      DEBUG_LOG("wait_event_interruptible() failed");
      return -ERESTARTSYS;
    }

    //lock again
    if (down_interruptible(&dev->sem) != 0) {
      DEBUG_LOG("down_interruptible() failed");
      return -ERESTARTSYS;
    }
  }
 
  //now read
  int nr_read_left = (int)(dev->wp - dev->rp);
  count = count < nr_read_left ? count : nr_read_left; 

  if (copy_to_user(buf, dev->rp, count) != 0) {
    up(&dev->sem);
    DEBUG_LOG("copy_to_user() failed");
    return -EFAULT;
  }

  //update read pointer
  dev->rp += count;
  if (dev->rp == dev->end) {
    DEBUG_LOG("read to end");
    dev->rp = dev->buffer;
    dev->wp = dev->buffer;
  }
  
  //unlock
  up(&dev->sem);

  //wake up writes
  wake_up_interruptible(&dev->outq);

  DEBUG_LOG("read %d bytes : %s", count, buf);
  
  return count;
}

static ssize_t
scull_pipe_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
  DEBUG_LOG_NULL;
  struct scull_pipe_dev *dev = (struct scull_pipe_dev *)filp->private_data;
  
  if (down_interruptible(&dev->sem) != 0) {
    DEBUG_LOG("down_interruptible() failed");
    return -ERESTARTSYS;
  }
  
  while (scull_pipe_freespace(dev) == 0) {
    up(&dev->sem);
   
    DEBUG_LOG("no space to write, wait...");
    //define wait_queue_t
    DEFINE_WAIT(wait);

    prepare_to_wait(&dev->outq, &wait, TASK_INTERRUPTIBLE);
    
    if (scull_pipe_freespace(dev) == 0) {
      schedule();
    }

    DEBUG_LOG("wake up");
    finish_wait(&dev->outq, &wait);

    if (signal_pending(current) != 0) {
      DEBUG_LOG("signale wake up me");
      return -ERESTARTSYS;
    }
    
    if (down_interruptible(&dev->sem) != 0) {
      DEBUG_LOG("down_interruptible() failed");
      return -ERESTARTSYS;
    } 
  }
  DEBUG_LOG("now write, %d space avaiable!", scull_pipe_freespace(dev));

  count = count < scull_pipe_freespace(dev) ? count : scull_pipe_freespace(dev);

  DEBUG_LOG_VALUE(count, %d);

  if (copy_from_user(dev->wp, buf, count) != 0) {
    DEBUG_LOG("copy_from_user() failed");
    up(&dev->sem);
    return -EFAULT;
  }

  //update write pointer
  dev->wp += count;

  up(&dev->sem);

  //wake up readers
  wake_up_interruptible(&dev->inq);

  return count;
}

static int 
scull_pipe_open(struct inode *inode, struct file *filp) {
  printk("scull_pipe, scull_pipe_open() start\n");
  struct scull_pipe_dev *dev;
  dev = container_of(inode->i_cdev, struct scull_pipe_dev, cdev);
  filp->private_data = dev;

  if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
    scull_pipe_trim(dev);    
  }
  return 0;
}


static unsigned int 
scull_pipe_poll(struct file *filp, poll_table *wait) {
  DEBUG_LOG_NULL;
  struct scull_pipe_dev *dev = filp->private_data;
  
  down(&dev->sem);
  poll_wait(filp, &dev->inq, wait);
  poll_wait(filp, &dev->outq, wait);
  
  unsigned int mask = 0;

  //can read
  if (dev->wp > dev->rp) {
    DEBUG_LOG("read available");
    mask |= POLLIN | POLLRDNORM;  
  }

  //can write
  if (dev->wp != dev->end) {
    DEBUG_LOG("write available");
    mask |= POLLOUT | POLLWRNORM;
  }

  up(&dev->sem);
  return mask;
}

static int
scull_pipe_release(struct inode *inode, struct file *filp) {
  printk("scull_pipe, scull_pipe_release() start\n");
  return 0;
}

static int 
scull_pipe_trim(struct scull_pipe_dev *dev) {
  DEBUG_LOG_NULL;
  return 0;
}

static int
scull_pipe_ioctl(struct inode * inode, struct file * flip, unsigned int cmd, unsigned long arg) {
  DEBUG_LOG_NULL;
  return 0;
}

static int
scull_pipe_freespace(struct scull_pipe_dev *dev) {
  DEBUG_LOG_VALUE(dev->rp, %p);
  DEBUG_LOG_VALUE(dev->wp, %p);
  return dev->end - dev->wp;
}
