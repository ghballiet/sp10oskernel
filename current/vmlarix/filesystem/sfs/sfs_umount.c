
#include <sfs.h>
#include <blkdev.h>
#include <vfs_mp.h>
#include <stddef.h>
#include <sfs_superblock.h>
#include <byteswap.h>
#include <sfs_private.h>


#ifdef _KERNEL_
#include <misc.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define kmalloc malloc
#define kfree free
#define kprintf printf
#endif


int sfs_umount(void *fs)
{
	kprintf("sfs_umount() not implemented\n\r");
   kprintf("========oE========oE========o========o========o========o\n\r");
	mount_point *mp;	//declares a pointer to a mounting point
   mp = (mount_point *)fs;
   
   sfs_mp_private *smp;
   smp = (sfs_mp_private *)mp->fs_private;
   
   /* CHECK IF AT LEAST ONE FILE IS OPEN OR NOT*/
   //>>int i;
	//>>for(i=0;i<NUM_FD;i++)
	//>>{
   //>> 	if(fdesc[i].in_use != 0)
   //>> 	{
   //>> 		kprintf("UMOUNT WILL NOT BE EXECUTED -AT LEAST ONE FILE IS OPEN\n\r");
   //>> 		return -1;
   //>> 	}
   //>>}
   
   /* WRITE THE SUPERBLOCK AND BITMAP BACK TO DISK */
   blk_dev[mp->major].write_fn(mp->minor,
			 smp->super->free_inode_bitmap,
			 (char*)smp->free_inode_bitmap,
			 smp->super->free_inode_bitmapblocks*smp->super->sectorsperblock);
			 
   kfree(smp->free_inode_bitmap);
   kfree(smp->free_block_bitmap);
   
   
   
   
   
   /* FREE THE PRIVATE DATA STRUCTURES */
   
   
}

