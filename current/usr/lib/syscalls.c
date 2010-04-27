
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/syscall.h>

int brk(void *end_data_segment)
{
return syscall(SYS_brk,end_data_segment);
}

ssize_t read(int fd, const void *buf, size_t count)
{
  return (ssize_t) syscall(SYS_read,fd,buf,count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
  printf("!!!!!!! - WRITE!\n\r")
  return (ssize_t) syscall(SYS_write,fd,buf,count);
}

int open(const char *pathname, int flags, mode_t mode)
{
   return syscall(SYS_open,pathname, flags, mode);
}

int close(int fd)
{
   return syscall(SYS_close,fd);
}

int fork()
{
  return syscall(SYS_fork);
}

int execve(const char *filename, char *const argv[],char *const envp[])
{
  return syscall(SYS_execve,filename,argv,envp);
}


/* int open(const char *pathname, int flags) */
/* { */
/*   mode_t mode; */
/*   mode = 0x777; /\* actually should get the current umask *\/ */
/*   return syscall(SYS_open,pathname, flags, mode); */
/* } */



