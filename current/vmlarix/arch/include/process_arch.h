
#ifndef PROCESS_ARCH_H
#define PROCESS_ARCH_H
#include <sys/types.h>

/* Allocates and initializes the architecture specific part of 
   a process record. Returns a pointer to the new record. */
void* process_arch_create(void *entry, void *stack);

/* Destroy the architecture specific portion of the process table
   entry */
void process_arch_destroy(void *arch_rec);

/* Begin execution of a process */
void process_arch_start(void *arch_rec);

/* Save the state of the currently executing process */
void process_arch_save(void *arch_rec);

/* Resume execution of a process */
void process_arch_resume(void *arch_rec);

/* override the process entry point */
void process_arch_set_entry(void *arch_rec, void *init_entry);


#endif
