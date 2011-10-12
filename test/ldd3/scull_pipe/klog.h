#ifndef _KLOG_H_
#define _KLOG_H_

#include <linux/module.h>

#define __MODULE_NAME__ "scull_pipe"

#define __DEBUG_ON__

extern const char *__get_filename(const char *);

#ifdef __DEBUG_ON__
//打印日志
#define DEBUG_LOG(fmt, ...) printk(__MODULE_NAME__": %s(%s:%d) " fmt "\n", __FUNCTION__,  __get_filename(__FILE__), __LINE__, ##__VA_ARGS__)
#define DEBUG_LOG_NULL printk(__MODULE_NAME__": %s(%s:%d)\n", __FUNCTION__,  __get_filename(__FILE__), __LINE__) 
#define DEBUG_LOG_VALUE(x, fmt) printk(__MODULE_NAME__": %s(%s:%d) " #x " = " #fmt "\n", __FUNCTION__,  __get_filename(__FILE__), __LINE__, x)

#else

#define DEBUG_LOG(fmt, ...) 
#define DEBUG_LOG_NULL
#define DEBUG_LOG_VALUE(x, fmt) 

#endif /* __DEBUG_ON__ */


#endif /* _KLOG_H_ */
