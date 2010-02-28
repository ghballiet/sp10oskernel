
#ifndef MMU_H
#define MMU_H
#include <sys/types.h>

/* These routines are achitecture specific, and are probably
   written in assembly language.  Each architecture must
   supply its own version of these routines */

/* Set up the kernel page table.  This is called before the
   MMU is enabled */
phys_mem_t setup_kernel_page_table();

/* Initialize the MMU with its first page table, and prepare it to
   be enabled */
void set_initial_page_table(phys_mem_t page_table);

/* Change the page table that is loaded into the MMU. This routine may need
   to flush the cache and do other work as well.  depends on the architecture
*/
void set_page_table(phys_mem_t page_table);

/* Get the physical address of the page table that is currently being
   used by the the MMU */
phys_mem_t get_page_table();

/* Enable the MMU */
void enable_mmu();

/* Disable the MMU */
void disable_mmu();

/* Set a single entry in a page table.  entry is the virtual address
   of the page table entry. */ 
void set_page_table_entry(void *entry,uint32_t value);

/* Set up the stacks.  This should be called after the MMU is enabled */
void setup_stacks();

#endif
