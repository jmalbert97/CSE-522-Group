# TODO: Change this to the location of your kernel source code
KERNEL_SOURCE=/home/warehouse/ceneblock/compile/linux_source/linux

EXTRA_CFLAGS += -DMODULE=1 -D__KERNEL__=1 -Werror

kernel_memory-objs := $(kernel_memory-y)
obj-m := kernel_memory.o

PHONY: all

all:
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) clean 
