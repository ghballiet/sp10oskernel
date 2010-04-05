

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
  int rval = -1;
  mount_point *l;
  mount_point *m = mounts;

  if(!strcmp(path,mounts->target))
    {
      mounts=mounts->next;
      vfs_delete_mp(m);
      rval = 0;
    }
  else
    {
      l=m;
      m=m->next;
      while((m!=NULL)&&(strcmp(path,m->target)))
	{
	  l = m;
	  m = m->next;
	}
      if(m!=NULL)
	{
	  if(m->open_count > 0)
	    rval = -1;
	  else
	    {
	      l->next = m->next;
	      /* call the filesystem unmount function */
	      
	      /* delete the mount point structure */
	      vfs_delete_mp(m);
	      rval = 0;
	    }
	}
    }
  return rval;
}

