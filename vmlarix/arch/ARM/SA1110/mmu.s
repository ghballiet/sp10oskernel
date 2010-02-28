	
	.text
	.code 32

/* phys_mem_t get_page_table() */
	.global	get_page_table
	.func	get_page_table
get_page_table:	
	mrc	p15, 0, r0, c2, c0, 0
	  /*   mcr p15, 0, r0, c2, c0, 1 */
	mov	pc,lr
	.size   get_page_table, . - get_page_table
	.endfunc


/* void enable_mmu() */
	.global	enable_mmu
	.func	enable_mmu
enable_mmu:
	mov	r0,#0
	add	r0,r0,#-1
	mcr	p15, 0, r0, c3, c0, 0
	MCR	p15, 0, r0, c8, c7, 0
	mrc	p15, 0, r0, c1, c0
	bic	r0, r0, #12
	orr	r0, r0, #1
	mcr	p15, 0, r0, c1, c0, 0
	mov	r3,r3
	mov	r3,r3
	mov	r3,r3
	mov	pc,lr
	.size   enable_mmu, . - enable_mmu
	.endfunc

/* void disable_mmu() */
	.global	disable_mmu
	.func	disable_mmu
disable_mmu:	
	mov	r0,#-1
	mcr	p15, 0, r0, c3, c0, 0
	MCR	p15, 0, r0, c8, c7, 0
	mrc	p15, 0, r0, c1, c0
	bic	r0, r0, #1
	mcr	p15, 0, r0, c1, c0, 0
	mov	r3,r3
	mov	r3,r3
	mov	r3,r3
	mov	pc,lr
	.size   disable_mmu, . - disable_mmu
	.endfunc

/* void set_page_table_entry(void *entry,uint32_t value) */
	.global	set_page_table_entry
	.func	set_page_table_entry
set_page_table_entry:	
	str	lr, [sp, #-4]!          /* save link reg on stack */
	str	r1, [r0]                /* equivalent C: *entry = value */
	mov	r0, r0   /* NOP delay to let prev instruction complete */
	bl	SA1110_flush_cache_all  /* clears IP */
	mcr	p15, 0, r0, c7, c10, 1  /* flush clean cache entry */
	mcr	p15, 0, r0, c7, c10, 4  /* drain the write buffer */
	ldr	lr, [sp], #4            /* load link reg from stack */
	mov	pc, lr                  /* return from subroutine */
	.size   set_page_table_entry, . - set_page_table_entry
	.endfunc

    .equ    NO_IRQ,         0x80                     /* mask to disable IRQ */
    .equ    NO_FIQ,         0x40                     /* mask to disable FIQ */
    .equ    NO_INT,         (NO_IRQ | NO_FIQ) /*mask to disable IRQ and FIQ */
    .equ    FIQ_MODE,       0x11
    .equ    IRQ_MODE,       0x12
    .equ    SYS_MODE,       0x1F
    .equ    SUP_MODE,       0x13

/* void set_page_table(phys_mem_t page_table) */
	.global	set_page_table
	.func	set_page_table
set_page_table:
	/* We are about to change the page table.  Pointer to new page
	   table is in r0.  Better make sure we are not using the user
	   stack, and turn off interrupts. */
	MRS     r1,cpsr             /* get the original CPSR in r1 */
	MSR     cpsr,#(SUP_MODE | NO_INT) /* disable both IRQ/FIQ */
	str     r1,[sp, #-4]!              /* save CPSR on stack */
	str	lr, [sp, #-4]!             /* save link reg on stack */
	bl	SA1110_flush_cache_all     /* clears IP */
	mcr	p15, 0, ip, c9, c0, 0      /* invalidate Read Buffer  */
	mcr	p15, 0, r0, c2, c0, 0      /* load page table pointer */
	mcr	p15, 0, ip, c8, c7, 0      /* invalidate all TLB entries */
	/* The MMU will do a page table lookup before the next instruction
	   gets executed. This subroutine better have the same mapping
	   in the current PT as it did in the old one, or the next instruction
	   won't get executed, and there is no telling where we will end up. */
	ldr	lr, [sp], #4      /* load link register from stack */
	ldr	r1, [sp], #4	  /* load cpsr from stack */
	MSR     cpsr,r1           /* restore the original CPSR from r1 */
	mov	pc,lr             /* return from subroutine */
	.size   set_page_table, . - set_page_table
	.endfunc

/* void set_initial_page_table(phys_mem_t page_table) */
	.global	set_initial_page_table
	.func	set_initial_page_table
set_initial_page_table:	
	mcr	p15, 0, r0, c2, c0, 0      /* load page table pointer */
	mcr	p15, 0, ip, c8, c7, 0      /* invalidate all TLB entries */
	mov	pc, lr                     /* return from subroutine */
	.size  set_initial_page_table, . - set_initial_page_table
	.endfunc




/* void SA1110_set_page_table_entry(void *entry,uint32_t value) */
        .global SA1110_set_page_table_entry
        .func   SA1110_set_page_table_entry
SA1110_set_page_table_entry:
        str     lr, [sp, #-4]!
        str     r1, [r0]
        mov     r0, r0   /* delay to let prev instruction complete */
        bl      SA1110_flush_cache_all     /* clears IP */
        mcr     p15, 0, r0, c7, c10, 1  /* flush clean cache entry */
        mcr     p15, 0, r0, c7, c10, 4  /* drain the write buffer */
        ldr     lr, [sp], #4
        mov     pc, lr 
        .size   SA1110_set_page_table_entry, . - SA1110_set_page_table_entry
        .endfunc


	
/*   SET UP THE STACKS */
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


    .global setup_stacks
    .func   setup_stacks

setup_stacks:
    /* We should currently be in SVC mode */
    /* Initialize stack pointers for all ARM modes */
    MSR     CPSR_c,#(IRQ_MODE | I_BIT | F_BIT)
    LDR     sp,=__irq_stack_top__            /* set the IRQ stack pointer */

    MSR     CPSR_c,#(FIQ_MODE | I_BIT | F_BIT)
    LDR     sp,=__fiq_stack_top__            /* set the FIQ stack pointer */

    MSR     CPSR_c,#(ABT_MODE | I_BIT | F_BIT)
    LDR     sp,=__abt_stack_top__            /* set the ABT stack pointer */

    MSR     CPSR_c,#(UND_MODE | I_BIT | F_BIT)
    LDR     sp,=__und_stack_top__            /* set the UND stack pointer */

    MSR     CPSR_c,#(SYS_MODE | I_BIT | F_BIT)
    LDR     sp,=__c_stack_top__              /* set the C stack pointer */

    MSR     CPSR_c,#(SVC_MODE | I_BIT | F_BIT)
    /* copy the temporary stack to the new svc stack, and adjust the svc stack pointer */    
    ;; LDR     sp,=__svc_stack_top__ /* set the SVC stack pointer */


    MOV     pc,lr

    .size   setup_stacks, . - setup_stacks
    .endfunc

	
