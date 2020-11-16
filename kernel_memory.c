  
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
#include <asm/uaccess.h>

//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

#include "listFunctions.h"

typedef enum {
  CALIBRATE = 0,
  RUN = 1
} _mode_t;

_mode_t mode = CALIBRATE;

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
    unsigned int i; 
    unsigned int j; 

    //2. check param for "run" value => set global mode var
    if(strcmp(mode_temp, "run") == 0){
        mode = RUN;
    }
    printk(KERN_INFO "Loaded kernel_memory module in (%s) mode\n", (mode == RUN) ? "run" : "calibrate");

    //initialize tasks
    for(i = 0; i < NUM_TASKS; i++){
        taskStruct[i] = kmalloc(sizeof(_task_t), GFP_KERNEL);
        taskStruct[i] = initTask((i+1)*100,i,NUM_SUBTASKS);
        //initialize subtasks
        for(j = 0; j < NUM_SUBTASKS; j++){
            initSubtask(taskStruct[i], (j+1)*10, j, 0, 0); 
        }
        setParams(taskStruct[i]);
        printk(KERN_INFO "Tasks Stuff: exec time: (%lu)\n", taskStruct[i]->exec_time_ms);
    }

    determineCore(taskStruct); 

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
    unsigned int i; 
    kthread_stop(kthread);
    //delTask(*taskStruct);
    for(i = 0; i < NUM_TASKS; i++){
        kfree(taskStruct[i]);
    }
    printk(KERN_INFO "Unloaded kernel_memory module\n");

}

module_init(kernel_memory_init);
module_exit(kernel_memory_exit);

MODULE_LICENSE ("GPL");
