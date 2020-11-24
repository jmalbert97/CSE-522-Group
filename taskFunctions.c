//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

#include <linux/sched.h>
#include <linux/kthread.h>
#include "taskFunctions.h"

int subtask_thread_func(void * data){
  int num_loops = (int) data; 
  unsigned int i; 
  printk("inside the subtask thread.."); 
  for(i = 0; i < num_loops; i++){
    ktime_get(); 
  }
  return 0; 
}

void subtask_func(_subtask_t * subtask_temp){
    unsigned int i; 
    for(i = 0; i < subtask_temp->loop_iterations_count; i++){
        ktime_get(); 
    }
}


void get_itterations(_subtask_t * subtaskTemp){
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
    subtask_func(subtaskTemp); 
    endTime = ktime_get(); 
    totalTime = ktime_to_ms(ktime_sub(endTime, startTime));

    if(totalTime < subtaskTemp->execution_time)
    {
      //double until time is exceeded 
      if(!time_exceeded)
      {
        subtaskTemp->loop_iterations_count = subtaskTemp->loop_iterations_count * 2; 
        increase = subtaskTemp->loop_iterations_count;
      }else{
        return; 
      }
    }else{
      //when time exceeded, subtract by half the last increase
      time_exceeded = 1; 
      subtaskTemp->loop_iterations_count = subtaskTemp->loop_iterations_count - (increase / 2); 
      increase = increase / 2; 
    }
  }
}

int calibrate_thread(void *threadData)
{
  unsigned int i = 0;
  _subtask_t **coreSubtasks = (_subtask_t **)threadData;
  printk("Calibrating thread ..\n"); 
  
  while(coreSubtasks[i] != NULL){
    get_itterations(coreSubtasks[i]);
    coreSubtasks[i]->task = kthread_create(subtask_thread_func, (void *) coreSubtasks[i]->loop_iterations_count, "subtask_thread_func");
    sched_setscheduler(coreSubtasks[i]->task, SCHED_FIFO, &coreSubtasks[i]->priority);
    printk("Task: %u Subtask: %u Itterations needed: %u to meet execution time of: %lu on core: %u \n", coreSubtasks[i]->parent_index, coreSubtasks[i]->sub_task_num, coreSubtasks[i]->loop_iterations_count, coreSubtasks[i]->execution_time, coreSubtasks[i]->core);
    i++;
  }
  return 0;
}


_subtask_t * subtask_lookup_function(struct hrtimer * timer){
  unsigned int x; 
  _subtask_t *tempSubtask;
  //go through each core and check timer address, see if match with provided address
  for(x = 0; x < NUM_CORES; x++){
    list_for_each_entry(tempSubtask, &taskStruct[x]->subtasks->sibling, sibling ){
      //if subtask timer address found, return pointer to that subtask 
      if(&tempSubtask->timer == timer){
        break;
      }
    }
  }
  return tempSubtask; 
}

enum hrtimer_restart timer_expiration_func(struct hrtimer *timer){
  _subtask_t * subtask_temp; 
  subtask_temp = subtask_lookup_function(timer); 
  wake_up_process(subtask_temp->task); 
  return HRTIMER_RESTART; 
}


