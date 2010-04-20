
#include <stddef.h>
#include <vfs.h>
#include <vfs_filedesc.h>
#include <vfs_fsops.h>

#ifdef _KERNEL_
#include <misc.h>
#else
#include <stdlib.h>
#include <string.h>
#define kmalloc malloc
#endif

int vfs_fcntl(int fd, int cmd, ... )
{
	kprintf("vfs_fcntl() has been implemented!\n\r");

  	filedesc *f;
  	if(fd>=NUM_FD)
    	return -1;
  	f = fdptr(fd);

  	/* add code to handle devices ! */
  	return f->mp->ops->fcntl_fn(f,cmd);
  
}
