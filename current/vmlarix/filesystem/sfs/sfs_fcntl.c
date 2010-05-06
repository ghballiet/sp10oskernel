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
#include <stdarg.h>


/* Defined by Designer, can this be placed in a better place?? */
#define F_DUPFD 1
#define F_GETFD 2
#define F_SETFD 3
#define F_GETFL 4
#define F_SETFL 5
#define F_SETLKW 6


int sfs_fcntl(filedesc *f, int cmd, ...)
{
	va_list ap;
	va_start(ap, cmd);
	
	
	if(f != NULL)	//check if we have a valid descriptor
	{
		sfs_inode_t *inode;	//declare pointer to inode
   		sfs_fd_private * sfs_fd;	//declare pointer to private part of a FD (File Descriptor)
   	
   		sfs_fd = (sfs_fd_private *)(f->fs_private); //Get the private part and casting it into the right type
   	
   		inode = sfs_fd->inode;	// setting the inode pointer to point into the right address
		
		switch(cmd)
		{
			//case F_DUPFD:
			//	kprintf("F_DUPFD\n\r");
			//========o========o========o========o========o========o
			/* (1). [File descriptor flags] --> F_GETFD (void): Read the file descriptor flags; arg is ignored.
			   (2). Read the close-on-exec flag. If the FD_CLOEXEC bit is 0, the file will remain open across exec, otherwise it will be closed. */
	
			case F_GETFD:
				kprintf("F_GETFD\n\r");
				return f->in_use;	//IT IS THIS ONE THE FLAG THAT WE SHOULD REFER TO??? IF NTO WHAT??
				

			/* (1). [File descriptor flags] --> F_SETFD (void): Set the file descriptor flags to the value specified by arg. 
			   (2). Set the close-on-exec flag to the value specified by the FD_CLOEXEC bit of arg. */
			case F_SETFD:
			
				kprintf("F_SETFD\n\r");
				 f->in_use = va_arg(ap, int);
				 va_end(ap);
				 return f->in_use;
			//========o========o========o========o========o========o
			/* [File status flags] --> F_GETFL (void): Read the file status flags; arg is ignored. */
			case F_GETFL:
				
				kprintf("F_GETFL\n\r");
				kprintf("O_NONBLOCK: %x\n\r",O_NONBLOCK);
				kprintf("O_APPEND: %x\n\r", O_APPEND);
				return f->in_use;  //IT IS THIS ONE THE STATUS FLAG??? IF NOT THEN WHAT???
				
			/* [File status flags] --> F_SETFL (long): set the file status flags to the value specified by arg.  
			File access mode (O_RDONLY, O_WRONLY, O_RDWR) and file creation flags (i.e.,
         	O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC) in arg are ignored.  On Linux this
         	command can only change the O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME, and
         	O_NONBLOCK flags.*/
			case F_SETFL:
			
				kprintf("F_SETFL\n\r");
				/* According with the description we are supposed to set this flags (change its value)
				but they were defined as (#define) which makes them constants */
				kprintf("According with the description we are supposed to set this flags (change its value)\n\r");
				kprintf("but they were defined as (#define) which makes them constants\n\r");
				return 1;
			//========o========o========o========o========o========o
			//>>case F_SETLKW:
			//>>	kprintf("F_SETLKW\n\r");
				
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



