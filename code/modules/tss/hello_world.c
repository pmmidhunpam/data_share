#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>

#include <linux/spinlock.h>

MODULE_LICENSE("GPL");

static int hello_init(void) {
  printk(KERN_EMERG "hello, world\n");
  
  unsigned int tr = 0;
  asm volatile("mov %%tr, %0"
			: "=r"(tr)
			:
			: "memory");
  
  printk(KERN_EMERG "tr = %0x\n", tr);
 
  return 0;
}

static void hello_exit(void) {
  printk(KERN_EMERG "Goodbya, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
