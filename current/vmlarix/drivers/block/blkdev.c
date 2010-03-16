#include <blkdev.h>
#include <dev_majors.h>
#include <stdarg.h>

#ifndef _KERNEL_
#include <stdlib.h>
#include <stdio.h>
#define kmalloc malloc
#define kfree free
#define kprintf printf
#define panic printf
#endif

/* define an array of block devices */

block_device blk_dev[BLK_DEV_MAX];

/* this routine calls the init method for all of the block device drivers,
   and probes for each device in block_dev_list. */
uint32_t block_dev_init(devdef *block_dev_list)
{
  int i;
  for(i=0;i<BLK_DEV_MAX;i++)
    blk_dev[i].registered = 0;

  ramdisk_init(&(blk_dev[RAMDISK_major]));

  while(block_dev_list->major != 0)
    {
      switch(block_dev_list->major)
	{
	case RAMDISK_major:
	  kprintf("Attaching RAMdisk\n\r"); 
	  if(ramdisk_attach(block_dev_list->address))
	    kprintf("RAMdisk attached\n\r"); 
	  else
	    kprintf("RAMdisk not found\n\r"); 
 	  break;
	default:
	  panic("Attempted to attach nonexistent device");
	  break;
	}
      block_dev_list++;
    }
}

/* examine blk_dev array and return appropriate error code */
int check_major(uint16_t major) {
  if(major>=BLK_DEV_MAX || major==0)
    return BLK_MAJINVAL;
  if(blk_dev[major].registered==0)
    return BLK_NOTREG;
  return 1;
}

/* return the number of bytes successfully written */
int32_t block_write(uint16_t major,uint16_t minor,uint32_t block,
		    char *buff,uint32_t nblocks)
{
  /* examine the blk_dev array and find out if the major device exists */  
  /* if so, call the driver's write method with the minor number and
     all other parameters. */
  /* if not,  return an error code */
  int e = check_major(major);
  if(e!=1) return e;
  return blk_dev[major].write_fn(minor,block,buff,nblocks);
}

/* return the number of bytes successfully read */
int32_t block_read(uint16_t major,uint16_t minor,uint32_t block,
		   char *buff,uint32_t nblocks)
{
  /* examine the blk_dev array and find out if the major device exists */  
  /* if so, call the driver's read method with the minor number and
     all other parameters. */
  /* if not,  return an error code */
  int e = check_major(major);
  if(e!=1) return e;
  return blk_dev[major].read_fn(minor,block,buff,nblocks);
}

int32_t block_ioctl(uint16_t major,uint16_t minor,...)
{
  /* examine the blk_dev array and find out if the major device exists */  
  /* if so, call the driver's ioctl method with the minor number and
     all other parameters. */
  /* if not,  return an error code */
}

int32_t num_blk(uint16_t major,uint16_t minor)
{
  /* examine the blk_dev array and find out if the major device exists */  
  /* if so, call the driver's num_blk method with the minor number and
     all other parameters. */
  /* if not,  return an error code */
  int e = check_major(major);
  if(e!=1) return e;
  return blk_dev[major].num_blk(minor);
}

int32_t blk_size(uint16_t major,uint16_t minor)
{
  /* examine the blk_dev array and find out if the major device exists */  
  /* if so, call the driver's blk_size method with the minor number and
     all other parameters. */
  /* if not,  return an error code */
  int e = check_major(major);
  if(e!=1) return e;
  return blk_dev[major].blk_size(minor);
}




