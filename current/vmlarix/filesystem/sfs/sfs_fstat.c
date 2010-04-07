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

int sfs_fstat(filedesc *f, struct fstat *buf)
{
  kprintf("sfs_fstat() function not implemented\n\r");
  /* TODO: populate the following fields of "buf":
  /* dev_t     st_dev;     /\* ID of device containing file *\/ */ 
  //   /* ino_t     st_ino;     /\* inode number *\/ */
  //   /* mode_t    st_mode;    /\* protection *\/ */
  //   /* nlink_t   st_nlink;   /\* number of hard links *\/ */
  //   /* uid_t     st_uid;     /\* user ID of owner *\/ */
  //   /* gid_t     st_gid;     /\* group ID of owner *\/ */
  /* dev_t     st_rdev;    /\* device ID (if special file) *\/ */
  //   /* off_t     st_size;    /\* total size, in bytes *\/ */
  //   /* blksize_t st_blksize; /\* blocksize for file system I/O *\/ */
  /* blkcnt_t  st_blocks;  /\* number of 512B blocks allocated *\/ */
  //   /* time_t    st_atime;   /\* time of last access *\/ */
  //   /* time_t    st_mtime;   /\* time of last modification *\/ */
  //   /* time_t    st_ctime;   /\* time of last fstatus change *\/ */
  
  buf->st_mode = f->mode;
  /* get blocksize for FS from superblock from private FS data */
  buf->st_blocksize = ((sfs_fd_private *)f->fs_private)->sb->block_size;
  /* TODO: what do I need to do for number of 512B blocks allocated? Does that
     mean allocated just for this file? If so, is this just division? */

  /* get inode pointer and inode number from file descriptor */
  sfs_inode_t *inode = ((sfs_fd_private *)f->fs_private)->inode;
  uint32_t inum = ((sfs_fd_private *)f->fs_private)->inum;
  buf->st_ino = inum;
  buf->st_atime = inode->atime;
  buf->st_mtime = inode->mtime;
  buf->st_ctime = inode->ctime;
  buf->st_nlink = inode->refcount;
  buf->st_uid = inode->owner;
  buf->st_gid = inode->group;
  buf->st_size = inode->size;
  /* TODO: where does 'ID of device containing file' come from? major/minor
     numbers can be retrieved from fieldesc->mp->major/minor, but are those
     what go here? And which one? */
  /* TODO: also, what about 'st_rdev' device ID for special files? */
  
}
