
#ifndef PT_ALLOC_H
#define PT_ALLOC_H

#include <stddef.h>
#include <sys/types.h>
#include <linker_vars.h>
#include <mmap.h>

/* must be called to set up page tables for the slab allocators */
void pt_alloc_init();

/* fast phys_to_virt for level two page tables */
second_level_page_table *pt_l2_lookup_virt(phys_addr pt);

/* fast phys_to_virt for level one page tables */
first_level_page_table *pt_l1_lookup_virt(phys_addr pt);

/* fast virt_to_phys for level two page tables */
phys_addr pt_l2_lookup_phys(second_level_page_table *virt);

/* fast virt_to_phys for level one page tables */
phys_addr pt_l1_lookup_phys(first_level_page_table *virt);

/* Allocate and return the virtual address of a new second level page
   table.  */
second_level_page_table *pt_l2_alloc();

/* Allocate and return the virtual address of a new 
   second level page table.  */
first_level_page_table *pt_l1_alloc();

/* Put a second level page table back on the linked list. Keep virtual
   addresses sorted in ascending order, so that we can free a page when
   all four page tables in it have been freed.
*/
void pt_l2_free(phys_addr pt);

/* Allocate and return the physical address of a new top level page
   table.  */
first_level_page_table *pt_l1_alloc();

void pt_l1_free(phys_addr pt);

/* if we run out of free pages of phys mem, we can call this routine
   to try to get a page from the memory manager's data structures. It
   returns the number of pages released */
int pt_release_unused_pagetable_memory();


#endif
