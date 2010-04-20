
#include <syscalls.h>
#include <misc.h>
#include <printf.h>
#include <fcntl.h>

int main()
{
  unsigned curchar;
  int fd,fd2,fd3,i,num;
  char c;
  char buf[bufsize];
  int charcount;

  fd = open("/dev/console",0,0);
  printf("Task A opened file with fd %d\n\r",fd);


  /* note: the print out of this file looked bad, because our serial port
     wants \n\r, and /test.c only has \n at the end of each line.  I
     hacked the serial driver to send \n\r whenever it sees \n */

  curchar = 0;
   printf("task C about to fork\n");
  if((pid = fork())==0)
    {
      printf("I am the child. Fork returned %d to me.\n",pid);
      while(1)
	printf("Task C child: %d\n\r",curchar++);


    }
  else
    {
      printf("I am the parent. Fork returned %d to me.\n",pid);
      while(1)
	printf("Task C parent: %d\n\r",curchar++);

  }


  while(1);
}
