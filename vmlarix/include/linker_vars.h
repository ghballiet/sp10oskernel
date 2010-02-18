

#ifndef LINKER_VARS_H
#define LINKER_VARS_H
//#include <mmap.h> 

extern phys_mem_t __kernel_ram_start__[];
extern phys_mem_t __kernel_ram_end__[];

/* memory region reserved for memory manager to map top level page tables */
extern second_level_page_table __mml1pagespace_start__[];
extern second_level_page_table __mml1pagespace_end__[];

/* memory region reserved for memory manager to map second level page tables */
extern second_level_page_table __mml2pagespace_start__[];
extern second_level_page_table __mml2pagespace_end__[];

/* location of the interrupt vector table */
extern char __ivt_start__ [];

/* starting locations of the kernel stacks (different processor modes) */
extern char __stack_start__[];
extern char __irq_stack_top__[];
extern char __fiq_stack_top__[];
extern char __svc_stack_top__[];
extern char __abt_stack_top__[];
extern char __und_stack_top__[];
extern char __c_stack_top__[];
extern char __stack_end__[];


/* a temporary stack to use before the MMU gets started */
extern char __tmp_stack_start__[];
extern char __tmp_stack_top__[];

/* start and end of region that should be set to zero early in the
   initialization process */
extern char __bss_start__[];
extern char __bss_end__[];

/* the bottom and top of the kernel heap virtual space */
extern char __kernel_heap_start__[];
extern char __kernel_heap_end__[];

/* a 1 Meg section for the kernel to use for its own 2nd level
   page tables for mapping the kernel heap */
extern second_level_page_table __kmallocl2pagespace_start__[];
extern second_level_page_table __kmallocl2pagespace_end__[];


/* a 1 Meg section used for flushing the minicache */
extern char __minicacheflush_start__[];
extern char __minicacheflush_end__[];


/* a 1 Meg section used for flushing the cache */
extern char __cacheflush_start__[];
extern char __cacheflush_end__[];

/* location of binary images for taska and taskb */
extern char _binary_taska_bin_end[];
extern char _binary_taska_bin_start[];
extern char _binary_taska_bin_size[];

extern char _binary_taskb_bin_end[];
extern char _binary_taskb_bin_start[];
extern char _binary_taskb_bin_size[];


extern char __RAMDISK_LOCATION__[];


#endif





