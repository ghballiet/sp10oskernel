
#include <sfs.h>
#include <blkdev.h>
#include <vfs_mp.h>
#include <stddef.h>
#include <sfs_superblock.h>
#include <byteswap.h>
#include <sfs_private.h>


#ifdef _KERNEL_
#include <misc.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define kmalloc malloc
#define kfree free
#define kprintf printf
#endif


int sfs_umount(void *fs_private)
{
  kprintf("sfs_umount() not implemented\n\r");
}

