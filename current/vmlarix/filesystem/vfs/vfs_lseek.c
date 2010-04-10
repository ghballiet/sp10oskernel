
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

int vfs_lseek(int fd, off_t offset, int whence)
{
  /* get file descriptor pointer */
  filedesc *f = fdptr(fd);
  if(f) {
    return f->mp->ops->lseek_fn(f, offset, whence);
  } else {
    return -1;
  }
}




