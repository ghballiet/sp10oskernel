
#include <sys/types.h>
#include <kprintf.h>
#include <phys_mem.h>
#include <dev_list.h>
#include <mmu.h>
#include <linker_vars.h>

uint16_t console_major;
uint16_t console_minor;

phys_mem_t kernel_pt;

#define BUFF_LEN 100
int main()
{
  uint32_t i;
  uint32_t j;
  uint32_t pages_free;
  char buffer[BUFF_LEN];

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

  kprintf("Loading MMU\n\r");
  set_initial_page_table(kernel_pt);

  kprintf("Enabling MMU\n\r");
  enable_mmu();

  kprintf("MMU enabled\n\r");

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
