
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
  /* get file descriptor pointer */
  filedesc *f = fdptr(fd);
  if(f) {
    return f->mp->ops->fchmod_fn(f, mode);
  } else {
    return -1;
  }
}




