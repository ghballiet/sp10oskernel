
#include <sys/types.h>
#include <kprintf.h>
#include <phys_mem.h>
#include <dev_list.h>

uint16_t console_major;
uint16_t console_minor;

#define MAX_PAGES 8000
//static phys_mem_t page_ptrs[MAX_PAGES];
void test_physmem()
{
  /* do a lot of testing of:  
     phys_mem_get_pages()
     phys_mem_free_page()
     phys_mem_count_free() 

     use kprintf and/or the debugger
  */

  phys_mem_t p1,p2,p3,p4,p5;
  int original_count;
  int i;

  original_count = phys_mem_count_free();
  kprintf("%d pages free\n\r",phys_mem_count_free());

  p1=phys_mem_get_pages(4,1);
  kprintf("got 4 pages starting at address %X\n\r",p1);

  p2=phys_mem_get_pages(4,2);
  kprintf("got 4 pages starting at address %X\n\r",p2);

  p3=phys_mem_get_pages(4,3);
  kprintf("got 4 pages starting at address %X\n\r",p3);

  p4=phys_mem_get_pages(4,4);
  kprintf("got 4 pages starting at address %X\n\r",p4);

  p5=phys_mem_get_pages(4,5);
  kprintf("got 4 pages starting at address %X\n\r",p5);

  phys_mem_free_page(p3,4);
  kprintf("freed 4 pages starting at address %X\n\r",p3);

  kprintf("%d pages free\n\r",phys_mem_count_free());

  p3=phys_mem_get_pages(4,1);
  kprintf("got 4 pages starting at address %X\n\r",p3);

  kprintf("%d pages free\n\r",phys_mem_count_free());

  /* Allocate one page at a time, until we run out of free memory */
  i=0; 
  while(phys_mem_count_free()>0)    
    {
      p3=phys_mem_get_pages(1,1);
      if(((i % 256)==0)||(p3 & 1)||(phys_mem_count_free()<50))
	kprintf("i=%d Address=%X  free=%d\n\r",i,p3,phys_mem_count_free());
      i++;
    }
  
  kprintf("got %d pages, one at a time.  Last Address=%X\n\r",i,p3);
  p3=phys_mem_get_pages(1,1);
  kprintf("tried to get another page. Address=%X\n\r",p3);      

  phys_mem_free_page(p2,4);
  kprintf("freed 4 pages starting at address %X\n\r",p2);

  p2=phys_mem_get_pages(2,1);
  kprintf("got 2 pages starting at address %X\n\r",p2);
}

#define BUFF_LEN 100
int main()
{
  uint32_t i;
  uint32_t j;
  uint32_t pages_free;


  char buffer[BUFF_LEN];

  arch_init();

  char_dev_init(char_dev_list);

  kprintf("Initializing memory\n\r");
  phys_mem_init(); 
  test_physmem();

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
