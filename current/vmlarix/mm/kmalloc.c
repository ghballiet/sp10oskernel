
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


/* Note: we are not currently handling special slabs at all */


/* You may need to define some helper functions here */

/* Fills a slab with a linked list of item_rec objects for the given item size
   (in bytes). Returns the number of item_rec objects created.

   The address of the first item_rec item in the slab will initially be the
   start address of the slab itself.*/
uint32_t populate_slab_records(char *slab, char *slab_end, uint32_t item_size) {
  uint32_t num_items = SLAB_BYTES / item_size;
  unsigned char *current = slab;
  uint32_t i;

  //kprintf("Populating slab with %d item blocks\r\n", num_items);

  for(i=0; i<(num_items-1); i++) {
    ((item_rec*)current)->next = (item_rec*)(current + item_size);
    current = (unsigned char *)((item_rec*)current)->next;
  }

  ((item_rec*)current)->next = NULL;
  return num_items;
}

/* Adds an entry to a slab's available item item_rec linked list at the
   specified address. 

   The address must be within th
e slab, and there must not already be an
   item_rec object allocated at that address. */
void add_slab_item_rec(slab_header *slab, void *address) {
  item_rec *current = slab->avail;
  /* check if there are any items in the available list */
  if(current==NULL) {
    slab->avail = (item_rec*)address;
    slab->avail->next = NULL;
  } else {
    /* find the end of the available item_rec list */
    while(current->next != NULL)
      current = current->next;
    /* append a new entry for the given address */
    current->next = (item_rec*)address;
    current->next->next = NULL;
  }
}

  
void kmalloc_init()
{
  /* Get the memory for the first slab */
  char *slab_start = (char *)slab_create(SLAB_PAGES);
  char *slab_end = slab_start + SLAB_BYTES - 1;

  /* Fill this slab with item_rec objects for the unused sections */
  uint32_t itemsize = sizeof(slab_header);
  populate_slab_records(slab_start, slab_end, itemsize);

  /* Store the first row header as the first item in the first slab */
  slab_row_header *first_row_header = ((slab_row_header *)slab_start);
  first_row_header->itemsize = itemsize;
  first_row_header->next_row = NULL;
  /* Set up the first slab's slab header as the second item in the first slab */
  slab_header *first_slab_header = ((slab_header *)(slab_start + itemsize));
  first_slab_header->itemsize = itemsize;
  first_slab_header->totalitems = SLAB_BYTES / itemsize;
  first_slab_header->freeitems = first_slab_header->totalitems - 2;
  first_slab_header->slab = slab_start;
  first_slab_header->slab_end = slab_end;
  first_slab_header->next_head = NULL;
  /* Set the first available item pointer to the third item */
  first_slab_header->avail = ((item_rec *)(slab_start + (2 * itemsize)));
  /* Give the first row header a pointer to the first slab header */
  first_row_header->first_slab = first_slab_header;

  /* And set the global row header pointer to point to the first row header */
  slabs = first_row_header;
}

/* NOTE: it's conceivable that we could run out of space in the first slab and
     need to create a new one... currently we do not handle that case, and so
     it would fail if we tried to get too many slabs; however, this does not
     seem to be related to our address-storing problem where we can't retrieve
     the 641st addresss since we never run out of free items in the first slab
     for the p04_main tests  */

slab_row_header *new_row(size_t size) {
  /* Create a new row for items of the given size */
  
  //kprintf("Creating new row for size %d\r\n", size);

  /* Place the slab row header for this row in the first available block in the
     first slab */
  slab_row_header *srh = (slab_row_header*)slabs->first_slab->avail;
  /* TODO: as discussed above, this might be NULL if we're out of space... */
  slabs->first_slab->avail = slabs->first_slab->avail->next;
  slabs->first_slab->freeitems--;

  /* Set up the slab row record */
  srh->itemsize = size;
  srh->next_row = NULL;
  srh->first_slab = NULL;

  /* Add a pointer to this row at the end of our slab row list */
  slab_row_header *current = slabs;
  while(current->next_row != NULL) {
    current = current->next_row; /* find the last row entry */
  }
  current->next_row = srh;
  return srh;
}

slab_header *new_slab(slab_row_header *row, size_t size) {
  /* Create a new slab for items of the given size */
  //kprintf("Creating new slab for size %d\r\n", size);

  /* Get the memory for the first slab */
  char *slab_start = (char *)slab_create(SLAB_PAGES);
  char *slab_end = slab_start + SLAB_BYTES; /* TODO: should the -1 be there or not? */

  /* Place the slab header for this row in the first available block in the
     first slab */
  slab_header *sh = (slab_header*)slabs->first_slab->avail;
  if(sh==NULL) {
    kprintf("ERROR: no more space for new slabs; about to crash\n\r");
  } 
  slabs->first_slab->avail = slabs->first_slab->avail->next;
  slabs->first_slab->freeitems--;

  /* Populate the slab with item_rec objects */
  populate_slab_records(slab_start, slab_end, size);

  /* Set up the slab record */
  sh->itemsize = size;
  sh->freeitems = SLAB_BYTES/size; 
  sh->totalitems = sh->freeitems;
  sh->slab = slab_start;
  sh->slab_end = slab_end;
  sh->avail = (item_rec*)slab_start;
  sh->next_head = NULL;
  
  /* Add a pointer to this slab at the end of the row */
  //kprintf("Starting to search for end of row\r\n");
  slab_header *current = row->first_slab;
  /* If this will be the first slab in the row, we don't have to search, but
     instead need to set the row's pointer */
  if(current == NULL) {
    row->first_slab = sh;
  } else {
    while(current->next_head != NULL) {
      current = current->next_head; /* find the last row entry */
    }
    //kprintf("Finished search for end of row\r\n");
    current->next_head = sh;
  }

  return sh;
}

/* Allocates a special slab (i.e. slab larger than SLAB_PAGES*SLAB_BYTES)  */
void *kmalloc_special_slab(size_t size) {
  kprintf("Special slab: %d\r\n",size);
}

void *kmalloc(size_t size)
{
  // check for special slabs
  if(size > SLAB_BYTES) {
    // find or allocate special slab
    kmalloc_special_slab(size);
  }
 
  //kprintf("Kmalloc: Entered kmalloc for size %d\r\n", size);
  /* Find (or allocate, if needed) the row for items of the given size */
  //kprintf("Looking for row.\r\n");
  slab_row_header *current = slabs;
  slab_row_header *srh = NULL;
  while(current->next_row != NULL) {
    if(current->next_row->itemsize == (uint32_t)size) {
      srh = current->next_row;
      break;
    }
    current = current->next_row;
  }
  if(srh == NULL) srh = new_row(size);

  //kprintf("Kmalloc: Looking for slab.\r\n");
  /* Find (or allocate, if needed) a first slab in that row with items remaining */
  slab_header *current_slab = srh->first_slab;
  slab_header *sh = NULL;
  while(current_slab != NULL) {
    if(current_slab->freeitems > 0) {
      sh = current_slab;
      break;
    }
    current_slab = current_slab->next_head;
  }
  if(sh == NULL) sh = new_slab(srh, size);

  //kprintf("Kmalloc: getting the address\r\n");

  /* Get a copy of the 'avail' address from that slab's header -- may as well
     return the very first available block in that slab */
  item_rec *address = sh->avail;
  //kprintf("freeitems count is %d\r\n", sh->freeitems);
  
  /* Set avail = avail-> next in that slab header, update items_remaining */
  sh->avail = sh->avail->next;
  sh->freeitems--;

  //kprintf("Done with kmalloc\r\n");

  /* return the address of the block */
  return (void*)address;
}

void kfree(void *p)
{
  //kprintf("kfree: In kfree\n\r");

  if(p==NULL) kprintf("kfree: trying to free a NULL\r\n");

  /* Find the slab header based upon the slab start and end addresses */
  slab_row_header *current = slabs;
  slab_header *sh = NULL;
  while(current != NULL) {
    slab_header *current_slab = current->first_slab;
    while(current_slab != NULL) {
      if(current_slab->slab <= (char *)p && current_slab->slab_end >= (char *)p) {
        sh = current_slab;
        break;
      }
      current_slab = current_slab->next_head;
    }
    current = current->next_row;
  }
  //kprintf("kfree: found slab header\r\n");
  if(sh==NULL) kprintf("kfree: failed to find slab for address.\r\n");

  /* Call add_slab_item_rec with the avail list pointer from that slab header
     and the given address p */
  add_slab_item_rec(sh, p);
  /* Update items_remaining in the slab header */
  sh->freeitems++;

  //kprintf("kfree: done with kfree\r\n");
}

int kmalloc_free_some_pages()
{
  int freed = 0;
  slab_row_header *last_row = NULL;
  slab_row_header *current_row = slabs;
  while(current_row != NULL) {
    slab_header *last_slab = NULL;
    slab_header *current_slab = current_row->first_slab;
    while(current_slab != NULL) {
      if(current_slab->freeitems == current_slab->totalitems) {
	freed++;
	void *record_address = (void *)current_slab;
	/* remove the slab header from the slab header list */
	//	if(last_slab != NULL) {
	if(current_row->first_slab != current_slab) {
	  last_slab->next_head = current_slab->next_head;
	} else {
	  current_row->first_slab = current_slab->next_head;
	}
	/* destroy the now-unused slab */
	slab_destroy((void *)(current_slab->slab), SLAB_PAGES);
	/* and release the section of the first slab used to store the slab header */
	add_slab_item_rec(slabs->first_slab, record_address);
	slabs->first_slab->freeitems++;
      }
      last_slab = current_slab;
      current_slab = current_slab->next_head;
    }
    if(current_row->first_slab == NULL) {
      /* if this row is now empty, remove the slab row header */
      void *record_address = (void *)current_row;
      if(last_row != NULL) {
	/* we don't want to ever remove the record for the first row -- it
	   should never be empty anyway, but we'll check here anyway... */
	last_row->next_row = current_row->next_row;
	add_slab_item_rec(slabs->first_slab, record_address);
	slabs->first_slab->freeitems++;
      }
    }
    last_row = current_row;
    current_row = current_row->next_row;
  }
  /*kprintf("Free items in first slab after freeing pages: %d\r\n",
            slabs->first_slab->freeitems);*/
  return freed;
}
