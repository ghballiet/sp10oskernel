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
  int i;
  for(i=0; i<PROCESS_TABLE_SIZE; i++)
    p_tab[i].state = PROCESS_AVAILABLE;
  /* set up next_PID */
  /* nothing should have PID 0, see comment for process_create */
  next_PID = 1;
  /* initialize the run queue */
  run_q = (pq *)kmalloc(sizeof(pq));
  pq_init(run_q);
}

/* Create a process by allocating a process table entry and
   initializing it appropriately.  Returns the PID of the process
   created, 0 means no free slots. */
proc_rec* process_create(PID_t parent, void *start, void *stack)
{
  /* Find a process table entry. */
  int i = 0;
  while(i<PROCESS_TABLE_SIZE && p_tab[i].state != PROCESS_AVAILABLE)
    i++;
  if(i==PROCESS_TABLE_SIZE) /* no free slots, return 0 */
    return 0;
  //kprintf("Process_create: found process table entry %d\r\n", i);

  /* Get a pointer to the process record */
  proc_rec *proc = &(p_tab[i]); /* TODO: double check this syntax (we could
				   just manipulate stuff like p_tab[i].PID, but
				   eventually we do need the pointer to the
				   record to return) */
  //kprintf("Process_create: got pointer to process table entry %d\r\n", i);
  /* Initialize the opaque architecture specific part of the process
     table entry. */
  proc->arch = process_arch_create(start, stack);
  /* Set the process state to STARTING. */
  proc->state = PROCESS_STARTING;
  /* Assign a PID. We really should test to make sure next_PID is not
     already assigned to a process in the process table, but we can
     skip it for now. */
  proc->PID = next_PID;
  /* Update next_PID */
  next_PID++;
  /* Set the PPID */
  proc->PPID = parent;
  /* Put the process record on the run queue. */
  kprintf("Process_create: adding proc object to run_q\r\n");
  pq_append(run_q, proc);
  /* Return a pointer to the process table entry */
  //kprintf("Process_create: done\r\n");
  return proc;
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
  process_arch_destroy(curr_proc->arch);
  curr_proc->arch = NULL;
  /* Mark the current process table entry as AVAILABLE */
  curr_proc->state = PROCESS_AVAILABLE;
  /* Call the scheduler to let some other process run */
  schedule();
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
  kprintf("schedule: entered schedule function\r\n");
  int i;  
  int next;
  /* TODO: what are we supposed to do with these i and next variables? I sent
     an email about this on Sat, 3/27 */

  /* Idle the CPU if no processes are ready. */
  kprintf("schedule: still in schedule function\r\n");
  while(pq_empty(run_q));
  kprintf("schedule: run_q is not empty\r\n");

  /* If a process is running, then save it's state and put it on the run
     queue. */
  if(curr_proc->state == PROCESS_RUNNING) {
    kprintf("schedule: Process currently running\r\n");
    process_arch_save(curr_proc->arch);
    curr_proc->state = PROCESS_WAITING;
    pq_append(run_q, curr_proc);
  }
  kprintf("schedule: current process is not RUNNING\r\n");
  /* Get the next process from the run queue. */
  curr_proc = pq_pop(run_q);
  /* Either start it, or resume it, depending on it's state */
  if(curr_proc->state == PROCESS_STARTING) {
    kprintf("schedule: current process status is STARTING\r\n");
    curr_proc->state = PROCESS_RUNNING;
    process_arch_start(curr_proc->arch);
  } else if(curr_proc->state == PROCESS_WAITING) {
    kprintf("schedule: current process status is WAITING\r\n");
    curr_proc->state = PROCESS_RUNNING;
    process_arch_resume(curr_proc->arch);
  } else {
    kprintf("Error: Process %d on front of process queue was not STARTING or WAITING\r\n",
	    curr_proc->PID);
  }
}

/* Process_exit may do some cleanup, and then calls process_destroy */
void process_exit()
{
  void process_destroy();
}
