
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

int sfs_creat(void *fs_private, char *path, mode_t mode)
{
  kprintf("sfs_creat() function not implemented\n\r");

  /* sfs_inode_t *inode; */
  /* int32_t inum; */

  /* sfs_mp_private *mp = (sfs_mp_private *)fs_private; */
  /* /\* NOTE: he said in class that a lot of the stuff we need to do here is */
  /*    already implemnted in 'open'. On first reading, it looks like open creates */
  /*    a file if we try to open a file that does not exist. *\/ */
  /* /\* I.e., the 'find the parent directory path' code from 'open' will work here */
  /*    as-is *\/ */

  /* /\* NOTE: How do we get the FD from fs_private? *\/ */

  /* /\* NOTE: Copied from sfs_open.c *\/ */

  /* /\* create the file with zero length *\/ */
  /* /\* open the parent directory for append, if open fails, we fail *\/ */
  /* sfs_inode_t *pinode; */
  /* uint32_t pinum; */
  /* char *ppath = strdup(path); */
  /* uint32_t index = strlen(ppath)-1; */

  /* /\* strip off any trailing '/' chars *\/ */
  /* while((index>0)&&(ppath[index]=='/')) */
  /*   ppath[index--]=0; */

  /* /\* search for last '/' char *\/ */
  /* while((index>0)&&(ppath[index]!='/')) */
  /*   index--; */

  /* char *newname; */
  /* int emptyparent = 0; */
  /* if(ppath[index] != '/') */
  /*   { */
  /*     emptyparent = 1; */
  /*     newname = ppath; */
  /*   } */
  /* else */
  /*   { */
  /*     ppath[index] = 0; */
  /*     newname = ppath+index+1; */
  /*   } */

  /* while(*newname == '/') */
  /*   newname++; */

  /* pinode = (sfs_inode_t*)kmalloc(sizeof(sfs_inode_t)); */
  /* if(emptyparent) */
  /*   pinum = sfs_lookup(mp,"",pinode); */
  /* else */
  /*   pinum = sfs_lookup(mp,ppath,pinode); */

  /* if((pinode->type != FT_DIR)) */
  /*   { */
  /*     kfree(pinode); */
  /*     kfree(inode); */
  /*     kfree(ppath); */
  /*     return -1; */
  /*   } */
  /* /\* open parent directory*\/ */
  /* if(sfs_openinode(mp, fd, pinum, pinode,O_WRONLY|O_APPEND,06)!=0) */
  /*   { */
  /*     kfree(pinode); */
  /*     kfree(inode); */
  /*     kfree(ppath); */
  /*     return -1; */
  /*   } */
  /* /\* allocate a new inode for the file we are creating *\/ */
  /* inum = first_cleared(p->free_inode_bitmap,p->super->num_inodes); */
  /* if(inum<0) */
  /*   { */
  /*     kfree(pinode); */
  /*     kfree(inode); */
  /*     kfree(ppath); */
  /*     return -1; */
  /*   } */
  /* set_bit(p->free_inode_bitmap,inum); */
  /* /\* write the inode bitmap *\/ */
  /* blk_dev[mp->major].write_fn(mp->minor, */
  /* 			      p->super->free_inode_bitmap, */
  /* 			      (char*)p->free_inode_bitmap, */
  /* 			      p->super->free_inode_bitmapblocks* */
  /* 			      p->super->sectorsperblock); */
  /* /\* write new dir entry *\/ */
  /* sfs_dirent d; */
  /* strncpy(d.name,newname,SFS_NAME_MAX); */
  /* d.inode = inum; */

  /* /\* swap bytes if we are on big-endian machine*\/ */
  /* byteswap32(&(d.inode)); */

  /* sfs_write(fd, (char *)&d, sizeof(sfs_dirent)); */
  /* /\* close parent directory *\/ */
  /* sfs_close(fd); /\* this should also free pinode *\/ */
  /* kfree(ppath); */

  /* /\* initialize new inode *\/ */
  /* inode->owner = 0; */
  /* inode->group = 0; */
  /* inode->ctime = 2; */
  /* inode->mtime = 3; */
  /* inode->atime = 4; */
  /* inode->perm = 0777; */
  /* inode->type = FT_NORMAL; */
  /* inode->size = 0; */
  /* inode->refcount = 0; */
  /* int aj; */
  /* for(aj=0;aj<NUM_DIRECT;aj++) */
  /*   inode->direct[aj] = 0; */
  /* inode->indirect = 0; */
  /* inode->dindirect = 0; */
  /* inode->tindirect = 0; */
	  

  /* sfs_put_inode(mp,inum,inode); */

  /* /\* now let's try the lookup again... *\/ */
  /* inum = sfs_lookup(mp,path,inode); */
  /* if(inum < 0)/\* lookup failed.. something really wrong...*\/ */
  /*   { */
  /*     kfree(inode); */
  /*     return -1; */
  /*   } */
}

