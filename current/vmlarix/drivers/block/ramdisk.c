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

int ramdisk_exists(char *mem_start)
{
  int i;
  /* look for a RAMdisk "magic number" */
  for(i = 0; i<strlen(ramdisk_magic); i++)
    if(mem_start[i]!=ramdisk_magic[i])
      return 0;
  return 1;
}

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
  static int next_minor = 0;
  static int firstcall = 1;
  ramdisk_minor *minor;

  kprintf("attaching ramdisk at %X\n\r",mem_start);
  kprintf("%s\n\r",ramdisk_magic);

  if(!ramdisk_exists(mem_start))
    return 0;

  /* Create a minor structure to hold info about this RAMdisk.  For a
     hardware disk, we would read the partition table and create
     possibly a lot of minors. */
  minor = (ramdisk_minor *)kmalloc(sizeof(ramdisk_minor));
  minor->num = next_minor++;

  char *ptr = mem_start+strlen(ramdisk_magic)+1;
  while(((ptrint_t)ptr) & 0x3)
    ptr++;
  minor->length = *((uint32_t*)ptr);
  byteswap32(&(minor->length));
  ptr += sizeof(int);
  minor->blocksize = *((uint32_t*)ptr);
  byteswap32(&(minor->blocksize));
  ptr += sizeof(int);
  minor->bitshift = *((uint32_t*)ptr);
  byteswap32(&(minor->bitshift));

  minor->data = mem_start+minor->blocksize; /* skip first sector */
  minor->next =  minors;
  minors = minor;

  return minor->length;
}

void ramdisk_detach(uint16_t minor)
{
}

int32_t ramdisk_read(uint16_t minor,
		     uint32_t block,
		     char *buffer,
		     uint32_t nblocks)
{
  char *dataptr;
  int i;
  ramdisk_minor *m_rec = minors;
  while((m_rec != NULL)&&(m_rec->num!=minor))
    m_rec = m_rec->next;
  if(m_rec == NULL)
    return BLK_NODEVICE;
  if(block+nblocks > m_rec->length)
    return BLK_BLKTOOBIG;
  dataptr = m_rec->data + (block<<m_rec->bitshift);
  for(i=0;i<(nblocks<<m_rec->bitshift);i++)
    {
      /* very inefficient to move one byte at a time */
      *(buffer++) = *(dataptr++);
    }
  return i;
}


int ramdisk_write(uint16_t minor,
		   uint32_t block,
		   char *buffer,
		   uint32_t nblocks)
{
  char *dataptr;
  int i;

  ramdisk_minor *m_rec = minors;
  while((m_rec != NULL)&&(m_rec->num!=minor))
    m_rec = m_rec->next;
  if(m_rec == NULL)
    return BLK_NODEVICE;
  if(block+nblocks > m_rec->length)
    return BLK_BLKTOOBIG;
  dataptr = m_rec->data + (block<<m_rec->bitshift);
  for(i=0;i<(nblocks<<m_rec->bitshift);i++)
    {
      /* very inefficient to move one byte at a time */
      *(dataptr++) = *(buffer++);
    }
  return i;
}

int ramdisk_num_blk(uint16_t minor)
{
  char *dataptr;
  int i;

  ramdisk_minor *m_rec = minors;
  while((m_rec != NULL)&&(m_rec->num!=minor))
    m_rec = m_rec->next;
  if(m_rec == NULL)
    return BLK_NODEVICE;
  return m_rec->length;
}

int ramdisk_block_size(uint16_t minor)
{
  ramdisk_minor *m_rec = minors;
  while((m_rec != NULL)&&(m_rec->num!=minor))
    m_rec = m_rec->next;
  if(m_rec == NULL)
    return BLK_NODEVICE;
  return m_rec->blocksize;
}

/* initialize the datastructures needed to manage ramdisks */
void ramdisk_init(block_device *blk_dev)
{
  /* First, we need to register the RAMdisk driver, so that the
     block device table has a pointer to its operations */
  blk_dev->registered = 1;
  blk_dev->name = "ramdisk";
  blk_dev->read_fn = &ramdisk_read;
  blk_dev->write_fn = &ramdisk_write;
  blk_dev->num_blk = &ramdisk_num_blk;
  blk_dev->blk_size = &ramdisk_block_size;
  blk_dev->ioctl = NULL;
  //blk_dev[RAMDISK_major].attach_fn = &ramdisk_attach;
  minors = NULL;
  kprintf("RAMdisk driver initialized.\n\r");
}

