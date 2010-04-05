
#include <sys/types.h>
#include <stdint.h> 
#include <sfs_inode.h>
#include <ffstat.h> 


int sfs_create_file(const char *pathname,int flags)
{
  /* split pathname to path/filename */
  /* lookup(path,&inode) */
  /* inode must be a directory */
  /* allocate inode2 */
  /* create directory entry in inode pointing to inode2 */
}

int sfs_chdir(const char *path);
int sfs_fchdir(int fd);

int sfs_mkdir(const char *pathname, mode_t mode)
{
  /* like open, but return 0 on success and -1 on fail */
}

int sfs_rmdir(const char *pathname)
{
  /* like unlink, but checks to make sure that only the . and
     .. entries are in the directory */
}

int sfs_fstat(const char *path, struct fstat *buf)
{
  sfs_inode_t inode;
  if(sfs_lookup(path,&inode))
    {
      /*fill in the values for buf */
    }
}

/* no symbolic links yet */
/* int lfstat(const char *path, struct fstat *buf); */

int sfs_link(const char *oldpath, const char *newpath)
{
  sfs_inode_t inode;
  sfs_inode_t inode2;
  if(sfs_lookup(oldpath,&inode))
    {
      if(sfs_lookup(newpath,&inode2))
	{
	  /* replace directory entry */
	}
      else
	{
	  /* create directory entry */
	}
    }
}

int sfs_unlink(const char *pathname)
{
  sfs_inode_t inode;
  if(!sfs_lookup(pathname,&inode))
    {
      /* remove directory entry */
      /* decrease refcount in inode */
      /* if refcount is 0, then release all blocks and release the inode */
    }
}


int sfs_mknod(const char *pathname, mode_t mode, dev_t dev)
{
  sfs_inode_t inode;
  if(sfs_lookup(pathname,&inode))
    {
      /* fail with EEXIST */
    }
  else
    {
      /* allocate inode */
      /* fill in values */
      /* create dir entry */
    }
}

