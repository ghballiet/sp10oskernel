
#ifndef SFS_PRIVATE_H
#define SFS_PRIVATE_H

#include <bitmap.h>
#include <sfs_inode.h>
#include <sfs_superblock.h>

/* This is the decscription of the sfs private data structures */

/* vfs file descriptors will have a pointer to one of these data
   structures, which is specific to the sfs filesystem */
typedef struct{
  uint32_t inum;       /* inode number */
  sfs_inode_t *inode;  /* inode in memory */
  sfs_superblock *sb;  /* superblock for the filesystem */
}sfs_fd_private;

/* vfs mount points will maintain a pointer to this private sfs
   data about the mounted filesystem */
typedef struct{
  sfs_superblock *super;  /* pointers to the superblock and bitmaps */
  bitmap_t *free_block_bitmap;
  bitmap_t *free_inode_bitmap;
}sfs_mp_private;

#endif
