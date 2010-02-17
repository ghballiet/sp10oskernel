
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

/* Define the second level page tables that kmalloc will use for managing
   pages in the kernel heap space.  A second level page table has 1024
   entries, requiring 4Kb of memory.  The second level page table must be
   aligned on a 4K boundary. kmalloc will use 64 second level page tables to
   manage 256 Mb of kernel virtual memory.  The page tables themselves will
   consume 256 Kb. The linker will supply the starting and ending addresses
   for the kernel heap space. That will tell us which entries in the top
   level page table need to be set to point at second level page tables. 
   Note that we have set up the kmalloc page tables so that they are 
   a single array of page table entries.  Each entry can map a single
   4 Kb page, and there is a total of 65536 entries. Which covers 256 Mbytes
   of virtual memory with a single long array of coarse page table entries.
*/
#define KMALLOC_NUM_PAGE_TABLES 64

#define KMALLOC_PT_SIZE (KMALLOC_NUM_PAGE_TABLES << 10)

second_level_page_table __attribute__ ((aligned (4*1024))) 
        kmalloc_page_table[KMALLOC_PT_SIZE];

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
      /* insert your code here */
      /* set physical address */
      /* Outer and inner cache WB, no write allocate */
      /* no domain access checking */
      /* cacheable */
      /* bufferable */
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
  /* set physical address to E00 */
  /* Outer and inner cache WB, no write allocate */
  /* no domain access checking */
  /* not cacheable */
  /* bufferable */
  
  /* Create a section entry for our minicache flush area.  The linker gives
     us the virtual address as __minicacheflush_start__.  We look at our
     SA1110 memory map in our Manual to and get the physical address of the
     zeros page. Choose an address that does not overlap the cache flush
     area chosen above. This area is used for clearing the minicache.  We
     clear the minicache by filling it with data from this part of memory.
     Very clunky way to clear a cache.  Newer ARMs have better methods.
     (B=0,C=0) */

  /* insert your code here*/
  
  /* Create sections for mapping our I/O registers.  The linker
     Does not tell us about these.  We have to look at the manual
     and the memory map that we have designed.  For the SA1110,
     the I/O registers that are in the region from 80000000 through BFFFFFFF,
     and we want a direct mapping, just like the kernel code.
     I/O registers should not be buffered or cached.
  */
  
  /* insert your code here */
  
  /* Take 1 meg of physical memory from directly after __kernel_ram_end__ 
     and map it to __stack_start__. This section will hold the
     OS stacks and the Interrupt Vector Table. MAKE SURE PHYS_MEM knows
     these pages are used. 
  */

  /* Now, it is time to set up the second level page tables that
     kmalloc will use to manage kernel heap space.  */
  /* First, Set all second level page table entries to "fault" (key = 0) */
  for(i=0;i<KMALLOC_PT_SIZE;i++)
    kmalloc_page_table[i].fault.key=0;

  /* Then set the top level kernel page table entries for all addresses between
     __kernel_heap_start__ and __kernel_heap_end__ so that they each refer to
     a second level page table. */

      /* Note: since the MMU is not enabled, the virtual address of the 
	 second level page table is the same as its physical address, so
	 we can just use the virtual address. 
      */

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


