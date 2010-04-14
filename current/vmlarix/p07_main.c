
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
#include <vfs.h>

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

  char c[50];
  int count;
  int fd = vfs_open("/create_ramdisk.c",O_RDONLY,0);
  kprintf("%X\n\r",fd);
  while((count=vfs_read(fd,&c,50*sizeof(char)))==(50*sizeof(char)))
    {
      for(j=0;j<50;j++)
        {
          if(c[j]=='\n')
            kprintf("\n\r");
          else
            kprintf("%c",c[j]);
        }
    }
  for(j=0;j<count;j++)
    {
      if(c[j]=='\n')
        kprintf("\n\r");
      else
        kprintf("%c",c[j]);
    }
  vfs_close(fd);

  /* TODO: chmod/chown/fstat testing */


  kprintf("\r\nlseek testing:\r\n");
  /* NOTE: sfs_open seems to be having trouble creating new files, this call is
     getting stuck in the if(result<0) bit at the end of vfs_open */
  /* so instead I'm just truncating the file we know exists */
  int fd2 = vfs_open("/create_ramdisk.c", O_RDWR & O_TRUNC,0);
  char *str = "hello";
  vfs_write(fd2, str, 5*sizeof(char));
  vfs_write(fd2, str, 5*sizeof(char));
  /* first, test jumping back to the beginning of the file */
  vfs_lseek(fd2, 0, SEEK_SET);
  char readbuf[8];
  vfs_read(fd2, &readbuf, 7*sizeof(char));
  kprintf("Read back 7 chars from start of file:\r\n   %s\r\n", &readbuf);
  /* now, test jumping to a position in the file */
  vfs_lseek(fd2, 3, SEEK_SET);
  vfs_read(fd2, &readbuf, 7*sizeof(char));
  kprintf("Read back 7 chars from position 3:\r\n   %s\r\n", &readbuf);
  /* now, test jumping to a position after the end of the file */
  vfs_lseek(fd2, 15, SEEK_SET);
  kprintf("Wroting another copy of str to position 15\r\n");
  vfs_write(fd2, str, 5*sizeof(char));
  char readbuf2[21];
  vfs_lseek(fd2, 0, SEEK_SET);
  vfs_read(fd2, &readbuf2, 20*sizeof(char));
  kprintf("Read back 20 chars from position 0:\r\n   %s\r\n", &readbuf);


  /* TODO: test other 'whence' modes of lseek (only the position calculation
     logic differs for them, the seeking logic is the same for all methods
     after calculating the new position) */

  /* TODO: test the block-position stuff in lseek -- we'll need a test file
     bigger than a single block for this */

  kprintf("Entering idle loop\n\r");

  while(1);

}
