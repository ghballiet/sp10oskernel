

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


/* look for a mount point that exactly matches path */
mount_point *vfs_get_mp(char *path)
{
  mount_point *m = mounts;
  while((m!=NULL)&&(strcmp(path,m->target)))
    m=m->next;
  return m;
}

/* look for a mount point that exactly matches the given source path */
mount_point *vfs_get_mp_source(char *path)
{
  mount_point *m = mounts;
  while((m!=NULL)&&(strcmp(path,m->source)))
    m=m->next;
  return m;
}

/* Find the mount point for filesystem containing 
   the given file path. */
mount_point* vfs_lookup(const char* path)
{
  mount_point *m = mounts;
  mount_point *best = NULL;
  int bestlen = 0;
  int n,match;
  /* search through mount points to find the longest exact match
     for leading part of our path.  That can save us from having
     to search through filesystems that are not important. */
  while(m!=NULL)
    {
      n = strlen(m->target);
      match = strncmp(m->target,path,n);
      if((!match)&&(n>bestlen))
	{
	  bestlen = n;
	  best = m;
	}
      m = m->next;
    }
  return best;
}

/* allocate a mount_point structure */
mount_point *vfs_new_mp()
{
  mount_point *mp;
  if((mp = (mount_point *)kmalloc(sizeof(mount_point)))==NULL)
    kprintf("unable to allocate kernel memory\n");
  else
    {
      mp->source = mp->target = NULL;
      mp->fstype = mp->flags = mp->major = mp->minor = mp->open_count = 0;
      mp->next = NULL;
    }
  return mp;
}

/* free a mount_point structure */
void vfs_delete_mp(mount_point *mp)
{
  uint32_t major = mp->major;
  uint32_t minor = mp->minor;

  kfree(mp->source);
  kfree(mp->target);
  kfree(mp);
}


