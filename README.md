# CSE-522-Group

Lab 2: 
End-to-End Real-Time Tasks

CSE 522: Advanced Operating Systems 
Joe Albert & Eric Neblock 

Due Date: 11/22/2020


# Group Information
   The people who worked on this lab are Joe Albert (jmalbert@wustl.edu) and Eric Neblock (ceneblock@wustl.edu).

# Initial Configuration
   To begin, the team created a basic linux kernel module and Makefile, taken from the studio: Kernel Memory Management: (https://www.cse.wustl.edu/~james.orr/courses/cse522_fl20/studios/02_kernel_memory.html)

   Next, a module parameter was added to allow the user to input the mode of operation (either default ‘calibrate’ mode, or ‘run’ mode.) 

   To ensure to mode selection is working, the module was tested with input where no parameter is given, a non-’run’ parameter is given, and finally ‘run’ parameters are given, respectively.  From these tests, it can be determined that the kernel module works as expected, and only allows for the two aforementioned modes of operation. 

# Module Design 

# <header.h> Design: 
   The header file simply contains two structs, one for task, and one for subtasks. An array of struct Tasks is then declared in the header file, such that other files can access the tasks. In order to access the subtasks, a list is used for easy insertion / deletion.  With the structs declared, an array of tasks which contain a pointer to a subtask, which acts as the list head, is defined and declared, as seen below: 

    struct Sub_task{
        unsigned long execution_time; 
        unsigned long relative_deadline; 
        unsigned int sub_task_num; 
        unsigned int parent_index; 

        //4. added params 
        struct hrtimer timer;
        struct task_struct * task; 
        ktime_t last_release_time; 
        unsigned int loop_iterations_count; 
        unsigned int cumulative_exec_time;
        unsigned int utilization;   unsigned int core; //cpumask.h
        unsigned int task_period; 
 
        unsigned int inUse; //set to 1 if this is a valid task.
        struct sched_param priority;

        struct list_head sibling;
    };

    struct Task{
        unsigned long period_ms;
        unsigned int task_num; 
        unsigned int num_subtasks; 
        unsigned long exec_time_ms; 

        uint8_t firstRun;
        _subtask_t *subtasks;
  
        struct list_head sibling;
    };

        static _task_t *taskStruct[NUM_TASKS];

# <listFunctions.h> Design: 
   This file contains the helper functions for initialization of the tasks and subtasks. The function initTask() and initSubtask() simply define and declare the fields of a new task and subtask, respectively. The delTask() and delSubtask() functions free the tasks and subtasks. The setParams() function simply initializes the task values for execution time, as well as cumulative execution time and utilization for each of the tasks subtasks. Finally, the determineCore() function sorts the array by utilization in order to assign core’s in a ‘first-fit’ manner. Next, the subtasks are sorted by core and relative deadline in order to assign priority values. While the array is sorted by core, the subtasks assigned to each core are copied to their appropriate container in the 2D array to be used for calibration. 

# <taskFunctions.h> Design: 
   The file contains a helper function for initializing each subtask's threads. The calibrate_thread() function sets up the threads and determines the loop iterations count for each subtask within a task. The subtask_func() simply calls ktime_get() for the given subtasks loop iteration count. The get_itterations() function calls the subtask_func() with a timer before and after to determine the correct number of loop iterations to meet timing requirements. The subtask_lookup_function() takes a pointer to a timer and returns a pointer to the corresponding subtask. The timer_expiration_func takes a pointer to a timer and calls the subtask_lookup_function() to get the corresponding subtask. The subtask is then woken. Finally, the run_thread_func is responsible for initialization of the timer last release time of each subtask. 

# Calibration Design: 
   If calibration mode is enabled, the module determines the correct amount of loops for each subtask to complete while meeting the required timing. To do this, a 2-dimensional array is used, as mentioned above, which contains pointers to the subtasks for each core. The array is created in the determineCore() function, declared in taskFunctions.c. 

   In the modules init function, a thread is woken which calls the function calibrateThreads(). The calibrateThreads() function creates a thread for each core, sets the scheduling policy and priority, and then sets a sleep timer for 100ms. Next, the threads are woken up and call their thread function, calibrate_thread(), found in taskFunctions.c. The function calls another function, get_itterations() which sets a timer before and after a call to the subtasks function. The function sets each subtasks loop_itterations_count field to the appropriate value which meets timing requirements, with a simple binary search for simplicity sake. The calibrate_thread() function then creates the thread for each subtask for the given core, and sets the thread’s policy and priority. 

# Run Design:
   If run mode is enabled, the first thing that we do is initialize our timer so that the end to end functionality can be achieved. Once the timer goes off, we'll wake up a given process (or depending on the circumstances the subsequent process). At that point, assuming that the thread can still be run (i.e. we haven't unloaded the module early) we then actually do our "work" by repeatedly calling ktime_get() for a particular number of times.

   Next we check if we're the first subtask. If we are then sleep for an "absolute" time that is our last_release_time (the time right be fore we started doing work). We will then sleep to finish out the remaining time. In the ideal world the timer would wake up right away because of how accurate the estimation was.

   If we aren't the first subtask, then we'll check if we've used up our time quantum. If so, then we'll wake up the next task (presuming it exists); otherwise we'll just sleep.


# Testing and Evaluation
  When it comes to testing, we were unable to fully complete it. We successfully got the calibrate functionality working; however when it came to the run functionality, there seemed to be some issues. In particular there were some invalid memory location accesses (despite passing in pointers and values where necessary. We went so far as to add -Werror in our make file to have gcc treat all warnings as errors to try and write the "best" code.

  The code that is presented should be enough to give an idea of what needs to happen for successful runs. The probablem seems to lie within the list_for_each_entry that determines what the next subtask is. The array which we used to hold pointers to the _subtasks_ for each task was determined to not be acessible within the file. We tried many fixes, from adding a get_subtask_from_task_index() function, to creating a copy of the taskStruct[] array, but none of them allowed us to access the tasks and subtasks. After many hours of struggling, we decided to turn in our best effort attempt. 

# Build Instructions
   In order to build the module, a Makefile has been included. The build should be compiled on a linux lab machine, and run on a raspberry pi. In order to build the module, run the following commands on a linux lab machine in the project directory: 

   module add raspberry

   KERNEL=kernel7

   LINUX_SOURCE=path_to_/linux_source/linux

   make -C $LINUX_SOURCE ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$PWD modules 

# Development Effort 
   For this assignment, the team (Joe & Eric) spent about 75 hours. It should be noted that the team did struggle significantly despite best efforts to make use of course resources (piazza, slides, lectures, and others); however, they ultimatly failed to resolve the most pressing issue from seg faults. Those required restarting the pi with every new attempt, and took up a significant amount of our debugging time.

