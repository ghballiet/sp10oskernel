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
  kprintf("**** calling ramdisk_attack with %X\n\r",(uint32_t)mem_start);
  /*  for loop checking through minors
  
      TODO: get address from minor #
  */
  ramdisk_minor *current = minors;
  while(current->next != NULL) {
    current = current->next;
    if(current->data == mem_start) {
      kprintf("**** FOUND ONE: %d\n\r",current->num);
      break;
    }
  }
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
}

int ramdisk_write(uint16_t minor,
		   uint32_t block,
		   char *buffer,
		   uint32_t nblocks)
{
}

int ramdisk_num_blk(uint16_t minor)
{
}

int ramdisk_block_size(uint16_t minor)
{
}

/* initialize the datastructures needed to manage ramdisks */
void ramdisk_init(block_device *blk_dev)
{
  kprintf("Init: %s\n\r",blk_dev->name);
  kprintf("RAMdisk driver initialized.\n\r");
}

