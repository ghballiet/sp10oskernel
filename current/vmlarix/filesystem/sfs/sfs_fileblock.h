
/* This file describes the functions for managing the blocks in a 
   file on an SFS filesystem. */

#ifndef SFS_FILEBLOCK_H
#define SFS_FILEBLOCK_H

#include <sys/types.h>
#include <stdint.h>

/* Given the file block number, find the filesystem block. Returns
   the filesystem block number, or 0 on failure. */
int sfs_log2phys(filedesc *f,uint32_t log_block);

/* Set the filesystem block for a specified file block. Returns the
   new block number on success, 0 for invalid block location, -1 if
   filesystem is full, and -2 if the file block is already set to a
   physical disk block.  */
int sfs_set_phys(filedesc *f,uint32_t log_block,uint32_t phys);

/* Delete the physical block for a given file block number. Returns
   0 on success, -2 if the block number is invalid, and -1 if the block
   number is not set.
*/
int sfs_del_phys(filedesc *f,uint32_t log_block);


#endif
