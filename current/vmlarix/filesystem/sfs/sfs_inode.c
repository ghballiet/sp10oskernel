
/* #include <superblock.h> */
/* #include <string.h> */


#ifdef _KERNEL_
#include <misc.h>
#include <kmalloc.h>
#else
#include <stdlib.h>
#include <string.h>
#define kmalloc malloc
#define kfree free
#endif

#include <blkdev.h>
#include <stddef.h>
#include <fcntl.h>
#include <vfs_filedesc.h>
#include <vfs_mp.h>
#include <vfs_fsops.h>
#include <sfs_dir.h>
#include <sfs_inode.h>
#include <sfs_private.h>
#include <byteswap.h>


sfs_inode_t *sfs_inode_from_fd(filedesc *f) {
  /* declared in sfs.h */
  return ((sfs_fd_private *)f->fs_private)->inode;
}

void sfs_get_inode(mount_point *mp,int inum,sfs_inode_t *inode)
{
  int i;
  sfs_mp_private *p = mp->fs_private;
  sfs_inode_t *inode_block = (sfs_inode_t *)kmalloc(p->super->block_size);
  int block,index;
  /* calculate which block inum is in */
  int inodesperblock = p->super->block_size/sizeof(sfs_inode_t);
  block = p->super->inodes + inum/inodesperblock;
  index = inum % inodesperblock;
  /* read the block from disk */
  blk_dev[mp->major].read_fn(mp->minor,block,(char *)inode_block,
			     p->super->sectorsperblock);
  /* copy out the inode data */
  *inode = inode_block[index];
  /* swap bytes to deal with big/little endianness issues*/
  /* inodes on disk are stored little-endian. If we are on a little-endian
     machine, the preprocessor removes all these byte-swap calls. */
  byteswap32(&(inode->owner));
  byteswap32(&(inode->group));
  byteswap32(&(inode->ctime));
  byteswap32(&(inode->mtime));
  byteswap32(&(inode->atime));
  byteswap16(&(inode->perm));
  byteswap64(&(inode->size));
  for(i=0;i<NUM_DIRECT;i++)
    byteswap32(&(inode->direct[i]));
  byteswap32(&(inode->indirect));
  byteswap32(&(inode->dindirect));
  byteswap32(&(inode->tindirect));

  kfree(inode_block);  
}



void sfs_put_inode(mount_point *mp,int inum,sfs_inode_t *inode)
{
  int i;
  sfs_mp_private *p = mp->fs_private;
  sfs_inode_t *inode_block = (sfs_inode_t *)kmalloc(p->super->block_size);
  int block,index;
  /* calculate which block inum is in */
  int inodesperblock = p->super->block_size/sizeof(sfs_inode_t);
  block = p->super->inodes + inum/inodesperblock;
  index = inum % inodesperblock;
  /* read the block from disk */
  blk_dev[mp->major].read_fn(mp->minor,block,(char *)inode_block,
			     p->super->sectorsperblock);

  /* set the inode data */
  inode_block[index] =  *inode;

  /* swap bytes to deal with big/little endianness issues*/
  /* inodes on disk are stored little-endian. If we are on a little-endian
     machine, the preprocessor removes all these byte-swap calls. */
  byteswap32(&(inode_block[index].owner));
  byteswap32(&(inode_block[index].group));
  byteswap32(&(inode_block[index].ctime));
  byteswap32(&(inode_block[index].mtime));
  byteswap32(&(inode_block[index].atime));
  byteswap16(&(inode_block[index].perm));
  byteswap64(&(inode_block[index].size));
  for(i=0;i<NUM_DIRECT;i++)
    byteswap32(&(inode_block[index].direct[i]));
  byteswap32(&(inode_block[index].indirect));
  byteswap32(&(inode_block[index].dindirect));
  byteswap32(&(inode_block[index].tindirect));


  /* write block to disk */
  blk_dev[mp->major].write_fn(mp->minor,block,(char *)inode_block,
			      p->super->sectorsperblock);
  kfree(inode_block);  
}


/* perform an open on a file descriptor with a given inode. return 0
   on success. -1 on fail. */
int sfs_openinode(mount_point *mp, filedesc *f, 
		  uint32_t inum, sfs_inode_t *inode,
		  uint32_t flags,uint32_t mode)
{
  sfs_mp_private *p = mp->fs_private;
  sfs_fd_private *fp = f->fs_private;
  fp->sb = p->super;
  fp->inum = inum;
  fp->inode = inode;
  /*   sfs_file_ops *fops;/\* function pointers for the filsystem *\/ */
  f->mp = mp;
  f->flags = flags;     /* read/write mode, buffered, unbuffered, etc */
  f->mode = mode;     /* read/write mode, buffered, unbuffered, etc */
  f->major = mp->major;    /* major number of device this file is on */
  f->minor = mp->minor;    /* minor number of device this file is on */
  f->bufsize = fp->sb->block_size;  /* size of I/O buffer */
  f->buffer = (char *)kmalloc(f->bufsize);   /* I/O buffer for this file */
  if(f->buffer == NULL)
    return -1;
  f->bufpos = 0;   /* current read/write pointer (buffer position) */
  f->filepos = 0;  /* current read/write pointer (file position) */

  if(fp->inode->size!=0)
    {
      f->curr_log = 0;
      f->curr_blk = fp->inode->direct[0];
/*       printf("reading block %d for inode %d\n",f->curr_blk,f->inum); */
      blk_dev[f->major].read_fn(f->minor,
				f->curr_blk,
				f->buffer,
				fp->sb->sectorsperblock);
    }
  else
    {
      f->curr_blk = -1;
      f->curr_log = -1;
    }

  return 0;
}


int sfs_dir_lookup(mount_point *mp,int inum,char *currsearch)
{
  sfs_inode_t *inode = (sfs_inode_t*)kmalloc(sizeof(sfs_inode_t));
  uint32_t fp = 0;  /* pointer to next byte in the file */
  uint32_t bp = 0;  /* pointer to next block in the file */
  int count;
  int found;
  sfs_dirent dent;
  sfs_get_inode(mp,inum,inode);

  /* create an unnumbered file descriptor for this inode */
  filedesc *f = (filedesc *)kmalloc(sizeof(filedesc));
  f->fs_private = kmalloc(sizeof(sfs_fd_private));

  /* perform an open on that file descriptor with the given inode */
  sfs_openinode(mp, f, inum, inode,O_RDONLY,0); 

  found = 0;
  /* perform reads using that file descriptor until eof or found */  
  while(!found && 
	((count=f->mp->ops->read_fn(f,&dent,sizeof(sfs_dirent)))==
	 sizeof(sfs_dirent)))
    {
      /* swap bytes if we are on big-endian machine*/
      byteswap32(&(dent.inode));  
      if(strcmp(currsearch,dent.name)==0)
	found = 1;
    }
  
  sfs_close(f); /* also does kfree on the inode and the buffer */
  kfree(f->fs_private);
  kfree(f);

  if(!found)
    return -1;

  return dent.inode;
}


/* lookup the path, starting at a given mountpoint.  Fill in the
   sfs_inode_t structure and return the inode number.  Return -1 on
   failure */
int sfs_lookup(mount_point* mp,const char *path,sfs_inode_t *inode)
{
  /* start with the root inode for the filesystem */
  int32_t inum = 0;

  char *workpath = strdup(path);
  char *savefirst = workpath;
  char *currsearch;

/*   workpath+=strlen(mp->target); /\* strip off the mount point *\/ */

  /* if we want to open '/' or '' (the root directory) just return inode 0 */
  if(strcmp(path,"/")&&strcmp(path,""))
    {
      /* take first part of path, up to the first '/' and search the directory
	 described by inum. */
      while((workpath != NULL)&&(*workpath != 0)&&(inum >= 0))
	{
	  currsearch = workpath;
	  workpath = strchr(workpath,'/');
	  if(workpath != NULL)
	    *(workpath++) = 0;
	  inum = sfs_dir_lookup(mp,inum,currsearch);
	}
    }
  else
    inum = 0;

  if(inum >= 0)
    {
      sfs_get_inode(mp,inum,inode);
    }
  kfree(savefirst);
  return inum;
}