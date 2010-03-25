#include <process_arch.h> 
#include <process.h>
#include <stddef.h>
#include <SA1110.h>
#include <mmap.h>
#include <dev_list.h>

/* The architecture specific portion of a process record */
typedef struct process_arch_rec {
  uint32_t regs [16]; // a place to store r0-r15 when process is not running
  uint32_t cpsr; // a place to store the cpsr when process is not running
  uint32_t fp; //dont comment out even though we dont use, breaks assembly
  uint32_t *saved_regs; // pointer to registers saved on the stack
}proc_arch_rec;

void cpu_idle()
{
  /* insert the inline asm statements needed to put the CPU in idle
     mode */
}

/* Performs the architecture dependent part of initializing a process */
void* process_arch_create(void *entry, void *stack)
{
  int i;
  proc_arch_rec *rec;

  if((rec = (proc_arch_rec*)kmalloc(sizeof(proc_arch_rec)))==NULL)
    return NULL;

  /* initialize all registers to contain their register number */
  for(i=0;i<16;i++)  
    rec->regs[i] = i;

  /* When r15 and r13 are loaded by the process_arch_start function,
     the CPU will begin executing at entry() with the stack pointer set
     to stack, and the return address set to process_exit() */
  rec->regs[13] = (uint32_t) stack;
  rec->regs[14] = (uint32_t) &process_exit;
  rec->regs[15] = (uint32_t) entry;

  /* if we wanted to pass arguments to the task, we would set r0 to
     argc, and we would set r1 to point at argv, which would be an
     array of char* 
  */

  /* for environment variables, we would set r3 to point to a 
     structure (array) containing the environment variables. */

  /* when this is loaded, the processor will be in user mode */
  rec->cpsr = 0x00000010;
  rec->saved_regs = (unsigned *)rec->regs[13];

  return rec;
}

/* destroy the architecture specific portion of a process table
   entry */
void process_arch_destroy(void *arch_rec)
{
  kfree(arch_rec);
}

