
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

int vfs_lseek(int fd, off_t offset, int whence)
{
  /* get file descriptor pointer */
  filedesc *f = fdptr(fd);
  if(f) {
    int x = f->mp->ops->lseek_fn(f, offset, whence);
    kprintf("vfs_lseek: curr_log=%d\r\n", f->curr_log);
    kprintf("vfs_lseek: bufpos=%d\r\n", f->bufpos);
    kprintf("vfs_lseek: filepos=%d\r\n", f->filepos);
    return x;
  } else {
    return -1;
  }
}




