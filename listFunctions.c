#include "listFunctions.h"

#include <linux/slab.h>

_subtask_t *initSubtask(uint8_t firstRun)
{
  _subtask_t *newSubtask = kmalloc(sizeof(_subtask_t), GFP_KERNEL);

  //memset(&newSubtask, 0, sizeof(_subtask_t));
  if(firstRun)
  {
    INIT_LIST_HEAD(&newSubtask->sibling);
  }
  //if we have already created the link list, then we'll add it in initTask

  hrtimer_init(&newSubtask->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

  return newSubtask;  
}

_task_t *initTask(unsigned long period_ms, unsigned int task_num, unsigned int num_subtasks)
{

  _task_t *newTask = kmalloc(sizeof(_task_t), GFP_KERNEL);

  //memset(newTask, 0, sizeof(_task_t));

  newTask->period_ms = period_ms;
  newTask->task_num = task_num;
  newTask->num_subtasks = num_subtasks;

  newTask->subtasks = initSubtask(1);

  return newTask;
}

void newTask(_task_t *taskList)
{
  _task_t *newTask = kmalloc(sizeof(_task_t), GFP_KERNEL);
  
  newTask = initTask(0,0,0);

  list_add(&newTask->sibling, &taskList->sibling);
}

//size chosen arbitrarily until else is specified

//CEN: we should probably use a list_head instead of a static array...it's
//standard and allows for dynamic insertation\removal. <linux/list.h>
//_task_t tasks[2] = {{.period_ms = 1000, .task_num = 0, .num_sub_tasks = 2, .exec_time_ms = 0}, {.period_ms = 1000, .task_num = 1, .num_sub_tasks = 2, .exec_time_ms = 2}}; 

//_subtask_t sub_tasks[4] = {{100, 0, 0}, {100, 1, 0}, {100, 0, 1}, {100, 1, 1}} ; 


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
