#ifndef MMAP_H
#define MMAP_H

#include <sys/types.h>

/* This file contains typedefs and defines for ARM page tables.  */

#define AP_NOACCESS 0
#define AP_NO_USER 1
#define AP_USER_READ 2
#define AP_ANYONE 3

/* The ARM architecture defines two-level page tables.  A top
   level page table has 4096 entries, where each entry maps
   1 Meg of virtual memory to physical memory.  Each entry has
   a two bit key in the two least significant bits.  The
   key tells how to interpret the remaining 30 bits.  The
   following union of bit fields defines the possible layouts
   for a top level page table entry.
*/
typedef union {
  struct { /* key = 0: unmapped, should generate page fault */
    unsigned key: 2;  /* two bit key defines entry type */
    unsigned IGN: 30;
  }fault; /* key = 0 */
  struct { /* key = 1: Pointer to second level PT */
    unsigned key: 2;  /* two bit key defines entry type */
    unsigned SBZ: 3;  /* these bits Should Be Zero */
    unsigned Domain: 4;  /* Defines the domain of the page (set to 3) */
    unsigned IMP: 1;  /* Implementation defined (set to zero */
    unsigned base_address: 22; /* phys addr of second level PT */
  }CPT; /* key = 1 */
  struct { /* key = 2: a section mapping */
    unsigned key: 2; /* two bit key defines entry type */
    unsigned B: 1;   /* should writes to this page be buffered? */
    unsigned C: 1;   /* should the cache be used for accessing this page? */
    unsigned SBZ_2: 1; /* set to zero */
    unsigned Domain: 4; /* Defines the domain of the page (set to 3) */
    unsigned IMP: 1; /* Implementation defined (set to zero */
    unsigned AP: 2; /* Access permissions use one of AP_ defined above*/
    unsigned TEX: 3; /* Type Extension: set to zero */
    unsigned SBZ: 5; /* Should Be set to Zero */
    unsigned base_address: 12;  /* pointer to first byte of 1M of phys mem */
  }section;
  struct {  /* key = 3: This should never happen. We are not using fine page tables. */
    unsigned key: 2;
    unsigned reserved: 30;
  }err;
}first_level_page_table;

typedef union {
  struct { /* key = 0  unmapped, should generate page fault*/
    unsigned key: 2; /* two bit key defines entry type */
    unsigned IGN: 30;
  }fault;
  struct { /* key = 1 large page mapping.  We are not using this. */
    unsigned key: 2; /* two bit key defines entry type */
    unsigned B: 1;
    unsigned C: 1;
    unsigned AP0: 2;
    unsigned AP1: 2;
    unsigned AP2: 2;
    unsigned AP3: 2;
    unsigned TEX: 3;
    unsigned SBZ: 1;
    unsigned base_address: 16;
  }large_page;
  struct { /* key = 2 small page mapping we are using this. */
    unsigned key: 2; /* two bit key defines entry type */
    unsigned B: 1;   /* should writes to this page be buffered? */
    unsigned C: 1;   /* should the cache be used for accessing this page? */
    unsigned AP0: 2; /* Access permissions use one of AP_ defined above*/
    unsigned AP1: 2; /* should be same as AP0 */
    unsigned AP2: 2; /* should be same as AP0 */
    unsigned AP3: 2; /* should be same as AP0 */
    unsigned base_address: 20; /* pointer to first byte of 4K of phys mem */
  }small_page;
  struct { /* key = 3 mini page mapping.  We are not using this. */
    unsigned key: 2;
    unsigned B: 1;
    unsigned C: 1;
    unsigned AP: 2;
    unsigned TEX: 3;
    unsigned SBZ: 3;
    unsigned base_address: 20;
  }ext_small_page;
}second_level_page_table;

extern first_level_page_table kernel_page_table[0x1000];




#endif
