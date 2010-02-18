#include <mem_list.h>

/* this is the list of physical memory regions that should not
   be probed when searching for physical memory pages. */
noprobe_region noprobe_list[]=
  {   
    {0x20000000,0x3FFFFFFF}, /* PCMCIA expansion bus area */
    {0x50000000,0x7FFFFFFF}, /* reserved space */
    {0x80000000,0xBFFFFFFF}, /* devices live here */
    //    {0xC0000000,0xC00FFFFF}, /* this is where the kernel is loaded */
    {0xE0000000,0xE7FFFFFF}, /* zeros bank */
    {0xE8000000,0xFFFFFFFF}, /* reserved space */
    {0,0}
  };
