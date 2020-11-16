#ifndef _LIST_FUNCTIONS_H
#define _LIST_FUNCTIONS_H

//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

/**
 * @brief creates a new task and adds it to the siblings of tasks
 * @param taskList the head of the all the tasks
 */
void newTask(_task_t *taskList);

/**
 * @brief creates and initializes a new task and subtask.
 * @param period_ms how long the period is (1/f)
 * @param task_num the task number
 * @param num_subtasks how many subtasks there are for this task
 * @return a new _task_t pointer
 */
_task_t *initTask(unsigned long period_ms, unsigned int task_num, unsigned int num_subtasks);

/**
 * @brief creates and initializes a new subtask
 * @param execution_time how long the task should run
 * @param sub_task_num how long the subtask should run
 * @param parent_index the parent task number
 * @param firstRun if this is the first run and we should initialize the linked
 * list
 * @return a new subtask
 */
_subtask_t *initSubtask(int execution_time, int sub_task_num, int parent_index, uint8_t firstRun);

/**
 * @brief deletes all of the tasks and subtasks
 * @param parentTask the top of the parentTask of everything to delete
 */
void delTask(_task_t parentTask);

/**
 * @brief deletes all the subtasks
 * @param parentSubtask the top of the subtask list of everything to delete
 */
void delSubtask(_subtask_t *parentSubtask);


#endif
