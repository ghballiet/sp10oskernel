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

int sfs_fchown(filedesc *f, uid_t owner, gid_t group)
{
	mount_point *mp;	//declares a pointer to a mounting point
   uint32_t inum = 0;    /* inode number */
	sfs_inode_t *inode;	//declare pointer to inode - /* inode in memory */
	
   sfs_fd_private *sfs_fd;	//declare pointer to private part of a FD (File Descriptor)
   sfs_fd = (sfs_fd_private *)(f->fs_private); //Get the private part and casting it into the right type, from [void] to [sfs_fd_private]
   
   mp = f->mp;
   inum = sfs_fd->inum;   
   inode = sfs_fd->inode;	// setting the inode pointer to point into the right address

   filedesc *fdptr(uint32_t fd);
   
   if(sfs_fd == NULL)
   	return -1;  
    
   kprintf("Previous Owner: %d\t; Previous Group %d\n\r", inode->owner, inode->group);
		
   inode = sfs_fd->inode;
   inode->owner = owner;
   inode->group = group;
   kprintf("NEW Owner: %d\t; NEW Group %d\n\r", inode->owner, inode->group);
   /*writing this change back to the ramdisk in order to make it PERMANENT*/
   sfs_put_inode(mp, inum, inode);
    
    //kprintf("sfs_fchown() function not implemented\n\r");
}
