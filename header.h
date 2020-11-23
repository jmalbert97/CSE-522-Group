/*
.................
This header file was designed by Joe Albert & Eric Neblock. 
This header file was generated ....
This header file task set use case is intented to test .... 
Nuances: 
.................
*/

#ifndef _HEADER_H
#define _HEADER_H

#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/cpumask.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>


#define NUM_CORES num_online_cpus()
#define NUM_TASKS 2
#define NUM_SUBTASKS 4

typedef struct Task _task_t;
typedef struct Sub_task _subtask_t;

struct Sub_task{
  unsigned long execution_time; 
  unsigned long relative_deadline; 
  unsigned int sub_task_num; 
  unsigned int parent_index; 

  //4. added params 
  struct hrtimer timer;
  struct task_struct * task; 
  ktime_t last_release_time; 
  unsigned int loop_iterations_count; 
  unsigned int cumulative_exec_time;
  unsigned int utilization; //I know we have an FPU in the pi, but apparently it's expensive to use 
  unsigned int core; //cpumask.h
 
  unsigned int inUse; //set to 1 if this is a valid task.
  struct sched_param priority;

  struct list_head sibling;
};

struct Task{
  //CEN: should probably use a uint32 or something instead of long...
  unsigned long period_ms;
  unsigned int task_num; 
  unsigned int num_subtasks;  //hmmm...may not need. TODO: check API to see if there is a fast way to get the number of subtasks
  unsigned long exec_time_ms; 

  uint8_t firstRun;
  _subtask_t *subtasks;
  
  struct list_head sibling;
};

static _task_t *taskStruct[NUM_TASKS];
//static _subtask_t *coreArraySubtasks[4][NUM_TASKS*NUM_SUBTASKS];
//static _subtask_t ***coreArraySubtasks;

 #endif
