#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

/* a list of possible process states */
#define PROCESS_AVAILABLE 0
#define PROCESS_STARTING  1
#define PROCESS_RUNNING   2
#define PROCESS_WAITING   3
#define PROCESS_EXITING   4

#ifndef PROCESS_TABLE_SIZE 
#define PROCESS_TABLE_SIZE 256
#endif

typedef struct process_rec {
  /* A pointer to the architecture specific portion */
  void *arch;

  uint32_t PID;    /* process ID */
  uint32_t PPID;   /* parent's process ID */
  uint32_t state;  /* Current state of the process from list above */

  int32_t *fd; /* pointer to table of file descriptors */
  int32_t num_fd; /* number of file descriptors that this program can have */

  /* Pointers for managing queues of process records */
  struct process_rec *next;
  struct process_rec *prev;
}proc_rec;

// initialize the process file descriptors
int32_t *process_fd_create();

/* Initialize the process table and associated data */
void process_table_init();

/* Create a process by allocating a process table entry and
   initializing it appropriately.  Returns the PID of the process
   created, 0 means the process table is full. */
proc_rec* process_create(PID_t parent, void *start, void *stack);

/* Releases the process table entry */
void process_exit();

/* Run the next available process */
void schedule();

#endif
