
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

int vfs_open_dev(int16_t major, int16_t minor,int32_t mode, uint32_t flags)
{
  /* find a file descriptor */
  int i = 0;
  while((i<NUM_FD)&&(fdesc[i].in_use))
    i++;
  if(i==NUM_FD)
    {
      /* errno = ENFD; no file descriptors */
      return -1;
    }
  fdesc[i].in_use = 1;

  fdesc[i].mp = NULL;
  // fdesc[i].sb = NULL;
  // fdesc[i].inode = NULL;
  fdesc[i].flags = flags;
  fdesc[i].mode = mode;
  fdesc[i].major = major;
  fdesc[i].minor = minor;
  fdesc[i].buffer = NULL;
  fdesc[i].bufsize = 0;
  fdesc[i].dirty = 0;
  fdesc[i].curr_blk = 0;
  fdesc[i].curr_log = 0;
  fdesc[i].bufpos = 0;
  fdesc[i].filepos = 0;
  fdesc[i].type = FT_CHAR_SPEC;
  return i;
}

int vfs_open(char *pathname, int flags, mode_t mode)
{
  int fd,result;
  /* find the mount point */
  mount_point *mp;
  if((mp = vfs_lookup(pathname))==NULL)
    {
      /* errno = EFNF; file not found */
      return -1;
    }
  pathname += strlen(mp->target); /* strip off the mount point */

  /* find a file descriptor */
  if((fd = alloc_fd())<0)
    return fd;
  filedesc *f = fdptr(fd);

  /* call the correct filesystem function to open the file 
     it will fill in all the file descriptor data in f */
  result = mp->ops->open_fn(mp,f,pathname,flags,mode);
  if(result<0)
    {
      free_fd(fd);
      return result;
    }
  return fd;
}

