
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

int vfs_fchown(int fd, uid_t owner, gid_t group)
{
  /* get file descriptor pointer */
  filedesc *f = fdptr(fd);
  if(f) {
    return f->mp->ops->fchown_fn(f, owner, group);
  } else {
    return -1;
  }
}




