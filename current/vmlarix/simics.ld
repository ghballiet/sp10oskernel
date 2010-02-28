
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

KERNELRAM_SIZE = 16m;
KERNELHEAP_SIZE = 256m;
MML2PAGESPACE_SIZE = 256m;
MML1PAGESPACE_SIZE = 64m;
CACHEFLUSH_SIZE = 1m;
MINICACHEFLUSH_SIZE = 1m;
KERNELSTACKS_SIZE = 1m - 64k;
INTERRUPT_TABLE_SIZE = 64k;

MEMORY {    /* memory map of SA1110 kernel regions */
    KERNELRAM (rwx) : ORIGIN = 0xc0000000, LENGTH = 16m
    KERNELHEAP (rwx) : ORIGIN = 0xD0000000, LENGTH = 256m
    MML2PAGESPACE (rwx)  : ORIGIN = 0xE0000000, LENGTH = 256m
    MML1PAGESPACE (rwx)  : ORIGIN = 0xF0000000, LENGTH = 64m
    CACHEFLUSH (rwx)  : ORIGIN = 0xFFD00000, LENGTH = 1m
    MINICACHEFLUSH (rwx)  : ORIGIN = 0xFFE00000, LENGTH = 1m
    KERNEL_STACKS (rwx) : ORIGIN = 0xFFF00000, LENGTH = 1m - 64k
    INTERRUPT_TABLE (rwx) : ORIGIN = 0xFFFF0000, LENGTH = 64k
}

/* The sizes of the stacks used by the kernel. */
C_STACK_SIZE   = 160K;
IRQ_STACK_SIZE = 160K;
FIQ_STACK_SIZE = 160K;
SVC_STACK_SIZE = 160K;
ABT_STACK_SIZE = 160K;
UND_STACK_SIZE = 160K;

/* area reserved for the interrupt vector table */
IVT_SIZE = 1024;

SECTIONS {

    .reset : {
        __kernel_ram_start__ = . ;
        *startup.o (.text)  /* startup code (ARM vectors and reset handler) */
        . = ALIGN(0x4);
     } >KERNELRAM

    .text : {
        CREATE_OBJECT_SYMBOLS
        *(.text .text.* .gnu.linkonce.t.*)
        *(.plt)
        *(.gnu.warning)
        *(.glue_7t) *(.glue_7)         /* NOTE: placed already in .fastcode */

        . = ALIGN (4);
        /* These are for static constructors and destructors under ELF */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        *(.rodata .rodata.* .gnu.linkonce.r.*)

        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)

        *(.init)
        *(.fini)

        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >KERNELRAM


    .data : {
        __data_load = LOADADDR (.data);
        __data_start = .;
        KEEP(*(.jcr))
        *(.got.plt) *(.got)
        *(.shdata)
        *(.data .data.* .gnu.linkonce.d.*)
        . = ALIGN (4);
        _edata = .;
    } >KERNELRAM

    .bss : {
        __bss_start__ = . ;
        *(.shbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN (8);
        __bss_end__ = .;
    } >KERNELRAM


    .tmpstack : {
	__tmp_stack_start__ = . ;
        . += 1024;
        . = ALIGN (4);
	__tmp_stack_top__ = . ;
    } > KERNELRAM

    _end = . ;
    __end = . ;
    PROVIDE(end = .);

    . += 1024;
    . = ALIGN (4);
    __kernel_ram_end__ = .;
        

    .stab 0 (NOLOAD) : {
        *(.stab)
    }

    .stabstr 0 (NOLOAD) : {
        *(.stabstr)
    }

    /* DWARF debug sections.
    * Symbols in the DWARF debugging sections are relative to the beginning
    * of the section so we begin them at 0.
    */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
    /DISCARD/ : { *(.note.GNU-stack)  }

.interrupt_table : { . = ALIGN(4); __ivt_start__ = .; } > INTERRUPT_TABLE
__ivt_end__ = __ivt_start__ + IVT_SIZE;

.kernelheap : { . = ALIGN(4); __kernel_heap_start__ = .; } > KERNELHEAP
__kernel_heap_end__ = __kernel_heap_start__ + KERNELHEAP_SIZE;

    /* a 256 meg region for the memory manager to use for second
     * level page table allocation/deallocation */
.mml2pagespace : { . = ALIGN(4); __mml2pagespace_start__ = .; } > MML2PAGESPACE
__mml2pagespace_end__ = __mml2pagespace_start__ + MML2PAGESPACE_SIZE;

    /* a 64 meg region for the memory manager to use for top
     * level page table allocation/deallocation */
.mml1pagespace : { . = ALIGN(4); __mml1pagespace_start__ = .; } > MML1PAGESPACE
__mml1pagespace_end__ = __mml1pagespace_start__ + MML1PAGESPACE_SIZE;

.minicacheflush : { . = ALIGN(4); __minicacheflush_start__ = .; } > MINICACHEFLUSH
__minicacheflush_end__ = __minicacheflush_start__ + MINICACHEFLUSH_SIZE;

.cacheflush : { . = ALIGN(4); __cacheflush_start__ = .; } > CACHEFLUSH
__cacheflush_end__ = __cacheflush_start__ + CACHEFLUSH_SIZE;


.stack : { . = ALIGN(4); __stack_start__ = .; } > KERNEL_STACKS
__irq_stack_top__ = __stack_start__ + IRQ_STACK_SIZE ;
__fiq_stack_top__ = __irq_stack_top__ + FIQ_STACK_SIZE;
__svc_stack_top__ = __fiq_stack_top__ + SVC_STACK_SIZE;   
__abt_stack_top__ = __svc_stack_top__ + ABT_STACK_SIZE;
__und_stack_top__ = __abt_stack_top__ + UND_STACK_SIZE;
__c_stack_top__ = __und_stack_top__ + C_STACK_SIZE;
        
__stack_end__ = __c_stack_top__;

__RAMDISK_LOCATION__ = 0xC0700000;


}

