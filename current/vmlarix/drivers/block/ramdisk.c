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


ramdisk_minor *get_rd_record(int minor) {
  ramdisk_minor *current = minors;
  while(current != NULL && current->num != minor)
    current = current->next;
  return current;
}


/* Takes a pointer and looks to see if there is a RAMdisk there.
   If so, return the size of the RAMdisk, in bytes. Returns 0 on
   failure.
*/
int32_t ramdisk_attach(char *mem_start)
{
  kprintf("Calling ramdisk_attach with %X\n\r",(uint32_t)mem_start);

  ramdisk_minor *new_rd = malloc(sizeof(ramdisk_minor));
  new_rd->next = NULL;

  if(minors == NULL) {
    kprintf("Attaching FIRST ramdisk\n\r");
    minors = new_rd;
    new_rd->num = 0;
  } else {
    kprintf("Ramdisk already set up, looping...\n\r");
    ramdisk_minor *current = minors;
    while(current->next != NULL)
      current = current->next;
    current->next = new_rd;
    new_rd->num = current->num + 1;
  }

  int i;
  for(i=0; i<strlen(ramdisk_magic)+1; i++) {
    if(mem_start[i] != ramdisk_magic[i])
      return 0;
  }

  while(i&3)
    i++;

  int *bptr = (int *)(mem_start+i);
  new_rd->length = *bptr;
  bptr++;
  new_rd->blocksize = *bptr;
  bptr++;
  new_rd->bitshift = *bptr;

  return (new_rd->length * new_rd->blocksize);
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
  
  ramdisk_minor *rd = get_rd_record(minor);
  if(rd) {
    char *source = rd->data + (block << rd->bitshift);
    int size = nblocks * rd->blocksize;
    memcpy(buffer, source, size);
    return size;
  }
  else
    return 0;
}

int ramdisk_write(uint16_t minor,
		   uint32_t block,
		   char *buffer,
		   uint32_t nblocks)
{  
  ramdisk_minor *rd = get_rd_record(minor);
  if(rd) {
    char *dest = rd->data + (block << rd->bitshift);
    int size = nblocks * rd->blocksize;
    memcpy(dest, buffer, size);
    return size;
  }
  else
    return 0;
}

int ramdisk_num_blk(uint16_t minor)
{
  kprintf("ramdisk_num_blk called with minor=%X\n\r",minor);
  ramdisk_minor *rd = get_rd_record(minor);
  return rd->length;
}

int ramdisk_block_size(uint16_t minor)
{
  kprintf("ramdisk_block_size called with minor=%X\n\r",minor);
  ramdisk_minor *rd = get_rd_record(minor);
  return rd->blocksize;
}

/* initialize the datastructures needed to manage ramdisks */
void ramdisk_init(block_device *blk_dev)
{
  // populating blk_dev device
  // set registered, name and function pointers
  blk_dev->read_fn = (int32_t (*)(uint16_t,uint32_t,char *,uint32_t))&ramdisk_read;
  blk_dev->write_fn = (int (*)(uint16_t,uint32_t,char *,uint32_t))&ramdisk_write;
  blk_dev->num_blk = (int (*)(uint16_t))&ramdisk_num_blk;
  blk_dev->blk_size = (int (*)(uint16_t))&ramdisk_block_size;
  blk_dev->registered = 1;
  kprintf("RAMdisk driver initialized.\n\r");
}

