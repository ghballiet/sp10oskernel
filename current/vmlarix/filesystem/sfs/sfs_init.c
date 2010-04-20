

#include <sfs.h>
#include <vfs_fsops.h>
#include <stddef.h>

char *fstypestr="sfs";

void sfs_init(vfs_fs_ops *ops)
{
  /* fill in the fs_ops structure */
  ops->fstype_str=fstypestr;

  ops->mount_fn=sfs_mount;
  ops->umount_fn=sfs_umount;
  ops->close_fn=sfs_close;
  ops->creat_fn=sfs_creat;
  ops->lseek_fn=sfs_lseek;
  ops->open_fn=sfs_open;
  ops->read_fn=sfs_read;
  ops->unlink_fn=sfs_unlink;
  ops->write_fn=sfs_write;
  ops->mknod_fn=sfs_mknod;
  ops->fchmod_fn=sfs_fchmod;
  ops->fchown_fn=sfs_fchown;
  ops->link_fn=sfs_link;
  ops->rename_fn=sfs_rename;
  ops->fstat_fn=sfs_fstat;
  ops->mkdir_fn=sfs_mkdir;
  ops->rmdir_fn=sfs_rmdir;
  ops->dup_fn=sfs_dup;
  ops->fcntl_fn=sfs_fcntl; 

}

