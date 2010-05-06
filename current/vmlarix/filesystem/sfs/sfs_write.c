
#ifndef _KERNEL_
#include <stdio.h>
#endif 

#include <vfs_filedesc.h>
#include <blkdev.h>
#include <fcntl.h>
#include <sfs_private.h>
#include <sfs_fileblock.h>

int  sfs_write(filedesc *f, void* bufptr, size_t nbytes)
{
  int i=0;
  int ok = 1;
  char *buf = bufptr;
  int32_t blocknum;
  /* f->private is our own private data */
  sfs_fd_private *fp = f->fs_private; 
  sfs_mp_private *mp = f->mp->fs_private;
  /* if we have never written to this file */
  if((fp->inode->size == 0)&&(nbytes>0))
    {
      /* allocate a new disk block and add to inode */
       f->curr_log = 0; /* logical file block zero */
      blocknum = first_cleared(mp->free_block_bitmap,
			       fp->sb->num_blocks);
/*       printf("Allocated phys disk block %d\n",blocknum); */
      if(blocknum<0)
	return -1;
      set_bit(mp->free_block_bitmap,blocknum);
      blk_dev[f->major].write_fn(f->minor,
				 fp->sb->free_block_bitmap,
				 (char*)mp->free_block_bitmap,
				 fp->sb->free_block_bitmapblocks*fp->sb->sectorsperblock);

/*       printf("Set phys disk block %d\n",blocknum); */
      if(sfs_set_phys(f,f->curr_log,blocknum)!=0)
	return -1;

      f->curr_blk = blocknum; 

    }
  else
    /* in append mode, always seek eof before writing */
    if(f->flags & O_APPEND)
      {
	f->filepos = fp->inode->size;
	f->bufpos = f->filepos % f->bufsize;
	f->curr_log = f->filepos / f->bufsize;
	/* get current block from curr_log */
	f->curr_blk = sfs_log2phys(f,f->curr_log);
	if((!f->curr_blk)&&(nbytes>0))
	  {
	    int64_t bn;
	    bn = first_cleared(mp->free_block_bitmap,
			       fp->sb->num_blocks);
	    if(bn<0)
	      return -1;
	    set_bit(mp->free_block_bitmap,bn);
	    blk_dev[f->major].write_fn(f->minor,
				       fp->sb->free_block_bitmap,
				       (char*)mp->free_block_bitmap,
				       fp->sb->free_block_bitmapblocks*fp->sb->sectorsperblock);
	    f->curr_blk = bn;
	    if(sfs_set_phys(f,f->curr_log,f->curr_blk)!=0)
	      return -1;
	  }
	blk_dev[f->major].read_fn(f->minor,
				  f->curr_blk,
				  f->buffer,
				  fp->sb->sectorsperblock);
      }

  while((i<nbytes)&&(ok))
    {
      /* if the next byte is going to overfill the buffer, then we
	 need to write the buffer and start filling it again. */
      if(f->bufpos >= f->bufsize) /* buffer is full.  write it out */
	{
	  f->bufpos = 0;
	  /*write */
/*  	  printf("writing block %d for inode %d\n",f->curr_blk,f->inum);  */
	  blk_dev[f->major].write_fn(f->minor,
				     f->curr_blk,
				     f->buffer,
				     fp->sb->sectorsperblock);
	  sfs_put_inode(f->mp,fp->inum,fp->inode);
	  f->dirty = 0;

	  if(f->filepos < fp->inode->size)
	    {
	      /* read next block from disk */
	      f->curr_log = f->filepos / f->bufsize;
	      f->curr_blk = sfs_log2phys(f,f->curr_log);
	      blk_dev[f->major].read_fn(f->minor,
					f->curr_blk,
					f->buffer,
					fp->sb->sectorsperblock);
	      f->dirty = 0;
	    }
	  else
	    {
	      /* allocate a new disk block and add to inode */
	      blocknum = first_cleared(mp->free_block_bitmap,
				       fp->sb->num_blocks);
	      if(blocknum<0)
		return -1;
	      set_bit(mp->free_block_bitmap,blocknum);
	      blk_dev[f->major].write_fn(f->minor,
					 fp->sb->free_block_bitmap,
					 (char*)mp->free_block_bitmap,
					 fp->sb->free_block_bitmapblocks*fp->sb->sectorsperblock);
	      f->curr_log++;
	      if(sfs_set_phys(f,f->curr_log,blocknum)!=0)
		return -1;
	      f->curr_blk = blocknum;
	      f->dirty = 0;
	      
	    }
	}
      
      f->buffer[f->bufpos++] = buf[i++];
      f->dirty = 1;
      f->filepos++;
/*       printf("file position: %ld\n",f->filepos); */
      if((f->filepos > fp->inode->size))
	{
	  fp->inode->size = f->filepos;
/* 	  printf("file size = %ld\n",f->inode->size); */
	}
    }
  return i;
}

