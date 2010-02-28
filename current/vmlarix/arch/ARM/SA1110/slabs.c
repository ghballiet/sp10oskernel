
#include <mmap.h>
#include <slabs.h>
#include <linker_vars.h>
#include <sys/types.h>
#include <stddef.h>

/* The slab allocator manages mapping and unmapping of the virtual
   memory between __kernel_heap_start__ and __kernel_heap_end__, using
   an array of second level page tables.  Let X = __kernel_heap_end__
   - __kernel_heap_start__. X is the number of bytes of memory in that
   range, so it takes (X+PAGESIZE-1)/PAGESIZE pages to fully map the
   region.  Therefore, it takes (X+PAGESIZE-1)/PAGESIZE second level
   page table entries to cover the region.  */

/* Here, we define a macro that evaluates to the number of second
   level page table entries required for the heap. Making sure that
   NUM_PTS is evenly divisible by 256 */

/* The following macros must match __kernel_heap_end__ and
   __kernel_heap_start__ defined by the linker script */
#define __KERNEL_HEAP_END__ 0xE0000000
#define	__KERNEL_HEAP_START__ 0xD0000000

#define NUM_PTS (((__KERNEL_HEAP_END__-__KERNEL_HEAP_START__ + PAGESIZE - 1)>>12)&0xFFFFFF00)

/* This is the number of top level page table entries that are
   required in order to reference the second level page tables: */
#define NUM_TLPTS (NUM_PTS>>8)

/* Next, we define the second level page tables that kmalloc will use
   for managing pages in the kernel heap space.  A coarse second level
   page table has 256 entries, requiring 1Kb of memory.  The second
   level page table must be aligned on a 1K boundary. kmalloc will use
   NUM_TLPTS second level page tables to manage the kernel heap.  The
   linker will supply the starting and ending addresses for the kernel
   heap space. That will tell us which entries in the top level page
   table need to be set to point at second level page tables.

   Note that we have set up the kmalloc page tables so that they are a
   single array of page table entries.  Each entry can map a single 4
   Kb page, and there is a total of NUM_PTS entries. Which spans the
   range of the kernel heap with a single long array of coarse page
   table entries.  This layout allows us to quickly add and remove
   pages from the kernel heap space. 
*/

second_level_page_table __attribute__ ((aligned (1024)))
        kmalloc_page_tables[NUM_PTS];

/* slab_init sets up the second level page tables. */
void slab_init()
{
  int i,entry;
  phys_mem_t offset;

  /* First, Set all second level page table entries to "fault" (key = 0) */
  for(i=0;i<NUM_PTS;i++)
    kmalloc_page_tables[i].fault.key=0;

  /* Then set the top level kernel page table entries for all
     addresses between __kernel_heap_start__ and __kernel_heap_end__
     so that they each refer to a second level page table. */

  kprintf("Setting up page tables for kernel heap from %X to %X\n\r",
 	  __kernel_heap_start__,__kernel_heap_end__);

  if(((int)__kernel_heap_start__ != __KERNEL_HEAP_START__)||
     ((int)__kernel_heap_end__ != __KERNEL_HEAP_END__))
    {
      kprintf("slab_init(): size mismatch.\n\r");
      kprintf("  __kernel_heap_start__=%X\n\r", __kernel_heap_start__);
      kprintf("  __KERNEL_HEAP_START__=%X\n\r", __KERNEL_HEAP_START__);
      kprintf("  __kernel_heap_end__=%X\n\r", __kernel_heap_end__);
      kprintf("  __KERNEL_HEAP_END__=%X\n\r", __KERNEL_HEAP_END__);
      panic("  slabs.c does not match ld definitions.");
    }

  offset= (phys_mem_t)__kernel_heap_start__>>20;

  for(entry = 0; entry < NUM_TLPTS; entry++)
    {
      /* Note: since the MMU is not enabled, the virtual address of the
 	 second level page table is the same as its physical address, so
 	 we can just use the virtual address.
      */
      kernel_page_table[entry+offset].CPT.key=1;
      kernel_page_table[entry+offset].CPT.SBZ=0;
      kernel_page_table[entry+offset].CPT.Domain=3;
      kernel_page_table[entry+offset].CPT.IMP=0;
      kernel_page_table[entry+offset].CPT.base_address=
	(((phys_mem_t)kmalloc_page_tables)>>10)+entry;
    }

}

void *slab_create(int slab_pages)
{
  phys_mem_t phys;
  second_level_page_table t;
  static int index = 0;
  int found = 0;
  int wrapped = 0;
  int i;
  int orig_index = index-slab_pages;
  void *address;
  uint32_t t_int;
  /* scan through kmalloc_page_tables to find slab_pages of unused
     virtual memory */
  while(!found && !wrapped)
    {
      if((index + slab_pages) >= NUM_PTS)
	index = 0;
      if(index == orig_index)
	wrapped = 1;
      else
	{
	  if(kmalloc_page_tables[index].fault.key == 0)
	    {
	      found = 1;
	      for(i=0;i<slab_pages;i++)
		{
		  if(kmalloc_page_tables[index+i].fault.key != 0)
		    found = 0;
		}
	    }
	  if(!found)
	    {
	      index+= slab_pages;
	      if((index + slab_pages) >= NUM_PTS)
		index = 0;
	    }
	}
    }

  if(wrapped)
    panic("Kernel ran out of heap space.");

  address = ((void *)__kernel_heap_start__) + (index<<12);

  /* found a block of virtual memory.  Now map it to phys pages */
  t.small_page.key = 2;
  t.small_page.AP3 = AP_NO_USER;
  t.small_page.AP2 = AP_NO_USER;
  t.small_page.AP1 = AP_NO_USER;
  t.small_page.AP0 = AP_NO_USER;
  t.small_page.C = 1;
  t.small_page.B = 1;

  for(i=0;i<slab_pages;i++)
    {
      phys = phys_mem_get_pages(1,1);
      if(phys & 1)
	panic("Unable to allocate a physical page.");
      t.small_page.base_address = phys>>12;
      t_int = *((uint32_t *)&t);
      set_page_table_entry(&kmalloc_page_tables[index++],t_int);
    }

  return address;
}


/* release a slab */
void slab_destroy(void *pt,int slab_pages)
{
  second_level_page_table t;
  int i;
  uint32_t index;
  phys_mem_t phys;
  uint32_t t_int;

  t.small_page.key = 0;
  t.small_page.AP3 = AP_NO_USER;
  t.small_page.AP2 = AP_NO_USER;
  t.small_page.AP1 = AP_NO_USER;
  t.small_page.AP0 = AP_NO_USER;
  t.small_page.C = 1;
  t.small_page.B = 1;
  t.small_page.base_address = 0;
  t_int = *((uint32_t *)&t);

  for(i=0;i<slab_pages;i++)
    {
      /* remove mappings from page tables and free the physical memory */
      index = (((uint32_t)pt)-((uint32_t)&__kernel_heap_start__)>>12)+i;
      phys = kmalloc_page_tables[index].small_page.base_address<<12;
      phys_mem_free_page(phys,1);
      set_page_table_entry(&kmalloc_page_tables[index],t_int);
    }
}

void *kphys_to_virt(phys_mem_t p)
{
  return NULL;
}

phys_mem_t kvirt_to_phys(void *v)
{
  return 0;
}

