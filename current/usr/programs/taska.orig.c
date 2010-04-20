
#include <syscalls.h>
#include <misc.h>
#include <printf.h>
#include <fcntl.h>

#define bufsize 32

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
  fd2 = open("/test.c",O_RDONLY,0);
  printf("Task A opened \"/test.c\" with fd %d\n\r",fd2);  
  while((charcount = read(fd2,&buf,bufsize))>0)
    write(fd,&buf,charcount);

  /* create a file to let task b know that it can continue */
  fd3 = open("/readyfile",O_WRONLY|O_CREAT,0666);

  curchar = 0;
  while(1)
    printf("Task A: %d\n\r",curchar++); 
  

/* /\*   num = close(fd2); *\/ */
/*   printf("Task A closed file with fd %d: return code was %d\n\r",fd2,num);  */
      

    
/*   close(fd); */

  while(1);
}
