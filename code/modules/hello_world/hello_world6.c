#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/memory.h>
#include <linux/stop_machine.h>
#include <linux/slab.h>

#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <asm/pgtable.h>

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

static void *gp;

static int hello_init(void) {
  printk(KERN_EMERG "hello, world\n");

  char  *p = vmalloc(1024*1024*30);
  gp = p;
  int i = 0; 
  for (i=0; i<100; i++) {
  p += i*4096;
  printk("vmalloc address = %p\n", p);
  struct page *page = vmalloc_to_page(p);
  printk("vmalloc to page = %p, high = %d\n", page, PageHighMem(page));
  
  printk("page address = %p\n", page_address(page));
   }
  return 0;
}

static void hello_exit(void) {
  vunmap(gp);
  printk(KERN_EMERG "Goodbya, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
