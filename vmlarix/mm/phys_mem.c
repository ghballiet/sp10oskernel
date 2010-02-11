#include <phys_mem.h>
#include <bitmap.h>
#include <mem_list.h>

/* the following is for the SA1110,  and should be defined somewhere else.  I'm
   open for suggestions. */
#define ARCH_NUM_PAGES 0x100000

#define PAGE_SIZE 32768
#define WORD_SIZE UNIT_BITS

int max_page;

/* Declare a bitmap.  Each bit will  correspond to one page of physical memory.
   A value of 0 indicates that the corresponding page is unavailable */
static bitmap_t bitmap[ARCH_NUM_PAGES/32];


/* phys_mem_init scans memory and initializes  a bit map to track all available
   physical pages. It uses information provided by the arch to know which pages
   cannot have RAM and should not be scanned */
void phys_mem_init()
{

  /* page size is 4KB */
  /* word size is 32 bits (UNIT_BITS in bitmap.h) */
  /* 4KB / 32b = 1024 so 1024 words per page */
  /* (up to) 4GB of physical memory, starting at address 0xC0000000 and
     covering the 2^30 addresses through 0xFFFFFFFF */

  /* TODO: disable the cache before probing */

  int p = 0;
  int start_phys_mem = 0xC0000000;
  while(p < ARCH_NUM_PAGES) {
    int *start_addr = (int*)(start_phys_mem + (p * (PAGE_SIZE / WORD_SIZE)));
    int *end_addr = (int *)(start_phys_mem + ((p+1) * (PAGE_SIZE / WORD_SIZE)) - 1);
    /* if this address falls in a restricted range, ignore it */
    /* otherwise, probe it */
    if(can_probe_region(start_addr, end_addr)) {
      /* if you try to read a nonexistant address, you just get 1s back */
      /* TODO: double-check this! */
      *start_addr = 0; 
      if(*start_addr == 0) { /* then this address exists */
	set_bit(bitmap, p);
      } else {
	kprintf("Probing failed at page %d (%x) (start addr %x).\n\r", p, p, start_addr);
	max_page = p-1;
	break;
      }
    }
    else {
      //kprintf("Not probing page %d (%x) (start addr %x).\n\r", p, p, start_addr);
    }
    p++;
  }
  
  kprintf("Last page probed successfully was %d (%x).\n\r", max_page, max_page);
  kprintf("Addresses for this page: %x, %x.\n\r",
	  (start_phys_mem + (max_page * (PAGE_SIZE / WORD_SIZE))),
	  (start_phys_mem + ((max_page+1) * (PAGE_SIZE / WORD_SIZE)) - 1));
  /* TODO: add some output statements (max page number, corresponding amount of
     physical memory, etc) */
}


/* return 1 if the  page defined by the given start and  end addresses does not
   overlap with any of the restricted memory ranges; return 0 otherwise */
int can_probe_region(int start_addr, int end_addr) {
  /* TODO: does this overlap with any of the restricted ranges? */

  /* noprobe_list is an array  containing noprobe_region structs with start and
     end phys_mem_t variables */

  /* for each entry in noprobe_list... */
  int i = 0;
  int a = noprobe_list[i].start;
  int b = noprobe_list[i].end;
  while(a != 0 || b != 0) {
    if((start_addr <= a && end_addr >= a) ||
       (start_addr >= a && end_addr <= b) ||
       (start_addr <= b && end_addr >= b)) {
      return 0;
    }
    i++;
    a = noprobe_list[i].start;
    b = noprobe_list[i].end;
  }
  return 1;
}

/* phys_mem_get_pages allocates a given  number of CONTIGUOUS pages of physical
   memory, returning  the physical address of  the first page.   A return value
   with  the  least  significant  bit  set  means that  there  are  not  enough
   contiguous free pages.  The 'align' parameter specifies alignment, in pages,
   of the  memory allocated.   align=1 means align  on a page  boundary align=2
   means align on a two page (8Kb)  boundary align=3 means align on a FOUR page
   (16Kb) boundary etc */
phys_mem_t phys_mem_get_pages(uint32_t pages, uint32_t align)
{
}


/* phys_mem_free marks the page(s) beginning at physical address "address" as
   free */
void phys_mem_free_page(phys_mem_t address, uint32_t pages)
{
  /* assuming address is always the start of a page... */

  int p = (address * WORD_SIZE) / PAGE_SIZE;
  int i = 0;
  while(i < pages) {
    set_bit(bitmap, p+i);
    i++;
  }
}


/* get the number of free physical pages */
uint32_t phys_mem_count_free()
{
}

