
/* this file contains the definitions of the mount point structure
   and helper routines for dealing with mount points */

#ifndef MOUNT_H
#define MOUNT_H

#include <stdint.h>

struct vfops;  /* need this here to avoid circular include */

typedef struct mp{
  char *source;           /* this is the path to the device or file */
  char *target;           /* this is the mount location.. ends in '/' */
  uint32_t major;         /* device major number */
  uint32_t minor;         /* device minor number */
  uint32_t open_count;    /* how many files are open? */
  uint32_t flags;         /* mount flags */   
  uint32_t fstype;        /* an index into the global array of filesystem
			     operations */
  struct mp* next;        /* pointer to next mount point in the list */
  struct vfops *ops;      /* direct pointer to the ops */
  void *fs_private;       /* pointer to filesystem specific private data 
			     for this mount point */
}mount_point;

extern mount_point *mounts;

mount_point *vfs_get_mp(char *path);

/* look for a mount point that exactly matches the given source path */
mount_point *vfs_get_mp_source(char *path);

/* Find the mount point for filesystem containing 
   the given file path. */
mount_point* vfs_lookup(const char* path);

/* allocate a mount_point structure */
mount_point *vfs_new_mp();

/* free a mount_point structure */
void vfs_delete_mp(mount_point *mp);


#endif
