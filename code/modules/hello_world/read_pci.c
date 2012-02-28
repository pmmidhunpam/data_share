#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>


#include <linux/file.h>


#include <linux/sysfs.h>
#include <linux/string.h>

#include <asm-generic/uaccess.h>
#include <linux/fs.h>
#include <linux/syscalls.h>

#include <linux/pci.h>


MODULE_LICENSE("GPL");

struct kobject *read_pci_kobj;

static ssize_t pci_addr_attr_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) 
{
	printk("Please write use format: [busno:devno.fnno] in decimal!\n");
	return 0;
}


static ssize_t  pci_addr_attr_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{

//	printk("pci_addr_attr_store start, %s\n", buf);
	char my_buf[1024];
	memset(my_buf, 0, sizeof(my_buf));
	strcpy(my_buf, buf);

	
	int busno = 0;
	int devno = 0;
	int fnno  = 0;

	char *p = my_buf;
    busno = simple_strtoul(p, my_buf+count, 10);

	while (*p++ != ':');
    devno = simple_strtoul(p, my_buf+count, 10);
	
	while (*p++ != '.');
    fnno = simple_strtoul(p, my_buf+count, 10);


	printk("busno = 0x%0x, devno = 0x%0x, fnno = 0x%0x\n", busno, devno, fnno);
    struct pci_bus *bus = pci_find_bus(0, busno);
	if (bus == NULL) {
		printk("can't find bus!\n");
		return count;
	}

	struct pci_dev *dev = pci_get_slot(bus, PCI_DEVFN(devno, fnno));
	if (dev == NULL) {
		printk("can't find pci device!\n");
		return count;
	}	
	

	printk("-------------------------------------------------\n");	
	unsigned int val;
	int i;
	for (i=0; i<256; i=i+4) {
		pci_read_config_dword(dev, i, &val);
		printk("%0.8X     ", val);
		if ((i/4+1) % 4  == 0)printk("\n");
	}
	printk("-------------------------------------------------\n");	

	return count;
}

static struct kobj_attribute pci_addr_attr =
	__ATTR(pci_addr_attr, 0666, pci_addr_attr_show, pci_addr_attr_store);


static struct attribute * g[] = {
    &pci_addr_attr,	
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = g,
};


static int read_pci_init(void) {
    printk(KERN_EMERG "read_pci_init()\n"); 

    read_pci_kobj = kobject_create_and_add("read_pci", NULL);
	if (!read_pci_kobj)
		return -ENOMEM;
	return sysfs_create_group(read_pci_kobj, &attr_group);
}

static void read_pci_exit(void) {
	kobject_del(read_pci_kobj);
    printk(KERN_EMERG "read_pci_exit()\n");
}

module_init(read_pci_init);
module_exit(read_pci_exit);
