
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
  kprintf("vfs_fchown() NOW implemented!\n\r");

  filedesc *f;
  if(fd>=NUM_FD)
    return -1;
  f = fdptr(fd);

  /* add code to handle devices ! */
  return f->mp->ops->fchown_fn(f,owner,group);

  //kprintf("vfs_fchown() not implemented!\n\r");
}




