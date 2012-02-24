#include "scull_fops.h"

struct scull_dev dev;
//EXPORT_SYMBOL(dev);

struct file_operations scull_fops = {
  .owner = THIS_MODULE,
  .llseek = scull_llseek,
  .read = scull_read,
  .write = scull_write,
  .ioctl = scull_ioctl,
  .open = scull_open,
  .release = scull_release
};
//EXPORT_SYMBOL(scull_fops);


static loff_t 
scull_llseek(struct file *filp, loff_t offp, int count) {
  return 0;
}

static ssize_t 
scull_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos) {
  printk("scull, scull_read() buff=%p, count=%i, *f_pos=%i\n", buff, count, *f_pos);
  struct scull_dev *dev = (struct scull_dev*)filp->private_data;
  struct scull_qset *p;
  int quantum = dev->quantum;
  int qset = dev->qset;
  int itemsize = qset * quantum;
  ssize_t retval = 0;

  if (down_interruptible(&dev->sem))
    return -ERESTARTSYS;
  if (*f_pos >= dev->size)
    goto out;
  if (*f_pos + count > dev->size)
    count = dev->size - *f_pos;

  int item = (long)*f_pos / itemsize;
  int rest = (long)*f_pos % itemsize;

  int s_pos = rest / quantum;
  int q_pos = rest % quantum;

  //follow the list up to the right qset
  p = scull_follow(dev, item);

  //printk("p = %p \n", p);
  //printk("p->data = %p \n", p->data);
  //printk("p->data[s_pos] = %p \n", p->data[s_pos]);
  
  if (p == NULL || p->data == NULL || p->data[s_pos] == NULL ) {
    printk("something is wrong\n");
    goto out;
  }
   
  char *str = p->data[s_pos] + q_pos;
  //read only up to the end of this quantum
  if (count > quantum - q_pos)
    count = quantum - q_pos;
  
  if (copy_to_user(buff, p->data[s_pos] + q_pos, count) != 0) {
    retval = -EFAULT;
    goto out;
  }  
  
  printk("scull, success read %i bytes\n", count);
  *f_pos += count;
  retval = count;

out:
  up(&dev->sem);
  return retval;
}

static ssize_t
scull_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos) {
  printk("scull, scull_write() start, buff=%p, count=%i, *f_pos=%i\n", buff, count, *f_pos);    
  struct scull_dev *dev = filp->private_data;
  int itemsize = dev->qset * dev->quantum;

  ssize_t retval = 0;
  if (down_interruptible(&dev->sem)) {
    return -ERESTARTSYS;
  }
  
  int item = (long)*f_pos / itemsize;
  int rest = (long)*f_pos % itemsize;
  int s_pos = rest / dev->quantum;
  int q_pos = rest % dev->quantum;
  
  struct scull_qset *p = scull_follow(dev, item);
  if (p == NULL) {
    printk("scull_follow() failed\n");
    goto out;
  }

  if (p->data == NULL) {
    p->data = kmalloc(sizeof(char*) * dev->qset, GFP_KERNEL);
    if (p->data == NULL)
      goto out;
    memset(p->data, 0, sizeof(char*) * dev->qset);  
  }
  if ((p->data)[s_pos] == NULL) {
    (p->data)[s_pos] = kmalloc(dev->quantum, GFP_KERNEL);
    if ((p->data)[s_pos] == NULL)
      goto out;
  }
  if (count > dev->quantum - q_pos)
    count = dev->quantum - q_pos;

  //printk("p = %p \n", p);
  //printk("p->data = %p \n", p->data);
  //printk("p->data[s_pos] = %p \n", p->data[s_pos]);

  if (copy_from_user(p->data[s_pos] + q_pos, buff, count) != 0) {
    retval = -EFAULT;
    goto out;
  }

  retval = count;
  *f_pos += count;
  
  if (dev->size < *f_pos) {
    dev->size = *f_pos;
  }
  printk("scull, copy_from_user %i bytes\n", count);

out:
  up(&dev->sem);
  return retval;
}

static int 
scull_open(struct inode *inode, struct file *filp) {
  printk("scull, scull_open() start\n");
  struct scull_dev *dev;
  dev = container_of(inode->i_cdev, struct scull_dev, cdev);
  filp->private_data = dev;

  if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
    scull_trim(dev);    
  }
  return 0;
}

static int
scull_release(struct inode *inode, struct file *filp) {
  printk("scull, scull_release() start\n");
  return 0;
}

static int 
scull_trim(struct scull_dev *dev) {
  printk("scull, scull_trim() start\n");
  struct scull_qset *p = dev->data;
  while (p != NULL) {
    int i;
    for (i=0; i<dev->qset; i++) {
      kfree((p->data)[i]);
    }
    kfree(p->data);
    struct scull_qset *next = p->next;
    kfree(p);
    p = next;
  }

  dev->size = 0;
  dev->data = NULL;
  return 0;
}

static struct scull_qset *
scull_follow(struct scull_dev *dev, int item) {
  printk("scull, scull_follow() start\n");
  struct scull_qset *p = dev->data;
  if (dev->data == NULL) {
    dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
    if (dev->data == NULL) 
      return NULL;
    p = dev->data;
    p->next = NULL;
    p->data = NULL;
  }

  while (item-- != 0) {
    if (p->next == NULL) {
      p->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
      if (p->next == NULL)
        return NULL;
      p->next->data = NULL;
      p->next->next = NULL;
    }
    p = p->next;
  }

  return p;
}

static int
scull_ioctl(struct inode * inode, struct file * flip, unsigned int cmd, unsigned long arg) {

  int err = 0, tmp;
  int retval = 0;

  printk("scull, scull_ioctl() start\n");

  if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC)
    return -ENOTTY;
  if (_IOC_NR(cmd) >= SCULL_IOC_MAXNR)
    return -ENOTTY;

  if (_IOC_DIR(cmd) & _IOC_READ != 0) {
    err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
  } else if (_IOC_DIR(cmd) & _IOC_WRITE != 0) {
    err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
  }

  if (err != 0)
    return -EFAULT;

  switch (cmd) {
    case SCULL_IOCRESET:
      dev.quantum = SCULL_QUANTUM;
      dev.qset = SCULL_QSET;
      break;
    case SCULL_IOCSQUANTUM: /* set: arg points to the value */
      if (capable(CAP_SYS_ADMIN) == 0)
        return -EPERM;
      retval = __get_user(dev.quantum, (int __user *)arg);
      break;
    case SCULL_IOCSQSET:
      if (capable(CAP_SYS_ADMIN) == 0)
        return -EPERM;
      retval = __get_user(dev.qset, (int __user *)arg);
      break;
    case SCULL_IOCTQUANTUM:
      if (capable(CAP_SYS_ADMIN) == 0)
        return -EPERM;
      dev.quantum = arg;
      break;
    case SCULL_IOCTQSET:
      if (capable(CAP_SYS_ADMIN) == 0)
        return -EPERM;
      dev.qset = arg;
      break;
    case SCULL_IOCGQUANTUM:
      printk("get: now quantum = %d\n", dev.quantum);
      retval = __put_user(dev.quantum, (int __user *)arg);
      break;
    case SCULL_IOCGQSET:
      retval = __put_user(dev.qset, (int __user *)arg);
      break;
    case SCULL_IOCQQUANTUM:
      return dev.quantum;
    case SCULL_IOCQQSET:
      return dev.qset;
    case SCULL_IOCXQUANTUM: //set + get
      if (capable(CAP_SYS_ADMIN) == 0)
        return -EPERM;
      tmp = dev.quantum;
      retval = __get_user(dev.quantum, (int __user *)arg);
      printk("ex: now quantum = %d\n", dev.quantum);
      if (retval == 0) 
        retval = __put_user(tmp, (int __user *)arg);
      break;
    case SCULL_IOCHQUANTUM: //tell + query
      if (capable(CAP_SYS_ADMIN) == 0)
        return -EPERM;
      tmp = dev.quantum;
      dev.quantum = arg;
      return tmp;
    default:
      printk("default \n");
      return -ENOTTY;
      break;
  }

  return retval;

}

