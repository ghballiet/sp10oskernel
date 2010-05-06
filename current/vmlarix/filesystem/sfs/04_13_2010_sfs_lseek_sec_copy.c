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
/* Answer: YES, These have been moved to fcntl.h @ vmlarix/include*/
/*
#define SEEK_SET  1
#define SEEK_CUR  2
#define SEEK_END  3
*/

//========o========o========o========o========o========o========o========o========o
int find_out_my_case(off_t computed_block_number);
//========o========o========o========o========o========o========o========o========o
/*LSEEK: Repositions the offset of the FILE DESCRIPTOR [FD] to the argument OFFSET */
int sfs_lseek(filedesc *f, off_t offset, int whence)
{
	kprintf("========oE========oE========o========o========o========o\n\r");
	mount_point *mp;	//declares a pointer to a mounting point
   uint32_t inum;    /* inode number */
	sfs_inode_t *inode;	//declare pointer to inode - /* inode in memory */
	
   sfs_fd_private *sfs_fd;	//declare pointer to private part of a FD (File Descriptor)
   sfs_fd = (sfs_fd_private *)(f->fs_private); //Get the private part and casting it into the right type, from [void] to [sfs_fd_private]
   
   //>> sfs_fd_private *fp = f->fs_private;
   int t;
   mp = f->mp;
   inum = sfs_fd->inum;   
   inode = sfs_fd->inode;	// setting the inode pointer to point into the right address
   
   off_t computed_block_number;
   computed_block_number = offset/128;
   
   off_t my_new_offset;
   my_new_offset = offset - (computed_block_number*128);
   
   int returned_value;
   int seek_end_special_flag =0;
   
  	kprintf("offset : %d\n\r", offset);
	kprintf("whence VALUE [@SFS_LSEEK.C]: %d\n\r", whence);	
	kprintf("Initial value of current block:%d\n\r",f->curr_blk);
	
	if(f->curr_blk == computed_block_number)
	{
		kprintf("Inside the current block...\n\r");
		f->filepos = offset;
		f->bufpos = offset;
		kprintf("f->filepos FIRST CHECK = %u\n\r", f->filepos);
		kprintf("f->bufpos FIRST CHECK = %u\n\r", f->bufpos);
	}
	
	else
	{
		kprintf("f->filepos SECOND CHECK = %u\n\r", f->filepos);
		kprintf("f->bufpos SECOND CHECK= %u\n\r", f->bufpos);
		/* IF THE CURRENT BLOCK IS DIRTY */
		if(f->dirty)  /* if current one is dirty, write it out */
		{			
			/* write it to disk -- write it out */
			kprintf("I AM HERE 2\n\r");
		  	blk_dev[f->major].write_fn(f->minor,
		  										f->curr_blk,
		    									f->buffer,
		    									sfs_fd->sb->sectorsperblock);  									  									
		    									
			sfs_put_inode(mp, inum, inode);		
			f->dirty = 0;
		}
		
		switch(whence)
		{
			//case SEEK_SET:
			case SEEK_SET:
			   kprintf("Inside case 1...\n\r");
				returned_value = sfs_log2phys(f, computed_block_number);
   			f->curr_blk = returned_value;
				kprintf("New current block:%d\n\r",f->curr_blk);
   		///			f->bufpos = 0;
   			
   			blk_dev[f->major].read_fn(f->minor,
					    f->curr_blk,
					    f->buffer,
					    sfs_fd->sb->sectorsperblock);
		      f->dirty = 0;
   			break;
   				
   		//case SEEK_CUR:
   		case SEEK_CUR:
   			kprintf("Inside case 2...\n\r");
   			f->curr_log = f->filepos / f->bufsize;	
   			
   			kprintf("f->curr_log:%d\n\r",f->curr_log);
		  		f->curr_blk = sfs_log2phys(f,f->curr_log);
		  		kprintf("f->curr_blk:%d\n\r",f->curr_blk);
		  		
   			blk_dev[f->major].read_fn(f->minor,
					    f->curr_blk,
					    f->buffer,
					    sfs_fd->sb->sectorsperblock);
		      f->dirty = 0;
		  		break;
		  		
		  	//case SEEK_END:
		  	case SEEK_END:
   			kprintf("Inside case 3...\n\r");
   			f->curr_log = (inode->size + offset) / f->bufsize;
   			kprintf("f->curr_log:%d\n\r",f->curr_log);
		  		f->curr_blk = sfs_log2phys(f,f->curr_log);
		  		kprintf("f->curr_blk:%d\n\r",f->curr_blk);
   			
   			blk_dev[f->major].read_fn(f->minor,
					    f->curr_blk,
					    f->buffer,
					    sfs_fd->sb->sectorsperblock);
		      f->dirty = 0;
		      seek_end_special_flag = 1;
		  		break;
		}
		
		if(seek_end_special_flag == 1)
		{
			kprintf("I am here in special flag section\n\r");
			f->filepos = (inode->size)+(offset);
			f->bufpos =  my_new_offset;	
			kprintf("inode->size:%d\n\r",inode->size);
			kprintf("f->filepos:%d\n\r",f->filepos);
			kprintf("f->bufpos:%d\n\r",f->bufpos);
		}
		
		else
		{
			kprintf("I am here inside the else\n\r");
			f->filepos += offset;
		
			if(offset < f->bufsize)
			{
				f->bufpos += offset;
			}
			else
			{
				f->bufpos += offset - (computed_block_number*f->bufsize);
			}
		}
		
		//f->bufpos = f->filepos - (f->curr_log*128);
		
		//>> f->bufpos = (offset % f->bufsize);
			
		
		t = f->bufpos;
		kprintf("New value of FPP:%d\n\r",f->filepos);
		kprintf("New value of BPP:%d\n\r",f->bufpos);
	} 
	
	char *buf;
	t = f->bufpos;
	while(t <= f->bufsize)
	{
		kprintf("%c",f->buffer[t++]);
	}
	kprintf("\n\r");
	
	/* char *buf;
	f->bufpos = 0;
	
	while(f->bufpos <= t)
	while(f->bufpos <= f->bufsize)
	{
		kprintf("%c",f->buffer[f->bufpos++]);
	}
	kprintf("\n\r"); */
		      
	kprintf("f->filepos FINAL = %u\n\r", f->filepos);
	kprintf("f->bufpos FINAL = %u\n\r", f->bufpos);
		      	      
	kprintf("\n\r");
	
	
}
