
#include <stddef.h>
#include <vfs.h>
#include <vfs_filedesc.h>
#include <vfs_fsops.h>

#ifdef _KERNEL_
#include <misc.h>
#else
#include <stdlib.h>
#include <string.h>
#define kmalloc malloc
#endif

int vfs_close(int fd)
{ 
  /* add code to take care of devices ! */

  int res;
  filedesc *f = fdptr(fd);
  if((res = (*f->mp->ops->close_fn)(f)) < 0)
    return res;
  free_fd(fd);
  return 0;
}



