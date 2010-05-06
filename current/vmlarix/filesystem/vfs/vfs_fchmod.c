
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

int vfs_fchmod(int fd, mode_t mode)
{
  kprintf("vfs_fchmod() has been implemented!\n\r");

  filedesc *f;
  if(fd>=NUM_FD)
    return -1;
  f = fdptr(fd);

  /* add code to handle devices ! */
  return f->mp->ops->fchmod_fn(f,mode);

  //kprintf("vfs_fchmod() not implemented!\n\r");
}




