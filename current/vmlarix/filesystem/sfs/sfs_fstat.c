#ifdef _KERNEL_
#include <misc.h>

#include "kprintf.h"
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

int sfs_fstat(filedesc *f, struct fstat *buf)
{
  kprintf("sfs_fstat() has been implemented\n\r");
  
  mount_point *mp;
  sfs_fd_private *sfs_fd;
  sfs_inode_t *inode;
  sfs_superblock *sb;

  if(f->mp == NULL)
  {
      kprintf("Mount Point is null\n\r");
      return -1;
  }
  else if(f->mp != NULL)
  {    
      mount_point *mp = f->mp;
  }

  if(mp->fs_private == NULL)
  {
      kprintf("SFS_FD_PRIVATE is null\n\r");
      return -1;
  }
  else if(mp->fs_private != NULL)
  {
      sfs_fd_private *sfs_fd = (sfs_fd_private *)f->fs_private;      
  }

  if(sfs_fd->inode == NULL)
  {
      kprintf("INODE is NULL\n\r");
      return -1;
  }
  else if(sfs_fd->inode != NULL)
  {
    sfs_inode_t *inode = sfs_fd->inode;
  }

  if(sfs_fd->sb == NULL)
  {
      kprintf("SUPERBLOCK is NULL\n\r");
      return -1;
  }
  else if(sfs_fd->sb != NULL)
  {
      sfs_superblock *sb = sfs_fd->sb;
  }

  buf->st_atime = inode->atime;
  kprintf("ST_TIME: %d\n\r",inode->atime);
  buf->st_blksize = sb->block_size;
  kprintf("Block_Size: %d\n\r",sb->block_size);
  buf->st_blocks = sb->num_blocks;
  kprintf("Number of blocks: %d\n\r",sb->num_blocks);
  buf->st_ctime = inode->ctime;
  kprintf("CTIME: %d\n\r",inode->ctime);
  buf->st_dev;
  buf->st_gid = inode->group;
  kprintf("GID: %d\n\r",inode->group);
  buf->st_ino = sb->inodes;
  kprintf("INODES: %d\n\r",sb->inodes);
  buf->st_mode = f->mode;
  kprintf("MODE :%d\n\r",f->mode);
  buf->st_mtime = inode->mtime;
  kprintf("MTIME: %d\n\r",inode->mtime);
  buf->st_nlink;
  buf->st_rdev;
  buf->st_size = inode->size;
  kprintf("SIZE: %d\n\r",inode->size);
  buf->st_uid = inode->owner;
  kprintf("UID: %d\n\r",inode->owner);
  
  return 1;

}
