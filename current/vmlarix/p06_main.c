
#include <sys/types.h>
#include <kprintf.h>
#include <phys_mem.h>
#include <dev_list.h>
#include <mmu.h>
#include <slabs.h>
#include <linker_vars.h>
#include <kmalloc.h>
#include <interrupts.h>
#include <process.h>

uint16_t console_major;
uint16_t console_minor;

uint16_t initrd_major;
uint16_t initrd_minor;

int taska()
{          
  while(1) 
    kprintf("a");
}                

int taskb()
{
  while(1)
    kprintf("b");
}          

int taskc()
{
  while(1)
    kprintf("c");
}

#define BUFF_LEN 100
int main()
{
  uint32_t i;
  uint32_t j;
  uint32_t pages_free;
  char buffer[BUFF_LEN];
  phys_mem_t kernel_pt;
  proc_rec *taska_ptr,*taskb_ptr,*taskc_ptr;
  char *taska_stack,*taskb_stack,*taskc_stack;

  arch_init();

  char_dev_init(char_dev_list);

  kprintf("Kernel start: %X\n\r",__kernel_ram_start__);
  kprintf("Kernel end: %X\n\r",__kernel_ram_end__);

  kprintf("Initializing memory\n\r");
  phys_mem_init(); 
  kprintf("There are %d pages free\n\r",phys_mem_count_free());

  kprintf("Setting up kernel page table\n\r");
  kernel_pt = setup_kernel_page_table();
  kprintf("There are %d pages free\n\r",phys_mem_count_free());

  kprintf("Setting up slab allocator\n\r");
  slab_init();

  kprintf("Loading MMU\n\r");
  set_initial_page_table(kernel_pt);

  kprintf("Enabling MMU\n\r");
  enable_mmu();
  kprintf("MMU enabled\n\r");

  kprintf("Setting up the stacks\n\r");
  setup_stacks();

  kprintf("Initializing kmalloc\n\r");
  kmalloc_init();

  kprintf("Initializing block devices\n\r");
  block_dev_init(block_dev_list);

  kprintf("Initializing process table\n\r");
  process_table_init();                     

  kprintf("Setting up taska\n\r");
  taska_stack = kmalloc(1024);
  taska_ptr = process_create(0, &taska, taska_stack);

  kprintf("Setting up taskb\n\r");
  taskb_stack = kmalloc(1024);
  taskb_ptr = process_create(0, &taskb, taskb_stack);

  kprintf("Setting up taskc\n\r");
  taskc_stack = kmalloc(1024);
  taskc_ptr = process_create(0, &taskc, taskc_stack);

  kprintf("Setting up interrupt controller\n\r");
  setup_interrupt_controller();                  

  kprintf("Enabling interrupts\n\r");
  enable_interrupts();

  kprintf("Starting system timer\n\r");
  start_timer();

  kprintf("Hello out there!\n\r");
  kprintf("Please type something to check if I can read it.\n\r");

  i=0;
  char_read(console_major,console_minor,&(buffer[i]),1);
  while((buffer[i]!='\r')&&(i<BUFF_LEN-1))
    char_read(console_major,console_minor,&(buffer[++i]),1);
  buffer[i]=0;
  kprintf("\nYou typed \'%s\' \n\r",buffer);

  kprintf("taska_ptr=%X\r\n",taska_ptr);
  kprintf("taskb_ptr=%X\r\n",taskb_ptr);
  kprintf("taskc_ptr=%X\r\n",taskc_ptr);
  
  while(1);
}
