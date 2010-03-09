#ifndef BLKDEV_H
#define BLKDEV_H

#ifdef NEED_STDINT
#include <stdint.h>
#endif
#include <sys/types.h>

#include <dev_list.h>
#include <stdarg.h>

/* this is the maximum for the major number of a block device.   */
/* Block device drivers can specify their own max minor numbers. */
#define BLK_DEV_MAX 256

/* define a general structure for block devices */

typedef struct {
  int registered;
  char *name;
  int32_t (*read_fn)(uint16_t,uint32_t,char *,uint32_t);
  int32_t (*write_fn)(uint16_t,uint32_t,char *,uint32_t);
  int32_t (*num_blk)(uint16_t);
  int32_t (*blk_size)(uint16_t);
  int32_t (*ioctl)(uint16_t,va_list);
}block_device;

/* define some error codes */
#define BLK_MAJINVAL  -1  /* The device major number is too big */
#define BLK_NOTREG    -2  /* The device driver is not registered */
#define BLK_NODEVICE  -3  /* The minor device does not exist */
#define BLK_BLKTOOBIG -4  /* The requested block is too big */

/* define an array of block devices */
extern block_device blk_dev[BLK_DEV_MAX];

/* this routine calls the init method for all of the block device drivers */
uint32_t block_dev_init(devdef *blk_dev_list);

int32_t block_write(uint16_t major,uint16_t minor,uint32_t block,
		    char *buff,uint32_t nblocks);
int32_t block_read(uint16_t major,uint16_t minor,uint32_t block,
		   char *buff,uint32_t nblocks);
int32_t block_ioctl(uint16_t major,uint16_t minor,...);
int32_t num_blk(uint16_t major,uint16_t minor);
int32_t blk_size(uint16_t major,uint16_t minor);

#endif
