#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>


#include <linux/file.h>


#include <linux/sysfs.h>
#include <linux/string.h>

#include <linux/fs.h>
#include <asm/uaccess.h>

#include <asm/io.h>
#include <asm/system.h>


MODULE_LICENSE("GPL");

struct kobject *write_mem_kobj;

unsigned int start_addr;
unsigned int mem_len;

static ssize_t phy_mem_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
	printk("write use format: echo phy_mem_address in_file > phy_mem\n");
	return 0;
}


static ssize_t  phy_mem_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{

	char my_buf[1024];
	memset(my_buf, 0, sizeof(my_buf));
	strcpy(my_buf, buf);


	char *in_file = NULL;
	char *p = my_buf;
	char *tmp = p;
	while (*p++ != ' ');
    start_addr = simple_strtoul(tmp, my_buf+count, 10);

	
	in_file = p;
	my_buf[count-1] = '\0';
	
	printk("phy_mem_store, start_addr = %u, in_file = [%s]\n", start_addr, in_file);



	/////////////////////////////////////////////////////////
    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS);

	struct file *filp = filp_open(in_file, O_CREAT|O_RDWR, 0);
	if (filp == NULL) {
		printk("filp open failed\n");
		return -1;
	}
    
	unsigned int file_size = filp->f_path.dentry->d_inode->i_size;
	printk("%s file size = %u\n", in_file, file_size); 

	char *line_addr = NULL;
    line_addr = ioremap_nocache(start_addr, file_size);
    if (line_addr == NULL) {
        printk("ioremap_nocache() failed\n");
        return -1;
    }


	char line_buf[1024];
	p = line_addr;
	loff_t pos = 0;
	while (1) {
		int n = vfs_read(filp, (char*)line_buf, sizeof(line_buf), &pos);
		if (n == 0) {
			printk("read eof\n");
			break;
		} else if (n < 0) {
			printk("read failed\n");
			return -1;
		}
		memcpy(p, line_buf, n);
		p += n;
	}

	
	set_fs(oldfs);	

    filp_close(filp, NULL);
	/////////////////////////////////////////////////////////

	iounmap(line_addr);

	return count;
}

static ssize_t vir_mem_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
	printk("write use format: echo vir_mem_address in_file > vir_mem\n");
	return 0;
}


static ssize_t  vir_mem_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	char my_buf[1024];
	memset(my_buf, 0, sizeof(my_buf));
	strcpy(my_buf, buf);


	char *in_file = NULL;
	char *p = my_buf;
	char *tmp = p;
	while (*p++ != ' ');
    start_addr = simple_strtoul(tmp, my_buf+count, 10);

	
	in_file = p;
	my_buf[count-1] = '\0';
	
	printk("vir_mem_store, start_addr = %u, in_file = [%s]\n", start_addr, in_file);


	/////////////////////////////////////////////////////////
    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS);

	struct file *filp = filp_open(in_file, O_CREAT|O_RDWR, 0);
	if (filp == NULL) {
		printk("filp open failed\n");
		return -1;
	}
    
	unsigned int file_size = filp->f_path.dentry->d_inode->i_size;
	printk("%s file size = %u\n", in_file, file_size); 


	char line_buf[1024];
	p = start_addr;
	loff_t pos = 0;
	while (1) {
		int n = vfs_read(filp, (char*)line_buf, sizeof(line_buf), &pos);
		if (n == 0) {
			printk("read eof\n");
			break;
		} else if (n < 0) {
			printk("read failed\n");
			return -1;
		}
		memcpy(p, line_buf, n);
		p += n;
	}

	
	set_fs(oldfs);	

    filp_close(filp, NULL);
	/////////////////////////////////////////////////////////


	return count;
}

static struct kobj_attribute vir_mem =
	__ATTR(vir_mem, 0666, vir_mem_show, vir_mem_store);

static struct kobj_attribute phy_mem =
	__ATTR(phy_mem, 0666, phy_mem_show, phy_mem_store);


static struct attribute * g[] = {
    &vir_mem,	
    &phy_mem,	
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = g,
};


static int write_mem_init(void) {
    printk(KERN_EMERG "write_mem start\n"); 

    write_mem_kobj = kobject_create_and_add("write_mem", NULL);
	if (!write_mem_kobj)
		return -ENOMEM;
	return sysfs_create_group(write_mem_kobj, &attr_group);
}

static void write_mem_exit(void) {
	kobject_del(write_mem_kobj);
    printk(KERN_EMERG "write_mem end\n");
}

module_init(write_mem_init);
module_exit(write_mem_exit);
