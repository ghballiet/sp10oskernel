

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

#include <stddef.h>
#include <blkdev.h>
#include <vfs_mp.h>
#include <vfs_filedesc.h>
#include <vfs_fsops.h>


/* unmount a filesystem */
int vfs_umount(const char *path)
{
	kprintf("I AM HERE 1\n\r");
  	int rval = -1;
  	mount_point *l;
  	mount_point *m = mounts;

  	if(!strcmp(path,mounts->target))
  	{
	  	kprintf("I AM HERE 2\n\r");
	  	kprintf("path %s\n\r", path );
    	kprintf("mounts->target %s\n\r", mounts->target);
    	
      mounts=mounts->next;
      vfs_delete_mp(m);
      rval = 0;
	}
  	
	else
   {
    	kprintf("I AM HERE 3\n\r");
      l=m;
      kprintf("Before \n\r");
      m=m->next;
      if(m == NULL)
      {
      	kprintf("After \n\r");
      }
      kprintf("Target:%s\n\r",m->target);
      while((m!=NULL)&&(strcmp(path,m->target)))
		{
	  		kprintf("Inside while loop...");
	  		l = m;
	  		m = m->next;
		}
      
		if(m!=NULL)
		{
			kprintf("I AM HERE 4\n\r");
	  		if(m->open_count > 0)
	  		{
		  		kprintf("m->open_count :%u\n\r", m->open_count); 
	  		   kprintf("UMOUNT WILL NOT BE EXECUTED -AT LEAST ONE FILE IS OPEN\n\r");
	  		   rval = -1;
	  		}
	   
	   	//===================================================
	  		/*	
	   		int i;
			for(i=0;i<NUM_FD;i++)
			{
    			if(fdesc[i].in_use != 0)
    			{
    				kprintf("UMOUNT WILL NOT BE EXECUTED -AT LEAST ONE FILE IS OPEN\n\r");
    				rval = -1;
    			}
   		}*/
   		//===================================================
   	
	  		else
	  		{	
		  		//if(rval != -1)
	  			//{
		  			l->next = m->next;
	      		/* call the filesystem unmount function */
	      		m->ops->umount_fn(m);
	      
	      		/* delete the mount point structure */
	      		vfs_delete_mp(m);
	      		rval = 0;
	   		//}
	   	}
		}
    }
    m->open_count = 0;
  kprintf("m->open_count[2]:%u\n\r", m->open_count);   
  kprintf("umount return value is :%u\n\r", rval);  
  return rval;
}

