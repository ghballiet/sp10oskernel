
#include <vfs_fsops.h>
#include <stddef.h>

vfs_fs_ops fs_ops[NUM_FS_TYPES];

void vfs_fsops_init()
{
  int i;
  for(i=0;i<NUM_FS_TYPES;i++)
    {
      fs_ops[i].fstype_str = NULL;
      fs_ops[i].fstype = -1;

      fs_ops[i].mount_fn=NULL;
      fs_ops[i].umount_fn=NULL;
      fs_ops[i].close_fn=NULL;
      fs_ops[i].creat_fn=NULL;
      fs_ops[i].lseek_fn=NULL;
      fs_ops[i].open_fn=NULL;
      fs_ops[i].read_fn=NULL;
      fs_ops[i].unlink_fn=NULL;
      fs_ops[i].write_fn=NULL;
      fs_ops[i].mknod_fn=NULL;
      fs_ops[i].fchmod_fn=NULL;
      fs_ops[i].fchown_fn=NULL;
      fs_ops[i].link_fn=NULL;
      fs_ops[i].rename_fn=NULL;
      fs_ops[i].fstat_fn=NULL;
      fs_ops[i].mkdir_fn=NULL;
      fs_ops[i].rmdir_fn=NULL;
      fs_ops[i].dup_fn=NULL;
      fs_ops[i].fcntl_fn=NULL; 
    }
}

vfs_fs_ops *get_fs_ops(const char *s)
{
  int i;
  for(i=0;i<NUM_FS_TYPES;i++)
    if(!strcmp(s,fs_ops[i].fstype_str))
      return &fs_ops[i];
  return NULL;
}


