//compile with -DX=1
#ifdef X
#include "otherHeader.h"
#else
#include "header.h"
#endif

#include "taskFunctions.h"

static int calibrate_thread(void *threadData)
{
  _subtask_t *coreSubtasks = (_subtask_t *)threadData;
  unsigned int x = 0;
  set_current_state(TASK_INTERRUPTIBLE);
  schedule();

  while(coreSubtasks[x]->inUse == 1)
  {
    //Set each subtasks's priority
    sched_setscheduler(coreSubtasks[x]->task, SCHED_FIFO, coreSubtasks[x]->priority);

    //Pass a pointer to the subtask function
    int64_t taskTime = subtask_func(coreSubtasks[x]);

    //Ideal situation that: 
    coreSubtasks[x]->loop_iterations_count = coreSubtasks[x]->execution_time / taskTime;

    kprintf("Task: %u Subtask: %u Itterations needed: %u\n", coreSubtasks[x]->parent_index, coreSubtasks[x]->sub_task_num, coreSubtasks[x]->loop_iterations_count);

    x++
  }

  return 0;
}

//No compiler optimization allowed
static int64_t suntask_func_calibrate(_subtask_t *subtaskTemp)  __attribute__ ((optimize(0)))
{
  ///Determine how long it takes to run a loop with one iteration
  unsigned int i = 0;
  preempt_disable(); //don't preempt me
  ktime_t startTime = ktime_get();
  for(i = 0; i < 1; i++)
      ktime_get(); 
  }
  ktime_t endTime = ktime_get();
  preempt_enable();

  return ktime_to_ms(ktime_sub(endTime, startTime));
}

static void subtask_func(_subtask_t * subtask_temp){
    unsigned int i; 
    for(i = 0; i < subtask_temp->loop_iterations_count; i++){
        ktime_get(); 
    }
}

