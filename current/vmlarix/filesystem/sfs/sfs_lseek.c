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
  #define SEEK_SET  0
  #define SEEK_CUR  1
  #define SEEK_END  2
*/

/*LSEEK: Repositions the offset of the FILE DESCRIPTOR [FD] to the argument OFFSET */
int sfs_lseek(filedesc *f, off_t offset, int whence)
{
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
   
  int current_physical_block;
  int seek_end_special_flag =0;
   
  static uint32_t temp_bufpos;   /* current read/write pointer (buffer position) */
  static uint64_t temp_filepos;  /* current read/write pointer (file position) */
  static int file_desc_flag;
   
  //~~~~~~~~~~~~~~~~~~~~~
  int offset_case;
   
  if(offset <= 127)
    {
      offset_case = 0;
      //kprintf("offset_case : %d\n\r", offset_case);
    }
  if((127 < offset)&&( offset<= inode->size))
    {
      offset_case = 1;
      //kprintf("offset_case : %d\n\r", offset_case);
    }
  if(offset > inode->size)
    {
      offset_case = 2;
      //kprintf("offset_case : %d\n\r", offset_case);
    }
  //~~~~~~~~~~~~~~~~~~~~~
   
  //========o========o========o========o========o========o========o
  /* Check if we have been given a valid whence value*/
  if((whence!=SEEK_SET)&&(whence!=SEEK_CUR)&&(whence!=SEEK_END))
    {
      kprintf("ERROR: INVALID VALUE FOR WHENCE PARAMETER\n\r");\
      kprintf("WHENCE PARAMETER VALUE IS = %u\n\r",whence);
      return -2;
    }
 
  //========o========o========o========o========o========o========o
 
   
   
  kprintf("offset : %d\n\r", offset);
  //kprintf("Initial value of current block [PHYSICAL]:%d\n\r",f->curr_blk);
	
  if(f->curr_blk == computed_block_number)
    {
      //kprintf("==== CHECKING IF IT IS IN THE SAME BLOCK ====\n\r");
      //kprintf("Inside the current block...\n\r");
      f->filepos = offset;
      f->bufpos = offset;
      //kprintf("f->filepos FIRST CHECK = %u\n\r", f->filepos);
      //kprintf("f->bufpos FIRST CHECK = %u\n\r", f->bufpos);
    }
	
  else
    {
      //kprintf("==== IT IS NOT IN THE SAME BLOCK ====\n\r");
      kprintf("f->filepos SECOND CHECK = %u\n\r", f->filepos);
      kprintf("f->bufpos SECOND CHECK= %u\n\r", f->bufpos);
      /* IF THE CURRENT BLOCK IS DIRTY */
      if(f->dirty)  /* if current one is dirty, write it out */
	{			
	  kprintf("==== IF WE ARE HERE IS BECAUSE IT IS DIRTY ====\n\r");
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
	  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	case SEEK_SET:
	  f->filepos = 0;	//ALWAYS START FROM ZERO
	  f->bufpos = 0;		//ALWAYS START FROM ZERO
		      
	  switch(offset_case)
	    {
	      //kprintf("offset_case : %d\n\r", offset_case);
	    case 0:
	      kprintf("*********************************\n\r");
	      kprintf("Inside case SEEK_SET - CASE 0...\n\r");
	      kprintf("*********************************\n\r");
	      //kprintf("current block [LOGICAL]:%d\n\r",computed_block_number);
	      current_physical_block = sfs_log2phys(f, computed_block_number);
	      f->curr_blk = current_physical_block;
	      //kprintf("New current block [PHYSICAL]:%d\n\r",f->curr_blk);
   			
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      f->filepos = offset;
	      f->bufpos = offset;
		      		
	      temp_filepos = f->filepos;
	      temp_bufpos = f->bufpos;  
	      file_desc_flag = 1; 
   					  
	      break;
					
	    case 1:
	      kprintf("*********************************\n\r");
	      kprintf("Inside case SEEK_SET - CASE 1...\n\r");
	      kprintf("*********************************\n\r");
	      kprintf("current block [LOGICAL]:%d\n\r",computed_block_number);
	      current_physical_block = sfs_log2phys(f, computed_block_number);
	      f->curr_blk = current_physical_block;
   					
	      kprintf("New current block [PHYSICAL]:%d\n\r",f->curr_blk);
	      kprintf("computed_block_number:%d\n\r",computed_block_number);
	      kprintf("(computed_block_number*128):%d\n\r",(computed_block_number*128));
						
	      kprintf("(computed_block_number*f->bufsize):%d\n\r",(computed_block_number*f->bufsize));
						
   			
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      f->filepos = offset;
	      f->bufpos = offset - (computed_block_number*128);
	      //f->bufpos = offset - (computed_block_number*f->bufsize);
		      		
	      temp_filepos = f->filepos;
	      temp_bufpos = f->bufpos; 
	      file_desc_flag = 1;
	      break;
					
	    case 2:
	      kprintf("*********************************\n\r");
	      kprintf("Inside case SEEK_SET - CASE 2...\n\r");
	      kprintf("*********************************\n\r");
	      kprintf("current block [LOGICAL]:%d\n\r",computed_block_number);
	      current_physical_block = sfs_log2phys(f, computed_block_number);
	      f->curr_blk = current_physical_block;
	      //kprintf("New current block [PHYSICAL]:%d\n\r",f->curr_blk);
   			
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      //kprintf("(inode->size):%d\n\r",(inode->size));
	      f->filepos = offset;
	      f->bufpos = offset - (computed_block_number*128);
	      //f->bufpos = offset - (computed_block_number*f->bufsize);
		      		
	      temp_filepos = f->filepos;
	      temp_bufpos = f->bufpos; 
	      file_desc_flag = 1;
		      		
	      break;
	    }
	  break;
	  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++			
	case SEEK_CUR:
	  switch(offset_case)
	    {
	    case 0:
	      /* In this case (offset <= 128) [f->bufsize] */
	
	      //kprintf("file_desc_flag:%u\n\r",file_desc_flag);	//These are the value to use
	      //kprintf("temp_filepos:%u\n\r",temp_filepos);	//These are the value to use
	      //kprintf("temp_bufpos:%u\n\r",temp_bufpos);	//These are the values to use
					
	      f->filepos = temp_filepos;
	      f->bufpos = temp_bufpos;
	      //kprintf("*********************************\n\r");
	      //kprintf("Inside case SEEK_CUR - 0...\n\r");
	      //kprintf("*********************************\n\r");
	      //kprintf("f->filepos:%d\n\r",f->filepos);
						
   					
	      /*Based on the current value of f->filepos, estimate the logical block*/
	      f->curr_log = f->filepos / f->bufsize;	//current logical block
   					
	      //-----------
	      //Check if because the offset we have to leave current block and move to the next one
	      if((f->bufpos + offset) > f->bufsize)
		{
		  //If I am here is because I have to leave the current logical block and move to the next one
		  f->curr_log = f->curr_log + 1;
		}
	      //===========
   			
	      //kprintf("f->curr_log:%d\n\r",f->curr_log);
	      f->curr_blk = sfs_log2phys(f,f->curr_log);	//compute the current physical 
	      //kprintf("f->curr_blk:%d\n\r",f->curr_blk);
		  		
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      /*The file position can be updated directly*/
	      f->filepos = (f->filepos) + offset;
		      		
	      //-----------
	      //Check if current [f->bufpos] + [offset] is bigger than the size of our buffer,
	      //if it is then we need to re-adjust our [f->bufpos] as follows
	      if((f->bufpos + offset) > f->bufsize)
		{
		  //kprintf("============================\n\r",f->bufpos);
		  //kprintf("f->bufpos:%d\n\r",f->bufpos);
		  //kprintf("f->bufsize:%d\n\r",f->bufsize);
		  //kprintf("offset:%d\n\r",offset);
		      			
		  f->bufpos = (f->bufpos + offset) - f->bufsize; 

		}
	      else
		{
		  /*Adjust simply as follows because still we are under 128 bytes*/
		  f->bufpos =  (f->bufpos) + offset;
		}
	      //===========
	      break;
		  			
	    case 1:
		  				
	      /* In this case (128 < offset)&&(offset <inode->size) */
	      f->filepos = temp_filepos;
	      f->bufpos = temp_bufpos;
	      //kprintf("*********************************\n\r");
	      //kprintf("Inside case SEEK_CUR - 1...\n\r");
	      //kprintf("*********************************\n\r");
	      //kprintf("f->filepos:%d\n\r",f->filepos);
   					
	      /*Based on the current value of f->filepos, estimate the logical block*/
	      f->curr_log = f->filepos / f->bufsize;	//current logical block
   					
	      //-----------
	      if((f->bufpos + offset) > f->bufsize)
		{
		  //Check if because the offset we have to leave current block and move to the next one
		  f->curr_log = f->curr_log + computed_block_number;
		}
	      //===========
   			
	      //kprintf("f->curr_log:%d\n\r",f->curr_log);
	      f->curr_blk = sfs_log2phys(f,f->curr_log);	//compute the current physical 
	      //kprintf("f->curr_blk:%d\n\r",f->curr_blk);
		  		
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      /*The file position can be updated directly*/
	      f->filepos = (f->filepos) + offset;
		      		
		      		
	      //kprintf("============================\n\r",f->bufpos);
	      //kprintf("f->bufpos:%d\n\r",f->bufpos);
	      //kprintf("f->bufsize:%d\n\r",f->bufsize);
	      //kprintf("offset:%d\n\r",offset);
		      		
	      //Recompute and adjust your bufpos according to your case and given offset
	      f->bufpos = (f->bufpos + offset) - (computed_block_number * f->bufsize); 
		      		
	      //===========
	      break;
		  				
	    case 2:
		  				
	      /* In this case  (offset > inode->size) */
	      f->filepos = temp_filepos;
	      f->bufpos = temp_bufpos;
	      //kprintf("*********************************\n\r");
	      //kprintf("Inside case SEEK_CUR - 2...\n\r");
	      //kprintf("*********************************\n\r");
	      //kprintf("f->filepos:%d\n\r",f->filepos);
   					
	      f->curr_log = f->filepos / f->bufsize;	//current logical block
   					
	      //-----------
	      if((f->bufpos + offset) > f->bufsize)
		{
		  f->curr_log = f->curr_log + computed_block_number;
		}
	      //===========
   			
	      //kprintf("f->curr_log:%d\n\r",f->curr_log);
	      f->curr_blk = sfs_log2phys(f,f->curr_log);	//compute the current physical 
	      //kprintf("f->curr_blk:%d\n\r",f->curr_blk);
		  		
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      f->filepos = (f->filepos) + offset;
		      		
	      //kprintf("============================\n\r",f->bufpos);
	      //kprintf("f->bufpos:%d\n\r",f->bufpos);
	      //kprintf("f->bufsize:%d\n\r",f->bufsize);
	      //kprintf("offset:%d\n\r",offset);
		      			
	      f->bufpos = (f->bufpos + offset) - (computed_block_number * f->bufsize); 
		      		
	      //===========
	      break;		  				
	    }
	  break;
	  //*******************************************************************		
   			
		  		
	case SEEK_END:
	  switch(offset_case)
	    {
	      f->filepos = 0;	//ALWAYS START FROM ZERO
	      f->bufpos = 0;		//ALWAYS START FROM ZERO
	    case 0:
	      /* In this case (offset <= 128) [f->bufsize] */
	      //kprintf("*********************************\n\r");
	      //kprintf("Inside case SEEK_END - 0...\n\r");
	      //kprintf("*********************************\n\r");
	      //kprintf("f->filepos:%d\n\r",f->filepos);
						
	      f->curr_log = inode->size / f->bufsize;	//current logical block
   			
	      //kprintf("f->curr_log:%d\n\r",f->curr_log);
	      f->curr_blk = sfs_log2phys(f,f->curr_log);	//compute the current physical 
	      //kprintf("f->curr_blk:%d\n\r",f->curr_blk);
		  		
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      f->filepos = (inode->size) + offset;
	      f->bufpos =  offset;
	      break;
		  			
	    case 1:
	      /* In this case (128 < offset)&&(offset <inode->size) */
	      //kprintf("*********************************\n\r");
	      //kprintf("Inside case SEEK_CUR - 1...\n\r");
	      //kprintf("*********************************\n\r");
	      //
	      //kprintf("f->filepos:%d\n\r",f->filepos);
   					
	      f->curr_log = inode->size / f->bufsize;	//current logical block
   			
	      //kprintf("f->curr_log:%d\n\r",f->curr_log);
	      f->curr_blk = sfs_log2phys(f,f->curr_log);	//compute the current physical 
	      //kprintf("f->curr_blk:%d\n\r",f->curr_blk);
		  		
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      f->filepos = (inode->size) + offset;
	      f->bufpos = (offset) - (computed_block_number * f->bufsize); 
	      break;
		  			
		  			
	    case 2:
	      /* In this case  (offset > inode->size) */
	      //kprintf("*********************************\n\r");
	      //kprintf("Inside case SEEK_CUR - 2...\n\r");
	      //kprintf("*********************************\n\r");
	      //
	      //kprintf("f->filepos:%d\n\r",f->filepos);
   					
	      f->curr_log = inode->size / f->bufsize;	//current logical block
   			
	      //kprintf("f->curr_log:%d\n\r",f->curr_log);
	      f->curr_blk = sfs_log2phys(f,f->curr_log);	//compute the current physical 
	      //kprintf("f->curr_blk:%d\n\r",f->curr_blk);
		  		
	      blk_dev[f->major].read_fn(f->minor, f->curr_blk, f->buffer, sfs_fd->sb->sectorsperblock);
	      f->dirty = 0;
		      		
	      f->filepos = (inode->size) + offset;
	      f->bufpos = (offset) - (computed_block_number * f->bufsize); 
		      	
	      break;
		  			
		  				
	      //===========
	      break;		  				
	    }
	  break;
	}
    } 
	
  /* The following segment has been added for testing purposes */
  char *buf;
  t = f->bufpos;
  kprintf("~~~~~~~~~~~~~~~~~~~~~~\n\r");
  kprintf("PRINTING FROM BUFFER\n\r");
  while(t <= f->bufsize)
    {
      kprintf("%c",f->buffer[t++]);
    }
  kprintf("\n\r");
  kprintf("~~~~~~~~~~~~~~~~~~~~~~\n\r");
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
	
  //=================================================================
  /* DO NOT DELETE THIS PART - VALUE TO RETURN.
     Upon sucessful completion return the resulting offset location
     measured in bytes from the beginning of the file */
  return f->filepos;
	
}

