

#ifndef SFS_H
#define SFS_H

#include <vfs_fsops.h>
#include <vfs_mp.h>
#include <sfs_private.h>

void sfs_init(vfs_fs_ops *ops);

sfs_inode_t *fs_inode_from_fd(filedesc *f);

uint64_t sfs_get_num_blocks(filedesc *f);

  void* sfs_mount(uint32_t major, 
		    uint32_t minor,
		    unsigned long mountflags,
		    const void *data);

  int sfs_umount(void *fs_private);

  int sfs_close(filedesc *f); 
  
  int sfs_creat(mount_point *mp,
           filedesc *fd,
		   char *path, 
		   mode_t mode);

  int sfs_lseek(filedesc *f, 
		  off_t offset, 
		  int whence);

  int sfs_open(mount_point *mp,
		 filedesc *fd,
		 char *path, 
		 int flags, 
		 mode_t mode);

  int sfs_read(filedesc *f, 
		 void* buffer, 
		 size_t count);

  int sfs_unlink(void *fs_private,     
		   char *path);

  int sfs_write(filedesc *f, 
		  void* buffer, 
		  size_t count);

  int sfs_mknod(void *fs_private,
		  char *path, 
		  mode_t mode, 
		  dev_t dev);

  int sfs_fchmod(filedesc *f,
		  mode_t mode);

  int sfs_fchown(filedesc *f,
		  uid_t owner,
		  gid_t group);

  int sfs_link(void *fs_private,
		 const char *oldpath, 
		 const char *newpath);

  int sfs_rename(void *fs_private,
		   const char *oldpath, 
		   const char *newpath);

  int sfs_fstat(filedesc *f, 
		 struct fstat *buf);

  int sfs_mkdir(void *fs_private,
		  const char *pathname, 
		  mode_t mode);

  int sfs_rmdir(void *fs_private,
		  const char *pathname);

  int sfs_dup(filedesc *f);

  int sfs_fcntl(filedesc *f, int cmd, ...); 

#endif
