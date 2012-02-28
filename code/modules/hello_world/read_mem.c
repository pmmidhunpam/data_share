#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>


#include <linux/file.h>


#include <linux/sysfs.h>
#include <linux/string.h>

#include <linux/fs.h>
#include <asm/uaccess.h>
MODULE_LICENSE("GPL");

struct kobject *read_mem_kobj;

unsigned int start_addr;
unsigned int mem_len;


static ssize_t cmd_attr_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
	printk("cmd_attr_show start, [%s]\n", (char *)start_addr);
	memcpy(buf, (char *)start_addr, mem_len);
	return mem_len;
}


static ssize_t  cmd_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{

	char my_buf[1024];
	memset(my_buf, 0, sizeof(my_buf));
	strcpy(my_buf, buf);


//	printk("jiffies_64 = %lld\n", jiffies_64);
//	printk("cmd_attr_store buf = [%s]\n", my_buf);

//	unsigned int start_addr = NULL;
//	unsigned int mem_len = 0;
	char *out_file = NULL;

	char *p = my_buf;

	char *tmp = p;
	while (*p++ != ' ');
    start_addr = simple_strtoul(tmp, my_buf+count, 10);

	
	tmp = p;
	while (*p++ != ' ');
    mem_len = simple_strtoul(tmp, my_buf+count, 10);


	out_file = p;
	my_buf[count-1] = '\0';
	
	printk("start_addr = %u, mem_len = %u, out_file = [%s]\n", start_addr, mem_len, out_file);

	/////////////////////////////////////////////////////////
    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS);

	struct flie *filp = filp_open(out_file, O_CREAT|O_RDWR, 0);
	if (filp == NULL)printk("filp open failed\n");

	loff_t pos = 0;
	int n = vfs_write(filp, (char*)start_addr, mem_len, &pos);
	printk("write %d bytes\n", n);

	
	set_fs(oldfs);	

    filp_close(filp, NULL);
	/////////////////////////////////////////////////////////

	return count;
}

static struct kobj_attribute cmd_attr =
	__ATTR(cmd_attr, 0666, cmd_attr_show, cmd_attr_store);


static struct attribute * g[] = {
    &cmd_attr,	
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = g,
};


static int read_mem_init(void) {
    printk(KERN_EMERG "read_mem start\n"); 

    read_mem_kobj = kobject_create_and_add("read_mem", NULL);
	if (!read_mem_kobj)
		return -ENOMEM;
	return sysfs_create_group(read_mem_kobj, &attr_group);
}

static void read_mem_exit(void) {
	kobject_del(read_mem_kobj);
    printk(KERN_EMERG "read_mem end\n");
}

module_init(read_mem_init);
module_exit(read_mem_exit);
