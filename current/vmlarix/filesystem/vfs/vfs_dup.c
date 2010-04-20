
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

int vfs_dup(int oldfd)
{
  kprintf("vfs_dup has been implemented\n\r");
  filedesc *f;
  //int fd;
   /* find a file descriptor */
  /*if((fd = alloc_fd())<0)
    return fd;*/
  f = fdptr(oldfd);
  mount_point *mp;
  if((mp = vfs_lookup("/"))==NULL)
  {
      /* errno = EFNF; file not found */
    kprintf("No mount point is found\n\r");
    return -1;
  }
  
  /*if(fd>=NUM_FD)
    return -1;
  f = fdptr(fd);*/

  /* add code to handle devices ! */
  f->mp = mp;
  
  return mp->ops->dup_fn(f);
    
   //kprintf("vfs_dup() not implemented!\n\r");
}
