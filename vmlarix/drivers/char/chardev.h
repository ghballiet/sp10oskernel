

#ifndef CHARDEV_H
#define CHARDEV_H

#ifdef NEED_STDINT
#include <stdint.h>
#endif
#include <sys/types.h>

/* this is the maximum for the major number of a char device.   */
/* Char device drivers can specify their own max minor numbers. */
#define CHAR_DEV_MAX 1024

/* define a general structure for char devices */

typedef struct {
  uint32_t registered;
  char *name;
  void *private;
  uint32_t (*read_fn)(uint16_t,uint8_t *,uint32_t);
  uint32_t (*write_fn)(uint16_t,uint8_t *,uint32_t);
  uint32_t (*ioctl)(uint16_t,...);
}char_device;


/* define an array of char devices */
extern char_device char_dev[CHAR_DEV_MAX];


/* this routine calls the init method for all of the char device drivers */
uint32_t char_dev_init();

int32_t char_write(uint16_t major,uint16_t minor,char *buff,uint32_t buflen);
int32_t char_read(uint16_t major,uint16_t minor,char *buff,uint32_t buflen);
int32_t char_ioctl(uint16_t major,uint16_t minor,...);


#endif
