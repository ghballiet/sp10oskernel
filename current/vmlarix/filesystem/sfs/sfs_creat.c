
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

  sfs_mp_private *p = (sfs_mp_private *)fs_private;
  /* NOTE: he said in class that a lot of the stuff we need to do here is
     already implemnted in 'open'. On first reading, it looks like open creates
     a file if we try to open a file that does not exist. */
  /* I.e., the 'find the parent directory path' code from 'open' will work here
     as-is */

  /* NOTE: Copied from sfs_open.c */

  /* create the file with zero length */
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
    pinum = sfs_lookup(p,"",pinode);
  else
    pinum = sfs_lookup(p,ppath,pinode);
  
}

