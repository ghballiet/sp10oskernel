
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

int vfs_fstat(int fd, struct fstat *buf)
{
  /* get file descriptor pointer */
  filedesc *f = fdptr(fd);
  if(f) {
    return f->mp->ops->fstat_fn(f, buf);
  } else {
    return -1;
    /* TODO: check on if this is what this is supposed to do... */
  }
}
