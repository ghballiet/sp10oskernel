
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

int sfs_open(mount_point *mp,filedesc *fd,char *path,int flags, mode_t mode)
{

  sfs_inode_t *inode;
  int32_t inum;
  sfs_mp_private *p = mp->fs_private;

  if((fd->fs_private = (void *)kmalloc(sizeof(sfs_fd_private)))==NULL)
    return -1;
  
  if((inode = (sfs_inode_t*)kmalloc(sizeof(sfs_inode_t)))==NULL)
    return -1;

  inum = sfs_lookup(mp,path,inode);
  
  if(inum < 0)
    { /* lookup failed.. should we create? */
      if((flags&(O_CREAT|O_TRUNC))&&
	 ((flags&O_WRONLY)||(flags&O_RDWR)))
	{ /* create the file with zero length */
	  /* open the parent directory for append, if open fails, we fail */
	  sfs_inode_t *pinode;
	  uint32_t pinum;
	  char *ppath = strdup(path);
	  uint32_t index = strlen(ppath)-1;

	  /* strip off any trailing '/' chars */
	  while((index>0)&&(ppath[index]=='/'))
	    ppath[index--]=0;

	  /* search for last '/' char */
	  while((index>0)&&(ppath[index]!='/'))
	    index--;

	  char *newname;
	  int emptyparent = 0;
	  if(ppath[index] != '/')
	    {
	      emptyparent = 1;
	      newname = ppath;
	    }
	  else
	    {
	      ppath[index] = 0;
	      newname = ppath+index+1;
	    }

	  while(*newname == '/')
	    newname++;

	  pinode = (sfs_inode_t*)kmalloc(sizeof(sfs_inode_t));
	  if(emptyparent)
	    pinum = sfs_lookup(mp,"",pinode);
	  else
	    pinum = sfs_lookup(mp,ppath,pinode);

	  if((pinode->type != FT_DIR))
	    {
	      kfree(pinode);
	      kfree(inode);
	      kfree(ppath);
	      return -1;
	    }
	  /* open parent directory*/
	  if(sfs_openinode(mp, fd, pinum, pinode,O_WRONLY|O_APPEND,06)!=0)
	    {
	      kfree(pinode);
	      kfree(inode);
	      kfree(ppath);
	      return -1;
	    }
	  /* allocate a new inode for the file we are creating */
	  inum = first_cleared(p->free_inode_bitmap,p->super->num_inodes);
	  if(inum<0)
	    {
	      kfree(pinode);
	      kfree(inode);
	      kfree(ppath);
	      return -1;
	    }
	  set_bit(p->free_inode_bitmap,inum);
	  /* write the inode bitmap */
	  blk_dev[mp->major].write_fn(mp->minor,
				      p->super->free_inode_bitmap,
				      (char*)p->free_inode_bitmap,
				      p->super->free_inode_bitmapblocks*
				      p->super->sectorsperblock);
	  /* write new dir entry */
	  sfs_dirent d;
	  strncpy(d.name,newname,SFS_NAME_MAX);
	  d.inode = inum;

	  /* swap bytes if we are on big-endian machine*/
	  byteswap32(&(d.inode));

	  sfs_write(fd, (char *)&d, sizeof(sfs_dirent));
	  /* close parent directory */
	  sfs_close(fd); /* this should also free pinode */
	  kfree(ppath);

	  /* initialize new inode */
	  inode->owner = 0;
	  inode->group = 0;
	  inode->ctime = 2;
	  inode->mtime = 3;
	  inode->atime = 4;
	  inode->perm = 0777;
	  inode->type = FT_NORMAL;
	  inode->size = 0;
	  inode->refcount = 0;
	  int aj;
	  for(aj=0;aj<NUM_DIRECT;aj++)
	    inode->direct[aj] = 0;
	  inode->indirect = 0;
	  inode->dindirect = 0;
	  inode->tindirect = 0;
	  

	  sfs_put_inode(mp,inum,inode);

	  /* now let's try the lookup again... */
	  inum = sfs_lookup(mp,path,inode);
	  if(inum < 0)/* lookup failed.. something really wrong...*/
	    {
	      kfree(inode);
	      return -1;
	    }
	}
      else  /* file not found */
	return -1;
    }

  if(sfs_openinode(mp, fd, inum, inode,flags,mode)==0)
    {
      if((flags&O_TRUNC)&&((flags&O_WRONLY)||(flags&O_RDWR)))
	sfs_trunc(fd);
      sfs_fd_private *tmp = (void*)kmalloc(sizeof(sfs_fd_private));
      tmp->sb = p->super;
      tmp->inode = inode;
      tmp->inum = inum;
      fd->fs_private = (void*)tmp;      
      return 0;
    }

  fd->fs_private=NULL;
  kfree(inode);
  return -1;
}
