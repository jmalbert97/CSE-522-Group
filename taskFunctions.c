//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

#include <linux/sched.h>
#include <linux/kthread.h>
#include "taskFunctions.h"


void subtask_func(_subtask_t * subtask_temp){
    unsigned int i; 
    for(i = 0; i < subtask_temp->loop_iterations_count; i++){
        ktime_get(); 
    }
}


void get_itterations(_subtask_t subtaskTemp){
  unsigned int steady_state = 0; 
  unsigned int increase = 1; 
  unsigned int time_exceeded = 0; 
  ktime_t startTime;
  ktime_t endTime;
  int64_t totalTime;

  //run subtask func until steady state 
  while (!steady_state)
  {
    startTime = ktime_get(); 
    subtask_func(&subtaskTemp); 
    endTime = ktime_get(); 
    totalTime = ktime_to_ms(ktime_sub(endTime, startTime));

    if(totalTime < subtaskTemp.execution_time)
    {
      //double until time is exceeded 
      if(!time_exceeded)
      {
        subtaskTemp.loop_iterations_count = subtaskTemp.loop_iterations_count * 2; 
        increase = subtaskTemp.loop_iterations_count;
      }else{
        return; 
      }
    }else{
      //when time exceeded, subtract by half the last increase
      time_exceeded = 1; 
      subtaskTemp.loop_iterations_count = subtaskTemp.loop_iterations_count - (increase / 2); 
      increase = increase / 2; 
    }
  }
  

}

int calibrate_thread(void *threadData)
{
  // unsigned int x = 0;
  _subtask_t *coreSubtasks = (_subtask_t *)threadData;
  printk("Calibrating thread ..\n"); 
  printk("loop its (%u)\n", coreSubtasks[0].loop_iterations_count); 

  //set_current_state(TASK_INTERRUPTIBLE);  
  //schedule();
  
  // printk("state is set ..\n"); 
  // printk("task is scheduled ..\n"); 

  // while(coreSubtasks[x].inUse == 1)
  // {
  //   //Set each subtasks's priority
  //   sched_setscheduler(&(coreSubtasks[x].task), SCHED_FIFO, &(coreSubtasks[x].priority));

  //   get_itterations(coreSubtasks[x]);

  //   printk("Task: %u Subtask: %u Itterations needed: %u to meet execution time of: %lu \n", coreSubtasks[x].parent_index, coreSubtasks[x].sub_task_num, coreSubtasks[x].loop_iterations_count, coreSubtasks[x].execution_time);

  //   x++;
  // }

  // printk("Calibration finished .. \n"); 

  return 0;
}


