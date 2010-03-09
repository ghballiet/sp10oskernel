
#include <mmap.h>
#include <misc.h>
#include <stddef.h>
#include <sys/types.h>
#include <linker_vars.h>
#include <mmu.h>
#include <kprintf.h>


/* Define a top level page table dividing our 4Gb virtual memory into
   chunks of 1M each.  A top level page table requires 16Kb of memory
   and must be aligned on a 16 Kb boundary. The first entry in the table
   maps virtual addresses 0x00000000 to 0x000FFFFF.  The second entry
   in the table maps virtual addresses 0x00100000 through 0x001FFFFF, and
   so on, up to entry 4095 (0xFFF), which maps addresses 0xFFF00000 through 
   0xFFFFFFFF */
first_level_page_table __attribute__ ((aligned (16*1024))) kernel_page_table[0x1000];


/* fast_virt_to_phys looks in the kernel top level page table to
   convert a virtual address to a physical address              
*/                                                              
phys_mem_t fast_virt_to_phys(void *v)                           
{                                                               
  /* convert v to a top level page table index */               
  uint32_t vi = ((uint32_t)v)>>20;                            

  /* and an offset */
  uint32_t vo = ((uint32_t)v)&0x000FFFFF;

  /* retrieve the entry from the top level page table */
  first_level_page_table t = kernel_page_table[vi];     

  /* If it is a section mapping, then return the phys address.  The 12
     most significant bits of the phys address come from the page     
     table entry, and the remaining bits come from the virtual address
     given by v.                                                      
  */
  if(t.section.key == 2)
    return (t.section.base_address << 20) | vo;

  /* else signal an error */
  kprintf("Bad mapping in fast_virt_to_phys\n");
  return 1;
}

/* Set up the kernel page table.  Each process will get its own copy
   of this page table, with extra entries for its data, stack, text,
   bss, etc. Whenever we change the top level kernel page table, we
   may have to copy the changes to the page table of every process.
   With processors that support two page table base addresses, we
   could set up the kernel page table pointer separately from the
   process page tables.*/
phys_mem_t setup_kernel_page_table()
{
  void *t;
  uint32_t entry;
  uint32_t i,j;
  phys_mem_t p_addr;
  /* initialize the top level page table */
  for (entry=0;entry<4096;entry++)
    kernel_page_table[entry].fault.key = 0;

  /* Create section entries to do a direct mapping for addresses
     __kernel_ram_start__ through __kernel_ram_end__ (The KERNELRAM space
     defined by the linker).  In other words, all addresses in this
     range should have identical virtual and physical addresses.
     Remember to shift the address provided by the linker 20 bits to the right.
  */

  for(entry = (((phys_mem_t)__kernel_ram_start__)>>20);
      entry <= (((phys_mem_t)__kernel_ram_end__)>>20); 
      entry++)
    {
      /* set the key to indicate a section mapping */
      kernel_page_table[entry].section.key = 2;
      /* set physical address to match virtual address */
      kernel_page_table[entry].section.base_address = entry;
      /* set SBZ bits to zero */
      kernel_page_table[entry].section.SBZ = 0;
      /* set outer and inner cache to write back, no write allocate */
      kernel_page_table[entry].section.TEX = 0; 
      /* set access permissions to not let user level processes access this section */
      kernel_page_table[entry].section.AP = AP_NO_USER;
      /* IMP needs to be zero */
      kernel_page_table[entry].section.IMP = 0;
      /* disable domain access checking */
      kernel_page_table[entry].section.Domain  = 3; 
      /* set SBZ bits to zero */
      kernel_page_table[entry].section.SBZ_2 = 0;
      /* mark section as cacheable and bufferable */
      kernel_page_table[entry].section.C = 1; /* cacheable */
      kernel_page_table[entry].section.B = 1; /* bufferable */
    }

  /* Create a section entry for our cache flush area.  The linker gives us
     the virtual address as __cacheflush_start__.  We look at the SA1110
     memory map in our Manual to and get the physical address of the zeros
     page.  This area is used for clearing the cache.  We clear the cache by
     filling it with data from this part of memory.  Very clunky way to
     clear a cache.  Newer ARMs have better methods (C=0,B=1)
  */

  entry =  ((phys_mem_t)__cacheflush_start__)>>20;

  /* Looks like above, but no loop (we just make a single entry */

  /* insert your code here */
  kernel_page_table[entry].section.key = 2;
  /* set physical address to E00 */
  kernel_page_table[entry].section.base_address = 0xE00;
  kernel_page_table[entry].section.SBZ = 0;
  kernel_page_table[entry].section.TEX = 0; /* Outer and inner cache WB, 
					       no write allocate */
  kernel_page_table[entry].section.AP = AP_NO_USER;
  kernel_page_table[entry].section.IMP = 0;
  kernel_page_table[entry].section.Domain  = 3; /* no domain 
						   access checking */
  kernel_page_table[entry].section.SBZ_2 = 0;
  kernel_page_table[entry].section.C = 0; /* cacheable */
  kernel_page_table[entry].section.B = 1; /* bufferable */
  
  /* Create a section entry for our minicache flush area.  The linker gives
     us the virtual address as __minicacheflush_start__.  We look at our
     SA1110 memory map in our Manual to and get the physical address of the
     zeros page. Choose an address that does not overlap the cache flush
     area chosen above. This area is used for clearing the minicache.  We
     clear the minicache by filling it with data from this part of memory.
     Very clunky way to clear a cache.  Newer ARMs have better methods.
     (B=0,C=0) */

  entry =  ((phys_mem_t)__minicacheflush_start__)>>20;

  /* insert your code here */
  kernel_page_table[entry].section.key = 2;
  /* set physical address to E01 */
  kernel_page_table[entry].section.base_address = 0xE01;
  kernel_page_table[entry].section.SBZ = 0;
  kernel_page_table[entry].section.TEX = 0; /* Outer and inner cache WB, 
					       no write allocate */
  kernel_page_table[entry].section.AP = AP_NO_USER;
  kernel_page_table[entry].section.IMP = 0;
  kernel_page_table[entry].section.Domain  = 3; /* no domain 
						   access checking */
  kernel_page_table[entry].section.SBZ_2 = 0;
  kernel_page_table[entry].section.C = 0; /* cacheable */
  kernel_page_table[entry].section.B = 0; /* bufferable */
  
  /* Create sections for mapping our I/O registers.  The linker
     Does not tell us about these.  We have to look at the manual
     and the memory map that we have designed.  For the SA1110,
     the I/O registers that are in the region from 80000000 through BFFFFFFF,
     and we want a direct mapping, just like the kernel code.
     I/O registers should not be buffered or cached.
  */
  
  /* insert your code here */
  for(entry = 0x800;entry < 0xC00;entry++)
    {
      kernel_page_table[entry].section.key = 2;
      /* set physical address to match virtual address */
      kernel_page_table[entry].section.base_address = entry;
      kernel_page_table[entry].section.SBZ = 0;
      kernel_page_table[entry].section.TEX = 0; /* Outer and inner cache WB, 
						   no write allocate */
      kernel_page_table[entry].section.AP = AP_NO_USER;
      kernel_page_table[entry].section.IMP = 0;
      kernel_page_table[entry].section.Domain  = 3; /* no domain 
						       access checking */
      kernel_page_table[entry].section.SBZ_2 = 0;
      kernel_page_table[entry].section.C = 0; /* cacheable */
      kernel_page_table[entry].section.B = 0; /* bufferable */
    }
  
  /* Take 1 meg of physical memory from phys_mem manager
     and map it to __stack_start__. This section will hold the
     OS stacks and the Interrupt Vector Table. */
  p_addr = phys_mem_get_pages(256,9);
  entry =  ((phys_mem_t)__stack_start__)>>20;

  kernel_page_table[entry].section.key = 2;
  kernel_page_table[entry].section.base_address = p_addr>>20;
  kernel_page_table[entry].section.SBZ = 0;
  kernel_page_table[entry].section.TEX = 0; /* Outer and inner cache WB, 
					       no write allocate */
  kernel_page_table[entry].section.AP = AP_NO_USER;
  kernel_page_table[entry].section.IMP = 0;
  kernel_page_table[entry].section.Domain  = 3; /* no domain 
						   access checking */
  kernel_page_table[entry].section.SBZ_2 = 0;
  kernel_page_table[entry].section.C = 1; /* cacheable */
  kernel_page_table[entry].section.B = 1; /* bufferable */

  /* Map 1 meg starting at __RAMDISK_LOCATION__ */

  entry =  ((phys_mem_t)__RAMDISK_LOCATION__)>>20;

  /* insert your code here */
  kernel_page_table[entry].section.key = 2;
  /* set physical address to C07 */
  kernel_page_table[entry].section.base_address = entry;
  kernel_page_table[entry].section.SBZ = 0;
  kernel_page_table[entry].section.TEX = 0; /* Outer and inner cache WB, 
					       no write allocate */
  kernel_page_table[entry].section.AP = AP_NO_USER;
  kernel_page_table[entry].section.IMP = 0;
  kernel_page_table[entry].section.Domain  = 3; /* no domain 
						   access checking */
  kernel_page_table[entry].section.SBZ_2 = 0;
  kernel_page_table[entry].section.C = 1; /* cacheable */
  kernel_page_table[entry].section.B = 1; /* bufferable */

  /* OK, now the base kernel page table is set up....  Make sure that you
     marked the pages that were used as NOT free in the physical memory
     bitmap.

     In the no_probe_list, we reserved a bunch of pages for the kernel, but
     the kernel did not use them all.  Instead of reserving 8 Meg of kernel
     memory in the no_probe_list, phys_mem_init() should mark kernel pages
     by using __kernel_ram_start__ and __kernel_ram__ end.  Make that change
     and see how many pages become available.
  */

  /* We will add some more mappings to this file when we start working on
     the Memory Manager, but to enable the MMU and get kmalloc working
  */
 
  return (phys_mem_t) kernel_page_table;
}


