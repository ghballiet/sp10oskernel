
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

int vfs_creat(char *path, mode_t mode)
{
  int fd,result;
  
  mount_point *mp;
  
  if((mp = vfs_lookup(path))==NULL)
    {
      /* errno = EFNF; file not found */
      return -1;
    }
  path += strlen(mp->target); /* strip off the mount point */
  
  /* find a file descriptor */
  if((fd = alloc_fd())<0)
    return fd;
  filedesc *f = fdptr(fd);
  
  result = mp->ops->creat_fn(mp,f,path,mode);
  if(result < 0) {
    free_fd(fd);
    return result;
  }
  return fd;
}



