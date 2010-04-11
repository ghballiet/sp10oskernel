
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

/* truncate file to zero length */
sfs_trunc(filedesc *f)
{
  kprintf("sfs_trunc() function not implemented\n\r");
  /* Inefficient basic solution for this:

     - Get the number of blocks in the file

     - Call sfs_del_phys to remove blocks one at a time, starting with the last
       block (sfs_del_phys takes a filedesc *f and a logical block uint32_t)
  */
  /* sfs_del_phys looks like it *should* delete indirect blocks as appropriate,
     but he's not sure if that's currently implemented fully or not */
}


