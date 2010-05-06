/* This is a small program to create a ramdisk image that can be loaded into
   simics.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifdef HOST_IS_BIG_ENDIAN
void byteswap64(void *t)
{
  unsigned char *a = (unsigned char *)t;
  unsigned char *b = (unsigned char *)t+1;
  unsigned char *c = (unsigned char *)t+2;
  unsigned char *d = (unsigned char *)t+3;
  unsigned char *e = (unsigned char *)t+4;
  unsigned char *f = (unsigned char *)t+5;
  unsigned char *g = (unsigned char *)t+6;
  unsigned char *h = (unsigned char *)t+7;
  unsigned tmp;
  tmp = *a;
  *a = *h;
  *h = tmp;
  tmp = *b;
  *b = *g;
  *g = tmp;
  tmp = *c;
  *c = *f;
  *f = tmp;
  tmp = *d;
  *d = *e;
  *e = tmp;
}

void byteswap32(void *t)
{
  unsigned *it = (unsigned *)t;
  unsigned a = *it & 0xFF;
  unsigned b = (*it>>8) & 0xFF;
  unsigned c = (*it >> 16 ) & 0xFF;
  unsigned d = (*it >> 24 ) & 0xFF;
  *it = a<<24 | b<<16 | c<<8 | d;
}

void byteswap16(void *t)
{
  unsigned *it = (unsigned *)t;
  unsigned a = *it & 0xFF;
  unsigned b = (*it>>8) & 0xFF;
  *it = a<<8 | b;
}

#else
#define byteswap64(a)
#define byteswap32(a)
#define byteswap16(a)
#endif


char ramdisk_magic[]="VMLARIX RAMDISK";

int log_base2(int num)
{
  int i=0;
  int tmp = num;
  while(tmp = (tmp>>1))
    i++;

  /*  printf("%h %h %h\n",i,i<<tm,num); */

  if(((1<<tmp)-1) & num)
    {
      printf("blocksize must be a power of 2\n");
      exit(1);
    }

  return i;
}

char *buffer;
int buflen;

void make_rd(char *filename,int blocksize,int numblocks)
{
  int i,j,ns,*tmp;
  struct stat file_stat;
  int fd;

  int ramdiskbitshift = log_base2(blocksize);
  
  if(!stat(filename,&file_stat)) 
    {
      printf("File already exists!\n");
      exit(1);
    }

  buflen = blocksize;
  buffer = malloc(buflen);
  
  for(i=0;i<strlen(ramdisk_magic)+1;i++)
    {
      buffer[i] = ramdisk_magic[i];
    }

  while(i&3)
    i++;

  int *bptr = (int *)(buffer+i);
  *bptr  = ns = numblocks-1;
  byteswap32(bptr); /* in case we are big-endian */
  i += sizeof(int);
  bptr++;

  *bptr  = blocksize;
  byteswap32(bptr); /* in case we are big-endian */
  i += sizeof(int);
  bptr++;

  *bptr  = ramdiskbitshift;
  byteswap32(bptr); /* in case we are big-endian */
  i += sizeof(int);
  bptr++;

  while(i<buflen)
    buffer[i++] = 0;
  
  if((fd = open(filename,O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO))<0)
    {
      perror("Unable to open file");
      exit(1);
    }
  if(write(fd,buffer,buflen)!=buflen)
    {
      perror("Unable to write file");
      exit(1);
    }

  tmp = (int *)buffer;
  for(i=0;(i<ns); i++)
    {
      for(j=0;j<buflen>>2;j++)
	{
	  tmp[j] = i;
	  byteswap32(&(tmp[j]));
	}
        
      if(write(fd,buffer,buflen)!=buflen)
	{
	  perror("Unable to write file");
	  exit(1);
	}
    }

  close(fd);
}


int main(int argc, char **argv)
{
  if(argc != 4)
    {
      printf("Usage: %s <filename> <blocksize> <numblocks>\n",argv[0]);
      exit(1);
    }
  
  int blocksize = atoi(argv[2]);
  int numblocks = atoi(argv[3]);

  make_rd(argv[1],blocksize,numblocks);
  return 0;
}
