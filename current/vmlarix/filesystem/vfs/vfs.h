
#ifndef VFS_H
#define VFS_H

#include <sys/types.h>
#include <stdint.h>
#include <fstat.h>

/* defines for different types of files */
#define FT_NORMAL 0
#define FT_DIR 1
#define FT_CHAR_SPEC 2
#define FT_BLOCK_SPEC 3
#define FT_PIPE 4
#define FT_SOCKET 5

/* vfs_init calls the init() functions for all of the filesystem
   drivers.  The result is that each filesystem driver is "registered"
   with the vfs. Basically, this means that the vfs has a pointer to
   the filesystem specific mount, umount, open, read, etc for each
   registered filesystem, along with a pointer to the string
   containing the filesystem type for each filesystem.  This string
   is used to determine which filesystem driver should be used
   for a mount command.

   vfs_init also initializes the global file descriptor table, by
   calling fdesc_init();
*/
void vfs_init();

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
                 const void *data);

/* Once we have /dev mounted, we can use the device files in /dev to
   mount other filesystems in the filsystem tree.  This is the
   "standard" mount function.

   It searches the filesystem for "source" and finds the appropriate
   block device driver, then calls the mount routine for the specified
   filesystem type.
*/
int vfs_mount(const char *source, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 const void *data);

/* vfs_umount calls the unmount function for the filesystem that
   is mounted at "target" 
*/
int vfs_umount(const char *target);

/* the following are passed on to the correct filesystem driver */
int vfs_close(int fd);
int vfs_creat(char *path, mode_t mode);
int vfs_lseek(int fd, off_t offset, int whence);
int vfs_open(char *path, int flags, mode_t mode);
int vfs_read(int fd, void* buffer, size_t count);
int vfs_unlink(char *path);
int vfs_write(int fd, void* buffer, size_t count);
int vfs_mknod(char *path, mode_t mode, dev_t dev);
int vfs_fchmod(int fd, mode_t mode);
int vfs_fchown(int fd, uid_t owner, gid_t group);
int vfs_link(const char *oldpath, const char *newpath);
int vfs_rename(const char *oldpath, const char *newpath);
int vfs_fstat(int fd, struct fstat *buf);
int vfs_mkdir(const char *pathname, mode_t mode);
int vfs_rmdir(const char *pathname);
int vfs_dup(int oldfd);
int vfs_fcntl(int fd, int cmd, ... );

/*  The following functions are implemented in the VFS layer, and not
    by each filesystem driver: */ 
int vfs_pipe(int pipefd[2]);
mode_t vfs_umask(mode_t mask);


#endif
