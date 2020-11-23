#ifndef TASK_FUNCTIONS_H
#define TASK_FUNCTIONS_H

//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

int calibrate_thread(void *threadData);

int64_t subtask_func_calibrate(_subtask_t *subtaskTemp);

void subtask_func(_subtask_t * subtask_temp);

void get_itterations(_subtask_t * subtaskTemp);

int thread_fn1(void * data); 

int subtask_thread_func(void * data); 

#endif
