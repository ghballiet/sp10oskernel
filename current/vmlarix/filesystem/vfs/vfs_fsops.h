
#ifndef VFS_FSOPS_H
#define VFS_FSOPS_H

#include <sys/types.h>
#include <stdint.h>
#include <vfs_mp.h>
#include <vfs_filedesc.h>
#include <fstat.h>

#define FS_TYPE_SFS  0
#define NUM_FS_TYPES 1

/* define a structure to hold function pointers for filesystem 
   operations.  The init function for each filesystem type will
   return one of these, that points to its filesystem-specific
   functions.  The following functions must be implemented for 
   each file system type:

  Function	POSIX Description
  close         Closes a file
  creat         Creates a new file or rewrites an existing one
  lseek         Repositions read/write file offset
  open 	        Opens a file
  read 	        Reads from a file
  unlink 	Removes a directory entry
  write 	Writes to a file

  mknod         Create blk or char special file (its use is discouraged)

  fchmod 	Changes file mode
  fchown 	Changes owner and/or group of a file
  link 	        Creates a link to a file
  rename 	Renames a file
  fstat 	        Gets information about a file

  mkdir 	Makes a directory
  rmdir 	Removes a directory

  dup 	        Duplicates an open file descriptor
  dup2 	        Duplicates an open file descriptor
  fcntl 	Manipulates an open file descriptor

  The following functions are implemented in the VFS layer, and not
    by each filesystem driver:
  pipe 	        Creates an interprocess channel 
  umask 	Sets the file creation mask (in the process record)

  The following functions are implemented in the user-level C library,
    using open, read, close, lseek, /etc/mtab, and knowledge of the
    directory structure for each filesystem type.
  opendir 	Opens a directory  (implemented in user C lib)
  readdir 	Reads a directory (implemented in user C lib)
  closedir 	Ends directory read operation (implemented in user C lib)
  rewinddir 	Resets the readdir() pointer (implemented in user C lib)

*/

typedef struct vfops{
  /* the filesystem specific mount function will return a pointer to
     any internal data structure it creates to manage the filesystem.
     All the other functions will get this pointer as their first
     parameter.
  */
  char *fstype_str; /* character string identifier for filesystem type */
  int fstype;       /* integer fs type */

  void* (*mount_fn)(uint32_t major, 
		    uint32_t minor,
		    unsigned long mountflags,
		    const void *data);

  int (*umount_fn)(void *fs_private);

  int (*close_fn)(filedesc *f); 
  
  int (*creat_fn)(mount_point *mp,     /* POSIX creat */
           filedesc *fd,
		   char *path, 
		   mode_t mode);

  int (*lseek_fn)(filedesc *f, 
		  off_t offset, 
		  int whence);

  int (*open_fn)(mount_point *mp,
		 filedesc *fd,
		 char *path, 
		 int flags, 
		 mode_t mode);

  int (*read_fn)(filedesc *f, 
		 void* buffer, 
		 size_t count);

  int (*unlink_fn)(void *fs_private,     
		   char *path);

  int (*write_fn)(filedesc *f, 
		  void* buffer, 
		  size_t count);

  int (*mknod_fn)(void *fs_private,
		  char *path, 
		  mode_t mode, 
		  dev_t dev);

  int (*fchmod_fn)(filedesc *f,
		  mode_t mode);

  int (*fchown_fn)(filedesc *f,
		  uid_t owner,
		  gid_t group);

  int (*link_fn)(void *fs_private,
		 const char *oldpath, 
		 const char *newpath);

  int (*rename_fn)(void *fs_private,
		   const char *oldpath, 
		   const char *newpath);

  int (*fstat_fn)(filedesc *f, 
		 struct fstat *buf);

  int (*mkdir_fn)(void *fs_private,
		  const char *pathname, 
		  mode_t mode);

  int (*rmdir_fn)(void *fs_private,
		  const char *pathname);

  int (*dup_fn)(filedesc *f);

  int (*fcntl_fn)(filedesc *f, int cmd, ...); 

}vfs_fs_ops;


/* In addition, each filesystem driver must provide a unique init
   function, which will be called by vfs_init(). The init function for
   all file system drivers must look like this: 
   int my_fs_init(*vfs_fs_ops ops); 
   The return value is 1 on success, 0 on failure.
*/
  
/* We will use the following array to store the ops for each
   filesystem type */
extern vfs_fs_ops fs_ops[NUM_FS_TYPES];

void vfs_fsops_init();

/* this function returns a pointer to the fs_ops structure for the
   given filesystem type */
vfs_fs_ops *get_fs_ops(const char *s);

#endif

