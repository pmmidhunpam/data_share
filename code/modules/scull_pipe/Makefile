obj-m := scull_pipe.o
scull_pipe-objs := scull_pipe_main.o scull_pipe_fops.o klog.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd) 
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -fr *.o *.ko *.symvers *.mod.c


