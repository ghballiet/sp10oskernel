
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

int vfs_unlink(char *path)
{
  int result;
  mount_point *mp;
  
  if ((mp = vfs_lookup(path))==NULL) {
    // errno = ERNF; file not found
    return -1;
  }
  
  path += strlen(mp->target); // strip off the mount point
  
  
}
