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

  /* So in the SEEK_END case, are we supposed to pad the file with 0s up to the
     new offset location? */

  /* Updating f->filepos is trivial; updated f->bufpos is harder (we may have
     to find/load a different block) */
  /* We can call sfs_fstat to populate an fstat structure to get
     fstat->st_blksize instead of directly having to get this from the private
     fs data -> superblock */
  /* We have sfs_log2phys which takes a logical file block number and returns
     the filesystem block number for that block (or 0 on failure) */
  /* There is code in sfs_write that deals with writing the current buffer and
     getting a new buffer that I can borrow from for writing out the current
     block if it's dirty. */
  kprintf("sfs_lseek() function not implemented\n\r");
}
