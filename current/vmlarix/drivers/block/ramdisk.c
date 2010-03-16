#include <stddef.h> 
#include <blkdev.h> 
#include <ramdisk.h> 
#include <stdint.h>
#include <byteswap.h>

#ifndef _KERNEL_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define kmalloc malloc
#define kprintf printf
#else
#include <misc.h>
#endif

#ifdef PTR64
typedef uint64_t ptrint_t;
#else
typedef uint32_t ptrint_t;
#endif

char ramdisk_magic[]="VMLARIX RAMDISK";

typedef struct rm{
  int num;             /* minor number for this RAMdisk */
  char *data;          /* start of data for this RAMdisk */
  uint32_t blocksize;  /* size of a block, in bytes */
  uint32_t length;     /* number of blocks */
  uint32_t bitshift;   /* number of bits to shift for indexing */
  struct rm *next;     /* pointer to next minor number */
}ramdisk_minor;

ramdisk_minor *minors;

/* Takes a pointer and looks to see if there is a RAMdisk there.
   If so, return the size of the RAMdisk, in bytes. Returns 0 on
   failure.
*/
int32_t ramdisk_attach(char *mem_start)
{
  kprintf("Calling ramdisk_attach with %X\n\r",(uint32_t)mem_start);
  if(minors == NULL) {
    kprintf("Attaching FIRST ramdisk\n\r");
    
  } else {
    kprintf("Ramdisk already set up, looping...\n\r");
  }
  /*  for loop checking through minors  */
}

void ramdisk_detach(uint16_t minor)
{
    kprintf("**** calling ramdisk_detach with %X\n\r",(uint32_t)minor);
  /*  for loop checking through minors
  
      TODO: get address from minor #
  */
}

int32_t ramdisk_read(uint16_t minor,
		     uint32_t block,
		     char *buffer,
		     uint32_t nblocks)
{
  kprintf("ramdisk_read **** \n\r");
}

int ramdisk_write(uint16_t minor,
		   uint32_t block,
		   char *buffer,
		   uint32_t nblocks)
{
}

int ramdisk_num_blk(uint16_t minor)
{
  kprintf("ramdisk_num_blk called with minor=%X\n\r",minor);
}

int ramdisk_block_size(uint16_t minor)
{
  // powers of 2
  kprintf("ramdisk_block_size called with minor=%X\n\r",minor);
}

/* initialize the datastructures needed to manage ramdisks */
void ramdisk_init(block_device *blk_dev)
{
  // calculate amt of shift required in each block
  // store in bitshift
  // to get blocksize
  // take block #, shift by bitshift

  // populating blk_dev device
  // set registered, name and function pointers
  blk_dev->read_fn = (int32_t (*)(uint16_t,uint32_t,char *,uint32_t))&ramdisk_read;
  blk_dev->write_fn = (int (*)(uint16_t,uint32_t,char *,uint32_t))&ramdisk_write;
  blk_dev->num_blk = (int (*)(uint16_t))&ramdisk_num_blk;
  blk_dev->blk_size = (int (*)(uint16_t))&ramdisk_block_size;
  kprintf("RAMdisk driver initialized.\n\r");
}

