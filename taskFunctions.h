#ifndef TASK_FUNCTIONS_H
#define TASK_FUNCTIONS_H

//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

#include <linux/sched.h>
#include <linux/kthread.h>

int calibrate_thread(void *threadData);

int64_t subtask_func_calibrate(_subtask_t *subtaskTemp);

void subtask_func(_subtask_t * subtask_temp);

void get_itterations(_subtask_t * subtaskTemp);

int thread_fn1(void * data); 

int subtask_thread_func(void * data); 

_subtask_t * subtask_lookup_function(struct hrtimer * timer);

enum hrtimer_restart timer_expiration_func(struct hrtimer *timer);

static int run_thread_func(void *data){
 
  _subtask_t *subtask_temp = (_subtask_t *)data; 
  _subtask_t *nextSubtask = NULL;
  unsigned int subtaskSiblingIndex = 0;

  
  int64_t absolute_time;
  static ktime_t timer_interval;
  hrtimer_init(&subtask_temp->timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS); 
  subtask_temp->timer.function = timer_expiration_func; 
  set_current_state(TASK_INTERRUPTIBLE);
  schedule(); 

  list_for_each_entry(nextSubtask, &taskStruct[subtask_temp->parent_index]->subtasks->sibling, sibling)
  {
    if(subtaskSiblingIndex == subtask_temp->sub_task_num + 1)
    {
      break;
    }
  }


  while(!kthread_should_stop()){
    subtask_temp->last_release_time = ktime_get(); 
    subtask_func(subtask_temp); 
    //check if subtask is first of its task 
    if(subtask_temp->sub_task_num == 0){
      absolute_time = ktime_to_ms(subtask_temp->last_release_time) + subtask_temp-> task_period; 
      //creat ktime interval for timer, 1000000 ns = 1ms 
      timer_interval = ktime_set(0, absolute_time * 1000000);
      hrtimer_start(&subtask_temp->timer, timer_interval, HRTIMER_MODE_ABS);
    }else if(taskStruct[subtask_temp->parent_index]->num_subtasks != subtask_temp->sub_task_num){
      ktime_t current_time = ktime_get();

      if(ktime_to_ms(current_time) < (subtask_temp->task_period + ktime_to_ms(nextSubtask->last_release_time)))
      {
        timer_interval = ktime_set(0, (nextSubtask->task_period * 1000000) + ktime_to_ns(nextSubtask->last_release_time));
        hrtimer_start(&nextSubtask->timer, timer_interval, HRTIMER_MODE_ABS);
      }
      else
      {
        wake_up_process(nextSubtask->task);
      }
    }
  }

  return 0; 


} 

#endif
