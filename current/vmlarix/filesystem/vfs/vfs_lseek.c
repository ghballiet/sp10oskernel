
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
  kprintf("vfs_lseek() not implemented!\n\r");
}




