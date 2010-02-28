
#include <stdio.h>
#include <kprintf.h>

int main()
{
  char buf[101];
  char count;

  buf[100] = 0;

  if(ksnprintf(buf,100,"THIS IS A TEST %u %lu %c %s",18910,(long long) -10, 
	       'Z', "This is the string!")>=100)
    printf("string too long: %s|\n",buf);
  else
    printf("%s|\n",buf);
    

}
