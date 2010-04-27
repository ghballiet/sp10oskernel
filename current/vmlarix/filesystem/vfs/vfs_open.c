
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

int vfs_open_dev(uint16_t major, uint16_t minor,uint32_t mode, uint32_t flags) {
  kprintf("vfs_open_dev called\n\r");
  
  // find a file descriptor
  int i = 0; 
  while((i<NUM_FD)&&(fdesc[i].in_use)) 
   i++; 
 if(i==NUM_FD) 
   { 
     // errno = ENFD; no file descriptors 
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

// COMMENTS 04.22.10
// function vfs_open_dev should be used
// to create system file descriptors referring to 
// the console device. 
// i.e. during process setup, this function should
// initialize the first three file descriptors to
// refer to the console device, up to NUM_FD,
// the maximum number of file descriptors allocated to
// each process.