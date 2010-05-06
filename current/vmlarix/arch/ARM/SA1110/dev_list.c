
#include <dev_list.h>
#include <dev_majors.h>
#include <stddef.h>
#include <linker_vars.h>

#define UART_BASE (void *)0x80010000
#define RAMDISK_BASE (void*)__RAMDISK_LOCATION__

int arch_init()
{ 
  console_major = 1;
  console_minor = 0;
  initrd_major = RAMDISK_major;
  initrd_minor = 0;
}

devdef char_dev_list[]={
  {1,UART_BASE},
  {0,NULL},
};

devdef block_dev_list[]={
  {RAMDISK_major,RAMDISK_BASE},
  {0,NULL}
};

