//Author: Ashish Parajuli
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

int sfs_rmdir(mount_point* mp, const char *pathname)
{
	int inum,ret=-1;
	filedesc* fd;
	sfs_inode_t *inode=(sfs_inode_t*)kmalloc(sizeof(sfs_inode_t));
	//kprintf("SFS Rmdir %s\n\r",pathname);
	inum=sfs_lookup(mp,pathname,inode);
	//kprintf("inum %d",inum);
	if(inum<0)
	{
		//kprintf("Dir not found");
		ret=-1;
	}
	else
	if(inode->type!=FT_DIR)
	{
		ret=-1; //Not a directory
		//kprintf("Not a dir\n\r");
	}
	else
	//Check if directory is empty or not
	if(inode->size==(2*sizeof(sfs_dirent)))
	{
		//kprintf("Rmdir dir empty\r\n");
		int fdnum;
		if((fdnum=alloc_fd())<0)
		{
			kfree(inode);
			return -1;
		}
		//First open the inode
		fd=fdptr(fdnum);
		//kprintf("Opening inode\r\n");
		if(sfs_openinode(mp,fd,inum,inode)==0)
		{
			sfs_mp_private *p;
			//free blocks bitmap. This deletes the 1st block, clears the bit in the bitmap,
			// and writes the bitmap back to disk. --We only need to delete the first block.
			//kprintf("Freeing blocks bitmap \r\n");
			if(sfs_del_phys(fd,0)<0)
			{
				kfree(inode);
				return -1;
			}
			//Clear the free inode bitmap
			//kprintf("Freeing inode bitmap \r\n");
			p=(sfs_mp_private *) mp->fs_private;
			clear_bit(p->free_inode_bitmap,inum);
			//write the inode bitmap back to disk.
			//kprintf("writing inode back to disk \r\n");
			blk_dev[mp->major].write_fn(mp->minor,
				      p->super->free_inode_bitmap,
				      (char*)p->free_inode_bitmap,
				      p->super->free_inode_bitmapblocks*
				      p->super->sectorsperblock);
			ret=0;
		}
		free_fd(fdnum);
	}
	else
	{
		ret=-1;
		//kprintf("Dir not empty\r\n");
	}
	kfree(inode);
	return ret;
}
