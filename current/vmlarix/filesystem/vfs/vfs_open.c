
#include <stddef.h>
#include <vfs.h>
#include <vfs_filedesc.h>
#include <vfs_fsops.h>
#include <kprintf.h>

#ifdef _KERNEL_
#include <misc.h>
#else
#include <stdlib.h>
#include <string.h>
#define kmalloc malloc
#endif

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

int vfs_open_dev(uint16_t major, uint16_t minor,uint32_t flags,uint32_t mode) {
  // called with console_major and console_minor
  int fd;
  
  // find a file descriptor
  if((fd = alloc_fd())<0)
    return fd;
  filedesc *f = fdptr(fd);
  
  kprintf("FD allocated: %d\n\r",fd);
  
  f->in_use = 1; 
  f->mp = NULL; 
  // fdesc[i].sb = NULL; 
  // fdesc[i].inode = NULL; 
  f->flags = flags; 
  f->mode = mode; 
  f->major = major; 
  f->minor = minor; 
  f->buffer = NULL; 
  f->bufsize = 0; 
  f->dirty = 0; 
  f->curr_blk = 0; 
  f->curr_log = 0; 
  f->bufpos = 0; 
  f->filepos = 0; 
  f->type = FT_CHAR_SPEC; 
  
  return fd;
} 