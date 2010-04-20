#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
#include <vfs.h>

#include <sys/stat.h>


devdef blk_dev_list[]={
  {RAMDISK_major,NULL},  /* we'll change the pointer when we have one */
  {0,NULL}
};

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

  /* initialize the vfs driver */
  vfs_init();
    
  /* try to attach the ramdisk */
  if(!ramdisk_attach(buffer))
    {
      printf("unable to attach ramdisk\n");
      exit(1);
    }
  
  int minor = 0;

  /* try to mount the filesystem */
  int res;

  // res = vfs_mount_root(RAMDISK_major,minor);
  res = vfs_mount_dev(RAMDISK_major, minor, "/","sfs",0,NULL);

  printf("Result of mount: %d\n",res);

  u_int64_t fpos;
  u_int32_t blknum;

  int fd = vfs_open("/",O_RDONLY,0); 
  sfs_dirent d;
  while(vfs_read(fd,&d,sizeof(sfs_dirent))==sizeof(sfs_dirent))
    printf("%s\n",d.name);
  vfs_close(fd);

}


