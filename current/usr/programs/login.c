
#include <syscalls.h>
#include <misc.h>
#include <printf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 128
int main()
{
  int done;
  int i;
  char username[BUFSIZE];
  char password[BUFSIZE];

  /* my job is to read the username and password, check them
     against the system database, and either give them their
     shell or exit.
  */

  printf("Login: \n\r");
  done=0;
  /* look for a \n character, or something that is too long */
  for(i=0;(i<BUFSIZE)&&!done;i++)
    {
      read(STDIN,username+i,1);
      if(username[i]=='\n')
	done = 1;
      if(username[i]=='\r')
        i--;      
    }
  if(i>=BUFSIZE)
    exit(-1);
  
  printf("password: \n\r");
  done=0;
  /* look for a \n character, or something that is too long */
  for(i=0;(i<BUFSIZE)&&!done;i++)
    {
      read(STDIN,password+i,1);
      if(password[i]=='\n')
	done = 1;
      if(password[i]=='\r')
        i--;      
    }
  if(i>=BUFSIZE)
    exit(-1);

  /* this is where I would look up the username in the system 
     database, and check the password.  If their password does
     not mach, then I exit.
     Otherwise, I exec the shell listed in the system database.
  */
  
  execve("shell",NULL,NULL);

  /* if all went well, then I no longer exist.  I have been replaced
     by the shell.
  */

  exit(-2);  
}
