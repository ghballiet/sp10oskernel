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

int sfs_dup(filedesc *f)
{
  
    kprintf("sfs_dup() has been implemented\n\r");
    filedesc *newfd;
    int fd;
    if((fd = alloc_fd())<0)
      return fd;
    newfd = fdptr(fd);

    //sfs_inode_t *inode;
    //sfs_fd_private * sfs_fd;
    newfd->fs_private = f->fs_private;
    newfd->buffer = f->buffer;
    newfd->bufpos = f->bufpos;
    newfd->bufsize = f->bufsize;
    newfd->curr_blk = f->curr_blk;
    newfd->curr_log = f->curr_log;
    newfd->dirty = f->dirty;
    newfd->filepos = f->filepos;
    newfd->flags = f->flags;
    newfd->in_use = f->in_use;
    newfd->major = f->major;
    newfd->minor = f->minor;
    newfd->mode = f->mode;
    newfd->mp = f->mp;
    newfd->type = f->type;
    newfd = f;
    return 1;

}
