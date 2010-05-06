
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* #include <stdint.h> */

#include <blkdev.h>
#include <ramdisk.h>
#include <sfs_superblock.h>
#include <sfs_inode.h>
#include <sfs_dir.h>
#include <bitmap.h>
#include <dev_majors.h>
#include <dev_list.h>
#include <byteswap.h>
#include <bitmap.h>

devdef blk_dev_list[]={
  {RAMDISK_major,NULL},  /* we'll change the pointer when we have one */
  {0,NULL}
};

#define DEFAULT_BLOCKSIZE 128

void print_superblock(sfs_superblock *super)
{
  printf("Filesystem block size: %d\n",super->block_size);
  printf("Disk blocks per filesystem block: %d\n",super->sectorsperblock);
  printf("Number of filesystem blocks: %d\n",super->num_blocks);
  printf("Starting block of free block bitmap: %d\n",
	 super->free_block_bitmap);
  printf("Size of free block bitmap: %d blocks\n",
	 super->free_block_bitmapblocks);

  printf("Number of inodes: %d\n",super->num_inodes);

  printf("Starting block of free inode bitmap: %d\n",
	 super->free_inode_bitmap);
  printf("Size of free inode bitmap: %d blocks\n",
	 super->free_inode_bitmapblocks);

  printf("Starting block of inode table: %d\n",
	 super->inodes);
  printf("Size of inode table: %d blocks\n",
	 super->num_inode_blocks);

  printf("Root directory starts at block: %d\n",super->rootdir);
  printf("There are %d free blocks\n",super->blocks_free);
  printf("There are %d free inodes.\n",super->inodes_free);
}

typedef struct{
  char name[8];
  u_int32_t startsect;
  u_int32_t numsect;
}partition;

partition default_partition={"",0,0};

#define NUM_PARTITIONS 8


void read_rd(char *filename,char **buffer,int *buflen)
{
  struct stat file_stat;
  int fd;
  if(stat(filename,&file_stat))
    {
      perror("can't stat testdisk");
      exit(1);
    }
  *buflen = file_stat.st_size;

  *buffer = malloc(*buflen);

  if((fd = open(filename,O_RDONLY))<0)
    {
      perror("can't open testdisk");
      exit(1);
    }

  if(read(fd,*buffer,*buflen)!=*buflen)
    {
      perror("can't read testdisk");
      exit(1);
    }
  close(fd);
}

void write_rd(char *filename,char *buffer,int buflen)
{
  int fd;

  if((fd = open(filename,O_WRONLY))<0)
    {
      perror("can't open file");
      exit(1);
    }

  if(write(fd,buffer,buflen)!=buflen)
    {
      perror("can't write file");
      exit(1);
    }
  close(fd);
}

int main(int argc, char **argv)
{
  int i;
  int curblock;

  char *buffer;  /* a buffer to hold the RAMdisk */
  int buflen;    /* size of the buffer */
  char *filename;

  if(argc != 2)
    {
      printf("Usage: %s <filename>\n",argv[0]);
      exit(1);
    }
  
  filename = argv[1];

  /* read in the ramdisk file */
  read_rd(filename,&buffer,&buflen);

  blk_dev_list[0].address = (void*)buffer;

   /* Initialize block device drivers (in this case, just ramdisk driver) */
  block_dev_init(blk_dev_list);

  /* try to attach the ramdisk */
  if(!ramdisk_attach(buffer))
    {
      printf("unable to attach ramdisk\n");
      exit(1);
    }

  int minor = 0;
  int BLOCKSIZE=DEFAULT_BLOCKSIZE;
  int sectors = blk_dev[RAMDISK_major].num_blk(minor);
  int sector_size = blk_dev[RAMDISK_major].blk_size(minor);
  if(BLOCKSIZE<sector_size)
    BLOCKSIZE = sector_size;

  int grouping = BLOCKSIZE/sector_size;/* how many disk blocks make a fsblock*/
  if(grouping==0)
    grouping = 1;
  int blocks = sectors/grouping;/* how many fs blocks */
  

  printf("ramdisk has %d hardware blocks\n",sectors);
  printf("filesystem has %d blocks\n",blocks);

  /* allocate a buffer to hold blocks */
  char *tmp = (char *)malloc(BLOCKSIZE);

  /* start at block 0 and work upwards */
  curblock = 0;

  /* first block of the disk is the stage one boot code */
  for(i=0;i<BLOCKSIZE;i++)
    tmp[i] = 0;
  strcpy(tmp,"Master Boot Record");
  blk_dev[RAMDISK_major].write_fn(minor,curblock,tmp,grouping);

  /* We have to make sure superblock is at a known place */
  curblock+=grouping;

  /* next comes the superblock */
  /* We need to calculate some values:

     1) Number of blocks on the disk determines how many inodes there
        will be.  One inode per data block seems extravagant, but there
	are a lot of very small files in a typical Unix system.
     2) Number of inodes determines how big
        a) the inode table will be.
        b) the free inode bitmap will be.
     3) Number of blocks on the disk determines how big the free block
        bitmap will be.
  */

  sfs_superblock *super = (sfs_superblock*)malloc(sizeof(sfs_superblock));

  super->fsmagic = VMLARIX_SFS_MAGIC;

  strcpy(super->fstypestr,VMLARIX_SFS_TYPESTR);

  super->block_size = BLOCKSIZE;

  super->sectorsperblock = grouping;

  super->num_blocks = blocks;

  super->superblock = curblock;

  super->free_block_bitmap=curblock+grouping;
  super->free_block_bitmapblocks=(blocks/8)/BLOCKSIZE;
  if((super->free_block_bitmapblocks * BLOCKSIZE * 8 < blocks))
    super->free_block_bitmapblocks++;

  /* Rule of thumb: one inode for every 4K of storage. */
  super->num_inodes = (blocks*BLOCKSIZE)/4096; 
  /* but not more than one inode per block */
  if(super->num_inodes > blocks)
    super->num_inodes = blocks;

  super->inodes_free = super->num_inodes - 1;

  /* calculate size and location of free inode bitmap */
  super->free_inode_bitmap=super->free_block_bitmap + super->free_block_bitmapblocks;

  super->free_inode_bitmapblocks=(super->num_inodes/8)/BLOCKSIZE;

  if((super->free_inode_bitmapblocks * BLOCKSIZE * 8 < super->num_inodes))
    super->free_inode_bitmapblocks++;

  /* calculate size and location of inode table */
  int inodesperblock = BLOCKSIZE/sizeof(sfs_inode_t);
  super->num_inode_blocks = super->num_inodes/inodesperblock;
  if(super->num_inode_blocks * inodesperblock < super->num_inodes)
    super->num_inode_blocks++;
  super->inodes = super->free_inode_bitmap+
    (grouping*super->free_inode_bitmapblocks);

  super->rootdir = super->inodes+super->num_inode_blocks*grouping;

  int totalblocks = 3 + 
    super->free_block_bitmapblocks+
    super->free_inode_bitmapblocks+
    super->num_inode_blocks;

  super->blocks_free = super->num_blocks - totalblocks - 1;

  /* swap bytes if we are on big-endian machine */
  byteswap32(&(super->fsmagic)); 
  byteswap32(&(super->block_size));
  byteswap32(&(super->sectorsperblock));
  byteswap32(&(super->superblock));
  byteswap32(&(super->num_blocks));
  byteswap32(&(super->free_block_bitmap));
  byteswap32(&(super->free_block_bitmapblocks));
  byteswap32(&(super->blocks_free));
  byteswap32(&(super->num_inodes));
  byteswap32(&(super->free_inode_bitmap));
  byteswap32(&(super->free_inode_bitmapblocks));
  byteswap32(&(super->inodes_free));


  byteswap32(&(super->num_inode_blocks));
  byteswap32(&(super->inodes));
  byteswap32(&(super->rootdir));
  byteswap32(&(super->open_count));

  /* write the superblock */
  char *tmp2 = (char *)super;
  for(i=0;i<sizeof(sfs_superblock);i++)
    tmp[i] = tmp2[i];
  while(i<BLOCKSIZE)
    tmp[i++]=0;


  blk_dev[RAMDISK_major].write_fn(minor,curblock,tmp,grouping);
  curblock+=grouping;

  /* swap bytes back if we are on big-endian machine */
  byteswap32(&(super->fsmagic)); 
  byteswap32(&(super->block_size));
  byteswap32(&(super->sectorsperblock));
  byteswap32(&(super->superblock));
  byteswap32(&(super->num_blocks));
  byteswap32(&(super->free_block_bitmap));
  byteswap32(&(super->free_block_bitmapblocks));
  byteswap32(&(super->blocks_free));

  byteswap32(&(super->num_inodes));
  byteswap32(&(super->free_inode_bitmap));
  byteswap32(&(super->free_inode_bitmapblocks));
  byteswap32(&(super->inodes_free));


  byteswap32(&(super->num_inode_blocks));
  byteswap32(&(super->inodes));
  byteswap32(&(super->rootdir));
  byteswap32(&(super->open_count));

  /* print superblock info */
  print_superblock(super);


  /* create the free blocks bitmap. every bit is zero except the
     blocks we used above, and the block for the root directory */

  int cb,mb;
  mb = 0;
  for(cb=0;cb<super->free_block_bitmapblocks;cb++)
    {
      for(i=0;i<BLOCKSIZE;i++)
	tmp[i] = 0;

      i = 0;
      while((mb<totalblocks)&&(i<(BLOCKSIZE*8)))
	{
	  set_bit((bitmap_t*)tmp,i);
	  i++;
	  mb++;
	}
      blk_dev[RAMDISK_major].write_fn(minor,
				      super->free_block_bitmap+cb,
				      tmp,
				      grouping);

    }

  /* create the free inodes bitmap. we will use inode 0 for the root dir */
  curblock = super->free_inode_bitmap;

  for(i=0;i<BLOCKSIZE;i++)
    tmp[i] = 0;              /* zero means free.  one means used */

  set_bit((bitmap_t*)tmp,0);  /* inode zero will be used for the root fs */

  /* write the free inodes bitmap */
  blk_dev[RAMDISK_major].write_fn(minor,curblock,tmp,grouping);
  curblock+=grouping;
  clear_bit((bitmap_t*)tmp,0);

  for(i=1;i<super->free_inode_bitmapblocks;i++)
    {
      blk_dev[RAMDISK_major].write_fn(minor,curblock,tmp,grouping);
      curblock+=grouping;
    }

  /* create the root inode */
  curblock = super->inodes;
  sfs_inode_t *ri = (sfs_inode_t *)tmp;
  ri->owner = 0;
  ri->group = 0;
  ri->ctime = 1;
  ri->mtime = 2;
  ri->atime = 3;
/*   ri->mode = 0x1FF; */
  ri->type = FT_DIR;
  ri->refcount = 1;
  ri->size = 2 * sizeof(sfs_dirent);
/*   ri->dev_major = RAMDISK_MAJOR; */
/*   ri->dev_minor = minor; */
  ri->direct[0] = super->rootdir;
  for(i=1;i<NUM_DIRECT;i++)
    ri->direct[i] = 0;
  ri->indirect = 0;
  ri->dindirect = 0;

  /* swap bytes back if we are on big-endian machine */
  byteswap32(&(ri->owner));
  byteswap32(&(ri->group));
  byteswap32(&(ri->ctime));
  byteswap32(&(ri->mtime));
  byteswap32(&(ri->atime));
  byteswap16(&(ri->perm));
  byteswap64(&(ri->size));
  for(i=0;i<NUM_DIRECT;i++)
    byteswap32(&(ri->direct[i]));
  byteswap32(&(ri->indirect));
  byteswap32(&(ri->dindirect));
  byteswap32(&(ri->tindirect));


  /* write the root inode */
  blk_dev[RAMDISK_major].write_fn(minor,curblock,tmp,grouping);
  curblock+=grouping;

  /* init remaining inodes */
  for(i=0;i<BLOCKSIZE;i++)
    tmp[i] = 0;

  for(i=1;i<super->num_inode_blocks;i++)
    {
      blk_dev[RAMDISK_major].write_fn(minor,curblock,tmp,grouping);
      curblock+=grouping;
    }

  /* write the root directory block */
  curblock = super->rootdir;
  sfs_dirent *d = (sfs_dirent*)tmp;
  strcpy(d->name,".");
  d->inode = 0;
  d++;
  strcpy(d->name,"..");
  d->inode = 0;

  blk_dev[RAMDISK_major].write_fn(minor,curblock,tmp,grouping);
  curblock+=grouping;

  /* save disk image back to disk */
  write_rd(filename,buffer,buflen);

  return 0;
}


  /* mount the root filesystem from the ramdisk */
  //  mount_root(RAMDISK_MAJOR,0);

  /* next comes the partition table */
  /* no partition tables on ramdisks */
  /*   partition* parts = (partition*)buffer; */

  /*   strcpy(parts[0].name,"root"); */
  /*   parts[0].startsect = sect+1; */
  /*   parts[0].numsect = blk_dev[RAMDISK_MAJOR].num_sect(minor); */

  /*   for(i=1;i<8;i++) */
  /*     parts[i]=default_partition; */

  /*   blk_dev[RAMDISK_MAJOR].write_fn(minor,sect,buffer,1); */

