# TODO: Change this to the location of your kernel source code
KERNEL_SOURCE=/home/warehouse/ceneblock/compile/linux_source/linux

EXTRA_CFLAGS += -DMODULE=1 -D__KERNEL__=1

obj-m := lab.o
lab-objs := kernel_memory.o listFunctions.o 

PHONY: all

all:
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) clean 
