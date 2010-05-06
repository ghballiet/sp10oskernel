
#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <misc.h>
#include <stddef.h>
#include <sys/types.h>
#include <linker_vars.h>



/* Functions to initialize the kernel page table are in mmap.h 
   This file provides functions to manipulate user page tables.
*/

/* allocate a new page table, copy all of the kernel portion of the
   page table into it, and initialize the private portion of the page
   table */
phys_addr pt_new();

/* Release a page table and all physical memory referenced by it. */
void pt_delete(phys_addr page_table_phys);

/* The following routine adds an entry into the page table pointed to
   by page_table_phys. This routine only maps one page of memory, but
   can be called multiple times to map more pages. */
void pt_add_mapping(phys_addr page_table_phys, void *virt, uint32_t phys);

/* The following routine removes an entry from the page table pointed
   to by page_table_phys, and returns its corresponding physical
   address. On error, the least significant bit of the physical
   address is set to 1.  This routine only removes one page of memory,
   but can be called multiple times to remove multiple pages. */
phys_addr pt_remove_mapping(phys_addr page_table_phys, void *virt); 

/* virt_to_phys takes the physical address of a top level page table
   and a virtual address, and returns the corresponding physical
   address.  */                                                           
phys_addr virt_to_phys(phys_addr page_table_phys, void *virt);

/* phys_to_virt takes the physical address of a top level page table
   and a physical address, and returns the corresponding virtual
   address. */                                              
void *phys_to_virt(phys_addr page_table_phys, phys_addr phys);

#endif
