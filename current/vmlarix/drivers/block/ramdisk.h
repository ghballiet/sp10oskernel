
#ifndef RAMDISK_H
#define RAMDISK_H

#include <sys/types.h>
#include <blkdev.h>

extern char ramdisk_magic[];

/* initialize the datastructures needed to manage ramdisks */
void ramdisk_init(block_device *blk_dev);

/* Takes a pointer and looks to see if there is a RAMdisk there.
   If yes, then it assigns a minor number and sets up the data
   structurs.  Returns the minor number that was assigned to the
   new RAMdisk, or a negative number on failure. */
int32_t ramdisk_attach(char *mem_start);

/* Destroy the data structures for managing the given RAMdisk,
   unmap it from memory, and mark its physical pages as free. */
void ramdisk_detach(uint16_t minor);


#endif
