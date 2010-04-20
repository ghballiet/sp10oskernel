#ifdef _KERNEL_
#include <misc.h>
#include <kprintf.h>
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

int sfs_rename(void *fs_private, const char *oldpath, const char *newpath)
{
  kprintf("sfs_rename() function has been implemented\n\r");
  /*filedesc* fd = (filedesc *)fs_private;*/
  mount_point *mp = (mount_point *)fs_private;
  /*kprintf("Oldpath: %s\n\r",mp->target);
  mp->target = newpath;
  kprintf("Newpath: %s\n\r",mp->target);
  return 1;*/

  int inum,ret = 1;
  filedesc* fd;
  int found, count;
  sfs_dirent dent;
  sfs_inode_t *inode=(sfs_inode_t*)kmalloc(sizeof(sfs_inode_t));
  kprintf("SFS oldname newname : %s %s\n\r",oldpath,newpath);
  if((inum=sfs_lookup(mp,oldpath,inode))<0)
  {
	kprintf("File not found\n\r");
	ret=-1;
  }
	else
  if(inode->type == FT_DIR)
  {
	ret=-1; //Not a directory
	kprintf("Not a File\n\r");
  }
  else if(inode->type == FT_NORMAL)
  {
	kprintf("Rename the file\r\n");
	/*int fdnum;
	if((fdnum=alloc_fd())<0)
	{
	        kfree(inode);
		return -1;
		}*/
		//First open the inode
	//fd=fdptr(fdnum);
        kprintf("INUM : %d\n\r",inum);
        fd = (filedesc *)kmalloc(sizeof(filedesc));
        fd->fs_private = (sfs_fd_private *)kmalloc(sizeof(sfs_fd_private));
        //sfs_get_inode(mp,inum,inode);
        sfs_get_inode(mp,0,inode);
	kprintf("Opening inode\r\n");
  
        //found = sfs_openinode(mp,fd,inum,inode,O_RDONLY,0);
        found = sfs_openinode(mp,fd,0,inode,O_RDONLY,0);
	kprintf("value of sfs_openinode:%d\n\r",found);
            sfs_mp_private *p;          
            found = 0;
	    //fd->mp = mp;  //assigning mp to the current fd
            /* perform reads using that file descriptor until eof or found */
	    while(!found &&(count = fd->mp->ops->read_fn(fd,&dent,sizeof(sfs_dirent)))== sizeof(sfs_dirent))
           {
	     
	     //fd->mp->ops->read_fn(fd,&dent,sizeof(sfs_dirent));

                kprintf("Previous DENT.NAME: %s\n\r",dent.name);
                /* swap bytes if we are on big-endian machine*/
                byteswap32(&(dent.inode));
                if(strcmp(oldpath,dent.name)==0)
                {
                    found = 1;
                    kprintf("Previous DENT.NAME: %s\n\r",dent.name);
                    newpath +=  strlen(mp->target);
                    strncpy(dent.name,newpath,SFS_NAME_MAX);
                    dent.inode = inum;
                    /* swap bytes if we are on big-endian machine*/
                    //byteswap32(&(d.inode));

                    sfs_write(fd, (char *)&dent, sizeof(sfs_dirent));
                    /* close parent directory */
                    sfs_close(fd); /* this should also free pinode */
                    kprintf("DENT.NAME HAS BEEN CHANGED to: %s\n\r",dent.name);

                    //writing back inode;
                    sfs_put_inode(mp,inum,inode);
                }
           }

             /* now let's try the lookup again... */
	  inum = sfs_lookup(mp,newpath,inode);
	  if(inum < 0)/* lookup failed.. something really wrong...*/
	    {
	      kfree(inode);
              kprintf("Look up failed for the newpath\n\r");
	      return -1;
	    }
		  
	  //	}
	  //free_fd(fdnum);
          kfree(fd);
   }
   else
   {
	ret=-1;
	kprintf("Rename operation failed\r\n");
   }
   kfree(inode);
   return ret;
  
}
