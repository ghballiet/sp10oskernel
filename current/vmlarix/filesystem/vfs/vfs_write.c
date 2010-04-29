
#ifdef _KERNEL_
#include <misc.h>
#include <chardev.h>
#else
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define kmalloc malloc
#define kfree free
#define kprintf printf
#endif

#include <vfs.h>
#include <stddef.h>
#include <blkdev.h>
#include <vfs_mp.h>
#include <vfs_filedesc.h>
#include <vfs_fsops.h>

int vfs_write(int fd, void* buffer, size_t count)
{
  filedesc *f;
  int rval;
  if(fd>=NUM_FD)
    return -1;
  f = fdptr(fd);

  /* add code to handle devices ! */
  
  switch(f->type) {
    case FT_NORMAL:
      rval = (f->mp->ops->write_fn(f,buffer,count);
      break;
    case FT_CHAR_SPEC:
      rval = char_write(f->major,f->minor,buffer,count);  
      break;
    case default:
      kprintf("Uinimplemented file type.\n\r");
      break;
  }
  
  return rval;
}