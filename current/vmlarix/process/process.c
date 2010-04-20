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
  int i;
  for(i=0;i<PROCESS_TABLE_SIZE;i++)
    {
      p_tab[i].arch=NULL;
      p_tab[i].state = PROCESS_AVAILABLE;
    }
  curr_proc = NULL;
  next_PID=1;
  run_q = (pq*)kmalloc(sizeof(pq));
  pq_init(run_q);
}

/* Create a process by allocating a process table entry and
   initializing it appropriately.  Returns the PID of the process
   created, 0 means no free slots. */
proc_rec* process_create(PID_t parent, void *start, void *stack)
{
  int i;
  PID_t procId;

  /* Find a process table entry. */  
  for(i=0;(i<PROCESS_TABLE_SIZE)&&(p_tab[i].state != PROCESS_AVAILABLE);i++);
  if((i>=PROCESS_TABLE_SIZE)||(p_tab[i].state != PROCESS_AVAILABLE))
    return 0;
  else
    procId=i;

  /* Initialize the opaque architecture specific part of the process
     table entry. */
  p_tab[procId].arch = process_arch_create(start,stack);

  /* Set the process state to STARTING. */
  p_tab[procId].state = PROCESS_STARTING;

  /* Assign a PID. We really should test to make sure next_PID is not
     already assigned to a process in the process table, but we can
     skip it for now. */
  p_tab[procId].PID = next_PID++;  

  /* Set the PPID */
  p_tab[procId].PPID = parent;

  /* Put the process record on the run queue. */
  pq_append(run_q,&(p_tab[procId]));  

  /* Return a pointer to the process table entry */
  return &(p_tab[procId]);
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
  curr_proc->arch=NULL;
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
  int i;  
  int  next;

  if(pq_empty(run_q) && (curr_proc == NULL))
    {
      /* Idle the CPU if no processes are ready. */
      cpu_idle();       
    }
  else
    {
      /* If a process is running, then save it's state and 
	 put it on the run queue. */
      if(curr_proc != NULL)
	{
	  process_arch_save(curr_proc->arch);
	  pq_append(run_q,curr_proc);
	}

      /* Get the next process from the run queue. */
      curr_proc = pq_pop(run_q);

      /* Either start it, or resume it, depending on it's state */
      switch (curr_proc->state)
	{
	case PROCESS_STARTING:
	  curr_proc->state = PROCESS_RUNNING;
	  process_arch_start(curr_proc->arch);
	  return;  
	case PROCESS_RUNNING:
	  /*Call the assembly function to resume this process*/    
	  process_arch_resume(curr_proc->arch); 
	  return;
	default:
	  panic("Invalid process state");
	  break; 
	}
    }
}

/* Process_exit may do some cleanup, and then calls process_destroy */
void process_exit()
{
  void process_destroy();
}
