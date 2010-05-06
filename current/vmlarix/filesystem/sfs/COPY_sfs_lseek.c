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

/* Defined by Designer, can this be placed in a better place?? */
#define SEEK_SET  1
#define SEEK_CUR  2
#define SEEK_END  3

/*LSEEK: Repositions the offset of the FILE DESCRIPTOR [FD] to the argument OFFSET */

int sfs_lseek(filedesc *f, off_t offset, int whence)
{
	if(f != NULL)	//check if we have a valid descriptor
	{
		sfs_inode_t *inode;	//declare pointer to inode
   	sfs_fd_private * sfs_fd;	//declare pointer to private part of a FD (File Descriptor)
   	
   	sfs_fd = (sfs_fd_private *)(f->fs_private); //Get the private part and casting it into the right type
   	
   	inode = sfs_fd->inode;	// setting the inode pointer to point into the right address
		
		switch(whence)
		{
			/* The offset is set to offset bytes */
			case SEEK_SET:
				f->filepos = offset;
				return (f->filepos);
				
			/* The offset is set to its current location + offset bytes */
			case SEEK_CUR:
				f->filepos = (f->filepos)+(offset);
				return (f->filepos);
			
			/* The offset is set to the size of the file + offset bytes */
			case SEEK_END:
				f->filepos = (inode->size)+(offset);
				return (f->filepos);
			
			default:
				kprintf("Invalid [whence] option\n\r");
				return -1;
		}
	}
	
	else
	{
		//Not a valid Descriptor, so QUIT
		kprintf("Invalid [Descriptor]\n\r");
		return -1;
	}  
}
