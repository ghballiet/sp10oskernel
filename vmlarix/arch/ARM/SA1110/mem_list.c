#include <mem_list.h>

/* this is the list of physical memory regions that should not
   be probed when searching for physical memory pages. */
noprobe_region noprobe_list[]=
  {   
    {0x00000000,0x3FFFFFFF}, /* static memory devices */
    {0x40000000,0x7FFFFFFF}, /* more devices */
    {0x80000000,0xBFFFFFFF}, /* devices live here */
    {0xC0000000,0xC00FFFFF}, /* this is where the kernel is loaded */
    {0,0}
  };
