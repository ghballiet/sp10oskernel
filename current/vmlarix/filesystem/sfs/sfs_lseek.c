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

int sfs_lseek(filedesc *f, off_t offset, int whence)
{
  /* From man file:
     If whence is SEEK_SET, offset is set to 'offset' bytes

     If whence is SEEK_CUR, offset is set to current offset plus 'offset' bytes

     If whence is SEEK_END, offset is set to the size of the file plus 'offset'
     bytes
  */
  /* TODO: fstat undeclared; adding an "include fstat" doesn't work... */
  fstat *fstat_buf = (fstat *)kmalloc(sizeof(fstat));
  sfs_fstat(f, fstat_buf);
  uint32_t blksize = fstat_buf->st_blksize;
  uint32_t fsize = fstat_buf->st_size;
  kfree(fstat_buf);
  uint32_t newpos;
  /* TODO: replace this with a switch stmt */
  if(whence==SEEK_SET)  /* TODO: these are undeclared */
    newpos = offset;
  else if(whence==SEEK_CUR)
    newpos = f->filepos + offset;
  else if(whence==SEEK_END)
    newpos = fsize + offset; /* +1? i.e., if given SEEK_END and offset 0,
				should it move it to point to the last byte in
				the file, or the first byte that would be after
				the current last byte? */
  else
    return -1; /* invalid whence argument */
  /* TODO: check that we're inside the file boundary? */
  /* TODO: special logic if we're outside the file (extend it with 0s?) */
  /* If we extend the file, what else will we have to update (i.e., is number
     of blocks stored anywhere, etc)? */

  /* finally, move the file pointer to the now-inside-the-file location */
  if(f->curr_blk * blksize <= newpos &&
     (f->curr_blk + 1) * blksize > newpos) {
    /* TODO: do I need to use bufsize from f instead of blksize? What's the
       relationship between these two items? */
    /* if we're moving to a different point in the current block */
    f->bufpos = offset - (f->curr_blk * blksize);
  } else {
    /* if we're moving to a different block */
    if(f->dirty) {
      blk_dev[f->major].write_fn(f->minor,
				 f->curr_blk,
				 f->buffer,
				 fp->sb->sectorsperblock);
      f->dirty=0;
    }
    /* so get logical block number we want, and calculate the offset in it */
    uint32_t logblk = offset/blksize;
    /* TODO: in sfs_write this is calculated by dividing by f->bufsize (line
       94), so again, what's the relationship between the two? */
    uint32_t buf_offset = offset - (logblk * blksize);
    /* get filesystem block number for that block */
    uint32_t fsblk = sfs_log2phys(f, logblk);
    if(fsblk==0) {
      /* something went wrong */
      return -1;
    }
    /* load that filesystem block into buffer */
    blk_dev[f->major].read_fn(f->minor,
			      fsblk,
			      f->buffer,
			      fp->sb->sectorsperblock);
    /* finally, update file descriptor */
    f->curr_log = logblk;
    f->curr_blk = fsblk;
    f->bufpos = buf_offset;
  }
  f->filepos = offset;
  kprintf("sfs_lseek() function not implemented\n\r");
}
