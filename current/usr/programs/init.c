#include <unistd.h>

//#include <syscalls.h>
#include <misc.h>
#include <printf.h>
#include <fcntl.h>

int main()
{
  int result;
  printf("Starting init...\n\r");

  /* this is where I would read /etc/rc* and do system initialization,
     by forking and initing shell scripts */

  /* This is where I would read /etc/inittab, and figure out
     which devices to open and start login sessions on. 
  */

  /* For our simple example, I will just start a login process
     giving it my stdin, stdout and stderr as it's own.
     When that child process exits, I will start another one.
     Here is how I do it:
  */

  while(1)
    {
      /* first, I call fork, creating a child process.  The only
	 difference between the parent and child, is the value
	 stored in the variable "result".  The parent has the
	 child's PID stored in result, and the child has zero.
	 If the fork fails, then result contiains something less
	 than zero.
      */
   printf("Forking the first time...\n\r");
      result = fork();
      while(result<0)
	{
	  printf("Fork failed: code is %d\n\r",result);
	}
      
      if(result>0)
	{
	  /* this is where I would do a wait() for the 
	     child, or just add the child to my list and
	     continue checking on all my children.
	  */
	  
     printf("Normally, I would wait here...\n\r");
	  /* wait(result,&status, 0) */
	  while(1)
	    printf("exec:parent is running\n\r");
	}
      else
	{
	  /* this is where I would execve the login program */

    printf("Normally, I would login here...\n\r");
	  /* execve("/login",NULL,NULL); */
	  while(1)
	    printf("child is running\n\r");
	}
    }
     
  printf("Something is wrong with init\n\r"); 
	     
}
