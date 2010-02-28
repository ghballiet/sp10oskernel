
/* each architecture must define a list of the devices that
   are connected.  

   A better approach would be to have platform and arch and
   have devices defined according to platform. 
*/



#ifndef DEV_LIST_H
#define DEV_LIST_H

#include <sys/types.h>

extern uint16_t console_major;
extern uint16_t console_minor;


typedef struct{
  int major;
  void *address;
}devdef;

extern devdef char_dev_list[];
extern devdef block_dev_list[];

int arch_init();

#endif
