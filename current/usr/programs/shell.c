
#include <syscalls.h>
#include <misc.h>
#include <printf.h>
#include <fcntl.h>
#include <stdio.h>

#define BUFSIZE 1024
#define CTRLD 0x04

int main()
{
  char command[BUFSIZE+16];
  int done;
  int i;
  printf("Hello.  I am your friendly shell!\n\r");

  done=0;
  while(!done)
    {
      printf("type a shell command: \n\r");
      done=0;
      /* look for a \n character, or something that is too long */
      for(i=0;(i<BUFSIZE)&&!done;i++)
	{
	  read(STDIN,command+i,1);
	  if(command[i]=='\n')
	    done = 1;
	  if(command[i]=='\r')
	    i--;      
	}
      if(i>=BUFSIZE)
	exit(-1);
      
      if(!strncmp(command,"exit",4))
	done=1;
      
      if(command[0]==CTRLD)
	done=1;

      if(!done)
	{
	  printf("I'm a really stupid shell.  I only understand\n\r");
	  printf("One command: 'exit' (or ctrl-D)\n\r");
	}

    }
  exit(0);

}
