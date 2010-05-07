
#include <misc.h>
#include <stddef.h>
#include <sys/types.h>
#include <linker_vars.h>
#include <pt_alloc.h>
#include <page_table.h>
#include <kprintf.h>

/* Functions to initialize the kernel page table are in mmap.c 
   This file provides functions to manipulate user page tables.
*/


/* The following routine adds an entry into the page table pointed to
   by page_table_phys. In order to make a change to the page table, it
   must first be mapped into virtual memory. We have already set up
   the kernel page table to do the mapping. */
void pt_add_mapping(phys_addr page_table_phys, void *virt, uint32_t phys)
{
  void *t;
  uint32_t *ti;                                                              
  phys_addr tmp_phys;
  uint32_t entry;
  first_level_page_table *tlpt;
  second_level_page_table *slpt;

  /* 1. Get virtual address of page table we want to work on */

  tlpt = pt_l1_lookup_virt(page_table_phys);

  /* 2. Now calculate the location of the page table entry for
     the virtual address we want to map. */                   

  entry = ((uint32_t)virt)>>20;  

  /* 3. If the key field on that entry is 2 or 3, then there is an error. */

  if((tlpt[entry].CPT.key==2) || (tlpt[entry].fault.key==3))
    panic("Bad page table entry found.");                   

  /* 4. If the key field is already 1, get phys and virt 
        addr of second level page table and skip to step 7 */

  if(tlpt[entry].CPT.key==1)
    {                       
      tmp_phys = tlpt[entry].CPT.base_address << 10;
      slpt = pt_l2_lookup_virt(tmp_phys);           
    }                                               
  else                                              
    {                                               
      /* 5. Otherwise allocate a second level page table and add its
            entry to the first level page table. */                   

      slpt = pt_l2_alloc();
      tmp_phys = pt_l2_lookup_phys(slpt);

      /* 6. Change key on current entry to 1, and the base address
         for the new second level page table, and set other fields */

      tlpt[entry].CPT.key = 1;
      tlpt[entry].CPT.SBZ = 0;
      tlpt[entry].section.Domain  = 3; /* no domain access checking */
      tlpt[entry].CPT.IMP = 0;                                        
      tlpt[entry].CPT.base_address = tmp_phys>>10;                    
      t = &tlpt[entry];                                               
      ti = (uint32_t *)t;                                             
      SA1110_set_page_table_entry(t, *ti);                            
    }                                                                 

  /* 7. Calculate the offset to the entry we want in the second level
        page table, based on virt */                                 

  entry = (((uint32_t)virt) >> 12)&0xFF;

  /* 8. Set the fields on the entry to make the mapping */
  slpt[entry].small_page.key = 2;                         
  slpt[entry].small_page.base_address = phys>>12;         
  slpt[entry].small_page.AP3 = AP_ANYONE;                 
  slpt[entry].small_page.AP2 = AP_ANYONE;                 
  slpt[entry].small_page.AP1 = AP_ANYONE;                 
  slpt[entry].small_page.AP0 = AP_ANYONE;                 
  slpt[entry].small_page.C = 1;                           
  slpt[entry].small_page.B = 1;                           
  t = &slpt[entry];                                       
  ti = (uint32_t *)t;                                     
  SA1110_set_page_table_entry(t, *ti);                    

}

/* This is a helper function for virt_to_phys */
phys_addr virt_to_phys2(phys_addr page_table_phys,void *virt)
{                                                            
  uint32_t index;                                            
  phys_addr address;                                         
  second_level_page_table *t;                                
                                                             
  /* 1. Map in the page which contains the second level page table
        that we are searching, and get the virtual address for the
        page table.                                               
  */                                                              

  t = pt_l2_lookup_virt(page_table_phys);

  /* 2. Get the mapping corresponding to virt, and return its physical
        address.                                                      
  */                                                                  

  index = ((uint32_t)virt>>12)&0xFF;
  switch (t[index].fault.key)       
    {                               
    case 0: /* no physical mapping */
      return NULL;                   
    case 1: /* large page mapping */ 
      address = t[index].large_page.base_address;
      return address << 16 + ((int)virt & 0x0000FFFF);      
    case 2: /* small page mapping */                        
      address = t[index].small_page.base_address;           
      return (address << 12) | ((int)virt & 0x00000FFF);    
    case 3: /* ext small page not implemented */            
      panic("bad page mapping");                            
    }                                                       
}                                                           


/* virt_to_phys takes the physical address of a first level page table
   and a virtual address, and returns the corresponding physical
   address.  */                                                           
phys_addr virt_to_phys(phys_addr page_table_phys, void *virt)
{                                                            
  uint32_t index;                                            
  phys_addr address;                                         
  first_level_page_table *t;                                   
  phys_addr return_val;                                      

  /* 1. Get virtual address of the first level page table that we want
     to search. */                                                  

  t = pt_l1_lookup_virt(page_table_phys);

  /* 2. Now calculate the location of the page table entry for the
        virtual address we need. */                               

  index = ((uint32_t)virt) >> 20;

  /* 3. Return a value, or proceed to the second level mapping. */

  switch(t[index].fault.key)
    {                       
    case 0: /* no physical mapping */
      return_val = NULL;             
      break;                         
    case 1: /* pointer to second level page table */
      address = t[index].CPT.base_address << 10;    
      return_val = virt_to_phys2(address,virt);     
      break;                                        
    case 2: /* direct section mapping */            
      address = t[index].section.base_address << 20;
      return_val = address | (((uint32_t)virt) & 0x000FFFFF);
      break;                                                 
    case 3: /* error ! */                                    
      panic("bad page mapping");                             
    }                                                        

  return return_val;
}                   


/* This is a helper roution for phys_to_virt.  It takes the physical
   address of a second level page table and a physical address, and
   returns the 8 bits of the virtual address corresponding to the
   second level mapping. */                  
uint32_t phys_to_virt2(phys_addr page_table_phys, phys_addr phys) 
{                                                                 
  uint32_t addr,i;                                                
  phys_addr address;                                              
  phys_addr tmp_phys;                                             
  second_level_page_table *t;                                     

  /* 1. Get virtual address of the second level page table that we want
        to search.  */                                                 

  t = pt_l2_lookup_virt(page_table_phys);

  /* look for a matching small_page mapping */
  tmp_phys = phys>>12;                        
  for(i=0;i<0x100;i++)                        
    if((t[i].small_page.key == 2)&&           
       (t[i].small_page.base_address == tmp_phys) )
      /* found a small page mapping */             
      return i;                                    

  /* look for a matching large_page mapping */
  tmp_phys = phys>>16;                        
  for(i=0;i<0x100;i++)                        
    if((t[i].large_page.key == 1)&&           
       (t[i].large_page.base_address == tmp_phys) )
      /* found a large page mapping */             
      return i;                                    

  /* this is not a value that can be returned if a page is found */
  return 0xFFFFFFFF;                                               
}                                                                  

/* phys_to_virt takes the physical address of a first level page table
   and a physical address, and returns the corresponding virtual
   address. */                                              
void *phys_to_virt(phys_addr page_table_phys, phys_addr phys)
{                                                            
  uint32_t addr,i,found;                                     
  phys_addr address;                                         
  phys_addr tmp_phys;                                        
  first_level_page_table *t;                                   
                                                             
  /* Get virtual address of the first level page table we want to seach */

  t = pt_l1_lookup_virt(page_table_phys);

  /* look for a matching section mapping */

  addr = NULL;
  tmp_phys = phys>>20;

  for(i=0,found=0;(i<0x1000)&&(!found);i++)
    if((t[i].section.key == 2)&&           
       (t[i].section.base_address == tmp_phys) )
      {                                         
        found = 1;                              
        addr = (i << 20) | (phys & 0x000FFFFF); 
      }                                         

  if(!found)
    /* search each individual coarse page table */
    for(i=0,found=0;(i<0x1000)&&(!found);i++)     
      if(t[i].CPT.key == 1)
        {
          address = (phys_addr)t[i].CPT.base_address<<10;
          tmp_phys = phys_to_virt2(address,phys);
          if(tmp_phys != 0xFFFFFFFF) /* found it ! */
            {
              found = 1;
              addr = (i<<20) | (tmp_phys<<12) | (phys & 0x00000FFF);
            }
        }

  return (void *)addr;
}


/* allocate a new page table, copy all of the kernel portion of the
   page table into it, and initialize the private portion of the page
   table */
phys_addr pt_new()
{
  // TODO: debug this method
  kprintf("Entering pt_new...\n\r");
  kprintf("Still working fine here...\n\r");
  phys_addr pt;
  kprintf("phys_addr was fine...\n\r");
  first_level_page_table *vt;
  kprintf("first_level_page_table was fine...\n\r");
  uint32_t i;
  void *t;
  uint32_t *ti;

  vt = pt_l1_alloc();
  kprintf("Got past pt_l1_alloc...\n\r");
  pt = pt_l1_lookup_phys(vt);
  kprintf("Got past pt_l1_lookup_phys(vt)...\n\r");
  kprintf("Initializing all the entries...\n\r");
  /* initialize all the entries */
  for(i=0;i<0x800;i++)
    {
      vt[i].fault.key = 0;
      t = &vt[i];
      ti = (uint32_t *)t;
      SA1110_set_page_table_entry(t, *ti);
    }

  for(i=0x800 ; i<0x1000;i++)
    {
      vt[i] = kernel_page_table[i];
      t = &vt[i];
      ti = (uint32_t *)t;
      SA1110_set_page_table_entry(t, *ti);
    }

  return pt;
}

/* pt_l2_delete() is a helper function for pt_delete it
   deletes a second level page table and releases any physical
   memory referenced by it */
void pt_l2_delete(phys_addr page_table_phys)
{
}

/* Release a page table and all physical memory referenced by it. */
void pt_delete(phys_addr page_table_phys)
{
  first_level_page_table *t;                                   
  /* 1. Get the virtual address of the page table */
  t = pt_l1_lookup_virt(page_table_phys);

}
