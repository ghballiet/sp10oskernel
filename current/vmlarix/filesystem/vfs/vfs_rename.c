
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

int vfs_rename(const char *oldpath, const char *newpath)
{
  
   kprintf("vfs_rename() has been implemented!\n\r");
   int fd,result;
  /* find the mount point */
   mount_point *mp;
  if((mp = vfs_lookup(oldpath))==NULL)
    {
      /* errno = EFNF; file not found */
      kprintf("No mountpoint is found\n\r");
      return -1;
    }
   oldpath += strlen(mp->target); /* strip off the mount point */

  /* find a file descriptor */
  /*if((fd = alloc_fd())<0)
    return fd;
  filedesc *f = fdptr(fd);*/
  /* add code to handle devices ! */
   return mp->ops->rename_fn(mp,oldpath,newpath);
   //return f->mp->ops->rename_fn(f,oldpath,newpath);
    //kprintf("vfs_rename() not implemented!\n\r");
}
