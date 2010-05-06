

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

/* At system initialization, we have no filesystem mounted, so cannot
   look for a device file to mount as root.  Instead, we use the
   major and minor number of the root filesystem to mount it.  So,
   the kernel has to "know" those numbers in order to mount root.

   We may also need to call this again if the root filesystem does
   not contain /dev  We will just require that the root filesystem
   contain /dev and that dev have any device files needed to mount
   the remaining filesystems.
*/
int vfs_mount_dev(uint32_t major, uint32_t minor, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 const void *data)
{
  vfs_fs_ops *ops;
  mount_point *mp;

  /* look up the filesystem ops structure for the filesystem type */
  ops = get_fs_ops(filesystemtype);
  if(ops == NULL)
    {
      kprintf("Unknown filesystem type.\n\r");
      return -1;
    }

  /* allocate a new mount point structure */
  /* if(!strcmp("/",target))  /\* are we mounting root? *\/ */
  /* if((mp=vfs_get_mp("/"))!=NULL) /\* if so, and it is already mounted *\/ */
  /* vfs_unmount(mp->target);     /\* then unmount it first *\/ */
  mp = vfs_new_mp();
  mp->next = mounts;
  mounts=mp;

  /* fill in the mount point information */
  mp->ops = ops;
  mp->source = strdup("");
  mp->target = strdup(target);
  mp->fstype = ops->fstype;
  mp->flags = mountflags;
  mp->major = major;
  mp->minor = minor;

  /* call the filesystem mount function and get a pointer to its private
     data */
  mp->fs_private = mp->ops->mount_fn(major,minor,mountflags,data);
  if(mp->fs_private==NULL)
    return -2;
  return 0;
}

/* Once we have /dev mounted, we can use the device files in /dev to
   mount other filesystems in the filsystem tree.  This is the
   "standard" mount function.

   It searches the filesystem for "source" and finds the appropriate
   block device driver, then calls the mount routine for the specified
   filesystem type.
*/
int vfs_mount(const char *source, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 const void *data)
{
  kprintf("vfs_mount() not implemented!\n\r");
}

