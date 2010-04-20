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
//========o========o========o========o========o========o========o========o
/* Declaring Function that will check the range for USER, GROUP and OTHER */
int check_range(int value);
void show_rwx_format(mode_t mode);
void show_permissions(int value);
//========o========o========o========o========o========o========o========o
int sfs_fchmod(filedesc *f, mode_t mode)
{	
	kprintf("========o========o========o========o========o========o\n\r");
	
	static uint16_t temp_inode_perm;
	static uint32_t temp_fd_mode;
	
	int user_owner = 0;
	int group = 0;
	int other = 0;
   
   mount_point *mp;	//declares a pointer to a mounting point
   uint32_t inum;    /* inode number */
	sfs_inode_t *inode;	//declare pointer to inode - /* inode in memory */
	
   sfs_fd_private *sfs_fd;	//declare pointer to private part of a FD (File Descriptor)
   sfs_fd = (sfs_fd_private *)(f->fs_private); //Get the private part and casting it into the right type, from [void] to [sfs_fd_private]
   sfs_fd_private *fp = f->fs_private;
   //>> sfs_fd_private *fp = f->fs_private;
   
   mp = f->mp;
   inum = sfs_fd->inum;   
   inode = sfs_fd->inode;	// setting the inode pointer to point into the right address
   
   //kprintf("INUM = %u\n\r", inum);
   //kprintf("INUM 2 = %u\n\r", fp->inum);
   //kprintf("INODE = %u\n\r", inode);
   //kprintf("INODE 2= %u\n\r", fp->inode);
   

	//sfs_put_inode(f->mp,fp->inum,fp->inode);
	
	user_owner = mode/100;
	group = (mode - user_owner*100)/10;
	other = mode-(user_owner*100 + group*10);

	if(check_range(user_owner)&&check_range(group)&&check_range(other))
   {
   	//If I am here is because ALL of the new attributes are VALID, so I CAN ACCEPT THEM
   	//kprintf("previous permissions were: %d\n\r", f->mode);
   	kprintf("previous permissions were: %d\n\r", inode->perm);
   	
   	/*The following two lines are required to keep synchrinization of our info*/
		f->mode = mode;	//For File Descriptor structure
		
		inode->perm = (uint16_t)mode;	//For Inode 
		
		//kprintf("NEW permissions are: %d\n\r", f->mode);
		kprintf("NEW permissions are: %d\n\r", inode->perm);
		show_rwx_format(f->mode);
      
      /*writing this change back to the ramdisk in order to make it PERMANENT*/
      //>> sfs_put_inode(f->mp,fp->inum,fp->inode);
      sfs_put_inode(mp, inum, inode);
      return f->mode;
   }
   else
   {
   	kprintf("If I am here is because at least ONE of the new attributes is INVALID\n\r");
   	//If I am here is because at least ONE of the new attributes is INVALID
      return -1;
   }
}
//========o========o========o========o========o========o========o========o
/* Defining Function that will check the range for USER, GROUP and OTHER */
int check_range(int value)
{
	if((value ==0)||(value == 7)||(value == 4)||(value == 2)||(value == 1))
	{
		//printf("PASS");
		return 1; //means new values for persmissions are correct
	}
	else
	{
		//printf("DID NOT PASS");
		return 0; //does not pass
	}
}
//========o========o========o========o========o========o========o========o
void show_rwx_format(mode_t mode)
{
	int user_owner = 0;
	int group = 0;
	int other = 0;
	
	user_owner = mode/100;
	group = (mode - user_owner*100)/10;
	other = mode-(user_owner*100 + group*10);
	
	kprintf("-----------------------\n\r"); 
	kprintf("FOR USER_OWNER:\n\r");
	show_permissions(user_owner);
	kprintf("-----------------------\n\r");
	kprintf("FOR GROUP:\n\r");
	show_permissions(group);
	kprintf("-----------------------\n\r");
	kprintf("FOR OTHER:\n\r");
	show_permissions(other);
}

//==========
//========o========o========o========o========o========o========o========o
void show_permissions(int value)
{
	char *permissions;
	
	switch(value)
	{
		case 0:
			permissions = "---";
			kprintf("Permissions: %s\n\r", permissions); 
			break;
		case 7:
			permissions = "rwx";
			kprintf("Permissions: %s\n\r", permissions);
			break;
		case 4:
			permissions = "r--";
			kprintf("Permissions: %s\n\r", permissions);
			break;
		case 2:
			permissions = "-w-";
			kprintf("UPermissions: %s\n\r", permissions);
			break;
		case 1:
			permissions = "--x";
			kprintf("Permissions: %s\n\r", permissions);
			break;
		default:
			break;
	}
}

