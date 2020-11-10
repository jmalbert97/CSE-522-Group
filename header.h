/*
.................
This header file was designed by Joe Albert & Eric Neblock. 
This header file was generated ....
This header file task set use case is intented to test .... 
Nuances: 
.................
*/

#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/cpumask.h>

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
  unsigned int num_sub_tasks;  //hmmm...may not need. TODO: check API to see if there is a fast way to get the number of subtasks
  unsigned long exec_time_ms; 

  uint8_t firstRun;
  _subtask_t subtasks;
};


_subtask_t initSubtask(int execution_time, int sub_task_num, int parent_index, int firstRun)
{
  _subtask_t newSubtask;

  if(firstRun)
  {
    INIT_LIST_HEAD(&newSubtask.sibling);
  }
  //if we have already created the link list, then we'll add it in initTask
  
  newSubtask.execution_time = execution_time;
  newSubtask.sub_task_num = sub_task_num;
  newSubtask.parent_index = parent_index;

  hrtimer_init(&newSubtask.timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

  //The other variables should be set at time of running
  
  return newSubtask;
}

_task_t initTask(unsigned long period_ms, unsigned int task_num, unsigned int num_subtasks, unsigned long exec_time_ms)
{

  _task_t newTask = { .period_ms = period_ms, .task_num = task_num, .num_sub_tasks = num_subtasks, .exec_time_ms = exec_time_ms, .firstRun = 1};

  newTask.subtasks = initSubtask(exec_time_ms,0,task_num,1);

  return newTask;
}

//size chosen arbitrarily until else is specified

//CEN: we should probably use a list_head instead of a static array...it's
//standard and allows for dynamic insertation\removal. <linux/list.h>
_task_t tasks[2] = {{.period_ms = 1000, .task_num = 0, .num_sub_tasks = 2, .exec_time_ms = 0}, {.period_ms = 1000, .task_num = 1, .num_sub_tasks = 2, .exec_time_ms = 2}}; 

//_subtask_t sub_tasks[4] = {{100, 0, 0}, {100, 1, 0}, {100, 0, 1}, {100, 1, 1}} ; 

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
