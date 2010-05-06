
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
#include <fcntl.h>
#include <elf_load.h>
#include <vfs_filedesc.h>

uint16_t console_major;
uint16_t console_minor;

uint16_t initrd_major;
uint16_t initrd_minor;

#define BUFF_LEN 100
int main()
{
  uint32_t i;
  uint32_t j;
  uint32_t pages_free;
  char buffer[BUFF_LEN];
  phys_mem_t kernel_pt;
  proc_rec *taska_ptr,*taskb_ptr;
  char *taska_stack,*taskb_stack;
  int32_t res;

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

  kprintf("Initializing virtual filesystem\n\r");
  vfs_init();

  kprintf("Mounting initial RAMdisk as root filesystem....");
  res = vfs_mount_dev(initrd_major, initrd_minor, "/","sfs",0,NULL);
  if(res < 0)
    panic("Failed!\n\r");

  kprintf("Opening console device\n\r");
  int stdin = vfs_open_dev(console_major, console_minor, O_RDONLY, 0);
  kprintf("stdin=%d\n\r",stdin);
  int stdout = vfs_open_dev(console_major, console_minor, O_WRONLY, 0);
  kprintf("stdout=%d\n\r",stdout);
  int stderr = vfs_open_dev(console_major, console_minor, O_WRONLY, 0);
  kprintf("stderr=%d\n\r",stderr);

  /* Look at p06_main.c to get an idea of how to do the following: 
     Call elf loader to read in taska 
     Call elf loader read in taskb 
     set up the tasks to run 
     start the system timer 
     You can put the program stacks at (entry_address + 128K)-4bytes
  */
  kprintf("Setting up taska\n\r");
  void *taska=elf_load("/taska");
  taska_stack = taska+32768;
  taska_ptr = process_create(0, taska, taska_stack);
  taska_ptr->fd[0]=stdin;
  fdesc[stdin].in_use++;
  taska_ptr->fd[1]=stdout;
  fdesc[stdout].in_use++;
  taska_ptr->fd[2]=stderr;
  fdesc[stderr].in_use++;


  kprintf("Setting up taskb\n\r");
  void *taskb=elf_load("/taskb");
  taskb_stack = taskb+32768;
  taskb_ptr = process_create(0, taskb, taskb_stack);
  taskb_ptr->fd[0]=stdin;
  fdesc[stdin].in_use++;
  taskb_ptr->fd[1]=stdout;
  fdesc[stdout].in_use++;
  taskb_ptr->fd[2]=stderr;
  fdesc[stderr].in_use++;

  kprintf("Setting up interrupt controller\n\r");
  setup_interrupt_controller();                  

  kprintf("Enabling interrupts\n\r");
  enable_interrupts();

  kprintf("Starting system timer\n\r");
  start_timer();

  kprintf("Entering idle loop\n\r");

  while(1);

}
