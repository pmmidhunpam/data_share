#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
/*
const char *__get_filename(const char *file) {
const char *p = file;
if (p[0] != '/')return p;
int len = strlen(p);
int i = 0;
for (i=len-1; i--; i>=0) {
  if (p[i] == '/') {
    p = p+i+1;
    break;
  }
}
return p;
}
*/
/*
#define MYLOG_S printk(KERN_WARNING "%s(%s:%d)\n", __FUNCTION__, __get_filename(__FILE__), __LINE__)
#define MYLOG_F(fmt, ...) printk(KERN_CRIT "%s(%s:%d) " fmt " \n", __FUNCTION__, __get_filename(__FILE__), __LINE__, ##__VA_ARGS__)
#define MYLOG_V(x, fmt) printk(KERN_ERR "%s(%s:%d) " #x " = " #fmt "\n", __FUNCTION__,  __get_filename(__FILE__), __LINE__, x)
 
*/
static int hello_init(void) {
  MYLOG_S;
  MYLOG_F("wo shi %s", "linhanjie");
  int a = 100;
  MYLOG_V(a, %d);
  return 0;
}

static void hello_exit(void) {
  printk(KERN_EMERG "Goodbya, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
