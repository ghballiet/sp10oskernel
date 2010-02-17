
/* Each architecture must define a list of physical memory regions
   that cannot contain RAM and should not be probed.  

   A better approach would be to have platform and arch and
   have devices defined according to platform. 
*/

#ifndef MEM_LIST_H
#define MEM_LIST_H

#include <sys/types.h>

typedef struct{
  phys_mem_t start;
  phys_mem_t end;
}noprobe_region;

extern noprobe_region noprobe_list[];

#endif
