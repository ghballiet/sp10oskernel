#include <sys/stddef.h>
#include <misc.h>

size_t strlen(const char *s)
{
  int i=0;
  while((s[i]!=0)&&(i>=0))i++;
  return i;
}

/* char *strdup(const char *s) */
/* { */
/*   int len = strlen(s)+1; */
/*   char *r = (char *)kmalloc(len); */
/*   char *v = r; */
/*   while(*s != 0) */
/*     *(v++) = *(s++); */
/*   *(v++) = *(s++); */
/*   return r; */
/* } */

char *strchr(char *s,int c)
{
  while((*s != 0)&&(*s != c))
    s++;
  if(*s == 0)
    return NULL;
  return s;
}

char *strncpy(char *dest, const char *src, size_t n)
{
  int i=0;
  char *orig = dest;
  while((*src != 0)&&(i<n))
    {
      *(dest++) = *(src++);
      i++;
    }

  *dest = *src;
  return orig;
}

int strcmp(const char *s1, const char *s2)
{
  while((*s1!=0)&&(*s2!=0)&&(*(s1++)== *(s2++)));
  if((*s1 == 0)&&(*s2 == 0))
    return 0;
  if(*s1<*s2)
    return -1;
  return 1;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  int num = n;
  while((--num >= 0)&&(*s1!=0)&&(*s2!=0)&&(*s1 == *s2))
    {
      s1++;
      s2++;
    }
  if(((*s1 == 0)&&(*s2 == 0))||((num<0)&&(*s1 == *s2)))
    return 0;
  if(*s1<*s2)
    return -1;
  return 1;
}


void int_to_hex(int val, char *buf)
{
  int i;
  unsigned tmpval;
  buf[10]=0;
  for(i=0;i<8;i++)
    {
      tmpval = (val>>(4*i)&0xF);
      if(tmpval>9)
	buf[9-i]=(val>>(4*i)&0xF)+'A'-10;
      else
	buf[9-i]=(val>>(4*i)&0xF)+'0';
    }
  buf[0]='0';
  buf[1]='x';
}


void byteswap(void * t)
{
  unsigned *it = (unsigned *)t;
  unsigned a = *it & 0xFF;
  unsigned b = (*it>>8) & 0xFF;
  unsigned c = (*it >> 16 ) & 0xFF;
  unsigned d = (*it >> 24 ) & 0xFF;
  *it = a<<24 | b<<16 | c<<8 | d;
}

