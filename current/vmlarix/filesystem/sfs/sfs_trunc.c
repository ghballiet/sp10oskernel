
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
  sfs_inode *inode = sfs_inode_from_fd(f);
  uint32_t num_blocks = sfs_get_num_blocks(f);
  uint32_t i = num_blocks;
  while(i>0) {
    i--;
    sfs_del_phys(f, i);
  }
  /* update filedescriptor */
  f->dirty=0;
  f->curr_blk = 0; /* is this right? */
  /* should we clear the buffer too? */
  f->curr_log = 0;
  f->bufpos = 0;
  f->filepos=0;
  /* finally, update inode to have size 0, and rewrite inode to disk */
  inode->size = 0;
  uint32_t inum = ((sfs_fd_private *)f->fs_private)->inum;
  sfs_put_inode(f->mp, inum, inode);
}


