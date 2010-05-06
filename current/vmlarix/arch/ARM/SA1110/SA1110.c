#include <sys/types.h>
#include <SA1110.h>
#include <linker_vars.h>
#include <interrupts.h>       

int SA1110_low_level_init()
{
  /* This function is called by startup, just before the call
     to main() */
  /* Its job is to initialize hardware to a known state. (enable the
     MMU, etc.) */

  /* arch_init(); */
  /* char_dev_init(char_dev_list); */

  /* kprintf("Kernel start: %X\n\r",__kernel_ram_start__); */
  /* kprintf("Kernel end: %X\n\r",__kernel_ram_end__); */

  /* kprintf("Initializing memory\n\r"); */
  /* phys_mem_init(); */
  /* kprintf("There are %d pages free\n\r",phys_mem_count_free()); */

  /* kprintf("Setting up kernel page table\n\r"); */
  /* kernel_pt = setup_kernel_page_table(); */
  /* kprintf("There are %d pages free\n\r",phys_mem_count_free()); */

  /* kprintf("Setting up slab allocator\n\r"); */
  /* slab_init(); */

  /* kprintf("Loading MMU\n\r"); */
  /* set_initial_page_table(kernel_pt); */

  /* kprintf("Enabling MMU\n\r"); */
  /* enable_mmu(); */
  /* kprintf("MMU enabled\n\r"); */

  /* kmalloc_init(); */

  return 0;

}

/* this routine should set the hardware to have the Interrupt
   vector table at FFFF0000 by setting bits 4 and 13 in      
   coprocessor 15 register 1 */                              
void set_IVT_address()                                
{                                                            
  int a,b;  /* reserve space on stack to store r0 and r1 */  
  asm ( "str   r0,[sp]" );  /* store r0 as b */              
  asm ( "str   r1,[sp, #4]" ); /* store r1 as a */           
  asm ( "mrc   p15, 0, r0, c1, c0" );                        
  asm ( "ldr   r1, =0x2010" ); /* mask to set bits with */   
  asm ( "orr   r0,r0,r1");     /* 'or' original with mask */ 
  asm ( "mcr   p15, 0, r0, c1, c0" );                        
  asm ( "ldr   r0,[sp]" );   /* restore r0 with b */         
  asm ( "ldr   r1,[sp, #4]" ); /* restore r1 with a */       
}                                                            


int SA1110_setup_vector_table()
{                              
  uint32_t tmp;                
  /* the following is the machine code for the LDR PC,[PC, 0] instruction
     it loads the program counter with the program counter + 0.          
     we can insert a different number x in it with the | operator,       
     to load the PC with a value that is offset from the pc by x */      
  static uint32_t const LDR_PC_PC = 0xE59FF000;                          

  /* The address of the external variable __ivt_start__ tells where the */
  /* Interrupt Vector Table is placed. */                                 
  /* This section will load the vector table with LDR PC, [PC, 0x18] */   
  /* instructions. 0x18 bytes after each entry will be a pointer to */    
  /* the ISR, which is loaded into the PC by the LDR PC, [PC, 0x18] */    
  /* instruction */                                                       
  /* The Interrupt Service Routines are defined in interrupts.h and  */   
  /* implemented in interrupts.s        */                                

  /* create a pointer to where we want the IVT to be */
  uint32_t *v = (uint32_t *) &__ivt_start__;           

  *(v + 0x00) = LDR_PC_PC | 0x18; /* reset vector FFFF0000 */  
  *(v + 0x01) = LDR_PC_PC | 0x18; /* undefined instruction FFFF0004 */
  *(v + 0x02) = LDR_PC_PC | 0x18; /* Software Interrupt FFFF0008 */   
  *(v + 0x03) = LDR_PC_PC | 0x18; /* instruction prefetch abort FFFF00C */
  *(v + 0x04) = LDR_PC_PC | 0x18; /* data abort FFFF0010 */               
  *(v + 0x05) = LDR_PC_PC | 0x18; /* not used FFFF0014 */                 
  *(v + 0x06) = LDR_PC_PC | 0x18; /* IRQ FFFF0018 */                      
  *(v + 0x07) = LDR_PC_PC | 0x18; /* FIQ FFFF001C */                      

  /* pointers to the ISRs that are loaded by the instructions in the */
  /* IVT above */                                                      
  *(v + 0x08) = (uint32_t)&ARM_reset;  /* reset vector */              
  *(v + 0x09) = (uint32_t)&ARM_undef;  /* undefined instruction */     
  *(v + 0x0A) = (uint32_t)&ARM_swi;  /* Software Interrupt */          
  *(v + 0x0B) = (uint32_t)&ARM_pAbort;  /* instruction prefetch abort */
  *(v + 0x0C) = (uint32_t)&ARM_dAbort;  /* data abort */                
  *(v + 0x0D) = (uint32_t)&ARM_reserved;  /* not used */                
  *(v + 0x0E) = (uint32_t)&ARM_irq;  /* IRQ */                          
  *(v + 0x0F) = (uint32_t)&ARM_fiq;  /* FIQ */                          

}

__attribute__ ((naked)) 
void SA1110_dcache_flush()
{                         
  asm ("mcr p15, 0, r0, c7, c6, 0 " );
  asm ("mov  pc, lr");                
}                                     

__attribute__ ((naked))
void SA1110_tlb_flush()
{                      
  asm ("mcr p15, 0, r0, c8, c7, 0 " );
  asm ("mov  pc, lr");                
}                                     

__attribute__ ((naked))
void SA1110_tlb_flush_addr(void *addr)
{                                     
  asm ("mcr p15, 0, r0, c8, c6, 1 " );
  asm ("mov  pc, lr");                
}                                     

__attribute__ ((naked))
int read_CPSR()        
{                      
    asm ("MRS  r0, CPSR");
    asm ("mov  pc, lr");  
}                         

__attribute__ ((naked))
void write_CPSR(int v) 
{                      
  asm ("MSR  CPSR, r0");
  asm ("mov  pc, lr");  
}                       

__inline void enable_IRQ(void)
{                             
    int tmp;                  
    asm("MRS r0, CPSR");      
    asm("BIC r0, r0, #0x80"); 
    asm("MSR CPSR, r0");      
}                             
__inline void disable_IRQ(void)
{                              
  int tmp;                     
  asm("MRS r0, CPSR");         
  asm("ORR r0, r0, #0x80");    
  asm("MSR CPSR, r0");         
}                              
__inline void enable_FIQ(void) 
{                              
    int tmp;                   
    asm("MRS r0, CPSR");       
    asm("BIC r0, r0, #0x40");  
    asm("MSR CPSR, r0");       
}                              
__inline void disable_FIQ(void)
{                              
    int tmp;                   
    asm("MRS r0, CPSR");       
    asm("ORR r0, r0, #0x40");  
    asm("MSR CPSR, r0");       
}                              

void start_timer()
{                       
  /* This routine sets up the interrupt hardware to allow interrupts
     from the OS timer. */                                          

  /* STEP 1: Set the interrupt enable for OS timer 0 */
  *ICMR = (*ICMR)|(1<<26); 
  *ICCR = (*ICCR)|1; 

  /* STEP 2: Set up the OS timer to give us interrupts at a fixed rate
     Use TIMER_INTERVAL defined in SA1110.h. Start with TIMER_INTERVAL
     set to generate interrupts at 1 second intervals.                
  */                                                                  
 *OSMR0 = *OSCR + TIMER_INTERVAL_1000MS;

  /* STEP 3: Enable OS timer 0 to send interrupts. */
 *OIER = (*OIER)|1;

  /* STEP 4: Clear the OS timer status register */
 *OSSR = *OSSR & 1; 
}

void setup_interrupt_controller()
{
  SA1110_setup_vector_table();
  /* set up the hardware to have the IVT at FFFF0000 */
  /* set bits 4 and 13 in coprocessor 15 register 1*/
  /* I think read_coproc is broken */
  set_IVT_address();

  /* initialize the interrupt controller */
  *ICLR = 0;
  *ICMR = 0;
  *ICCR = 0;
}

void enable_interrupts()
{
  enable_IRQ();
  enable_FIQ();
}


