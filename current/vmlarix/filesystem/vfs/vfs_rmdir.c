//Author: Ashish Parajuli
#include <stddef.h>
#include <vfs.h>
#include <vfs_filedesc.h>
#include <vfs_fsops.h>

#ifdef _KERNEL_
#include <misc.h>
#else
#include <stdlib.h>
#include <string.h>
#define kmalloc malloc
#endif

int vfs_rmdir(const char *pathname)
{
	int fd,result;
	/* find the mount point */
	mount_point *mp;
	if((mp = vfs_lookup(pathname))==NULL)
	{
		/* errno = EFNF; file not found */
		return -1;
	}
	pathname += strlen(mp->target); /* strip off the mount point */
	
	result = mp->ops->rmdir_fn(mp,pathname);
	return result;
}
