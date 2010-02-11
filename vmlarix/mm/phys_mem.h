
#ifndef PHYS_MEM_H
#define PHYS_MEM_H
#include <stdint.h>
#include <sys/types.h>

/* phys_mem_init scans memory and initializes a bit 
   map to track all available physical pages. It uses
   information provided by the arch to know which
   pages cannot have RAM and should not be scanned */
void phys_mem_init();

/* phys_mem_get_pages allocates a given number of 
   CONTIGUOUS pages of physical memory, returning the
   physical address of the first page.  A return
   value with the least significant bit set means
   that there are not enough contiguous free pages. 
   The 'align' parameter specifies alignment, in pages,
   of the memory allocated.  
   align=1  means align on a page boundary
   align=2  means align on a two page (8Kb) boundary
   align=3  means align on a FOUR page (16Kb) boundary
   etc
*/
phys_mem_t phys_mem_get_pages(uint32_t pages, uint32_t align);

/* phys_mem_free marks the page(s) beginning at physical
   address "address" as free */
void phys_mem_free_page(phys_mem_t address, uint32_t pages);

/* get the number of free physical pages */
uint32_t phys_mem_count_free();

#endif
