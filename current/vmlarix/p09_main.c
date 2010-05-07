
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
  proc_rec *init_prec;
  char *init_stack;
  void* init_entry;
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

  kprintf("Setting up init\n\r");
  init_stack = (void*)0x7FFFFFFC;
  /* note: in process_create, the entry point is set to zero.  we rely
     on elf_load to provide the correct entry point.
  */
  
  kprintf("Calling process_create...\n\r");
  init_prec = process_create(0, (void*)0, init_stack);
  // TODO: doesn't look like it's getting beyond this point
  kprintf("Got beyond process_create...\n\r");
  init_prec->page_table = pt_new();
  init_prec->page_table_virt = (void*)phys_to_virt(kernel_pt,init_prec->page_table);
  init_prec->fd[0]=stdin;
  fdesc[stdin].in_use++;
  init_prec->fd[1]=stdout;
  fdesc[stdout].in_use++;
  init_prec->fd[2]=stderr;
  fdesc[stderr].in_use++;
  kprintf("ELF load init...\n\r");
  init_entry = elf_load("/init",init_prec);
  /* reset the entry point to the one that the elf loader gave us.
     this routine has been written in arch/ARM/SA1110/process_arch.c,
     but has not been tested.
  */
  kprintf("Resetting entry point to %d\n\r...",init_entry);
  process_arch_set_entry(init_prec->arch,init_entry);

  kprintf("Setting up interrupt controller\n\r");
  setup_interrupt_controller();                  

  kprintf("Enabling interrupts\n\r");
  enable_interrupts();

  kprintf("Starting system timer\n\r");
  start_timer();

  kprintf("Entering idle loop\n\r");

  while(1);

}
