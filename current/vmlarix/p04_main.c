
#include <sys/types.h>
#include <kprintf.h>
#include <phys_mem.h>
#include <dev_list.h>
#include <mmu.h>
#include <slabs.h>
#include <linker_vars.h>
#include <kmalloc.h>

uint16_t console_major;
uint16_t console_minor;


void kmalloc_test()
{
  int *a,*b,*c,**a2,i,freed;
  
  a = kmalloc(sizeof(int));
  *a=1000;                 
  c = kmalloc((*a)*sizeof(int));
  kprintf("Size of a and c %d\n\r",10*sizeof(int));
  kprintf(" value of a %d\n\r",*a);
  kprintf("%X\n\r",a);
  kprintf("%X\n\r",c);

  a2=(int**)c;
  for(i=0;i<*a;i++)
    {
      *a2=kmalloc(2048);
      if(*a2==NULL) kprintf("Iteration: %d/%d got a NULL address\n\r",i+1,*a);
      a2+=sizeof(int);
      if(i%100==0) kprintf("Iteration: %d/%d\n\r",i+1,*a);
    }
  kprintf("Cleared the first one.\n\r");
  a2=(int**)c;
  for(i=0;i<*a;i++)
    {
      kfree(*a2);
      a2+=sizeof(int);
      if(i%100==0) kprintf("Iteration: %d/%d\n\r",i+1,*a);
    }
  kprintf("Cleared the second loop.\n\r");
  b = kmalloc(4096);
  kprintf("%X\n\r",b);
  kfree(a);
  kfree(b);
  kfree(c);

  freed=kmalloc_free_some_pages();
  kprintf("I freeded the people and %d pages\n\r",freed);
  /* do a lot more testing here */

}

#define BUFF_LEN 100
int main()
{
  uint32_t i;
  uint32_t j;
  uint32_t pages_free;
  char buffer[BUFF_LEN];
  phys_mem_t kernel_pt;

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

  kprintf("Testing kmalloc\n\r");
  kmalloc_test();

  kprintf("Hello out there!\n\r");
  kprintf("Please type something to check if I can read it.\n\r");

  i=0;
  char_read(console_major,console_minor,&(buffer[i]),1);
  while((buffer[i]!='\r')&&(i<BUFF_LEN-1))
    char_read(console_major,console_minor,&(buffer[++i]),1);
  buffer[i]=0;
  kprintf("\nYou typed \'%s\' \n\r",buffer);

  while(1);
}
