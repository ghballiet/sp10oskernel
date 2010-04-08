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

int sfs_fchmod(filedesc *f, mode_t mode)
{
  /* update mode in file descriptor */
  f->mode = mode;
  /* get inode pointer and inode number from file descriptor */
  sfs_inode_t *inode = ((sfs_fd_private *)f->fs_private)->inode;
  uint32_t inum = ((sfs_fd_private *)f->fs_private)->inum;
  /* update mode in inode structure */
  inode->perm = mode;
  /* write updated inode to disk */
  sfs_put_inode(f->mp, inum, inode);
  return 0; /* TODO: what are these supposed to return? 0 for success, -1 for
	       failure? What would be a failure case for chmod? */
}
