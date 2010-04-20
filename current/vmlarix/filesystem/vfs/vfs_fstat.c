
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
  //kprintf("vfs_fstat() has been implemented!\n\r");
    
  kprintf("vfs_fstat has been implemented. \n\r");
  filedesc *f;
  if(fd>=NUM_FD)
    return -1;
  f = fdptr(fd);
  
  mount_point *mp;
  if((mp = vfs_lookup("/"))==NULL)
    {
      /* errno = EFNF; file not found */
      kprintf("no mp found\n\r");
      return -1;
    }
  //path += strlen(mp->target); /* strip off the mount point */

  f->mp = mp; 

  /* add code to handle devices ! */
  return mp->ops->fstat_fn(f,buf);

    //kprintf("vfs_fstat() not implemented!\n\r");
}
