
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

int sfs_close(filedesc *f)
{
  sfs_fd_private *fp = f->fs_private;
  if(f->dirty)
    {
      /*write */
/*       printf("writing block %d for inode %d\n",f->curr_blk,f->inum); */
      blk_dev[f->major].write_fn(f->minor,
				 f->curr_blk,
				 f->buffer,
				 fp->sb->sectorsperblock);
      f->dirty = 0;
    }
  sfs_put_inode(f->mp,fp->inum,fp->inode);
  kfree(fp->inode);
  kfree(f->buffer);
  f->in_use = 0;
  return 0;
}

