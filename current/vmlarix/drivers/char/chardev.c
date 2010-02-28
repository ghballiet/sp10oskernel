

#include <chardev.h>
#include <dev_list.h>
#include <dev_majors.h>
#include <16550.h>

/* define an array of char devices */

char_device char_dev[CHAR_DEV_MAX];

/* this routine calls the init method for all of the char device drivers */
uint32_t char_dev_init(devdef chardevs[])
{
  int32_t i;
  
  // show all char devices as unregistered
  for(i=0;i<CHAR_DEV_MAX;i++)
    char_dev[i].registered = 0;
  
  // Make the init call for each character driver
  for(i=0; chardevs[i].major != 0; i++)
    {
      switch(chardevs[i].major)
	{
	case UART_16550_major:
	  // find and initialize all UARTs
	  char_dev[UART_16550_major].registered = 
	    UART_16550_init(chardevs[i].address);
	  char_dev[UART_16550_major].read_fn=
	    (uint32_t (*)(uint16_t,uint8_t *,uint32_t))&UART_16550_read;
	  char_dev[UART_16550_major].write_fn=
	    (uint32_t (*)(uint16_t,uint8_t *,uint32_t))&UART_16550_write;
	default:
	  // attempted to register unknown device major
	  ;
	}  
    }
}


int32_t char_write(uint16_t major,uint16_t minor,char *buff,uint32_t buflen)
{
  /* examine the char_dev array and find out if the major device exists */

  /* if so, call the driver's write method with the minor number and
     all other parameters. */

  /* if not,  return an error code */

	if(major>=CHAR_DEV_MAX||major==0)
		return -1;

	if(char_dev[major].registered==0)
		return -2;

	return char_dev[major].write_fn(minor,buff,buflen);
	
	//UART_16550_write(minor,buff,buflen);
}

int32_t char_read(uint16_t major,uint16_t minor,char *buff,uint32_t buflen)
{
  /* examine the char_dev array and find out if the major device exists */

  /* if so, call the driver's read method with the minor number and
     all other parameters. */

  /* if not,  return an error code */

//UART_16550_read(minor,buff,buflen);
	if(major>=CHAR_DEV_MAX||major==0)
		return -1;

	if(char_dev[major].registered==0)
		return -2;

	return char_dev[major].read_fn(minor,buff,buflen);

}

int32_t char_ioctl(uint16_t major,uint16_t minor,...)
{
  /* examine the char_dev array and find out if the major device exists */

  /* if so, call the driver's ioctl method with the minor number and
     all other parameters. */

  /* if not,  return an error code */

}

