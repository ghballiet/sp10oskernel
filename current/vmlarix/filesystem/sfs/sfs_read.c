
#ifndef _KERNEL_
#include <stdio.h>
#endif 

#include <vfs_filedesc.h>
#include <blkdev.h>
#include <fcntl.h>
#include <sfs_private.h>
#include <sfs_fileblock.h>

int sfs_read(filedesc *f, void* bufptr, size_t nbytes)
{
  int i=0;
  int ok = 1;
  char *buf = bufptr;
  /* f->private is our own private data */
  sfs_fd_private *fp = f->fs_private; 
  while((i<nbytes)&&(ok))
    {
      if(f->filepos >= fp->inode->size)
	ok = 0;
      else
	{
	  buf[i++] = f->buffer[f->bufpos++];
	  f->filepos++;
	  if(f->bufpos >= f->bufsize) /* time to read next block */
	    {
	      f->bufpos = 0;
	      if(f->dirty)  /* if current one is dirty, write it out */
		{
/* 		  printf("writing block %d for inode %d\n",f->curr_blk,f->inum); */
		  blk_dev[f->major].write_fn(f->minor,
					     f->curr_blk,
					     f->buffer,
					     fp->sb->sectorsperblock);
		  sfs_put_inode(f->mp,fp->inum,fp->inode);
		  f->dirty = 0;
		}
	      if(f->filepos < fp->inode->size) /* are there more blocks? */
		{
		  /* read next block */
		  /* read next block from disk */
		  f->curr_log = f->filepos / f->bufsize;
		  f->curr_blk = sfs_log2phys(f,f->curr_log);
/* 		  printf("reading block %d for inode %d\n",f->curr_blk,f->inum); */
		  blk_dev[f->major].read_fn(f->minor,
					    f->curr_blk,
					    f->buffer,
					    fp->sb->sectorsperblock);
		  f->dirty = 0;
		}
	    }
	}
    }
  return i;
}

