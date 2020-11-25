//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

#include <linux/sched.h>
#include <linux/kthread.h>
#include "taskFunctions.h"
#include <linux/kernel.h>
#include <uapi/linux/sched/types.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>

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

int run_thread_func(void *data){ 
  //printk("inside run_thread_func for run mode..\n"); 
  
  _subtask_t *subtask_temp = (_subtask_t *)data; 
  _subtask_t *nextSubtask = NULL;
  unsigned int subtaskSiblingIndex = 0;
  int64_t absolute_time;
  static ktime_t timer_interval;
  unsigned int x; 
  printk("initializing timer -> subtask loop it count: (%u)\n", subtask_temp->loop_iterations_count); 
  hrtimer_init(&(subtask_temp->timer), CLOCK_MONOTONIC, HRTIMER_MODE_ABS); 
  printk("timer init done.\n"); 
  subtask_temp->timer.function = timer_expiration_func; 
  printk("timer function set\n"); 

  while(!kthread_should_stop()){
    printk("inside run_thread_func loop for run mode..\n"); 
    set_current_state(TASK_INTERRUPTIBLE);
    printk("state set\n"); 
    schedule(); 


    for(x = 0; x < NUM_CORES; x++){
      list_for_each_entry(nextSubtask, &(get_subtasks_from_tasks(x)->sibling), sibling ){
        // if((nextSubtask->sub_task_num == subtask_temp->sub_task_num + 1) && (nextSubtask->parent_index == subtask_temp->parent_index))
        // {
        //   break;
        // } 
      }
    }
    // list_for_each_entry(nextSubtask, &taskStruct[subtask_temp->parent_index]->subtasks->sibling, sibling)
    // {
    //   printk("LOOP ITS COUNT IN LIST LOOP: (%u)\n",nextSubtask->loop_iterations_count);
    //   // if(subtaskSiblingIndex == subtask_temp->sub_task_num + 1)
    //   // {
    //   //   break;
    //   // } 
    // }

    // subtask_temp->last_release_time = ktime_get(); 
    // subtask_func(subtask_temp); 
    // //check if subtask is first of its task 
    // if(subtask_temp->sub_task_num == 0){
    //   absolute_time = ktime_to_ms(subtask_temp->last_release_time) + subtask_temp-> task_period; 
    //   //creat ktime interval for timer, 1000000 ns = 1ms 
    //   timer_interval = ktime_set(0, absolute_time * 1000000);
    //   hrtimer_start(&subtask_temp->timer, timer_interval, HRTIMER_MODE_ABS);
    // }else if(taskStruct[subtask_temp->parent_index]->num_subtasks != subtask_temp->sub_task_num){
    //   ktime_t current_time = ktime_get();

    //   if(ktime_to_ms(current_time) < (subtask_temp->task_period + ktime_to_ms(nextSubtask->last_release_time)))
    //   {
    //     timer_interval = ktime_set(0, (nextSubtask->task_period * 1000000) + ktime_to_ns(nextSubtask->last_release_time));
    //     hrtimer_start(&nextSubtask->timer, timer_interval, HRTIMER_MODE_ABS);
    //   }
    //   else
    //   {
    //     wake_up_process(nextSubtask->task);
    //   }
    //}
      printk("inside run_thread_func loop for run mode..\n"); 
  }

  return 0; 


} 

