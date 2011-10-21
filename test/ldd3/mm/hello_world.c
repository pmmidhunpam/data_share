#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>

#include <linux/spinlock.h>

#include <asm/pgtable.h>
MODULE_LICENSE("GPL");

int index = 1;
static void binary(unsigned long x, int id) 
{ 
  char buf[33];
  buf[32] = '\0';
  
  int i;
  int j=0;
  for (i=31; i>=0; i--) {
    if (x>>i & 1) buf[j] = '1';
    else buf[j] = '0';
    j++;
  }
  printk(KERN_EMERG "index %d , %0x = %s\n", id, x, buf);
}


static int hello_init(void) {
  printk(KERN_EMERG "hello, world\n");

  printk(KERN_EMERG "current pid = %d\n", current->pid);
  printk(KERN_EMERG "current->mm->pgd = %p\n", current->mm->pgd);
  printk(KERN_EMERG "current->active_mm->pgd = %p\n", current->active_mm->pgd);
  pgd_t *pgd_pa = NULL;
  asm("movl %%cr3, %0\n":"=r"(pgd_pa));
  printk(KERN_EMERG "pgd pa from cr3 = %p\n", pgd_pa);

/*
  pgd_t *pgd = current->mm->pgd;
  int i = 0;
  for (i=0; i<1024; i++) {
    binary(pgd[i].pgd, index);
    binary(((unsigned long)pgd[i].pgd >> 22 & 0x1f) << 12, index);
    binary(__va(((unsigned long)pgd[i].pgd >> 22 & 0x1f) << 12), index);
    index++;
  }

  index = 1;
 // printk(KERN_EMERG "900 desc = %p\n", pmd_page(*(pmd_t*)(&pgd[900])));

  pte_t *pte = __va(((unsigned long)pgd[768].pgd >> 22 & 0x1f) << 12);
  for (i=0; i<1024; i++) {
    binary(pte[i].pte, index);
    index++;
  }

*/
  printk(KERN_EMERG "mem_map = %p\n", mem_map);
  struct page * p = alloc_pages(GFP_KERNEL, 8);
  printk(KERN_EMERG "p = %p, index in mem_map = %d\n", p, p-mem_map);

  return 0;
}

static void hello_exit(void) {
  printk(KERN_EMERG "Goodbya, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
