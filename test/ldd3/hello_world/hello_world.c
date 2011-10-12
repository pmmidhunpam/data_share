#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>

MODULE_LICENSE("GPL");


void my_printk(const char *fmt, ...) {

	static unsigned int cnt = 0;
	if (cnt++ % 10 == 0) {
		
	}
}
static int hello_init(void) {

  return 0;
}

static void hello_exit(void) {
  printk(KERN_EMERG "Goodbya, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
