
#ifndef SLABS_H
#define SLABS_H
#include <sys/types.h>

/* Initialize the slab allocator.  This must be called after
   setup_kernel_page_table(), but before the MMU is enabled */
void slab_init();

/* Allocate a slab of slab_pages*PAGESIZE bytes in virtual memory, and
   map some physical pages to it */
void *slab_create(int slab_pages);

/* Release a slab, unmapping and releasing its physical memory */
void slab_destroy(void *pt,int slab_pages);

/* Convert a physical address to the corresponding virtual address */
void *kphys_to_virt(phys_mem_t p);

/* Convert a virtual address to the corresponding physical address */
phys_mem_t kvirt_to_phys(void *v);

#endif
