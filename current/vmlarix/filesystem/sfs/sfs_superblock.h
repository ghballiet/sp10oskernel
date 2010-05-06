
#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#ifdef NEED_STDINT
#include <stdint.h>
#endif

#include <sys/types.h>


#define VMLARIX_SFS_MAGIC 0x00112233


#define VMLARIX_SFS_TYPESTR "vmlarix_sfs"

typedef struct {
  uint32_t fsmagic;         /* filesystem type             */
  char fstypestr[32];
  uint32_t block_size;      /* size of a block, in bytes   */
  uint32_t sectorsperblock; /* size of a block, in sectors */

  uint32_t superblock;      /* location of the superblock */

  uint32_t num_blocks;      /* number of blocks */
  uint32_t free_block_bitmap; /* first block of the free block bitmap */
  uint32_t free_block_bitmapblocks; /*number of blocks in free block bitmap*/
  uint32_t blocks_free;

  uint32_t num_inodes;   /* number of inodes */
  uint32_t free_inode_bitmap; /* first block of the free inode bitmap */
  uint32_t free_inode_bitmapblocks; /*number of blocks in free inode bitmap*/
  uint32_t inodes_free;

  uint32_t num_inode_blocks; /* number of blocks in the inode table */
  uint32_t inodes; /* first block of the inode table */

  uint32_t rootdir; /* first block of the root directory */
  uint32_t open_count;
}sfs_superblock;

#endif
