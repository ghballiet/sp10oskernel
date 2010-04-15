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

int sfs_unlink(void *fs_private, char *path)
{
  kprintf("sfs_unlink() function not implemented\n\r");
  /* Unlink algorithm from 4/8 notes:
     - Get the inode for this file

     - Find this file's entry in its parent directory

     - From the file inode, get the disk blocks to free, and set them to free
       in the free block bitmap

     - Free the inode itself (in the free inode bitmap)

     - Write the bitmaps back to disk

     - Finally, remove the file entry from its parent directory, shifting other
       file entries in that directory up
  */

  /* He recommends writings sfs_trunc first, and making a call to sfs_trunc to
     truncate the file down to 0 bytes here to handle freeing the disk
     blocks */

  /* To find the parent directory, see the path-parsing code in 'sfs_open' */
  
  /*
  Algorithm:
  
  1.  Get inode for file using sfs_get_inode.
  2.  Find the entry in the parent directory (using code from creat).
  3.  Free stuff on disc using sfs_trunc.
  4.  Remove entry from parent directory.
  */
  
  
  
}