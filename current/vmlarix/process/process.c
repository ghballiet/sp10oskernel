#include <process.h> 
#include <stddef.h>
#include <process_arch.h>
#include <process_q.h>
#include <mmap.h>
#include <dev_list.h>
#include <fcntl.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define PROC_FD_DEFAULT -1
#define PROC_NUM_FD 256

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

proc_rec *get_running_process() {
  return curr_proc;
}


// initialize the process file descriptor table
int32_t *process_fd_create() {
  // define p->fd as an array of ints
  // of size NUM_FD
  
  // call vfs_open_dev(console_major,console_minor,mode,flags) to open
  // returns a FD which we can assign
  
  // STDIN
  int32_t fd_stdin = vfs_open_dev(console_major,console_minor,O_RDONLY,0);
  
  // STDOUT
  int32_t fd_stdout = vfs_open_dev(console_major,console_minor,O_WRONLY,0);
  
  // STDERR
  int32_t fd_stderr = vfs_open_dev(console_major,console_minor,O_WRONLY,0);
  
  int32_t fds[PROC_NUM_FD];
  int i;
  
  fds[STDIN] = fd_stdin;
  fds[STDOUT] = fd_stdout;
  fds[STDERR] = fd_stderr;
  
  for(i=3; i < PROC_NUM_FD; i++) {
    fds[i] = PROC_FD_DEFAULT;
  }
  
  return fds; 
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

  // // initialize the process file descriptor table
  // kprintf("Setting up file descriptors:\n\r");
  // p_tab[procId].fd = process_fd_create();
  // kprintf("STDIN = %d\n\r",p_tab[procId].fd[0]);
  // kprintf("STDOUT = %d\n\r",p_tab[procId].fd[1]);
  // kprintf("STDERR = %d\n\r\n\r",p_tab[procId].fd[2]);

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
