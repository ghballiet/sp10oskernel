
#ifdef _KERNEL_
#include <misc.h>
#else
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define kmalloc malloc
#define kfree free
#define kprintf printf
#endif

#include <sfs.h>
#include <stddef.h>
#include <sys/types.h>
#include <fcntl.h>
#include <blkdev.h>
#include <vfs_filedesc.h>
#include <sfs_dir.h>
#include <sfs_private.h>
#include <byteswap.h>
#include <vfs_mp.h>

int sfs_creat(void *fs_private, char *path, mode_t mode)
{
  kprintf("sfs_creat() function not implemented\n\r");

  sfs_mp_private *p = (sfs_mp_private *)fs_private;
  /* NOTE: he said in class that a lot of the stuff we need to do here is
     already implemnted in 'open'. On first reading, it looks like open creates
     a file if we try to open a file that does not exist. */
  /* I.e., the 'find the parent directory path' code from 'open' will work here
     as-is */
}

