
#ifndef SA1110_INTERRUPTS_H
#define SA1110_INTERRUPTS_H

#include <sys/types.h>

/* each entry in the interrupt vector table is an ivt_entry */
typedef uint32_t * ivt_entry;

uint32_t ARM_int_lock_SYS();
void ARM_int_unlock_SYS(uint32_t key);
void ARM_irq(void);  
void ARM_fiq(void);
void ARM_reset(void);
void ARM_undef(void);
void ARM_swi(void);
void ARM_pAbort(void);
void ARM_dAbort(void);
void ARM_reserved(void);
void ARM_except(void);

#endif
