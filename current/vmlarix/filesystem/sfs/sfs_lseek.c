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
  sfs_fd_private *fp = f->fs_private;
  uint32_t blksize = f->bufsize;
  uint64_t fsize = fp->inode->size;
  uint64_t newpos;
  if(whence==SEEK_SET)
    newpos = offset;
  else if(whence==SEEK_CUR)
    newpos = f->filepos + offset;
  else if(whence==SEEK_END)
    newpos = fsize + offset;
  else {
    kprintf("sfs_lseek error: whence=%d\r\n", whence);
    return -1; /* invalid whence argument */
  }

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
  }
  
  /* calculate the new logical block and buffer position */
  int64_t new_log = newpos/blksize;
  uint32_t new_bufpos = newpos - (new_log * blksize);
  if(new_log != f->curr_log) { /* if we need to change blocks */
    kprintf("sfs_lseek: changing logical blocks\r\n");
    /* write out the current block if it's dirty */
    if(f->dirty) {
      blk_dev[f->major].write_fn(f->minor,
				 f->curr_blk,
				 f->buffer,
				 fp->sb->sectorsperblock);
      f->dirty=0; 
    }
    /* find and load the new physical block */
    int64_t fsblk = sfs_log2phys(f, new_log);
    if(fsblk==0) {
      kprintf("sfs_lseek errror: fs block not loaded\r\n");
      return -2; /* something went wrong */
    }
    blk_dev[f->major].read_fn(f->minor,
			      fsblk,
			      f->buffer,
			      fp->sb->sectorsperblock);
    f->curr_blk = fsblk;
  }
  /* finally, update the file descriptor */
  kprintf("sfs_lseek: new_log=%d\r\n", new_log);
  kprintf("sfs_lseek: new_bufpos=%d\r\n", new_bufpos);
  kprintf("sfs_lseek: newpos=%d\r\n", newpos);
  f->filepos = newpos;
  f->curr_log = new_log;
  f->bufpos = new_bufpos;
  kprintf("sfs_lseek: curr_log=%d\r\n", f->curr_log;
  kprintf("sfs_lseek: bufpos=%d\r\n", f->bufpos);
  kprintf("sfs_lseek: filepos=%d\r\n", f->filepos);
  return newpos;
}
  
