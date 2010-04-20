
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


/* int sfs_mount(const char *source, const char *target, */
/* 	  const char *filesystemtype, unsigned long mountflags, */
/* 	  const void *data) */
/* { */
/* } */


/* mount the filesystem, using the major and minor driver numbers.
   return pointer to private data on success, NULL on failure. */
void *sfs_mount(uint32_t major, uint32_t minor,unsigned long mountflags,
		const void *data)
{
  int sectors = blk_dev[major].num_blk(minor);
  int sector_size = blk_dev[major].blk_size(minor);
  int i;
  char *buffer = (char *)kmalloc(sector_size);
  kprintf("sector_size = %d\n\r" ,sector_size);
  mount_point *mp;

  sfs_superblock *super = (sfs_superblock*)buffer;

  /* Step 1, get the superblock. We know it is somewhere near the
     start of the disk, and a really good filesystem would keep a
     duplicate somewhere. */
  i = 1;
  do
    {
      blk_dev[major].read_fn(minor,i++,buffer,1);
      byteswap32(&(super->fsmagic));
    }
  while((i<sectors) && (((sfs_superblock*)buffer)->fsmagic != VMLARIX_SFS_MAGIC));
  if(i==sectors)
    return NULL;

  kprintf("superblock found at sector %d\n\r",--i);
  /* swap bytes if we are on a big-endian machine */
  byteswap32(&(super->block_size));
  byteswap32(&(super->sectorsperblock));
  byteswap32(&(super->superblock));
  byteswap32(&(super->num_blocks));
  byteswap32(&(super->free_block_bitmap));
  byteswap32(&(super->free_block_bitmapblocks));
  byteswap32(&(super->blocks_free));
  byteswap32(&(super->num_inodes));
  byteswap32(&(super->free_inode_bitmap));
  byteswap32(&(super->free_inode_bitmapblocks));
  byteswap32(&(super->inodes_free));
  byteswap32(&(super->num_inode_blocks));
  byteswap32(&(super->inodes));
  byteswap32(&(super->rootdir));
  byteswap32(&(super->open_count));

  /* We have found the superblock!! Now we can create a private
     data structure */
  sfs_mp_private *p = (sfs_mp_private*)kmalloc(sizeof(sfs_mp_private));

  p->super = super;

  /* read the free block bitmap */
  p->free_block_bitmap=(void*)kmalloc(super->free_block_bitmapblocks*
			       super->block_size);
  blk_dev[major].read_fn(minor,
			 super->free_block_bitmap,
			 (char*)p->free_block_bitmap,
			 super->free_block_bitmapblocks*super->sectorsperblock
			 );
  /* read the free inode bitmap */
  p->free_inode_bitmap=(void*)kmalloc(super->free_inode_bitmapblocks*
			       super->block_size);
  blk_dev[major].read_fn(minor,
			 super->free_inode_bitmap,
			 (char*)p->free_inode_bitmap,
			 super->free_inode_bitmapblocks*super->sectorsperblock
			 );
  return p;
}



