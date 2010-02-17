
/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */

    .equ    I_BIT,          0x80      /* when I bit is set, IRQ is disabled */
    .equ    F_BIT,          0x40      /* when F bit is set, FIQ is disabled */

    .equ    USR_MODE,       0x10
    .equ    FIQ_MODE,       0x11
    .equ    IRQ_MODE,       0x12
    .equ    SVC_MODE,       0x13
    .equ    ABT_MODE,       0x17
    .equ    UND_MODE,       0x1B
    .equ    SYS_MODE,       0x1F

/* constant to pre-fill the stack */
    .equ    STACK_FILL,     0xAAAAAAAA

/****************************************************************************
  The startup code should be loaded at the beginning of RAM, which
  is 0xc0000000 on the SA1110.  The entry point is _start, which
  should end up at address 0xc0000000
**/
  
    .section .text
    .code 32

    .global _start
    .func   _start


_start:
    B       _reset          /* jump over the copyright notice */

    /* The following embeds a copyright notice in your kernel code */
    .string "Copyright (c) 2008 Larry D. Pyeatt. All Rights Reserved."
    .align 4                               /* re-align to the word boundary */


/*****************************************************************************
* _reset - performs initialization of the hardware.  Calls a C routine to
*          finish setting things up.
*/
	
_reset:

    /* Call the platform-specific low-level initialization routine
    *  SA1110_low_level_init()
    */
	
    LDR     sp,=__tmp_stack_top__ /* set the temporary stack pointer */

    /* now we prepare for executing the low-level C initialization routine */
    /* Clear the .bss segment (zero init) */
    LDR     r1,=__bss_start__
    LDR     r2,=__bss_end__
    MOV     r3,#0
1:
    CMP     r1,r2
    STMLTIA r1!,{r3}
    BLT     1b

    LDR     r0,=_reset      /* pass the reset address as the 1st argument */
    LDR     r1,=_cstartup   /* pass the return address as the 2nd argument */
    MOV     lr,r1           /* set the return address after the remap */
    LDR     r12,=SA1110_low_level_init
    BX	    r12    /* relative branch enables remap when we
    					enable the MMU */
_cstartup:			
    /* Initialize stack pointers for all ARM modes */
    MSR     CPSR_c,#(IRQ_MODE | I_BIT | F_BIT)
    LDR     sp,=__irq_stack_top__            /* set the IRQ stack pointer */

    MSR     CPSR_c,#(FIQ_MODE | I_BIT | F_BIT)
    LDR     sp,=__fiq_stack_top__            /* set the FIQ stack pointer */

    MSR     CPSR_c,#(SVC_MODE | I_BIT | F_BIT)
    LDR     sp,=__svc_stack_top__            /* set the SVC stack pointer */

    MSR     CPSR_c,#(ABT_MODE | I_BIT | F_BIT)
    LDR     sp,=__abt_stack_top__            /* set the ABT stack pointer */

    MSR     CPSR_c,#(UND_MODE | I_BIT | F_BIT)
    LDR     sp,=__und_stack_top__            /* set the UND stack pointer */

    MSR     CPSR_c,#(SYS_MODE | I_BIT | F_BIT)
    LDR     sp,=__c_stack_top__              /* set the C stack pointer */

    LDR     sp,=__tmp_stack_top__ /* set the temporary stack pointer, we'll
	delete this line once we have the MMU set up and have real stacks. */

    SUB     r11,sp,#16    /* set the frame pointer */
    
    /* Invoke the static constructors */
    /* We may need this later, but not now (we are not linking any libc) */
    /* LDR     r12,=__libc_init_array */
    /* MOV     lr,pc               */ /* set the return address */
    /* BX      r12                 */ /* the target code can be ARM or THUMB */


    /* Enter the C/C++ code at main */
    LDR     r12,=main
    MOV     lr,pc           /* set the return address */
    BX      r12             /* the target code can be ARM or THUMB */

    SWI     0xFFFFFF        /* cause exception if main() ever returns */

    .size   _start, . - _start
    .endfunc

    .end
