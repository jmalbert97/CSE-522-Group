  
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include "header.h"
#include <asm/uaccess.h>

typedef enum {
  CALIBRATE = 0,
  RUN = 1
} mode_t;

mode_t mode = CALIBRATE;

char * mode_temp = " "; 
module_param(mode_temp, charp, 0644); 

static struct task_struct * kthread = NULL;

static int
thread_fn(void * data)
{
    printk("Hello from thread\n");

    while (!kthread_should_stop()) {
        schedule();
    }

    return 0;
}

static int
kernel_memory_init(void)
{
    //2. check param for "run" value => set global mode var
    if(strcmp(mode_temp, "run") == 0){
        mode = RUN;
    }
    printk(KERN_INFO "Loaded kernel_memory module in (%s) mode\n", (mode == RUN) ? "run" : "calibrate");

    kthread = kthread_create(thread_fn, NULL, "k_memory");
    if (IS_ERR(kthread)) {
        printk(KERN_ERR "Failed to create kernel thread\n");
        return PTR_ERR(kthread);
    }
    
    wake_up_process(kthread);

    return 0;
}

static void 
kernel_memory_exit(void)
{
    kthread_stop(kthread);
    printk(KERN_INFO "Unloaded kernel_memory module\n");
}

module_init(kernel_memory_init);
module_exit(kernel_memory_exit);

MODULE_LICENSE ("GPL");
