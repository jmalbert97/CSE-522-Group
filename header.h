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
#include <linux/kthread.h>
#define NUM_CORES 4

struct Task{
    int period;
    int task_num; 
    int num_sub_tasks; 
    int sub_task_index;
    int exec_time; 
}

struct Sub_task{
    int execution_time; 
    int sub_task_num; 
    int parent_index; 

    //4. added params 
    struct hrtimer timer;
    struct task_struct; 
    ktime_t last_release_time; 
    int loop_iterations_count; 
    int cumulative_exec_time;
    double utilization; 
    int core; 
}

//size chosen arbitrarily until else is specified
struct Task tasks[2] = {{1000, 0, 2, 0}, {1000, 1, 2, 2}}; 
struct Sub_task sub_tasks[4] = {{100, 0, 0}, {100, 1, 0}, {100, 0, 1}, {100, 1, 1}} ; 

//4c. initialize sub tasks loop count to ~1ms of execution time (9804 iterations)
for(int i = 0; i < sizeof(sub_tasks)/sizeof(sub_tasks[0]); i++){
    sub_tasks[i].loop_iterations_count = 9804; 
}

//4(e,d). calculate sub-tasks cumulative execution time & tasks execution time
for(int i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++){
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
for(int i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++){
    int sub_index = tasks[i].sub_task_index; 
    for(int j = 0; j < tasks[i].num_sub_tasks; j++){
        sub_tasks[sub_index + j].utilization = sub_tasks[sub_index + j].execution_time / tasks[i].period; 
    }
}

//4.g determine core for each subtask 
//sort array in descending order
for(int i = 0; i < sizeof(tasks)/sizeof(tasks[0]); i++){
    int sub_index = tasks[i].sub_task_index; 
    for(int j = 0; j < tasks[i].num_sub_tasks; j++){
        for(int k = 0; k < NUM_CORES; k++){
            
        }
    }
}