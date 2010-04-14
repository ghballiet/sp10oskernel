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
#include <vfs.h>

int sfs_lseek(filedesc *f, off_t offset, int whence)
{
  /* From man file:
     If whence is SEEK_SET, offset is set to 'offset' bytes

     If whence is SEEK_CUR, offset is set to current offset plus 'offset' bytes

     If whence is SEEK_END, offset is set to the size of the file plus 'offset'
     bytes
  */
  struct fstat *fstat_buf = (struct fstat *)kmalloc(sizeof(struct fstat));
  sfs_fstat(f, fstat_buf);
  sfs_fd_private *fp = f->fs_private; 
  uint32_t blksize = fstat_buf->st_blksize;
  uint32_t fsize = fstat_buf->st_size;
  kfree(fstat_buf);
  uint64_t newpos;
  if(whence==SEEK_SET)
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

  if(newpos > fsize) {
    /* first, seek to end of file with a recursive call */
    sfs_lseek(f, fsize, SEEK_SET);
    /* next, append the necessary number of 0s */
    uint32_t to_write = newpos-fsize;
    char *zerobuf = (char *)kmalloc(to_write);
    uint32_t i;
    for(i=0; i<to_write; i++) {
      *(zerobuf+i) = 0;
    }
    sfs_write(f, zerobuf, to_write);
    if(f->dirty) {
    	blk_dev[f->major].write_fn(f->minor,
    				   f->curr_blk,
    				   f->buffer,
    				   fp->sb->sectorsperblock);
    }
  } else {
    /* move the file pointer to the now inside-the-file location */
    if(f->curr_log * blksize <= newpos &&
       (f->curr_log + 1) * blksize > newpos) {
      /* if we're moving to a different point in the current block */
      /* NOTE: I'm assuming here that blksize/bufsize will always be the same */
      /* kprintf("mode1: newpos=%d\r\n", newpos); */
      f->bufpos = (uint32_t)(newpos - (f->curr_log * blksize));
      /* kprintf("mode1: f->curr_log=%d\r\n", f->curr_log); */
      /* kprintf("mode1: f->bufpos=%d\r\n", f->bufpos); */
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
      uint32_t logblk = newpos/blksize;
      /* NOTE: in sfs_write this is calculated by dividing by f->bufsize (line
	 94); I'm assuming here that these two will always be the same */
      uint32_t buf_offset = (uint32_t)(newpos - (logblk * blksize));
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
      /* kprintf("mode2: newpos=%d\r\n", newpos); */
      /* kprintf("mode2: f->curr_log=%d\r\n", f->curr_log); */
      /* kprintf("mode2: f->bufpos=%d\r\n", f->bufpos); */
    }
    f->filepos = newpos;
  }
  //kprintf("end of lseek: newpos=%d\r\n", newpos); 
  //kprintf("end of lseek: f->filepos=%d\r\n", f->filepos); 
  kprintf("end of lseek: newpos=%d, f->curr_log=%d, f->bufpos=%d, f->filepos=%d\r\n",
	  newpos, f->curr_log, f->bufpos, f->filepos);
  /* TODO: why aren't these being set anymore? */
}
