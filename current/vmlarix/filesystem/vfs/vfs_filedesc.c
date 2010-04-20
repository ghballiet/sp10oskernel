
#include <vfs_filedesc.h>
#include <misc.h>

void vfs_init_filedesc()
{
  int i;
  for(i=0;i<NUM_FD;i++)
    fdesc[i].in_use = 0;
}
  
filedesc *fdptr(uint32_t fd)
{
  if(fd>=NUM_FD)
    panic("Attempt to access illegal file descriptor");
  return &fdesc[fd];
}

int alloc_fd()
{
  int i;
  for(i=0;i<NUM_FD;i++)
    if(!fdesc[i].in_use)
      {
	fdesc[i].in_use = 1;
	return i;
      }
  return -1;
}

void free_fd(uint32_t fd)
{
  if(fd>=NUM_FD)
    panic("Attempt to free illegal file descriptor");
  fdesc[fd].in_use=0;
}
