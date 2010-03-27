#include <process.h> 
#include <stddef.h>
#include <process_arch.h>
#include <process_q.h>
#include <mmap.h>
#include <dev_list.h>

/* This is for tracking the next PID to be assigned. */
PID_t next_PID;         

/* The process table. */
proc_rec p_tab[PROCESS_TABLE_SIZE];

/* Pointer to the currently executing process. */
proc_rec *curr_proc;

/* Pointer to the run queue. (will be allocated with kmalloc) */
pq *run_q;

/* Initialize the process table entries, curr_proc, and run_q */
void process_table_init()
{
  /* set up curr_proc pointer */
  curr_proc = NULL;
  /* initialize process table entries */
  /* TODO: should this do more? */
  int i;
  for(i=0; i<PROCESS_TABLE_SIZE; i++)
    p_tab[i] = NULL;
  /* set up next_PID */
  /* nothing should have PID 0, see comment for process_create */
  next_PID = 1;
}

/* Create a process by allocating a process table entry and
   initializing it appropriately.  Returns the PID of the process
   created, 0 means no free slots. */
proc_rec* process_create(PID_t parent, void *start, void *stack)
{
  /* Find a process table entry. */
  int i;
  while(i<PROCESS_TABLE_SIZE && p_tab[i] != NULL)
    i++;
  if(i==PROCESS_TABLE_SIZE) /* no free slots, return 0 */
    return 0;
  /* TODO: what's the relationship between PID and process table index? */

  /* Initialize the opaque architecture specific part of the process
     table entry. */

  /* Set the process state to STARTING. */

  /* Assign a PID. We really should test to make sure next_PID is not
     already assigned to a process in the process table, but we can
     skip it for now. */

  /* Set the PPID */

  /* Put the process record on the run queue. */

  /* Return a pointer to the process table entry */

}

/* Called by a process when it exits. It should call the
   process_arch_destroy function to release its architecture specific
   portion, set the process table entry state to AVAILABLE, and then
   call the scheduler to let another process run. Later, it will also
   need to release it's page table and all of its pages, but not
   yet. */
void process_destroy()
{
  /* destroy architecture specific part of current process and set to
     NULL */

  /* Mark the current process table entry as AVAILABLE */

  /* Call the scheduler to let some other process run */

}

/* Puts a process on the ready queue. This function will be used by
   device drivers when they complete an I/O request and want to allow
   the process to run again. */
void process_resume(proc_rec *p)
{
  pq_append(run_q,p);
}

/* The scheduler is a key piece of the OS. It checks the run queue and
   transfers control to the next process. If no process is ready to
   run, then it puts the CPU into sleep mode. */
void schedule()
{
  int i;  
  int  next;

      /* Idle the CPU if no processes are ready. */

      /* If a process is running, then save it's state and 
	 put it on the run queue. */

      /* Get the next process from the run queue. */

      /* Either start it, or resume it, depending on it's state */

}

/* Process_exit may do some cleanup, and then calls process_destroy */
void process_exit()
{
  void process_destroy();
}
