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
//array to keep track of core utilization 
static double core_util_tracker[NUM_CORES]; 

typedef struct Task _task_t;
typedef struct Sub_task _subtask_t;

struct Sub_task{
  int execution_time; 
  int sub_task_num; 
  int parent_index; 

  //4. added params 
  struct hrtimer timer;
  struct task_struct * task; 
  ktime_t last_release_time; 
  int loop_iterations_count; 
  int cumulative_exec_time;
  double utilization; //I know we have an FPU in the pi, but apparently it's expensive to use 
  double relative_deadline; 
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

_task_t initTask(unsigned long period_ms, unsigned int task_num, unsigned int num_subtasks)
{

  _task_t newTask = { .period_ms = period_ms, .task_num = task_num, .num_sub_tasks = num_subtasks, .firstRun = 1};

  newTask.subtasks = initSubtask(exec_time_ms,0,task_num,1);

  return newTask;
}

//size chosen arbitrarily until else is specified

//CEN: we should probably use a list_head instead of a static array...it's
//standard and allows for dynamic insertation\removal. <linux/list.h>
_task_t tasks[2] = {{.period_ms = 1000, .task_num = 0, .num_sub_tasks = 2}, {.period_ms = 1000, .task_num = 1, .num_sub_tasks = 2}}; 

//initialize core utilization tracker
for(unsigned int i = 0; i < NUM_CORES; i++){
    core_util_tracker[i] = 0; 
}

//4. Adding more parameters 
for(unsigned int i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++){
    struct  list_head *pos;
    tasks[i].exec_time_ms = 0;
    
    //4g. sort subtasks by utilization in descending order
    //JA: *** IF WE CAN GET LIST SORTED HERE, CORE STUFF WILL WORK BELOW. 

    list_for_each(pos, &(tasks[i].subtasks.sibling)){
        _subtask_t* p = list_entry(pos, _subtask_t, sibling); 
        //4b. intitialize release time 
        p->last_release_time = 0; 
        //4c. initialize sub tasks loop count to ~1ms of execution time (9804 iterations)
        p->loop_iterations_count = 9804; 
        //4e. calculate task execution time
        tasks[i].exec_time_ms += p->execution_time; 
        //4d. calculate subtasks cumulative exec time 
        p->cumulative_exec_time = tasks[i].exec_time_ms; 
        //4f. caculate utilization 
        p->utilization = (p->execution_time / tasks[i].period_ms) * 100; 
        //4h. calculate relative priority
        p->relative_deadline = (tasks[i].period_ms * p->cumulative_exec_time) / tasks[i].exec_time_ms; 

    }
}

for(unsigned int i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++){
    struct  list_head *pos;
    //JA: *** SORT ALL SUBTASKS IN DESCENDING UTILIZATION HERE *** 
    list_for_each(pos, &(tasks[i].subtasks.sibling)){
        _subtask_t* p = list_entry(pos, _subtask_t, sibling); 
        //4g. determine core      
        //JA: *** LIST NEEDS TO BE SORTED FIRST, THEN THIS WILL WORK HOW HE WANTS ...    
        for(unsigned int j = 0; j < NUM_CORES; j++){
            if(core_util_tracker[j]+p->utilization < 100){
                p->core = j; 
                core_util_tracker[j] += subtask_utilization; 
                break; 
            }
        }
        //4i. Need core to be assigned first 

    }
}


