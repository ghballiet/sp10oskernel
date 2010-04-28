
#include <syscalls.h>
#include <misc.h>
#include <printf.h>
#include <fcntl.h>

int main()
{
  unsigned int curchar;
  curchar = 0;
  while(1)
    printf("Task A: %d\n\r",curchar++); 
  
}
