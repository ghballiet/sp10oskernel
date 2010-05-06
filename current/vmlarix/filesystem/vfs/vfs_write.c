
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
#define char_write(a,b,c,d) printf(c,d)
#endif

#include <vfs.h>
#include <stddef.h>
#include <blkdev.h>
#include <vfs_mp.h>
#include <vfs_filedesc.h>
#include <vfs_fsops.h>

int32_t vfs_write(int32_t fd, void* buffer, size_t count)
{
  filedesc *f;
  int rval;
  if(fd>=NUM_FD)
    return -1;
  f = fdptr(fd);

  /* add code to handle devices ! */
  switch(f->type)
    {
    case FT_NORMAL:
      rval = f->mp->ops->write_fn(f,buffer,count);
      break;
    case FT_DIR:
      kprintf("unimplemented file type in vfs_write\r\n");
      rval= -1;
      break;
    case FT_CHAR_SPEC:
      rval = char_write(f->major,f->minor,buffer,count);
      break;
    case FT_BLOCK_SPEC:
      kprintf("unimplemented file type in vfs_write\r\n");
      rval= -1;
      break;
    case FT_PIPE:
      kprintf("unimplemented file type in vfs_write\r\n");
      rval= -1;
      break;
    case FT_SOCKET:
      kprintf("unimplemented file type in vfs_write\r\n");
      rval= -1;
      break;
    default: 
      kprintf("unknown file type in vfs_write\r\n");
      rval= -1;
    }     
  return rval;
}


