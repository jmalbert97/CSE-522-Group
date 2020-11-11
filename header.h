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

#define NUM_CORES num_online_cpus()

typedef struct Task _task_t;
typedef struct Sub_task _subtask_t;

struct Sub_task{
  int execution_time; 
  int sub_task_num; 
  int parent_index; 

  //4. added params 
  struct hrtimer timer;
  struct task_struct task; 
  ktime_t last_release_time; 
  int loop_iterations_count; 
  int cumulative_exec_time;
  double utilization; //I know we have an FPU in the pi, but apparently it's expensive to use 
  unsigned int core; //cpumask.h

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

static _task_t *taskStruct;

/*

//4c. initialize sub tasks loop count to ~1ms of execution time (9804 iterations)
for(unsigned int i = 0; i < sizeof(sub_tasks)/sizeof(sub_tasks[0]); i++){
    sub_tasks[i].loop_iterations_count = 9804; 
}

//4(e,d). calculate sub-tasks cumulative execution time & tasks execution time
for(unsigned int i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++){
    tasks[i].exec_time = 0;
    int sub_index = tasks[i].sub_task_index;  
    for(int j = 0; j < tasks[i].num_sub_tasks; j++){
        tasks[i].exec_time += sub_tasks[sub_index + j].execution_time; 
        if(j == sub_tasks[sub_index + j].sub_task_num){
            sub_tasks[sub_index + j].cumulative_exec_time = tasks[i].exec_time; 
        }
    }
}

//4.f calculate utilization 
for(unsigned int i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++){
    int sub_index = tasks[i].sub_task_index; 
    for(unsigned int j = 0; j < tasks[i].num_sub_tasks; j++){
        sub_tasks[sub_index + j].utilization = sub_tasks[sub_index + j].execution_time / tasks[i].period; 
    }
}

//4.g determine core for each subtask 
//sort array in descending order
for(unsigned int i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++){
    int sub_index = tasks[i].sub_task_index; 
    for(unsigned int j = 0; j < tasks[i].num_sub_tasks; j++){
        for(unsigned int k = 0; k < NUM_CORES; k++){
            
        }
    }
}
*/


#endif
