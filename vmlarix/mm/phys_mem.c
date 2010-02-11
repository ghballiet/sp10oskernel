#include <phys_mem.h>
#include <bitmap.h>
#include <mem_list.h>

/* the following is for the SA1110, and should be
   defined somewhere else.  I'm open for suggestions. */
#define ARCH_NUM_PAGES 0x100000
#define PAGE_SIZE 32768
#define WORD_SIZE 8

/* Declare a bitmap.  Each bit will correspond to one page
   of physical memory.  A value of 0 indicates that the
   corresponding page is unavailable */
static bitmap_t bitmap[ARCH_NUM_PAGES/WORD_SIZE];

/* phys_mem_init scans memory and initializes a bit 
   map to track all available physical pages. It uses
   information provided by the arch to know which
   pages cannot have RAM and should not be scanned */
void phys_mem_init() {
  kprintf("\r\n\nStart memory init...\r\n\n");
  int p = 0;
  while(p < ARCH_NUM_PAGES) {
    int first_addr = p * (PAGE_SIZE / WORD_SIZE);
    int last_addr = ((p+1) * (PAGE_SIZE / WORD_SIZE)) - 1;
    
    /* if this address is in a restricted range, ignore it */
    /* otherwise, probe it */
    
    if(first_addr > 0xC00FFFFF) {
      can_probe_page(first_addr,last_addr);
    }
    
    // if(can_probe_page(first_addr,last_addr)) {
    //       kprintf("%d\t%X\t%X\r\n",p,first_addr,last_addr);      
    //       kprintf("contents = %X\r\n", *((int*)first_addr));
    //       *((int*)first_addr) = 0;
    //       kprintf("contents = %X\r\n", *((int*)first_addr));
    //       if(*((int *)first_addr) == 0) {
    //         set_bit(bitmap,p);
    //       } else {
    //         kprintf("\n\n\n!!!!\tEND OF MEMORY REACHED\t!!!!\r\n");
    //         break;
    //       }
    //     } else {
    //       /* reserved memory */
    //     }
    
    p++;
  }
  kprintf("\r\n\nEnd memory init at %d\r\n\n",p);
}

/* returns 1 if we can probe this address */
/* returns 0 otherwise */
int can_probe_page(int start, int end) {
  int i = 0;
  while(1) {
    int saddr = noprobe_list[i].start;
    int eaddr = noprobe_list[i].end;
    
    if(saddr == 0 && eaddr == 0) {
      kprintf("%X - OKAY\r\n",start);
      return 1;
    } else {
      /* three cases:
       * 1) start | end       |
       * 2)       | start end |
       * 3)       | start     | end
       */
       if(start <= saddr && end >= saddr) {
         return 0;
       } else if(start >= saddr && end <= eaddr) {
         return 0;
       } else if(start >= saddr && end >= eaddr) {
         return 0;
       }
    }
    
    i++;
  }
}

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
phys_mem_t phys_mem_get_pages(uint32_t pages, uint32_t align)
{
}


/* phys_mem_free marks the page(s) beginning at physical
   address "address" as free */
void phys_mem_free_page(phys_mem_t address, uint32_t pages)
{
}


/* get the number of free physical pages */
uint32_t phys_mem_count_free()
{
}

