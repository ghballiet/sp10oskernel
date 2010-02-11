
#include <sys/types.h>
#include <kprintf.h>

#include <dev_list.h>

uint16_t console_major;
uint16_t console_minor;

void test_physmem()
{
  /* do a lot of testing of:  
     phys_mem_get_pages()
     phys_mem_free_page()
     phys_mem_count_free() 

     use kprintf and/or the debugger

  */

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
