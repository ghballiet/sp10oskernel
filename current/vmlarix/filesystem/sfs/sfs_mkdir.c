//Author: Ashish Parajuli
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
 

#include <sfs.h>
#include <stddef.h>
#include <sys/types.h>
#include <fcntl.h>
#include <blkdev.h>
#include <vfs_filedesc.h>
#include <sfs_dir.h>
#include <sfs_private.h>
#include <byteswap.h>
#include <vfs_mp.h>
//#include <misc.h>

uint32_t get_parent_inum(mount_point* mp,const char* path)
{
	sfs_inode_t *pinode;
	uint32_t pinum;
	char *ppath = strdup(path);
	uint32_t index = strlen(ppath)-1;

	/* strip off any trailing '/' chars */
	while((index>0)&&(ppath[index]=='/'))
		ppath[index--]=0;

	/* search for last '/' char */
	while((index>0)&&(ppath[index]!='/'))
		index--;

	int emptyparent = 0;
	if(ppath[index] != '/')
		emptyparent = 1;
	else
		ppath[index] = 0;

	pinode = (sfs_inode_t*)kmalloc(sizeof(sfs_inode_t));
	if(emptyparent)
		pinum = sfs_lookup(mp,"",pinode);
	else
		pinum = sfs_lookup(mp,ppath,pinode);
	kfree(pinode);
	kfree(ppath);
	return pinum;
}


//int sfs_mkdir(void *fs_private, const char *pathname, mode_t mode)
int sfs_mkdir(mount_point *mp,filedesc *fd,const char *pathname, mode_t mode)
{
	
	sfs_inode_t *inode;
	int32_t inum,pinum;
	//kprintf("At mkdir Pathname %s\r\n",pathname);
	//Check to see if a directory already exists
	if((inode = (sfs_inode_t*)kmalloc(sizeof(sfs_inode_t)))==NULL)
		return -1;
	inum=sfs_lookup(mp,pathname,inode);
	//kprintf("Checking path Inode num: %d \r\n",inum);
	//Check FT_DIR or not
	if(inum>0 && inode->type==FT_DIR)
	{
		kfree(inode);
		return -1; //Directory already exists
	}
	//kprintf("No prev dir \r\n");
	//Create an empty file
	//if(sfs_open(mp,fd,pathname,O_CREAT|O_WRONLY,0)<0)
	if(sfs_creat(mp,fd,pathname,O_RDWR)<0)
	{	
		kfree(inode);
		return -1;
	}
	//kprintf("File created \r\n");
	//Get the inode and Change the type to FD_DIR	
	inum=sfs_lookup(mp,pathname,inode);
	//kprintf("Inode num %d \r\n",inum);
	if(inum<0)
	{
		kfree(inode);
		return -1; //Something is wrong
	}
	inode->type=FT_DIR;
	//kprintf("Changed type to %d \r\n",inode->type);
	sfs_put_inode(mp,inum,inode);
	//Add . and .. dir in the dir
	//kprintf("opening inode \r\n");
	if(sfs_openinode(mp,fd,inum,inode,O_WRONLY|O_APPEND,06)!=0)
	{
		kfree(inode);
		return -1;
	}
	sfs_dirent dir;
	strncpy(dir.name,".",SFS_NAME_MAX);
	dir.inode=inum;
	sfs_write(fd,(char*)&dir,sizeof(sfs_dirent));
	strncpy(dir.name,"..",SFS_NAME_MAX);
	dir.inode=get_parent_inum(mp,pathname);
	sfs_write(fd,(char*)&dir,sizeof(sfs_dirent));
	sfs_close(fd);
	//sfs_get_inode(mp,inum,inode);
	//kprintf("Looking up again inode num:  %d type: %d ( DIR:%d)\r\n",inum,inode->type,FT_DIR);
	kfree(inode);
	return 0;
}

