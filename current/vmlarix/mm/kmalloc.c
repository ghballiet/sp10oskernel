
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

/* allocate an item from the given slab */
void *slab_alloc_item(slab_header *s)
{
  item_rec *item;
  if(s->freeitems == 0)
    return NULL;
  item = s->avail;
  s->avail = s->avail->next;
  s->freeitems--;
  return (void *) item;
}

/* return an item to a slab.  Returns 1 if the item belongs
   to the slab.  Returns 0 if the item does not belong to
   the slab. */
int slab_free_item(slab_header *slab_head, item_rec *item)
{
  int val;
  if((item >= (item_rec *)slab_head->slab)&&
     (item < (item_rec *)slab_head->slab_end))
    {
      val = 1;
      slab_head->freeitems++;
      item->next = slab_head->avail;
      slab_head->avail = item;
      
    }
  else
    {
      val = 0;
    }
  return val;
}

/* returns number of items in the list */
int32_t slab_make_linked_list(void *slab_data,uint32_t itemsize)
{
  uint32_t i,j;
  item_rec* curr_record = slab_data;

  for(i=0,j=0;i < (SLAB_BYTES-itemsize);i += itemsize,j++)
    {
      curr_record->next = (((void *)curr_record) + itemsize);
      curr_record = curr_record->next;
    }
  curr_record->next = NULL;
  return j;
}

/* kmalloc_slab_new creates a slab of items, where each item is "itemsize"
   bytes */
slab_header *kmalloc_slab_new(uint32_t itemsize)
{
  slab_header *header;
  item_rec *linked_list;
  uint32_t used;
  char *slab_data;
  uint32_t header_size = sizeof(slab_header);
  int32_t items;

  slab_data = slab_create(SLAB_PAGES);

  linked_list = (item_rec *)slab_data;
  items =  slab_make_linked_list(slab_data,itemsize);

  /* Get a slab header.  We may need to get the header
     from the slab that we just now created. */
  if((header_size <= itemsize)&&(header_size>(itemsize>>1)))
    {
      header=(slab_header *)linked_list;
      linked_list = linked_list->next;
      used = 1;
    }
  else
    {
      header=(slab_header *)kmalloc(sizeof(slab_header));
      used = 0;
    }

  header->itemsize = itemsize;
  header->totalitems = items;
  header->freeitems = header->totalitems - used;
  header->slab = slab_data;
  header->slab_end = slab_data + SLAB_BYTES;
  header->avail = linked_list;
  header->next_head = NULL;

  return header;
}

void kmalloc_slab_delete(slab_header *h)
{
  slab_destroy(h->slab,SLAB_PAGES);
  kfree(h);
}

  
void kmalloc_init()
{
  int i;
  uint32_t itemsize;
  slab_header *tmpslab;

  /* Create a slab for allocating row headers and slab headers. */
  /* Round itemsize to next highest power of two. */
  if(sizeof(slab_header)>sizeof(slab_row_header))
    for(itemsize=1; itemsize < sizeof(slab_header);itemsize=itemsize<<1);
  else
    for(itemsize=1; itemsize < sizeof(slab_row_header);itemsize=itemsize<<1);

  tmpslab = kmalloc_slab_new(itemsize);

  slab_row_header *firstrow,*nextrow;
  nextrow = NULL;

  for(i=(SLAB_PAGES<<12);i>=4;i>>=1)
    {
      firstrow = (slab_row_header*)slab_alloc_item(tmpslab);
      firstrow->next_row = nextrow;
      if(i==itemsize)
	firstrow->first_slab = tmpslab;
      else
	firstrow->first_slab = NULL;
      firstrow->itemsize = i;
      nextrow = firstrow;
    }
  slabs = firstrow;
}


void *kmalloc(size_t size)
{
  special_slab *s;
  slab_row_header *curr;
  void *tmp;
  int pages_needed;
  slab_header *h;

  curr = slabs;
  while((curr != NULL)&&(curr->itemsize<size))
    curr=curr->next_row;
  if(curr == NULL)
    {
      /* large block, do special allocation */
      pages_needed = size >> 12;
      if((pages_needed << 12)<size)
	pages_needed++;
      /* map in the number of pages needed, get pointer to location */
      tmp = slab_create(pages_needed);
      /* if we got what we needed, make a record of it and store in on
	 the linked list of special slabs */
      if(tmp != NULL)
	{
	  s = kmalloc(sizeof(special_slab));
	  s->pages = pages_needed;
	  s->data = tmp;
	  s->next = special_slabs;
	  special_slabs = s;
	}
      return tmp;
    }
  
  h = curr->first_slab;
  while((h!=NULL)&&(h->freeitems==0))
    h = h->next_head;

  if(h==NULL)
    {
      /* add a slab to the row */
      h = kmalloc_slab_new(curr->itemsize);
      h->next_head = curr->first_slab;
      curr->first_slab = h;
    }
  return slab_alloc_item(h);
}

void special_delete(special_slab *s)
{
  slab_destroy(s->data,s->pages);
  kfree(s);
}

void kfree(void *p)
{
  slab_row_header *curr = slabs;
  slab_header *h;
  int done = 0;
  special_slab *s,*lasts;

  /* search each row, looking for the slab that this item belongs to */
  while((curr != NULL)&&!done)
    {
      h = curr->first_slab;

      /* check each slab in the current row, to see if it accepts the item */
      while((h != NULL)&&(!done))
	{
	  if(slab_free_item(h,p))
	    done = 1;
	  else
	    h = h->next_head;
	}
      if(!done)
	curr=curr->next_row;
    }
  if(!done)
    {
      /* check to see if it is a specially allocated large block */
      if(special_slabs == NULL)
	panic("attempt to free a bogus pointer");
      else
	{
	  if(p==special_slabs->data)
	    {
	      s = special_slabs;
	      special_slabs = special_slabs->next;
	      special_delete(s);
	    }
	  else
	    {
	      lasts = special_slabs;
	      s = special_slabs->next;
	      while((s != NULL)&&(p!=s->data))
		{
		  lasts = s;
		  s=s->next;
		}
	      if(s==NULL)
		panic("attempt to free a bogus pointer");
	      else
		{
		  lasts->next = s->next;
		  special_delete(s);
		}
	    }
	}
    }
}


  /* Tries to release pages of phys mem by releasing any slabs that have
     all of their items unallocated.  Returns number of pages released. */
int kmalloc_free_some_pages()
{
  slab_header *h,*lasthead;
  slab_row_header *curr = slabs;
  int pages=0;

  /* search each row, looking for a slab that has all its items */
  while(curr != NULL)
    {
      /* check each slab in the current row, to see if it has zero
	 items allocated */

      /* First, pull slabs off of the head of the list, as many times
	 as possible. */
      while((curr->first_slab != NULL)&&
	    (curr->first_slab->totalitems==curr->first_slab->freeitems))
	{
	  h = curr->first_slab;
	  curr->first_slab = curr->first_slab->next_head;
	  kmalloc_slab_delete(h);
	  pages += SLAB_PAGES;
	}

      /* Then go all the way through the remainder of the list,
	 looking for other slabs to release. */
      if(curr->first_slab!=NULL)
	{
	  lasthead = curr->first_slab;
	  h = curr->first_slab->next_head;
	  while(h != NULL)
	    {
	      if(h->totalitems == h->freeitems)
		{
		  lasthead->next_head = h->next_head;
		  kmalloc_slab_delete(h);
		  h = lasthead->next_head;
		  pages += SLAB_PAGES;
		}
	      else
		{
		  lasthead = h;
		  h = h->next_head;
		}
	    }
	}
      curr=curr->next_row;
    }

  return pages;
}
