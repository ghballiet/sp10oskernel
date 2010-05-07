
#include <stddef.h>
#include <sys/types.h>
#include <pt_alloc.h>
#include <linker_vars.h>
#include <phys_mem.h>
#include <kprintf.h>
#include <misc.h>

/* Since a second level page table occupies 1024 bytes, and our page
   size is 4096 bytes, we can pack 4 second level page tables in a
   page. This saves memory. However, this makes it more difficult to
   operate on them, because we can only map in a complete page, and
   then operate on one of four page tables.  We set up a simple slab
   allocator to manage allocating and freeing second level page
   tables.  The slabs will occupy 256 Meg of VIRTUAL memory space 
   in kernel virtual memory. 

   That leaves room for 256K second level page tables.  Since
   processes have 2Gb of virtual memory, no process can have more than
   2048 second level page tables. This will support 128 concurrent
   processes if ALL of them are using ALL of their virtual address
   space (which is VERY unlikely).  It also provides multiple
   second-level page table entries for every possible physical address
   (to support shared memory), and best of all, the kernel can
   maintain this mapping with 32 static second level page tables (32K
   bytes).  (Note: if an address to phys mem is greater than 32 bits,
   then the space available to store page tables should be increased
   to make sure that each physical page can appear in at least two
   page tables.)  That's just a rule-of-thumb.
 */

/* Declare 256 second level page tables (65536 entries) to map up to
   262144 user-process second level page tables into kernel virtual
   memory. Align on page boundary to make addresses simpler to
   compute. */

#define NUM_LEV_2_PTS 65536

second_level_page_table __attribute__ ((aligned (4096))) lev_2_pts[NUM_LEV_2_PTS];

/* Each top level page table occupies 4 pages of memory.  In order to
   support 1024 processes, we would need 1024 page tables.  That means
   we need to be able to map 16 Meg of memory to hold top level page
   tables, so we need 16 second level page tables to do the mapping.
   Top level page tables will be mapped in a 64 Meg space.  That is
   enough space to support 4096 processes.
*/

#define NUM_LEV_1_PTS 16384

second_level_page_table __attribute__ ((aligned (4096))) lev_1_pts[NUM_LEV_1_PTS];


/* Define a 1024 byte object that makes a linked list to hold free
   second level page tables. */
typedef struct avail_l2_pt{
  struct avail_l2_pt *next;
  char filler[1024-sizeof(struct avail_l2_pt *)];
}fr_l2_pt;

/* Global pointer for linked list of 1024 byte objects */
fr_l2_pt *avail_l2_pts = NULL; 


/* Define a 16K byte object that makes a linked list to hold free
   top level page tables. */
typedef struct avail_l1_pt{
  struct avail_l1_pt *next;
  char filler[16384-sizeof(struct avail_l1_pt *)];
}fr_l1_pt;

/* Global pointer for linked list of 1024 byte objects */
fr_l1_pt *avail_l1_pts = NULL; 


/* must be called to set up page tables for the slab allocators */
void pt_alloc_init()
{
  void *t;
  uint32_t *ti;
  second_level_page_table * l2_virt_ad;
  first_level_page_table * l1_virt_ad;
  uint32_t i;
  uint32_t entry,last_entry;
  phys_addr phys;



  /* we have some static arrays (defined above) of second level
     page table entries that we will use to map other page tables.
     We need to initialize them and add them to the kernel page table */

  /* Set up for managing second level page tables */
  phys = (phys_addr) &lev_2_pts;
  l2_virt_ad = (second_level_page_table*) &__mml2pagespace_start__;

  /* initialize the page tables */
  for(i=0;i<NUM_LEV_2_PTS;i++)
      lev_2_pts[i].fault.key = 0;

  /* find offset into the kernel page tables */
  entry = ((uint32_t)l2_virt_ad) >> 20;
  last_entry = entry + (NUM_LEV_2_PTS >> 8);

  while(entry < last_entry)
    {
      kernel_page_table[entry].CPT.key = 1;
      kernel_page_table[entry].CPT.base_address = (phys>>10);
      kernel_page_table[entry].CPT.IMP = 0;
      kernel_page_table[entry].CPT.Domain = 3;
      kernel_page_table[entry].CPT.SBZ = 0;
      t = &kernel_page_table[entry];
      ti = (uint32_t *)t;
      SA1110_set_page_table_entry(t, *ti);
      entry++;
      phys += (256 * sizeof(second_level_page_table));
    }


  /* Set up for managing top level page tables */

  phys = (phys_addr) &lev_1_pts;
  l1_virt_ad = (first_level_page_table*) &__mml1pagespace_start__;

  /* initialize the page tables */
  for(i=0;i<NUM_LEV_1_PTS;i++)
      lev_1_pts[i].fault.key = 0;

  /* find offset into the kernel page tables */
  entry = ((uint32_t)l1_virt_ad) >> 20;
  last_entry = entry + (NUM_LEV_1_PTS >> 8);

  while(entry < last_entry)
    {
      kernel_page_table[entry].CPT.key = 1;
      kernel_page_table[entry].CPT.base_address = (phys>>10);
      kernel_page_table[entry].CPT.IMP = 0;
      kernel_page_table[entry].CPT.Domain = 3;
      kernel_page_table[entry].CPT.SBZ = 0;
      t = &kernel_page_table[entry];
      ti = (uint32_t *)t;
      SA1110_set_page_table_entry(t, *ti);
      entry++;
      phys += (256 * sizeof(second_level_page_table));
    }

}

/* fast phys_to_virt for level two page tables */
second_level_page_table *pt_l2_lookup_virt(phys_addr pt)
{
  int i = 0;
  int found = 0;
  void *virt_ad;
  do
    {
      if((lev_2_pts[i].small_page.key == 2)&&
         (lev_2_pts[i].small_page.base_address == (pt>>12)))
        found = 1;
      else
        i++;
    }
  while((i<NUM_LEV_2_PTS)&&(!found));

  if(!found)
    panic("Unable to find second level page table!");
  
  virt_ad = (void *)(((uint32_t)&__mml2pagespace_start__) + (i << 12));
  virt_ad += (pt & (0x03<<10));
  return virt_ad;
}

/* fast phys_to_virt for level one page tables */
first_level_page_table *pt_l1_lookup_virt(phys_addr pt)
{
  int i = 0;
  int found = 0;
  void *virt_ad;
  do
    {
      if((lev_1_pts[i].small_page.key == 2)&&
         (lev_1_pts[i].small_page.base_address == (pt>>12)))
        found = 1;
      else
        i++;
    }
  while((i<NUM_LEV_1_PTS)&&(!found));

  if(!found)
    panic("*Unable to find top level page table!");
  
  virt_ad = (void *)(((uint32_t)&__mml1pagespace_start__) + (i << 12));
  return virt_ad;
}


/* fast virt_to_phys for level two page tables */
phys_addr pt_l2_lookup_phys(second_level_page_table *virt)
{
  uint32_t v;
  int index;
  phys_addr p;

  /* find offset of virt from start of page management space */
  v = (uint32_t)virt - (uint32_t)&__mml2pagespace_start__;
  /* page table entry is that offset divided by 4096 */
  index = v>>12;
  p = lev_2_pts[index].small_page.base_address << 12;
  /* add offset within the page */
  p += v & (0x3<<10);
  return p;
}

/* fast virt_to_phys for level one page tables */
phys_addr pt_l1_lookup_phys(first_level_page_table *virt)
{
  uint32_t v;
  int index;
  phys_addr p;

  /* find offset of virt from start of page management space */
  v = (uint32_t)virt - (uint32_t)&__mml1pagespace_start__;
  /* page table entry is that offset divided by 4096 */
  index = v>>12;
  p = lev_1_pts[index].small_page.base_address << 12;
  return p;
}

/* Allocate and return the physical address of a new second level page
   table.  */
second_level_page_table *pt_l2_alloc()
{
  void *t;
  uint32_t *ti;
  fr_l2_pt* retval;
  void *virt_ad;
  phys_addr phys_ad;
  uint32_t i;
  if(avail_l2_pts != NULL)
    {
      retval = avail_l2_pts;
      avail_l2_pts = avail_l2_pts->next;
    }
  else
    {
      /* find an available virtual address between __mml2pagespace_start__
         and __mml2pagespace_end__ for the new block of page tables */
      for(i=0;(i<NUM_LEV_2_PTS)&&(lev_2_pts[i].fault.key!=0);i++);
      if(i==NUM_LEV_2_PTS)
        panic("Out of second level page table space.");

      virt_ad = (void *)(((uint32_t)&__mml2pagespace_start__) + (i << 12));
      retval = virt_ad;

      /* get a page of phys mem */
      phys_ad = phys_mem_get_pages(1,1);


      /* panic if we are out of phys mem. */
      if(phys_ad == NULL)
        panic("unable to allocate a physical page.");

      /* map the page */
      lev_2_pts[i].small_page.key=2;
      lev_2_pts[i].small_page.base_address =  phys_ad >> 12;
      lev_2_pts[i].small_page.AP3 = AP_NO_USER;
      lev_2_pts[i].small_page.AP2 = AP_NO_USER;
      lev_2_pts[i].small_page.AP1 = AP_NO_USER;
      lev_2_pts[i].small_page.AP0 = AP_NO_USER;
      lev_2_pts[i].small_page.C = 1;
      lev_2_pts[i].small_page.B = 1;
      t = &lev_2_pts[i];
      ti = (uint32_t *)t;
      SA1110_set_page_table_entry(t, *ti);

      /* The page contains four page tables, 1024 bytes each.  When
         not in use, we store them on a linked list. We have just
         allocated a page of phys mem and will return a pointer to the
         first page table in that page.  Now we are going to create a
         linked list out of the remaning three page tables.
      */
      avail_l2_pts = retval + 1;

      for(i=0;i<2;i++)
        {
          avail_l2_pts->next = avail_l2_pts+1;
          avail_l2_pts = avail_l2_pts->next;
        }
      avail_l2_pts->next = NULL;
      avail_l2_pts = retval + 1;
    }

  /*   serial_write(0,"address: ",sizeof("address: ")); */
  /*   print_addr(retval); */
  /* now get the address of the first page table in the page */
  ti = (uint32_t *)retval;
  for(i=0;i<256;i++)
    ti[i] = 0;
  
  return (second_level_page_table *)retval;
}

/* Put a second level page table back on the linked list. Keep virtual
   addresses sorted in ascending order, so that we can free a page when
   all four page tables in it have been freed.
*/
void pt_l2_free(phys_addr pt)
{

  /* look up its virtual address */
  fr_l2_pt* ll_pt = (fr_l2_pt*)pt_l2_lookup_virt(pt);

  fr_l2_pt* trail = avail_l2_pts;
  fr_l2_pt* lead = avail_l2_pts->next;
  if(ll_pt<avail_l2_pts)
    {
      ll_pt->next = avail_l2_pts;
      avail_l2_pts=ll_pt;
    }
  else
    {
      while((lead != NULL)&&(ll_pt<lead))
        {
          trail = lead;
          lead = lead->next;
        }
      trail->next = ll_pt;
      ll_pt->next = lead;
    }
}


/* Allocate and return the physical address of a new top level page
   table.  */
first_level_page_table *pt_l1_alloc()
{
  kprintf("Inside of pt_l1_alloc\n\r");
  void *t;
  uint32_t *ti;
  fr_l1_pt *retval;
  void *virt_ad;
  phys_addr phys_ad;
  uint32_t i,j;
  if(avail_l1_pts != NULL)
    {
      retval = avail_l1_pts;
      avail_l1_pts = avail_l1_pts->next;
      kprintf("avail_l1_pts != NULL\n\r");
    }
  else
    {
      /* find an available virtual address between __mml1pagespace_start__
         and __mml1pagespace_end__ for the new page table */
      for(i=0;(i<NUM_LEV_1_PTS)&&(lev_1_pts[i].fault.key!=0);i+=4);
      kprintf("Got past (possibly infinite) loop.\n\r");
      if(i==NUM_LEV_1_PTS)
        panic("Out of top level page table space.");

      virt_ad = (void *)(((uint32_t)&__mml1pagespace_start__) + (i << 12));
      retval = virt_ad;

      /* get four pages of phys mem, aligned on 16K boundary */
      phys_ad = phys_mem_get_pages(4,4);

      /* panic if we are out of phys mem. */
      if(phys_ad == NULL)
        panic("unable to allocate a physical page.");

      kprintf("Mapping the pages...\n\r");
      /* map the pages */
      for(j=i;j<i+4;j++)
        {
          lev_1_pts[j].small_page.key=2;
          lev_1_pts[j].small_page.base_address =  (phys_ad >> 12) + (j-i);
          lev_1_pts[j].small_page.AP3 = AP_NO_USER;
          lev_1_pts[j].small_page.AP2 = AP_NO_USER;
          lev_1_pts[j].small_page.AP1 = AP_NO_USER;
          lev_1_pts[j].small_page.AP0 = AP_NO_USER;
          lev_1_pts[j].small_page.C = 1;
          lev_1_pts[j].small_page.B = 1;
          t = &lev_1_pts[j];
          ti = (uint32_t *)t;
          SA1110_set_page_table_entry(t, *ti);
        }
        kprintf("Done mapping the pages.\n\r");
    }

  /* initialize the entries */
  kprintf("Initializing entries from %d...\n\r",i);
  // TODO: wtf is happening with ti here?
  ti = (uint32_t *)retval;
  for(i=0;i<4096;i++) {
    ti[i] = 0;
    kprintf("%d/4096\n\r",i);
  }
  kprintf("Done initializing entries.\n\r");
  return (first_level_page_table *)retval;
}

void pt_l1_free(phys_addr pt)
{
  fr_l1_pt* ll_pt = (fr_l1_pt*)pt_l1_lookup_virt(pt);
  ll_pt->next = avail_l1_pts;
  avail_l1_pts = ll_pt;
}



/* if we run out of free pages of phys mem, we can call this routine
   to try to get a page from the memory manager's data structures. It
   returns the number of pages released */
int pt_release_unused_pagetable_memory()
{
  /* not implemented yet */

  /* free all unused top level page tables on the linked list avil_l1_pts */

  /* scan the linked list (avail_l2_pts), looking for four virtual
     addresses which all reside on the same page. If you find an
     occurrence, unlink all four addresses and release the page */

}

