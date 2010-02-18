#include <phys_mem.h>
#include <bitmap.h>
#include <mem_list.h>
#include <kprintf.h>
#include <linker_vars.h>

/* the following is for the SA1110, and should be
   defined somewhere else.  I'm open for suggestions. */

/* The number of bits in a physical address */
#define PHYS_ADDR_BITS 32
/* The number of bits of address within each page */
#define PAGE_BITS 12
/* The number of physical pages in the address space */
#define ARCH_NUM_PAGES (1<< (PHYS_ADDR_BITS - PAGE_BITS))
/* The number of bits in a bitmap_t (number of bytes times 8) */
#define BITMAP_T_BITS (sizeof(bitmap_t)<<3)

/* Declare a bitmap.  Each bit will correspond to one page
   of physical memory.  A value of 0 indicates that the
   corresponding page is unavailable */
static bitmap_t bitmap[ARCH_NUM_PAGES/BITMAP_T_BITS];
/* Declare a variable to track the number of free pages */
static uint32_t num_free = 0;

/* phys_mem_init scans memory and initializes a bit map to track all available
   physical pages. It uses information provided by the arch to know which pages
   cannot have RAM and should not be scanned */
void phys_mem_init()
{
  phys_mem_t i;
  phys_mem_t j;

  /*set the default to free for all pages! */
  for(i=0;i<ARCH_NUM_PAGES/BITMAP_T_BITS;i++) 
    bitmap[i] = ~(bitmap_t)0;

  j=0;
  /* Mark the reserved pages as used! */
  for(j=0;(noprobe_list[j].start!=0)||(noprobe_list[j].end!=0);j++)
    for(i=(noprobe_list[j].start>>PAGE_BITS);
	i<=(noprobe_list[j].end>>PAGE_BITS);
	i++ ) 
      clear_bit(bitmap,i);

  /* Also mark the pages the kernel is using. */
  phys_mem_mark_range((phys_mem_t)__kernel_ram_start__, 
		      ((uint_32_t)(__kernel_ram_end__ - __kernel_ram_start__));

  /* check all remaining pages! */
  int counter=0;
  for(i=0;i<ARCH_NUM_PAGES;i++) 
    {
      if(get_bit(bitmap,i)==1)
	{
	  phys_mem_t address=i<<PAGE_BITS;
	  uint32_t *test=(uint32_t*)address;
	  (*test)=0xA5A5A5A5;
	  if(*test!=0xA5A5A5A5)
	    clear_bit(bitmap, i);
	  else
	    counter++;	
	}
    }
  num_free = counter;
}

/* phys_mem_get_pages allocates a given number of CONTIGUOUS pages of physical
   memory, returning the physical address of the first page.  A return value
   with the least significant bit set means that there are not enough
   contiguous free pages.  The 'align' parameter specifies alignment, in pages,
   of the memory allocated.
   align=1  means align on a one page boundary
   align=2  means align on a two page boundary
   align=3  means align on a FOUR page boundary
   etc
*/
phys_mem_t phys_mem_get_pages(uint32_t pages, uint32_t align)
{
  static uint32_t current=0;
  uint32_t stride = (1<<(align-1));
  uint32_t mask = ~(stride-1);
  uint32_t first;
  int wrapped=0;
  int found=0;
  int i;

  current &= mask;  /* adjust for alignment */
  first = current;  /* keep track of where we started */

  do
    {
      /* find a properly aligned bit */
      found = get_bit(bitmap,current);
      while(!wrapped && !found)
	{
	  current += stride;
	  if((current+pages) > ARCH_NUM_PAGES)
	    current=0;
	  if(current==first)
	    wrapped=1;
	  else
	    found = get_bit(bitmap,current);
	}

      /* now see if there are enough contiguous bits */
      for(i=current+1;i<(current+pages);i++)
	found &= get_bit(bitmap,i);      
    }
  while(!wrapped && !found);

      /* if(found) */
      /* 	kprintf("found a bit at %08X\n\r",current); */
      /* else */
      /* 	kprintf("nothing found at %08X\n\r",current); */


  /* if there is no set of bits to satisfy the request, then return
     error code */
  if(wrapped)
    return PHYS_MEM_NONE;

  /* otherwise, clear the bits and return the address */
  num_free -= pages;
  for(i=current;i<(current+pages);i++)
    clear_bit(bitmap,i);

#ifdef DEBUG
  kprintf("phys_mem_get_pages returning %08X\n\r",current<<PAGE_BITS);
#endif
  return current<<PAGE_BITS;  
}


/* phys_mem_free marks the page(s) beginning at physical
   address "address" as free */
void phys_mem_free_page(phys_mem_t address, uint32_t pages)
{
  int i;
  address = address>>PAGE_BITS;
  for(i=address;i<(address+pages);i++)
    set_bit(bitmap,i);
  num_free += pages;
}


/* get the number of free physical pages */
uint32_t phys_mem_count_free()
{
  return num_free;
}

/* Mark the pages corresponding to a range of addresses in the physical memory
   bitmap as in use.

   'size' is the number of addresses in the range.  */
void phys_mem_mark_range(phys_mem_t address, uint32_t size) {
  int i;
  for(i=(address >> PAGE_BITS); i < ((address + size) >> PAGE_BITS); i++)
    clear_bit(bitmap, i);
  num_free -= ((address + size) >> PAGE_BITS) - (address >> PAGE_BITS);
}
