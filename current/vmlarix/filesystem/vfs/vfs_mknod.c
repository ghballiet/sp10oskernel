
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

int vfs_mknod(char *path, mode_t mode, dev_t dev)
{
  kprintf("vfs_mknod() not implemented!\n\r");
}




