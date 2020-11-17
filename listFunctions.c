#include "listFunctions.h"
#include <linux/math64.h>
#include <linux/slab.h>

_subtask_t *initSubtask(_task_t *parentTask, int execution_time, int sub_task_num, int parent_index, uint8_t firstRun)
{
  _subtask_t *newSubtask = kmalloc(sizeof(_subtask_t), GFP_KERNEL);

  newSubtask->execution_time = execution_time;
  newSubtask->sub_task_num = sub_task_num;
  newSubtask->parent_index = parent_index;
  newSubtask->last_release_time = 0; 
  newSubtask->loop_iterations_count = 9804; 
  newSubtask->inUse = 1;
  if(firstRun)
  {
    INIT_LIST_HEAD(&newSubtask->sibling);
  }else{
    list_add(&newSubtask->sibling, &parentTask->subtasks->sibling);
  }
  //if we have already created the link list, then we'll add it in initTask

  hrtimer_init(&newSubtask->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

  return newSubtask;  
}

_task_t *initTask(unsigned long period_ms, unsigned int task_num, unsigned int num_subtasks)
{

  _task_t *newTask = kmalloc(sizeof(_task_t), GFP_KERNEL);

  newTask->period_ms = period_ms;
  newTask->task_num = task_num;
  newTask->num_subtasks = num_subtasks;


  newTask->subtasks = initSubtask(newTask, 0,0,task_num,1);

  return newTask;
}

_task_t * newTask(_task_t *taskList, unsigned long period_ms, unsigned int task_num, unsigned int num_subtasks)
{
  _task_t *newTask = kmalloc(sizeof(_task_t), GFP_KERNEL);
  
  newTask = initTask(period_ms,task_num,num_subtasks);

  list_add(&newTask->sibling, &taskList->sibling);

  return newTask; 
}

//size chosen arbitrarily until else is specified

void delSubtask(_subtask_t *parentSubtask)
{
  _subtask_t *tempSubtask;
  list_for_each_entry(tempSubtask, &parentSubtask->sibling, sibling)
  {
    // Remove the component from the list
    list_del(&tempSubtask->sibling);
    kfree(tempSubtask);
  }
}

void delTask(_task_t parentTask)
{

  _task_t *tempTask;
  list_for_each_entry(tempTask, &parentTask.sibling, sibling )
  {
    //Clear out the subtasks
    delSubtask(tempTask->subtasks);

    // Remove the component from the list
    list_del(&tempTask->sibling);
    kfree(tempTask);
  }
}

void setParams(_task_t *taskStruct_temp)
{
  _subtask_t *tempSubtask;
  taskStruct_temp->exec_time_ms = 0;
  //subtasks params 
  list_for_each_entry(tempSubtask, &taskStruct_temp->subtasks->sibling, sibling )
  {
    //4e. calculate exec time 
    taskStruct_temp->exec_time_ms += tempSubtask->execution_time; 
    //4d. calculate cumulative exec time s
    tempSubtask->cumulative_exec_time = taskStruct_temp->exec_time_ms; 
    //4f. calculate utilization 
    tempSubtask->utilization = (tempSubtask->execution_time * 100) / taskStruct_temp->period_ms; 
    //4g. determine core to run on 
  }

}

//helper functions for determineCore
void selectionSortUtil(_subtask_t *arr[])  
{  
    int i, j;   
    _subtask_t *temp;

    // One by one move boundary of unsorted subarray  
    for (i = 0; i < NUM_TASKS*NUM_SUBTASKS-1; i++)  
    {  
        for (j = i+1; j < NUM_TASKS*NUM_SUBTASKS; j++){
          if (arr[i]->utilization < arr[j]->utilization){
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp; 
          }  
        }
    }
 
}  

//helper functions for determineCore
void selectionSortCore(_subtask_t *arr[])  
{  
  int i, j, k;
  _subtask_t *temp;
  int priority_index = 0; 

  // sort ascending by core   
  for (i = 0; i < NUM_TASKS*NUM_SUBTASKS-1; i++)  
  {  
      for (j = i+1; j < NUM_TASKS*NUM_SUBTASKS; j++){
        //sort by ascending core 
        if (arr[i]->core > arr[j]->core){
          temp = arr[i];
          arr[i] = arr[j];
          arr[j] = temp; 
        }  
      }
  }

  //sort descending by relative deadline for each core 
  for (i = 0; i < NUM_TASKS*NUM_SUBTASKS-1; i++)
  {  
    for (j = i+1; j < NUM_TASKS*NUM_SUBTASKS; j++){
      //sort by relatvie deadlines on same core 
      if ((arr[i]->relative_deadline < arr[j]->relative_deadline) && (arr[i]->core == arr[j]->core)){
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp; 
      } 
    }
  }

  //if both subtasks on same core, give first (higher rel deadline) a lower priority 
  for(i = 0; i < NUM_SUBTASKS*NUM_TASKS-1; i++){
    if(arr[i]->core == arr[i+1]->core){
      arr[i]->priority = priority_index; 
      priority_index++; 
    }else{
      arr[i]->priority = priority_index; 
      priority_index=0; 
    }
    if(i == NUM_SUBTASKS*NUM_TASKS-2){
      arr[i+1]->priority = priority_index;
    }
  }

  //At this point the tasks should be sorted, so when I see if there a
  //core number > current->core that means time to increment arrays
  j = 0;
  k = 0;
  for(i = 0; i < NUM_SUBTASKS * NUM_TASKS; i++)
  {
 
    arr[i]->inUse = 1;
    if(i == 0)
    {
      coreArraySubtasks[j][k] = arr[i];
    }
    else if(coreArraySubtasks[j][k - 1]->core < arr[i]->core)
    {
      k = 0;
      j++
      coreArraySubtasks[j][k] = arr[i];
    }
    else
    {
      k++;
      coreArraySubtasks[j][k] = arr[i];
    }
  }
}  


void determineCore(_task_t *taskStruct_temp[]){
    unsigned int h; 
    unsigned int i; 
    unsigned int j;
    unsigned int k;  
    unsigned int index = 0; 
    //var to keep track of aggregate core utilization
    unsigned int core_tracker[NUM_CORES]; 
    _subtask_t *tempSubtask;
    _subtask_t *sorted_arr[NUM_TASKS * NUM_SUBTASKS]; 

    for(h = 0; h < NUM_CORES; h++){
      core_tracker[h] = 0; 
    }

    //populate array with all subtasks pointers to be sorted
    for(i = 0; i < NUM_TASKS; i++){
      list_for_each_entry(tempSubtask, &taskStruct_temp[i]->subtasks->sibling, sibling ){
        sorted_arr[index] = tempSubtask; 
        //calculate relative deadline 
        tempSubtask->relative_deadline = (taskStruct_temp[i]->period_ms * tempSubtask->cumulative_exec_time) / taskStruct_temp[i]->exec_time_ms; 
        printk("subtask(%u) has period (%lu) cum exec time (%lu) task exec time (%lu) => relative deadline: (%lu)\n", index,taskStruct_temp[i]->period_ms , tempSubtask->cumulative_exec_time,taskStruct_temp[i]->exec_time_ms, tempSubtask->relative_deadline); 
        index++; 
      }
    }
    //sort array by utilization in decesnding order 
    selectionSortUtil(sorted_arr); 

    for(j = 0; j < NUM_TASKS*NUM_SUBTASKS; j++){
      for(k = 0; k < NUM_CORES; k++){
        if(core_tracker[k]+sorted_arr[j]->utilization < 100){
          core_tracker[k]+=sorted_arr[j]->utilization;
          sorted_arr[j]->core = k; 
          printk("subtask: (%u) with utilization (%u) assigned to core (%u) which has total utilization (%u)\n", j, sorted_arr[j]->utilization, k, core_tracker[k] );
          break; 
        }
      }
    }

    //sort array by core in ascending order 
    selectionSortCore(sorted_arr); 
    
    for(j = 0; j < NUM_TASKS*NUM_SUBTASKS; j++){
      printk("subtask: (%u) with relative deadline (%lu) assigned priority (%u) on core (%u)\n", j, sorted_arr[j]->relative_deadline, sorted_arr[j]->priority, sorted_arr[j]->core);

    }

}

