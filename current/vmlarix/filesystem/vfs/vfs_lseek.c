
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

int vfs_lseek(int fd, off_t offset, int a)
{
	kprintf("vfs_lseek() has been implemented!\n\r");

  	filedesc *f;
  	if(fd>=NUM_FD)
  	{
    	kprintf("ERROR: NOT A VALID FILE DESCRIPTOR\n\r");
    	return -1;
   }
   
  	f = fdptr(fd);
  	
  	/* add code to handle devices ! */
  	kprintf("offset : %d\n\r", offset);
  	return f->mp->ops->lseek_fn(f, offset, a);
  
	//kprintf("vfs_lseek() not implemented!\n\r");
  
}




