#include <SA1110.h> 
#include <process.h> 
#include <kprintf.h>

int c_IRQ_handler(unsigned *saved_regs, unsigned spsr)
{
  static int first_call = 1;

  /* initially, you should do the following:              */
  /*                                                      */
  /* Read the OSSR and check for interrupt from timer 0   */
  /* If timer 0 is signaling an interrupt, then           */
  /*   clear the interrupt                                */
  /*   reset the OS Timer Match Register                  */
  /*   print a short message                              */

 
  /* After you have that much working, you can modify     */
  /* this handler so that it switches from one process    */
  /* (task) to another, and speed up the timer            */

  /* Helper functions for starting tasks and switching    */
  /* between tasks can be found in process.c and          */
  /* SA1110_process.s                                     */

  /*OS Timer 0 caused interrupt, switch tasks*/
  if(*OSSR & 1)
    { 
      /*Clear the interrupt by writing a 1 at pos 0*/
      *OSSR = *OSSR & 1; 
      /*Set next OSMR*/
      *OSMR0 = *OSCR + TIMER_INTERVAL_1000MS; 
      /*Switch process*/
      schedule(); 
    }
}

int c_FIQ_handler()
{
  kprintf("FIQ received\n\r");
}


int32_t sys_undefined(int32_t syscall_num)
{
  kprintf("Undefined system call: %d\n\r",syscall_num);
}


uint32_t c_SWI_handler(uint32_t syscall_num, 
			uint32_t p1, 
			uint32_t p2, 
			uint32_t p3)
{
  switch(syscall_num)
    {
/*     case SYS_open: */
/*       return sys_open((char *)p1,p2,p3); */
/*       break; */
/*     case SYS_close: */
/*       return sys_close(p1); */
/*       break; */
/*     case SYS_read: */
/*       return sys_read(p1,(void *)p2,p3); */
/*       break; */
/*     case SYS_write: */
/*       return sys_write(p1,(void *)p2,p3); */
/*       break; */
    default:
      return sys_undefined(syscall_num);
      break;
    }
}

int c_abort_handler(char *msg, int address)
{
  kprintf("%s at address %X\n\r",msg,address);
}



