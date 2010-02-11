
/*
  call-seq:
     snprintf(buffer, bufsize, format_string [, arguments...] )  
  returns:  
     snprintf only prints up to n characters to the buffer.  If the
     output was truncated due to this limit then the return value is
     the number of characters (not including the trailing ’¡Ç\0’¡Ç)
     which would have been written to the final string if enough space
     had been available.  Thus, a return value of size or more means
     that the output was truncated.  If there is an error during the
     conversion, snprintf returns -1;

  Creates a string resulting from applying format_string to any
  additional arguments. The length of the string is limited to
  bufsize.  Within the format string, any characters other than format
  sequences are copied to the result. A format sequence consists of a
  percent sign, followed by optional flags, followed by a field type
  character. The field type controls how the corresponding argument is
  to be interpreted. The field type characters are listed in the table
  at the end of this section.

  The field types are:

      Field |  Conversion
      ------+--------------------------------------------------------------
        b   | Convert argument as a binary number.
        c   | Argument is the numeric code for a single character.
        d   | Convert argument as a decimal number.
	i   | Identical to `d'.
        o   | Convert argument as an octal number.
        s   | Argument is a string to be substituted. 
        u   | Treat argument as an unsigned decimal number.
        X   | Convert argument as a hexadecimal number using uppercase
            | letters. Negative numbers will be displayed with two
            | leading periods (representing an infinite string of
            | leading 'FF's.
        x   | Convert argument as a hexadecimal number.
            | Negative numbers will be displayed with two
	    | leading periods (representing an infinite string of
            | leading 'ff's.

   The flags are:
      Flag  | Applies to | Description
      ------+--------------------------------------------------------------
        l   | bdiouXx    | number is to be treated as a "long" int

 */

#include <misc.h>
#include <stdarg.h>
#include <dev_list.h>

void convert_dec(char **buf,int *remain,long long param,int long_flag)
{
  long long tmp;
  int digcount;
  char *bptr;
  int negative;
  long long paramabs;
  int i;
  char tmpstr[32];  /* a buffer for the result */
  char *ptr = *buf;

/*   printf("Buffer is %X\n",ptr);   */

  if(param<0)
    {
      negative = 1;
      paramabs = -param;
    }
  else
    {
      negative = 0;
      paramabs = param;
    }


  /* find out how many digits it will take */
  if(paramabs == 0)
    digcount = 1;
  else
    {
      digcount = 0;
      tmp = paramabs;
      while(tmp>0)
	{
	  tmp/=10;
	  digcount++;
	}
    }

/*   printf("negative:%d paramabs:%d digcount:%d\n",negative,paramabs,digcount); */

  /* if there is no room at all in the buffer, just count these bytes
     and return */
  if(*remain<0)
    {
      *remain -= digcount;
      return;
    }

  /* make string in reverse order */
  tmpstr[digcount+negative] = 0;
  bptr = tmpstr+digcount+negative;
  tmp = paramabs;
  for(i=0;i<digcount;i++)
    {
      *(--bptr) = (tmp%10) + '0';
      tmp /= 10;
    }
  if(negative)
    *(--bptr) = '-';

  /* copy string to output buffer */
  /* and adjust buffer and remain */
  for(i=0;i<digcount+negative;i++)
    {
      if(*remain > 0)
	*(ptr++) = tmpstr[i];
      (*remain)--;
    }
  *buf = ptr;
}

convert_bin(char **buf,int *remain,unsigned long long param,int long_flag)
{
 char *cur;
  int numbits;
  int i;
  int val;
  char *ptr = *buf;

  if(long_flag)
    numbits = 63;
  else
    numbits = 31;

  for(i=numbits;i>=0;i--)
    {
      if(*remain>0)
	{
	  val = (param & ((unsigned long long)1<<i))>>i;
	  *(ptr++) = val+'0';
	}
      (*remain)--;
    }
  *buf = ptr; 
}

convert_oct(char **buf,int *remain,unsigned long long param,int long_flag)
{
  char *cur;
  int numbits;
  int i;
  int val;
  char *ptr = *buf;

  if(long_flag)
    numbits = 63;
  else
    numbits = 30;

  for(i=numbits;i>=0;i-=3)
    {
      if(*remain>0)
	{
	  val = (param & ((unsigned long long)7<<i))>>i;
	  *(ptr++) = val+'0';
	}
      (*remain)--;
    }
  *buf = ptr; 
}

convert_hex(char **buf,int *remain,long long param,int long_flag,int cbase)
{
  char *cur;
  int numbits;
  int i;
  int val;
  char *ptr = *buf;

  if(long_flag)
    numbits = 60;
  else
    numbits = 28;

  for(i=numbits;i>=0;i-=4)
    {
      if(*remain>0)
	{
	  val = (param & ((unsigned long long)15<<i))>>i;
	  if(val<10)
	    *(ptr++) = val + '0';
	  else
	    *(ptr++) = val - 10 + cbase;
	}
      (*remain)--;
    }
  *buf = ptr; 
}

convert_udec(char **buf,int *remain,unsigned long long param,int long_flag)
{
  unsigned long long tmp;
  int digcount;
  char *bptr;
  int i;
  char tmpstr[32];  /* a buffer for the result */
  char *ptr = *buf;

/*   printf("Buffer is %X\n",ptr);   */

  /* find out how many digits it will take */
  if(param == 0)
    digcount = 1;
  else
    {
      digcount = 0;
      tmp = param;
      while(tmp>0)
	{
	  tmp/=10;
	  digcount++;
	}
    }

/*   printf("negative:%d param:%d digcount:%d\n",negative,param,digcount); */

  /* if there is no room at all in the buffer, just count these bytes
     and return */
  if(*remain<0)
    {
      *remain -= digcount;
      return;
    }

  /* make string in reverse order */
  tmpstr[digcount] = 0;
  bptr = tmpstr+digcount;
  tmp = param;
  for(i=0;i<digcount;i++)
    {
      *(--bptr) = (tmp%10) + '0';
      tmp /= 10;
    }

  /* copy string to output buffer */
  /* and adjust buffer and remain */
  for(i=0;i<digcount;i++)
    {
      if(*remain > 0)
	*(ptr++) = tmpstr[i];
      (*remain)--;
    }
  *buf = ptr;
}




int ksnprintf(char *buf,int bufsize,char *fmt,...)
{

  int fd = 0;

  int long_flag;
  int remain = bufsize;

  unsigned long long int argui;
  long long int argsi;
  char argc;
  char *argstr;

  char *bufptr = buf;

  va_list args;
  va_start(args,fmt);


  while((*fmt != 0)&&(remain>0))
    {
      if(*fmt != '%')
	{ 
	  *(bufptr++) = *(fmt++);
	  remain--; 
/* 	  printf("char: %c bufptr: %X remain:%d\n",*(bufptr-1),bufptr,remain); */
	}
      else
	{ /* begin format sequence */
	  long_flag = 0;
	  if(*(++fmt) == 'l')
	    {
	      long_flag = 1;
	      fmt++;
	    }
	  switch(*(fmt++))
	    {
	    case 'b':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_bin(&bufptr,&remain,argui,long_flag);
	      break;
	    case 'c':
	      *(bufptr++) = va_arg(args,int);
	      remain--;
	      break;
	    case 'd':
	    case 'i':
	      if(long_flag)
		argsi = va_arg(args,long long int);
	      else
		argsi = va_arg(args,int);
	      convert_dec(&bufptr,&remain,argsi,long_flag);
	      break;
	    case 'o':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_oct(&bufptr,&remain,argui,long_flag);
	      break;
	    case 's':
	      argstr = va_arg(args,char *);
	      bufptr = strncpy(bufptr,argstr,remain);
	      remain -= strlen(argstr);
	      bufptr += strlen(argstr);
	      if(remain<=0)
		bufptr = buf+bufsize-1;
	      break;
	    case 'u':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_udec(&bufptr,&remain,argui,long_flag);
	      break;
	    case 'X':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_hex(&bufptr,&remain,argui,long_flag,'A');
	      break;
	    case 'x':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_hex(&bufptr,&remain,argui,long_flag,'a');
	      break;
	    default:
	      return -1;
	    }
	}
    }
  if(remain>=0)
    *bufptr = 0;
  va_end(args);
  return bufsize-remain;
};




#define KPRINTF_BUFSIZE 1024

int kprintf(char *fmt,...)
{

  int fd = 0;

  /* this routine is not re-entrant! */
  static char buf[KPRINTF_BUFSIZE];
  int remain = KPRINTF_BUFSIZE;

  int long_flag;

  unsigned long long int argui;
  long long int argsi;
  char argc;
  char *argstr;

  char *bufptr = buf;

  va_list args;
  va_start(args,fmt);


  while((*fmt != 0)&&(remain>0))
    {
      if(*fmt != '%')
	{ 
	  *(bufptr++) = *(fmt++);
	  remain--; 
/* 	  printf("char: %c bufptr: %X remain:%d\n",*(bufptr-1),bufptr,remain); */
	}
      else
	{ /* begin format sequence */
	  long_flag = 0;
	  if(*(++fmt) == 'l')
	    {
	      long_flag = 1;
	      fmt++;
	    }
	  switch(*(fmt++))
	    {
	    case 'b':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_bin(&bufptr,&remain,argui,long_flag);
	      break;
	    case 'c':
	      *(bufptr++) = va_arg(args,int);
	      remain--;
	      break;
	    case 'd':
	    case 'i':
	      if(long_flag)
		argsi = va_arg(args,long long int);
	      else
		argsi = va_arg(args,int);
	      convert_dec(&bufptr,&remain,argsi,long_flag);
	      break;
	    case 'o':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_oct(&bufptr,&remain,argui,long_flag);
	      break;
	    case 's':
	      argstr = va_arg(args,char *);
	      bufptr = strncpy(bufptr,argstr,remain);
	      remain -= strlen(argstr);
	      bufptr += strlen(argstr);
	      if(remain<=0)
		bufptr = buf+KPRINTF_BUFSIZE-1;
	      break;
	    case 'u':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_udec(&bufptr,&remain,argui,long_flag);
	      break;
	    case 'X':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_hex(&bufptr,&remain,argui,long_flag,'A');
	      break;
	    case 'x':
	      if(long_flag)
		argui = va_arg(args,unsigned long long int);
	      else
		argui = va_arg(args,unsigned int);
	      convert_hex(&bufptr,&remain,argui,long_flag,'a');
	      break;
	    default:
	      return -1;
	    }
	}
    }
  va_end(args);
  if(!remain)
    buf[KPRINTF_BUFSIZE-1] = 0;
  else
    *bufptr = 0;

  bufptr = buf;

  char_write(console_major,console_minor,bufptr,KPRINTF_BUFSIZE-remain-1);
  
  return KPRINTF_BUFSIZE-remain-1;

};



