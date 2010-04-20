#include <mem_list.h>
#include <linker_vars.h>

/* this is the list of physical memory regions that should not
   be probed when searching for physical memory pages. */
noprobe_region noprobe_list[]=
  {   
    {0x20000000,0x3FFFFFFF}, /* PCMCIA expansion bus area */
    {0x50000000,0x7FFFFFFF}, /* reserved space */
    {0x80000000,0xBFFFFFFF}, /* devices live here */
    /* {0xC0000000,0xC00FFFFF}, this is where the kernel is loaded -- 
	  phys_mem_init uses linker_vars to handle that now */
    {0xC0500000,0xC05FFFFF},  /* area mapped into kernel memory 
				for loading user processes for project 8 */
    {(uint32_t)__RAMDISK_LOCATION__, (uint32_t)__RAMDISK_LOCATION__+0x000FFFFF},
    {0xE0000000,0xE7FFFFFF}, /* zeros bank */
    {0xE8000000,0xFFFFFFFF}, /* reserved space */
    {0,0}
  };
