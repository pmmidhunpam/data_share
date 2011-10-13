#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>

#include <linux/spinlock.h>

MODULE_LICENSE("GPL");

static __always_inline void my_lock(raw_spinlock_t *lock)
{
        printk("my lock = %0x", *(unsigned int *)lock);
	short inc = 0x0100;

	asm volatile (
		LOCK_PREFIX "xaddw %w0, %1\n"
		"1:\t"
		"cmpb %h0, %b0\n\t"
		"je 2f\n\t"
		"rep ; nop\n\t"
		"movb %1, %b0\n\t"
		/* don't need lfence here, because loads are in-order */
		"jmp 1b\n"
		"2:"
		: "+Q" (inc), "+m" (lock->slock)
		:
		: "memory", "cc");
}

static int hello_init(void) {
  printk(KERN_EMERG "hello, world\n");
  spinlock_t lock;
  spin_lock_init(&lock);
  
  printk(KERN_EMERG "try\n");
  //spin_lock(&lock);
  my_lock(&(&lock)->raw_lock);
  printk(KERN_EMERG "ok\n");
  return 0;
}

static void hello_exit(void) {
  printk(KERN_EMERG "Goodbya, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
