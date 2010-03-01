
#include <stddef.h>
#include <sys/types.h> 
#include <kmalloc.h>
#include <mmap.h>
#include <slabs.h>

/* This file implements a slab-based memory manager.
   It provides two main functions: 
   void *kmalloc(size_t nbytes)
   kfree(void *p)
   From the outside, kmalloc and kfree look very much like the stdlib
   functions malloc() and free(), but internally, they are more suited
   to use within our kernel.
*/

/* Slabs are made up of a fixed number of pages: SLAB_PAGES */

/* Slabs are subdivided into objects of the same size, as many as will
   fit in the slab. There are slabs of one word objects, slabs of two
   word objects, slabs of four word objects, slabs of eight word
   objects, slabs of 16 word objects... 32...  64...
   128... 256... 512... up to SLAB_PAGES.  When kmalloc is called for
   an object larger than SLAB_PAGES*PAGE_SIZE, the object is allocated
   specially. */

/* When a slab is created, a small record is also allocated, which
   points to the slab data, points to the first available item in the
   slab, keeps track of how many free items there are, and also points
   to the next slab record of objects that are the same size.  */

/* Make SLAB_PAGES a power of 2!!! */
#define SLAB_PAGES 2
#define SLAB_WORDS ((PAGESIZE*SLAB_PAGES)>>2)
#define SLAB_BYTES (PAGESIZE*SLAB_PAGES)

/* when objects are stored in their slab (unallocated), we overlay
   them with this type so that we can form a linked list of
   objects. */
typedef struct ir{
  struct ir *next;
}item_rec;

/* Each slab has a slab_header structure to keep track of information
   about the slab and other slabs full of objects of the same size */
typedef struct slhead{
  uint32_t itemsize;
  uint32_t freeitems;
  uint32_t totalitems;
  char *slab;
  char *slab_end;
  item_rec *avail;
  struct slhead *next_head;
}slab_header;

/* Slabs are stored in rows. All slabs in a row contain objects of the
   same size.  Each row has a header */
typedef struct srhead{
  uint32_t itemsize;
  struct srhead *next_row;
  struct slhead *first_slab;
}slab_row_header;

/* If we need to allocate an object bigger than SLAB_PAGES, then we do
   a special allocation, and keep track of it in a special_slab
   structure.  The special_slab structures are stored on a linked
   list. */
typedef struct special_s{
  struct special_s *next;
  void *data;
  int pages;
}special_slab;

/* Declare a global pointer to the data structure for managing slabs */
static slab_row_header *slabs;

/* Declare a global pointer to the linked list of special slabs */
static special_slab *special_slabs;


/* You may need to define some helper functions here */

/* TODO: create a similar populate_first_slab function that will store slab and
   row headers. **How do we manage storing row/column headers?** */

/* Fills a slab with a linked list of item_rec objects for the given item size
   (in bytes). Returns the number of item_rec objects created.

   The address of the first item_rec item in the slab will initially be the
   start address of the slab itself.*/
uint32_t populate_slab_records(char *slab, char *slab_end, uint32_t item_size) {
  /* TODO: do I need to take *slab_end as an argument too, or could I figure
     that out from SLAB_BYTES? (Do I even need it at all?)*/
  uint32_t num_items = SLAB_BYTES / item_size;
  unsigned char *current = slab;
  uint32_t i;

  for(i=0; i<(num_items-1); i++) {
    ((item_rec*)current)->next = (item_rec*)(current + item_size);
    current = (unsigned char *)((item_rec*)current)->next;
  }

  ((item_rec*)current)->next = NULL;
  return num_items;
}


  
void kmalloc_init()
{

}

void *kmalloc(size_t size)
{

}

void kfree(void *p)
{

}

int kmalloc_free_some_pages()
{

}
