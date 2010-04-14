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
  /* populates the following fields of "buf":
  //   /* dev_t     st_dev;     /\* ID of device containing file *\/ */ 
  //   /* ino_t     st_ino;     /\* inode number *\/ */
  //   /* mode_t    st_mode;    /\* protection *\/ */
  //   /* nlink_t   st_nlink;   /\* number of hard links *\/ */
  //   /* uid_t     st_uid;     /\* user ID of owner *\/ */
  //   /* gid_t     st_gid;     /\* group ID of owner *\/ */
  //   /* dev_t     st_rdev;    /\* device ID (if special file) *\/ */
  //   /* off_t     st_size;    /\* total size, in bytes *\/ */
  //   /* blksize_t st_blksize; /\* blocksize for file system I/O *\/ */
  //   /* blkcnt_t  st_blocks;  /\* number of blocks allocated *\/ */
  //   /* time_t    st_atime;   /\* time of last access *\/ */
  //   /* time_t    st_mtime;   /\* time of last modification *\/ */
  //   /* time_t    st_ctime;   /\* time of last fstatus change *\/ */
  
  buf->st_mode = f->mode;
  /* get blocksize for FS from superblock from private FS data */
  buf->st_blksize = ((sfs_fd_private *)f->fs_private)->sb->block_size;
  /* get inode pointer and inode number from file descriptor, populate fields
     from inode */
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
  /* Shift major number left and add minor number to get device number */
  uint32_t major = f->mp->major;
  uint32_t minor = f->mp->minor;
  /* TODO: I'm assuming here that dev_t is a 64-bit type; double-check this... */
  buf->st_dev = (((uint64_t)major)<<32) + minor;
  /* handle device number for special files */
  if(inode->type==FT_CHAR_SPEC || inode->type==FT_BLOCK_SPEC) {
    buf->st_rdev = (((uint64_t)(inode->direct[0])) << 32) + inode->direct[1];
  }
  /* Compute number of blocks from file size and blocksize */
  buf->st_blocks = sfs_get_num_blocks(f);
  
}
