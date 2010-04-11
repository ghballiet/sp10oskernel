
/* This file provides functions for managing the blocks in a 
   file on an SFS filesystem. */

#ifndef _KERNEL_
#include <stdlib.h>
#define kmalloc malloc
#define kfree free
#endif

#include <sys/types.h>
#include <blkdev.h>
#include <vfs_filedesc.h>
#include <sfs_private.h>
#include <sfs_fileblock.h>

/* a file block can be one of the following types */
#define BT_DIRECT 0
#define BT_INDIRECT 1
#define BT_DINDIRECT 2
#define BT_TINDIRECT 3

/* This structure describes the location of a block */
typedef struct{
  uint32_t block_type;  /* which of the four types of block it is */
  uint32_t index[3];    /* indices for each level block pointers */
}sfs_block_loc;


uint64_t sfs_get_num_blocks(filedesc *f) {
  /* declared in sfs.h */
  sfs_inode_t *inode = sfs_inode_from_fd(f);
  uint32_t blksize = ((sfs_fd_private *)f->fs_private)->sb->block_size;
  /* is this going to be the same as f->bufsize ? */
  uint64_t blocks = inode->size / blksize;
  if(inode->size % blksize != 0)
    blocks++;
  return blocks;
}

/* Finds the block indices for a specified file position, and fills in
   the sfs_block_loc structure.  Returns 1 on success, 0 on failure.
*/
int sfs_calc_block_loc(filedesc *f,uint32_t blocknum,sfs_block_loc *loc)
{
  sfs_fd_private *fp = f->fs_private;
  if(blocknum < NUM_DIRECT)
    {
      loc->block_type = BT_DIRECT;
      loc->index[0] = blocknum;
      return 1;
    }

  /* find # of four-byte block numbers that can fit in a block */
  uint32_t num_indirect = fp->sb->block_size>>2;
  uint32_t block_number;
  if(blocknum < (NUM_DIRECT + num_indirect))
    {
      /* it's a single indirect */
      block_number = blocknum-NUM_DIRECT;
      loc->block_type = BT_INDIRECT;
      loc->index[0] = block_number;
      return 1;
    }

  uint32_t num_dindirect = num_indirect * num_indirect;
  /* A block size must be a power of two.  Find out the power, and create
     a mask. */
  uint32_t block_shift = 0;
  uint32_t block_mask = 0;
  uint32_t tmp = num_indirect>>1;
  while((block_shift < 32)&&(tmp > 0))
    {
      tmp = tmp >> 1;
      block_shift++;
      block_mask = (block_mask<<1) + 1;
    }

  if(blocknum < (NUM_DIRECT + num_indirect + num_dindirect))
    {
      /* it's a double indirect */
      /* calculate top level offset */
      block_number = (blocknum-NUM_DIRECT-num_indirect) >> block_shift;
      loc->block_type = BT_DINDIRECT;
      loc->index[0] = block_number;

      /* calculate offset into second level block */
      block_number = (blocknum-NUM_DIRECT-num_indirect) & block_mask;
      loc->index[1] = block_number;
      return 1;
    }
  
  uint32_t num_tindirect = num_dindirect * num_indirect;
  
  if(blocknum < (NUM_DIRECT + num_indirect + num_dindirect + num_tindirect))
    {
      /* it's a triple indirect */
      /* calculate top level offset */
      block_number = (blocknum-NUM_DIRECT-num_indirect-num_dindirect);
      block_number = (block_number>>block_shift)>>block_shift;
      loc->block_type = BT_TINDIRECT;
      loc->index[0] = block_number;

      /* calculate offset into second level */
      block_number = blocknum-NUM_DIRECT-num_indirect-num_dindirect;
      block_number = (block_number >> block_shift) & block_mask;
      loc->index[1] = block_number;

      /* calculate offset into third level */
      block_number = blocknum-NUM_DIRECT-num_indirect-num_dindirect;
      block_number = block_number & block_mask;
      loc->index[2] = block_number;
      return 1;
    }
  return 0;  /* we failed */
}

/* recursive routine to look up the block number, given an array of
   indices 
*/
int sfs_log2phys_recurse(filedesc *f,
			 uint32_t blocknum,
			 uint32_t *index,
			 int depth )
{
  sfs_fd_private *fp=f->fs_private;
  char *buffer = (char *)kmalloc(fp->sb->block_size);
  uint32_t *ptr = (uint32_t*) buffer;
  if(blocknum)
    {
      blk_dev[f->major].read_fn(f->minor,
				blocknum,
				buffer,
				fp->sb->sectorsperblock);
      blocknum = ptr[*index];
    }
  if(depth>0)
    blocknum = sfs_log2phys_recurse(f,blocknum,index+1,depth-1);

  kfree(buffer);
  return blocknum;
}

/* Finds the physical block number for a specified file logical block number.
   returns 0 on failure.
*/
int sfs_log2phys(filedesc *f,uint32_t log_block)
{
  sfs_fd_private *fp = f->fs_private;
  sfs_block_loc loc;
  uint32_t blocknum;

  if(!sfs_calc_block_loc(f,log_block,&loc))
    return 0;

  switch(loc.block_type)
    {
    case BT_DIRECT:
      blocknum = fp->inode->direct[loc.index[0]];
      break;
    case BT_INDIRECT:
      blocknum = sfs_log2phys_recurse(f,fp->inode->indirect,loc.index,0);
      break;
    case BT_DINDIRECT:
      blocknum = sfs_log2phys_recurse(f,fp->inode->dindirect,loc.index,1);
      break;
    case BT_TINDIRECT:
      blocknum = sfs_log2phys_recurse(f,fp->inode->tindirect,loc.index,2);
      break;
    default:
      break;
    }
  return blocknum;
}

/* recursive routine to set the block number */
int sfs_set_phys_recurse(filedesc *f,
			 uint32_t blocknum,
			 uint32_t *index,
			 int depth,
			 uint32_t phys )
{
  sfs_fd_private *fp = f->fs_private;
  sfs_mp_private *mp = f->mp->fs_private;
  char *buffer = (char *)kmalloc(fp->sb->block_size);
  uint32_t *ptr = (uint32_t*) buffer;
  int need_write = 0;
  uint32_t orig_ptr;
  int32_t tmp;
  int i;

  if(blocknum)
    { /* we already have a block of pointers at this level.  Read it in */
      blk_dev[f->major].read_fn(f->minor,
				blocknum,
				buffer,
				fp->sb->sectorsperblock);
    }
  else
    { /* we don't have a block of pointers at this level.  Allocate one */
      tmp = first_cleared(mp->free_block_bitmap,fp->sb->num_blocks);
      if(tmp < 0)
	{ /* file system is full */
	  kfree(buffer);
	  return -1;
	}
      set_bit(mp->free_block_bitmap,tmp);
      /* WRITE THE ENTIRE FREE_BLOCK BITMAP. 
         This could be a lot more efficient. 
      */
      blk_dev[f->major].write_fn(f->minor,
				 fp->sb->free_block_bitmap,
				 (char*)mp->free_block_bitmap,
				 fp->sb->free_block_bitmapblocks*
				 fp->sb->sectorsperblock);
      blocknum = tmp;
      /* set all block ptrs in the new block of ptrs to zero */
      for(i=0;i<(fp->sb->block_size>>2);i++)
	ptr[i] = 0;
      need_write = 1;
    }

  if(depth>0)
    {
      orig_ptr = ptr[*index];
      ptr[*index] = sfs_set_phys_recurse(f,orig_ptr,index+1,depth-1,phys);
      if(ptr[*index] < 0)
	{ /* error occurred somewhere in the recursive call */
	  int i = ptr[*index];
	  kfree(buffer);
	  return i;
	}
      if(ptr[*index] != orig_ptr)
	need_write = 1;
    }
  else
    {
      if(ptr[*index]) 
	{ /*error! phys was already set */
	  kfree(buffer);
	  return -2;
	}
      ptr[*index] = phys;
      need_write = 1;
    }
  /* Write the block of pointers back to disk. */
  if(need_write)
    blk_dev[f->major].write_fn(f->minor,
			       blocknum,
			       buffer,
			       fp->sb->sectorsperblock);
  kfree(buffer);
  return blocknum;
}

/* Set the physical block for a given file block number.  We may have
   to allocate blocks to hold block pointers as we go. We can only add
   blocks contiguously to the end of the file. */
int sfs_set_phys(filedesc *f,uint32_t log_block,uint32_t phys)
{
  sfs_fd_private *fp = f->fs_private;
  sfs_mp_private *mp = f->mp->fs_private;
  sfs_block_loc loc;
  uint32_t blocknum;
  int32_t tmp;

  if(!sfs_calc_block_loc(f,log_block,&loc))
    return 0;
  
  switch(loc.block_type)
    {
    case BT_DIRECT:
      if(fp->inode->direct[loc.index[0]]!=0) 
	/* block num already set */
	return -1;
      fp->inode->direct[loc.index[0]] = phys;
      sfs_put_inode(f->mp,fp->inum,fp->inode);
      break;
    case BT_INDIRECT:
      blocknum = fp->inode->indirect;
      tmp=fp->inode->indirect=sfs_set_phys_recurse(f,
						   fp->inode->indirect,
						   loc.index,
						   0,
						   phys);
      if(tmp<=0)
	return tmp;
      if(blocknum != fp->inode->indirect)
	sfs_put_inode(f->mp,fp->inum,fp->inode);
      break;
    case BT_DINDIRECT:
      blocknum = fp->inode->dindirect;
      tmp=fp->inode->dindirect=sfs_set_phys_recurse(f,
						    fp->inode->dindirect,
						    loc.index,
						    1,
						    phys);
      if(tmp<=0)
	return tmp;
      if(blocknum != fp->inode->dindirect)
	sfs_put_inode(f->mp,fp->inum,fp->inode);
      break;
    case BT_TINDIRECT:
      blocknum = fp->inode->tindirect;
      tmp=fp->inode->tindirect=sfs_set_phys_recurse(f,
						    fp->inode->tindirect,
						    loc.index,
						    2,
						    phys);
      if(tmp<=0)
	return tmp;
      if(blocknum != fp->inode->tindirect)
	sfs_put_inode(f->mp,fp->inum,fp->inode);
      break;
    default:
      break;
    }
  return 0;
}

/* Recursive routine to delete a physical block from a file */
/* We must only delete blocks from the end of the file. */
int sfs_del_phys_recurse(filedesc *f,
			 uint32_t blocknum,
			 uint32_t *index,
			 int depth)
{
  sfs_fd_private *fp = f->fs_private;
  sfs_mp_private *mp = f->mp->fs_private;
  char *buffer = (char *)kmalloc(fp->sb->block_size);
  uint32_t *ptr = (uint32_t*) buffer;
  int need_write = 0;
  uint32_t orig_ptr;
  int64_t tmp;

  if(!blocknum)  /* file does not have a physical block here */
    {
      kfree(buffer);
      return -1;
    }
  /* read the block of pointers */
  blk_dev[f->major].read_fn(f->minor,
			    blocknum,
			    buffer,
			    fp->sb->sectorsperblock);
  if(depth>0)
    {
      int tmp = sfs_del_phys_recurse(f,ptr[*index],index+1,depth-1);
      if(tmp == -1)
	{
	  kfree(buffer);
	  return -1;
	}
    }
  else
    {
      /* Bottom level.  Release the data block */
      clear_bit(mp->free_block_bitmap,ptr[*index]);
      /*  WRITE THE FREE_BLOCK BITMAP */
      blk_dev[f->major].write_fn(f->minor,
				 fp->sb->free_block_bitmap,
				 (char*)mp->free_block_bitmap,
				 fp->sb->free_block_bitmapblocks*
				 fp->sb->sectorsperblock);      
      tmp = 0;
    }
  
  if(tmp == 0)
    {
      /* next level block was deleted */
      /* update this block */
      ptr[*index] = 0;
      /* check to see if all blocks empty at this level */
      if(*index == 0)
	{
	  /* release current block */
	  clear_bit(mp->free_block_bitmap,blocknum);
	  blocknum = 0;
	}
      else
	{
	  /* write current block */
	  blk_dev[f->major].write_fn(f->minor,
				     blocknum,
				     buffer,
				     fp->sb->sectorsperblock);
	}
    }
  kfree(buffer);
  return blocknum;
}

/* Delete the physical block for a given file block number.  We may have
   to release blocks that hold block pointers, recursively. Ouch. */
int sfs_del_phys(filedesc *f,uint32_t log_block)
{
  sfs_fd_private *fp = f->fs_private;
  sfs_mp_private *mp = f->mp->fs_private;
  sfs_block_loc loc;
  uint32_t blocknum;
  int32_t tmp;

  if(!sfs_calc_block_loc(f,log_block,&loc))
    return -2;

  switch(loc.block_type)
    {
    case BT_DIRECT:
      /* Release the data block */
      if(!fp->inode->direct[loc.index[0]])
	{ /* there is an error.. block not allocated */
	  return -1;
	}
      clear_bit(mp->free_block_bitmap,fp->inode->direct[loc.index[0]]);
      /*  WRITE THE ENTIRE FREE_BLOCK BITMAP.
          This could be a lot more efficient. */
      blk_dev[f->major].write_fn(f->minor,
				 fp->sb->free_block_bitmap,
				 (char*)mp->free_block_bitmap,
				 fp->sb->free_block_bitmapblocks*
				 fp->sb->sectorsperblock);
      fp->inode->direct[loc.index[0]] = 0;
      sfs_put_inode(f->mp,fp->inum,fp->inode);
      blocknum = 0;
      break;
    case BT_INDIRECT:
      if(!fp->inode->indirect) /* we don't have an indirect block yet */
	{ /* there is an error.. block not allocated */
	  return -1;
	}
      fp->inode->indirect = sfs_del_phys_recurse(f,
						fp->inode->indirect,
						loc.index,
						0);
      if(!fp->inode->indirect)
	sfs_put_inode(f->mp,fp->inum,fp->inode);
      blocknum = 0;
      break;
    case BT_DINDIRECT:
      if(!fp->inode->dindirect) /* we don't have an indirect block yet */
	{ /* there is an error.. block not allocated */
	  return -1;
	}
      fp->inode->dindirect = sfs_del_phys_recurse(f,
						 fp->inode->dindirect,
						 loc.index,
						 1);
      if(!fp->inode->dindirect)
	sfs_put_inode(f->mp,fp->inum,fp->inode);
      blocknum = 0;
      break;
    case BT_TINDIRECT:
      if(!fp->inode->tindirect) /* we don't have an indirect block yet */
	{ /* there is an error.. block not allocated */
	  return -1;
	}
      fp->inode->tindirect = sfs_del_phys_recurse(f,
						 fp->inode->tindirect,
						 loc.index,
						 2);
      if(!fp->inode->tindirect)
	sfs_put_inode(f->mp,fp->inum,fp->inode);
      blocknum = 0;
      break;
    default:
      break;
    }
  return 0;
}


