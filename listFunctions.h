#ifndef _LIST_FUNCTIONS_H
#define _LIST_FUNCTIONS_H

//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif


void newTask(_task_t *taskList);

_task_t *initTask(unsigned long period_ms, unsigned int task_num, unsigned int num_subtasks);

_subtask_t *initSubtask(int execution_time, int sub_task_num, int parent_index, uint8_t firstRun);

void delTask(_task_t parentTask);

void delSubtask(_subtask_t *parentSubtask);


#endif
