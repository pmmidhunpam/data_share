#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>

#include <linux/kobject.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");


#define kobj_attr(_name) \
static struct kobj_attribute _name##_attr = {	\
	.attr	= {				\
		.name = __stringify(_name),	\
		.mode = 0644,			\
	},					\
	.show	= _name##_show,			\
	.store	= _name##_store,		\
}


static ssize_t byte_read_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	
	return 0;
}

static ssize_t byte_read_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	return n;
}

kobj_attr(byte_read);
//kobj_attr(byte_write);

static struct attribute * g[] = {
	&byte_read_attr.attr,
//	&byte_write_attr.attr,
	NULL
};

static struct attribute_group attr_group = {
	.attrs = g
};

static int port_rw_init(void) {
  printk("Hello, port_rw\n");

  struct kobject *port_rw_kobj = kobject_create_and_add("port_rw", NULL);
  sysfs_create_group(port_rw_kobj, &attr_group);
  return 0;
}

static void port_rw_exit(void) {
  printk("Goodbya, port_rw\n");
}

module_init(port_rw_init);
module_exit(port_rw_exit);
