
#include <dev_list.h>
#include <stddef.h>

#define UART_BASE (void *)0x80010000

int arch_init()
{ 
  console_major = 1;
  console_minor = 0;
}

devdef char_dev_list[]={
  {1,UART_BASE},
  {0,NULL},
};

devdef block_dev_list[]={
  {0,NULL}
};

