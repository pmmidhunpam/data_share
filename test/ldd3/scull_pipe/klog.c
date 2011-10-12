#include "klog.h"

#include <linux/module.h>

//打印日志
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

