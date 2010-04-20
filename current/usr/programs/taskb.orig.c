
#include <syscalls.h>
#include <misc.h>
#include <printf.h>
#include <fcntl.h>

int main()
{
  unsigned curchar;
  int fd,fd2;


  fd = open("/dev/console",O_RDWR,0666);


  /* taskb will repeat the next line until taska creates the file */
  while((fd2 = open("/readyfile",O_RDONLY,0))<0);

  printf("Task B opened file with fd %d\n\r",fd2);

  write(0,"hello from task b\n\r",strlen("hello from task b\n\r"));

  curchar = 0;
  while(1)
    printf("Task B: %d\n\r",curchar++);

/*   close(fd); */

  while(1);
}
