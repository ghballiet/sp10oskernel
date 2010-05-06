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


int c_abort_handler(char *msg, int address)
{
  kprintf("%s at address %X\n\r",msg,address);
}



