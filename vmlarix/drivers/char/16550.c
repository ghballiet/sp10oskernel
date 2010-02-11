/* Copyright */
/* Larry Pyeatt, Spring 2009 */
/* Greg McChesney, Spring 2009 */

#include <16550.h>
#include <chardev.h>
#include <stddef.h>

void* uart_16550_minors[UART_16550_MINOR_MAX];
int initialized = 0;

int32_t UART_16550_putchar(uint16_t minor, char c)
{
  if(minor>UART_16550_MINOR_MAX||uart_16550_minors[minor]==0)
    return 0;
	  	
  uint8_t *address=uart_16550_minors[minor];
  uint32_t status;
  do
    {
      status=*(address+0x20);
      status=status&4;
    }while(status!=4);

  *(address+0x14)=c;
  return 1;
}

int32_t UART_16550_write(uint16_t minor, uint8_t *msgptr, uint32_t len)
{
  if(minor>UART_16550_MINOR_MAX||uart_16550_minors[minor]==0)
    return 0;
  uint32_t count=0;
  while(*msgptr!=0&&count<=len)
    {
      UART_16550_putchar(0,*msgptr);
      *msgptr++;
      count++;
    }
  return 1;
}

uint8_t UART_16550_getchar(uint16_t minor)
{
  if(minor>UART_16550_MINOR_MAX||uart_16550_minors[minor]==0)
    return 0;
  char *address=(char *)uart_16550_minors[minor];
  uint32_t status;
  do
    {
      status=*(address+0x20);
      status=status&2;
    }while(status!=2);
  char c=(char)*(address+0x14);
  //UART_16550_putchar(minor,c);
  return c;
}

int32_t UART_16550_read(uint16_t minor, uint8_t *msgptr, uint32_t len)
{
  if(minor>UART_16550_MINOR_MAX||uart_16550_minors[minor]==0)
    return 0;

  uint32_t count=0;
  char *ptr=msgptr;
  //msgptr=UART_16550_getchar(minor);
  //while(1)
  char letter;
  do
    {
      letter=UART_16550_getchar(minor);
      UART_16550_putchar(minor,letter);
      *ptr=letter;
      ptr++;
      count++;
      if((int)letter==13)
	{
	  UART_16550_putchar(minor,'\n');
	  if(count<len)
	    {
	      *ptr='\n';
	      ptr++;
	      count++;
	    }
	}
    }while(count<len&&(int)letter!=13);
  return 1;
}

char mes[]="UART initilized with address ";
char mes2[]=" and minor ";

int32_t UART_16550_init(void *addr)
{
  if(!initialized)
    {
      int i=0;
      for(i=0;i<UART_16550_MINOR_MAX;i++)
	uart_16550_minors[i]=0;
      initialized=1;
    }
  int firstFreeMinor=0;
  while(firstFreeMinor<UART_16550_MINOR_MAX
	&&uart_16550_minors[firstFreeMinor]!=0)
    firstFreeMinor++;
  if(firstFreeMinor==UART_16550_MINOR_MAX) 
    //no more UART slots available
    return 0;

  uart_16550_minors[firstFreeMinor]=addr;

  char *address= (char *) addr;	
  *(address+0x1c) = 0xff;
  *(address+0x20) = 0xff;
  
  /* disable the UART */
  *(address+0x0c) = 0;
  
  /* Set the serial port to sensible defaults: no break, no interrupts, */
  /* no parity, 8 databits, 1 stopbit. */
  *(address+0x0) = 0x08;
  
  /* Set the Baud rate */
  *(address+0x04) = 0x03;
  *(address+0x08) = 0;

  /* enable the UART */
  *(address+0x0c) = 3;

  /* debug message */
  char *msgptr=mes;

  while(*msgptr!=0)
    {
        
      UART_16550_putchar(firstFreeMinor,*msgptr);
      msgptr++;
    }	
  //char *temp=*address;
  msgptr=mes;  
  int_to_hex(address,msgptr);   
  while(*msgptr!=0)
    {
      UART_16550_putchar(firstFreeMinor, *msgptr);
      msgptr++;
    }

  char *msgptr2=mes2;  
  while (*msgptr2!=0)
    {
      UART_16550_putchar(firstFreeMinor, *msgptr2);
      msgptr2++;

    }

  int_to_hex(firstFreeMinor,msgptr2);
  while(*msgptr2!=0)
    {
      UART_16550_putchar(firstFreeMinor, *msgptr2);
      msgptr2++;
    }
  UART_16550_putchar(firstFreeMinor, '\n');
  UART_16550_putchar(firstFreeMinor, '\r');

  return 1;
  
}
