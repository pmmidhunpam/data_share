obj-m := scull.o
scull-objs := scull_main.o scull_fops.o klog.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd) 
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -fr *.o *.ko *.symvers *.mod.c


