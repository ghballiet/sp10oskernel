    .equ    NO_IRQ,         0x80                     /* mask to disable IRQ */
    .equ    NO_FIQ,         0x40                     /* mask to disable FIQ */
    .equ    NO_INT,         (NO_IRQ | NO_FIQ) /*mask to disable IRQ and FIQ */
    .equ    FIQ_MODE,       0x11
    .equ    IRQ_MODE,       0x12
    .equ    SYS_MODE,       0x1F
    .equ    SUP_MODE,       0x13

/********************************************************************/
/* save the state of the process into the process table entry       */
/* void process_arch_save(proc_arch_rec *prec)                      */
	.global	process_arch_save
	.func	process_arch_save
process_arch_save:

  /* on entry to this routine, ARM_irq has saved some things on the stack:
     saved_regs[7] refers to the saved value of the psr 
     saved_regs[6] refers to the saved value of the return address
     saved_regs[5] refers to the saved value of r14
     saved_regs[4] refers to the saved value of r12
     saved_regs[3] refers to the saved value of r3
     saved_regs[2] refers to the saved value of r2
     saved_regs[1] refers to the saved value of r1
     saved_regs[0] refers to the saved value of r0
  */

  /* save contents of r4 through r11 into the process table entry for PID */
  /* the pointer to the process table entry should be in r0 */
	ADD     r0,r0,#16	/* adjust pointer for STMIA  */
	STMIA   r0,{r4-r11}	/* store registers r4 through r11 */
	SUB     r0,r0,#16	/* adjust pointer for STMIA  */
	MOV	pc,lr           /* return */
	.size  process_arch_save, . - process_arch_save
	.endfunc


/******************************************************************/

/* load information from the process table (entry PID) and
   resume the task, we only load certain registers, because
   we will return through c_IRQ_handler and ARM_irq, which
   are using some of the registers. ARM_irq will finish
   restoring our registers from the stack. */
/* void process_arch_resume(proc_rec *prec) */
	.global	process_arch_resume
	.func	process_arch_resume
process_arch_resume:

  /* r0 contains a pointer to our process table entry */
  /* we only need to restore registers r4 through r11, and r13 (sp) */
  /* load registers r4 through r11 from process table entry */
	ADD     r0,r0,#16	/* adjust pointer for LDMIA  */
	LDMIA   r0,{r4-r11}	/* load registers r4 through r11 */
	SUB     r0,r0,#16	/* adjust pointer back where it was  */
	MOV	pc,lr           /* the ISR will restore the rest */
	.size  process_arch_resume, . - process_arch_resume
	.endfunc

/********************************************************************/
/* void process_arch_start(proc_rec *prec) */
/* We are in system mode and still using the stack of the last
   process that was running, but it's stack pointer should have 
   been saved by ARM_irq, c_IRQ_handler, and save_task */
	.global	process_arch_start
	.func	process_arch_start
process_arch_start:
        /* switch to supervisor mode IRQ/FIQ disabled */
	MSR	cpsr_c,#(SUP_MODE | NO_INT) /* disable both IRQ/FIQ */
	/* load user mode registers */
	ADD	r0,r0,#4     /* adjust pointer for LDMIA  */
        /* load user registers r1 through r14 */
	LDMIA	r0,{r1-r14}^ /* the ^ makes it load usr mode r13 and r14 */
	NOP
	LDR	lr,[r0,#60] /* load the link register with spsr value */
	MSR	spsr_cxsf,lr /* copy it into spsr_supervisor */
	LDR	lr,[r0,#56]  /* load the link register with entry address */
	LDR	r0,[r0,#-4]     /* load register zero */
	MOVS	pc,lr       /* jump to [lr] loading the cpsr as we go */
	.size  process_arch_start, . - process_arch_start
	.endfunc


