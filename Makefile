# TODO: Change this to the location of your kernel source code
ifeq (ceneblock, $(USER))
	KERNEL_SOURCE=/home/warehouse/ceneblock/compile/linux_source/linux 
else ifeq (jmalbert, $(USER))
	KERNEL_SOURCE=/home/warehouse/jmalbert/compile/linux_source/linux
else
	$(error set your KERNEL_SOURCE here)
endif



EXTRA_CFLAGS += -DMODULE=1 -D__KERNEL__=1 -Wno-unused-variable 

obj-m := lab.o
lab-objs := kernel_memory.o listFunctions.o taskFunctions.o


all: 
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) clean 
	
.PHONY: all clean
