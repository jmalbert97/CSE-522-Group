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
#include <linux/delay.h>

//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

#include "listFunctions.h"
#include "taskFunctions.h"

typedef enum {
  CALIBRATE = 0,
  RUN = 1
} _mode_t;

_mode_t mode = CALIBRATE;

char * mode_temp = " "; 
module_param(mode_temp, charp, 0644); 
static _subtask_t *coreArraySubtasks[4][NUM_TASKS*NUM_SUBTASKS];
struct task_struct *subTasks[NUM_TASKS * NUM_SUBTASKS];
static struct task_struct * kthread = NULL;

void calibrateThreads(void)
{
  unsigned int x = 0;
  struct sched_param p;
  struct task_struct **calibrateThreads;
  calibrateThreads = kmalloc(sizeof(struct task_struct) * NUM_CORES, GFP_KERNEL);
  p.sched_priority = 99; 
  printk("in the calibrate func.. \n"); 

  for(x = 0; x < NUM_CORES; x++)
  {
    printk("Core (%u) task struct created.. \n", x); 
    //calibrateThreads[x] = kthread_create(thread_fn1, NULL, "test");
    calibrateThreads[x] = kthread_create(calibrate_thread, coreArraySubtasks[x], "calibrate_task");
    kthread_bind(calibrateThreads[x], x);
    sched_setscheduler(calibrateThreads[x], SCHED_FIFO, &p);
  }
  msleep(100); 
  for(x = 0; x < NUM_CORES; x++)
  {
    printk("Waking up thread (%u) ... \n", x); 
    wake_up_process(calibrateThreads[x]);
  }
  //kfree(calibrateThreads);
}

void setupThreads(void)
{
  _subtask_t *tempSubtask = NULL;
  int x = 0, y = 0;
   
  for(x = 0; x < NUM_TASKS; x++)
  {
    y = 0;
    list_for_each_entry(tempSubtask, &taskStruct[x]->subtasks->sibling, sibling )
    {
      subTasks[y + x * NUM_SUBTASKS] = kthread_create(run_thread_func, tempSubtask, "run_task");

      kthread_bind(subTasks[y + x * NUM_SUBTASKS], tempSubtask->core);
      sched_setscheduler(subTasks[y + x * NUM_SUBTASKS], SCHED_FIFO, &tempSubtask->priority);
    }
  }
}

void runThreads(void)
{
  _subtask_t *tempSubtask = NULL;
  int x = 0, y = 0;
   
  for(x = 0; x < NUM_TASKS; x++)
  {
    y = 0;
    list_for_each_entry(tempSubtask, &taskStruct[x]->subtasks->sibling, sibling )
    {
      wake_up_process(subTasks[y + x * NUM_SUBTASKS]);
    }
  }
}

static int
thread_fn(void * data)
{
    printk("Hello from thread\n");

    switch(mode)
    {
      case RUN:
        printk("In run mode...\n");
        setupThreads();
        printk("Going to run threads...\n");
        runThreads();
        break;
      case CALIBRATE:
        printk("calibrating the threads ...\n"); 
        calibrateThreads();
        break;
      default:
        break;
    }

    while (!kthread_should_stop()) {
        schedule();
    }

    return 0;
}

static int
kernel_memory_init(void)
{
    unsigned int i = 0; 
    unsigned int j = 0; 

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
            initSubtask(taskStruct[i], (j+1)*10, j, i, 0); 
        }
        setParams(taskStruct[i]);
        printk(KERN_INFO "Tasks Stuff: exec time: (%lu)\n", taskStruct[i]->exec_time_ms);
    }

    determineCore(taskStruct, coreArraySubtasks); 
    //THIS CAUSES A SEG FAULT BUT I HAVE NO CLUE WHY... 
    printk("first element for core.. loop its (%u)\n", coreArraySubtasks[0][0]->loop_iterations_count); 

    kthread = kthread_create(thread_fn, NULL, "k_memory");
    if (IS_ERR(kthread)) {
        printk(KERN_ERR "Failed to create kernel thread\n");
        return PTR_ERR(kthread);
    }
    printk("Waking up init thread now .. \n");
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
    for(i = 0; i < NUM_CORES; i++)
    {
      //kfree(coreArraySubtasks[i]);
    }
    //kfree(coreArraySubtasks);
    
    printk(KERN_INFO "Unloaded kernel_memory module\n");
    
}

module_init(kernel_memory_init);
module_exit(kernel_memory_exit);

MODULE_LICENSE ("GPL");
