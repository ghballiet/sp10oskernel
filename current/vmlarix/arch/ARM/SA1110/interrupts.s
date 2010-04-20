
    .equ    NO_IRQ,         0x80                     /* mask to disable IRQ */
    .equ    NO_FIQ,         0x40                     /* mask to disable FIQ */
    .equ    NO_INT,         (NO_IRQ | NO_FIQ) /*mask to disable IRQ and FIQ */
    .equ    FIQ_MODE,       0x11
    .equ    IRQ_MODE,       0x12
    .equ    SYS_MODE,       0x1F
    .equ    SUP_MODE,       0x13

    .text
    .code 32

/*****************************************************************************
* uint32_t ARM_int_lock_SYS(void);  disables the IRQ/FIQ
*/
    .global ARM_int_lock_SYS
    .func   ARM_int_lock_SYS
ARM_int_lock_SYS:
    MRS     r0,cpsr             /* get the original CPSR in r0 to return */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* disable both IRQ/FIQ */
    BX      lr                  /* return the original CPSR in r0 */

    .size   ARM_int_lock_SYS, . - ARM_int_lock_SYS
    .endfunc


/*****************************************************************************
* void ARM_int_unlock_SYS(uint32_t key);  restores previous IRQ/FIQ state
*/
    .global ARM_int_unlock_SYS
    .func   ARM_int_unlock_SYS
ARM_int_unlock_SYS:
    MSR     cpsr_c,r0           /* restore the original CPSR from r0 */
    BX      lr                  /* return to ARM or THUMB */

    .size   ARM_int_unlock_SYS, . - ARM_int_unlock_SYS
    .endfunc


/*****************************************************************************
* void ARM_irq(void);  entry point for the IRQ handler
*/
    .global ARM_irq
    .func   ARM_irq
ARM_irq:
/* IRQ entry {{{ */
	
/* at this point, the CPU is in IRQ mode. r13 (sp) and r14 (lr) in
        this mode have replaced the user mode r13 and r14 and will be
	swapped out again when we enter system mode. We are going to
	save r0 and r1 into the IRQ mode r13 and r14 registers, so
	that we can restore them later. We are going to trash r13 (the
	irq stack pointer) in order to free up some other registers.
	Why not just push r0 and r1 onto the IRQ stack and pass the
	IRQ stack address to system mode through r0? The method used
	here is faster because it avoids memory references. */
    MOV     r13,r0           /* save r0 in r13_irq, and free r0 */
    SUB     r0,r14,#4        /* put return address in r0, free r14_irq */
    MOV     r14,r1           /* save r1 in r14_irq (lr), and free r1 */
    MRS     r1,spsr          /* put the SPSR in r1 */

/* we are about to switch to system mode.  This will swap out the
	r13_irq and r14_irq registers and replace them with the user mode 
	r13 and r14 (sp and lr) registers (user mode and system mode use
	the same r13 and r14 ) */

    MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* SYSTEM, no IRQ, but FIQ enabled! */
    STMFD   r13!,{r0,r1}            /* save SPSR and PC on user stack */
    STMFD   r13!,{r2-r3,r12,lr} /* save other regs on sys/user stack*/
	/* the sys/usr stack now contains the user program context:
		spsr 
		return address (pc)
		r14 (link register)
		r12
		r3
		r2 <- top of stack
	*/
	
/* now we give the IRQ mode a pointer to the system/user mode stack, and
	adjust the system/user mode stack for what IRQ mode is going to put
	there */
    MOV     r0,sp               /* make the sp_SYS visible to IRQ mode */
    SUB     sp,sp,#(2*4)        /* make room for stacking (r0_SYS, r1_SYS) */

/* now we can switch back to IRQ mode and save the link register and
	stack pointer onto the system/user mode stack */
	
    MSR     cpsr_c,#(IRQ_MODE | NO_IRQ) /* IRQ mode, IRQ/FIQ disabled */
    STMFD   r0!,{r13,r14}       /* finish saving the context (r0_SYS,r1_SYS)*/

	/* the SYS/USER stack now contains the user program context:
		spsr 
		return address (pc) (r15)
		r14 (link register)
		r12
		r3
		r2
		r1
		r0 <- top of stack ( stack grows downward )
	*/

/* now we switch back to system mode and call the C IRQ handler */	
    MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* SYSTEM mode, IRQ disabled */	

    /* NOTE: the C IRQ handler might re-enable IRQ interrupts (the FIQ
	is enabled already), if IRQs are prioritized by an interrupt
	controller.
    */
    LDR     r12,=c_IRQ_handler
    MOV     lr,pc               /* copy the return address to link register */
    MOV     pc,r12              /* call the C IRQ-handler (ARM/THUMB) */


/* we have now returned from the C irq handler */
/* make sure we are in system mode, with no interrupts, and prepare to
	change to IRQ mode. Put a pointer to the usr/sys stack into
	r0, so IRQ mode can access our stack, which should contain the
	following user context:*/
	/*	
		spsr 
		return address (pc)
		r14 (link register)
		r12
		r3
		r2
		r1
		r0	
	 */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    MOV     r0,sp               /* make sp_SYS visible to IRQ mode */
    ADD     sp,sp,#(8*4)        /* fake unstacking 8 registers from sp_SYS */

/* switch to IRQ mode and load registers from the stack */	
    MSR     cpsr_c,#(IRQ_MODE | NO_INT) /* IRQ mode, both IRQ/FIQ disabled */
    MOV     sp,r0               /* copy sp_SYS to sp_IRQ */
    LDR     r0,[sp,#(7*4)]      /* load the saved SPSR from the stack */
    MSR     spsr_cxsf,r0        /* copy it into spsr_IRQ */

    LDMFD   sp,{r0-r3,r12,lr}^  /* unstack all saved USER/SYSTEM registers */
				/* note: lr in the prev instruction refers */
				/* to the user/sys lr, even though we are  */
				/* in IRQ mode (that's what the ^ means)  */
    NOP                         /* can't access banked lr_irq immediately */
    LDR     lr,[sp,#(6*4)]      /* load return address from the SYS stack */
    MOVS    pc,lr               /* return restoring CPSR from SPSR, */
				/* and swapping in the user mode sp and lr */

    .size   ARM_irq, . - ARM_irq
    .endfunc


/*****************************************************************************
* void ARM_fiq(void);  entry point for FIQ handler
*/
    .global ARM_fiq
    .func   ARM_fiq
ARM_fiq:
/* FIQ entry {{{ */
    MOV     r13,r0              /* save r0 in r13_FIQ */
    SUB     r0,lr,#4            /* put return address in r0_SYS */
    MOV     lr,r1               /* save r1 in r14_FIQ (lr) */
    MRS     r1,spsr             /* put the SPSR in r1_SYS */

    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    STMFD   sp!,{r0,r1}         /* save SPSR and PC on SYS stack */
    STMFD   sp!,{r2-r3,r12,lr}  /* save APCS-clobbered regs on SYS stack */
    MOV     r0,sp               /* make the sp_SYS visible to FIQ mode */
    SUB     sp,sp,#(2*4)        /* make room for stacking (r0_SYS, SPSR) */

    MSR     cpsr_c,#(FIQ_MODE | NO_INT) /* FIQ mode, IRQ/FIQ disabled */
    STMFD   r0!,{r13,r14}       /* finish saving the context (r0_SYS,r1_SYS)*/

    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
/* FIQ entry }}} */

    /* NOTE: NOTE: c_FIQ_handler must NEVER enable IRQ/FIQ interrrupts!
    */
    LDR     r12,=c_FIQ_handler
    MOV     lr,pc               /* store the return address */
    BX      r12                 /* call the C FIQ-handler (ARM/THUMB)


/* FIQ exit {{{ */              /* both IRQ/FIQ disabled (see NOTE above) */
    MOV     r0,sp               /* make sp_SYS visible to FIQ mode */
    ADD     sp,sp,#(8*4)        /* fake unstacking 8 registers from sp_SYS */

    MSR     cpsr_c,#(FIQ_MODE | NO_INT) /* FIQ mode, IRQ/FIQ disabled */
    MOV     sp,r0               /* copy sp_SYS to sp_FIQ */
    LDR     r0,[sp,#(7*4)]      /* load the saved SPSR from the stack */
    MSR     spsr_cxsf,r0        /* copy it into spsr_FIQ */

    LDMFD   sp,{r0-r3,r12,lr}^  /* unstack all saved USER/SYSTEM registers */
    NOP                         /* can't access banked reg immediately */
    LDR     lr,[sp,#(6*4)]      /* load return address from the SYS stack */
    MOVS    pc,lr               /* return restoring CPSR from SPSR */
/* FIQ exit }}} */

    .size   ARM_fiq, . - ARM_fiq
    .endfunc


/*****************************************************************************
* void ARM_reset(void);  entry point for reset handler
*/
    .global ARM_reset
    .func   ARM_reset
ARM_reset:
    LDR     r0,=Cstring_reset
    B       ARM_except
    .size   ARM_reset, . - ARM_reset
    .endfunc

/*****************************************************************************
* void ARM_undef(void);
*/
    .global ARM_undef
    .func   ARM_undef
ARM_undef:
    LDR     r0,=Cstring_undef
    B       ARM_except
    .size   ARM_undef, . - ARM_undef
    .endfunc

/*****************************************************************************
* void ARM_swi(void);
*/
    .global ARM_swi
    .func   ARM_swi
ARM_swi:

/* SWI entry {{{ */
	
/* at this point, the CPU is in SUP mode. r13 (sp) and r14 (lr) in
	this mode have replaced the user mode r13 and r14 and will be
	swapped out again when we enter system mode. We are going to
	save r0-r4 onto the SUP mode stack, then copy r14 (lr) into
	r0, copy spsr into r1, and put the SUP stack pointer in r3.
	Then we switch to sys/usr mode, and push everything onto the
	sys mode stack.  Then we can call the C SWI code.  When it
	returns, we trash registers r2 and r3, preserve the contents
	of r0 and r1, and restore all other registers to the way they
	were when we entered this handler */

/* we can't trash the SUP mode stack, like we did the IRQ stack, because
	other parts of the system need it. */
/* Push usr/sys r0-r3 onto SUP stack */	
	STMFD	sp!,{r0-r3}     /* save r0-r3 to SUP mode stack */
	MOV     r0,lr		/* put return address in r0 */
	MRS     r1,spsr         /* put the SPSR in r1 */
	MOV     r2,sp		/* put SUP stack pointer in r3 */
/*  sys mode will take r0-r3 off of the SUP stack, so we fake the pop here */
	add     sp,sp,#(4*4)    /* adjust SUP stack to remove r0-r3 */
		
/* we are about to switch to system mode.  This will swap out the
	r13_sys and r14_sys registers and replace them with the user
	mode r13 and r14 (sp and lr) registers (user mode and system
	mode use the same r13 and r14 ) */
	MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* no IRQ, but FIQ enabled! */
	STMFD   sp!,{r0,r1}	/* save original SPSR and lr on user stack */
	STMFD   sp!,{r12,lr}	/* save other regs on sys/user stack*/
	/* the sys/usr stack now contains the user program context:
		spsr 
		return address (pc)
		r14 (link register)
		r12
	*/
	LDMFD   r2,{r0-r3}	/* get the 4 registers that sup mode saved 
				   on its stack, overwriting r2 */
	STMFD	sp!,{r0-r3}	/* and push them onto the sys/usr stack */
		
	/* the sys/usr stack now contains the user program context:
		spsr 
		return address (pc)
		r14 (link register)
		r12
		r3
		r2 
		r1
		r0 <- top of stack
	*/
	
    /* NOTE: the C SWI handler might re-enable IRQ interrupts (the FIQ
	is enabled already), if IRQs are prioritized by an interrupt
	controller.
    */
	LDR     r12,=c_SWI_handler
	MOV     lr,pc               /* copy return address to link register */
	MOV     pc,r12              /* call the C IRQ-handler */

/* we have now returned from the C SWI handler */
/* make sure we are in system mode, with no interrupts, and prepare to
	change to SUP mode. Put a pointer to the usr/sys stack into
	r0, so SUP mode can access our stack, which should contain the
	following user context:*/
	/*	
		spsr 
		return address (pc)
		r14 (link register)
		r12
		r3
		r2
		r1
		r0	
	 */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    MOV     r3,sp               /* Copy sp to r3, so SUP mode can access it */
    ADD     sp,sp,#(8*4)        /* fake unstacking 8 registers from sys sp */

/* switch to SUP mode and load registers from the stack */	
    MSR     cpsr_c,#(SUP_MODE | NO_INT) /* IRQ mode, both IRQ/FIQ disabled */
    LDR     r2,[r3,#(7*4)]      /* load the saved SPSR from the stack */
    MSR     spsr,r2             /* copy it into spsr_SUP */

    add     r3,r3,#(4*4)	/* fake unstacking the params */
    LDMFD   r3,{r12,lr}^        /* unstack fp and lr */
				/* note: lr in the prev instruction refers */
				/* to the user/sys lr, even though we are  */
				/* in SUP mode (that's what the ^ means)  */
    NOP                         /* can't access banked lr_sup immediately */
    LDR     lr,[r3,#(2*4)]      /* load return address from the SYS stack */
    MOVS    pc,lr               /* return restoring CPSR from SPSR, */
				/* and swapping in the user mode sp and lr */

    .size   ARM_swi, . - ARM_swi
    .endfunc

/*****************************************************************************
* void ARM_pAbort(void);
*/
    .global ARM_pAbort
    .func   ARM_pAbort
ARM_pAbort:
    LDR     r0,=Cstring_pAbort
    B       ARM_except
    .size   ARM_pAbort, . - ARM_pAbort
    .endfunc

/*****************************************************************************
* void ARM_dAbort(void);
*/
    .global ARM_dAbort
    .func   ARM_dAbort
ARM_dAbort:
    LDR     r0,=Cstring_dAbort
    B       ARM_except
    .size   ARM_dAbort, . - ARM_dAbort
    .endfunc

/*****************************************************************************
* void ARM_reserved(void);
*/
    .global ARM_reserved
    .func   ARM_reserved
ARM_reserved:
    LDR     r0,=Cstring_rsrvd
    B       ARM_except
    .size   ARM_reserved, . - ARM_reserved
    .endfunc

/*****************************************************************************
* void ARM_except(void);
*/
    .global ARM_except
    .func   ARM_except
ARM_except:
    SUB     r1,lr,#4            /* set exception address */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    LDR     r12,=c_abort_handler
    MOV     lr,pc               /* store the return address */
    BX      r12                 /* call the assertion-handler (ARM/THUMB) */
    /* the assertion handler should not return, but in case it does
    * hang up the machine in the following endless loop
    */
    B       .

Cstring_reset:  .string  "Reset"
Cstring_undef:  .string  "Undefined"
Cstring_swi:    .string  "Software Interrupt"
Cstring_pAbort: .string  "Prefetch Abort"
Cstring_dAbort: .string  "Data Abort"
Cstring_rsrvd:  .string  "Reserved Exception"

    .size   ARM_except, . - ARM_except
    .endfunc

    .end
