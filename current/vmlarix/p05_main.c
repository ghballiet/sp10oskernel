
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

uint16_t initrd_major;
uint16_t initrd_minor;

void ramdisk_test()
{
  /* #define TEST_SIZE nblocks */
#define TEST_SIZE 4
  int block_size=blk_size(initrd_major,initrd_minor);
  int nblocks=num_blk(initrd_major,initrd_minor);
  char *buffer;
  int *intbuf;
  int i,j;
  int result;

  kprintf("Ramdisk block size is 0x%X (%d) ",block_size,block_size);
  kprintf("and there are 0x%X (%d) blocks\n\r",nblocks,nblocks);
  
  if((buffer = kmalloc(block_size))==NULL)
    panic("Unable to allocate buffer");
  intbuf=(uint32_t)buffer;

  for(i=0;i < TEST_SIZE; i++)
    {
      kprintf("Initial read: block %X\n\r",i);
      if(block_read(initrd_major,initrd_minor,i,buffer,1)!=block_size)
	panic("Error in initial read.");
      for(j=0;j<block_size/4;j++)
	{
	  if(!( (j+1) % 8))
	    kprintf("%X\n\r",intbuf[j]);
	  else
	    kprintf("%X ",intbuf[j]);
	}
      kprintf("\n\r");

      for(j=0;j<block_size/4;j++)
	intbuf[j] += j;
      if(block_write(initrd_major,initrd_minor,i,buffer,1)!=block_size)
	panic("Error in write.");
    }

  for(i=0;i < TEST_SIZE; i++)
    {
      kprintf("Second read: block %X\n\r",i);
      if(block_read(initrd_major,initrd_minor,i,buffer,1)!=block_size)
	panic("Error in second read.");
      for(j=0;j<block_size/4;j++)
	{
	  if(!( (j+1) % 8))
	    kprintf("%X\n\r",intbuf[j]);
	  else
	    kprintf("%X ",intbuf[j]);
	}
      kprintf("\n\r");
    }
  kprintf("Attempting to read a non-existent block.\n\r");
  if((result=block_read(initrd_major,initrd_minor,nblocks,buffer,1))>=0)
    panic("Test failed");
  kprintf("Read returned %d\n\r",result);  

  kprintf("Attempting to write a non-existent block.\n\r");
  if((result=block_write(initrd_major,initrd_minor,nblocks,buffer,1))>=0)
    panic("Test failed");
  kprintf("Write returned %d\n\r",result);  
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

  kprintf("Initializing block devices\n\r");
  block_dev_init(block_dev_list);

  kprintf("Testing ramdisk\n\r");
  ramdisk_test();

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
