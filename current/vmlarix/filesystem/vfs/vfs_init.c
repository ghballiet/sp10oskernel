#include <stddef.h>
#include <vfs_filedesc.h>
#include <vfs_fsops.h>



mount_point *mounts=NULL;

filedesc fdesc[NUM_FD];

vfs_fs_ops fs_ops[NUM_FS_TYPES] ;

void vfs_init()
{
  int i;

  /* initialize the file descriptor table */
  vfs_init_filedesc();
  
  /* call the init functions for all of the filesystem drivers,
     and have them fill in their own fs_ops structure. */
  sfs_init(&(fs_ops[FS_TYPE_SFS]));

}

